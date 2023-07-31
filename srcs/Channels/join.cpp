/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgaillag <cgaillag@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/30 17:36:40 by cgaillag          #+#    #+#             */
/*   Updated: 2023/07/31 15:32:24 by cgaillag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"
#include "utils.hpp"

/**
 * @brief 	JOIN command specific case : 	JOIN 0
 *
 *			This case means client leaves all channels it was joined on
 *			<--> equivalent to PART <all client channels>
 *
 */
bool	Server::handleJoinZero( int clientSocket, const std::vector< std::string >& tokens )
{
	Client	*client = &_clients.at( clientSocket );

	if ( tokens.size() == 1 && tokens[ 0 ] == "0" )
	{
		if ( client->getClientChannels().size() > 0 ) {
			std::vector< std::string > copyClientChannels = client->getClientChannels();
			for ( size_t i = 0; i < copyClientChannels.size(); i++ ) {
				leaveChannel( clientSocket, copyClientChannels[ i ], "" , "PART");
			}
		}
		else
			std::cout << MSGINFO << client->getNickname() << " isn't joined on any channel\n\n";
		return true ;
	}
	return false ;
}

/**
 * @brief 	Check JOIN param size: there must be 1 or 2 parameters
 *
 */
bool	Server::checkParamSize( int clientSocket, const std::string& param, const std::vector< std::string >& tokens )
{
	const std::string&	source = _clients.at( clientSocket ).getSource();
	const std::string&	nickname = _clients.at( clientSocket ).getNickname();

	if ( param.empty() || tokens.size() == 0 ) {
		replyMsg( clientSocket, ERR_NEEDMOREPARAMS( source, nickname, "JOIN" ) );
		return false ;
	}
	if ( tokens.size() > 2 ) {
		std::cout << MSGERROR << "JOIN : too many parameters for this command\n" << std::endl;
		return false ;
	}
	return true ;
}

/**
 * @brief	Check if channel name is valid
 *
 *			Specific case when there is no channel name (only spaces)
 *			Check each channel --> if there is at least one channel
 *			Valid characters: prefix '#', then alphanum and "-", "_", or "."
 */

bool	Server::isValidChanName( int clientSocket, const std::string& channelName )
{
	const Client&	client = _clients.at( clientSocket );
	std::string		chanName = ( channelName.empty() ) ? "<empty>" : channelName;

	if ( chanName == "<empty>" || channelName.find( CHANTYPES ) != 0
		|| ( channelName.find( CHANTYPES ) == 0 && channelName.length() == 1 )
		|| ( channelName.find( CHANTYPES ) == 0 && !isValidToken( chanName.substr( 1 ) ) ) ) {
		replyMsg( clientSocket, ERR_BADCHANMASK( client.getSource(), chanName ) );
		return false ;
	}
	return true ;
}

/**
 * @brief	Check channel format, content & criteria
 *
 *	- check if the channel name is valid (start with a '#' and has valid characters)
 *	- check if the client is already on too many channels
 *	- check if the client has already joined this channel
 *	- check if the channel is available upon invitation
 *	- check if client is banned from this channel --> not handled (not compulsory)
 *	- check if the channel required password
 *	- check if channel is full
 */

bool	Server::checkChanPreJoin( int clientSocket, const std::vector< std::string >& tokens, size_t index )
{
	const Client&				client = _clients.at( clientSocket );
	std::vector< std::string >	tokenChannels = splitString( tokens[ 0 ], ',' );
	std::string					channelName = tokenChannels[ index ];
	const std::string&			source = client.getSource();
	const std::string&			nickname = client.getNickname();

	if ( !isValidChanName( clientSocket, channelName ) )
		return false ;
	if ( client.getClientChannels().size() >= CHANLIMIT ) {
		replyMsg( clientSocket, ERR_TOOMANYCHANNELS( source, nickname, channelName ) );
		return false ;
	}
	if ( existingChannel( channelName ) ) {
		Channel&	channel = _channels.at( channelName );

		if ( channel.getChannelMembers().find( nickname ) != channel.getChannelMembers().end() ) {
			std::cout << MSGINFO << nickname << " is alreay on channel " << channelName << CRLF;
			return false ;
		}
		if ( channel.getChannelMembers().size() >= MAXMEMBERS
			|| ( channel.getLimitStatus() == true
			&&  static_cast<int>( channel.getChannelMembers().size() ) >= channel.getLimitBis() ) ) {
			replyMsg( clientSocket, ERR_CHANNELISFULL( source, nickname, channelName ) );
			return false ;
		}
		if ( channel.getInviteOnlyStatus() == true && !channel.isInvited( nickname ) ) {
			replyMsg( clientSocket, ERR_INVITEONLYCHAN( source, nickname, channelName) );
			return false ;
		}
		if ( channel.getKeyStatus() == true ) {
			if ( tokens.size() == 2 ) {
				std::vector< std::string > keys = splitString( tokens[ 1 ], ',' );
				if ( index <= keys.size() && keys[ index ] == channel.getKey() )
					return true;
			}
			replyMsg( clientSocket, ERR_BADCHANNELKEY( source, nickname, channelName ) );
			return false ;
		}
	}
	return true;
}

/**
 * @brief	Create a channel and put client as chanOp (chan creator)
 *
 */

void	Server::createChanWithOp( int clientSocket, const std::string& channelName )
{
	Client	*client = &_clients.at( clientSocket );

	_channels.insert( std::make_pair( channelName, Channel ( channelName ) ) );

	Channel	*channel = &_channels[ channelName ];

	channel->addChannelOps( client );
}

/**
 * @brief	JOIN command
 *			syntax:	JOIN <channel>{,<channel>} [<key>{,<key>}]
 *
 *			If channel does not exist: create it, then client joins it and 
 *			become chanOp
 *			If channel already exists: join channel as regular member
 *			Perform checks on channel format, mode and possibility to join it
 *
 */
void	Server::handleJoin( int clientSocket, std::string param )
{
	Client						*client = &_clients.at( clientSocket );
	const std::string&			source = client->getSource();
	const std::string&			nickname = client->getNickname();
	std::vector< std::string >	tokens = splitString( param, ' ' );

	if ( checkParamSize( clientSocket, param, tokens ) == false 
		|| handleJoinZero( clientSocket, tokens ) == true )
		return ;
	std::vector< std::string >	chanNames = splitString( tokens[ 0 ], ',' );
	if ( !chanNames.size() ) {
		replyMsg( clientSocket, ERR_BADCHANMASK( client->getSource(), "<empty>" ) );
		return ;
	}

	for ( size_t i = 0; i < chanNames.size(); ++i ) {
		std::string& channelName = chanNames[ i ];
		if ( channelName.size() > CHANNELLEN )
			channelName = channelName.substr( 0, CHANNELLEN );
		if ( checkChanPreJoin( clientSocket, tokens, i ) == false )
			continue ;
		if ( !existingChannel( channelName ) )
			createChanWithOp( clientSocket, channelName );

		Channel	*channel = &_channels[ channelName ];
		channel->addChannelMember( client );
		if ( channel->getInviteOnlyStatus() == true )
			channel->removeInvitedMember( nickname );
		client->addChannel( channelName );

		channelMsgToAll( clientSocket, channelName, RPL_JOIN( source, nickname, channelName ) );
		if ( !channel->getTopic().empty() )
			replyMsg( clientSocket, RPL_TOPIC( source, nickname, channelName, _channels[ channelName ].getTopic() ) );
		handleNames( clientSocket, channelName );
	}
	return ;
}

/* ****************************** ***
** CHANNEL MESSAGE TO ALL MEMBERS ***
** ****************************** ***
*/
void	Server::channelMsgToAll( int clientSocket, const std::string& channelName, const std::string& message )
{
	channelMsgNotClient( clientSocket, channelName, message );
	replyMsg( clientSocket, message, 0 );
}

/* ******************************* ***
** CHANNEL MESSAGE TO CHANOPS ONLY ***
** ******************************* ***
*/
void	Server::channelMsgToChanOps( int clientSocket, const std::string& channelName, const std::string& message )
{
	Channel&							channel = _channels.at( channelName );
	Channel::mapClientsPtr::iterator	it;
	int									socket;

	for ( it = channel.getChannelOps().begin() ; it != channel.getChannelOps().end() ; ++it )
	{
		socket = it->second->getFd();
		if ( socket != clientSocket ) {
			replyMsg( socket, message, 0 );
		}
	}
	// displays reply message on the server only once
	std::cout << MSGREPLY << message << std::endl;
}

/* ******************************************** ***
** CHANNEL MESSAGE TO ALL MEMBERS EXCEPT CLIENT ***
** ******************************************** ***
*/
void	Server::channelMsgNotClient( int clientSocket, const std::string& channelName, const std::string& message )
{
	Channel&							channel = _channels.at( channelName );
	Channel::mapClientsPtr::iterator	it;
	int									socket;

	for ( it = channel.getChannelMembers().begin() ; it != channel.getChannelMembers().end() ; ++it )
	{
		socket = it->second->getFd();
		if ( socket != clientSocket ) {
			replyMsg( socket, message, 0 );
		}
	}
	// displays reply message on the server only once
	std::cout << MSGREPLY << message << std::endl;
}

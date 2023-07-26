/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmelard <lmelard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/30 17:36:40 by cgaillag          #+#    #+#             */
/*   Updated: 2023/07/26 16:08:56 by lmelard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"
#include "utils.hpp"

/**
 * @brief       JOIN command
 *
 *				spécification IRC (RFC 2812)
 *				http://abcdrfc.free.fr/rfc-vf/rfc2812.html
 *				https://modern.ircdocs.horse/#connection-setup

				syntax:
					JOIN <channel> [key] [invitationkey] [mode]

				Numeric Replies:
					ERR_NEEDMOREPARAMS (461)	x
					ERR_NOSUCHCHANNEL (403)		x (not used here)
					ERR_TOOMANYCHANNELS (405)	x
					ERR_BADCHANNELKEY (475)		x
					ERR_BANNEDFROMCHAN (474)	not handled
					ERR_CHANNELISFULL (471)		x
					ERR_INVITEONLYCHAN (473)	x
					ERR_BADCHANMASK (476)		x
					RPL_TOPIC (332)				x
					RPL_TOPICWHOTIME (333)
					RPL_NAMREPLY (353)			x
					RPL_ENDOFNAMES (366)		x
 *
 */

bool	Server::handleJoinZero( int clientSocket, const std::vector< std::string >& tokens )
{
	Client	*client = &_clients.at( clientSocket );

	if ( tokens.size() == 1 && tokens[ 0 ] == "0" )
	{
		if ( client->getClientChannels().size() > 0 ) {
			std::vector< std::string > copyClientChannels = client->getClientChannels();

			// std::cout << ZZ_MSGTEST << client->getNickname() << " has joined [" << client->getClientChannels().size() <<"] channels\n";
			for ( size_t i = 0; i < copyClientChannels.size(); i++ ) {
				leaveChannel( clientSocket, copyClientChannels[ i ], "" , "PART");
			}
		}
		return true ;
	}
	return false ;
}

void	Server::handleJoin( int clientSocket, std::string param )
{
	std::vector< std::string >	tokens = splitString( param, ' ' );
	Client						*client = &_clients.at( clientSocket );

	// std::cout << ZZ_MSGTEST << cmd << " - nombre de tokens: " << tokens.size() << std::endl;// TO BE DELETED - ZZ_TEST

	/* *************************************************** ***
	** CHECK 1 - PARAM SIZE                                ***
	** *************************************************** ***
	*/
	if ( param.empty() || tokens.size() == 0 ) {
		replyMsg( clientSocket, ERR_NEEDMOREPARAMS( client->getSource(), client->getNickname(), "JOIN" ) );
		return ;
	}
	if ( tokens.size() > 2 ) {
		std::cout << MSGERROR << "JOIN" << " : too many parameters for this command\n" << std::endl;
		return ;
	}

	std::vector< std::string >	chanNames = splitString( tokens[ 0 ], ',' );
	if ( !chanNames.size() ) {
		replyMsg( clientSocket, ERR_BADCHANMASK( client->getSource(), client->getNickname(), "<empty>" ) );
		return ;
	}

	/* *************************************************** ***
	** CHECK 2 - SPECIFIC CASE WHEN PARAM IS ONLY "0"      ***
	** *************************************************** ***
	*/
	if ( handleJoinZero( clientSocket, tokens ) == true ) {
		return ;
	}

	// if ( tokens.size() == 1 && tokens[ 0 ] == "0" )
	// {
	// 	if ( client->getClientChannels().size() > 0 ) {

	// 		std::cout << ZZ_MSGTEST << client->getNickname() << " has joined [" << client->getClientChannels().size() <<"] channels\n";
	// 		while ( client->getClientChannels().size() > 0 ) {
	// 			std::string chanToPart = client->getClientChannels()[0];

	// 			// std::cout << "targeted channel is " << chanToPart << std::endl;
	// 			leaveChannel( clientSocket, chanToPart, "" );
	// 		}
	// 	}
	// 	return ;
	// }

	/* *************************************************** ***
	** CHECK 3 - CHANNEL FORMAT & CRITERIA                 ***
	** & PROCESS CREATE and/or ADD CLIENT                  ***
	** *************************************************** ***
	*/
	for ( size_t i = 0; i < chanNames.size(); ++i ) {
		std::string	channelName = chanNames[ i ];
		bool		chanExists = existingChannel( channelName );

		// check channel format, content & criteria
		if ( checkChanPreJoin( clientSocket, tokens, i ) == false ) {
			continue ;
		}

		// if channel does not exist yet --> create it, add to _channels and client become a chanOp
		if ( !chanExists ) {
			createChanWithOp( clientSocket, channelName );
		}

		Channel	*channel = &_channels[ channelName ];
		// add client to the list of channel members
		channel->addChannelMember( client );
		// delete client from list of channel invitees, if client was mentionned on it
		if ( channel->getInviteOnlyStatus() == true ) {
			channel->removeInvitedMember( client->getNickname() );
		}

		// add channel name to '_clientChannels', the list of channels for the current client
		client->addChannel( channelName );

		// info to all (incl. client): client has been added to the channel
		channelMsgToAll( clientSocket, channelName, RPL_JOIN( client->getSource(), client->getNickname(), channelName ) );
		// display RPL_TOPIC (if existing - should not at creation) and NAMES
		if ( !channel->getTopic().empty() ) {
			replyMsg( clientSocket, RPL_TOPIC( client->getSource(), client->getNickname(), channelName, _channels[ channelName ].getTopic() ) );
		}
		handleNames( clientSocket, channelName );
	}
	return ;
}

/* *************************************************** ***
** CREATE A CHANNEL AND PUT CLIENT AS CHANOP (creator) ***
** *************************************************** ***
*/
void	Server::createChanWithOp( int clientSocket, const std::string& channelName )
{
	Client	*client = &_clients.at( clientSocket );

	_channels.insert( std::make_pair( channelName, Channel ( channelName ) ) );
	Channel	*channel = &_channels[ channelName ];

	channel->addChannelOps( client );
}

/* ************************************ ***
** CHECK CHANNEL FORMAT & CRITERIA      ***
** ************************************ ***
**	- check if the channel name is valid (start with a '#' and has valid characters)
**	- check if the client is already on too many channels
**	- check if the client has already joined this channel
**	- check if the channel is available upon invitation
**	- check if client is banned from this channel --> not handled (not compulsory)
**	- check if the channel required password
**	- check if channel is full
*/
bool	Server::checkChanPreJoin( int clientSocket, const std::vector< std::string >& tokens, size_t index )
{
	const Client&				client = _clients.at( clientSocket );
	std::vector< std::string >	tokenChannels = splitString( tokens[ 0 ], ',' );
	std::string					channelName = tokenChannels[ index ];

	if ( !isValidChanName( clientSocket, channelName ) ) {
		return false ;
	}
	if ( client.getClientChannels().size() >= CHANLIMIT ) {
		replyMsg( clientSocket, ERR_TOOMANYCHANNELS( client.getSource(), client.getNickname(), channelName ) );
		return false ;
	}

	if ( existingChannel( channelName ) ) {
		Channel&	channel = _channels.at( channelName );

		if ( channel.getChannelMembers().find( client.getNickname() ) != channel.getChannelMembers().end() ) {
			std::cout << MSGINFO << client.getNickname() << " is alreay on channel " << channelName << "'\n" << std::endl;
			return false ;
		}
		if ( channel.getChannelMembers().size() >= MAXMEMBERS
			|| ( channel.getLimitStatus() == true &&  static_cast<int>( channel.getChannelMembers().size() ) >= channel.getLimitBis() ) ) {
			replyMsg( clientSocket, ERR_CHANNELISFULL( client.getSource(), channelName ) );
			return false ;
		}
		if ( channel.getInviteOnlyStatus() == true && !channel.isInvited( client.getNickname() ) ) {
			replyMsg( clientSocket, ERR_INVITEONLYCHAN( client.getSource(), channelName) );
			return false ;
		}
		if ( channel.getKeyStatus() == true ) {
			if ( tokens.size() == 2 ) {
				std::vector< std::string > keys = splitString( tokens[ 1 ], ',' );

				if ( keys[ index ] == channel.getKey() ) {
					return true;
				}
			}
			replyMsg( clientSocket, ERR_BADCHANNELKEY( client.getSource(), channelName ) );
			return false ;
		}
	}
	return true;
}

/* ****************************** ***
** CHECK IF CHANNEL NAME IS VALID ***
** ****************************** ***
** specific case when there is no channel name (only spaces)
** check each channel --> if there is at least one channel
** valid characters: prefix '#' and then alphanum and specif ones as "-", "_", or "."
*/
bool	Server::isValidChanName( int clientSocket, const std::string& channelName )
{
	const Client&	client = _clients.at( clientSocket );
	std::string		chanName = ( channelName.empty() ) ? "<empty>" : channelName;

	if ( chanName == "<empty>" || channelName.find( CHANTYPES ) != 0
		|| ( channelName.find( CHANTYPES ) == 0 && channelName.length() == 1 )
		|| ( channelName.find( CHANTYPES ) == 0 && !isValidToken( chanName.substr( 1 ) ) ) ) {
		replyMsg( clientSocket, ERR_BADCHANMASK( client.getSource(), client.getNickname(), chanName ) );
		return false ;
	}
	return true ;
}

/* ****************************** ***
** CHANNEL MESSAGE TO ALL MEMBERS ***
** ****************************** ***
*/
void	Server::channelMsgToAll( int clientSocket, std::string channelName, std::string message )
{
	channelMsgNotClient( clientSocket, channelName, message );
	replyMsg( clientSocket, message, 0 );
}

/* ******************************************** ***
** CHANNEL MESSAGE TO ALL MEMBERS EXCEPT CLIENT ***
** ******************************************** ***
*/
void	Server::channelMsgNotClient( int clientSocket, std::string channelName, std::string message )
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

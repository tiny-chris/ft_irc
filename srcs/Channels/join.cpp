/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgaillag <cgaillag@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/30 17:36:40 by cgaillag          #+#    #+#             */
/*   Updated: 2023/07/24 13:23:47 by cgaillag         ###   ########.fr       */
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

void	Server::handleJoin( int clientSocket, std::string cmd, std::string param )
{
	std::vector< std::string >	tokens = splitString( param, ' ' );
	Client& 					client = _clients.at( clientSocket );

	std::cout << ZZ_MSGTEST << cmd << " - nombre de tokens: " << tokens.size() << std::endl;// TO BE DELETED - ZZ_TEST

	/* *************************************************** ***
	** CHECK 1 - PARAM SIZE                                ***
	** *************************************************** ***
	*/
	if ( param.empty() || tokens.size() == 0 ) {
		replyMsg( clientSocket, ERR_NEEDMOREPARAMS( client.getSource(), client.getNickname(), cmd ) );
		return ;
	}
	if ( tokens.size() > 2 ) {
		std::cout << MSGERROR << cmd << " : too many parameters for this command\n" << std::endl;
		return ;
	}
	if ( tokens.size() == 1 && tokens[ 0 ] == "0" )
	{
		std::cout << ZZ_MSGTEST << "TODO supprimer le client de tous les canaux (utiliser PART)\n" << std::endl;
	}

	/* *************************************************************** ***
	** CHECK 2 - FORMAT CHANNEL: EACH ONE STARTS WITH '#' AND IS VALID ***
	** *************************************************************** ***
	**	Let's consider tokens[0] to get the channels
	**	- if there is 0 channel
	**		--> reject the request as not aligned with our convention (there must be at least a channel starting with #)
	**	- if a channel is empty or if it does not start with # or if it is invalid
	**		--> ERR_BADCHANMASK (476) + leave
	**
	**	"channel" (nom du canal) : Le nom du canal doit commencer par un préfixe spécifique
	**	pour indiquer le type de canal, suivi d'un nom unique. Les préfixes couramment
	**	utilisés sont "#" pour les canaux publics et "&" pour les canaux privés.
	**	Le nom du canal peut contenir des lettres, des chiffres et certains caractères
	**	spéciaux tels que "-", "_", et ".".
	*/
	std::vector< std::string >	channels = splitString( tokens[ 0 ], ',' );
	if ( validChannelNames( clientSocket, channels ) == false ) {
 		return ;
	}

	/* *************************************************** ***
	** CHECK 3 & ADD CLIENT - EACH CHANNEL ALREADY EXISTS OR NOT        ***
	** *************************************************** ***
	*/
	for ( size_t i = 0; i < channels.size(); ++i )
	{
		std::string	channelName = channels[ i ];
		bool		chanExists = existingChannel( channels[ i ] );

		// faire les checks
		if ( checkPreAddChan( clientSocket, tokens, i, chanExists ) == false ) {
			continue ;
		}

		// autrement si existing : on cree
		if ( !chanExists ) {
			// j'ajoute un nouveau channel à _channels
			_channels.insert( std::make_pair( channelName, Channel ( channelName ) ) );
			// j'ajoute le client à ce nouveau channel et je le mets en tant que chanOp
			_channels[ channelName ].addChannelOps( &client );
		}
		// et j'ajoute le client au channel
		_channels[ channelName ].addChannelMember( &client );

		// j'ajoute le nom du channel dans le vecteur 'clientChannels' du client
		client.addChannel( channelName );

		// info: client has been added to the channel --> to all on channel
		channelMsgToAll( clientSocket, channelName, RPL_JOIN( client.getSource(), client.getNickname(), channelName ) );

		// display RPL_TOPIC (if existing - should not at creation) and NAMES
		if ( !_channels[ channelName ].getTopic().empty() ) {
			replyMsg( clientSocket, RPL_TOPIC( client.getSource(), client.getNickname(), channelName, _channels[ channelName ].getTopic() ) );
		}
		handleNames( clientSocket, channelName );
	}
}

/* ************************************ ***
** CHECK PRE ADD CHANNEL                ***
** ************************************ ***
**	- check if the client has already joined this channel
**	- check if the client is already on too many channels
**	- check if the channel is available upon invitation
**	- check if client is banned from this channel --> not handled (not compulsory)
**	- check if the channel required password
**	- check if channel is full
*/
bool	Server::checkPreAddChan( int clientSocket, std::vector< std::string > tokens, size_t index, bool chanExists )
{
	Client&						client = _clients.at( clientSocket );
	std::vector< std::string >	tokenChannels = splitString( tokens[ 0 ], ',' );
	std::string					channelName = tokenChannels[ index ];
	Channel						channel;


	if ( chanExists ) {
		channel = _channels.at( channelName );

	}

	if ( chanExists && channel.getChannelMembers().find( client.getNickname() ) != channel.getChannelMembers().end() ) {
		std::cout << MSGINFO << client.getNickname() << " is alreay on channel " << channelName << "'\n" << std::endl;
		return false ;
	}
	if ( client.getClientChannels().size() >= CHANLIMIT ) {
		replyMsg( clientSocket, ERR_TOOMANYCHANNELS( client.getSource(), client.getNickname(), channelName ) );
		return false ;
	}
	if ( chanExists && ( channel.getChannelMembers().size() >= MAXMEMBERS
		|| ( channel.getLimitStatus() == true &&  static_cast<int>( channel.getChannelMembers().size() ) >= channel.getLimitBis() ) ) ) {
		replyMsg( clientSocket, ERR_CHANNELISFULL( client.getSource(), channelName ) );
		return false ;
	}
	if ( chanExists && channel.getInviteOnlyStatus() == true && !channel.isInvited( client.getNickname() ) ) {
		replyMsg( clientSocket, ERR_INVITEONLYCHAN( client.getSource(), channelName) );//.substr( 1 ) ) );// CHECK ICI
		return false ;
	}
	if ( chanExists && channel.getKeyStatus() == true ) {
		if ( tokens.size() == 2 ) {
			std::vector< std::string > keys = splitString( tokens[ 1 ], ',' );
			if ( keys[ index ] == channel.getKey() ) {
				return true;
			}
		}
		replyMsg( clientSocket, ERR_BADCHANNELKEY( client.getSource(), channelName ) );
		return false ;
	}
	return true;
}

/* ****************************** ***
** CHECK IF CHANNEL NAME IS VALID ***
** ****************************** ***
** specific case when there is no channel name (only spaces)
** check each channel --> if there is at least one channel
*/
bool	Server::validChannelNames( int clientSocket, std::vector<std::string>& channelNames )
{
	Client&	client = _clients.at( clientSocket );

	if ( !channelNames.size() ) {
		replyMsg( clientSocket, ERR_BADCHANMASK( client.getSource(), client.getNickname(), "<empty>" ) );
		return false;
	}

	for ( size_t i = 0; i < channelNames.size(); ++i )
	{
		std::string chanName = "<empty>";
		if ( !channelNames[ i ].empty() && channelNames[ i ].find( CHANTYPES ) != 0 ) {
			chanName = channelNames[ i ];
		}
		else if ( channelNames[ i ].length() > 1 ) {
			chanName = channelNames[ i ];
		}

		if ( chanName.compare( "<empty>" ) == 0 || channelNames[ i ].find( CHANTYPES ) != 0
			|| ( channelNames[ i ].find( CHANTYPES ) == 0 && channelNames[ i ].length() == 1 )
			|| ( channelNames[ i ].find( CHANTYPES ) == 0 && !isValidToken( chanName.substr ( 1 ) ) ) ) {
			replyMsg( clientSocket, ERR_BADCHANMASK( client.getSource(), client.getNickname(), chanName ) );
			return false ;
		}

		std::cout << ZZ_MSGTEST << "val de channelName = " << chanName << std::endl;// TO BE DELETED - ZZ_TEST
	}
	return true ;
}

/* ****************************** ***
** CHANNEL MESSAGE TO ALL MEMBERS ***
** ****************************** ***
** specific case when there is no channel name (only spaces)
** check each channel --> if there is at least one channel
*/
void	Server::channelMsgToAll( int clientSocket, std::string channelName, std::string message )
{
	channelMsgNotClient( clientSocket, channelName, message );
	replyMsg( clientSocket, message );
}

/* ******************************************** ***
** CHANNEL MESSAGE TO ALL MEMBERS EXCEPT CLIENT ***
** ******************************************** ***
** specific case when there is no channel name (only spaces)
** check each channel --> if there is at least one channel
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
	// permet d'afficher l'info sur le server aussi
	std::cout << MSGREPLY << message << std::endl;
}

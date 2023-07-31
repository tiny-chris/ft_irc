/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgaillag <cgaillag@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/30 17:36:40 by cgaillag          #+#    #+#             */
/*   Updated: 2023/07/31 12:18:47 by cgaillag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"
#include "utils.hpp"

/**
 * @brief       PRIVMSG command
 *				syntax:			PRIVMSG <target> <message>
 *	
 *	Send a message to the target (client or channel)
 *	- Check for a channel target: channel exists, client is joined to it 
 *		(specific case: if only addressed to chanOps)
 * 	- Check for a client target: client exists
 *
 */

void	Server::handlePrivmsg( int clientSocket, std::string param )
{
	Client				*client = &_clients.at( clientSocket );
	const std::string&	source = client->getSource();
	const std::string&	nickname = client->getNickname();
	std::string 		targetList = "", textToBeSent = "";

	if ( param.empty() || splitStringInTwo( param, ' ', &targetList, &textToBeSent ) == false ) {
		replyMsg( clientSocket, ERR_NORECIPIENT( source, nickname, "PRIVMSG" ) );
		replyMsg( clientSocket, ERR_NOTEXTTOSEND( source, nickname ) );
		return ;
	}
	else if ( targetList.empty() || textToBeSent.empty() ) {
		replyMsg( clientSocket, ERR_NEEDMOREPARAMS( source, nickname, "PRIVMSG" ) );
		return ;
	}

	if ( textToBeSent.at( 0 ) == ':' )
		textToBeSent.erase( 0, 1 );
	std::vector< std::string >	targetNames = splitString( targetList, ',' );

	for ( size_t i = 0; i < targetNames.size() && i < TARGMAXMSG ; ++i ) {
		std::string	target = targetNames[ i ];
		size_t		sharp = target.find("#");

		if ( !target.empty() ) {
			std::string reply = RPL_PRIVMSG( source, nickname, target, textToBeSent );

			// if CHANNEL (#)
			if ( sharp != std::string::npos && ( sharp == 0 || sharp == 1 )) {

				std::string	channelName = target.substr( sharp );
				if ( channelName.size() > CHANNELLEN )
					channelName = channelName.substr(0, CHANNELLEN);
				if ( !existingChannel( channelName ) ) { // if channel does not exist
					replyMsg( clientSocket, ERR_NOSUCHCHANNEL( source, nickname, channelName ) );
					continue ;
				}

				Channel	channel = _channels.at( channelName );
				if (  channel.checkChannelMembers( nickname ) ) { // if client is on Channel
					if ( target.find( "@#" ) == 0 ) { // if starts with '@#' --> to chanOps only
						channelMsgToChanOps( clientSocket, channelName, reply );
						continue ;
					}
					else if ( target.find( "#" ) == 0 ) { // if start with '#' --> all members
						channelMsgNotClient( clientSocket, target, reply );
						continue ;
					}
				}
				// client is not on the channel or channel prefixes is not '#' neither "@#"
				replyMsg( clientSocket, ERR_CANNOTSENDTOCHAN( source, nickname, target  ));
			}
			// if CLIENT
			else if ( existingClient( target ) )
				replyMsg( findClientFd( target ), reply );
			else
				replyMsg( clientSocket, ERR_NOSUCHNICK( source, target ) );
		}
		else {
			replyMsg( clientSocket, ERR_NOSUCHNICK( source, "" ) );
		}
	}
}

int	Server::findClientFd( const std::string& name )
{
	for ( mapClients::iterator it = _clients.begin() ; it != _clients.end(); ++it) {
		if ( it->second.getNickname() == name )
			return ( it->second.getFd() );
	}
	return -1 ;
}

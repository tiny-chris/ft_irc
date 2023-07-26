/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsg.cpp                                        :+:      :+:    :+:   */
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

/*
	Numeric Replies:

		x	ERR_NOSUCHNICK (401)
			ERR_NOSUCHSERVER (402)
		x	ERR_CANNOTSENDTOCHAN (404)
			ERR_TOOMANYTARGETS (407)
		x	ERR_NORECIPIENT (411)
		x	ERR_NOTEXTTOSEND (412)
			ERR_NOTOPLEVEL (413)
			ERR_WILDTOPLEVEL (414)
			RPL_AWAY (301)
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
	else if ( targetList.empty() ) {
		replyMsg( clientSocket, ERR_NORECIPIENT( source, nickname, "PRIVMSG" ) );
		return ;
	}
	else if ( textToBeSent.empty() ) {
		replyMsg( clientSocket, ERR_NOTEXTTOSEND( source, client->getNickname() ) );
		return ;
	}

	textToBeSent = " :" + textToBeSent;
	std::vector< std::string >	targetNames = splitString( targetList, ',' );
	// std::vector< std::string >	targets;
	for ( size_t i = 0; i < targetNames.size(); ++i ) {
		std::string	target = targetNames[ i ];
		size_t		sharp = target.find("#");

		if ( !target.empty() ) {
			std::string reply = RPL_PRIVMSG( source, nickname, target, textToBeSent );

			// if CHANNEL (#)
			if ( sharp != std::string::npos) {
				if ( existingChannel( target.substr( sharp ) ) ) {
					Channel	channel = _channels.at( target.substr( sharp ) );

					// if client is not on Channel --> cannot send to channel
					if (  !channel.checkChannelMembers( nickname ) ) {
						replyMsg( clientSocket, ERR_CANNOTSENDTOCHAN( source, nickname, target ));
						continue ;
					}
					// it start with '@#' --> text to be sent to chanops only
					else if ( target.find( "@#" ) == 0 ) {
						channelMsgToChanOps( clientSocket, target.substr( 1 ), reply );
						continue ;
					}
					// start with '#' --> text to be sent to channel members
					else if ( target.find( "#" ) == 0 ) {
						channelMsgNotClient( clientSocket, target, reply );
						continue ;
					}
				}
			}
			else {// if CLIENT
				if ( existingClient( target ) ) {
					mapClients::iterator	it;

					for ( it = _clients.begin() ; it != _clients.end(); ++it) {
						if ( it->second.getNickname() == target ) {
							replyMsg( it->second.getFd(), reply );
							continue ;
						}
					}
				}
			}
			replyMsg( clientSocket, ERR_NOSUCHNICK( source, nickname ) );
		}
		replyMsg( clientSocket, ERR_NOSUCHNICK( source, nickname ) );
	}
}

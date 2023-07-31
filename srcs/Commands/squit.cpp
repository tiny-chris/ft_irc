/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   squit.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgaillag <cgaillag@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/27 15:07:17 by cgaillag          #+#    #+#             */
/*   Updated: 2023/07/31 15:41:48 by cgaillag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Socket.hpp"
#include "utils.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"

/**
 * @brief       QUIT command
 * 				syntax: SQUIT <server> <comment>
 * 
 * 				The client closes the server - must be a serverOp 
 *   
 */

void	Server::handleSQuit( int clientSocket, std::string param )
{
	Client				*client = &_clients.at( clientSocket );
	const std::string&	source = client->getSource();
	const std::string&	nickname = client->getNickname();
	std::string			serverToQuit = "", comment = "";

	if ( param.empty() || splitStringInTwo( param, ' ', &serverToQuit, &comment ) == false
		|| serverToQuit.empty() || comment.empty() ) { // uncomplete command
		replyMsg( clientSocket, ERR_NEEDMOREPARAMS( source, nickname, "SQUIT" ) );
		return ;
	}
	if ( serverToQuit != _serverName ) { // server does not exists
		replyMsg( clientSocket, ERR_NOSUCHSERVER( source, serverToQuit ) );
		return ;
	}
	if ( !client->getOperatorMode() ) { // client is not a serverOp (of this server)
		replyMsg( clientSocket, ERR_NOPRIVILEGES( source ) );
		return ;
	}
	broadcastMsgToAll( clientSocket, RPL_SQUIT( source, comment ) );
	stop();
}

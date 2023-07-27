/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   squit.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgaillag <cgaillag@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/27 15:07:17 by cgaillag          #+#    #+#             */
/*   Updated: 2023/07/27 17:10:29 by cgaillag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Socket.hpp"
#include "utils.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"

/*
//	syntax	 SQUIT <server> <comment>

	Numeric replies:

		ERR_NOSUCHSERVER (402)
		ERR_NEEDMOREPARAMS (461)
		ERR_NOPRIVILEGES (481)
		ERR_NOPRIVS (723)

*/

void	Server::handleSQuit( int clientSocket, std::string param )
{
	Client				*client = &_clients.at( clientSocket );
	const std::string&	source = client->getSource();
	const std::string&	nickname = client->getNickname();

	std::string			serverToQuit = "", comment = "";

	// uncomplete command
	if ( param.empty() || splitStringInTwo( param, ' ', &serverToQuit, &comment ) == false
		|| serverToQuit.empty() || comment.empty() ) {
		replyMsg( clientSocket, ERR_NEEDMOREPARAMS( source, nickname, "SQUIT" ) );
		return ;
	}

	// server does not exists
	if ( serverToQuit != _serverName ) {
		replyMsg( clientSocket, ERR_NOSUCHSERVER( source, serverToQuit ) );
		return ;
	}

	// // to uncomment once mode Server Op is on
	// // client is not a Server Operator (of this server)
	// if ( client->getServerOperator() != false ) {
	// 	replyMsg( clientSocket, ERR_NOPRIVILEGES( source ) );
	// 	return ;
	// }
	if ( clientSocket != 5 && clientSocket != 6 ) {
		replyMsg( clientSocket, ERR_NOPRIVILEGES( source ) );
		return ;
	}
	// // to delete as just for testing

	broadcastMsgToAll( clientSocket, RPL_SQUIT( source, comment ) );
	stop();
}

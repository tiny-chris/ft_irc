/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   squit.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgaillag <cgaillag@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by 2.fr>             #+#    #+#             */
/*   Updated: 2023/08/01 13:00:19 by cgaillag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Server.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"
#include "utils.hpp"

/**
 * @brief       QUIT command
 * 				      syntax: SQUIT <server> <comment>
 *
 * 				      The client closes the server - must be a serverOp
 */

void Server::handleSQuit( int clientSocket, std::string param ) {
  Client*            client = &_clients.at( clientSocket );
  const std::string& source = client->getSource();
  const std::string& nickname = client->getNickname();
  std::string        serverToQuit = "", comment = "";

  if( param.empty()
      || splitStringInTwo( param, ' ', &serverToQuit, &comment ) == false
      || serverToQuit.empty() || comment.empty() ) {  // if uncomplete command
    replyMsg( clientSocket, ERR_NEEDMOREPARAMS( source, nickname, "SQUIT" ) );
    return;
  }
  if( comment.at( 0 ) == ':' ) {
    comment.erase( 0, 1 );
  }
  if( serverToQuit != _serverName ) {  // server does not exists
    replyMsg( clientSocket, ERR_NOSUCHSERVER( source, serverToQuit ) );
    return;
  }
  if( !client->getOperatorMode() ) {  // client is not serverOp (this server)
    replyMsg( clientSocket, ERR_NOPRIVILEGES( source ) );
    return;
  }
  broadcastMsgToAll( clientSocket, RPL_SQUIT( source, comment ) );
  stop();
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   squit.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By:                                            +#+  +:+       +#+        */
/*       lmelard <lmelard@student.42.fr>          +#+#+#+#+#+   +#+           */
/*       cgaillag <cgaillag@student.42.fr>             #+#    #+#             */
/*       cvidon <cvidon@student.42.fr>                ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Server.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"
#include "utils.hpp"

/**
 * @brief       QUIT command
 * 				syntax: SQUIT <server> <comment>
 *
 * 				The client closes the server - must be a serverOp
 *
 */

void Server::handleSQuit( int clientSocket, std::string param ) {
  Client*            client = &_clients.at( clientSocket );
  const std::string& source = client->getSource();
  const std::string& nickname = client->getNickname();
  std::string        serverToQuit = "", comment = "";

  if( param.empty()
      || splitStringInTwo( param, ' ', &serverToQuit, &comment ) == false
      || serverToQuit.empty() || comment.empty() ) {  // uncomplete command
    replyMsg( clientSocket, ERR_NEEDMOREPARAMS( source, nickname, "SQUIT" ) );
    return;
  }
  if( serverToQuit != _serverName ) {  // server does not exists
    replyMsg( clientSocket, ERR_NOSUCHSERVER( source, serverToQuit ) );
    return;
  }
  if( !client->getOperatorMode() ) {  // client is not a serverOp (of this
                                      // server)
    replyMsg( clientSocket, ERR_NOPRIVILEGES( source ) );
    return;
  }
  broadcastMsgToAll( clientSocket, RPL_SQUIT( source, comment ) );
  stop();
}

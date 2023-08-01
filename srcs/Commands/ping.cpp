/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgaillag <cgaillag@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by 2.fr>             #+#    #+#             */
/*   Updated: 2023/08/01 09:32:04 by cgaillag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Server.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"
#include "utils.hpp"

/**
 * @brief	    PING command
 * 			      syntax:	PING <token>
 *							<token> cannot contain space (it may be any non-empty string)
 *
 *       To check if the other side of the connection is still connected (check 
 *       for connection latency, at the application layer)
 *       Answer syntax (from server): PONG <server> <token>
 */

void Server::handlePing( int clientSocket, std::string param ) {
  const std::string&       source = _clients.at( clientSocket ).getSource();
  const std::string&       nickname = _clients.at( clientSocket ).getNickname();
  std::vector<std::string> tokens = splitString( param, ' ' );

  if( param.empty() ) {
    replyMsg( clientSocket, ERR_NEEDMOREPARAMS( source, nickname, "PING" ) );
  } else if( tokens[0].empty() ) {
    replyMsg( clientSocket, ERR_NOORIGIN( source, nickname ) );
  } else {
    replyMsg( clientSocket, "PONG " + _serverName + " " + tokens[0] + CRLF );
  }
  return;
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
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
 * @brief   USER command
 *          syntax:   USER <username> 0 * :<realname>
 *
 * Command used at the beginning of a connection to specify the username and
 * realname of a new user
 *
 */

void Server::handleUser( int clientSocket, std::string param ) {
  Client*                  client = &_clients.at( clientSocket );
  const std::string&       nickname = client->getNickname();
  std::string              realname = "";
  size_t                   colon;
  std::vector<std::string> tokens;

  // check if already registered
  if( client->getIfRegistered() == true ) {
    replyMsg( clientSocket, ERR_ALREADYREGISTRED( _serverName, nickname ) );
    return;
  }

  // check param content & last parameter (realname) must not be empty
  colon = param.find( ':', 0 );
  if( param.empty() || colon == std::string::npos || colon == 0
      || colon == param.length() - 1 ) {
    replyMsg( clientSocket,
              ERR_NEEDMOREPARAMS( _serverName, nickname, "USER" ) );
    return;
  }
  realname = param.substr( colon + 1, param.length() - ( colon + 1 ) );
  client->setRealname( realname );

  // check 3 first parameters are valid
  tokens = splitString( param.substr( 0, colon ), ' ' );
  if( tokens.size() < 3 || tokens[0].length() < 1 ) {
    replyMsg( clientSocket,
              ERR_NEEDMOREPARAMS( _serverName, nickname, "USER" ) );
    return;
  } else if( tokens.size() > 3 || isValidUser( tokens[0] ) == false
             || !isValidParam( tokens[1] ) || !isValidParam( tokens[2] )
             || ( colon > 0 && param[colon - 1] != ' ' ) ) {
    std::cout << MSGERROR << "wrong parameters with USER command\n"
              << std::endl;
    return;
  }
  client->setUsername( tokens[0].substr( 0, USERLEN ) );
  std::cout << MSGINFO << "user and real names provided!" << std::endl;

  if( client->getIfRegistered() == false ) {
    checkRegistration( clientSocket );
  }
  std::cout << std::endl;
}

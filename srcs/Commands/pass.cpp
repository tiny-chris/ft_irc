/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pass.cpp                                           :+:      :+:    :+:   */
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
 * @brief       First command that a user needs to make to register
 *              Checks if the a password is given
 *              Checks if the password valid
 *              Sets the password status to true to continue registration
 */

void Server::handlePass( int clientSocket, std::string param ) {
  Client*     client = &_clients.at( clientSocket );
  std::string source = client->getSource();
  std::string nick = client->getNickname();
  if( client->getIfRegistered() ) {  // client cannot re registered
    replyMsg( clientSocket,
              ERR_ALREADYREGISTRED( source, client->getNickname() ) );
    return;
  }
  if( param.empty() ) {  // if no assword entered -> error need more param
    replyMsg( clientSocket, ERR_NEEDMOREPARAMS( source, nick, "PASS" ) );
    return;
  }
  std::vector<std::string> tokens = splitString( param, ' ' );
  std::string              passEntered = tokens[0].substr( 0, PASSMAXLEN );
  if( passEntered != _password ) {  // if the password is not valid ->error
    replyMsg( clientSocket,
              ERR_PASSWDMISMATCH( source, client->getNickname() ) );
    replyMsg( clientSocket, KILL_MSG( source, client->getNickname() ) );
    disconnectAClient( clientSocket );
    return;
  }
  client->setPassStatus( true );  // set pass status to true for registration
  std::cout << MSGINFO << "valid password provided!\n" << std::endl;
}

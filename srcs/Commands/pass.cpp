/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pass.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmelard <lmelard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/03 15:16:32 by lmelard           #+#    #+#             */
/*   Updated: 2023/07/17 17:16:50 by cvidon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Socket.hpp"
#include "utils.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"

/*  ******************************************************************** */
/*  ***** following functions should be put in the Commands folder ***** */
/*  ******************************************************************** */

/* Peut etre mettre des comments dans le serveur (pour indiquer par exemple que le mot de passe est correct....)*/
  // checking if the Client is already registered
  // meaning checking if PASS, NICK, USER are already set
  // if not ERR_ALREADYREGISTERED numeric reply is sent

void	Server::handlePass( int clientSocket, std::string param )
{
  if ( _clients.at( clientSocket ).getIfRegistered() == true ) {
    replyMsg( clientSocket, ERR_ALREADYREGISTRED( _clients.at( clientSocket ).getSource(), _clients.at( clientSocket ).getNickname() ) );
  }
  // else if there is no param to the PASS command
  // ERR_NEEDMOREPARAMS numeric reply is sent
  else if ( param.compare( "" ) == 0 ) {
    replyMsg( clientSocket, ERR_NEEDMOREPARAMS ( _clients.at( clientSocket ).getSource(), _clients.at( clientSocket ).getNickname(), "PASS" ) );
  }
  // else if Pass command's param is different from the password set for the Server
  // then ERR_PASSDMISMATCH error is sent and Client is killed et disconnected
  else if ( param.compare( _password ) != 0 || param.size() != _password.size() ) {
    replyMsg( clientSocket, ERR_PASSWDMISMATCH( _clients.at( clientSocket ).getSource(), _clients.at( clientSocket ).getNickname() ) );
    replyMsg( clientSocket, KILL_MSG( _clients.at( clientSocket ).getSource(), _clients.at( clientSocket ).getNickname() ) );
    disconnectAClient( clientSocket );
  }
  // else if it's the right password, the client is not yet registered then setPassStatus to true
  else {
    _clients.at( clientSocket ).setPassStatus( true );
    std::cout << MSGINFO << "valid password provided!\n" << std::endl;
  }
  return ;
}

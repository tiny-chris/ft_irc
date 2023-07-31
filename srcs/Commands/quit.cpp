/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quit.cpp                                           :+:      :+:    :+:   */
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
 * 				syntax: QUIT [<reason>]
 *
 * 				The client quits the server and every channel he joined.
 * 				All channel members receive a reply saying that client left the
 * channel if client is the last chanop, a new one is asigned but default
 */

void Server::handleQuit( int clientSocket, std::string param ) {
  std::vector<std::string> tokens = splitString( param, ' ' );
  std::string              reason;
  if( tokens.size() && tokens[0].find( ':', 0 ) != std::string::npos ) {
    while( tokens[0].find( ':', 0 ) != std::string::npos ) {
      tokens[0].erase( 0, 1 );
    }
    for( size_t i = 0; i < tokens.size(); i++ ) {
      if( i != 0 ) {
        reason += " ";
      }
      reason += tokens[i];
    }
  }
  if( reason.empty() ) {
    reason = "Quit: ";
  }
  Client* client = &_clients.at( clientSocket );
  if( client->getClientChannels().size() > 0 ) {
    std::vector<std::string> copyClientChannels = client->getClientChannels();
    for( size_t i = 0; i < copyClientChannels.size(); i++ ) {
      leaveChannel( clientSocket, copyClientChannels[i], reason, "QUIT" );
    }
  }
  disconnectAClient( clientSocket );
}

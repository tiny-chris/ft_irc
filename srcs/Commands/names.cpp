/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   names.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgaillag <cgaillag@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by 2.fr>             #+#    #+#             */
/*   Updated: 2023/08/01 17:44:19 by cgaillag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Server.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"
#include "utils.hpp"

/**
 * @brief       Displays member nicknames on one channel by using RPL_NAMEREPLY 
 *              and RPL_ENDOFNAMES
 * 
 *    		      /!\ if a user is invisible --> do not show
 *				      unless requesting client is also joined to that channel
 */

void Server::displayNames( int clientSocket, Channel& channel ) {
  const std::string&      source = _clients.at( clientSocket ).getSource();
  const std::string&      nickname = _clients.at( clientSocket ).getNickname();
  Channel::mapClientsPtr& chanMembers = channel.getChannelMembers();
  Channel::mapClientsPtr::const_iterator it;
  std::string                            listMembers;

  bool requestorIsOnChannel = ( chanMembers.find( nickname )
                                != chanMembers.end() )
                                ? true
                                : false;
  for( it = chanMembers.begin(); it != chanMembers.end(); it++ ) {
    if( it->second->getInvisibleMode() == true
        && requestorIsOnChannel == false )
      continue;
    if( it != chanMembers.begin() ) {
      listMembers += " ";
    }
    if( channel.checkChannelOps( it->second->getNickname() ) == true ) {
      listMembers += "@";
    }
    listMembers += it->first;
  }
  replyMsg(
    clientSocket,
    RPL_NAMREPLY( source, nickname, channel.getChannelName(), listMembers ) );
  replyMsg( clientSocket,
            RPL_ENDOFNAMES( source, nickname, channel.getChannelName() ) );
}

/**
 * @brief       NAMES command
 *              syntax:	NAMES <channel>{,<channel>}
 * 
 *        To view the nicknames joined to a channel and their channel membership 
 *        prefix (@ for chanOp or nothing for normal client)
 *        There can be one or more channels (limited by TARGMAXNAMES), separated 
 *        by a comma --> evaluated one by one
 *        If is only 'NAMES' --> display all channels (one by one)
 */

void Server::handleNames( int clientSocket, std::string param ) {
  const std::string&       source = _clients.at( clientSocket ).getSource();
  const std::string&       nickname = _clients.at( clientSocket ).getNickname();
  mapChannels::iterator    chanIt;
  std::vector<std::string> tokens = splitString( param, ',' );

  if( param.empty() || !tokens.size() 
    || vecStringsAllEmpty( splitString( param, ' ' ) ) ) {
    if( _channels.empty() ) {
      std::cout << MSGINFO << "there is no channel created yet" << std::endl;
      return;
    }
    for( chanIt = _channels.begin(); chanIt != _channels.end(); ++chanIt ) {
      Channel& channel = chanIt->second;
      displayNames( clientSocket, channel );
    }
  } else {
    for( size_t i = 0; i < tokens.size() && i < TARGMAXNAMES; i++ ) {
      chanIt = _channels.find( tokens[i] );
      if( chanIt != _channels.end() ) {
        Channel& channel = chanIt->second;
        displayNames( clientSocket, channel );
      } else {
        replyMsg( clientSocket, RPL_ENDOFNAMES( source, nickname, tokens[i] ) );
      }
    }
  }
  return;
}

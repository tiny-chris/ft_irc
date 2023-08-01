/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgaillag <cgaillag@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by 2.fr>             #+#    #+#             */
/*   Updated: 2023/08/01 09:42:31 by cgaillag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Server.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"
#include "utils.hpp"

/**
 * @brief       Gets the reason why the client was kicked
 * 				      If no reason indicated, default reason displayed.
 */

std::string Server::getKickReason( std::vector<std::string>& tokens ) {
  std::string reason;
  if( tokens.size() >= 3 && tokens[2].find( ':', 0 ) != std::string::npos ) {
    while( tokens[2].find( ':', 0 )
           != std::string::npos ) { 
      tokens[2].erase( 0, 1 );
    }
    for( size_t i = 2; i < tokens.size(); i++ ) {
      if( i != 2 ) {
        reason += " ";
      }
      reason += tokens[i];
    }
  } else {
    reason = KICK_REASON;
  }
  reason = reason.substr( 0, KICKLEN );
  return ( reason );
}

/**
 * @brief       Kicks one user of the channel and deletes it from all map
 * 				      Exception if he the last operator among other clients
 * 			      	Displays a message to all members left on the channel
 */

void Server::kickUser( int clientSocket, Channel* chan, std::string nick,
                       std::string toKick, std::string reason ) {
  Client*     clientToKick = chan->getChannelMembers().at( toKick );
  std::string channelName = chan->getChannelName();
  bool        isLastOperator = ( chan->getChannelOps().size() == 1 );
  bool        isLastChannelMember = ( chan->getChannelMembers().size() == 1 );
  if( isLastOperator && isLastChannelMember ) {
    replyMsg( clientSocket, DEFAULTKICK( nick, channelName, toKick, reason ) );
    replyMsg( clientSocket,
              CHANNEL_DELETED( _clients.at( clientSocket ).getSource(), toKick,
                               channelName ) );
    _channels.erase( channelName );
    clientToKick->removeClientChannel( channelName );
    return;
  }
  if( isLastOperator ) {
    changeChannelOperator( clientSocket, clientToKick, chan );
  }
  channelMsgToAll( clientSocket, channelName,
                   DEFAULTKICK( nick, channelName, toKick, reason ) );
  clientToKick->removeClientChannel( channelName );
  chan->getChannelMembers().erase( toKick );
  chan->getChannelOps().erase( toKick );
}

/**
 * @brief       Iterate on the vector of client to kick and calls kick User
 * 				      for each one of them
 */

void Server::kickSelectedClients( int clientSocket, Channel* chan,
                                  std::vector<std::string> toKick,
                                  std::string              reason ) {
  std::string source = _clients.at( clientSocket ).getSource();
  std::string nick = _clients.at( clientSocket ).getNickname();

  for( size_t i = 0; i < toKick.size() && i < TARGMAXKICK; ++i ) {
    if( !chan->checkChannelMembers( toKick[i] ) ) { 
      replyMsg( clientSocket, ERR_USERNOTINCHANNEL( source, nick, toKick[i],
                                                    chan->getChannelName() ) );
      continue;
    }
    kickUser( clientSocket, chan, nick, toKick[i], reason );
  }
}

/**
 * @brief       KICK command
 * 				      syntax: KICK <channel> <user> *( "," <user> ) [<comment>]
 *
 * 				      Kicks a client off of a Channel
 *              - Checks the parameters are valid (number, channel name)
 * 				      - Check if client has the right to kick on this channel
 * 				      - Kicks at most TARGMAXKICK clients of the channel if they are 
 *                on it
 */

void Server::handleKick( int clientSocket, std::string param ) {
  std::string              source = _clients.at( clientSocket ).getSource();
  std::string              nick = _clients.at( clientSocket ).getNickname();
  std::vector<std::string> tokens = splitString( param, ' ' );

  if( param.empty() || tokens.size() < 2 ) {
    replyMsg( clientSocket, ERR_NEEDMOREPARAMS( source, nick, "KICK" ) );
    return;
  }
  std::string channelName = tokens[0].substr( 0, CHANNELLEN );
  if( !existingChannel( channelName ) ) {
    replyMsg( clientSocket, ERR_NOSUCHCHANNEL( source, nick, channelName ) );
    return;
  }
  Channel* chan = &_channels[channelName];
  if( !chan->checkChannelMembers( nick ) ) { 
    replyMsg( clientSocket, ERR_NOTONCHANNEL( source, nick, channelName ) );
    return;
  }
  if( !chan->checkChannelOps( nick )
      && !_clients.at( clientSocket ).getOperatorMode() ) { 
    replyMsg( clientSocket, ERR_CHANOPRIVSNEEDED( source, nick, channelName ) );
    return;
  }
  std::vector<std::string> toKick = splitString( tokens[1], ',' );
  kickSelectedClients( clientSocket, chan, toKick, getKickReason( tokens ) );
}

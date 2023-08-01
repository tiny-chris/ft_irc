/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgaillag <cgaillag@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by 2.fr>             #+#    #+#             */
/*   Updated: 2023/08/01 19:32:47 by cgaillag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Server.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"
#include "utils.hpp"

/**
 * @brief       Function that displays user mode updates and errors
 */

void Server::displayUserModeChanges( Client*                         client,
                                     const std::vector<std::string>& tokens,
                                     std::string& modechange ) {
  if( modechange.size() > 1 ) {
    replyMsg(
      client->getFd(),
      MSG_MODE( client->getSource(), client->getNickname(), modechange, "" ) );
    size_t diff = tokens[1].size() - modechange.size();
    while( diff ) {
      replyMsg(
        client->getFd(),
        ERR_UMODEUNKNOWNFLAG( client->getSource(), client->getNickname() ) );
      diff--;
    }
  }
}

/**
 * @brief       Function that go through the modeString
 *              and set or unset the given user modes
 */

void Server::updateUserMode( Client*                         client,
                             const std::vector<std::string>& tokens,
                             std::string&                    modechange ) {
  int clientSocket = client->getFd();
  std::string source = client->getSource();
  std::string nickname = client->getNickname();
  char modePrefix = getModePrefix( tokens[1] );
  if (modePrefix == 'O')
    std::cout << MSGINFO << "Prefix must be + or -\n" << std::endl;
  modechange += modePrefix;
  std::string modeString = tokens[1].substr( 1, tokens[1].size() - 1 );
  for( size_t i = 0; i < modeString.size(); i++ ) {
    char modeChar = modeString[i];
    if( modePrefix == '+' ) {
      if (!client->handleUserModeSet( modeChar, &modechange )){
        replyMsg( clientSocket, ERR_UMODEUNKNOWNFLAG( source, nickname ) );
      }
    } else if( modePrefix == '-' ) {
      if (!client->handleUserModeUnset( modeChar, &modechange )) {
        replyMsg( clientSocket, ERR_UMODEUNKNOWNFLAG( source, nickname ) );
      }
    }
  }
}

/**
 * @brief       Set or unset modes from a given user
 *              If no modestring given the function displays current user modes
 *              All changes are displayed to client
 */

void Server::handleUserMode( int                       clientSocket,
                             std::vector<std::string>& tokens ) {
  Client*     client = &_clients.at( clientSocket );
  std::string source = client->getSource();
  std::string nickname = client->getNickname();
  std::string userName = tokens[0];
  std::string modechange;

  if( !existingNick( userName ) ) {
    replyMsg( clientSocket, ERR_NOSUCHNICK( source, nickname, userName ) );
    return;
  }
  if( userName != nickname ) {
    replyMsg( clientSocket, ERR_USERSDONTMATCH( source, nickname ) );
    return;
  }
  if( tokens.size() < 2 ) { 
    modechange = client->getModes();
    replyMsg( clientSocket, RPL_UMODEIS( source, nickname, modechange ) );
    return;
  }
  updateUserMode( client, tokens, modechange );
  if( modechange.size() > 1 ) {
    replyMsg( clientSocket, MSG_MODE( source, nickname, modechange, "" ) );
  }
}

/**
 * @brief       Set or unset modes from a given channel
 *              If no modestring given the function displays current channel
 * modes The client need to be chanop to call this function All changes are
 * displayed to channel members
 */

void Server::handleChannelMode( int clientSocket, std::string& channelName,
                                const std::vector<std::string>& tokens ) {
  Client*     client = &_clients.at( clientSocket );
  std::string source = client->getSource();
  std::string clientName = client->getNickname();
  std::string modeChange, modeArgs;

  if( !existingChannel( channelName ) ) {
    replyMsg( clientSocket,
              ERR_NOSUCHCHANNEL( source, clientName, channelName ) );
    return;
  }
  if( tokens.size() < 2 ) {
    modeChange = _channels[channelName].getModes();
    modeArgs = _channels[channelName].getModesArgs();
    replyMsg( clientSocket, RPL_CHANNELMODEIS( source, clientName, channelName,
                                               modeChange, modeArgs ) );
    return;
  }
  Channel* chan = &_channels[channelName];
  if( tokens[1] == "b" ) {
    replyMsg( clientSocket,
              RPL_ENDOFBANLIST( source, clientName, channelName ) );
    return;
  }
  if( !chan->checkChannelOps( clientName ) && !client->getOperatorMode() ) {
    replyMsg( clientSocket,
              ERR_CHANOPRIVSNEEDED( source, clientName, channelName ) );
    return;
  }
  chan->updateChannelMode( tokens, modeChange, modeArgs );
  if( modeChange.size() > 1 ) {
    channelMsgToAll(
      clientSocket, channelName,
      MSG_MODE_CUSTOM( source, channelName, modeChange + " " + modeArgs ) );
  }
}

/**
 * @brief       MODE command
 *              MODE <target> [<modestring> [<mode arguments>...]]
 *
 *              Sets or removes options (or modes) from a given target.
 *              2 types of tagets : Channel and User
 */

void Server::handleMode( int clientSocket, std::string param ) {
  std::vector<std::string> tokens = splitString( param, ' ' );
  if( param.empty() || vecStringsAllEmpty( tokens ) ) {
    replyMsg(
      clientSocket,
      ERR_NEEDMOREPARAMS( _clients.at( clientSocket ).getSource(),
                          _clients.at( clientSocket ).getNickname(), "MODE" ) );
    return;
  }
  bool isChannelMode = tokens[0].find( '#', 0 ) != std::string::npos;
  if( isChannelMode ) {
    handleChannelMode( clientSocket, tokens[0], tokens );
  } else {
    handleUserMode( clientSocket, tokens );
  }
}

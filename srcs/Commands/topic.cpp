/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgaillag <cgaillag@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by 2.fr>             #+#    #+#             */
/*   Updated: 2023/08/01 17:59:19 by cgaillag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Server.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"
#include "utils.hpp"

/**
 * @brief       Sends the new topic and its creation info
 *              to every channel members
 */

void Server::sendTopicToChannelMembers( Channel* chan ) {
  std::string channelName = chan->getChannelName();
  std::string topic = chan->getTopic();
  std::string topicSetter = chan->getTopicSetter();
  std::string topicDate = chan->getTopicDate();

  for( mapClientsPtr::iterator it = chan->getChannelMembers().begin();
       it != chan->getChannelMembers().end(); it++ ) {
    int         socket = it->second->getFd();
    std::string source = it->second->getSource();
    std::string nick = it->second->getNickname();

    replyMsg( socket, RPL_TOPIC( source, nick, channelName, topic ) );
    replyMsg( socket, RPL_TOPICWHOTIME( source, nick, channelName, topicSetter,
                                        topicDate ) );
  }
}

/**
 * @brief       Updates the channel topic and the creation informations
 */

void Server::updateChannelTopic( Channel* chan, std::string newTopic,
                                 std::string nick ) {
  chan->setTopicDate( getCurrentDate() );
  chan->setTopic( newTopic );
  chan->setTopicSetter( nick );
}

/**
 * @brief       Gets the new channel topic
 */

std::string Server::getNewTopic( std::vector<std::string>& tokens ) {
  std::string newTopic = "";
  if( tokens.size() >= 2 && tokens[1].find( ':', 0 ) != std::string::npos ) {
    while( tokens[1].find( ':', 0 ) != std::string::npos ) {
      tokens[1].erase( 0, 1 );
    }
    for( size_t i = 1; i < tokens.size(); i++ ) {
      if( i != 1 ) {
        newTopic += " ";
      }
      newTopic += tokens[i];
    }
  } else if( tokens.size() >= 2
             && tokens[1].find( ':', 0 ) == std::string::npos ) {
    newTopic = tokens[1];
  }
  return ( newTopic.substr( 0, TOPICLEN ) );
}

/**
 * @brief       Displays the current topic, it creation time and who created it
 *              When no topic set a NO_TOPIC reply is sent
 */

void Server::handleTopicDisplay( int clientSocket, Channel* chan ) {
  std::string source = _clients.at( clientSocket ).getSource();
  std::string nick = _clients.at( clientSocket ).getNickname();
  std::string channelName = chan->getChannelName();

  if( chan->getTopic().empty() ) {
    replyMsg( clientSocket, RPL_NOTOPIC( source, nick, channelName ) );
  } else {
    replyMsg( clientSocket,
              RPL_TOPIC( source, nick, channelName, chan->getTopic() ) );
    replyMsg( clientSocket, RPL_TOPICWHOTIME( source, nick, channelName,
                                              chan->getTopicSetter(),
                                              chan->getTopicDate() ) );
  }
}

/**
 * @brief       TOPIC command
 *              TOPIC <channel> [<topic>]
 *
 *              Changes or views a channel topic
 *              - Checks rights to change topic (+t mode and chanops)
 *              - Displays the topic if no arguments are given
 *              - Updates the topic and displays it to all chan members
 */

void Server::handleTopic( int clientSocket, std::string param ) {
  std::string source = _clients.at( clientSocket ).getSource();
  std::string nick = _clients.at( clientSocket ).getNickname();
  std::vector<std::string> tokens = splitString( param, ' ' );
  if( param.empty() || vecStringsAllEmpty( tokens )) {
    replyMsg( clientSocket, ERR_NEEDMOREPARAMS( source, nick, "TOPIC" ) );
    return;
  }
  std::string              channelName = tokens[0];
  if( !existingChannel( channelName ) ) {
    replyMsg( clientSocket, ERR_NOSUCHCHANNEL( source, nick, channelName ) );
    return;
  }
  Channel* chan = &_channels[channelName];
  if( !chan->checkChannelMembers( nick ) ) {
    replyMsg( clientSocket, ERR_NOTONCHANNEL( source, nick, channelName ) );
    return;
  }
  if( tokens.size() < 2 ) {
    handleTopicDisplay( clientSocket, chan );
    return;
  }
  if( chan->getTopicRestrictionStatus() == true
      && !chan->checkChannelOps( nick )
      && !_clients.at( clientSocket ).getOperatorMode() ) {
    replyMsg( clientSocket, ERR_CHANOPRIVSNEEDED( source, nick, channelName ) );
    return;
  }
  std::string newTopic = getNewTopic( tokens );
  if( newTopic != chan->getTopic() ) {
    updateChannelTopic( chan, newTopic, nick );
    sendTopicToChannelMembers( chan );
  }
}

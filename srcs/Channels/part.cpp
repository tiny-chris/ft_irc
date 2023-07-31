/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   part.cpp                                           :+:      :+:    :+:   */
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
 * @brief       Pre-check before leaving a channel: channel exists and client is
 * joined to it
 *
 */

bool Server::checkChanPrePart( int                clientSocket,
                               const std::string& channelName ) {
  Client& client = _clients.at( clientSocket );

  if( existingChannel( channelName ) == false ) {
    replyMsg( clientSocket,
              ERR_NOSUCHCHANNEL( client.getSource(), client.getNickname(),
                                 channelName ) );
    return false;
  }

  Channel& channel = _channels.at( channelName );
  if( channel.getChannelMembers().find( client.getNickname() )
      == channel.getChannelMembers().end() ) {
    replyMsg( clientSocket,
              ERR_NOTONCHANNEL( client.getSource(), client.getNickname(),
                                channelName ) );
    return false;
  }
  return true;
}

/**
 * @brief       Leave channel: removing client from the channel (as member, as
 *chanOp if applied) and removing channel into client object
 *
 *	Specific cases:
 *	- if client is last member (chanOp): remove client from channel & delete
 *channel
 *	- if client ifslast chanOp with other members: design another chanOp and
 *remove client from channel
 *
 */

void Server::leaveChannel( int clientSocket, const std::string& channelName,
                           const std::string& reason, const std::string& cmd ) {
  Client*  client = &_clients.at( clientSocket );
  Channel* channel = &_channels.at( channelName );

  if( channel->checkChannelOps( client->getNickname() ) == true
      && channel->getChannelOps().size() == 1 ) {
    if( channel->getChannelMembers().size() == 1 ) {
      if( cmd == "PART" )
        replyMsg( clientSocket,
                  RPL_PART( client->getSource(), client->getNickname(),
                            channelName, reason ) );
      else if( cmd == "QUIT" )
        std::cout << MSGINFO << client->getNickname() << " has Quit: " << reason
                  << std::endl;
      channel->removeChannelOp( client );
      channel->removeChannelMember( client );
      _channels.erase( channelName );
      client->removeClientChannel( channelName );
      return;
    } else {
      changeChannelOperator( clientSocket, client,
                             channel );  // function in kick.cpp file
    }
  }
  if( cmd == "PART" )  // client is not chanOps or is one of many chanOps -->
                       // remove client from Channel
    channelMsgToAll( clientSocket, channelName,
                     RPL_PART( client->getSource(), client->getNickname(),
                               channelName, reason ) );
  else if( cmd == "QUIT" ) {
    channelMsgNotClient(
      clientSocket, channelName,
      RPL_QUIT( client->getSource(), client->getNickname(), reason ) );
    std::cout << MSGINFO << client->getNickname() << " has Quit: " << reason
              << std::endl;
  }
  channel->removeChannelOp( client );
  channel->removeChannelMember( client );
  client->removeClientChannel( channelName );
}

/**
 * @brief       PART command
 *				syntax:			PART <channel>{,<channel>} [<reason>]
 *
 *	For each channel in the parameter of this command:
 *	- if the channel exists and the client is joined to it --> client leaves
 *	- if not --> consider replies ERR_NOSUCHCHANNEL, ERR_NOTONCHANNEL
 *
 */

void Server::handlePart( int clientSocket, std::string param ) {
  Client&                  client = _clients.at( clientSocket );
  std::string              channelList = "", reason = "";
  std::vector<std::string> channelNames;

  if( param.empty()
      || splitStringInTwo( param, ' ', &channelList, &reason ) == false ) {
    replyMsg(
      clientSocket,
      ERR_NEEDMOREPARAMS( client.getSource(), client.getNickname(), "PART" ) );
    return;
  }
  if( !reason.empty() ) {
    if( reason.at( 0 ) == ':' )
      reason = " " + reason;
    else
      reason = " :" + reason;
  }
  channelNames = splitString( channelList, ',' );
  for( size_t i = 0; i < channelNames.size(); ++i ) {
    std::string channelName = channelNames[i];
    if( channelName.size() > CHANNELLEN )
      channelName = channelName.substr( 0, CHANNELLEN );
    if( checkChanPrePart( clientSocket, channelName ) == false )
      continue;
    leaveChannel( clientSocket, channelName, reason, "PART" );
  }
}

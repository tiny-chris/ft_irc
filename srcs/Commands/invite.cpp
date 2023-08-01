/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgaillag <cgaillag@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by 2.fr>             #+#    #+#             */
/*   Updated: 2023/08/01 17:31:10 by cgaillag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Server.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"
#include "utils.hpp"

/**
 * @brief       Invites the designted client to the designated channel
 *              - Checks if the client exists
 *              - Adds the client to _invitedMember vector
 *              - Sends an invite reply to the client
 */

void Server::inviteClientToChannel( int clientSocket, std::string clientNick,
                                    std::string nameInvitee, Channel* chan ) {
  Client* toAdd = getClientByNickname( nameInvitee );
  if( toAdd == NULL ) {
    std::cout << MSGERROR << " No such Nickname, cannot invite this client"
              << std::endl;
    return;
  }
  chan->addInvitedMember( nameInvitee );
  replyMsg( clientSocket,
            RPL_INVITING( _clients.at( clientSocket ).getSource(), clientNick,
                          nameInvitee, chan->getChannelName() ) );
  replyMsg( toAdd->getFd(),
            INVITE( clientNick, nameInvitee, chan->getChannelName() ) );
}

/**
 * @brief       INVITE command
 *              INVITE <nickname> <channel>
 *
 *              Invites a client on a Channel
 *              - Checks rights to invite
 *              - Checks on invited client
 *              - Invites client on channel
 */

void Server::handleInvite( int clientSocket, std::string param ) {
  std::string              source = _clients.at( clientSocket ).getSource();
  std::string              nick = _clients.at( clientSocket ).getNickname();
  std::vector<std::string> tokens = splitString( param, ' ' );

  if( param.empty() || tokens.size() < 2 || vecStringsAllEmpty( tokens ) ) { 
    replyMsg( clientSocket, ERR_NEEDMOREPARAMS( source, nick, "KICK" ) );
    return;
  }
  std::string channelName = tokens[1];
  if( !existingChannel( channelName ) ) { 
    replyMsg( clientSocket, ERR_NOSUCHCHANNEL( source, nick, channelName ) );
    return;
  }
  Channel* chan = &_channels[channelName];
  if( !chan->checkChannelMembers( nick ) ) {  // if client is channel member
    replyMsg( clientSocket, ERR_NOTONCHANNEL( source, nick, channelName ) );
    return;
  }
  if( !chan->checkChannelOps( nick )
      && !_clients.at( clientSocket ).getOperatorMode() ) { 
    replyMsg( clientSocket, ERR_CHANOPRIVSNEEDED( source, nick, channelName ) );
    return;
  }
  std::string toInvite = tokens[0];
  if( chan->checkChannelMembers( toInvite ) ) {
    replyMsg( clientSocket,
              ERR_USERONCHANNEL( source, nick, toInvite, channelName ) );
    return;
  }
  inviteClientToChannel( clientSocket, nick, toInvite, chan );
}

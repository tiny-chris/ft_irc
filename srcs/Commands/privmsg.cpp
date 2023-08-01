/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgaillag <cgaillag@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by 2.fr>             #+#    #+#             */
/*   Updated: 2023/08/01 12:16:41 by cgaillag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Server.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"
#include "utils.hpp"


bool  Server::checkTargetPrivmsg( int clientSocket, std::string& target )
{
  Client*             client = &_clients.at( clientSocket );
  const std::string&  source = client->getSource();
  const std::string&  nickname = client->getNickname();
 
  if ( target.empty()) {
      replyMsg( clientSocket, ERR_NOSUCHNICK( source, nickname, "<empty>" ) );
      return false;
  }
  if( target.find( "#" ) != std::string::npos ) {
    if ( target.find( "#" ) != 0 && target.find( "@#" ) != 0 ) {
      replyMsg( clientSocket, ERR_NOSUCHCHANNEL( source, nickname, target ) );
      return false;
    }
    target = target.substr( target.find( "#" ) );
    if( target.size() > CHANNELLEN ) {
      target = target.substr( 0, CHANNELLEN );
    }
    // if channel does not exist
    if( !existingChannel( target ) ) {  
      replyMsg( clientSocket,
                ERR_NOSUCHCHANNEL( source, nickname, target ) );
      return false;
    }
    Channel channel = _channels.at( target );
    // if client is on Channel
    if( !channel.checkChannelMembers( nickname ) ) {
      replyMsg( clientSocket,
                  ERR_CANNOTSENDTOCHAN( source, nickname, target ) );
      return false;
    }
  }
  else {
    // if client does not exist
    if( !existingClient( target ) ) {
      replyMsg( clientSocket, ERR_NOSUCHNICK( source, nickname, target ) );
      return false;
    }
    // if client wants to send a message to itself
    if( target == nickname ) {
      replyMsg( clientSocket, "clients cannot send message to themselves");
      return false;
    }
  }
  return true;
}

/**
 * @brief       PRIVMSG command
 *			      	syntax: PRIVMSG <target> <message>
 *
 *	Send a message to the target (client or channel)
 *	- Check for a channel target: channel exists, client is joined to it
 *		(specific case: if only addressed to chanOps)
 * 	- Check for a client target: client exists
 *
 */

void Server::handlePrivmsg( int clientSocket, std::string param ) {
  Client*            client = &_clients.at( clientSocket );
  const std::string& source = client->getSource();
  const std::string& nickname = client->getNickname();
  std::string        targetList = "", textToBeSent = "";

  if( param.empty()
      || splitStringInTwo( param, ' ', &targetList, &textToBeSent ) == false ) {
    replyMsg( clientSocket, ERR_NORECIPIENT( source, nickname, "PRIVMSG" ) );
    replyMsg( clientSocket, ERR_NOTEXTTOSEND( source, nickname ) );
    return;
  } else if( targetList.empty() || textToBeSent.empty() ) {
    replyMsg( clientSocket, ERR_NEEDMOREPARAMS( source, nickname, "PRIVMSG" ) );
    return;
  }

  if( textToBeSent.at( 0 ) == ':' ) {
    textToBeSent.erase( 0, 1 );
  }
  std::vector<std::string> targetNames = splitString( targetList, ',' );

  for( size_t i = 0; i < targetNames.size() && i < TARGMAXMSG; ++i ) {
    std::string target = targetNames[i];

    if( checkTargetPrivmsg( clientSocket, target ) == false ) {
      continue;
    }
    std::string reply = RPL_PRIVMSG( source, nickname, target, textToBeSent );
    if ( target[0] == '#' ) {
      channelMsgNotClient( clientSocket, target, reply );
    }
    else if ( target[0] == '@' ) {
      channelMsgToChanOps( clientSocket, target, reply );
    }
    else {
      replyMsg( findClientFd( target ), reply );
    }
  }
}

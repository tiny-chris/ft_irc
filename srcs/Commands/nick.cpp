/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmelard <lmelard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/03 15:16:17 by lmelard           #+#    #+#             */
/*   Updated: 2023/07/26 15:00:38 by lmelard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Socket.hpp"
#include "utils.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"

void		Server::handleNick( int clientSocket, std::string param )
{

  std::string source = _clients.at( clientSocket ).getSource();
	std::string nick = _clients.at( clientSocket ).getNickname();
  std::vector<std::string> tokens = splitString( param, ' ' ); 
  
  if ( param.empty() ) {
    replyMsg(clientSocket, ERR_NONICKNAMEGIVEN(source, nick));
  }
  std::string newNick = tokens[0];
  if (newNick.size() > NICKLEN) {
    newNick = newNick.substr(0, NICKLEN);
  }
  // else if there are invalid char in the nickname, an erroneus nickname message is sent
  if (!isValidNick(newNick)) {
    replyMsg(clientSocket, ERR_ERRONEUSNICKNAME(source, nick, newNick));
  }
  // else if the nickname is the same nickname as another client
  else if (existingNick(newNick)) {
    // and if our client is not yet registered then an nick collision occurs and the client is killed
    if (_clients.at( clientSocket ).getIfRegistered() == false) {
      replyMsg(clientSocket, ERR_NICKCOLLISION(source, nick, newNick));
      replyMsg(clientSocket, KILL_MSG(source, nick));
      disconnectAClient( clientSocket );
    }
    // but if our client is already registered then a nickname in use error occurs
    // the client keeps its nickname
    else {
      replyMsg(clientSocket, ERR_NICKNAMEINUSE( source, nick, newNick ));
    }
  }
  // else meaning if the nickname given is valid and does not already exist
  // nickname is set to param value and nickstatus set to true
  else {
    // std::string oldNickname = _clients.at( clientSocket ).getNickname();
    replyMsg(clientSocket, RPL_NICK(nick, newNick));
    _clients.at( clientSocket ).setNickStatus( true );
    _clients.at( clientSocket ).setNickname( newNick );
    std::cout << MSGINFO << "valid nickname provided!" << std::endl;
    if ( _clients.at( clientSocket ).getIfRegistered() == false ) {
      checkRegistration( clientSocket );
    }
    else { // maj de la source + maj des nickname dans les map de chanops et de chanmembers dans les channels;
    
      _clients.at( clientSocket ).setSource( newNick, _clients.at( clientSocket ).getUsername() );
      updateChannelMemberNick( nick, newNick );
      updateChannelOpsNick( nick, newNick );
      updateInvitedMembersNick( nick, newNick );
    }
    std::cout << std::endl;
  }
  return ;
}

bool		Server::isValidNick(std::string param)
{
  for (size_t i = 0; i < param.size(); i++) {
    char c = param[i];
    if (!(isalnum(c) || c == '[' || c == ']' || c == '{' \
          || c == '}' || c == '\\' || c == '|' || c == '_'))
      return false;
  }
  return true;
}

bool  Server::existingNick( std::string param )
{
  for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
    if (it->second.getNickname() == param ) {
      return true;
    }
  }
  return false;
}

void  Server::updateChannelMemberNick( std::string &oldNickname, std::string nickName )
{
  for (std::map<std::string, Channel>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
    std::map< std::string, Client* >	&mapClients = it->second.getChannelMembers();
    std::map<std::string, Client *>::iterator elem = mapClients.find(oldNickname);
    if (elem != mapClients.end()) {
      Client *tmp = elem->second;
      mapClients.erase( oldNickname );
      mapClients.insert(std::make_pair(nickName, tmp));
    }
  }
}

void  Server::updateChannelOpsNick( std::string &oldNickname, std::string nickName )
{
  for (std::map<std::string, Channel>::iterator it = _channels.begin(); it != _channels.end(); ++it)
  {
    std::map< std::string, Client* >	&mapClients = it->second.getChannelOps();
    std::map<std::string, Client *>::iterator elem = mapClients.find(oldNickname);
    if (elem != mapClients.end()) {
      Client *tmp = elem->second;
      mapClients.erase( oldNickname );
      mapClients.insert(std::make_pair(nickName, tmp));
    }
  }
}

void  Server::updateInvitedMembersNick( std::string &oldNickname, std::string nickName )
{
  for (std::map<std::string, Channel>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
    std::vector< std::string >	&invitedmembers = it->second.getInvitedMembers();
    for (size_t i = 0; i < invitedmembers.size() ; ++i) {
      if (invitedmembers.at(i) == oldNickname) {
        invitedmembers.erase(invitedmembers.begin() + i);
        invitedmembers.push_back(nickName);
        break ;
      }
    }
  }
}
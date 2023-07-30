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


/**
 * @brief       Command that allows to change the nickname of a Client
 *              - Checks if the parameter are valid
 *              - Checks if the new nickname is already in use
 *              - Updates the nickname of the client everywhere it is used
 */

void		Server::handleNick( int clientSocket, std::string param ) {
  Client *client = &_clients.at( clientSocket );
  std::string source = client->getSource();
	std::string nick = client->getNickname();
  std::vector<std::string> tokens = splitString( param, ' ' ); 
  if ( param.empty() ) {
    replyMsg(clientSocket, ERR_NONICKNAMEGIVEN(source, nick));
    return ;
  }
  std::string newNick = tokens[0].substr(0, NICKLEN);
  if (!isValidNick(newNick)) {
    replyMsg(clientSocket, ERR_ERRONEUSNICKNAME(source, nick, newNick));
    return;
  }
  else if (existingNick(newNick)) { // if another client already has its nickname
    handleExistingNick( clientSocket, newNick );
    return ;
  }
  updateNickname( clientSocket, nick, newNick );
  if ( !client->getIfRegistered() ) {
    checkRegistration( clientSocket );
    return ; 
  }
  updateNickInChannels(clientSocket, nick, newNick );
  std::cout << std::endl;
}

/**
 * @brief       Checks if the nickname format is valid
 *              Allows all alphanumerical characters, 
 *              square and curly brackets ([]{}), 
 *              backslashes (\), and pipe (|) 
 *              no leading # character, leading colon (:), ASCII space
 */

bool		Server::isValidNick(std::string param) {
  for (size_t i = 0; i < param.size(); i++) {
    char c = param[i];
    if (!(isalnum(c) || c == '[' || c == ']' || c == '{' \
          || c == '}' || c == '\\' || c == '|' || c == '_'))
      return false;
  }
  return true;
}

/**
 * @brief       Checks if the new nickname has already been assigned
 *              to another client
 */

bool  Server::existingNick( std::string param ) {
  for (mapClients::iterator it = _clients.begin(); it != _clients.end(); ++it) {
    if (it->second.getNickname() == param ) {
      return true;
    }
  }
  return false;
}

/**
 * @brief       Updates nickname in channel members maps
 */

void  Server::updateChannelMemberNick( std::string &oldNickname, std::string nickName ) {
  for (mapChannels::iterator it = _channels.begin(); it != _channels.end(); ++it) {
    mapClientsPtr	&mapClients = it->second.getChannelMembers();
    mapClientsPtr::iterator elem = mapClients.find(oldNickname);
    if (elem != mapClients.end()) {
      Client *tmp = elem->second;
      mapClients.erase( oldNickname );
      mapClients.insert(std::make_pair(nickName, tmp));
    }
  }
}

/**
 * @brief       Updates nickname in channel operators maps
 */

void  Server::updateChannelOpsNick( std::string &oldNickname, std::string nickName ) {
  for (mapChannels::iterator it = _channels.begin(); it != _channels.end(); ++it) {
    mapClientsPtr	&mapClients = it->second.getChannelOps();
    mapClientsPtr::iterator elem = mapClients.find(oldNickname);
    if (elem != mapClients.end()) {
      Client *tmp = elem->second;
      mapClients.erase( oldNickname );
      mapClients.insert(std::make_pair(nickName, tmp));
    }
  }
}

/**
 * @brief       Updates nickname in invited Members vector
 */

void  Server::updateInvitedMembersNick( std::string &oldNickname, std::string nickName ) {
  for (mapChannels::iterator it = _channels.begin(); it != _channels.end(); ++it) {
    vecString	&invitedmembers = it->second.getInvitedMembers();
    for (size_t i = 0; i < invitedmembers.size() ; ++i) {
      if (invitedmembers.at(i) == oldNickname) {
        invitedmembers.erase(invitedmembers.begin() + i);
        invitedmembers.push_back(nickName);
        break ;
      }
    }
  }
}

/**
 * @brief       Sends error numeric replies if the nickname is
 *              already in use
 */

void  Server::handleExistingNick(int clientSocket, std::string newNick)
{
    Client *client = &_clients.at( clientSocket );
    std::string source = client->getSource();
	  std::string nick = client->getNickname();

    if (!client->getIfRegistered()) { // and if our client is not yet registered then an nick collision occurs and the client is killed
    replyMsg(clientSocket, ERR_NICKCOLLISION(source, nick, newNick));
    replyMsg(clientSocket, KILL_MSG(source, nick));
    disconnectAClient( clientSocket );
    }
    else {  // but if our client is already registered then a nickname in use error occurs the client keeps its nickname
      replyMsg(clientSocket, ERR_NICKNAMEINUSE( source, nick, newNick ));
    }
}

/**
 * @brief       Updates the nickname in Client with the new 
 *              valid nickname
 */

void  Server::updateNickname(int clientSocket, std::string nick, std::string newNick)
{
  Client *client = &_clients.at( clientSocket );
  replyMsg(clientSocket, RPL_NICK(nick, newNick));
  client->setNickStatus( true );
  client->setNickname( newNick );
  std::cout << MSGINFO << "valid nickname provided!" << std::endl;
}

/**
 * @brief       Updates the client source and 
 *              nickname in all the client's Channels
 */

void  Server::updateNickInChannels( int clientSocket, std::string nick, std::string newNick )
{
  Client *client = &_clients.at( clientSocket );
  client->setSource( newNick, client->getUsername() );
  updateChannelMemberNick( nick, newNick );
  updateChannelOpsNick( nick, newNick );
  updateInvitedMembersNick( nick, newNick );
}

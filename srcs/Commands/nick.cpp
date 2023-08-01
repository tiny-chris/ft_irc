/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgaillag <cgaillag@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by 2.fr>             #+#    #+#             */
/*   Updated: 2023/08/01 19:51:40 by cgaillag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "utils.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"


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
  std::cout << MSGINFO << "valid nickname provided!\n" << std::endl;
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

    if (!client->getIfRegistered()) { 
    replyMsg(clientSocket, ERR_NICKCOLLISION(source, nick, newNick));
    replyMsg(clientSocket, KILL_MSG(source, nick));
    disconnectAClient( clientSocket );
    }
    else {  
      replyMsg(clientSocket, ERR_NICKNAMEINUSE( source, nick, newNick ));
    }
}

/**
 * @brief       NICK command
 *              NICK <nickname>
 *
 *              Changes the nickname of a Client
 *              - Checks if the parameter are valid
 *              - Checks if the new nickname is already in use
 *              - Updates the nickname of the client everywhere it is used
 */

void		Server::handleNick( int clientSocket, std::string param ) {
  Client *client = &_clients.at( clientSocket );
  std::string source = client->getSource();
	std::string nick = client->getNickname();
  std::vector<std::string> tokens = splitString( param, ' ' );
  if ( param.empty() || vecStringsAllEmpty(tokens ) ) {
    replyMsg(clientSocket, ERR_NONICKNAMEGIVEN(source, nick));
    return ;
  }
  std::string newNick = tokens[0].substr(0, NICKLEN);
  if (!isValidNick(newNick)) {
    replyMsg(clientSocket, ERR_ERRONEUSNICKNAME(source, nick, newNick));
    return;
  }
  else if (existingNick(newNick)) { 
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

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/03 15:16:17 by lmelard           #+#    #+#             */
/*   Updated: 2023/07/24 17:22:22 by codespace        ###   ########.fr       */
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
  // std::string reply;
  // if no param to a nick command -> No nickname given error
  // OPTION WE TRUNC PARAM IF ITS TOO LONG
  if (param.size() > NICKLEN)
    param = param.substr(0, NICKLEN);
  if (param.compare("") == 0)
  {
    replyMsg(clientSocket, ERR_NONICKNAMEGIVEN(_clients.at( clientSocket ).getSource(), _clients.at( clientSocket ).getNickname()));
  }
  // else if there are invalid char in the nickname, an erroneus nickname message is sent
  else if (isValidNick(param) == false)
  {
    replyMsg(clientSocket, ERR_ERRONEUSNICKNAME(_clients.at( clientSocket ).getSource(), _clients.at( clientSocket ).getNickname(), param));
  }
  // else if the nickname is the same nickname as another client
  else if (existingNick(param) == true)
  {
    // and if our client is not yet registered then an nick collision occurs and the client is killed
    if (_clients.at( clientSocket ).getIfRegistered() == false)
    {
      replyMsg(clientSocket, ERR_NICKCOLLISION(_clients.at( clientSocket ).getSource(), _clients.at( clientSocket ).getNickname(), param));
      replyMsg(clientSocket, KILL_MSG(_clients.at( clientSocket ).getSource(), _clients.at( clientSocket ).getNickname()));
      disconnectAClient( clientSocket );
    }
    // but if our client is already registered then a nickname in use error occurs
    // the client keeps its nickname
    else
    {
      replyMsg(clientSocket, ERR_NICKNAMEINUSE(_clients.at( clientSocket ).getSource(), _clients.at( clientSocket ).getNickname(), param));
    }
  }
  // else meaning if the nickname given is valid and does not already exist
  // nickname is set to param value and nickstatus set to true
  else
  {
    std::string oldNickname = _clients.at( clientSocket ).getNickname();
    replyMsg(clientSocket, RPL_NICK(_clients.at( clientSocket ).getNickname(), param));
    _clients.at( clientSocket ).setNickStatus( true );
    _clients.at( clientSocket ).setNickname( param );
    std::cout << MSGINFO << "valid nickname provided!" << std::endl;


// /*  ************************************  */
// /*       POUR CHECKER LES SOCKETS !!      */
// /*  ************************************  */
// std::cout << "\n\t *** INFO BEFORE CHECKING REGISTRATION FUNCTION: ***\n";
// std::cout << "\t getPassStatus <"<< _clients.at( clientSocket ).getPassStatus() << ">\n";
// std::cout << "\t getNickStatus <"<< _clients.at( clientSocket ).getNickStatus() << ">\n";
// std::cout << "\t getUsername <"<< _clients.at( clientSocket ).getUsername() << ">\n";
// std::cout << std::endl;
// /*  ************************************  */
// /*  ************************************  */


    if ( _clients.at( clientSocket ).getIfRegistered() == false ) {
      checkRegistration( clientSocket );
    }
    else // maj de la source + maj des nickname dans les map de chanops et de chanmembers dans les channels;
    {
      _clients.at( clientSocket ).setSource( _clients.at( clientSocket ).getNickname(), _clients.at( clientSocket ).getUsername() );
      updateChannelMemberNick( oldNickname, _clients.at( clientSocket ).getNickname() );
      updateChannelOpsNick( oldNickname, _clients.at( clientSocket ).getNickname() );
    }
    std::cout << std::endl;
  }
  return ;
}

bool		Server::isValidNick(std::string param)
{
  for (size_t i = 0; i < param.size(); i++)
  {
    char c = param[i];
    if (!(isalnum(c) || c == '[' || c == ']' || c == '{' \
          || c == '}' || c == '\\' || c == '|' || c == '_'))
      return false;
  }
  return true;
}

bool  Server::existingNick( std::string param )
{
  for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
  {
    if (it->second.getNickname() == param )
      return true;
  }
  return false;
}

void  Server::updateChannelMemberNick( std::string &oldNickname, std::string nickName )
{
  for (std::map<std::string, Channel>::iterator it = _channels.begin(); it != _channels.end(); ++it)
  {
    std::map< std::string, Client* >	&mapClients = it->second.getChannelMembers();
    std::map<std::string, Client *>::iterator elem = mapClients.find(oldNickname);
    if (elem != mapClients.end())
    {
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
    if (elem != mapClients.end())
    {
      Client *tmp = elem->second;
      mapClients.erase( oldNickname );
      mapClients.insert(std::make_pair(nickName, tmp));
    }
  }
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgaillag <cgaillag@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/03 15:16:17 by lmelard           #+#    #+#             */
/*   Updated: 2023/07/06 13:40:10 by cgaillag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Socket.hpp"
#include "utils.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"

void		Server::handleNick( size_t cid, std::string param )
{
  std::string reply;
  // if no param to a nick command -> No nickname given error
  if (param.compare("") == 0)
  {
    reply = ERR_NONICKNAMEGIVEN(_clients[cid].getSource(), _clients[cid].getNickname());
    // reply = ERR_NOSUCHNICK(SOURCE(_clients[cid].getNickname(), _clients[cid].getUsername()), _clients[cid].getNickname());
    std::cout << "print reply: " << reply << std::endl; // to del
    send(_clients[cid].getFd(), reply.c_str(), reply.length(), 0);
  }
  // else if there are invalid char in the nickname, an erroneus nickname message is sent
  else if (isValidNick(param) == false)
  {
    reply = ERR_ERRONEUSNICKNAME(_clients[cid].getSource(), _clients[cid].getNickname(), param);
    std::cout << "print reply: " << reply << std::endl; // to del
    send(_clients[cid].getFd(), reply.c_str(), reply.length(), 0);
  }
  // else if the nickname is the same nickname as another client
  else if (existingNick(param) == true)
  {
    // and if our client is not yet registered then an nick collision occurs and the client is killed
    if (_clients[cid].getIfRegistered() == false)
    {
      reply = ERR_NICKCOLLISION(_clients[cid].getSource(), _clients[cid].getNickname(), param);
      std::cout << "print reply: " << reply << std::endl; // to del
      send(_clients[cid].getFd(), reply.c_str(), reply.length(), 0);
      reply = KILL_MSG(_clients[cid].getSource(), _clients[cid].getNickname());
      std::cout << "print reply: " << reply << std::endl; // to del
      send(_clients[cid].getFd(), reply.c_str(), reply.length(), 0);
      this->delConnection( cid );
    }
    // but if our client is already registered then a nickname in use error occurs
    // the client keeps its nickname
    else
    {
      reply = ERR_NICKNAMEINUSE(_clients[cid].getSource(), _clients[cid].getNickname(), param);
      send(_clients[cid].getFd(), reply.c_str(), reply.length(), 0);
    }
  }
  // else meaning if the nickname given is valid and does not already exist
  // nickname is set to param value and nickstatus set to true
  else
  {
	  reply = RPL_NICK(_clients[cid].getNickname(), param);
    std::cout << "print reply: " << reply << std::endl; // to del
    send(_clients[cid].getFd(), reply.c_str(), reply.length(), 0);
    _clients[cid].setNickStatus(true);
    _clients[cid].setNickname(param);
    if (_clients[cid].getIfRegistered() == true)
      _clients[cid].setSource( param, _clients[cid].getUsername());
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

bool  Server::existingNick(std::string param)
{
  for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
  {
    if (it->getNickname().compare(param) == 0 && it->getNickname().size() == param.size())
      return true;
  }
  return false;
}


/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmelard <lmelard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/03 15:16:17 by lmelard           #+#    #+#             */
/*   Updated: 2023/07/10 15:37:47 by lmelard          ###   ########.fr       */
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
  // std::string reply;
  // if no param to a nick command -> No nickname given error
  // OPTION WE TRUNC PARAM IF ITS TOO LONG
  if (param.size() > NICKLEN)
    param.erase(NICKLEN, param.size() - NICKLEN);
  if (param.compare("") == 0)
  {
    replyMsg(cid, ERR_NONICKNAMEGIVEN(_clients[cid].getSource(), _clients[cid].getNickname()));
  }
  // else if there are invalid char in the nickname, an erroneus nickname message is sent
  else if (isValidNick(param) == false)
  {
    replyMsg(cid, ERR_ERRONEUSNICKNAME(_clients[cid].getSource(), _clients[cid].getNickname(), param));
  }
  // else if the nickname is the same nickname as another client
  else if (existingNick(param) == true)
  {
    // and if our client is not yet registered then an nick collision occurs and the client is killed
    if (_clients[cid].getIfRegistered() == false)
    {
      replyMsg(cid, ERR_NICKCOLLISION(_clients[cid].getSource(), _clients[cid].getNickname(), param));
      replyMsg(cid, KILL_MSG(_clients[cid].getSource(), _clients[cid].getNickname()));
      this->delConnection( cid );
    }
    // but if our client is already registered then a nickname in use error occurs
    // the client keeps its nickname
    else
    {
      replyMsg(cid, ERR_NICKNAMEINUSE(_clients[cid].getSource(), _clients[cid].getNickname(), param));
    }
  }
  // else meaning if the nickname given is valid and does not already exist
  // nickname is set to param value and nickstatus set to true
  else
  {
    replyMsg(cid, RPL_NICK(_clients[cid].getNickname(), param));
    _clients[cid].setNickStatus(true);
    _clients[cid].setNickname(param);
    std::cout << "info:\t valid nickname provided!\n" << std::endl;
    checkRegistration(cid);
    // if (_clients[cid].getIfRegistered() == true)
    //   _clients[cid].setSource( param, _clients[cid].getUsername());
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


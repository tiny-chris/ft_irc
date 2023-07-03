/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmelard <lmelard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/30 17:36:40 by cgaillag          #+#    #+#             */
/*   Updated: 2023/07/03 15:04:30 by lmelard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"

/**
 * @brief       USER command used at the beginning of a connection to specify the username and realname of a new user
 *
 *              spécification IRC (RFC 2812)
 *              http://abcdrfc.free.fr/rfc-vf/rfc2812.html
 *              https://modern.ircdocs.horse/#connection-setup
 *
 */

static size_t  findCharTimes(std::string str, char c)
{
  size_t nbChar = 0;

  for (size_t i = 0; i < str.length(); ++i)
  {
    if (str[i] == c)
      nbChar++;
  }
  return (nbChar);
}

static bool		isValidUser(std::string name)
{
  for (size_t i = 0; i < name.size(); i++)
  {
    char c = name[i];
    if (c == '\0' || c == '\r' || c == '\n' || c == ' ')
      return false;
  }
  return true;
}


void		Server::handleUser( size_t cid, std::string param )
{
  std::string	reply;
  std::string realname;
  size_t      colon;

  std::cout << "client " << _clients[cid].getNickname() << " - param full content is <" << param << ">" << std::endl;

  // *** **************************** ***
  // *** CHECK 1 - ALREADY REGISTERED ***
  // *** **************************** ***
  // check if Client is already registered (i.e. PASS, NICK, USER are already set)
	// if so, cannot use the USER command again
  // and send ERR_ALREADYREGISTERED numeric reply
  if (_clients[cid].getIfRegistered() == true)
  {
    reply = ERR_ALREADYREGISTRED(_serverName, _clients[cid].getNickname());
		std::cout << "print reply: " << reply << std::endl; // to del
		send(_clients[cid].getCfd(), reply.c_str(), reply.length(), 0);
    return ;
  }

  // *** **************************** ***
  // *** CHECK 2 - PASS & NICK MUST BE SET UPSTREAM ***
  // *** **************************** ***
  // check if PASS and NICK are set... otherwise, return (no numeric_reply)
  if (!_clients[cid].getPassStatus() || !_clients[cid].getNickStatus())
  {
    std::cout << "client " << _clients[cid].getNickname() << " - wants to use USER command: error as PASS or NICK not set yet" << reply << std::endl;
    return ;
  }

  // *** **************************** ***
  // *** CHECK 3 - PARAM CONTENT MUST NOT BE EMPTY ***
  // *** **************************** ***
  // check if param is not empty (min param = 1)
  // if so, cannot use the USER command and send ERR_NEEDMOREPARAMS numeric reply
  if (param.empty())
  {
    reply = ERR_NEEDMOREPARAMS(_serverName, _clients[cid].getNickname(), "USER");
		// reply = ERR_NEEDMOREPARAMS(_serverName, _clients[cid].getNickname(), command);
		std::cout << "print reply: " << reply << std::endl; // to del
		send(_clients[cid].getCfd(), reply.c_str(), reply.length(), 0);
    return ;
  }

  // *** **************************** ***
  // *** CHECK 4 - LAST PARAMETER 'realname' ***
  // *** **************************** ***
  // check if ':' exists to split the last parameter 'realname' (after ':') from the others
  // if ':' does not exist or if realname is empty --> send a ERR_NEEDMOREPARAMS reply
  // otherwise --> fill the '_realname' with content after ':' (space is allowed inside)
  colon = param.find(':', 0);
  std::cout << "client " << _clients[cid].getNickname() << " - colon ':' on index <" << colon << ">" << std::endl;

  if (colon <= param.length())
  {
    realname = param.substr(colon + 1, param.length() - (colon + 1));
    std::cout << "client " << _clients[cid].getNickname() << " - last param content 'realname' is <" << realname << ">" << std::endl;
  }
  if (colon == std::string::npos || realname.empty())
  {
    reply = ERR_NEEDMOREPARAMS(_serverName, _clients[cid].getNickname(), "USER");
    // reply = ERR_NEEDMOREPARAMS(_serverName, _clients[cid].getNickname(), command);
    std::cout << "print reply: " << reply << std::endl; // to del
    send(_clients[cid].getCfd(), reply.c_str(), reply.length(), 0);
    return ;
  }
  _clients[cid].setRealname(realname);
  std::cout << "client " << _clients[cid].getNickname() << " - param content before ':' is <" << param.substr(0, colon) << ">" << std::endl;

  // *** **************************** ***
  // *** CHECK 5 - PARAMETERS before ':' ***
  // *** **************************** ***

  // STEP 5.1 : check number of spaces
  // if there are not 3 spaces before ':' --> incorrect number of parameters (ERR_NEEDMOREPARAMS or just return)
  size_t nbSpaces = findCharTimes(param.substr(0, colon), ' ');
  std::cout << "client " << _clients[cid].getNickname() << " - number of spaces before ':' is " << nbSpaces << std::endl;
  if ( nbSpaces != 3)
  {
    if (nbSpaces < 3)
    {
      reply = ERR_NEEDMOREPARAMS(_serverName, _clients[cid].getNickname(), "USER");
      // reply = ERR_NEEDMOREPARAMS(_serverName, _clients[cid].getNickname(), command);
      std::cout << "print reply: " << reply << std::endl; // to del
      send(_clients[cid].getCfd(), reply.c_str(), reply.length(), 0);
    }
    return ;
  }

  // STEP 5.2 : split param until ':' with ' ' delimiter and send each substring into a std::vector<std::string>
  std::vector<std::string>  tokens;
  std::istringstream        iss(param.substr(0, colon));
  std::string               token;

  while (std::getline(iss, token, ' '))
  {
    tokens.push_back(token);
  }
  std::cout << "client " << _clients[cid].getNickname() << " - number of params before ':' is " << tokens.size() << std::endl;
  for (size_t i = 0; i < tokens.size(); ++i)
  {
    std::cout << "client " << _clients[cid].getNickname() << " - param[" << i << "] is <" << tokens[i] << ">" << std::endl;
  }

  // STEP 5.3 : check number of substrings and check each substring
  // if there are less than 3 substrings or if the 1st substring has less than 1 char (cannot be empty)
  // --> ERR_NEEDMOREPARAMS
  // if the 1st substring contains invalid char or if there are more than 3 substrings
  // or if 2nd substring is different from default "0" or the 3rd substring is different from default "*"
  // --> return
  // otherwise --> fill the '_username' with content of 1st substring
  if (tokens.size() < 3 || tokens[0].length() < 1)
  {
    reply = ERR_NEEDMOREPARAMS(_serverName, _clients[cid].getNickname(), "USER");
    // reply = ERR_NEEDMOREPARAMS(_serverName, _clients[cid].getNickname(), command);
    std::cout << "print reply: " << reply << std::endl; // to del
    send(_clients[cid].getCfd(), reply.c_str(), reply.length(), 0);
    return ;
  }
  else if (isValidUser(tokens[0]) == false || (tokens.size() >= 3 && (tokens[1].compare("0") != 0 || tokens[2].compare("*") != 0)))
  {
    std::cout << "client " << _clients[cid].getNickname() << " - wrong param" << std::endl;
    return ;
  }
  _clients[cid].setUsername(tokens[0]);
  std::cout << "client " << _clients[cid].getNickname() << " - username is : <" << _clients[cid].getUsername() << ">" << std::endl;

  // *** **************************** ***
  // *** CHECK 6 - CLIENT IS NOW REGISTERED ***
  // *** **************************** ***
  if (_clients[cid].getPassStatus() == true && _clients[cid].getNickStatus() == true && !_clients[cid].getUsername().empty())
  {
    _clients[cid].setIfRegistered(true);
    _clients[cid].setSource(_clients[cid].getNickname(), _clients[cid].getUsername());
    std::cout << "client " << _clients[cid].getNickname() << " --> is registered" << std::endl;
  }
}

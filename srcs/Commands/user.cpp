/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgaillag <cgaillag@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/30 17:36:40 by cgaillag          #+#    #+#             */
/*   Updated: 2023/07/06 15:30:05 by cgaillag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"
#include "utils.hpp"

/**
 * @brief       USER command used at the beginning of a connection to specify the username and realname of a new user
 *
 *              spécification IRC (RFC 2812)
 *              http://abcdrfc.free.fr/rfc-vf/rfc2812.html
 *              https://modern.ircdocs.horse/#connection-setup
 *
 */

// static size_t  findCharTimes(std::string str, char c)
// {
//   size_t nbChar = 0;

//   for (size_t i = 0; i < str.length(); ++i)
//   {
//     if (str[i] == c)
//       nbChar++;
//   }
//   return (nbChar);
// }

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
  std::string	              reply;
  std::string               realname = "";
  size_t                    colon;
  std::vector<std::string>  tokens;

  // *** **************************** ***
  // *** CHECK 1 - ALREADY REGISTERED ***
  // *** **************************** ***
  // check if Client is already registered (i.e. PASS, NICK, USER are already set)
	// if so, cannot use the USER command again --> send ERR_ALREADYREGISTERED numeric reply
  if (_clients[cid].getIfRegistered() == true)
  {
    reply = ERR_ALREADYREGISTRED(_serverName, _clients[cid].getNickname());
		std::cout << "print reply: " << reply << std::endl; // to del
		send(_clients[cid].getFd(), reply.c_str(), reply.length(), 0);
    return ;
  }

  // *** **************************** ***
  // *** CHECK 2 - PASS & NICK MUST BE SET UPSTREAM ***
  // *** **************************** ***
  // check if PASS and NICK are set... otherwise, return (no numeric_reply)
  if (!_clients[cid].getPassStatus() || !_clients[cid].getNickStatus())
  {
    std::cout << "-----client cid<" << cid << "> " << _clients[cid].getNickname() << "-----\n" ;
    std::cout << "Error: PASS or NICK are not set yet" << std::endl;
    return ;
  }

  // *** **************************** ***
  // *** CHECK 3 - LAST PARAMETER 'realname' + PARAM CONTENT MUST NOT BE EMPTY ***
  // *** **************************** ***
  // check if ':' exists to split the last parameter 'realname' (after ':') from the others
  // if ':' does not exist or if realname is empty --> send a ERR_NEEDMOREPARAMS reply
  // otherwise --> fill the '_realname' with content after ':' (space is allowed inside)
  colon = param.find(':', 0);
  if (param.empty() || colon == std::string::npos || colon == 0 || colon == param.length() - 1)
  {
    reply = ERR_NEEDMOREPARAMS(_serverName, _clients[cid].getNickname(), "USER");
    std::cout << "print reply: " << reply << std::endl;
    send(_clients[cid].getFd(), reply.c_str(), reply.length(), 0);
    return ;
  }
  realname = param.substr(colon + 1, param.length() - (colon + 1));
  _clients[cid].setRealname(realname);

  // *** **************************** ***
  // *** CHECK 4 - PARAMETERS before ':' ***
  // *** **************************** ***
  // split param until ':' with ' ' delimiter and send each substring into a std::vector<std::string>
  // check number of substrings (must be 3) and check each substring
  // -if there are less than 3 substrings or if the 1st substring has less than 1 char (cannot be empty) --> ERR_NEEDMOREPARAMS
  // - if the 1st substring contains invalid char or if there are more than 3 substrings or if 2nd/3rd are not valid --> return
  // otherwise --> fill the '_username' with content of 1st substring
  /*
  The second and third parameters of this command SHOULD be sent as one zero ('0', 0x30) and one asterisk character ('*', 0x2A) 
  by the client, as the meaning of these two parameters varies between different versions of the IRC protocol.
  */
  tokens = splitString(param.substr(0, colon), ' ');
  if (tokens.size() < 3 || tokens[0].length() < 1)
  {
    reply = ERR_NEEDMOREPARAMS(_serverName, _clients[cid].getNickname(), "USER");
    std::cout << "print reply: " << reply << std::endl;
    send(_clients[cid].getFd(), reply.c_str(), reply.length(), 0);
    return ;
  }
  else if (tokens.size() > 3 || isValidUser(tokens[0]) == false
      || tokens[1].empty() || tokens[2].empty() || (colon > 0 && param[colon - 1] != ' '))
  {
    std::cout << "-----client cid<" << cid << "> " << _clients[cid].getNickname() << "-----\n" ;
    std::cout << "Error: wrong parameters with USER command" << std::endl;
    return ;
  }
  _clients[cid].setUsername(tokens[0].substr(0, USERLEN));

  // *** **************************** ***
  // *** CHECK x - PARAM CONTENT MUST NOT BE EMPTY ***
  // *** **************************** ***
  // check if param is not empty (min param = 1)
  // if so, cannot use the USER command and send ERR_NEEDMOREPARAMS numeric reply
  // if (param.empty())
  // {
  //   reply = ERR_NEEDMOREPARAMS(_serverName, _clients[cid].getNickname(), "USER");
	// 	// reply = ERR_NEEDMOREPARAMS(_serverName, _clients[cid].getNickname(), command);
	// 	std::cout << "print reply: " << reply << std::endl; // to del
	// 	send(_clients[cid].getFd(), reply.c_str(), reply.length(), 0);
  //   return ;
  // }

  // // *** **************************** ***
  // // *** CHECK z - LAST PARAMETER 'realname' + PARAM CONTENT MUST NOT BE EMPTY ***
  // // *** **************************** ***
  // // check if ':' exists to split the last parameter 'realname' (after ':') from the others
  // // if ':' does not exist or if realname is empty --> send a ERR_NEEDMOREPARAMS reply
  // // otherwise --> fill the '_realname' with content after ':' (space is allowed inside)
  // colon = param.find(':', 0);
  // std::cout << "client " << _clients[cid].getNickname() << " - colon ':' on index <" << colon << ">" << std::endl;
  // std::cout << "client " << _clients[cid].getNickname() << " - param length is <" << param.length() << ">" << std::endl;
  // if (param.empty() || colon == std::string::npos || colon == param.length() - 1)
  // {
  //   reply = ERR_NEEDMOREPARAMS(_serverName, _clients[cid].getNickname(), "USER");
  //   // reply = ERR_NEEDMOREPARAMS(_serverName, _clients[cid].getNickname(), command);
  //   std::cout << "print reply: " << reply << std::endl; // to del
  //   send(_clients[cid].getFd(), reply.c_str(), reply.length(), 0);
  //   return ;
  // }
  // realname = param.substr(colon, param.length() - colon);
  // std::cout << "client " << _clients[cid].getNickname() << " - last param content 'realname' with ':' is <" << realname << ">" << std::endl;
  // realname.erase(0, 1);
  // std::cout << "client " << _clients[cid].getNickname() << " - last param content 'realname' is <" << realname << ">" << std::endl;
  // _clients[cid].setRealname(realname);
  // std::cout << "client " << _clients[cid].getNickname() << " - param content before ':' is <" << param.substr(0, colon) << ">" << std::endl;

  // *** **************************** ***
  // *** CHECK y - PARAMETERS before ':' ***
  // *** **************************** ***

  // STEP 5.1 : check number of spaces
  // if there are not 3 spaces before ':' --> incorrect number of parameters (ERR_NEEDMOREPARAMS or just return)
  // size_t nbSpaces = findCharTimes(param.substr(0, colon), ' ');
  // std::cout << "client " << _clients[cid].getNickname() << " - number of spaces before ':' is " << nbSpaces << std::endl;
  // if ( nbSpaces != 3)
  // {
  //   if (nbSpaces < 3)
  //   {
  //     reply = ERR_NEEDMOREPARAMS(_serverName, _clients[cid].getNickname(), "USER");
  //     // reply = ERR_NEEDMOREPARAMS(_serverName, _clients[cid].getNickname(), command);
  //     std::cout << "print reply: " << reply << std::endl; // to del
  //     send(_clients[cid].getFd(), reply.c_str(), reply.length(), 0);
  //   }
  //   return ;
  // }

  // STEP 5.2 : split param until ':' with ' ' delimiter and send each substring into a std::vector<std::string>
  // check number of substrings (must be 3) and check each substring
  // if there are less than 3 substrings or if the 1st substring has less than 1 char (cannot be empty)
  // --> ERR_NEEDMOREPARAMS
  // if the 1st substring contains invalid char or if there are more than 3 substrings
  // or if 2nd substring is different from default "0" or the 3rd substring is different from default "*"
  // --> return
  // otherwise --> fill the '_username' with content of 1st substring

  /*
  The second and third parameters of this command SHOULD be sent as one zero ('0', 0x30) and one asterisk character ('*', 0x2A) 
  by the client, as the meaning of these two parameters varies between different versions of the IRC protocol.
  */
  // std::vector<std::string>  tokens;
  // std::istringstream        iss(param.substr(0, colon));
  // std::string               token;

  // while (std::getline(iss, token, ' '))
  // {
  //   tokens.push_back(token);
  // }
  // std::cout << "client " << _clients[cid].getNickname() << " - number of params before ':' is " << tokens.size() << std::endl;
  // for (size_t i = 0; i < tokens.size(); ++i)
  // {
  //   std::cout << "client " << _clients[cid].getNickname() << " - param[" << i << "] is <" << tokens[i] << ">" << std::endl;
  // }
  // if (tokens.size() < 3 || tokens[0].length() < 1)
  // {
  //   reply = ERR_NEEDMOREPARAMS(_serverName, _clients[cid].getNickname(), "USER");
  //   // reply = ERR_NEEDMOREPARAMS(_serverName, _clients[cid].getNickname(), command);
  //   std::cout << "print reply: " << reply << std::endl; // to del
  //   send(_clients[cid].getFd(), reply.c_str(), reply.length(), 0);
  //   return ;
  // }
  // else if (tokens.size() > 3 || isValidUser(tokens[0]) == false)
  // {
  //   std::cout << "client " << _clients[cid].getNickname() << " - wrong param" << std::endl;
  //   return ;
  // }
  // _clients[cid].setUsername(tokens[0].substr(0, USERLEN));
  // std::cout << "client " << _clients[cid].getNickname() << " - username is <" << _clients[cid].getUsername() << ">" << std::endl;

  // *** **************************** ***
  // *** CHECK 5 - CLIENT IS NOW REGISTERED ***
  // *** **************************** ***
  if (_clients[cid].getPassStatus() == true && _clients[cid].getNickStatus() == true && !_clients[cid].getUsername().empty())
  {
    _clients[cid].setIfRegistered(true);
    _clients[cid].setSource(_clients[cid].getNickname(), _clients[cid].getUsername());
    std::cout << "-----client cid<" << cid << "> " << _clients[cid].getNickname() << "-----\n" ;
    std::cout << "Client is now registered!" << std::endl;
    // DISPLAY WELCOME MESSAGES
    sendWelcomeMsg( cid );
    // EQUIVALENT OF LUSERS received 
    sendLusersMsg( cid );
  }
}

void		Server::sendWelcomeMsg( size_t cid ) const
{
  std::string reply;
  std::time_t	t = std::time(0);
	std::tm* local = std::localtime(&t);
	
  char formattedDate[30];
  std::strftime(formattedDate, sizeof(formattedDate), "%a %b %d %H:%M:%S %Y", local);
  std::string date(formattedDate);
  
  reply = RPL_WELCOME(_clients[cid].getSource(), _clients[cid].getNickname());
  send(_clients[cid].getFd(), reply.c_str(), reply.length(), 0);
  reply = RPL_YOURHOST(_clients[cid].getSource(), _clients[cid].getNickname());
  send(_clients[cid].getFd(), reply.c_str(), reply.length(), 0);
  reply = RPL_CREATED(_clients[cid].getSource(), _clients[cid].getNickname(), date);
  send(_clients[cid].getFd(), reply.c_str(), reply.length(), 0);
  reply = RPL_MYINFO(_clients[cid].getSource(), _clients[cid].getNickname());
  send(_clients[cid].getFd(), reply.c_str(), reply.length(), 0);
  reply = RPL_ISUPPORT(_clients[cid].getSource(), _clients[cid].getNickname(), getSupportToken());
  send(_clients[cid].getFd(), reply.c_str(), reply.length(), 0);
}

std::string Server::getSupportToken() const
{  
  std::stringstream token;
  token << " CHANLIMIT=#:" << CHANLIMIT << " ";
  token << " CHANMODE=" << CHANMODES << " ";
  token << " CHANNELLEN=" << CHANNELLEN << " ";
  token << " CHANTYPES=" << CHANTYPES << " ";
  token << " HOSTLEN=" << HOSTLEN << " ";
  token << " KICKLEN=" << KICKLEN << " ";
  token << " NICKLEN=" << NICKLEN << " ";
  token << " PREFIX=" << PREFIX << " ";
  token << " STATUSMSG=" << STATUSMSG << " ";
  token << " TOPICLEN=" << TOPICLEN << " ";
  return (token.str());
} 

void  Server::sendLusersMsg( size_t cid ) const
{
  std::string reply;
  std::stringstream nbrClients;

  nbrClients << _clients.size();

  reply = RPL_LUSERCLIENT(_clients[cid].getSource(), _clients[cid].getNickname(), nbrClients.str());
  send(_clients[cid].getFd(), reply.c_str(), reply.length(), 0);
  reply = RPL_LUSEROP(_clients[cid].getSource(), _clients[cid].getNickname(), "0"); // A MODIFIER getOpsNbr()
  send(_clients[cid].getFd(), reply.c_str(), reply.length(), 0);
  reply = RPL_LUSERUNKNOWN(_clients[cid].getSource(), _clients[cid].getNickname(), "0"); // A Modifier getUnknownStateUsers()
  send(_clients[cid].getFd(), reply.c_str(), reply.length(), 0);
  reply = RPL_LUSERCHANNELS(_clients[cid].getSource(), _clients[cid].getNickname(), "0"); // A MODIFIER getChannelNbr();
  send(_clients[cid].getFd(), reply.c_str(), reply.length(), 0);
  reply = RPL_LUSERCLIENT(_clients[cid].getSource(), _clients[cid].getNickname(), nbrClients.str());
  send(_clients[cid].getFd(), reply.c_str(), reply.length(), 0);
}
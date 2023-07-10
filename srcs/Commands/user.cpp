/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmelard <lmelard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/30 17:36:40 by cgaillag          #+#    #+#             */
/*   Updated: 2023/07/10 15:43:50 by lmelard          ###   ########.fr       */
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

static bool	isValidUser(std::string name)
{
  for (size_t i = 0; i < name.size(); i++)
  {
    char c = name[i];
    if (c == '\0' || c == '\r' || c == '\n' || c == ' ')
      return false;
  }
  return true;
}

// A REVOIR !!!!!
static bool	isValidParam(std::string name)
{
  if (name.empty())
    return false;
  for (size_t i = 0; i < name.size(); i++)
  {
    char c = name[i];
    if (c == '\0' || c == '\r' || c == '\n' || c == ' ')
      return false;
  }
  return true;
}

void				Server::handleUser( size_t cid, std::string param )
{
  std::string               realname = "";
  size_t                    colon;
  std::vector<std::string>  tokens;

  /* **************************** ***
  ** CHECK 1 - ALREADY REGISTERED ***
  ** **************************** ***
  ** check if Client is already registered (i.e. PASS, NICK, USER are already set)
	** if so, cannot use the USER command again --> send ERR_ALREADYREGISTERED numeric reply
  */
  if (_clients[cid].getIfRegistered() == true)
  {
    replyMsg(cid, ERR_ALREADYREGISTRED(_serverName, _clients[cid].getNickname()));
    return ;
  }

  /* *************************************************** ***
  ** CHECK 2 - PASS & NICK COMMANDS MUST BE SET UPSTREAM ***
  ** *************************************************** ***
  ** check if PASS and NICK are set... otherwise, return (no numeric_reply)
  */
  if (!_clients[cid].getPassStatus() || !_clients[cid].getNickStatus())
  {
    std::cout << "error:\t PASS or NICK are not set yet" << std::endl;
    return ;
  }

  /* *********************************************************************** ***
  ** CHECK 3 - PARAM CONTENT AND LAST PARAMETER (realname) MUST NOT BE EMPTY ***
  ** *********************************************************************** ***
  ** check if ':' exists to split the last parameter 'realname' (after ':') from the others
  ** if ':' does not exist or if realname is empty --> send a ERR_NEEDMOREPARAMS reply
  ** otherwise --> fill the '_realname' with content after ':' (space is allowed inside)
  */
  colon = param.find(':', 0);
  if (param.empty() || colon == std::string::npos || colon == 0 || colon == param.length() - 1)
  {
    replyMsg(cid, ERR_NEEDMOREPARAMS(_serverName, _clients[cid].getNickname(), "USER"));
    return ;
  }
  realname = param.substr(colon + 1, param.length() - (colon + 1));
  _clients[cid].setRealname(realname);

  /* *********************************************** ***
  ** CHECK 4 - MUST BE 3 VALID PARAMETERS before ':' ***
  ** *********************************************** ***
  ** split 'param' until ':' with ' ' delimiter and send each substring into a std::vector<std::string>
  ** check number of substrings (must be 3) and check each substring
  ** - if there is not exactly 3 substrings or substring are incorrect --> ERR_NEEDMOREPARAMS or just return
  ** otherwise --> fill the '_username' with content of 1st substring
  ** NB: 2nd and 3rd parameters SHOULD be sent as one zero ('0', 0x30) and one asterisk character ('*', 0x2A)
  ** by the client, as the meaning of these two parameters varies between different versions of the IRC protocol.
  */
  tokens = splitString(param.substr(0, colon), ' ');
  if (tokens.size() < 3 || tokens[0].length() < 1)
  {
    replyMsg(cid, ERR_NEEDMOREPARAMS(_serverName, _clients[cid].getNickname(), "USER"));
    return ;
  }
  else if (tokens.size() > 3 || isValidUser(tokens[0]) == false
      || !isValidParam(tokens[1]) || !isValidParam(tokens[2]) || (colon > 0 && param[colon - 1] != ' '))
  {
    std::cout << "error:\t wrong parameters with USER command\n" << std::endl;
    return ;
  }
  _clients[cid].setUsername(tokens[0].substr(0, USERLEN));

  /* ********************************** ***
  ** CHECK 5 - CLIENT IS NOW REGISTERED ***
  ** ********************************** ***
  */
  if (_clients[cid].getPassStatus() == true && _clients[cid].getNickStatus() == true && !_clients[cid].getUsername().empty())
  {
    _clients[cid].setIfRegistered(true);
    _clients[cid].setSource(_clients[cid].getNickname(), _clients[cid].getUsername());
    std::cout << "info:\t " << _clients[cid].getNickname() << " is now registered!\n" << std::endl;
    // DISPLAY WELCOME MESSAGES
    sendWelcomeMsg( cid );
    // EQUIVALENT OF LUSERS received
    sendLusersMsg( cid );
  }
}

void		Server::sendWelcomeMsg( size_t cid ) const
{
  // std::string reply;
  std::time_t	t = std::time(0);
	std::tm* local = std::localtime(&t);

  char formattedDate[30];
  std::strftime(formattedDate, sizeof(formattedDate), "%a %b %d %H:%M:%S %Y", local);
  std::string date(formattedDate);
  
  replyMsg( cid, RPL_WELCOME(_clients[cid].getSource(), _clients[cid].getNickname()), 0);
  replyMsg( cid, RPL_YOURHOST(_clients[cid].getSource(), _clients[cid].getNickname()), 0);
  replyMsg( cid, RPL_CREATED(_clients[cid].getSource(), _clients[cid].getNickname(), date), 0);
  replyMsg( cid, RPL_MYINFO(_clients[cid].getSource(), _clients[cid].getNickname()), 0);
  replyMsg( cid, RPL_ISUPPORT(_clients[cid].getSource(), _clients[cid].getNickname(), getSupportToken()), 0);

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
  // std::string reply;
  std::stringstream nbrClients;

  nbrClients << _clients.size();

  replyMsg(cid, RPL_LUSERCLIENT(_clients[cid].getSource(), _clients[cid].getNickname(), nbrClients.str()), 0);
  replyMsg(cid, RPL_LUSEROP(_clients[cid].getSource(), _clients[cid].getNickname(), "0"), 0); // A MODIFIER getOpsNbr()
  replyMsg(cid, RPL_LUSERUNKNOWN(_clients[cid].getSource(), _clients[cid].getNickname(), "0"), 0); // A Modifier getUnknownStateUsers()
  replyMsg(cid, RPL_LUSERCHANNELS(_clients[cid].getSource(), _clients[cid].getNickname(), "0"), 0); // A MODIFIER getChannelNbr();
  replyMsg(cid, RPL_LUSERCLIENT(_clients[cid].getSource(), _clients[cid].getNickname(), nbrClients.str()), 0);
}

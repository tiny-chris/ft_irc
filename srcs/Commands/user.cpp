/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgaillag <cgaillag@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/30 17:36:40 by cgaillag          #+#    #+#             */
/*   Updated: 2023/06/30 18:31:06 by cgaillag         ###   ########.fr       */
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
 *              1. PASS must be correct --> already a check in handleRequest
 *              2. NICK must be correct (not empty and valid - no duplicate)
 *              3. respect specific format (params):
 *                  old = USER <username> <mode> <realname> :<description>  --> version désuète (RFC 1459)
 *                  new = USER <username> <mode> <not_used> :<realname>     --> as of RFC 2812 spec for IRC
 *
 *                  with
 *                  - <username>  = un identifiant unique ou une chaîne arbitraire (pas d'espace: mais '-' ou '_')
 *                                  --> on peut imposer des restrictions sur le contenu du nom (que a-z A-Z et '-' '_')
 *                  - <mode>      = mode de connexion souhaité avec '0' comme valeur par défaut recommandée (absence de mode spécifique)
 *                                  /!\ La spécification RFC 2812 ne définit pas de modes spécifiques pour le champ <mode> de la commande "USER".
 *                                  Les modes d'utilisateur spécifiques sont généralement définis dans le contexte des canaux (channels) dans IRC,
 *                                  plutôt que dans la commande "USER". Ainsi, pour la commande "USER", il est courant d'utiliser la valeur "0"
 *                                  dans le champ <mode> pour indiquer qu'aucun mode spécifique n'est appliqué à l'utilisateur.
 *                                --> on ne va garder que ce mode par défaut
 *                  - <not_used>  = présent pour raison historique
 *                  - <realname>  = nom réel de l'utilisateur (ou tout autre nom qu'il souhaite utiliser)
 *                                   /!\ il peut y avoir des espaces si on le souhaite et dans ce cas, on utilise des quotes (' ou ")
 *                                   --> A NOUS DE VOIR CE QUE NOUS VOULONS
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
  
  std::cout << "client " << _clients[cid].getNickname() << " - param = <" << param << ">" << std::endl;
 
  // *** CHECK 1 *** 
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

  // *** CHECK 2 ***
  // check if PASS and NICK are set... otherwise, return --> create a NUMERIC REPLY ??
  if (!_clients[cid].getPassStatus() || !_clients[cid].getNickStatus())
  {
    std::cout << "client " << _clients[cid].getNickname() << " - wants to use USER command: error as PASS or NICK not set yet" << reply << std::endl;
    return ;
  }

  // *** CHECK 3 *** 
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

  // *** CHECK 4 ***
  // check each param:
  //  - find the first ':' (after this it is only the 'realname', space is allowed inside)
  //  - split param until ':' and send each substring into a std::vector<std::string>
  //  - there should be 3 subtrings --> otherwise ERR_NEEDMOREPARAMS
  //  - check on vec[0]: min 1 char, cannot be empty --> ERR_NEEDMOREPARAMS
  //  - check on vec[1]: should be 0
  //  - check on vec[2]: should be *
  //  - then consider substring of param from ':' until the end
  //      --> check content

  // get the first colon (:) to split the last parameter 'realname' (after ':') from the others
  // *** CHECK LAST PARAMETER 'realname' ***
  // check if empty
  //  if so --> send a ERR_NEEDMOREPARAMS reply
  //  if not --> fill the '_realname' with content after ':'
  size_t      colon = param.find(':', 0);
  std::string realname;
  std::cout << "client " << _clients[cid].getNickname() << " - colon ':' on index <" << colon << ">" << std::endl;
  if (colon <= param.length())
  {
    realname = param.substr(colon + 1, param.length() - (colon + 1));
    std::cout << "client " << _clients[cid].getNickname() << " - realname is <" << realname << ">" << std::endl;
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
  std::cout << "client " << _clients[cid].getNickname() << " - param content before ':' <" << param.substr(0, colon) << ">" << std::endl;

  // *** CHECK PARAMETERS before ':' ***
  // check number of spaces
  size_t nbSpaces = findCharTimes(param.substr(0, colon), ' ');
  std::cout << "client " << _clients[cid].getNickname() << " - number of spaces before ':' " << nbSpaces << std::endl;
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

  // get a vector of strings
  std::vector<std::string>  tokens;
  std::istringstream        iss(param.substr(0, colon));
  std::string               token;

  while (std::getline(iss, token, ' '))
  {
    tokens.push_back(token);
  }
  std::cout << "client " << _clients[cid].getNickname() << " - number of params before ':' " << tokens.size() << std::endl;

  for (size_t i = 0; i < tokens.size(); ++i)
  {
    std::cout << "client " << _clients[cid].getNickname() << " - 1st param <" << tokens[i] << ">" << std::endl; 
  }
  // check each string
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
  std::cout << "client " << _clients[cid].getNickname() << " - username is : <" << _clients[cid].getRealname() << ">" << std::endl;

  if (_clients[cid].getPassStatus() == true && !_clients[cid].getNickname().empty() && !_clients[cid].getUsername().empty())
  {
    _clients[cid].setIfRegistered(true);
    std::cout << "client " << _clients[cid].getNickname() << " --> is registered" << std::endl;
  }
}

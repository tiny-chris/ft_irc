/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   registration.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/30 17:36:40 by cgaillag          #+#    #+#             */
/*   Updated: 2023/07/15 14:12:34 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"
#include "utils.hpp"

void	Server::checkRegistration( size_t cid )
{
  if (_clients[cid].getPassStatus() == true && _clients[cid].getNickStatus() == true && !_clients[cid].getUsername().empty())
  {
    _clients[cid].setIfRegistered(true);
    _clients[cid].setSource(_clients[cid].getNickname(), _clients[cid].getUsername());
    std::cout << "info:\t " << _clients[cid].getNickname() << " is now registered!\n" << std::endl;
    // DISPLAY WELCOME MESSAGES
    sendWelcomeMsg( cid );
    // EQUIVALENT OF LUSERS received
    sendLusersMsg( cid );
    std::cout << "info:\t welcome message displayed\n" << std::endl;
    /************************************/
    // TO DEL JUSTE POUR TESTER MODE !!
    // OBLIGEE DE NEW UN CHANNEL POUR CA SINON LES MODIFS REALISEES SUR LE CHANNEL NE SONT QUE LOCALES
    // EX: J EPUSH BACK UN CLIENT A LA MAP DE CHANOPS DU CHANNEL DANS MON CONSTRUCTEUR MAIS QUAND JE VEUX Y ACCEDER DEPUIS 
    // UNE AUTRE FONCTION DU SERVEUR CA NE MARCHE PAS LA SIZE DE MA MAP EST A 0 .....
    // CHAT GPT DIT QUE Les objets std::map sont stockés en tant que paires de clé-valeur et lorsqu'une paire est insérée, 
    // une copie des objets est effectuée. Contrairement au std::vector
    if (cid == 1)
    {
      std::string name = "#chantest";
      _channels.insert(std::pair<std::string, Channel>(name, Channel(name)));
      _channels[name].addChannelOps(&_clients[cid]);
    }
    /************************************/
  }
}

void		Server::sendWelcomeMsg( size_t cid ) const
{
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
  std::stringstream nbrClients;

  nbrClients << _clients.size();

  replyMsg(cid, RPL_LUSERCLIENT(_clients[cid].getSource(), _clients[cid].getNickname(), nbrClients.str()), 0);
  replyMsg(cid, RPL_LUSEROP(_clients[cid].getSource(), _clients[cid].getNickname(), "0"), 0); // A MODIFIER getOpsNbr()
  replyMsg(cid, RPL_LUSERUNKNOWN(_clients[cid].getSource(), _clients[cid].getNickname(), "0"), 0); // A Modifier getUnknownStateUsers()
  replyMsg(cid, RPL_LUSERCHANNELS(_clients[cid].getSource(), _clients[cid].getNickname(), "0"), 0); // A MODIFIER getChannelNbr();
  replyMsg(cid, RPL_LUSERCLIENT(_clients[cid].getSource(), _clients[cid].getNickname(), nbrClients.str()), 0);
}

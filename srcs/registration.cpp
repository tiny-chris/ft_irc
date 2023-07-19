/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   registration.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmelard <lmelard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/30 17:36:40 by cgaillag          #+#    #+#             */
/*   Updated: 2023/07/19 14:58:27 by lmelard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"
#include "utils.hpp"

void	Server::checkRegistration( int clientSocket )
{
// /*  ************************************  */
// /*       POUR CHECKER LES SOCKETS !!      */
// /*  ************************************  */
// std::cout << "\n\t *** INFO INSIDE CHECKING REGISTRATION FUNCTION: ***\n";
// std::cout << "\t getPassStatus <"<< _clients.at( clientSocket ).getPassStatus() << ">\n";
// std::cout << "\t getNickStatus <"<< _clients.at( clientSocket ).getNickStatus() << ">\n";
// std::cout << "\t getUsername <"<< _clients.at( clientSocket ).getUsername() << ">\n";
// std::cout << std::endl;
// /*  ************************************  */
// /*  ************************************  */

  if ( _clients.at( clientSocket ).getPassStatus() == true && _clients.at(clientSocket ).getNickStatus() == true && _clients.at( clientSocket ).getUsername().empty() == false )
  {
    _clients.at( clientSocket ).setIfRegistered( true );
    _clients.at( clientSocket ).setSource( _clients.at( clientSocket ).getNickname(), _clients.at( clientSocket ).getUsername() );
    std::cout << MSGINFO << _clients.at( clientSocket ).getNickname();
    std::cout << " is now registered!" << std::endl;
    // DISPLAY WELCOME MESSAGES
    sendWelcomeMsg( clientSocket );
    // EQUIVALENT OF LUSERS received
    sendLusersMsg( clientSocket );
    std::cout << MSGINFO << "welcome message displayed" << std::endl;
    /************************************/
    // TO DEL JUSTE POUR TESTER MODE !!
    // OBLIGEE DE NEW UN CHANNEL POUR CA SINON LES MODIFS REALISEES SUR LE CHANNEL NE SONT QUE LOCALES
    // EX: J EPUSH BACK UN CLIENT A LA MAP DE CHANOPS DU CHANNEL DANS MON CONSTRUCTEUR MAIS QUAND JE VEUX Y ACCEDER DEPUIS
    // UNE AUTRE FONCTION DU SERVEUR CA NE MARCHE PAS LA SIZE DE MA MAP EST A 0 .....
    // CHAT GPT DIT QUE Les objets std::map sont stockés en tant que paires de clé-valeur et lorsqu'une paire est insérée,
    // une copie des objets est effectuée. Contrairement au std::vector
    std::string name = "#chantest";
    if ( clientSocket == 5 )
    {
      _channels.insert( std::pair<std::string, Channel>( name, Channel( name ) ) );
      _channels[ name ].addChannelOps(&_clients.at( clientSocket ) );
      _channels[ name ].addChannelMembers(&_clients.at( clientSocket ) );
    }
    if ( clientSocket == 6 )
    {
      _channels[ name ].addChannelMembers( &_clients.at( clientSocket ) );
    }
    /************************************/
  }
}

void		Server::sendWelcomeMsg( int clientSocket)
{
  std::time_t	t = std::time( 0 );
	std::tm* local = std::localtime( &t );

  char formattedDate[ 30 ];
  std::strftime( formattedDate, sizeof( formattedDate ), "%a %b %d %H:%M:%S %Y", local );
  std::string date( formattedDate );

  replyMsg( clientSocket, RPL_WELCOME( _clients.at( clientSocket ).getSource(), _clients.at( clientSocket ).getNickname() ), 0 );
  replyMsg( clientSocket, RPL_YOURHOST( _clients.at( clientSocket ).getSource(), _clients.at( clientSocket ).getNickname() ), 0 );
  replyMsg( clientSocket, RPL_CREATED( _clients.at( clientSocket ).getSource(), _clients.at( clientSocket ).getNickname(), date ), 0 );
  replyMsg( clientSocket, RPL_MYINFO( _clients.at( clientSocket ).getSource(), _clients.at( clientSocket ).getNickname() ), 0 );
  replyMsg( clientSocket, RPL_ISUPPORT( _clients.at( clientSocket ).getSource(), _clients.at( clientSocket ).getNickname(), getSupportToken() ), 0 );
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

void  Server::sendLusersMsg( int clientSocket )
{
  std::stringstream nbrClients;

  nbrClients << _clients.size();

  replyMsg( clientSocket, RPL_LUSERCLIENT( _clients.at( clientSocket ).getSource(), _clients.at( clientSocket ).getNickname(), nbrClients.str() ), 0 );
  replyMsg( clientSocket, RPL_LUSEROP( _clients.at( clientSocket ).getSource(), _clients.at( clientSocket ).getNickname(), "0" ), 0 ); // A MODIFIER getOpsNbr()
  replyMsg( clientSocket, RPL_LUSERUNKNOWN( _clients.at( clientSocket ).getSource(), _clients.at( clientSocket ).getNickname(), "0" ), 0 ); // A Modifier getUnknownStateUsers()
  replyMsg( clientSocket, RPL_LUSERCHANNELS( _clients.at( clientSocket ).getSource(), _clients.at( clientSocket ).getNickname(), "0" ), 0 ); // A MODIFIER getChannelNbr();
  replyMsg( clientSocket, RPL_LUSERCLIENT( _clients.at( clientSocket ).getSource(), _clients.at( clientSocket ).getNickname(), nbrClients.str() ), 0 );
}

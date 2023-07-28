/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   registration.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmelard <lmelard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/30 17:36:40 by cgaillag          #+#    #+#             */
/*   Updated: 2023/07/28 16:03:29 by lmelard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Server.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"
#include "utils.hpp"

void Server::checkRegistration( int clientSocket ) {
  // /*  ************************************  */
  // /*       POUR CHECKER LES SOCKETS !!      */
  // /*  ************************************  */
  // std::cout << "\n\t *** INFO INSIDE CHECKING REGISTRATION FUNCTION: ***\n";
  // std::cout << "\t getPassStatus <"<< _clients.at( clientSocket
  // ).getPassStatus() << ">\n"; std::cout << "\t getNickStatus <"<<
  // _clients.at( clientSocket ).getNickStatus() << ">\n"; std::cout << "\t
  // getUsername <"<< _clients.at( clientSocket ).getUsername() << ">\n";
  // std::cout << std::endl;
  // /*  ************************************  */
  // /*  ************************************  */

  if( _clients.at( clientSocket ).getPassStatus() == true
      && _clients.at( clientSocket ).getNickStatus() == true
      && _clients.at( clientSocket ).getUsername().empty() == false ) {
    _clients.at( clientSocket ).setIfRegistered( true );
    _clients.at( clientSocket )
      .setSource( _clients.at( clientSocket ).getNickname(),
                  _clients.at( clientSocket ).getUsername() );
    std::cout << MSGINFO << _clients.at( clientSocket ).getNickname();
    std::cout << " is now registered!" << std::endl;
    // DISPLAY WELCOME MESSAGES
    sendWelcomeMsg( clientSocket );
    sendLusersMsg( clientSocket );  // EQUIVALENT OF LUSERS received
    sendMotdMsg( clientSocket );    // EQUIVALENT OF MOTD command received
    if (_clients.size() == 1)
    {
      handleMode( clientSocket, _clients.at( clientSocket ).getNickname() + " +o");
      std::cout << "operator mode = " << _clients.at( clientSocket ).getOperatorMode() << std::endl;
    }
    std::cout << MSGINFO << "welcome message displayed" << std::endl;
  }
}

void Server::sendWelcomeMsg( int clientSocket ) {
  std::time_t t = std::time( 0 );
  std::tm*    local = std::localtime( &t );

  char formattedDate[30];
  std::strftime( formattedDate, sizeof( formattedDate ), "%a %b %d %H:%M:%S %Y",
                 local );
  std::string date( formattedDate );
  std::string source = _clients.at( clientSocket ).getSource();
  std::string nick = _clients.at( clientSocket ).getNickname();

  replyMsg( clientSocket, RPL_WELCOME( source, nick ), 0 );
  replyMsg( clientSocket, RPL_YOURHOST( source, nick ), 0 );
  replyMsg( clientSocket, RPL_CREATED( source, nick, date ), 0 );
  replyMsg( clientSocket, RPL_MYINFO( source, nick ), 0 );
  replyMsg( clientSocket, RPL_ISUPPORT( source, nick, getSupportToken() ), 0 );
}

std::string Server::getSupportToken() const {
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
  return ( token.str() );
}

void Server::sendLusersMsg( int clientSocket ) {
  std::stringstream nbrClients;

  nbrClients << _clients.size();
  std::string source = _clients.at( clientSocket ).getSource();
  std::string nick = _clients.at( clientSocket ).getNickname();

  replyMsg( clientSocket, RPL_LUSERCLIENT( source, nick, nbrClients.str(), getInvisibleUserNbr() ),
            0 );
  replyMsg( clientSocket, RPL_LUSEROP( source, nick, getOpsNbr() ),
            0 );
  replyMsg( clientSocket, RPL_LUSERUNKNOWN( source, nick, getUnknownStateUsers() ),
            0 );
  replyMsg( clientSocket, RPL_LUSERCHANNELS( source, nick, getChannelNbr() ),
            0 );
  replyMsg( clientSocket, RPL_LUSERME( source, nick, nbrClients.str() ),
            0 );
}

void Server::sendMotdMsg( int clientSocket ) {
  std::string   source = _clients.at( clientSocket ).getSource();
  std::string   nick = _clients.at( clientSocket ).getNickname();
  std::ifstream motdFile;
  motdFile.open( "./motd.txt", std::ifstream::in );
  if( !motdFile ) {
    throw std::runtime_error( "Error opening motd.txt file" );
  }

  replyMsg( clientSocket, RPL_MOTDSTART( source, nick, _serverName ), 0 );
  std::string line;
  while( std::getline( motdFile, line ) )
    replyMsg( clientSocket, RPL_MOTD( source, nick ) + line + "\r\n", 0 );
  motdFile.close();
  replyMsg( clientSocket, RPL_ENDOFMOTD( source, nick ), 0 );
}

std::string Server::getOpsNbr( void )
{
  int countOps = 0;
  for ( mapClients::iterator it = _clients.begin(); it != _clients.end(); it++ )
  {
    if (it->second.getOperatorMode() == true)
      countOps++;
  }
  return ( intToString( countOps ) );
}

std::string Server::getChannelNbr( void )
{
  return ( intToString( _channels.size() ) );
}

std::string Server::getUnknownStateUsers( void )
{
  int countUnknownState = 0;
  for ( mapClients::iterator it = _clients.begin(); it != _clients.end(); it++ )
  {
    if (it->second.getIfRegistered() == false)
      countUnknownState++;
  }
  return ( intToString( countUnknownState ) );
}

std::string Server::getInvisibleUserNbr( void )
{
  int countInvisibleUSer = 0;
  for ( mapClients::iterator it = _clients.begin(); it != _clients.end(); it++ )
  {
    if (it->second.getInvisibleMode())
      countInvisibleUSer++;
  }
  return ( intToString( countInvisibleUSer ) );
}
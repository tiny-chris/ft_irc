/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   registration.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmelard <lmelard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/30 17:36:40 by cgaillag          #+#    #+#             */
/*   Updated: 2023/07/31 15:03:07 by lmelard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Server.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"
#include "utils.hpp"

/**
 * @brief       Gets the number of operators of the server
 */

std::string Server::getOpsNbr( void )
{
  int countOps = 0;
  for ( mapClients::iterator it = _clients.begin(); it != _clients.end(); it++ ) {
    if (it->second.getOperatorMode() == true)
      countOps++;
  }
  return ( intToString( countOps ) );
}

/**
 * @brief       Gets the number of channels on server
 */

std::string Server::getChannelNbr( void ) {
  return ( intToString( _channels.size() ) );
}

/**
 * @brief       Gets the number of unknown state users (non registered clients)
 */

std::string Server::getUnknownStateUsers( void ) {
  int countUnknownState = 0;
  for ( mapClients::iterator it = _clients.begin(); it != _clients.end(); it++ ) {
    if (it->second.getIfRegistered() == false) {
      countUnknownState++;
    }
  }
  return ( intToString( countUnknownState ) );
}

/**
 * @brief       Gets the number of invisible user on the server 
 *              (user mode +i)
 */

std::string Server::getInvisibleUserNbr( void ) {
  int countInvisibleUSer = 0;
  for ( mapClients::iterator it = _clients.begin(); it != _clients.end(); it++ ) {
    if (it->second.getInvisibleMode()) {
      countInvisibleUSer++;
    }
  }
  return ( intToString( countInvisibleUSer ) );
}

/**
 * @brief      Gets tokens for RPL_ISUPPORT
 *             (parameters supported by the server)
 */

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

/**
 * @brief       Displays the welcome messages, after registration
 *   
 */

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

/**
 * @brief       Displays statistics about local 
 *              and global users, as numeric replies.
 */

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

/**
 * @brief       Gets the “Message of the Day” of the given server
 */

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

/**
 * @brief       Checks the user is registered
 *              Valid password + valid nickname + valid username
 *              If he is the first user registered he is set as a server operator
 */

void Server::checkRegistration( int clientSocket ) {
  if( _clients.at( clientSocket ).getPassStatus() == true
      && _clients.at( clientSocket ).getNickStatus() == true
      && _clients.at( clientSocket ).getUsername().empty() == false ) {
    _clients.at( clientSocket ).setIfRegistered( true );
    _clients.at( clientSocket )
      .setSource( _clients.at( clientSocket ).getNickname(),
                  _clients.at( clientSocket ).getUsername() );
    std::cout << MSGINFO << _clients.at( clientSocket ).getNickname();
    std::cout << " is now registered!" << std::endl;
    sendWelcomeMsg( clientSocket ); // DISPLAY WELCOME MESSAGES
    sendLusersMsg( clientSocket );  // EQUIVALENT OF LUSERS received
    sendMotdMsg( clientSocket );    // EQUIVALENT OF MOTD command received
    if (_clients.size() == 1) {
      handleMode( clientSocket, _clients.at( clientSocket ).getNickname() + " +o");
      std::cout << MSGINFO << "operator mode = " << _clients.at( clientSocket ).getOperatorMode() << std::endl;
    }
    std::cout << MSGINFO << "welcome message displayed" << std::endl;
  }
}

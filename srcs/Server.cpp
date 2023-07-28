/* srcs/Server */
/* Created: 230725 07:55:18 by clem@spectre */
/* Updated: 230725 07:55:18 by clem@spectre */
/* Maintainer: Clément Vidon */
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/22 14:40:23 by lmelard           #+#    #+#             */
/*   Updated: 2023/07/24 18:44:22 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <netdb.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <string>

#include "defines.hpp"
#include "numericReplies.hpp"
#include "utils.hpp"

#include "Client.hpp"
#include "Server.hpp"
#include "Utility.hpp"

/*  CANON ------------------------------------------- */

Server::Server( void )
  : _port( 0 ),
    _password( "" ),
    _serverName( "" ),
    _serverSocket( -1 ),
    _epollFd( -1 ) {
  initCommands();  // TODO any vars there to init / copy ??
  createServerSocket();
  return;
}

Server::Server( size_t port, const char* password, std::string serverName )
  : _port( port ),
    _password( password ),
    _serverName( serverName ),
    _serverSocket( -1 ),
    _epollFd( -1 ) {
  initCommands();
  createServerSocket();
  return;
}

Server::Server( const Server& src )
  : _port( src._port ),
    _password( src._password ),
    _serverName( src._serverName ),
    _serverSocket( src._serverSocket ),
    _epollFd( src._epollFd ) {
  std::map<int, Client>::const_iterator it;

  for( it = src._clients.begin(); it != src._clients.end(); ++it ) {
    _clients[it->first] = Client( it->second );
  }
  _disconnectedClients = src._disconnectedClients;
}

Server::~Server( void ) {
  stop();
  std::cout << MSGINFO << "Server shutting down...\n";
}

Server& Server::operator=( Server const& rhs ) {
  std::map<int, Client>::const_iterator it;

  if( this == &rhs ) {
    return *this;
  }
  _port = rhs._port;
  _password = rhs._password;
  _serverName = rhs._serverName;
  _serverSocket = rhs._serverSocket;
  _epollFd = rhs._epollFd;
  for( it = rhs._clients.begin(); it != rhs._clients.end(); ++it ) {
    _clients[it->first] = Client( it->second );
  }
  _disconnectedClients = rhs._disconnectedClients;
  return *this;
}

void Server::print( std::ostream& o ) const {
  o << "Server:";
  o << "  Server socket:" << _serverSocket;
  o << "  Connected Clients: " << _disconnectedClients.size() << "\n";
  o << "  Disconnected Clients: " << _disconnectedClients.size() << "\n";
}

std::ostream& operator<<( std::ostream& o, Server const& i ) {
  i.print( o );
  return o;
}

/*  ACCESSORS --------------------------------------- */

void   Server::setPort( size_t& port ) { _port = port; }
size_t Server::getPort( void ) const { return _port; }

void Server::setPassword( std::string& password ) { _password = password; }
std::string Server::getPassword( void ) const { return _password; }

/*  IMPLEMENTATION ---------------------------------- */

/**
 * @brief       Stop the server.
 */

void Server::stop( void ) {
  broadcastQuitToAll();
  disconnectAllClients();
  removeDisconnectedClients();
  Utility::closeFd( _epollFd );
  Utility::closeFd( _serverSocket );
}

/**
 * @brief      Removes disconnected clients from the server.
 */

void Server::removeDisconnectedClients( void ) {
  std::size_t disconnectCientsNumber = _disconnectedClients.size();
  size_t      clientsNumber = _clients.size();

  for( std::size_t i = 0; i < disconnectCientsNumber; ++i ) {
    _clients.erase( _disconnectedClients[i] );
    Utility::closeFd( _disconnectedClients[i] );
  }
  if( disconnectCientsNumber != 0 ) {
    std::cout << MSGINFO << "<" << disconnectCientsNumber << "/"
              << clientsNumber << "_clients removed>\n";
  }
  _disconnectedClients.clear();
}

/**
 * @brief      Disconnects all the clients.
 */

void Server::disconnectAllClients() {
  std::map<int, Client>::const_iterator it;

  for( it = _clients.begin(); it != _clients.end(); ++it ) {
    std::cout << MSGINFO << "<" << it->second.getNickname();
    std::cout << " disconnected>\n";
    _disconnectedClients.push_back( it->first );
  }
}

/**
 * @brief      Disconnects a client at the specified index.
 */

void Server::disconnectAClient( int clientSocket ) {
  std::cout << MSGINFO << "<" << _clients.at( clientSocket ).getNickname();
  std::cout << " disconnected>\n";
  _disconnectedClients.push_back( static_cast<int>( clientSocket ) );
}

// /**
//  * @brief       Broadcasts a message to all connected clients except the
//  *              sender
//  */

// void Server::broadcastMsg( std::string& msg, int clientSocket ) {
//   std::map<int, Client>::const_iterator it;
//   int                                   recipient;

//   msg = _clients.at( clientSocket ).getNickname() + ": " + msg + "\n";
//   for( it = _clients.begin(); it != _clients.end(); ++it ) {
//     recipient = it->first;
//     if( recipient != _serverSocket && recipient != clientSocket ) {
//       if( send( recipient, msg.c_str(), msg.length(), 0 ) < 0 ) {
//         std::string message = "send: " + std::string( strerror( errno ) );
//         throw std::runtime_error( message );
//       }
//     }
//   }
//   std::cout << msg;
// }

/**
 * @brief       Broadcasts a message that says that the server shut down to all 
 *              connected clients
 */
void   Server::broadcastQuitToAll( void )
{
  for (mapClients::iterator it = _clients.begin(); it != _clients.end(); ++it)
      replyMsg( it->second.getFd(), RPL_QUIT(it->second.getSource(), "", "Disconnected from server"), 0);
}

/**
 * @brief       Broadcasts a message to all connected clients including the
 *              sender
 */
void Server::broadcastMsgToAll( int clientSocket, const std::string& message ) 
{
  broadcastMsgNotClient( clientSocket, message );
  replyMsg( clientSocket, message, 0 );
}

/**
 * @brief       Broadcasts a message to all connected clients except the
 *              sender
 */
void Server::broadcastMsgNotClient( int clientSocket, const std::string& message )
{
  mapClients::const_iterator	it;
  int   					            socket;

  for( it = _clients.begin(); it != _clients.end(); ++it ) {
    socket = it->first;
    if( socket != clientSocket ) {
      replyMsg( socket, message, 0 );
    }
  }
  std::cout << MSGREPLY << message << std::endl;
}

/**
 * @brief       Send message to the client with all specific parameter (incl.
 * numeric replies) and copy it on the server side if flag is 1 (otherwise, do
 * nothing on the server)
 *
 */
void Server::replyMsg( int clientSocket, std::string reply,
                       bool copyToServer ) {
  if( copyToServer == true ) {
    std::cout << MSGREPLY << reply << std::endl;
  }
  if ( send( clientSocket, reply.c_str(), reply.length(), 0 ) < 0 ) {
    std::string errMsg = "send: " + std::string( strerror( errno ) );
    throw std::runtime_error( errMsg );
  }
  return;
}

/**
 * @brief       Handle request by identifying command and parameters
 */

void Server::handleRequest( int clientSocket, std::string request ) {
  std::string command = "";
  std::string parameters = "";
  size_t      splitter;
  int         commandKey = 0;

  std::cout << "-----client [ ";
  if( _clients.at( clientSocket ).getNickStatus() ) {
    std::cout << _clients.at( clientSocket ).getNickname() << " ";
  }
  std::cout << "on socket " << clientSocket << " ]-----\n";
  // std::cout << "-----client [" <<  << " ";
  // std::cout << _clients.at( clientSocket ).getNickname() << "]-----\n" ;
  std::cout << "request: <" << request << ">" << std::endl;

  splitter = request.find( ' ', 0 );

  /* ********************************* */
  /* ACTION 1   - get command & params */
  /* ********************************* */
  // if <request> has no space --> command is request string
  // else --> split command & parameters
  if( splitter != std::string::npos ) {
    command = request.substr( 0, splitter );
    parameters = request.substr( splitter + 1, std::string::npos );
  }
  else
    command = request;
  
  /* ********************************* */
  /* ACTION 2   - check the case when the client is disconnected and return */
  /* ********************************* */
  //   /* TODO:  prévoir le cas où le client est / a été déconnecté et breaker
  //   */

  /* ********************************* */
  /* ACTION 3   - check if 'command' is part of the _commands and get the
   * command key */
  /* ********************************* */
  for( mapCommands::const_iterator it = _commands.begin();
       it != _commands.end(); ++it ) {
    if( command == it->second ) {
      commandKey = it->first;
    }
  }

  /* ********************************* */
  /* ACTION 4   - if 'command' is in _commands */
  /* ********************************* */
  //
  //  SUB-ACTION 4.1   - 'PASS' must be the first command entered, if not -->
  //  return (numeric_reply ??) SUB-ACTION 4.2   - if command is NOT an
  //  authentifier (PASS, NICK, USER) or QUIT and if client is not registered
  //  yet --> ERR_NOTREGISTERED
  //
  //    ???? QUESTION ????  pour 'quit' --> doit-on etre registered ? idem
  //    pour potentiel shutdown
  //
  if( commandKey != UNDEFINED && commandKey != CAP ) {
    if( command != "PASS"
        && _clients.at( clientSocket ).getPassStatus() == false ) {
      std::cout << MSGERROR << "must set PASS command first\n" << std::endl;
      return;
    }
    if( !_clients.at( clientSocket ).getIfRegistered()
        && !( command == "PASS" || command == "NICK" || command == "USER"
              || command == "QUIT" ) ) {
      replyMsg( clientSocket,
                ERR_NOTREGISTERED(
                  _serverName, _clients.at( clientSocket ).getNickname() ) );
      return;
    }
  }
  
  /* ********************************* */
  /* ACTION 5   - handle command */
  /* ********************************* */
  // This message is not required for a server implementation to work, but
  // SHOULD be implemented. If a command is not implemented, it MUST return
  // the ERR_UNKNOWNCOMMAND (421) numeric.
  switch( commandKey ) {
    case CAP:
      std::cout << std::endl;
      break;
    case PASS:
      handlePass( clientSocket, parameters );
      break;
    case NICK:
      handleNick( clientSocket, parameters );
      break;
    case USER:
      handleUser( clientSocket, parameters );
      break;
    case PING:
      handlePing( clientSocket, parameters );
      break;
    case MODE:
      handleMode( clientSocket, parameters );
      break;
    case JOIN:
      handleJoin( clientSocket, parameters );
      break;
    case PRIVMSG:
      handlePrivmsg( clientSocket, parameters );
      break;
    case KICK:
      handleKick( clientSocket, parameters );
      break;
    case TOPIC:
      handleTopic( clientSocket, parameters );
      break;
    case INVITE:
      handleInvite( clientSocket, parameters );
      break;
    case NAMES:
      handleNames( clientSocket, parameters );
      break;
    case WHO:
      handleWho( clientSocket, parameters );
      break;
    case PART:
      handlePart( clientSocket, parameters );
      break;
    case QUIT:
      handleQuit( clientSocket, parameters );
      break;
    case KILL:
      handleKill( clientSocket, parameters );
      break;
    case SQUIT:// ' /shutdown '
      handleSQuit( clientSocket, parameters );
      break;
    default: {
      if( !command.empty() || ( command.empty() && !parameters.empty() ) ) {
        replyMsg( clientSocket,
          ERR_UNKNOWNCOMMAND(_serverName, _clients.at( clientSocket ).getRealname(), command ) );
      }
    } break;
  }

  // /*  ************************************  */
  // /*    POUR CHECKER LES DATA CLIENTS !     */
  // /*  ************************************  */
  // std::cout << "\n\t *** CHECK CLIENT DATA (apres requete): ***\n";
  // std::cout << "\t client's getPassStatus <"
  //           << _clients.at( clientSocket ).getPassStatus() << ">\n";
  // std::cout << "\t client's getNickStatus <"
  //           << _clients.at( clientSocket ).getNickStatus() << ">\n";
  // std::cout << "\t client's getIfRegistered <"
  //           << _clients.at( clientSocket ).getIfRegistered() << ">\n";
  // std::cout << "\t client's getInvisibleMode <"
  //           << _clients.at( clientSocket ).getInvisibleMode() << ">\n";

  // std::cout << "\t client's getNickname <"
  //           << _clients.at( clientSocket ).getNickname() << ">\n";
  // std::cout << "\t client's getUsername <"
  //           << _clients.at( clientSocket ).getUsername() << ">\n";
  // std::cout << "\t client's getRealname <"
  //           << _clients.at( clientSocket ).getRealname() << ">\n";
  // std::cout << "\t client's getSource <"
  //           << _clients.at( clientSocket ).getSource() << ">\n";
  // std::cout << std::endl;
  // /*  ************************************  */
  // /*  ************************************  */
}

/**
 * @brief       Handles communication with existing clients.
 *
 * TODO ask chris for explanation
 */

void Server::handleExistingClient( int clientSocket ) {
  static std::string bufs[MAXCONNECTION + 1];
  char               buf[BUFMAXLEN];
  bool               isClear = false;
  ssize_t            bytesRead;

  // TODO REDO
  // DEPRECATED + 1 cause NO _clients[0] ANYMORE
  memset( buf, 0, sizeof( buf ) );
  bytesRead = recv( clientSocket, buf, sizeof( buf ), 0 );
  if( bytesRead < 0 ) {
    std::string message = "recv: " + std::string( strerror( errno ) );
    throw std::runtime_error( message );
  } else if( bytesRead == 0 ) {
    disconnectAClient( clientSocket );
    return;
  }
  // Turn "^M\n" into "\0" TODO OS compatibility
  // faire un pour verifier que ca finit bien par un
  bufs[clientSocket] += buf;
  // std::cout << "|INFO| initial buf: " << bufs[clientSocket] <<
  // std::endl;
  while( bufs[clientSocket].size() >= 2
         && bufs[clientSocket].find( CRLF ) != std::string::npos ) {
    isClear = true;
    handleRequest( clientSocket, bufs[clientSocket].substr(
                                   0, bufs[clientSocket].find( CRLF ) ) );
    bufs[clientSocket].erase( 0, bufs[clientSocket].find( CRLF ) + 2 );
  }
  if( isClear == true ) {
    bufs[clientSocket].clear();
  }
}

/**
 * @brief       Handles new client connections.
 */

void Server::handleNewClient( void ) {
  sockaddr_storage clientAddress;
  socklen_t        clientAddressLength;
  int              clientSocket;

  clientAddressLength = sizeof( clientAddress );
  clientSocket = accept( _serverSocket,
                         reinterpret_cast<struct sockaddr*>( &clientAddress ),
                         &clientAddressLength );
  if( clientSocket < 0 ) {
    std::cerr << "accept: " << strerror( errno ) << "\n";
    return;
  }
  if ( _clients.size() >= MAXCONNECTION ) {
    replyMsg( clientSocket, RPL_QUIT( _serverName, "", "Too many clients connected already") );
    Utility::closeFd( clientSocket );
    return;
  }
  std::cout << "-----------------------------" << std:: endl;
  std::cout << "New connection from " << Utility::ntop( clientAddress ) << "\n";
  struct epoll_event event;
  memset( &event, 0, sizeof( event ) );
  event.events = EPOLLIN;
  event.data.fd = clientSocket;
  if( epoll_ctl( _epollFd, EPOLL_CTL_ADD, event.data.fd, &event ) < 0 ) {
    std::string message = "epoll_ctl: " + std::string( strerror( errno ) );
    throw std::runtime_error( message );
  }
  _clients.insert( std::make_pair( clientSocket, Client( clientSocket ) ) );
  std::cout << "<New connection via socket " << clientSocket << ">\n";
}

/**
 * @brief       Creates a listening server socket.
 */

void Server::createServerSocket( void ) {
  int             opt = 1;
  int             status;
  struct addrinfo hints, *res, *p;

  std::memset( &hints, 0, sizeof( hints ) );
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  status = getaddrinfo(
    NULL, Utility::intToString( static_cast<int>( getPort() ) ).c_str(), &hints,
    &res );
  if( status != 0 ) {
    std::string message = "selectserver: " + Utility::gaiStrerror( status );
    throw std::runtime_error( message );
  }
  for( p = res; p != NULL; p = p->ai_next ) {
    _serverSocket = socket( p->ai_family, p->ai_socktype, p->ai_protocol );
    if( _serverSocket < 0 ) {
      continue;
    };
    if( setsockopt( _serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt,
                    sizeof( int ) )
        != 0 ) {
      freeaddrinfo( res );
      std::string message = "setsockopt: " + std::string( strerror( errno ) );
      throw std::runtime_error( message );
    }
    if( bind( _serverSocket, p->ai_addr, p->ai_addrlen ) != 0 ) {
      Utility::closeFd( _serverSocket );
      continue;
    }
    break;
  }
  freeaddrinfo( res );
  if( p == NULL ) {
    throw std::runtime_error( "Error creating socket" );
  }
  if( listen( _serverSocket, 10 ) == -1 ) {
    throw std::runtime_error( "Error listening on socket" );
  }
  return;
}

/**
 * @brief       Initialize the commands to be handled in IRC into a
 *              map<int,std::string>
 */

void Server::initCommands( void ) {
  _commands.insert( std::make_pair( 0, "UNDEFINED" ) );
  _commands.insert( std::make_pair( 99, "CAP" ) );
  _commands.insert( std::make_pair( 100, "PASS" ) );
  _commands.insert( std::make_pair( 101, "NICK" ) );
  _commands.insert( std::make_pair( 102, "USER" ) );
  _commands.insert( std::make_pair( 103, "PING" ) );
  _commands.insert( std::make_pair( 109, "MODE" ) );
  _commands.insert( std::make_pair( 110, "JOIN" ) );
  _commands.insert( std::make_pair( 111, "PRIVMSG" ) );
  _commands.insert( std::make_pair( 112, "KICK" ) );
  _commands.insert( std::make_pair( 113, "TOPIC" ) );
  _commands.insert( std::make_pair( 114, "INVITE" ) );
  _commands.insert( std::make_pair( 120, "NAMES" ) );
  _commands.insert( std::make_pair( 130, "PART" ) );
  _commands.insert( std::make_pair( 131, "WHO" ) );
  _commands.insert( std::make_pair( 1000, "SQUIT" ) );//ex- /shutdown
  _commands.insert( std::make_pair( 1001, "QUIT" ) );
  _commands.insert( std::make_pair( 1002, "KILL" ) );
  return;
}

/**
 * @brief       Start listening.
 */

void Server::start( void ) {
  int                eventsSize;
  struct epoll_event events[MAX_EVENTS];
  struct epoll_event event;

  _epollFd = epoll_create( 1 );
  if( _epollFd < 0 ) {
    std::string message = "epoll_create: " + std::string( strerror( errno ) );
    throw std::runtime_error( message );
  }
  memset( &event, 0, sizeof( event ) );
  event.events = EPOLLIN;
  event.data.fd = _serverSocket;
  if( epoll_ctl( _epollFd, EPOLL_CTL_ADD, event.data.fd, &event ) < 0 ) {
    std::string message = "epoll_ctl: " + std::string( strerror( errno ) );
    throw std::runtime_error( message );
  }
  std::cout << "Welcome on " << _serverName << "!\n\n";
  std::cout << "  hostname:\t\t"
            << "127.0.0.1 (localhost)\n";
  std::cout << "  port:\t\t\t" << _port << "\n";
  std::cout << "  server socket:\t" << _serverSocket << "\n\n";
  while( _serverSocket != -1 ) {
    eventsSize = epoll_wait( _epollFd, events, MAX_EVENTS, -1 );  // 3.
    if( eventsSize == -1 ) {
      std::string message = "epoll_wait: " + std::string( strerror( errno ) );
      throw std::runtime_error( message );
    }
    for( int i = 0; i < eventsSize; i++ ) {
      if( events[i].events & EPOLLIN ) {
        if( events[i].data.fd == _serverSocket ) {
          handleNewClient();
        } else {
          handleExistingClient( events[i].data.fd );
        }
      }
    }
    removeDisconnectedClients();
  }
}

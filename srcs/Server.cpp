/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgaillag <cgaillag@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by 2.fr>             #+#    #+#             */
/*   Updated: 2023/08/01 21:02:24 by cgaillag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <netdb.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <string>
#include <fcntl.h>

#include "defines.hpp"
#include "numericReplies.hpp"
#include "utils.hpp"

#include "Client.hpp"
#include "Server.hpp"
#include "Utility.hpp"

/* CONSTRUCTORS ***************************************************************/

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

/* DESTRUCTORS ****************************************************************/

Server::~Server( void ) {
  stop();
  std::cout << MSGINFO << "Server shutting down...\n";
}

/* OVERLOADS ******************************************************************/

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

/**
 * @brief       Function to check if a client exists by nickname
 *              and return a pointer to the client
 */

Client* Server::getClientByNickname( const std::string& nickname ) {
  for( mapClients::iterator it = _clients.begin(); it != _clients.end();
      it++ ) {
    if( it->second.getNickname() == nickname ) {
      return ( &it->second );
    }
  }
  return NULL;
}

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
 * @brief       Gets the client fd (socket) when a client exists
 */

int Server::findClientFd( const std::string& name ) {
  for( mapClients::iterator it = _clients.begin(); it != _clients.end();
      ++it ) {
    if( it->second.getNickname() == name )
      return ( it->second.getFd() );
  }
  return -1;
}

/**
 * @brief       Checks if the new nickname has already been assigned
 *              to another client
 */

bool Server::existingNick( std::string param ) {
  for( mapClients::iterator it = _clients.begin(); it != _clients.end();
      ++it ) {
    if( it->second.getNickname() == param ) {
      return true;
    }
  }
  return false;
}

/**
 * @brief       Checks if the Channel exists on the server
 */

bool Server::existingChannel( std::string param ) {
  if( _channels.find( param ) != _channels.end() ) {
    return true;
  }
  return false;
}

/**
 * @brief       Function that makes another client chanops when the only
 * 				      one chanop is quitting server or is kicked of the channel
 * 				      Channel cannot remain without a chanop
 */

void Server::changeChannelOperator( int clientSocket, Client* toLeave,
    Channel* chan ) {
  Client* toBeChanOp = NULL;
  for( mapClientsPtr::iterator it = chan->getChannelMembers().begin();
      it != chan->getChannelMembers().end(); it++ ) {
    if( it->second != toLeave ) {
      toBeChanOp = it->second;
      break;
    }
  }
  if( toBeChanOp != NULL ) {
    std::string toBeChanOpName = toBeChanOp->getNickname();
    std::string param = chan->getChannelName() + " +o " + toBeChanOpName;
    handleMode( clientSocket, param );
  }
  return;
}

/**
 * @brief      Removes disconnected clients from the server.
 */

void Server::removeDisconnectedClients( void ) {
  std::size_t disconnectClientsNumber = _disconnectedClients.size();
  size_t      clientsNumber = _clients.size();

  for( std::size_t i = 0; i < disconnectClientsNumber; ++i ) {
    _clients.erase( _disconnectedClients[i] );
    Utility::closeFd( _disconnectedClients[i] );
  }
  if( disconnectClientsNumber != 0 ) {
    std::cout << MSGINFO << "<" << disconnectClientsNumber << "/"
      << clientsNumber << "_clients removed>\n\n";
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

/**
 * @brief       Broadcasts a message that says that the server shut down to all
 *              connected clients
 */

void Server::broadcastQuitToAll( void ) {
  for( mapClients::iterator it = _clients.begin(); it != _clients.end(); ++it )
    replyMsg(
        it->second.getFd(),
        RPL_QUIT( it->second.getSource(), "", "Disconnected from server" ), 0 );
}

/**
 * @brief       Broadcasts a message to all connected clients including the
 *              sender
 */

void Server::broadcastMsgToAll( int clientSocket, const std::string& message ) {
  broadcastMsgNotClient( clientSocket, message );
  replyMsg( clientSocket, message, 0 );
}

/**
 * @brief       Broadcasts a message to all connected clients except the
 *              sender
 */

void Server::broadcastMsgNotClient( int                clientSocket,
    const std::string& message ) {
  mapClients::const_iterator it;
  int                        socket;

  for( it = _clients.begin(); it != _clients.end(); ++it ) {
    socket = it->first;
    if( socket != clientSocket ) {
      replyMsg( socket, message, 0 );
    }
  }
  std::cout << MSGREPLY << message << std::endl;
}

/**
 * @brief       Channel message to all members
 */

void Server::channelMsgToAll( int clientSocket, const std::string& channelName,
    const std::string& message ) {
  channelMsgNotClient( clientSocket, channelName, message );
  replyMsg( clientSocket, message, 0 );
}

/**
 * @brief       Channel message to chanops only
 */

void Server::channelMsgToChanOps( int                clientSocket,
    const std::string& channelName,
    const std::string& message ) {
  Channel&                         channel = _channels.at( channelName );
  Channel::mapClientsPtr::iterator it;
  int                              socket;

  for( it = channel.getChannelOps().begin();
      it != channel.getChannelOps().end(); ++it ) {
    socket = it->second->getFd();
    if( socket != clientSocket ) {
      replyMsg( socket, message, 0 );
    }
  }
  // displays reply message on the server only once
  std::cout << MSGREPLY << message << std::endl;
}

/**
 * @brief       Channel message to all members except client
 */

void Server::channelMsgNotClient( int                clientSocket,
    const std::string& channelName,
    const std::string& message ) {
  Channel&                         channel = _channels.at( channelName );
  Channel::mapClientsPtr::iterator it;
  int                              socket;

  for( it = channel.getChannelMembers().begin();
      it != channel.getChannelMembers().end(); ++it ) {
    socket = it->second->getFd();
    if( socket != clientSocket ) {
      replyMsg( socket, message, 0 );
    }
  }
  // displays reply message on the server only once
  std::cout << MSGREPLY << message << std::endl;
}

/**
 * @brief       Sends message to the client with all specific parameters
 *              (incl. numeric replies) and copy it on the server side if
 *              'copyToServer' is true (otherwise, do not copy)
 */

void Server::replyMsg( int clientSocket, std::string reply,
    bool copyToServer ) {
  if( copyToServer == true ) {
    std::cout << MSGREPLY << reply << std::endl;
  }
  if( send( clientSocket, reply.c_str(), reply.length(), 0 ) < 0 ) {
    std::string errMsg = "send: " + std::string( strerror( errno ) );
    throw std::runtime_error( errMsg );
  }
  return;
}

void  Server::handleCommand( int clientSocket, int key, const std::string& command, const std::string& parameters ) {
  switch( key ) {
  case CAP:
    std::cout << std::endl;
    break;
  case INVITE:
    handleInvite( clientSocket, parameters );
    break;
  case JOIN:
    handleJoin( clientSocket, parameters );
    break;
  case KICK:
    handleKick( clientSocket, parameters );
    break;
  case KILL:
    handleKill( clientSocket, parameters );
    break;
  case MODE:
    handleMode( clientSocket, parameters );
    break;
  case NAMES:
    handleNames( clientSocket, parameters );
    break;
  case NICK:
    handleNick( clientSocket, parameters );
    break;
  case PART:
    handlePart( clientSocket, parameters );
    break;
  case PASS:
    handlePass( clientSocket, parameters );
    break;
  case PING:
    handlePing( clientSocket, parameters );
    break;
  case PRIVMSG:
    handlePrivmsg( clientSocket, parameters );
    break;
  case QUIT:
    handleQuit( clientSocket, parameters );
    break;
  case SQUIT:
    handleSQuit( clientSocket, parameters );
    break;
  case TOPIC:
    handleTopic( clientSocket, parameters );
    break;
  case USER:
    handleUser( clientSocket, parameters );
    break;
  case WHO:
    handleWho( clientSocket, parameters );
    break;
  default: {
    if( !command.empty() || ( command.empty() && !parameters.empty() ) ) {
      replyMsg( clientSocket, ERR_UNKNOWNCOMMAND(
          _serverName, _clients.at( clientSocket ).getNickname(), command ) );
    } else  // if ( command.empty() && parameter.empty() )
      std::cout << MSGINFO << "request is empty\n" << std::endl;
    } break;
  }
}

/**
 * @brief       Handle request by identifying command and parameters
 *              step 1: get command & params
 *              step 2: if 'command' is in _commands, get the command key
 *              step 3: check 'command' (authentifiers PASS, NICK, USER first)
 *              step 4: handle command
 */

void Server::handleRequest( int clientSocket, std::string request ) {
  std::string command = "";
  std::string parameters = "";
  int         commandKey = 0;

  std::cout << "-----client [ ";
  if( _clients.at( clientSocket ).getNickStatus() ) {
    std::cout << _clients.at( clientSocket ).getNickname() << " ";
  }
  std::cout << "on socket " << clientSocket << " ]-----\n";
  std::cout << "request: <" << request << ">" << std::endl;

  splitStringInTwo( request, ' ', &command, &parameters );
  for( mapCommands::const_iterator it = _commands.begin();
      it != _commands.end(); ++it ) {
    if( command == it->second ) {
      commandKey = it->first;
    }
  }
  if( commandKey != UNDEFINED && commandKey != CAP ) {
    if( command != "PASS"
        && _clients.at( clientSocket ).getPassStatus() == false ) {
      std::cout << MSGERROR << "must set PASS command first\n" << std::endl;
      return;
    }
    if( !_clients.at( clientSocket ).getIfRegistered()
        && !( command == "PASS" || command == "NICK" || command == "USER" ) ) {
      replyMsg( clientSocket,
          ERR_NOTREGISTERED(
            _serverName, _clients.at( clientSocket ).getNickname() ) );
      return;
    }
  }
  handleCommand( clientSocket, commandKey, command, parameters );
}

/**
 * @brief       Handles communication with existing clients.
 */

void Server::handleExistingClient( int clientSocket ) {
  static std::string bufs[MAXCONNECTION];
  char               buf[BUFMAXLEN];
  bool               isClear = false;
  ssize_t            bytesRead = 0;

  std::memset( buf, 0, BUFMAXLEN );
  while(true) {

    bytesRead = recv( clientSocket, buf, sizeof( buf ), 0 );

    if( bytesRead < 0 ) {
      std::string message = "recv: " + std::string( strerror( errno ) );
      throw std::runtime_error( message );
    } else if( bytesRead == 0 ) {
      handleQuit( clientSocket, ":bye bye" );
      return;
    }

    bufs[clientSocket] += std::string( buf, static_cast<size_t>( bytesRead ) );

    if( bufs[clientSocket].size() > MAX_MESSAGE_SIZE ) {
      std::cout << "Error: Received message is too long.\n\n";
      bufs[clientSocket].clear();
      char tempBuf[BUFMAXLEN];
      while( recv( clientSocket, tempBuf, BUFMAXLEN, MSG_DONTWAIT ) > 0 ) {
      }
      return;
    }

    while( bufs[clientSocket].size() >= 2 && bufs[clientSocket].find( CRLF ) != std::string::npos ) {
      isClear = true;
      handleRequest( clientSocket, bufs[clientSocket].substr( 0, bufs[clientSocket].find( CRLF ) ) );
      bufs[clientSocket].erase( 0, bufs[clientSocket].find( CRLF ) + 2 );
    }
    if( isClear == true ) {
      bufs[clientSocket].clear();
      return;
    }
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
  if( _clients.size() >= MAXCONNECTION ) {
    replyMsg( clientSocket, RPL_QUIT( _serverName, "",
          "Too many clients connected already" ) );
    Utility::closeFd( clientSocket );
    return;
  }
  std::cout << "-----------------------------" << std::endl;
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
  std::cout << "<New connection via socket " << clientSocket << ">\n\n";
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
  if (fcntl( _serverSocket, F_SETFL, O_NONBLOCK ) == -1) {
      throw std::runtime_error("Error setting socket to non-blocking");
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
  _commands.insert( std::make_pair( 1000, "SQUIT" ) );
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

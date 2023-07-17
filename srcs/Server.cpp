/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/22 14:40:23 by lmelard           #+#    #+#             */
/*   Updated: 2023/07/14 18:45:02 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <netdb.h>  // recv, send, sockaddr, accept, addrinfo, getaddrinfo, socket, setsockopt, bind, freeaddrinfo, listen
#include <stdlib.h>     // exit XXX
#include <sys/epoll.h>  // epoll stuff
#include <unistd.h>     // close
#include <cerrno>       // errno
#include <cstring>      // strerror
#include <iostream>     // cerr, cout
#include <string>       // string

#include "utils.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"

#include "Client.hpp"
#include "Server.hpp"
#include "Utility.hpp"

/* ----------------------- CONSTRUCTORS & DESTRUCTOR ------------------------ */

Server::Server( size_t port, const char *password, std::string serverName ) :
  _port(port),
  _password(password),
  _serverName(serverName) {

    _serverSocket = -1;
    _epollFd = -1;
    initCommands();

    createServerSocket();
    return;
  }

Server::~Server( void ) {
  stop();
  std::cout << "Server shutting down...\n";
}


/* ---------------------- MEMBER FUNCTIONS: ACCESSORS ----------------------- */

size_t      Server::getPort( void ) const { return _port; }
std::string Server::getPassword( void ) const { return _password; }
// std::string Server::getSource( void ) const { return _source; }

void        Server::setPort( size_t& port ) { _port = port; }
void        Server::setPassword( std::string& password ) { _password = password; }
// void        Server::setSource ( std::string nickname, std::string username ) {
//   _source.erase();
//   if (nickname.compare("") != 0 && username.compare("") != 0)
//     _source = ":" + nickname + "!" + username + "@localhost";
//   // else
//   //   _source = ":";
// }

/* -------------------- MEMBER FUNCTIONS: TO BE DEFINED --------------------- */

void Server::print( std::ostream& o ) const {
  o << "Server:";
  o << "  Server socket:" << _serverSocket;
  o << "  Connected Clients: " << _disconnectedClients.size() << "\n";
  o << "  Disconnected Clients: " << _disconnectedClients.size() << "\n";
}

/**
 * @brief      Removes disconnected clients from the server.
 */

void Server::removeDisconnectedClients( void ) {
  std::size_t totalDisconnectedClients = _disconnectedClients.size();
  size_t      totalClients = _clients.size();

  for( std::size_t i = 0; i < totalDisconnectedClients; ++i ) {
    _clients.erase( _disconnectedClients[i] );
    Utility::closeFd( _disconnectedClients[i] );
  }
  // DEV_BEG
  if( totalDisconnectedClients ) {
    std::cout << "<" << totalDisconnectedClients << "/" << totalClients;
    std::cout << "_clients removed>\n";
  }
  // END_END
  _disconnectedClients.clear();
}


void Server::stop( void ) {
  disconnectAllClients();
  removeDisconnectedClients();
  /* _clients.clear(); */
  Utility::closeFd( _epollFd );
  Utility::closeFd( _serverSocket );
}

/**
 * @brief      Disconnects a client at the specified index.
 *
 * @param[in]  index The index of the client to disconnect.
 */

void Server::disconnectAClient( int clientSocket ) {
  std::cout << "<" << _clients.at( clientSocket ).getFd();
  std::cout << " disconnected>\n";
  _disconnectedClients.push_back( static_cast<int>( clientSocket ) );
}

/**
 * @brief      Disconnects all the clients
 */

void Server::disconnectAllClients() {
  std::map<int, Client>::const_iterator it;

  for( it = _clients.begin(); it != _clients.end(); ++it ) {
    std::cout << "<" << it->second.getNickname() << " disconnected>\n";
    _disconnectedClients.push_back( it->first );
  }
}

/**
 * @brief       Broadcasts a message to all connected clients except the sender
 *
 * @param[in]   msg The message to broadcast.
 * @param[in]   clientSocket The client socket
 */

void Server::broadcastMsg( std::string& msg, int clientSocket ) {
  std::map<int, Client>::const_iterator it;
  int                                   recipient;

  msg = _clients.at( clientSocket ).getNickname() + ": " + msg + "\n";
  for( it = _clients.begin(); it != _clients.end(); ++it ) {
    recipient = it->first;
    if( recipient != _serverSocket && recipient != clientSocket ) {
      if( send( recipient, msg.c_str(), msg.length(), 0 ) < 0 ) {
        std::cerr << "send: " << strerror( errno ) << "\n";
        exit( 1 );
      }
    }
  }
  std::cout << msg;
}

/**
 * @brief       Initialize the commands to be handled in IRC into a map<int,std::string>
 */

void	Server::initCommands( void )
{
  _mapCommands.insert(std::make_pair(0, "UNDEFINED"));
  _mapCommands.insert(std::make_pair(99, "CAP"));
  _mapCommands.insert(std::make_pair(100, "PASS"));
  _mapCommands.insert(std::make_pair(101, "NICK"));
  _mapCommands.insert(std::make_pair(102, "USER"));
  _mapCommands.insert(std::make_pair(103, "PING"));
  _mapCommands.insert(std::make_pair(109, "MODE"));
  _mapCommands.insert(std::make_pair(110, "JOIN"));
  _mapCommands.insert(std::make_pair(111, "PRIVMSG"));

  // temp elements --> will be replaced by valid command
  _mapCommands.insert(std::make_pair(1000, "/shutdown"));
  _mapCommands.insert(std::make_pair(1001, "/quit"));
  _mapCommands.insert(std::make_pair(1003, "/msg"));
  return ;
}

/**
 * @brief       Send message to the client with all specific parameter (incl. numeric replies)
 *              and copy it on the server side if flag is 1 (otherwise, do nothing on the server)
 *
 */
void	Server::replyMsg( int clientSocket, std::string reply, int flag )
{
  if (flag == 1)
  {
    std::cout << "reply:\t " << reply << std::endl;
  }
  send(clientSocket, reply.c_str(), reply.length(), 0);
  return ;
}

/**
 * @brief       Handle request by identifying command and parameters
 */

void  Server::handleRequest( int clientSocket, std::string request )
{
  std::string command;
  std::string parameters = "";
  size_t      splitter;
  int         commandKey = 0;

  std::cout << "-----client [" << clientSocket;
  std::cout << _clients.at( clientSocket ).getNickname() << "]-----\n" ;
  std::cout << "request: <" << request << ">" << std::endl;

  splitter = request.find(' ', 0);

  /* ********************************* */
  /* ACTION 1   - get command & params */
  /* ********************************* */
  // if <request> has no space --> command is request string
  // else --> split command & parameters
  if (splitter == std::string::npos)
    command = request;
  else
  {
    command = request.substr(0, splitter);
    parameters = request.substr(splitter + 1, std::string::npos);
  }

  // /* ********************************* */
  // /* SUBACTION  - turn command content into capital letters */     TO KEPP ??????
  // /* ********************************* */
  // for (size_t i = 0; i < command.length(); ++i)
  //   command[i] = std::toupper(command[i]);

  /* ********************************* */
  /* ACTION 2   - check the case when the client is disconnected and return */
  /* ********************************* */
  //   /* TODO:  prévoir le cas où le client est a été déconnecté et breaker */


  /* ********************************* */
  /* ACTION 3   - check if 'command' is part of the _mapCommands and get the command key */
  /* ********************************* */
  for (std::map<int, std::string>::const_iterator it = _mapCommands.begin(); it != _mapCommands.end(); ++it)
  {
    if (command == it->second) {
      commandKey = it->first;
    }
  }

  /* ********************************* */
  /* ACTION 4   - if 'command' is in _mapCommands */
  /* ********************************* */
  //
  //  SUB-ACTION 4.1   - 'PASS' must be the first command entered, if not --> return (numeric_reply ??)
  //  SUB-ACTION 4.2   - if command is NOT an authentifier (PASS, NICK, USER) or QUIT and if client is not registered yet --> ERR_NOTREGISTERED
  //
  //    ???? QUESTION ????  pour 'quit' --> doit-on etre registered ? idem pour potentiel shutdown
  //
  if (commandKey != UNDEFINED && commandKey != CAP)
  {
    if (command != "PASS" && _clients.at( clientSocket ).getPassStatus() == false )
    {
      std::cout << "error:\t must set PASS command first\n" << std::endl;
      return;
    }
    if (!_clients.at( clientSocket ).getIfRegistered()
        && !(command == "PASS" || command == "NICK" || command == "USER" || command == "QUIT"))
    {
      replyMsg(clientSocket, ERR_NOTREGISTERED( _serverName,_clients.at( clientSocket ).getNickname() ));
      return;
    }
  }
  /* ********************************* */
  /* ACTION 5   - handle command */
  /* ********************************* */
  // This message is not required for a server implementation to work, but SHOULD be implemented.
  // If a command is not implemented, it MUST return the ERR_UNKNOWNCOMMAND (421) numeric.
  switch(commandKey)
  {
    case CAP:     std::cout << std::endl; break;
    case PASS:    handlePass( clientSocket, parameters ); break;
    case NICK:    handleNick( clientSocket, parameters ); break;
    case USER:    handleUser( clientSocket, parameters ); break;
    case PING:    handlePing( clientSocket, parameters ); break;
    case MODE:    handleMode( clientSocket, parameters ); break;
    case JOIN:        std::cout << "client " << _clients.at( clientSocket ).getNickname() << " - use function to handle JOIN command" << std::endl; break;
    case PRIVMSG:     std::cout << "client " << _clients.at( clientSocket ).getNickname() << " - use function to handle PRIVMSG command" << std::endl; break;

                      // keeping Clement's initial commands just in case... - START
    case ZZ_SHUTDOWN: stop(); break;                          // ' /shutdown '
    case ZZ_QUIT:     disconnectAClient( clientSocket ); break;                // ' /quit '

    case ZZ_MSG:      broadcastMsg( parameters, clientSocket ); break;        // ' /msg <message to broadcast>'
    default:			{
                    if (!command.empty() || (command.empty() && !parameters.empty()))
                    {
                      replyMsg(clientSocket, ERR_UNKNOWNCOMMAND( _serverName, _clients.at( clientSocket ).getRealname(), command ));
                    }
                  } break;
  }
}

void Server::handleExistingClient( int clientSocket ) {
  int checkClear = 0;
  char buf[BUFMAXLEN];  // Buffer for client data
                        // TODO A REFAIRE !!!
  static std::string bufs[MAXCONNECTION + 1]; // TODO plus de client 0
  memset(buf, 0, sizeof(buf));


  ssize_t bytesRead = recv( clientSocket, buf, sizeof( buf ), 0 );

  if( bytesRead < 0 ) {
    std::cerr << "recv: " << strerror( errno ) << "\n";
    exit( 1 );
  } else if( bytesRead == 0 ) {
    disconnectAClient( clientSocket );
    return;
  }
  // Turn "^M\n" into "\0" TODO OS compatibility
  //faire un pour verifier que ca finit bien par un
  bufs[clientSocket] += buf;
  // std::cout << "|INFO| initial buf: " << bufs[clientSocket] << std::endl;
  while (bufs[clientSocket].size() >= 2 && bufs[clientSocket].find(CRLF) != std::string::npos)
  {
    checkClear = 1;
    handleRequest( clientSocket, bufs[clientSocket].substr(0, bufs[clientSocket].find(CRLF)) );
    bufs[clientSocket].erase(0, bufs[clientSocket].find(CRLF) + 2);
  }
  if (checkClear == 1){
    bufs[clientSocket].clear();
  }
}

/**
 * @brief       Creates a listening server socket.
 */

void Server::createServerSocket( void ) {
  int             opt = 1;  // For setsockopt() SO_REUSEADDR, below
  int             status;
  struct addrinfo hints, *res, *p;

  std::memset( &hints, 0, sizeof( hints ) );
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  status = getaddrinfo( NULL, Utility::intToString( getPort() ).c_str(), &hints, &res );
  if( status != 0 ) {
    std::cerr << "selectserver: " << Utility::gaiStrerror( status ) << "\n";
    exit( 1 );
  }
  for( p = res; p != NULL; p = p->ai_next ) {
    _serverSocket = socket( p->ai_family, p->ai_socktype, p->ai_protocol );
    if( _serverSocket < 0 ) {
      continue;
    };
    if( setsockopt( _serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt,
          sizeof( int ) )
        != 0 ) {
      std::cerr << "setsockopt: " << strerror( errno ) << "\n";
      exit( 1 );
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
  std::cout << "New connection from " << Utility::ntop( clientAddress );
  /* std::cout << ":" << ntohs( clientAddress.sin_port ) << std::endl; */
  std::cout << std::endl;

  struct epoll_event event;
  memset( &event, 0, sizeof( event ) );
  event.events = EPOLLIN;  // TODO EPOLLIN | EPOLLONESHOT
  event.data.fd = clientSocket;
  if( epoll_ctl( _epollFd, EPOLL_CTL_ADD, event.data.fd, &event ) < 0 ) {
    std::cerr << "epoll_ctl: " << strerror( errno ) << "\n";
    exit( 1 );
  }

  _clients.insert( std::make_pair( clientSocket, Client( clientSocket ) ) );

  std::cout << "<" << _clients.at( clientSocket ).getFd();
  std::cout << " joined the channel>\n";

  std::cout << "-------------------" << std::endl;
  std::cout << Utility::fdIsValid(_clients.find( clientSocket )->first) << "\n";
  std::cout << Utility::fdIsValid(_clients.at( clientSocket ).getFd()) << "\n";
  std::cout << "-------------------" << std::endl;
}

/**
 * @brief       Start listening.
 */

#define MAX_EVENTS 10 // TODO

void Server::start( void ) {
  int                eventsSize;
  struct epoll_event events[MAX_EVENTS];

  _epollFd = epoll_create( 1 );
  if( _epollFd < 0 ) {
    std::cerr << "epoll_create: " << strerror( errno ) << "\n";
    exit( 1 );
  }
  struct epoll_event event;
  memset( &event, 0, sizeof( event ) );
  event.events = EPOLLIN;  // TODO | EPOLLONESHOT ?
  event.data.fd = _serverSocket;
  if( epoll_ctl( _epollFd, EPOLL_CTL_ADD, event.data.fd, &event ) < 0 ) {
    std::cerr << "epoll_ctl: " << strerror( errno ) << "\n";
    exit( 1 );
  }
  while( _serverSocket != -1 ) {
    eventsSize = epoll_wait( _epollFd, events, MAX_EVENTS, -1 );  // 3.
    if( eventsSize == -1 ) {
      std::cerr << "epoll_wait: " << strerror( errno ) << "\n";
      exit( 1 );
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

std::ostream& operator<<( std::ostream& o, Server const& i ) {
  i.print( o );
  return o;
}

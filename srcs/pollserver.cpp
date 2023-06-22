/* study/pollserver */
/* Created: 230529 16:50:17 by clem9nt@imac */
/* Updated: 230529 16:50:19 by clem9nt@imac */
/* Maintainer: Clément Vidon */

#include <cerrno>    // errno
#include <cstring>   // strerror
#include <iostream>  // cerr, cout
#include <sstream>   // stringstream
#include <string>    // string
#include <vector>    // vector

#include <arpa/inet.h>  // inet_ntoa
#include <netdb.h>  // recv, send, sockaddr, accept, addrinfo, getaddrinfo, socket, setsockopt, bind, freeaddrinfo, listen
#include <poll.h>   // pollfd, poll
#include <stdlib.h>  // exit
#include <unistd.h>  // close

/**
 * TODO
 * - [ ] const correctness
 * - [ ] design pattern suggestion: RAII TODO Socket class !!
 * - [ ] add a way to quit the server
 * - [ ] add a way to quit the client
 * - [ ] turn exit() into exceptions
 */

#define PORT "6667"  // Port we're listening on

// /**
//  * @brief       Int to String
//  */

// std::string intToString( int number ) {
//   std::ostringstream oss;
//   oss << number;
//   return oss.str();
// }

// /**
//  * @brief       Forbidden gai_strerror implementation
//  */

// std::string gaiStrerror( int errorCode ) {
//   switch( errorCode ) {
//     case 0:
//       return "Success";
//     case EAI_AGAIN:
//       return "Temporary failure in name resolution";
//     case EAI_BADFLAGS:
//       return "Invalid value for ai_flags";
//     case EAI_FAIL:
//       return "Non-recoverable failure in name resolution";
//     case EAI_FAMILY:
//       return "ai_family not supported";
//     case EAI_MEMORY:
//       return "Out of memory";
//     case EAI_NONAME:
//       return "Name or service not known";
//     case EAI_SERVICE:
//       return "Invalid value for service";
//     case EAI_SOCKTYPE:
//       return "ai_socktype not supported";
//     case EAI_SYSTEM:
//       return strerror( errno );
//     default:
//       return "Unknown error";
//   }
// }

// /**
//  * @brief       Forbidden inet_ntop implementation
//  */

// std::string ntop( const struct sockaddr_storage& socket ) {
//   std::stringstream ss;

//   if( socket.ss_family == AF_INET ) {
//     const struct sockaddr_in* sockaddr;
//     const uint8_t*            addr;
//     sockaddr = reinterpret_cast<const struct sockaddr_in*>( &socket );
//     addr = reinterpret_cast<const uint8_t*>( &sockaddr->sin_addr.s_addr );
//     ss << static_cast<int>( addr[0] ) << ".";
//     ss << static_cast<int>( addr[1] ) << ".";
//     ss << static_cast<int>( addr[2] ) << ".";
//     ss << static_cast<int>( addr[3] ) << ".";
//     return ss.str();
//   } else if( socket.ss_family == AF_INET6 ) {
//     const struct sockaddr_in6* sockaddr;
//     const uint8_t*             addr;
//     sockaddr = reinterpret_cast<const struct sockaddr_in6*>( &socket );
//     addr = sockaddr->sin6_addr.s6_addr;
//     for( int i = 0; i < 16; ++i ) {
//       if( i > 0 && i % 2 == 0 ) {
//         ss << "::";
//       }
//       ss << static_cast<int>( addr[i] );
//     }
//     return ss.str();
//   } else {
//     return std::string( "UNKN_ADDR" );
//   }
// }

// /**
//  * @brief       Socket
//  */

// class Socket {
//  public:
//   Socket() : _sockfd( -1 ) {}
//   ~Socket() { closeSocket(); }

//   int createListenerSocket( void );
//   int getSocket( void ) { return _sockfd; }

//  private:
//   void closeSocket( void );  // useless?
//   /* int accept( struct sockaddr* addr, socklen_t* addrlen ); */
//   int _sockfd;
// };

// void Socket::closeSocket( void ) {
//   if( _sockfd != -1 ) {
//     close( _sockfd );
//     _sockfd = -1;
//   }
// }

// int Socket::createListenerSocket( void ) {
//   int             yes = 1;  // For setsockopt() SO_REUSEADDR, below
//   int             status;
//   struct addrinfo hints, *res, *p;
//   // Get us a socket and bind it
//   std::memset( &hints, 0, sizeof( hints ) );
//   hints.ai_family = AF_UNSPEC;
//   hints.ai_socktype = SOCK_STREAM;
//   hints.ai_flags = AI_PASSIVE;
//   status = getaddrinfo( NULL, PORT, &hints, &res );
//   if( status != 0 ) {
//     std::cerr << "selectserver: " << gaiStrerror( status ) << "\n";
//     exit( 1 );
//   }
//   for( p = res; p != NULL; p = p->ai_next ) {
//     _sockfd = socket( p->ai_family, p->ai_socktype, p->ai_protocol );
//     if( _sockfd < 0 ) {
//       continue;
//     };
//     if( setsockopt( _sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof( int ) )
//         != 0 ) {
//       std::cerr << "setsockopt: " << strerror( errno ) << "\n";
//       exit( 1 );
//     }
//     if( bind( _sockfd, p->ai_addr, p->ai_addrlen ) != 0 ) {
//       close( _sockfd );
//       continue;
//     }
//     break;
//   }
//   freeaddrinfo( res );
//   if( p == NULL ) {
//     return -1;
//   }
//   if( listen( _sockfd, 10 ) == -1 ) {
//     return -1;
//   }
//   return 0;
// }

// /**
//  * @brief       Client
//  */

// struct Client {
//  public:
//   Client( std::string name ) : _name( name ) {}

//   void         setName( std::string const& name );
//   std::string& getName( void );

//  private:
//   std::string _name;
// };

// void Client::setName( std::string const& name ) {
//   _name = name;
// }

// std::string& Client::getName( void ) {
//   return _name;
// }

// /**
//  * @brief       Server
//  */

// class Server {
//  public:
//   Server() { setup(); }
//   ~Server() { shutdown(); }

//   void run();

//  private:
//   void shutdown( void );                              // close
//   void delConnection( size_t cid );                   // close
//   void broadcastMsg( std::string& msg, size_t cid );  // send
//   void parseData( const char* data, size_t cid );
//   void receiveData( size_t cid );  // recv, senb
//   void addConnection();            // accept
//   void setup();

//   // add a map of clients that contains their name, a struct of their
//   // socket and a struct of their status
//   std::vector<pollfd> _pfds;      // Pollable file descriptors
//   std::vector<Client> _clients;   // Pollable file descriptors
//   Socket              _listener;  // Use Socket for managing the listener socket
// };

// void Server::shutdown( void ) {
//   std::cout << "Server shutting down...\n";
//   for( size_t cid = 0; cid < _pfds.size(); ++cid ) {
//     if( _pfds[cid].fd != _listener.getSocket() ) {
//       close( _pfds[cid].fd );
//     }
//   }
//   _pfds.clear();
//   _clients.clear();
//   /* _listener.close(); */
//   exit( 0 );
// }

// /**
//  * @brief       Delete a connection
//  *
//  * Copy one of the fd in place of the deleted one to prevent the
//  * re-indexing of our list.
//  */

// void Server::delConnection( size_t cid ) {
//   close( _pfds[cid].fd );

//   _pfds[cid] = _pfds.back();
//   _pfds.pop_back();

//   _clients[cid] = _clients.back();
//   _clients.pop_back();

//   std::cout << "<" << _clients[cid].getName() << " left the channel>\n";
// }

// /**
//  * @brief       Broadcast a message to all clients
//  */

// void Server::broadcastMsg( std::string& msg, size_t cid ) {
//   int dest;

//   msg = _clients[cid].getName() + ": " + msg + "\n";
//   for( size_t i = 0; i < _pfds.size(); ++i ) {
//     dest = _pfds[i].fd;
//     if( dest != _listener.getSocket() && dest != _pfds[cid].fd ) {
//       if( send( dest, msg.c_str(), msg.length(), 0 ) == -1 ) {
//         std::cerr << "send: " << strerror( errno ) << "\n";
//       }
//     }
//   }
//   std::cout << msg;
// }

// void Server::parseData( const char* data, size_t cid ) {
//   std::string msg( data );

//   if( msg == "/shutdown" ) {
//     shutdown();
//   } else if( msg == "/quit" ) {
//     delConnection( cid );
//   } else if( msg.substr( 0, 6 ) == "/name " ) {
//     std::cout << "<" << _clients[cid].getName();
//     _clients[cid].setName( msg.substr( 6 ) );
//     std::cout << " became " << _clients[cid].getName() << ">\n";
//   } else {
//     broadcastMsg( msg, cid );
//   }
// }

// void Server::receiveData( size_t cid ) {
//   char buf[256];  // Buffer for client data
//   long nbytes = recv( _pfds[cid].fd, buf, sizeof( buf ), 0 );

//   if( nbytes <= 0 ) {
//     if( nbytes == 0 ) {
//       std::cout << "server: socket " << _pfds[cid].fd << " hung up\n";
//     } else {
//       std::cerr << "recv: " << strerror( errno ) << "\n";
//     }
//     delConnection( cid );
//     return;
//   }
//   // Turn "^M\n" into "\0" TODO OS compatibility
//   buf[nbytes - 2] = '\0';
//   parseData( buf, cid );
// }

// void Server::addConnection() {
//   int                     newsocket;   // Newly accept()ed socket descriptor
//   pollfd                  pfd;         // New pollfd for new connection
//   struct sockaddr_storage remoteAddr;  // Client address
//   socklen_t               remoteAddrLen;

//   remoteAddrLen = sizeof( remoteAddr );
//   newsocket = accept( _listener.getSocket(),
//                       reinterpret_cast<struct sockaddr*>( &remoteAddr ),
//                       &remoteAddrLen );
//   if( newsocket == -1 ) {
//     std::cerr << "accept: " << strerror( errno ) << "\n";
//     return;
//   }

//   // IPv6 TEST
//   /* sockaddr->sin6_family = AF_INET6; */
//   /* sockaddr->sin6_addr.s6_addr[15] = 1; */
//   /* std::string ipAddress = ntop( socket ); */
//   /* std::cout << "IPv6 Address: " << ipAddress << std::endl; */

//   // CREATE POLLFD
//   pfd.fd = newsocket;
//   pfd.events = POLLIN;  // Check ready-to-read
//   pfd.revents = 0;      // prevent conditional jump in run()
//   _pfds.push_back( pfd );

//   if( _clients.size() == 0 ) {
//     _clients.push_back( Client( "NONE" ) );
//   }
//   _clients.push_back( Client( "Anon_0" + intToString( newsocket ) ) );

//   std::cout << "<Anon_0" << newsocket << " joined the channel>\n";
//   /* std::cout << "pollserver: new connection from "; */
//   /* std::cout << ntop( remoteAddr ); */
//   /* std::cout << " on socket " << newsocket << "\n"; */
//   return;
// }

// /**
//  * @brief       Create the listener socket
//  */

// void Server::setup() {
//   pollfd pfd;

//   if( _listener.createListenerSocket() == -1 ) {
//     std::cerr << "error creating listening socket\n";
//     exit( 1 );
//   }
//   pfd.fd = _listener.getSocket();
//   pfd.events = POLLIN;  // Report read to read on incoming connection
//   pfd.revents = 0;      // Report read to read on incoming connection
//   _pfds.push_back( pfd );
//   // connection
// }

// /**
//  * @brief       Server main loop
//  */

// void Server::run() {
//   while( true ) {
//     // pfds is a vector that contains all our pollfd structs that contains all
//     // our socket
//     // - the first one is the listening socket that is bind()ed to 4242 port so
//     //   clients trying to connect to it can be detected. So the listener can
//     //   only accept / deny detected incoming connections
//     // - the other pfds are the connections that has been detected and accepted:
//     //   our clients, that can thus send / recv data
//     // - poll monitors each pollfd structs stored in pfds and handle the POLLIN
//     // event, so we know that if the POLLIN event occur on the listener socket
//     // it will be for a new connection otherwise if it occur on other socket it
//     // will be for send / recv

//     if( poll( _pfds.data(), _pfds.size(), -1 ) < 0 ) {
//       std::cerr << "poll: " << strerror( errno ) << "\n";
//       return;
//     }
//     if( _pfds[0].revents & POLLIN ) {
//       addConnection();
//     }
//     for( size_t cid = 0; cid < _pfds.size(); ++cid ) {
//       if( _pfds[cid].revents & POLLIN ) {
//         receiveData( cid );
//       }
//     }
//   }
// }

// int main( void ) {
//   Server server;
//   server.run();
//   return 0;
// }

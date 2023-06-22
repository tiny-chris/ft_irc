/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmelard <lmelard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/22 14:40:23 by lmelard           #+#    #+#             */
/*   Updated: 2023/06/22 18:27:05 by lmelard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Socket.hpp"
#include "Utils.hpp"

Server::Server(size_t port, const char *password) : _port(port), _password(password) { 
  std::cout << _password << std::endl;
  setup();
}

void Server::shutdown( void ) {
  std::cout << "Server shutting down...\n";
  for( size_t cid = 0; cid < _pfds.size(); ++cid ) {
    if( _pfds[cid].fd != _listener.getSocket() ) {
      close( _pfds[cid].fd );
    }
  }
  _pfds.clear();
  _clients.clear();
  /* _listener.close(); */
  exit( 0 );
}

/**
 * @brief       Delete a connection
 *
 * Copy one of the fd in place of the deleted one to prevent the
 * re-indexing of our list.
 */

void Server::delConnection( size_t cid ) {
  close( _pfds[cid].fd );

  _pfds[cid] = _pfds.back();
  _pfds.pop_back();

  _clients[cid] = _clients.back();
  _clients.pop_back();

  std::cout << "<" << _clients[cid].getName() << " left the channel>\n";
}

/**
 * @brief       Broadcast a message to all clients
 */

void Server::broadcastMsg( std::string& msg, size_t cid ) {
  int dest;

  msg = _clients[cid].getName() + ": " + msg + "\n";
  for( size_t i = 0; i < _pfds.size(); ++i ) {
    dest = _pfds[i].fd;
    if( dest != _listener.getSocket() && dest != _pfds[cid].fd ) {
      if( send( dest, msg.c_str(), msg.length(), 0 ) == -1 ) {
        std::cerr << "send: " << strerror( errno ) << "\n";
      }
    }
  }
  std::cout << msg;
}

void Server::parseData( const char* data, size_t cid ) {
  std::string msg( data );

  if( msg == "/shutdown" ) {
    shutdown();
  } else if( msg == "/quit" ) {
    delConnection( cid );
  } else if( msg.substr( 0, 6 ) == "/name " ) {
    std::cout << "<" << _clients[cid].getName();
    _clients[cid].setName( msg.substr( 6 ) );
    std::cout << " became " << _clients[cid].getName() << ">\n";
  } else {
    broadcastMsg( msg, cid );
  }
}

void Server::receiveData( size_t cid ) {
  char buf[256];  // Buffer for client data
  long nbytes = recv( _pfds[cid].fd, buf, sizeof( buf ), 0 );

  if( nbytes <= 0 ) {
    std::cout << "del connection" << std::endl;
    if( nbytes == 0 ) {
      std::cout << "server: socket " << _pfds[cid].fd << " hung up\n";
    } else {
      std::cerr << "recv: " << strerror( errno ) << "\n";
    }
    delConnection( cid );
    return;
  }
  // Turn "^M\n" into "\0" TODO OS compatibility
  buf[nbytes - 2] = '\0';
  parseData( buf, cid );
}

void Server::addConnection() {
  int                     newsocket;   // Newly accept()ed socket descriptor
  pollfd                  pfd;         // New pollfd for new connection
  struct sockaddr_storage remoteAddr;  // Client address
  socklen_t               remoteAddrLen;

  remoteAddrLen = sizeof( remoteAddr );
  newsocket = accept( _listener.getSocket(),
                      reinterpret_cast<struct sockaddr*>( &remoteAddr ),
                      &remoteAddrLen );
  if( newsocket == -1 ) {
    std::cerr << "accept: " << strerror( errno ) << "\n";
    return;
  }

  // IPv6 TEST
  /* sockaddr->sin6_family = AF_INET6; */
  /* sockaddr->sin6_addr.s6_addr[15] = 1; */
  /* std::string ipAddress = ntop( socket ); */
  /* std::cout << "IPv6 Address: " << ipAddress << std::endl; */

  // CREATE POLLFD
  pfd.fd = newsocket;
  pfd.events = POLLIN;  // Check ready-to-read
  pfd.revents = 0;      // prevent conditional jump in run()
  _pfds.push_back( pfd );

  if( _clients.size() == 0 ) {
    _clients.push_back( Client( "NONE" ) );
  }
  _clients.push_back( Client( "Anon_0" + intToString( newsocket ) ) );

  std::cout << "<Anon_0" << newsocket << " joined the channel>\n";
  /* std::cout << "pollserver: new connection from "; */
  /* std::cout << ntop( remoteAddr ); */
  /* std::cout << " on socket " << newsocket << "\n"; */
  return;
}

/**
 * @brief       Create the listener socket
 */

void Server::setup() {
  pollfd pfd;

  if( _listener.createListenerSocket(this->_port) == -1 ) {
    std::cerr << "error creating listening socket\n";
    exit( 1 );
  }
  pfd.fd = _listener.getSocket();
 // std::cout << "pfd.fd: " << pfd.fd << std::endl;
  pfd.events = POLLIN;  // Report read to read on incoming connection
  pfd.revents = 0;      // Report read to read on incoming connection
  _pfds.push_back( pfd );
  // connection
}

/**
 * @brief       Server main loop
 */

void Server::run() {
  while( true ) {
    // pfds is a vector that contains all our pollfd structs that contains all
    // our socket
    // - the first one is the listening socket that is bind()ed to 4242 port so
    //   clients trying to connect to it can be detected. So the listener can
    //   only accept / deny detected incoming connections
    // - the other pfds are the connections that has been detected and accepted:
    //   our clients, that can thus send / recv data
    // - poll monitors each pollfd structs stored in pfds and handle the POLLIN
    // event, so we know that if the POLLIN event occur on the listener socket
    // it will be for a new connection otherwise if it occur on other socket it
    // will be for send / recv

    if( poll( _pfds.data(), _pfds.size(), -1 ) < 0 ) {
      std::cerr << "poll: " << strerror( errno ) << "\n";
      return;
    }
    if( _pfds[0].revents & POLLIN ) {
     // std::cout << "trying a new connection" << std::endl;     
      addConnection();
    }
    else
    {
    //  std::cout << "rentre dans le else" << std::endl;
      for( size_t cid = 1; cid < _pfds.size(); ++cid ) 
      {
      //  std::cout << "cid: " << cid << std::endl;
        if( _pfds[cid].revents & POLLIN ) {
      //    std::cout << "test received data" << std::endl;
          receiveData( cid );
        }
      }
    }
  }
}
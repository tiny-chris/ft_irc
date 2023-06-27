/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmelard <lmelard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/22 14:40:23 by lmelard           #+#    #+#             */
/*   Updated: 2023/06/27 11:44:16 by lmelard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Socket.hpp"
#include "Utils.hpp"
#include "Commands.hpp"

/* ----------------------- CONSTRUCTORS & DESTRUCTOR ------------------------ */

Server::Server( size_t port, const char *password ) : _port(port), _password(password) { 
 // std::cout << _password << std::endl;
  setup();
}

Server::~Server( void ) { shutdown(); }

/* ---------------------- MEMBER FUNCTIONS: ACCESSORS ----------------------- */

size_t      Server::getPort( void ) const { return _port; }
std::string Server::getPassword( void ) const { return _password; }

void        Server::setPort( size_t& port ) { _port = port; }
void        Server::setPassword( std::string& password ) { _password = password; }

/* -------------------- MEMBER FUNCTIONS: TO BE DEFINED --------------------- */

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

/**
 * @brief       Handle request by identifying command and parameters
 * 
 *      A REVOIR CAR BOUCLE PAS CONSEILLEE (plutot while (std::getline(iss, line))...)
 *      MAIS PAS PROTEGE CORRECTEMENT... A PREVOIR !!!!!!!    
 */
// void  Server::handleRequest( Client& client, char *buffer )
void  Server::handleRequest( size_t cid, char *buffer )
{
  // int                 clientFdSocket = _clients[cid].getCfd();
  std::string         msg(buffer);//code de Clem pour la fin
  std::istringstream  iss(buffer);

  while (!iss.eof())//
  {
    size_t      splitter;
    std::string line;
    std::string command;
    std::string parameters = "";

    std::getline(iss, line);
    if (line.empty())
      continue;
    splitter = line.find(' ', 0);
    if (splitter == std::string::npos)
      command = line;
    else
    {
      command = line.substr(0, splitter);
      parameters = line.substr(splitter + 1, std::string::npos);
    }
    
    /* TODO:  prévoir le cas où le client est a été déconnecté et breaker */

    /* TODO: faire un check sur le nom de la commande : 
              --> voir si elle fait partie de la liste des commandes */

    /* TODO:  il faut que PASS soit la premiere commande prise en compte !
              --> passer (continue;) si la commande indiquee n'est pas PASS 
                  ET si le mot de passe n'est pas valide
        "de-commenter" le code deja pret mais non pertinent sans la fonction handlePASS */
    // if (command != "PASS" && !_clients[cid].getPassStatus())
    //   continue;

    /*  TODO: verifier s'il s'agit d'une des commandes d'authentification (PASS, NICK, USER)
              et si NON, s'assurer que les 3 commandes precedentes ont bien ete completees
                avant de prendre en compte d'autres commande... */
    // if (client.getIfResgistered() || command == "NICK" || command == "PASS" || command == "")
    //    "rentrer dans les differentes commandes"
    if (command == "PASS")
    {
      std::cout << "mettre la fonction pour PASS" << std::endl;
      handlePass(_clients[cid], parameters, _password);
    }
    
    /*  Reprise du code de Clement afin de pouvoir quitter et faire qqs manip... */
    else
    {
      if ( command == "/shutdown" ) {
        shutdown();
      } else if ( command == "/quit" ) {
        delConnection( cid );
      } else if (command == "/name") {
        std::cout << "<" << _clients[cid].getName();
        _clients[cid].setName( msg.substr( 6 ) );
        std::cout << " became " << _clients[cid].getName() << ">\n";
      } else {
        broadcastMsg( msg, cid );
      }
      // std::cout << "si pas PASS: on n'a pas encore gere les autres commandes" << std::endl;
    }
  }
}

void Server::receiveData( size_t cid ) {
  char buf[256];  // Buffer for client data
  memset(buf, 0, sizeof(buf));
  long nbytes = 0;
  while (1)
  {
    memset(buf, 0, sizeof(buf));
    nbytes = recv( _pfds[cid].fd, buf, sizeof( buf ) - 1, 0 );
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
    //faire un pour verifier que ca finit bien par un 
    if (nbytes >= 2 && buf[nbytes - 2] == '\r' && buf[nbytes - 1] == '\n')
    {
      buf[nbytes - 2] = '\0';
      buf[nbytes - 1] = '\0';
      break;
    }
  }
  std::cout << "display buffer content (with potential \\r\\n) : " << buf << std::endl;
  // parseData( buf, cid );
  handleRequest( cid, buf );
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

  // If there is no client --> create a first client "NONE" to start the client counting at 1
  if( _clients.size() == 0 ) {
    _clients.push_back( Client( "NONE", -1 ) );
  }
  _clients.push_back( Client( "Anon_0" + intToString( newsocket ), newsocket ) );

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
          // std::cout << "test received data" << std::endl;
          receiveData( cid );
        }
      }
    }
  }
}
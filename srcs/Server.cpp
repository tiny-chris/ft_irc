/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmelard <lmelard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/22 14:40:23 by lmelard           #+#    #+#             */
/*   Updated: 2023/07/03 15:00:08 by lmelard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Socket.hpp"
#include "utils.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"

/* ----------------------- CONSTRUCTORS & DESTRUCTOR ------------------------ */

Server::Server( size_t port, const char *password, std::string serverName ) : _port(port), _password(password), _serverName(serverName) {
 // std::cout << _password << std::endl;
  // setSource("", "");
  setup();
  initCommands();
}

Server::~Server( void ) { shutdown(); }

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

  std::cout << "<" << _clients[cid].getRealname() << " left the channel>\n";
}

/**
 * @brief       Broadcast a message to all clients
 */

void Server::broadcastMsg( std::string& msg, size_t cid ) {
  int dest;

  msg = _clients[cid].getRealname() + ": " + msg + "\n";
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

/**
 * @brief       Initialize the commands to be handled in IRC into a map
 */

void	Server::initCommands( void )
{
  _mapCommands.insert(std::make_pair(0, "UNDEFINED"));
  _mapCommands.insert(std::make_pair(100, "PASS"));
  _mapCommands.insert(std::make_pair(101, "NICK"));
  _mapCommands.insert(std::make_pair(102, "USER"));
  _mapCommands.insert(std::make_pair(103, "JOIN"));
  _mapCommands.insert(std::make_pair(104, "PRIVMSG"));

  _mapCommands.insert(std::make_pair(1000, "/shutdown"));
  _mapCommands.insert(std::make_pair(1001, "/quit"));
  _mapCommands.insert(std::make_pair(1002, "/name"));
  _mapCommands.insert(std::make_pair(1003, "/msg"));

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
 */

void  Server::handleRequest( size_t cid, std::string request )
{
  std::string command;
  std::string parameters = "";
  std::string reply;
  size_t      splitter;
  int         commandKey = 0;

  splitter = request.find(' ', 0);

  /* ********************************* */
  /* ACTION 1   - get command & params */
  /* ********************************* */
  // if <request> has no space --> command is request string
  //    should also take into account the empty message (initially only '\r\n' and empty after receiveData())
  // else --> split command & parameters
  if (splitter == std::string::npos)
      command = request;
  else
  {
    command = request.substr(0, splitter);
    parameters = request.substr(splitter + 1, std::string::npos);
  }

  /* ********************************* */
  /* ACTION 2   - check the case when the client is disconnected and return */
  /* ********************************* */
  //   /* TODO:  prévoir le cas où le client est a été déconnecté et breaker */


  /* ********************************* */
  /* ACTION 3   - check if 'command' is part of the _mapCommands and get the command key */
  /* ********************************* */
  for (std::map<int, std::string>::const_iterator it = _mapCommands.begin(); it != _mapCommands.end(); ++it)
  {
    if (command == it->second)
      commandKey = it->first;
  }

  // // display to check:
  // std::cout << "command = " << command << std::endl;
  // std::cout << "param = " << parameters << std::endl;
  // std::cout << "commandKey = " << commandKey << std::endl;

  /* ********************************* */
  /* ACTION 4   - if 'command' is in _mapCommands */
  /* ********************************* */
  //
  //  SUB-ACTION 4.1   - 'PASS' must be the first command entered, if not --> return (numeric_reply ??)
  //  SUB-ACTION 4.2   - if command is NOT an authentifier (PASS, NICK, USER) or QUIT and if client is not registered yet --> ERR_NOTREGISTERED
  //
  //    ???? QUESTION ????  pour 'quit' --> doit-on etre registered ? idem pour potentiel shutdown
  //
  if (commandKey != UNDEFINED)
  {
    if (command != "PASS" && _clients[cid].getPassStatus() == false )
      return;

    if (!_clients[cid].getIfRegistered()
      && !(command == "PASS" || command == "NICK" || command == "USER" || command == "QUIT"))
    {
      reply = ERR_NOTREGISTERED( _serverName,_clients[cid].getNickname() );
      std::cout << "print reply: " << reply << std::endl; // to del
      send(_clients[cid].getCfd(), reply.c_str(), reply.length(), 0);
      return;
    }
  }

  /* ********************************* */
  /* ACTION 5   - handle command */
  /* ********************************* */
  switch(commandKey)
  {
    // case PASS:        handlePass( cid, parameters ); break;
    // case NICK:        handleNick( cid, parameters ); break;
    // case PASS:        handleUser( cid, parameters ); break;
    case PASS:        {
                        std::cout << "client " << _clients[cid].getNickname() << " - use function to handle PASS command" << std::endl;
                        handlePass( cid, parameters );
                      } break;
    case NICK:        {
                        std::cout << "client " << _clients[cid].getNickname() << " - use function to handle NICK command with " << parameters << std::endl;
                        handleNick( cid, parameters );
                      } break;
    case USER:        {
                        std::cout << "client " << _clients[cid].getNickname() << " - use function to handle USER command" << std::endl;
                        handleUser( cid, parameters );
                      } break;
    case JOIN:        std::cout << "client " << _clients[cid].getNickname() << " - use function to handle JOIN command" << std::endl; break;
    case PRIVMSG:     std::cout << "client " << _clients[cid].getNickname() << " - use function to handle PRIVMSG command" << std::endl; break;


    // keeping Clement's initial commands just in case... - START
    case ZZ_SHUTDOWN: shutdown(); break;                          // ' /shutdown '
    case ZZ_QUIT:     delConnection( cid ); break;                // ' /quit '
    case ZZ_NAME:     {                                           // ' /name <new_name>'
                        std::cout << "<" << _clients[cid].getRealname();
                        _clients[cid].setRealname( parameters );
                        std::cout << " became " << _clients[cid].getRealname() << ">\n";
                      } break;
    case ZZ_MSG:      broadcastMsg( parameters, cid ); break;        // ' /msg <message to broadcast>'
    // keeping Clement's initial commands just in case... - END

    default:        	{
                        reply = ERR_UNKNOWNCOMMAND( _serverName, _clients[cid].getRealname(), command );
                        std::cout << "print reply: " << reply << std::endl; // to del
                        send(_clients[cid].getCfd(), reply.c_str(), reply.length(), 0);
                        return;
                      } break;
  }
}

/*  Peut etre penser a enlever le '\n' tout seul ?? ou quid de '\r' sans le '\n' ? */

void Server::receiveData( size_t cid ) {
  char buf[BUFMAXLEN];  // Buffer for client data
  static std::string bufs[MAXCONNECTION + 1];
  memset(buf, 0, sizeof(buf));
  long nbytes = 0;
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
  bufs[cid] += buf;
  int size = bufs[cid].size();
  if (size >= 2 && bufs[cid][size - 2] == '\r' && bufs[cid][size - 1] == '\n')
  {
    // std::cout << "display bufs[cid] before deleting \\r\\n : '" << bufs[cid] << "'" << std::endl;
    // bufs[cid][size - 2] = '\0';
    // bufs[cid][size - 1] = '\0';
    bufs[cid].erase(size - 1, 1);
    bufs[cid].erase(size - 2, 1);
    // std::cout << "display bufs[cid] after deleting \\r\\n : '" << bufs[cid] << "'" << std::endl;
    handleRequest( cid, bufs[cid] );
    bufs[cid].clear();
    // break;
  }
  //}
  // parseData( buf, cid );
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

  std::cout << "<Anon_0" << newsocket << " is connectedl>\n";//new/
  // std::cout << "<Anon_0" << newsocket << " joined the channel>\n";
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


/*  ******************************************************************** */
/*  ***** following functions should be put in the Commands folder ***** */
/*  ******************************************************************** */

/* Peut etre mettre des comments dans le serveur (pour indiquer par exemple que le mot de passe est correct....)*/

void	Server::handlePass( size_t cid, std::string param )
{
	std::string	reply;
	// checking if the Client is already registered
	// meaning checking if PASS, NICK, USER are already set
	// if not ERR_ALREADYREGISTERED numeric reply is sent
	if (_clients[cid].getIfRegistered() == true)
	{
		// ERR_ALREADYREGISTERED numeric reply is sent
		reply = ERR_ALREADYREGISTRED(_clients[cid].getSource(), _clients[cid].getNickname());
		std::cout << "print reply: " << reply << std::endl; // to del 
		send(_clients[cid].getCfd(), reply.c_str(), reply.length(), 0);
	}
	// else if there is no param to the PASS command
	// ERR_NEEDMOREPARAMS numeric reply is sent
	else if (param.compare("") == 0)
	{
		std::string command = "pass";
		reply = ERR_NEEDMOREPARAMS (_clients[cid].getSource(), _clients[cid].getNickname(), command);
		std::cout << "print reply: " << reply << std::endl; // to del
		send(_clients[cid].getCfd(), reply.c_str(), reply.length(), 0);
	}
	// else if Pass command's param is different from the password set for the Server
	// then ERR_PASSDMISMATCH error is sent and Client is killed et disconnected
	else if (param.compare(_password) != 0 || param.size() != _password.size())
	{
		reply = ERR_PASSWDMISMATCH(_clients[cid].getSource(), _clients[cid].getNickname());
		std::cout << "print reply: " << reply << std::endl; // to del
		send(_clients[cid].getCfd(), reply.c_str(), reply.length(), 0);
		reply = KILL_MSG(_clients[cid].getSource(), _clients[cid].getNickname());
    send(_clients[cid].getCfd(), reply.c_str(), reply.length(), 0);
   	this->delConnection( cid );
	}
	// else if it's the right password, the client is not yet registered then setPassStatus to true
	else
		_clients[cid].setPassStatus(true);
	return ;
}

void		Server::handleNick( size_t cid, std::string param )
{
  std::string reply;
  // if no param to a nick command -> No nickname given error
  if (param.compare("") == 0)
  {
    reply = ERR_NONICKNAMEGIVEN(_clients[cid].getSource(), _clients[cid].getNickname());
    // reply = ERR_NOSUCHNICK(SOURCE(_clients[cid].getNickname(), _clients[cid].getUsername()), _clients[cid].getNickname());
    send(_clients[cid].getCfd(), reply.c_str(), reply.length(), 0);
  }
  // else if there are invalid char in the nickname, an erroneus nickname message is sent
  else if (isValidNick(param) == false)
  {
    reply = ERR_ERRONEUSNICKNAME(_clients[cid].getSource(), _clients[cid].getNickname(), param);
    send(_clients[cid].getCfd(), reply.c_str(), reply.length(), 0);
  }
  // else if the nickname is the same nickname as another client
  else if (existingNick(param) == true)
  {
    // and if our client is not yet registered then an nick collision occurs and the client is killed
    if (_clients[cid].getIfRegistered() == false)
    {
      reply = ERR_NICKCOLLISION(_clients[cid].getSource(), _clients[cid].getNickname(), param);
      send(_clients[cid].getCfd(), reply.c_str(), reply.length(), 0);
      reply = KILL_MSG(_clients[cid].getSource(), _clients[cid].getNickname());
      send(_clients[cid].getCfd(), reply.c_str(), reply.length(), 0);
      this->delConnection( cid );
    }
    // but if our client is already registered then a nickname in use error occurs
    // the client keeps its nickname
    else
    {
      reply = ERR_NICKNAMEINUSE(_clients[cid].getSource(), _clients[cid].getNickname(), param);
      send(_clients[cid].getCfd(), reply.c_str(), reply.length(), 0);
    }
  }
  // else meaning if the nickname given is valid and does not already exist
  // nickname is set to param value and nickstatus set to true
  else
  {
    reply = _clients[cid].getSource() + _clients[cid].getNickname() + " NICK " + param + "\r\n";
    send(_clients[cid].getCfd(), reply.c_str(), reply.length(), 0);
    _clients[cid].setNickStatus(true);
    _clients[cid].setNickname(param);
    if (_clients[cid].getIfRegistered() == true)
      _clients[cid].setSource( param, _clients[cid].getUsername());
  }
  return ;
}

bool		Server::isValidNick(std::string param)
{
  for (size_t i = 0; i < param.size(); i++)
  {
    char c = param[i];
    if (!(isalnum(c) || c == '[' || c == ']' || c == '{' \
      || c == '}' || c == '\\' || c == '|'))
      return false;
  }
  return true;
}

bool  Server::existingNick(std::string param)
{
  for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
  {
    if (it->getNickname().compare(param) == 0 && it->getNickname().size() == param.size())
      return true;
  }
  return false;
}

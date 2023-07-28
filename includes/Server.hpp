/* includes/Server */
/* Created: 230725 10:02:18 by clem@spectre */
/* Updated: 230725 10:02:18 by clem@spectre */
/* Maintainer: Clément Vidon */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <ctime>
#include <fstream>
#include <iosfwd>
#include <map>
#include <string>
#include <vector>

#include "Channel.hpp"
#include "Client.hpp"

#define MAX_EVENTS 10  // TODO

/**
 * @brief       Handles the communication between multiple clients, managing
 *              client connections, receiving and sending data, and handling
 *              disconnections.
 */
class Client;
class Channel;

class Server {

 public:

  typedef std::map<int, Client>          mapClients;
  typedef std::map<std::string, Channel> mapChannels;
  typedef std::map<int, std::string>     mapCommands;
  typedef std::vector< std::string >     vecString;

  void checkRegistration( int clientSocket );
  void sendWelcomeMsg( int clientSocket );
  void sendLusersMsg( int clientSocket );
  void sendMotdMsg( int clientSocket );

  // TODO URGENCE -> Create class Commands.

  /* ********* COMMANDS ******* */

  /* ********* PASS CMD ******* */
  void handlePass( int clientSocket, std::string param );
  /* ********* NICK CMD ******* */
  void handleNick( int clientSocket, std::string param );
  bool isValidNick( std::string param );
  bool existingNick( std::string param );
  /* ********* USER CMD ******* */
  void handleUser( int clientSocket, std::string param );
  /* ********* PING CMD ******* */
  void handlePing( int clientSocket, std::string param );
  /* ********* NAMES CMD ******* */
  void handleNames( int clientSocket, std::string param );
  void displayNames( int clientSocket, Channel& channel );
  /* ********* JOIN CMD ******* */
  void handleJoin( int clientSocket, std::string param );
  bool handleJoinZero( int clientSocket, const std::vector< std::string >& tokens );
  bool checkChanPreJoin( int clientSocket, const std::vector< std::string >& tokens,
                        size_t index );
  bool isValidChanName( int clientSocket, const std::string& channelName );
  void createChanWithOp( int clientSocket, const std::string& channelName );
  /* ********* PART CMD ******* */
  void handlePart( int clientSocket, std::string param );
  void leaveChannel ( int clientSocket, const std::string& channelName,
                     const std::string& reason, const std::string& cmd );
  bool checkChanPrePart( int clientSocket, const std::string& channelName );

  void handlePrivmsg( int clientSocket, std::string param );
  int  findClientFd(const std::string& name );
  /* ********* MODE CMD ******* */
  void handleMode( int clientSocket, std::string param );
  void handleUserMode( int clientSocket, std::vector<std::string>& tokens );
  void handleChannelMode( int clientSocket, std::string& channelName,
                          const std::vector<std::string>& tokens );
  bool existingChannel( std::string param );
  char getModePrefix( std::string const& token );
  /* ********* KICK CMD ******* */
  void        handleKick( int clientSocket, std::string param );
  void        kickUser( int clientSocket, Channel* chan, std::string nick,
                        std::string toKick, std::string reason );
  std::string getReason( std::vector<std::string>& tokens );
  /* ********* TOPIC CMD ******* */
  void        handleTopic( int clientSocket, std::string param );
  std::string getNewTopic( std::vector<std::string>& tokens );
  /* ********* INVITE CMD ******* */
  void handleInvite( int clientSocket, std::string param );
  void inviteClientToChannel( int clientSocket, std::string clientNick,
                              std::string nameInvitee, Channel* chan );
  /* ********* WHO CMD ******* */
  void        handleWho( int clientSocket, std::string param );
  /* ********* QUIT CMD ******* */
  void	      handleQuit( int clientSocket, std::string param );
  /* ********* SQUIT CMD ******* */
  void  handleSQuit( int clientSocket, std::string param );
  /* ********* KILL CMD ******* */
  void	handleKill( int clientSocket, std::string param );


  std::string	getSupportToken() const;

  void channelMsgToAll( int clientSocket, const std::string& channelName,
                        const std::string& message );
  void channelMsgNotClient( int clientSocket, const std::string& channelName,
                            const std::string& message );
  void channelMsgToChanOps( int clientSocket, const std::string& channelName,
                            const std::string& message );

 public: // pk y a ecrit public plusieurs fois ?
  void updateChannelMemberNick( std::string& oldNickname,
                                std::string  nickName );
  void updateChannelOpsNick( std::string& oldNickname, std::string nickName );
  void updateInvitedMembersNick( std::string &oldNickname, std::string nickName );

  //
  //
  //
  //
  //
  //
  // SERVER

 public:
  Server( void );
  Server( size_t port, const char* password, std::string serverName );
  Server( Server const& src );
  ~Server( void );
  Server&      operator=( Server const& rhs );
  virtual void print( std::ostream& o ) const;

  void        setPort( size_t& port );
  size_t      getPort( void ) const;
  void        setPassword( std::string& password );
  std::string getPassword( void ) const;

 private:
  void stop( void );

  void removeDisconnectedClients( void );
  void disconnectAllClients();
  void disconnectAClient( int clientSocket );

  // void broadcastMsg( std::string& msg, int clientSocket );
  void broadcastMsgToAll( int clientSocket, const std::string& message );
  void broadcastMsgNotClient( int clientSocket, const std::string& message );
  void replyMsg( int clientSocket, std::string reply, bool copyToServer = 1 );
  void handleRequest( int clientSocket, std::string request );
  void handleExistingClient( int clientSocket );
  void handleNewClient( void );
  void createServerSocket( void );
  void initCommands( void );  // TODO --->>> class Command constructor
  bool        existingClient(std::string clientName);

 public:
  void start( void );

 private:
  size_t      _port;
  std::string _password;
  std::string _serverName;

  int _serverSocket;
  int _epollFd;

  mapClients       _clients;
  std::vector<int> _disconnectedClients;

  mapChannels _channels;
  mapCommands _commands;
};

std::ostream& operator<<( std::ostream& o, Server const& i );

#endif /* __SERVER_HPP__ */

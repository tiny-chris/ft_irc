/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmelard <lmelard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by 2.fr>             #+#    #+#             */
/*   Updated: 2023/08/02 10:24:45 by lmelard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
  typedef std::vector<std::string>       vecString;
  typedef std::map<std::string, Client*> mapClientsPtr;

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
  Client*     getClientByNickname( const std::string& nickname );

 private:
  void stop( void );
  
  void removeDisconnectedClients( void );
  void disconnectAllClients();
  void disconnectAClient( int clientSocket );

  /* ********* REGISTRATION ******* */
  std::string getOpsNbr( void );
  std::string getChannelNbr( void );
  std::string getUnknownStateUsers( void );
  std::string getInvisibleUserNbr( void );
  std::string getSupportToken() const;
  void        sendWelcomeMsg( int clientSocket );
  void        sendLusersMsg( int clientSocket );
  void        sendMotdMsg( int clientSocket );
  void        checkRegistration( int clientSocket );

  /* ********* COMMANDS ******* */

  /* ********* PASS CMD ******* */
  void handlePass( int clientSocket, std::string param );
  /* ********* NICK CMD ******* */
  bool isValidNick( std::string param );
  void updateChannelMemberNick( std::string& oldNickname,
                                std::string  nickName );
  void updateChannelOpsNick( std::string& oldNickname, std::string nickName );
  void updateInvitedMembersNick( std::string& oldNickname,
                                 std::string  nickName );
  void updateNickInChannels( int clientSocket, std::string nick,
                             std::string newNick );
  void updateNickname( int clientSocket, std::string nick,
                       std::string newNick );
  void handleExistingNick( int clientSocket, std::string newNick );
  void handleNick( int clientSocket, std::string param );
  /* ********* USER CMD ******* */
  void handleUser( int clientSocket, std::string param );
  /* ********* PING CMD ******* */
  void handlePing( int clientSocket, std::string param );
  /* ********* NAMES CMD ******* */
  void handleNames( int clientSocket, std::string param );
  void displayNames( int clientSocket, Channel& channel );
  /* ********* JOIN CMD ******* */
  void handleJoin( int clientSocket, std::string param );
  bool handleJoinZero( int                             clientSocket,
                       const std::vector<std::string>& tokens );
  bool checkChanPreJoin( int                             clientSocket,
                         const std::vector<std::string>& tokens, size_t index );
  bool isValidChanName( int clientSocket, const std::string& channelName );
  void createChanWithOp( int clientSocket, const std::string& channelName );
  bool checkParamSize( int clientSocket, const std::string& param,
                       const std::vector<std::string>& tokens );
  /* ********* PART CMD ******* */
  void handlePart( int clientSocket, std::string param );
  void leaveChannel( int clientSocket, const std::string& channelName,
                     const std::string& reason, const std::string& cmd );
  bool checkChanPrePart( int clientSocket, const std::string& channelName );
  /* ********* PRIVMSG CMD ******* */
  bool checkTargetPrivmsg( int clientSocket, std::string& target );
  void handlePrivmsg( int clientSocket, std::string param );
  /* ********* MODE CMD ******* */
  void displayUserModeChanges( Client*                         client,
                               const std::vector<std::string>& tokens,
                               std::string&                    modechange );
  void updateUserMode( Client* client, const std::vector<std::string>& tokens,
                       std::string& modechange );
  void handleUserMode( int clientSocket, std::vector<std::string>& tokens );
  void handleChannelMode( int clientSocket, std::string& channelName,
                          const std::vector<std::string>& tokens );
  void handleMode( int clientSocket, std::string param );
  /* ********* KICK CMD ******* */
  std::string getKickReason( std::vector<std::string>& tokens );
  void        kickUser( int clientSocket, Channel* chan, std::string nick,
                        std::string toKick, std::string reason );
  void        kickSelectedClients( int clientSocket, Channel* chan,
                                   std::vector<std::string> toKick,
                                   std::string              reason );
  void        handleKick( int clientSocket, std::string param );
  /* ********* TOPIC CMD ******* */
  void        sendTopicToChannelMembers( Channel* chan );
  void        updateChannelTopic( Channel* chan, std::string newTopic,
                                  std::string nick );
  std::string getNewTopic( std::vector<std::string>& tokens );
  void        handleTopicDisplay( int clientSocket, Channel* chan );
  void        handleTopic( int clientSocket, std::string param );
  /* ********* INVITE CMD ******* */
  void inviteClientToChannel( int clientSocket, std::string clientNick,
                              std::string nameInvitee, Channel* chan );
  void handleInvite( int clientSocket, std::string param );
  /* ********* WHO CMD ******* */
  void handleWhoChannel( int clientSocket, std::string source, std::string nick,
                         std::string target );
  void handleWhoClient( int clientSocket, std::string source, std::string nick,
                        std::string target );
  void handleWho( int clientSocket, std::string param );
  /* ********* QUIT CMD ******* */
  void handleQuit( int clientSocket, std::string param );
  /* ********* SQUIT CMD ******* */
  void handleSQuit( int clientSocket, std::string param );
  /* ********* KILL CMD ******* */
  void handleKill( int clientSocket, std::string param );
  bool checkChanPreKill( int clientSocket, const std::string& param,
                         const std::string& nickToKill,
                         const std::string& comment );

  /* ********* PRIVATE MEMBER FUNCTIONS ******* */
  void channelMsgToAll( int clientSocket, const std::string& channelName,
                        const std::string& message );
  void channelMsgNotClient( int clientSocket, const std::string& channelName,
                            const std::string& message );
  void channelMsgToChanOps( int clientSocket, const std::string& channelName,
                            const std::string& message );
  int  findClientFd( const std::string& name );
  bool existingNick( std::string param );
  bool existingChannel( std::string param );
  void changeChannelOperator( int clientSocket, Client* toLeave,
                              Channel* chan );

  void broadcastQuitToAll( void );
  void broadcastMsgToAll( int clientSocket, const std::string& message );
  void broadcastMsgNotClient( int clientSocket, const std::string& message );
  void replyMsg( int clientSocket, std::string reply, bool copyToServer = 1 );
  void handleCommand( int clientSocket, int key, const std::string& command, 
                      const std::string& parameters );
  void handleRequest( int clientSocket, std::string request );
  void handleExistingClient( int clientSocket );
  void nonBlockingSocket( int &socket );
  void handleNewClient( void );
  void createServerSocket( void );
  void initCommands( void );
  bool existingClient( std::string clientName );

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

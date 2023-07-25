/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/22 14:39:05 by lmelard           #+#    #+#             */
/*   Updated: 2023/07/24 16:13:35 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <cerrno>    // errno
# include <cstring>   // strerror
# include <iostream>  // cerr, cout
# include <fstream>
# include <sstream>   // stringstream
# include <string>    // string
# include <vector>    // vector
# include <map>			// map

# include <arpa/inet.h>  // inet_ntoa
# include <netdb.h>  // recv, send, sockaddr, accept, addrinfo, getaddrinfo, socket, setsockopt, bind, freeaddrinfo, listen
# include <poll.h>   // pollfd, poll
# include <unistd.h>  // close
# include <ctime>

# include "Client.hpp"
# include "Socket.hpp"
# include "Channel.hpp"

class Client;
class Channel;
class Socket;

class Server {

  public:

    typedef std::map< int, Client >						mapClients;
		typedef std::map< std::string, Channel >	mapChannels;
    typedef std::map< int, std::string >			mapCommands;

    void				checkRegistration( int clientSocket );
    void				sendWelcomeMsg( int clientSocket );
    void				sendLusersMsg( int clientSocket );
    void        sendMotdMsg( int clientSocket );


    /*** COMMANDS ***/

    void				handlePass( int clientSocket, std::string param );
    void				handleNick( int clientSocket, std::string param );
    bool				isValidNick( std::string param );
    bool				existingNick( std::string param );
    void				handleUser( int clientSocket, std::string param );
    void				handlePing( int clientSocket, std::string param );

    void				handleNames( int clientSocket, std::string param );
    void				displayNames( int clientSocket, Channel& channel );

    void				handleJoin( int clientSocket, std::string cmd, std::string param );
    bool				isValidChanName( int clientSocket, std::string channelName );
    // bool				validChannelNames( int clientSocket, std::vector<std::string>& channelNames );
    bool        checkPreAddChan( int clientSocket, std::vector< std::string > tokens, size_t index, bool chanExists );

    void				handlePart( int clientSocket, std::string param );
    void				leaveChannel ( int clientSocket, std::string channelName, std::string reason );
    bool				checkPrePartChan( int clientSocket, std::string channelName );

    void				handleMode( int clientSocket, std::string param );
    void				handleUserMode (int clientSocket, std::vector<std::string> &tokens );
    void				handleChannelMode (int clientSocket, std::string& channelName, const std::vector<std::string> &tokens );
    bool				existingChannel( std::string param );
    void				handleChannelModeSet(Channel *chan, char modeChar, std::string* modeArgs, std::string* modeChange, const std::vector<std::string>& tokens, size_t *j);
    void        handleChannelModeUnset(Channel *chan, char modeChar, std::string* modeArgs, std::string* modeChange, const std::vector<std::string>& tokens, size_t *j);

    void				handleModeSetKey(Channel *chan, std::string* modeArgs, std::string* modeChange, const std::vector<std::string> &tokens, size_t *j);
    void				handleModeSetLimit(Channel *chan, std::string* modeArgs, std::string* modeChange, const std::vector<std::string> &tokens, size_t *j);
    void				handleModeSetInviteOnly(Channel *chan, std::string* modeChange);
    void				handleModeSetTopicRestriction(Channel *chan, std::string* modeChange);
    void        handleModeSetOperator(Channel *chan, std::string* modeArgs, std::string* modeChange, const std::vector<std::string> &tokens, size_t *j);

    void				handleModeUnsetKey(Channel *chan, std::string* modeChange);
    bool				isValidModeChar( char const modeChar );
    char				getModePrefix( std::string const& token );
    void				handleModeUnsetLimit(Channel *chan, std::string* modeChange);
    void				handleModeUnsetInviteOnly(Channel *chan, std::string* modeChange);
    void				handleModeUnsetTopicRestriction(Channel *chan, std::string* modeChange);
    void        handleModeUnsetOperator(Channel *chan, std::string* modeArgs, std::string* modeChange, const std::vector<std::string> &tokens, size_t *j);

    void        handleKick( int clientSocket, std::string param );
    void	      kickUser( int clientSocket, Channel *chan, std::string nick, std::string toKick, std::string reason );
    std::string	getReason( std::vector<std::string> &tokens );

    void		    handleTopic( int clientSocket, std::string param );
    std::string getNewTopic( std::vector<std::string> &tokens );

    void		    handleInvite( int clientSocket, std::string param );
    void        inviteClientToChannel( int clientSocket, std::string clientNick, std::string nameInvitee, Channel *chan );

    std::string	getSupportToken() const;

    void        channelMsgToAll( int clientSocket, std::string channelName, std::string message );
    void        channelMsgNotClient( int clientSocket, std::string channelName, std::string message );

  public:

    Server( size_t port, const char *password, std::string serverName );
    ~Server( void );

    void				start( void );
    virtual void print( std::ostream& o ) const;


  private:

    size_t			getPort( void ) const;
    std::string	getPassword( void ) const;
    void				setPort( size_t& port );
    void				setPassword( std::string& password );
    void				stop( void );
    void				removeDisconnectedClients( void );
    void				disconnectAClient( int clientSocket );
    void				disconnectAllClients();
    void				broadcastMsg( std::string& msg, int clientSocket );
    void				initCommands( void );
    void				replyMsg( int clientSocket, std::string reply, bool copyToServer = 1 );
    void				handleRequest( int clientSocket, std::string request );
    void				handleExistingClient( int clientSocket );
    void				handleNewClient( void );
		void				createServerSocket( void );
    void        updateChannelMemberNick( std::string &oldNickname, std::string nickName );
    void        updateChannelOpsNick( std::string &oldNickname, std::string nickName );


    size_t				  	_port;
    std::string				_password;
    std::string				_serverName;

    int								_serverSocket;
    int								_epollFd;

    std::vector<int>	_disconnectedClients;

    mapClients				_clients;
    mapChannels				_channels;
    mapCommands				_commands;
};

std::ostream& operator<<( std::ostream& o, Server const& i );


#endif /* __SERVER_HPP__ */

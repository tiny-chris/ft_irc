/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmelard <lmelard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/22 14:39:05 by lmelard           #+#    #+#             */
/*   Updated: 2023/07/03 14:58:07 by lmelard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <cerrno>    // errno
# include <cstring>   // strerror
# include <iostream>  // cerr, cout
# include <sstream>   // stringstream
# include <string>    // string
# include <vector>    // vector
# include <map>			// map

# include <arpa/inet.h>  // inet_ntoa
# include <netdb.h>  // recv, send, sockaddr, accept, addrinfo, getaddrinfo, socket, setsockopt, bind, freeaddrinfo, listen
# include <poll.h>   // pollfd, poll
# include <stdlib.h>  // exit
# include <unistd.h>  // close

# include "Client.hpp"
# include "Socket.hpp"

class Server {

	public:

		Server( size_t port, const char *password, std::string serverName );
		~Server( void );

		size_t		getPort( void ) const;
		std::string	getPassword( void ) const;
		// std::string getSource( void ) const;

		void		setPort( size_t& port );
		void		setPassword( std::string& password );
		// void		setSource( std::string nickname, std::string username );

		void		run();

		void		initCommands( void );
		void		handleRequest( size_t cid, std::string request );

		/*** COMMANDS ***/
		void		handlePass( size_t cid, std::string param );
		void		handleNick( size_t cid, std::string param );
		void		handleUser( size_t cid, std::string param );

	private:

		void shutdown( void );                              // close
		void delConnection( size_t cid );                   // close
		void broadcastMsg( std::string& msg, size_t cid );  // send
		// void parseData( const char* data, size_t cid );
		void receiveData( size_t cid );  // recv, senb
		void addConnection();            // accept
		void setup();

		// add a map of clients that contains their name, a struct of their
		// socket and a struct of their status
		std::vector<pollfd> _pfds;      // Pollable file descriptors
		std::vector<Client> _clients;   // Pollable file descriptors
		Socket              _listener;  // Use Socket for managing the listener socket

		std::map<int,std::string>	_mapCommands;

		size_t		_port;
		std::string	_password;
		std::string	_serverName; // a supprimer peut etre remplce par _source
		// std::string _source;

		bool		isValidNick(std::string param);
		bool		existingNick(std::string param);
};

#endif

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgaillag <cgaillag@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/22 14:39:05 by lmelard           #+#    #+#             */
/*   Updated: 2023/06/23 19:47:12 by cgaillag         ###   ########.fr       */
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

# include <arpa/inet.h>  // inet_ntoa
# include <netdb.h>  // recv, send, sockaddr, accept, addrinfo, getaddrinfo, socket, setsockopt, bind, freeaddrinfo, listen
# include <poll.h>   // pollfd, poll
# include <stdlib.h>  // exit
# include <unistd.h>  // close

# include "Client.hpp"
# include "Socket.hpp"

class Server {
	
	public:
	
		Server( size_t port, const char *password );
		~Server( void );

		size_t		getPort( void ) const;
		std::string	getPassword( void ) const;

		void		setPort( size_t& port );
		void		setPassword( std::string& password );

		void		run();

		// void		handleRequest( Client& client, char *buffer );
		void		handleRequest( size_t cid, char *buffer );

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

		size_t		_port;
		std::string	_password;
};

#endif
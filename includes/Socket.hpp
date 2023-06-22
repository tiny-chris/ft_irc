/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmelard <lmelard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/22 15:09:52 by lmelard           #+#    #+#             */
/*   Updated: 2023/06/22 15:26:35 by lmelard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
# define SOCKET_HPP

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

# define PORT "6667"

/**
 * @brief       Socket
 */

class Socket {
	public:
	
		Socket() : _sockfd( -1 ) {}
		~Socket() { closeSocket(); }

		int createListenerSocket( void );
		int getSocket( void ) { return _sockfd; }

	private:
	
		void closeSocket( void );  // useless?
		/* int accept( struct sockaddr* addr, socklen_t* addrlen ); */
		int _sockfd;
};

#endif
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgaillag <cgaillag@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/22 14:45:43 by lmelard           #+#    #+#             */
/*   Updated: 2023/06/23 15:33:26 by cgaillag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

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

class Client {

	public:
	
		Client( std::string name );
		~Client( void );

		// getters
		std::string&	getName( void );
		int&			getCid( void );
		bool&			getPassStatus( void );

		// setters
		void			setName( std::string const& name );
		void			setCid( int cid );
		void			setPassStatus( bool const& passwordStatus );

	private:
	
		std::string _name;
		int			_cid;
		bool		_passStatus;
};

#endif
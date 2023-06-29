/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmelard <lmelard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/22 14:45:43 by lmelard           #+#    #+#             */
/*   Updated: 2023/06/29 16:04:07 by lmelard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <cerrno>		// errno
# include <cstring>		// strerror
# include <iostream>	// cerr, cout
# include <sstream>		// stringstream
# include <string>		// string
# include <vector>		// vector

# include <arpa/inet.h>	// inet_ntoa
# include <netdb.h>		// recv, send, sockaddr, accept, addrinfo, getaddrinfo, socket, setsockopt, bind, freeaddrinfo, listen
# include <poll.h>		// pollfd, poll
# include <stdlib.h>	// exit
# include <unistd.h>	// close

class Client {

	public:
	
		Client( std::string name, int socket );
		~Client( void );

		int				getCfd( void ) const;
		std::string		getName( void ) const;
		std::string		getUsername( void ) const;
		std::string		getNickname( void ) const;
		bool			getPassStatus( void ) const;
		bool			getIfRegistered( void ) const;

		void			setCfd( int& clientFd );
		void			setName( std::string const& name );
		void			setUsername( std::string const& username );
		void			setNickname( std::string const& nickname );
		void			setPassStatus( bool const& passwordStatus );
		void			setIfRegistered( bool const& RegistrationStatus );

	private:
	
		int			_fd;
		
		std::string _name;// remplacer par realname
		std::string	_username;
		std::string	_nickname;

		bool		_passStatus;
		bool		_isRegistered;
		// bool		_validNick;

		// penser a :
		/* 
		bool				_welcomeStatus;
		std::string			_userModes;
		std::string			_serverName;
		*/
};

#endif
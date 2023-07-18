/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgaillag <cgaillag@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/22 14:45:43 by lmelard           #+#    #+#             */
/*   Updated: 2023/07/18 10:56:07 by cgaillag         ###   ########.fr       */
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

# include "Channel.hpp"

class Channel;
class Client {

	public:

		Client();
		Client( int socket );
		Client( Client const & src );
		~Client( void );

		Client &   operator=( Client const& rhs );

		// Client&			operator=(const Client& other);

		int				getFd( void ) const;
		std::string		getNickname( void ) const;
		std::string		getUsername( void ) const;
		std::string		getRealname( void ) const;
		bool			getPassStatus( void ) const;
		bool			getNickStatus( void ) const;
		bool			getIfRegistered( void ) const;
		std::string		getSource( void ) const;
		bool			getUserModes ( void ) const;
		bool			getIfInvisible( void ) const;

		// void			setFd( int& clientFd );//not used as _fd is const
		void			setNickname( std::string const& name );
		void			setUsername( std::string const& name );
		void			setRealname( std::string const& name );
		void			setPassStatus( bool const& status );
		void			setNickStatus( bool const& status );
		void			setIfRegistered( bool const& status );
		void			setSource( std::string nickname, std::string username );
		void			setUserModes( bool const& status );
		bool			setChannelModes( std::string const& mode );
		void			setIfInvisible( bool const& status );

	private:


		int	_fd;

		std::string	_name; //  TODO delete

		std::string	_nickname;
		std::string	_username;
		std::string _realname;

		std::string	_source;
		// std::string	_userModes;

		bool		_passStatus;
		bool		_nickStatus;
		bool		_isRegistered;
		bool		_userModes;
		bool		_isInvisible;

		/* 	TMP IN ORDER TO TEST MODE CMD -> WAITING FOR CLEM CHANGES	*/
		//std::vector<Channel>	_clientChannels;

		// penser a :
		/*
		bool				_welcomeStatus;
		std::string			_userModes;
		std::string			_serverName;
		*/
};

#endif /* __CLIENT_HPP__ */

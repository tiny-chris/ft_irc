/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/22 14:45:43 by lmelard           #+#    #+#             */
/*   Updated: 2023/07/24 15:39:11 by codespace        ###   ########.fr       */
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
# include "deepCopyUtils.hpp"

class Channel;

class Client {

	public:

		// typedef std::vector< Channel* >	channelPtr;

		Client();
		Client( int socket );
		Client( Client const & src );
		~Client( void );

		Client &   operator=( Client const& rhs );

		// Client&			operator=(const Client& other);

		int				getFd( void ) const;
		bool			getPassStatus( void ) const;
		bool			getNickStatus( void ) const;
		bool			getIfRegistered( void ) const;
		bool			getUserModes ( void ) const;
		std::string		getNickname( void ) const;
		std::string		getUsername( void ) const;
		std::string		getRealname( void ) const;
		std::string		getSource( void ) const;
		// channelPtr		getClientChannels( void ) const;
		std::vector< std::string >	getClientChannels( void ) const;

		void			setPassStatus( bool const& status );
		void			setNickStatus( bool const& status );
		void			setIfRegistered( bool const& status );
		void			setUserModes( bool const& status );
		void			setNickname( std::string const& name );
		void			setUsername( std::string const& name );
		void			setRealname( std::string const& name );
		void			setSource( std::string nickname, std::string username );
		// void			setClientChannels( channelPtr channels ) const;
		void			setClientChannels( std::vector< std::string > channels ) const;

		// bool			setChannelModes( std::string const& mode );
		void			addChannel( std::string channelName );
		void			removeClientChannel( std::string chanName);

	private:

		int	_fd;

		bool		_passStatus;
		bool		_nickStatus;
		bool		_isRegistered;
		bool		_userModes;

		std::string	_name; //  TODO delete

		std::string	_nickname;
		std::string	_username;
		std::string _realname;
		std::string	_source;
		// std::string	_userModes;

		// channelPtr	_clientChannels;
		std::vector< std::string >	_clientChannels;

		/* 	TMP IN ORDER TO TEST MODE CMD -> WAITING FOR CLEM CHANGES	*/
		//std::vector<Channel>	_clientChannels;

		// penser a :
		/*
		bool				_welcomeStatus;
		std::string			_serverName;
		*/
};

#endif /* __CLIENT_HPP__ */

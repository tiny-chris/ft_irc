/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/22 15:13:43 by lmelard           #+#    #+#             */
/*   Updated: 2023/07/14 20:22:55 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

/* ----------------------- CONSTRUCTORS & DESTRUCTOR ------------------------ */

Client::Client() : _fd( -1 ) {}

Client::Client( int socket ) :
			_fd( socket ),
			_nickname( "" ),
			_username( "" ),
			_realname( "" ),
			_source( "" ),
			// _channelModes( "" ),
			_passStatus( false ),
			_nickStatus( false ),
			_isRegistered( false ),
			_userModes ( true )
{}

Client::Client( Client const & src ) : _fd( src.getFd() ) {
	*this = src;
	return ;
}

Client &    Client::operator=( Client const& rhs ) {
    if ( this != &rhs )
	{
		// _fd = rhs.getFd();
		_nickname = rhs.getNickname();// ne peut pas être identique
		_username = rhs.getUsername();
		_realname = rhs.getRealname();
		_source = rhs.getSource();
		_userModes = rhs.getUserModes();
		// _channelModes = rhs.getChannelModes();
		_passStatus = getPassStatus();
		_nickStatus = getNickStatus();
		_isRegistered = getIfRegistered();
	}
	return ( *this );
}

Client::~Client( void ) {}

/* ------------ MEMBER FUNCTION: OVERLOAD OF ASSIGNMENT OPERATOR ------------ */

/*	<REMARK> no change for the fd as '_fd' is a 'const int'
*/
// Client&		Client::operator=(const Client& rhs)
// {
// 	if (this != &rhs)
// 	{
// 		this->_nickname = rhs.getNickname();
// 		this->_username = rhs.getUsername();
// 		this->_realname = rhs.getRealname();
// 		this->_source = rhs.getSource();
// 		this->_passStatus = rhs.getPassStatus();
// 		this->_nickStatus = rhs.getNickStatus();
// 		this->_isRegistered = rhs.getIfRegistered();
// 	}
// 	return (*this);
// }

/* ---------------------- MEMBER FUNCTIONS: ACCESSORS ----------------------- */

int			Client::getFd( void ) const { return _fd; }
std::string	Client::getNickname( void ) const {	return _nickname; }
std::string	Client::getUsername( void ) const { return _username; }
std::string	Client::getRealname( void ) const {	return _realname; }
bool		Client::getPassStatus( void ) const { return _passStatus; }
bool		Client::getNickStatus( void ) const { return _nickStatus; }
bool 		Client::getIfRegistered( void ) const { return _isRegistered; }
std::string	Client::getSource( void ) const { return _source; }
bool		Client::getUserModes ( void ) const { return _userModes; }
// std::string	Client::getChannelModes ( void ) const { return _channelModes; }

// void		Client::setFd( int& clientFd ) { _fd = clientFd; }//not used as _fd is const
void		Client::setNickname( std::string const& name ) { _nickname = name; }
void		Client::setUsername( std::string const& name ) { _username = name; }
void		Client::setRealname( std::string const& name ) { _realname = name; }
void		Client::setPassStatus( bool const& status ) { _passStatus = status; }
void		Client::setNickStatus( bool const& status ) { _nickStatus = status; }
void		Client::setIfRegistered( bool const& status ) { _isRegistered = status; }
void		Client::setSource( std::string nickname, std::string username ) {
	_source = nickname + "!" + username + "@localhost";
}
void		Client::setUserModes( bool const& mode ) { _userModes = mode; }

// // TO COMPLETE
// bool		Client::setChannelModes( std::string const& mode ) {
// 	return false;
// }

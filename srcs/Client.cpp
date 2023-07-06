/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgaillag <cgaillag@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/22 15:13:43 by lmelard           #+#    #+#             */
/*   Updated: 2023/07/06 14:03:05 by cgaillag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

/* ----------------------- CONSTRUCTORS & DESTRUCTOR ------------------------ */

Client::Client( std::string name, int socket ) :
			_fd( socket ),
			_nickname( "" ),
			_username( "" ),
			_realname( name ),
			_source( "" ),
			_passStatus( false ),
			_nickStatus( false ),
			_isRegistered( false )
{}

Client::~Client( void ) {}

/* ------------ MEMBER FUNCTION: OVERLOAD OF ASSIGNMENT OPERATOR ------------ */

/*	<REMARK> no change for the fd as '_fd' is a 'const int'
*/
Client&		Client::operator=(const Client& rhs)
{
	if (this != &rhs)
	{
		this->_nickname = rhs.getNickname();
		this->_username = rhs.getUsername();
		this->_realname = rhs.getRealname();
		this->_source = rhs.getSource();
		this->_passStatus = rhs.getPassStatus();
		this->_nickStatus = rhs.getNickStatus();
		this->_isRegistered = rhs.getIfRegistered();
	}
	return (*this);
}

/* ---------------------- MEMBER FUNCTIONS: ACCESSORS ----------------------- */

int			Client::getFd( void ) const { return _fd; }
std::string	Client::getNickname( void ) const {	return _nickname; }
std::string	Client::getUsername( void ) const { return _username; }
std::string	Client::getRealname( void ) const {	return _realname; }
bool		Client::getPassStatus( void ) const { return _passStatus; }
bool		Client::getNickStatus( void ) const { return _nickStatus; }
bool 		Client::getIfRegistered( void ) const { return _isRegistered; }
std::string	Client::getSource( void ) const { return _source; }

// void		Client::setFd( int& clientFd ) { _fd = clientFd; }
void		Client::setNickname( std::string const& name ) { _nickname = name; }
void		Client::setUsername( std::string const& name ) { _username = name; }
void		Client::setRealname( std::string const& name ) { _realname = name; }
void		Client::setPassStatus( bool const& status ) { _passStatus = status; }
void		Client::setNickStatus( bool const& status ) { _nickStatus = status; }
void		Client::setIfRegistered( bool const& status ) { _isRegistered = status; }
void		Client::setSource( std::string nickname, std::string username ) {
	_source = nickname + "!" + username + "@localhost";
}
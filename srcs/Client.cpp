/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmelard <lmelard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/22 15:13:43 by lmelard           #+#    #+#             */
/*   Updated: 2023/06/30 17:37:06 by lmelard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

/* ----------------------- CONSTRUCTORS & DESTRUCTOR ------------------------ */

Client::Client( std::string name, int socket ) :
			_fd( socket ),
			_name( name ),
			_username( "" ),
			_nickname( "" ),
			_source( ":" ),
			_passStatus( false ),
			_nickStatus( false ),
			_isRegistered( false )
{}

Client::~Client( void ) {}

/* ------------ MEMBER FUNCTION: OVERLOAD OF ASSIGNMENT OPERATOR ------------ */



/* ---------------------- MEMBER FUNCTIONS: ACCESSORS ----------------------- */

int			Client::getCfd( void ) const { return _fd; }
std::string	Client::getName( void ) const {	return _name; }
std::string	Client::getUsername( void ) const { return _username; }
std::string	Client::getNickname( void ) const {	return _nickname; }
bool		Client::getPassStatus( void ) const { return _passStatus; }
bool 		Client::getIfRegistered( void ) const { return _isRegistered; }
bool		Client::getNickStatus( void ) const { return _nickStatus; }
std::string	Client::getSource( void ) const { return _source; }

void		Client::setCfd( int& clientFd ) { _fd = clientFd; }
void		Client::setName( std::string const& name ) { _name = name; }
void		Client::setUsername( std::string const& name ) { _username = name; }
void		Client::setNickname( std::string const& name ) { _nickname = name; }
void		Client::setPassStatus( bool const& pwdStatus ) { _passStatus = pwdStatus; }
void		Client::setNickStatus( bool const&  nickStatus ) { _nickStatus = nickStatus; }
void		Client::setIfRegistered( bool const& regStatus ) { _isRegistered = regStatus; }
void		Client::setSource( std::string nickname, std::string username ) {
	_source = ":" + nickname + "!" + username + "@localhost";
}
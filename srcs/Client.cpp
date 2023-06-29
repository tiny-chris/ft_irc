/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmelard <lmelard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/22 15:13:43 by lmelard           #+#    #+#             */
/*   Updated: 2023/06/29 18:06:59 by lmelard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

/* ----------------------- CONSTRUCTORS & DESTRUCTOR ------------------------ */

Client::Client( std::string name, int socket ) :
			_fd( socket ),
			_name( name ),
			_username( "" ),
			_nickname( "" ),
			_passStatus( false ),
			_isRegistered( false ),
			_validNick( false )
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
bool		Client::getValidNick( void ) const { return _validNick; }

void		Client::setCfd( int& clientFd ) { _fd = clientFd; }
void		Client::setName( std::string const& name ) { _name = name; }
void		Client::setUsername( std::string const& name ) { _username = name; }
void		Client::setNickname( std::string const& name ) { _nickname = name; }
void		Client::setPassStatus( bool const& pwdStatus ) { _passStatus = pwdStatus; }
void		Client::setIfRegistered( bool const& regStatus ) { _isRegistered = regStatus; }
void		Client::setValidNick( bool const&  nickStatus ) { _validNick = nickStatus; }

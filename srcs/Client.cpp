/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/22 15:13:43 by lmelard           #+#    #+#             */
/*   Updated: 2023/07/24 15:47:24 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

/* ----------------------- CONSTRUCTORS & DESTRUCTOR ------------------------ */

Client::Client() : _fd( -1 ) {}

Client::Client( int socket ) :
			_fd( socket ),
			_passStatus( false ),
			_nickStatus( false ),
			_isRegistered( false ),
			_userModes ( true ),
			_nickname( "" ),
			_username( "" ),
			_realname( "" ),
			_source( "" ),
			_clientChannels()
			// _channelModes( "" ),
{}

Client::Client( Client const & src ) : _fd( src.getFd() ) {
	*this = src;
	return ;
}

Client::~Client( void ) {}

/* ------------ MEMBER FUNCTION: OVERLOAD OF ASSIGNMENT OPERATOR ------------ */

Client &    Client::operator=( Client const& rhs ) {
    if ( this != &rhs )
	{
		_fd = rhs.getFd();
		_passStatus = rhs.getPassStatus();
		_nickStatus = rhs.getNickStatus();
		_isRegistered = rhs.getIfRegistered();
		_userModes = rhs.getUserModes();
		_nickname = rhs.getNickname();
		_username = rhs.getUsername();
		_realname = rhs.getRealname();
		_source = rhs.getSource();
		_clientChannels = rhs.getClientChannels();
	}
	return ( *this );
}

/* ---------------------- MEMBER FUNCTIONS: ACCESSORS ----------------------- */

int			Client::getFd( void ) const { return _fd; }
bool		Client::getPassStatus( void ) const { return _passStatus; }
bool		Client::getNickStatus( void ) const { return _nickStatus; }
bool 		Client::getIfRegistered( void ) const { return _isRegistered; }
bool		Client::getUserModes ( void ) const { return _userModes; }
std::string	Client::getNickname( void ) const {	return _nickname; }
std::string	Client::getUsername( void ) const { return _username; }
std::string	Client::getRealname( void ) const {	return _realname; }
std::string	Client::getSource( void ) const { return _source; }
// std::vector< Channel* >	Client::getClientChannels( void ) const { return _clientChannels; }
std::vector< std::string >	Client::getClientChannels( void ) const { return _clientChannels; }
// std::string	Client::getChannelModes ( void ) const { return _channelModes; }

// void		Client::setFd( int& clientFd ) { _fd = clientFd; }//not used as _fd is const
void		Client::setPassStatus( bool const& status ) { _passStatus = status; }
void		Client::setNickStatus( bool const& status ) { _nickStatus = status; }
void		Client::setIfRegistered( bool const& status ) { _isRegistered = status; }
void		Client::setUserModes( bool const& mode ) { _userModes = mode; }
void		Client::setNickname( std::string const& name ) { _nickname = name; }
void		Client::setUsername( std::string const& name ) { _username = name; }
void		Client::setRealname( std::string const& name ) { _realname = name; }
void		Client::setSource( std::string nickname, std::string username ) {
	_source = nickname + "!" + username + "@localhost";
}
// // TO COMPLETE
// bool		Client::setChannelModes( std::string const& mode ) {
// 	return false;
// }

void		Client::addChannel( std::string channelName ) {
	if ( !channelName.empty() ) {
		_clientChannels.push_back( channelName );
	}
}

void		Client::removeClientChannel( std::string chanName )
{
	for (size_t i = 0; i < _clientChannels.size(); i++)
    {
		if (_clientChannels[i] == chanName)
		{
			_clientChannels.erase(_clientChannels.begin() + i );
			break;
		}
	}
}

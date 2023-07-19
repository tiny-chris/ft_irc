/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgaillag <cgaillag@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/22 15:13:43 by lmelard           #+#    #+#             */
/*   Updated: 2023/07/19 15:03:20 by cgaillag         ###   ########.fr       */
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
		deepCopyVector( rhs._clientChannels, _clientChannels );
		// ChannelPtr copyClientChannels;
		// copyClientChannels.reserve( rhs._clientChannels.size() );
		// for ( size_t i = 0; i < rhs._clientChannels.size(); ++i ) {
		// 	Channel* ptr = rhs._clientChannels[ i ];
		// 	//copie profonde:
		// 	Channel* newPtr = new Channel(*ptr);
		// 	copyClientChannels.push_back( newPtr );
		// }
		// for ( size_t i = 0; i < rhs._clientChannels.size(); ++i ) {
		// 	delete _clientChannels[ i ];
		// }
		// _clientChannels = copyClientChannels;

		// _channelModes = rhs.getChannelModes();
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
bool		Client::getPassStatus( void ) const { return _passStatus; }
bool		Client::getNickStatus( void ) const { return _nickStatus; }
bool 		Client::getIfRegistered( void ) const { return _isRegistered; }
bool		Client::getUserModes ( void ) const { return _userModes; }
std::string	Client::getNickname( void ) const {	return _nickname; }
std::string	Client::getUsername( void ) const { return _username; }
std::string	Client::getRealname( void ) const {	return _realname; }
std::string	Client::getSource( void ) const { return _source; }
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

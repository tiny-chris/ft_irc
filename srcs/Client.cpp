/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmelard <lmelard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by 2.fr>             #+#    #+#             */
/*   Updated: 2023/08/02 10:36:28 by lmelard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

/* ----------------------- CONSTRUCTORS & DESTRUCTOR ------------------------ */

Client::Client() : _fd( -1 ) {}

Client::Client( int socket )
  : _fd( socket ),
    _passStatus( false ),
    _nickStatus( false ),
    _isRegistered( false ),
    _invisibleMode( false ),
    _operatorMode( false ),
    _nickname( "" ),
    _username( "" ),
    _realname( "" ),
    _source( "" ),
    _clientChannels(),
    _buf("") {}

Client::Client( Client const& src ) : _fd( src.getFd() ) {
  *this = src;
  return;
}

Client::~Client( void ) {}

/* ------------  OVERLOAD OF ASSIGNMENT OPERATOR ------------ */

Client& Client::operator=( Client const& rhs ) {
  if( this != &rhs ) {
    _fd = rhs.getFd();
    _passStatus = rhs.getPassStatus();
    _nickStatus = rhs.getNickStatus();
    _isRegistered = rhs.getIfRegistered();
    _invisibleMode = rhs.getInvisibleMode();
    _operatorMode = rhs.getOperatorMode();
    _nickname = rhs.getNickname();
    _username = rhs.getUsername();
    _realname = rhs.getRealname();
    _source = rhs.getSource();
    _clientChannels = rhs.getClientChannels();
    _buf = rhs.getBuf();
  }
  return ( *this );
}

/* ---------------------- ACCESSORS ----------------------- */

int         Client::getFd( void ) const { return _fd; }
bool        Client::getPassStatus( void ) const { return _passStatus; }
bool        Client::getNickStatus( void ) const { return _nickStatus; }
bool        Client::getIfRegistered( void ) const { return _isRegistered; }
bool        Client::getInvisibleMode( void ) const { return _invisibleMode; }
bool        Client::getOperatorMode( void ) const { return _operatorMode; }
std::string Client::getModes( void ) const {
  std::string mode = "";
  if( getInvisibleMode() )
    mode += "i";
  if( getOperatorMode() )
    mode += "o";
  if( mode.size() != 0 )
    mode = "+" + mode;
  return mode;
}
std::string              Client::getNickname( void ) const { return _nickname; }
std::string              Client::getUsername( void ) const { return _username; }
std::string              Client::getRealname( void ) const { return _realname; }
std::string              Client::getSource( void ) const { return _source; }
std::vector<std::string> Client::getClientChannels( void ) const {
  return _clientChannels;
}
std::string              Client::getBuf( void ) const { return _buf; }

/* ----------------------  SETTERS ------------------------- */

void Client::setPassStatus( bool const& status ) { _passStatus = status; }
void Client::setNickStatus( bool const& status ) { _nickStatus = status; }
void Client::setIfRegistered( bool const& status ) { _isRegistered = status; }
void Client::setInvisibleMode( bool const& mode ) { _invisibleMode = mode; }
void Client::setOperatorMode( bool const& mode ) { _operatorMode = mode; }
void Client::setNickname( std::string const& name ) { _nickname = name; }
void Client::setUsername( std::string const& name ) { _username = name; }
void Client::setRealname( std::string const& name ) { _realname = name; }
void Client::setSource( std::string nickname, std::string username ) {
  _source = nickname + "!" + username + "@localhost";
}
void Client::setBuf( std::string const& buf ) { _buf = buf; }

/* ---------------------- METHODS ------------------------- */

/**
 * @brief       Adds a channel to the _clientChannels map
 */

void Client::addChannel( std::string channelName ) {
  if( !channelName.empty() ) {
    _clientChannels.push_back( channelName );
  }
}

/**
 * @brief       Removes a channel to the _clientChannels map
 */

void Client::removeClientChannel( std::string chanName ) {
  for( size_t i = 0; i < _clientChannels.size(); i++ ) {
    if( _clientChannels[i] == chanName ) {
      _clientChannels.erase( _clientChannels.begin() + i );
      break;
    }
  }
}

/**
 * @brief       Sets user modes (invisible i, operator o)
 */

bool Client::handleUserModeSet( char modeChar, std::string* modechange ) {
  switch( modeChar ) {
    case 'i':
      if( !getInvisibleMode() ) {
        setInvisibleMode( true );
        *modechange += "i";
      }
      return true;
    case 'o':
      if( !getOperatorMode() ) {
        setOperatorMode( true );
        *modechange += "o";
      }
      return true;
  }
  return false;
}

/**
 * @brief       Unsets user modes (invisible i, operator o)
 */

bool Client::handleUserModeUnset( char modeChar, std::string* modechange ) {
  switch( modeChar ) {
    case 'i':
      if( getInvisibleMode() ) {
        setInvisibleMode( false );
        *modechange += "i";
      }
      return true;
    case 'o':
      if( getOperatorMode() ) {
        setOperatorMode( false );
        *modechange += "o";
      }
      return true;
  }
  return false;
}

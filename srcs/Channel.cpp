/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgaillag <cgaillag@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/07 15:34:55 by codespace         #+#    #+#             */
/*   Updated: 2023/07/28 16:42:51 by cgaillag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"


/* CONSTRUCTORS ***************************************************************/

Channel::Channel() {}

Channel::Channel(std::string const& name) :
  _channelName( name ),
  _channelOps(),
  _channelMembers(),
  _invitedMembers(),
  _keyStatus( 0 ),
  _limitStatus( 0 ),
  _inviteOnlyStatus( 0 ),
  _topicRestrictionStatus( 1 ),
  _limitBis( 0 ),
  _key( "" ),
  _limit( "" ),
  _topic( "" ),
  _topicSetter( "" ),
  _topicDate( "" )
{}

Channel::Channel( Channel const & src ) {
  *this = src;
  return ;
}

/* DESTRUCTORS ****************************************************************/

Channel::~Channel() {}

/* OVERLOADS ******************************************************************/

Channel &    Channel::operator=( Channel const& rhs ){
  if (this != &rhs) {
    _channelName = rhs._channelName;
    _channelOps = rhs._channelOps;
    _channelMembers = rhs._channelMembers;
    _invitedMembers = rhs._invitedMembers;
    _keyStatus = rhs._keyStatus;
    _limitStatus = rhs._limitStatus;
    _inviteOnlyStatus = rhs._inviteOnlyStatus;
    _topicRestrictionStatus = rhs._topicRestrictionStatus;
    _limitBis = rhs._limitBis;
    _key = rhs._key;
    _limit = rhs._limit;
    _topic = rhs._topic;
    _topicSetter = rhs._topicSetter;
    _topicDate = rhs._topicDate;
  }
  return (*this);
}

/* METHODS ********************************************************************/

void        Channel::addChannelOps( Client* client )
{
  if (client != NULL ) {
    _channelOps.insert(std::make_pair( client->getNickname(), client ));
  }
}

void        Channel::removeChannelOp( Client* client )
{
  if ( client != NULL ) {
    Channel::mapClientsPtr::iterator it = _channelOps.begin();
    
    for ( ; it != _channelOps.end(); ++it ) {
      if ( it->first == client->getNickname() ) {
        _channelOps.erase( it );
        break;
      }
    }
  }
}

void        Channel::addChannelMember( Client* client )
{
  if (client != NULL) {
    _channelMembers.insert(std::make_pair( client->getNickname(), client ));
  }
}

void        Channel::removeChannelMember( Client* client )
{
  Channel::mapClientsPtr::iterator it = _channelMembers.begin();
	for ( ; it != _channelMembers.end(); ++it ) {
		if ( it->first == client->getNickname() ) {
		  _channelMembers.erase( it );
			break;
    }
	}
}

void        Channel::addInvitedMember( const std::string& clientName )
{
  if ( !clientName.empty() ) {
    if ( isInvited( clientName ) ) {
      std::cout << MSGERROR << clientName << " is already invited in channel " << _channelName << std::endl;
      return;
    }
    _invitedMembers.push_back( clientName );
  }
  else {
    std::cout << MSGERROR << "there is no client invited (empty name)" << std::endl;
  }
}

void        Channel::removeInvitedMember( const std::string& clientName )
{
  if ( !clientName.empty() ) {
    std::vector< std::string >::iterator it = _invitedMembers.begin();
    for ( ; it != _invitedMembers.end(); ++it ) {
      if ( *it == clientName ) {
        _invitedMembers.erase( it );
        break;
      }
    }
  }
}

std::string Channel::getChannelName( void ) const { return ( _channelName ); }
bool        Channel::getKeyStatus( void ) const { return ( _keyStatus ); }
bool        Channel::getLimitStatus( void ) const { return ( _limitStatus ); }
bool        Channel::getInviteOnlyStatus( void ) const { return ( _inviteOnlyStatus ); }
bool        Channel::getTopicRestrictionStatus( void ) const { return ( _topicRestrictionStatus ); }
int         Channel::getLimitBis( void ) const { return ( _limitBis ); }
std::string	Channel::getLimit( void ) const { return (_limit ); }
std::string	Channel::getKey( void ) const { return ( _key ); }
std::string Channel::getTopic( void ) const { return ( _topic ); }
std::string Channel::getTopicSetter( void ) const { return ( _topicSetter ); }
std::string Channel::getTopicDate( void ) const { return ( _topicDate ); }


std::string	Channel::getModes( void ) const {
  std::string modes;

  modes.push_back('+');
  if ( getKeyStatus() == true ) {
    modes.push_back('k');
  }
  if ( getLimitStatus() == true ) {
    modes.push_back('l');
  }
  if ( getInviteOnlyStatus() == true ) {
    modes.push_back('i');
  }
  if ( getTopicRestrictionStatus() == true ) {
    modes.push_back('t');
  }
  if (modes.size() == 1) {
    modes.clear();
  }
  return (modes);
}

std::string Channel::getModesArgs( void ) const {
  std::string modesArgs;

  if (getKey() != "") {
    modesArgs += (getKey() + " ");
  }
  if (getLimit() != "") {
    modesArgs += getLimit();
  }
  return (modesArgs);

}

Channel::mapClientsPtr&   Channel::getChannelMembers( void ) { return (_channelMembers); }
Channel::mapClientsPtr&   Channel::getChannelOps( void ) { return (_channelOps); }

bool        Channel::checkValidLimit(std::string limit) const {
  std::stringstream	iss(limit);
  int	num;

  iss >> num;
  if (num > 1 && num < MAXMEMBERS && intToString(num) == limit) {
    return true;
  }
  return false;
}

Channel::vecString&   Channel::getInvitedMembers( void ) { return _invitedMembers; }

void		Channel::setChannelName( std::string& name ) { _channelName = name; }
void    Channel::setKeyStatus( bool const& status ) { _keyStatus = status; }
void    Channel::setLimitStatus( bool const& status ) { _limitStatus = status; }
void    Channel::setInviteOnlyStatus( bool const& status ) { _inviteOnlyStatus = status; }
void    Channel::setTopicRestrictionStatus( bool const& status ) { _topicRestrictionStatus = status; }
void		Channel::setLimitBis( int const& limit ) { _limitBis = limit; }
void		Channel::setLimit( std::string const& limit ) { _limit = limit; }
void		Channel::setKey( std::string const& key ) { _key = key; }
void    Channel::setTopic( std::string const& topic ) { _topic = topic; }
void    Channel::setTopicSetter( std::string const& topicSetter ) { _topicSetter = topicSetter; }
void    Channel::setTopicDate( std::string const& topicDate ) { _topicDate = topicDate; }

bool        Channel::checkChannelOps( std::string name ) {
  if (_channelOps.find(name) != _channelOps.end()) {
    return true;
  }
  return false;
}

bool        Channel::isInvited( std::string clientName ) const {
  vecString::const_iterator it = _invitedMembers.begin();
  for ( ; it != _invitedMembers.end(); ++it ) {
    if ( *it == clientName ) {
      return true;
    }
  }
  return false;
}

bool        Channel::checkChannelMembers( std::string name ) {
  if (_channelMembers.find(name) != _channelMembers.end()) {
    return true;
  }
  return false;
}

// Prefix + -> Sets new channel modes
void		Channel::handleChannelModeSet( char modeChar, std::string* modeArgs, std::string* modeChange,\
    const std::vector<std::string>& tokens, size_t *j ) {
    switch (modeChar) {
        case 'k':
            handleModeSetKey(modeArgs, modeChange, tokens, j);
            break;
        case 'l':
            handleModeSetLimit(modeArgs, modeChange, tokens, j);
            break;
        case 'i':
            handleModeSetInviteOnly(modeChange);
            break;
        case 't':
            handleModeSetTopicRestriction(modeChange);
            break;
        case 'o':
            handleModeSetOperator(modeArgs, modeChange, tokens, j);
            break;
    }
}

// Sets key mode +k <modearg>
void		Channel::handleModeSetKey( std::string* modeArgs, std::string* modeChange,\
    const std::vector<std::string> &tokens, size_t *j ) {
    if (tokens.size() > *j && getKeyStatus() == false && isValidParam(tokens[*j]) == true) {
        setKeyStatus(true);
        setKey(tokens[*j]);
        *modeChange += "k";
        *modeArgs += tokens[*j];
    }
    (*j)++;
}

// Sets limit mode +l <modearg>
void 		Channel::handleModeSetLimit( std::string* modeArgs, std::string* modeChange,\
    const std::vector<std::string> &tokens, size_t *j ) {
    if ( tokens.size() > *j && getLimitStatus() == false && checkValidLimit(tokens[*j]) == true ) {
        setLimitStatus( true );
        setLimitBis( StringToInt( tokens[ *j ] ) );
        setLimit( tokens[ *j ] );
        *modeChange += "l";
        *modeArgs += tokens[ *j ];
    }
    ( *j )++;
}

// Sets invite only mode +i
void 		Channel::handleModeSetInviteOnly(std::string* modeChange) {
    if (getInviteOnlyStatus() == false) {
        setInviteOnlyStatus(1);
        *modeChange += "i";
    }
}

// Sets topic restriction change mode +t
void 		Channel::handleModeSetTopicRestriction(std::string* modeChange) {
    if (getTopicRestrictionStatus() == false) {
        setTopicRestrictionStatus(1);
        *modeChange += "t";
    }
}

// Sets the designated user as a channop
void        Channel::handleModeSetOperator(std::string* modeArgs, std::string* modeChange, const std::vector<std::string> &tokens, size_t *j) {
    if (tokens.size() > *j && checkChannelOps(tokens[*j]) == false && checkChannelMembers(tokens[*j]) == true) {
        *modeChange += "o";
        *modeArgs += tokens[*j];
        Client *toAdd = getChannelMembers().at(tokens[*j]);
        addChannelOps(toAdd);
    }
    (*j)++;
}

// Prefix - -> Unsets channel modes
void		Channel::handleChannelModeUnset(char modeChar, std::string* modeArgs, std::string* modeChange,\
    const std::vector<std::string>& tokens, size_t *j) {
    switch(modeChar) {
        case 'k':
            handleModeUnsetKey(modeChange);
            break;
        case 'l':
            handleModeUnsetLimit(modeChange);
            break;
        case 'i':
            handleModeUnsetInviteOnly(modeChange);
            break;
        case 't':
            handleModeUnsetTopicRestriction(modeChange);
            break;
        case 'o':
            handleModeUnsetOperator(modeArgs, modeChange, tokens, j);
            break;
    }
}

// Unsets key mode
void		Channel::handleModeUnsetKey(std::string* modeChange) {
    if (getKeyStatus() == true) {
        setKeyStatus(false);
        setKey("");
        *modeChange += "k";
    }
}

// Unsets limit mode
void		Channel::handleModeUnsetLimit(std::string* modeChange) {
    if (getLimitStatus() == true) {
        setLimitStatus(false);
        setLimit("");
        *modeChange += "l";
    }
}

// Unsets invite only mode
void		Channel::handleModeUnsetInviteOnly(std::string* modeChange) {
    if (getInviteOnlyStatus() == true) {
        setInviteOnlyStatus(false);
        *modeChange += "i";
    }
}

// Unsetstopic restriction mode
void		Channel::handleModeUnsetTopicRestriction(std::string* modeChange) {
    if (getTopicRestrictionStatus() == true) {
        setTopicRestrictionStatus(false);
        *modeChange += "t";
    }
}

void        Channel::handleModeUnsetOperator( std::string* modeArgs, std::string* modeChange, const std::vector<std::string> &tokens, size_t *j) {
    if (tokens.size() > *j && getChannelOps().size() > 1 && checkChannelOps(tokens[*j]) == true && checkChannelMembers(tokens[*j]) == true) {
        *modeChange += "o";
        *modeArgs += tokens[*j];
        getChannelOps().erase(tokens[*j]);
    }
    (*j)++;
}

// Checks if the mode is a valid mode k,l,i,t
bool		Channel::isValidModeChar( char const modeChar ) {
    if (modeChar == 'k' || modeChar == 'l' || modeChar == 'i' || modeChar == 't' || modeChar == 'o')
        return true;
    return false;
}
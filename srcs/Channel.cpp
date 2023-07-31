/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmelard <lmelard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/07 15:34:55 by codespace         #+#    #+#             */
/*   Updated: 2023/07/31 14:58:48 by lmelard          ###   ########.fr       */
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

/* GETTERS ********************************************************************/


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
Channel::vecString&   Channel::getInvitedMembers( void ) { return _invitedMembers; }


/* SETTERS ********************************************************************/

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



/* METHODS ********************************************************************/

/**
 * @brief       Adds a new Channel Operator to _channelOps map
 */

void        Channel::addChannelOps( Client* client )
{
  if (client != NULL ) {
    _channelOps.insert(std::make_pair( client->getNickname(), client ));
  }
}

/**
 * @brief       Removes a Channel Operator from _channelOps map
 */

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

/**
 * @brief       Checks if a Client is a Channel operator
 */

bool        Channel::checkChannelOps( std::string name ) {
  if (_channelOps.find(name) != _channelOps.end()) {
    return true;
  }
  return false;
}

/**
 * @brief       Checks if the channel mode to set or unset is handled by our server
 *              k mode: key mode
 *              l mode: limit mode
 *              i mode: invite only mode
 *              t mode: topic mode
 *              o mode: chanop mode
 */

bool		Channel::isValidModeChar( char const modeChar ) {
    if (modeChar == 'k' || modeChar == 'l' || modeChar == 'i' || modeChar == 't' || modeChar == 'o')
        return true;
    return false;
}

/**
 * @brief       Adds a new Channel Member to _channelMembers map
 */

void        Channel::addChannelMember( Client* client )
{
  if (client != NULL) {
    _channelMembers.insert(std::make_pair( client->getNickname(), client ));
  }
}

/**
 * @brief       Removes a Channel Member from _channelOps map
 */

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

/**
 * @brief       Checks if a Client is a Channel member
 */

bool        Channel::checkChannelMembers( std::string name ) {
  if (_channelMembers.find(name) != _channelMembers.end()) {
    return true;
  }
  return false;
}

/**
 * @brief       Adds a Client to _invitedMember vector
 */

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

/**
 * @brief       Adds a Client from _invitedMember vector
 */

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

/**
 * @brief       Checks if the channel mode limit (+l) set is valid
 *              Unsigned integer > 1 and < MAXMEMBERS
 */

bool        Channel::checkValidLimit(std::string limit) const {
  std::stringstream	iss(limit);
  int	num;

  iss >> num;
  if (num > 1 && num < MAXMEMBERS && intToString(num) == limit) {
    return true;
  }
  return false;
}

/**
 * @brief       Checks if a Client is in the invited members vector
 */

bool        Channel::isInvited( std::string clientName ) const {
  vecString::const_iterator it = _invitedMembers.begin();
  for ( ; it != _invitedMembers.end(); ++it ) {
    if ( *it == clientName ) {
      return true;
    }
  }
  return false;
}

/**
 * @brief       Set the key mode : join channel with a key (must be a valid param)
 */

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

/**
 * @brief       Sets the limit mode : limits the channel member to a certain amount of clients
 */

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

/**
 * @brief       Sets the invite only mode : makes the channel accessible only by invite
 */

void 		Channel::handleModeSetInviteOnly(std::string* modeChange) {
    if (getInviteOnlyStatus() == false) {
        setInviteOnlyStatus(1);
        *modeChange += "i";
    }
}

/**
 * @brief       Sets the topic mode : makes the channel's topic modifiable by channops
 */

void 		Channel::handleModeSetTopicRestriction(std::string* modeChange) {
    if (getTopicRestrictionStatus() == false) {
        setTopicRestrictionStatus(1);
        *modeChange += "t";
    }
}

/**
 * @brief       Sets a designated user as channop
 */
void        Channel::handleModeSetOperator(std::string* modeArgs, std::string* modeChange, const std::vector<std::string> &tokens, size_t *j) {
    if (tokens.size() > *j && checkChannelOps(tokens[*j]) == false && checkChannelMembers(tokens[*j]) == true) {
        *modeChange += "o";
        *modeArgs += tokens[*j];
        Client *toAdd = getChannelMembers().at(tokens[*j]);
        addChannelOps(toAdd);
    }
    (*j)++;
}

/**
 * @brief       Unset the key mode : don't need a key to join channel 
 */

void		Channel::handleModeUnsetKey(std::string* modeChange) {
    if (getKeyStatus() == true) {
        setKeyStatus(false);
        setKey("");
        *modeChange += "k";
    }
}

/**
 * @brief       Unsets the limit mode 
 */

void		Channel::handleModeUnsetLimit(std::string* modeChange) {
    if (getLimitStatus() == true) {
        setLimitStatus(false);
        setLimit("");
        *modeChange += "l";
    }
}

/**
 * @brief       Unsets the invite only mode, no need to beinvited to join channel
 */

void		Channel::handleModeUnsetInviteOnly(std::string* modeChange) {
    if (getInviteOnlyStatus() == true) {
        setInviteOnlyStatus(false);
        *modeChange += "i";
    }
}

/**
 * @brief       Unsets topic mode : any channel member can modify the channel topic
 */

void		Channel::handleModeUnsetTopicRestriction(std::string* modeChange) {
    if (getTopicRestrictionStatus() == true) {
        setTopicRestrictionStatus(false);
        *modeChange += "t";
    }
}

/**
 * @brief       Remove a client from channel operators
 */

void        Channel::handleModeUnsetOperator( std::string* modeArgs, std::string* modeChange, const std::vector<std::string> &tokens, size_t *j) {
    if (tokens.size() > *j && getChannelOps().size() > 1 && checkChannelOps(tokens[*j]) == true && checkChannelMembers(tokens[*j]) == true) {
        *modeChange += "o";
        *modeArgs += tokens[*j];
        getChannelOps().erase(tokens[*j]);
    }
    (*j)++;
}

/**
 * @brief       Sets channel modes given in the modeStrings
 *              k mode: key mode
 *              l mode: limit mode
 *              i mode: invite only mode
 *              t mode: topic mode
 *              o mode: chanop mode
 */

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

/**
 * @brief       Unsets channel modes given in the modeStrings
 *              k mode: key mode
 *              l mode: limit mode
 *              i mode: invite only mode
 *              t mode: topic mode
 *              o mode: chanop mode
 */

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
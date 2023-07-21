/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmelard <lmelard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/07 15:34:55 by codespace         #+#    #+#             */
/*   Updated: 2023/07/21 16:41:17 by lmelard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel() {}

// Channel::Channel(std::string const& name, const Client& chanop) :
// _channelName(name),
// _keyStatus(0),
// _limitStatus(0),
// _inviteOnlyStatus(0),
// _topicRestrictionStatus(1),
// _key(""),
// _limit("")
// {
// 	std::cout << "chanop.getNickname(): " << chanop.getNickname() << std::endl;
// 	_channelOps.insert(std::pair< std::string, const Client* >(chanop.getNickname(), &chanop));
// 	std::cout << "first channop size: " << _channelOps.size() << std::endl;
// 	setTopicRestrictionStatus(1);
// }

Channel::Channel(std::string const& name) :
  _channelName( name ),
  _channelOps(),
  _channelMembers(),
  _invitedMembers(),
  _keyStatus( 0 ),
  _limitStatus( 0 ),
  _inviteOnlyStatus( 0 ),
  _topicRestrictionStatus( 1 ),
  _key( "" ),
  _limit( "" )
{}

Channel::~Channel() {}

Channel::Channel( Channel const & src ) {
  *this = src;
  return ;
}

Channel &    Channel::operator=( Channel const& rhs ){
  if (this != &rhs)
  {
    _channelName = rhs._channelName;
    _channelOps = rhs._channelOps;
    _channelMembers = rhs._channelMembers;
    _invitedMembers = rhs._invitedMembers;
    _keyStatus = rhs._keyStatus;
    _limitStatus = rhs._limitStatus;
    _inviteOnlyStatus = rhs._inviteOnlyStatus;
    _topicRestrictionStatus = rhs._topicRestrictionStatus;
    _key = rhs._key;
    _limit = rhs._limit;
  }
  return (*this);
}

void        Channel::addChannelOps( Client* client ){
  if (client != NULL )
  {
    _channelOps.insert(std::make_pair( client->getNickname(), client ));
  }
}

void        Channel::removeChannelOp( Client* client )
{
  Channel::mapClientsPtr::iterator it = _channelOps.begin();
	for ( ; it != _channelOps.end(); ++it )
	{
		if ( it->first == client->getNickname() ) {
		  _channelOps.erase( it );
			break;
    }
	}
}

void        Channel::addChannelMember( Client* client )
{
  if (client != NULL)
  {
    _channelMembers.insert(std::make_pair( client->getNickname(), client ));
  }
}

void        Channel::removeChannelMember( Client* client )
{
  Channel::mapClientsPtr::iterator it = _channelMembers.begin();
	for ( ; it != _channelMembers.end(); ++it )
	{
		if ( it->first == client->getNickname() ) {
		  _channelMembers.erase( it );
			break;
    }
	}
}

void        Channel::addInvitedMember( const std::string& clientName )
{
  if ( !clientName.empty() )
  {
    if ( isInvited( clientName ) ) 
    {
      std::cout << MSGERROR << clientName << " is already invited in channel " << _channelName << std::endl;
      return;
    }
    _invitedMembers.push_back( clientName );
  }
  else
  {
    std::cout << MSGERROR << "there is no client invited (empty name)" << std::endl;
  }
}

void        Channel::removeInvitedMember( const std::string& clientName )
{
  std::vector< std::string >::iterator it = _invitedMembers.begin();
	for ( ; it != _invitedMembers.end(); ++it )
	{
		if ( *it == clientName ) {
		  _invitedMembers.erase( it );
			break;
    }
	}
}

std::string Channel::getChannelName( void ) const { return ( _channelName ); }
bool        Channel::getKeyStatus( void ) const { return ( _keyStatus ); }
bool        Channel::getLimitStatus( void ) const { return ( _limitStatus ); }
bool        Channel::getInviteOnlyStatus( void ) const { return ( _inviteOnlyStatus ); }
bool        Channel::getTopicRestrictionStatus( void ) const { return ( _topicRestrictionStatus ); }
std::string	Channel::getLimit( void ) const { return (_limit ); }
std::string	Channel::getKey( void ) const { return ( _key ); }

std::string	Channel::getModes( void ) const {
  std::string modes;

  modes.push_back('+');
  if (getKeyStatus() == true)
    modes.push_back('k');
  if (getLimitStatus() == true)
    modes.push_back('l');
  if (getInviteOnlyStatus() == true)
    modes.push_back('i');
  if (getTopicRestrictionStatus() == true)
    modes.push_back('t');
  if (modes.size() == 1)
    modes.clear();
  return (modes);
}

std::string Channel::getModesArgs( void ) const {
  std::string modesArgs;

  if (getKey() != "")
    modesArgs += (getKey() + " ");
  if (getLimit() != "")
    modesArgs += getLimit();
  return (modesArgs);

}

Channel::mapClientsPtr&   Channel::getChannelMembers( void ) { return (_channelMembers); }
Channel::mapClientsPtr&   Channel::getChannelOps( void ) { return (_channelOps); }

bool        Channel::checkValidLimit(std::string limit) const {
  std::stringstream	iss(limit);
  int	num;

  iss >> num;
  if (num > 1 && num < CHANLIMIT && intToString(num) == limit)
    return true;
  return false;
}

Channel::vecString   Channel::getInvitedMembers( void ) const { return _invitedMembers; }

void		Channel::setChannelName( std::string& name ) { _channelName = name; }
void    Channel::setKeyStatus( bool const& status ) { _keyStatus = status; }
void    Channel::setLimitStatus( bool const& status ) { _limitStatus = status; }
void    Channel::setInviteOnlyStatus( bool const& status ) { _inviteOnlyStatus = status; }
void    Channel::setTopicRestrictionStatus( bool const& status ) { _topicRestrictionStatus = status; }
void		Channel::setLimit( std::string const& limit ) { _limit = limit; }
void		Channel::setKey( std::string const& key ) { _key = key; }

bool        Channel::checkChannelOps( std::string name )
{
  if (_channelOps.find(name) != _channelOps.end()) {
    return true;
  }
  return false;
}

bool        Channel::isInvited( std::string clientName ) const
{
  vecString::const_iterator it = _invitedMembers.begin();
  for ( ; it != _invitedMembers.end(); ++it )
  {
    if ( *it == clientName )
    {
      return true;
    }
  }
  return false;
}
bool        Channel::checkChannelMembers( std::string name )
{
  if (_channelMembers.find(name) != _channelMembers.end()) {
    return true;
  }
  return false;
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmelard <lmelard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/07 15:34:55 by codespace         #+#    #+#             */
/*   Updated: 2023/07/13 19:18:10 by lmelard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel() {}

Channel::Channel(std::string const& name, Client* chanop) : 
_channelName(name),
_keyStatus(0),
_limitStatus(0),
_inviteOnlyStatus(0),
_topicRestrictionStatus(1),
_key(""),
_limit("")
{
	std::cout << "chanop.getNickname(): " << chanop->getNickname() << std::endl;
	// _channelOps.insert(std::pair< std::string *, Client * >(&(chanop->getNickname()), chanop));
	_channelOps.push_back(chanop);
	std::cout << "first channop size: " << std::endl;
	setTopicRestrictionStatus(1);
	std::cout << "topic restriction status" << _topicRestrictionStatus << std::endl;
}

Channel::~Channel() {}

Channel::Channel( Channel const & src ) {
	*this = src;
	return ;
}

Channel &    Channel::operator=( Channel const& rhs ){
    if (this != &rhs)
	{
		_connectedClients = rhs._connectedClients;
		_channelOps = rhs._channelOps;
		_channelName = rhs._channelName;
		_keyStatus = rhs._keyStatus;
		_limitStatus = rhs._limitStatus;
		_inviteOnlyStatus = rhs._inviteOnlyStatus;
		_topicRestrictionStatus = rhs._topicRestrictionStatus;
		_key = rhs._key;
		_limit = rhs._limit;
	}
	return (*this);
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
		modesArgs += getKey();
	if (getLimit() != "")
		modesArgs += getLimit();
	return (modesArgs);
	
}

bool        Channel::checkValidLimit(std::string limit) const {
	
	if (limit < "1" || limit > intToString(CHANLIMIT))
		return false;
	return true;
}


void		Channel::setChannelName( std::string& name ) { _channelName = name; }
void        Channel::setKeyStatus( bool const& status ) { _keyStatus = status; }
void        Channel::setLimitStatus( bool const& status ) { _limitStatus = status; }
void        Channel::setInviteOnlyStatus( bool const& status ) { _inviteOnlyStatus = status; }
void        Channel::setTopicRestrictionStatus( bool const& status ) { _topicRestrictionStatus = status; }
void		Channel::setLimit( std::string const& limit ) { _limit = limit; } 
void		Channel::setKey( std::string const& key ) { _key = key; }

bool        Channel::checkChannelOps( std::string name ) 
{
	std::cout << "test checkChannelOps" << std::endl;
	std::cout << "_channelOps.size(): " << _channelOps.size() << std::endl;
	for (unsigned i=0; i < _channelOps.size(); i++)
    {
		std::cout << "Client nickname: " << _channelOps.at(i)->getNickname() << std::endl;
		std::cout << "name: " << name << std::endl;
		if (_channelOps.at(i)->getNickname() == name)
			return true;
    }
    return false;
}

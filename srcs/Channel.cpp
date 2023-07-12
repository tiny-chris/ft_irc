/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmelard <lmelard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/07 15:34:55 by codespace         #+#    #+#             */
/*   Updated: 2023/07/12 18:33:53 by lmelard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel() {}

Channel::Channel(std::string const& name) : _channelName(name),
_keyStatus(false),
_limitStatus(false),
_inviteOnlyStatus(false),
_topicRestrictionStatus(true),
_limit(""),
 {}

Channel::~Channel() {}

Channel::Channel( Channel const & src ) {
	*this = src;
	return ;
}

Channel &    Channel::operator=( Channel const& rhs ){
    if (this != &rhs)
	{
		_connectedClients = rhs._connectedClients;
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


void		Channel::setChannelName( std::string& name ) { _channelName = name; }
void        Channel::setKeyStatus( bool const& status ) { _keyStatus = status; }
void        Channel::setLimitStatus( bool const& status ) { _limitStatus = status; }
void        Channel::setInviteOnlyStatus( bool const& status ) { _inviteOnlyStatus = status; }
void        Channel::setTopicRestrictionStatus( bool const& status ) { _topicRestrictionStatus = status; }
void		Channel::setLimit( std::string const& limit ) { _limit = limit; } 
void		Channel::setKey( std::string const& key ) { _key = key; }

bool        Channel::checkChannelOps( std::string name ) 
{
	if (_channelOps.find(name) == _channelOps.end())
		return false;
	return true;
}

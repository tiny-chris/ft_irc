/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/07 15:34:55 by codespace         #+#    #+#             */
/*   Updated: 2023/07/07 16:15:55 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel() {}

Channel::Channel(std::string name) : _channelName(name) {}

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
void		Channel::setChannelName( std::string& name ) { _channelName = name; }
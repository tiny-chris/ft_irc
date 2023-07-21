/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/19 18:19:57 by lmelard           #+#    #+#             */
/*   Updated: 2023/07/21 12:49:59 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Socket.hpp"
#include "utils.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"

void        Server::handleKick( int clientSocket, std::string param )
{
	std::string source = _clients.at( clientSocket ).getSource();
	std::string nick = _clients.at( clientSocket ).getNickname();
	// Client *client = &_clients.at( clientSocket);
	
	// if no param are entered then a need more params err msg is displayed
    std::vector<std::string> tokens = splitString( param, ' ' );
    if ( param.empty() || tokens.size() < 2 )
    {
        replyMsg(clientSocket, ERR_NEEDMOREPARAMS( source, nick, "MODE"));
        return ;
    }
	std::string channelName = tokens[ 0 ];
	// cropping the first param (channel name) if its length is over the define CHANNELLEN
	if (channelName.size() > CHANNELLEN)
		channelName = channelName.substr(0, CHANNELLEN);
	// if the channel entered doesn't exist no such Channel error displayed
	if (!existingChannel( channelName ))
		replyMsg(clientSocket, ERR_NOSUCHCHANNEL( source, nick, channelName));
	else
	{
		Channel *chan = &_channels[ channelName ];
		if ( !chan->checkChannelOps( nick ) ) // if the client doesn't have chanops privileges 
		{
			replyMsg(clientSocket, ERR_CHANOPRIVSNEEDED( source, nick, channelName ));
			return ;
		}
		std::string toKick = tokens[ 1 ];
		if ( !chan->checkChannelMembers( toKick ) ) // checking if the client to kick is a chanmember
		{
			replyMsg(clientSocket, ERR_USERNOTINCHANNEL( source, nick, toKick, channelName ));
			return ;
		}
		if ( !chan->checkChannelMembers( nick ) ) // checking if the client that makes the request is a channel member
		{
			replyMsg(clientSocket, ERR_NOTONCHANNEL( source, nick, channelName ));
			return ;
		}
		std::string reason = (tokens.size() > 3) ? tokens[3] : "bye bye looser!";
		kickUser(clientSocket, chan, nick, toKick, reason);
	}
}

void	Server::kickUser(int clientSocket, Channel *chan, std::string nick, std::string toKick, std::string reason )
{
	// check if Client is the last operator on the channel
	// - if he is the last one is he the only one ? 
	//		- if yes delete the channel
	//		- if not display error msg
	if (chan->getChannelOps().size() == 1)
	{
		if ( chan->getChannelMembers().size() == 1 )
		{
			_channels.erase( chan->getChannelName() ); // delete the Channel
			replyMsg(clientSocket, DEFAULTKICK(nick, chan->getChannelName(), toKick, reason));
			replyMsg(clientSocket, chan->getChannelName() + " was deleted\r\n");
			return ;
		}
		else if (toKick == nick)
		{
			replyMsg( clientSocket, "Error: Cannot delete last operator\r\n");
			return ;
		}	
	}
	int socket;
	for ( std::map<std::string, Client *>::iterator it = chan->getChannelMembers().begin(); it != chan->getChannelMembers().end(); it++ )
	{
		socket = it->second->getFd();
		replyMsg(socket, DEFAULTKICK(nick, chan->getChannelName(), toKick, reason));
	}
	// remove client from channel members and channel operators
	chan->getChannelMembers().erase(toKick);
	chan->getChannelOps().erase(toKick);
}
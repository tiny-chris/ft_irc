/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/19 18:19:57 by lmelard           #+#    #+#             */
/*   Updated: 2023/07/24 11:40:15 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Socket.hpp"
#include "utils.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"


// Checks if all parameters are valids, user chanops rights, and kicks the indicated user if in the channel.
void        Server::handleKick( int clientSocket, std::string param )
{
	std::string source = _clients.at( clientSocket ).getSource();
	std::string nick = _clients.at( clientSocket ).getNickname();

	// if no param are entered then a need more params err msg is displayed
    std::vector<std::string> tokens = splitString( param, ' ' );
    if ( param.empty() || tokens.size() < 2 )
    {
        replyMsg(clientSocket, ERR_NEEDMOREPARAMS( source, nick, "KICK"));
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
		if ( !chan->checkChannelMembers( nick ) ) // checking if the client that makes the request is a channel member
		{
			replyMsg(clientSocket, ERR_NOTONCHANNEL( source, nick, channelName ));
			return ;
		}
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
		std::string reason = getReason(tokens);
		kickUser(clientSocket, chan, nick, toKick, reason);
	}
}

// Kicks user if not the last chanops, displays action to all chanmembers, and deletes channel if needed.
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
			// TO DO ERASE CHANNEL IN CLIENTS VECTORS
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

// Gets the reason why the client is kicked of the channel. If no reason indicated, default reason displayed.
std::string		Server::getReason(std::vector<std::string> &tokens)
{
	std::string reason;
	if ( tokens.size() >= 3 && tokens[2].find(':', 0) != std::string::npos)
	{
		while (tokens[2].find(':', 0) != std::string::npos)
			tokens[2].erase(0, 1);
		for (size_t i = 2; i < tokens.size(); i++)
		{
			if (i != 2)
				reason += " ";
			reason += tokens[i];
		}
	}
	else
		reason = KICK_REASON;
	if (reason.size() > KICKLEN)
		reason = reason.substr(0, KICKLEN);
	return (reason);
}

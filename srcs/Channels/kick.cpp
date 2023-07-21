/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/19 18:19:57 by lmelard           #+#    #+#             */
/*   Updated: 2023/07/21 10:07:20 by codespace        ###   ########.fr       */
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
	std::cout << "param: " << param << std::endl;
	std::cout << "client socket: " << clientSocket << std::endl;
	// if no param are entered then a need more params err msg is displayed
    std::vector<std::string> tokens = splitString( param, ' ' );
    if (param.empty() || tokens.size() < 2)
    {
        replyMsg(clientSocket, ERR_NEEDMOREPARAMS( source, nick, "MODE"));
        return ;
    }
	// parsing params
	bool    isChannel = tokens[ 0 ].find( '#', 0 ) != std::string::npos;
	if (isChannel)
	{
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
			std::string reason = "bye bye looser!"; // reason by default
			// int socket = chan->getChannelMembers().at(toKick)->getFd();
			if ( tokens.size() > 3 ) //checking if a reason is passed as an argument
				reason = tokens[3];
			int socket;
            for (std::map<std::string, Client *>::iterator it = chan->getChannelMembers().begin(); it != chan->getChannelMembers().end(); it++)
            {
                socket = it->second->getFd();
				replyMsg(socket, DEFAULTKICK(nick, channelName, toKick, reason));
            }
			// remove client from channel members and channel operators
			chan->getChannelMembers().erase(toKick);
			chan->getChannelOps().erase(toKick);
		}
	}
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   who.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/24 18:42:25 by codespace         #+#    #+#             */
/*   Updated: 2023/07/24 23:16:28 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Socket.hpp"
#include "utils.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"

void    Server::handleWho( int clientSocket, std::string param )
{
    std::vector<std::string> tokens = splitString( param, ' ' );
    if (tokens.size() >= 1)
    {
        std::string source = _clients.at( clientSocket ).getSource();
	    std::string nick = _clients.at( clientSocket ).getNickname();
        bool    isChannel = tokens[0].find('#', 0) != std::string::npos;
        if (isChannel && existingChannel(tokens[0]))
        {
            Channel *chan = &_channels[tokens[0]];
            std::string channelName = tokens[0];
            for (std::map<std::string, Client *>::iterator it = chan->getChannelMembers().begin(); it != chan->getChannelMembers().end(); it++)
            {
                std::string flag = chan->checkChannelOps( nick ) == true ? "H@" : "H+";
                replyMsg(clientSocket, RPL_WHOREPLY(source, nick, channelName, it->second->getUsername(), SERVERNAME, it->second->getNickname(), flag, it->second->getRealname()));
            }
            replyMsg(clientSocket, RPL_ENDOFWHO(source, nick, tokens[0]));
        }
        else
        {
            std::string clientName = tokens[0];
            if (existingClient(clientName))
            {
                Client toDisplay;
                for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); it++)
                {
                    if (it->second.getNickname() == clientName)
                    {
                        toDisplay = it->second;
                        break ;
                    }
                }
                replyMsg(clientSocket, RPL_WHOREPLY(source, nick, "*", toDisplay.getUsername(), SERVERNAME, toDisplay.getNickname(), "H", toDisplay.getRealname()));
                replyMsg(clientSocket, RPL_ENDOFWHO(source, nick, tokens[0]));
            }
        }
    }
}

bool        Server::existingClient(std::string clientName)
{
    for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); it++)
    {
       if (it->second.getNickname() == clientName)
        return true;
    }
    return false;
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 10:14:07 by codespace         #+#    #+#             */
/*   Updated: 2023/07/24 12:20:47 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Socket.hpp"
#include "utils.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"

// The TOPIC command is used to change or view the topic of the given channel. 
void		Server::handleTopic( int clientSocket, std::string param )
{
    std::string source = _clients.at( clientSocket ).getSource();
	std::string nick = _clients.at( clientSocket ).getNickname();
    if (param.empty()) // if no param are entered then a need more params err msg is displayed
    {
        replyMsg(clientSocket, ERR_NEEDMOREPARAMS(source, nick, "TOPIC"));
        return ;
    }
    std::vector<std::string> tokens = splitString( param, ' ' );
    std::string channelName = tokens[ 0 ];
	if (channelName.size() > CHANNELLEN)  // cropping the first param (channel name) if its length is over the define CHANNELLEN
		channelName = channelName.substr(0, CHANNELLEN);
	if (!existingChannel( channelName ))  // if the channel entered doesn't exist no such Channel error displayed
		replyMsg(clientSocket, ERR_NOSUCHCHANNEL( source, nick, channelName));
    else
	{
        Channel *chan = &_channels[ channelName ];
        if ( !chan->checkChannelMembers( nick ) ) // checking if the client that makes the request is a channel member
		{
			replyMsg(clientSocket, ERR_NOTONCHANNEL( source, nick, channelName ));
			return ;
		}
        else if (tokens.size() < 2) // if <topic> is not given, RPL_TOPIC Or RPL_NOTOPIC returned
        {
            if (chan->getTopic().empty())
                replyMsg(clientSocket, RPL_NOTOPIC(source, nick, channelName));
            else
            {
                replyMsg(clientSocket, RPL_TOPIC(source, nick, channelName, chan->getTopic()));
                replyMsg(clientSocket, RPL_TOPICWHOTIME(source, nick, channelName, chan->getTopicSetter(), chan->getTopicDate()));
            }
            return ; 
        }
		else if ( chan->getTopicRestrictionStatus() == true && !chan->checkChannelOps( nick ) ) // error if the client doesn't have chanops privileges 
		{
			replyMsg(clientSocket, ERR_CHANOPRIVSNEEDED( source, nick, channelName ));
			return ;
		}
        std::string newTopic = getNewTopic(tokens);
        if (newTopic != chan->getTopic())
        {
            chan->setTopicDate( getCurrentDate() );
            chan->setTopic( newTopic );
            chan->setTopicSetter(nick);
            int socket;
            for ( std::map<std::string, Client *>::iterator it = chan->getChannelMembers().begin(); it != chan->getChannelMembers().end(); it++ )
            {
                socket = it->second->getFd();
                replyMsg(socket, RPL_TOPIC(source, it->second->getNickname(), channelName, chan->getTopic()));
                replyMsg(socket, RPL_TOPICWHOTIME(source, it->second->getNickname(), channelName, chan->getTopicSetter(), chan->getTopicDate()));
            }
        }
    }
}

std::string Server::getNewTopic( std::vector<std::string> &tokens )
{
    std::string newTopic = ""; 
    if ( tokens.size() >= 2 && tokens[1].find(':', 0) != std::string::npos)
    {
        while (tokens[1].find(':', 0) != std::string::npos)
			tokens[1].erase(0, 1);
        for (size_t i = 1; i < tokens.size(); i++)
		{
			if (i != 1)
				newTopic += " ";
			newTopic += tokens[i];
		}
    }
    else if (tokens.size() >= 2 && tokens[1].find(':', 0) == std::string::npos)
        newTopic = tokens[1];
    if (newTopic.size() > TOPICLEN)
        newTopic = newTopic.substr(0, TOPICLEN);
    return newTopic;
}
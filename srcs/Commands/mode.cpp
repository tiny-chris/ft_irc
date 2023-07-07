/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/06 16:05:53 by codespace         #+#    #+#             */
/*   Updated: 2023/07/07 16:16:38 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Socket.hpp"
#include "utils.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"

void		Server::handleMode( size_t cid, std::string param )
{
    std::string reply;
    std::vector<std::string>  tokens;
    std::istringstream        iss(param);
    std::string               token;
    if (param.compare("") == 0)
    {
        reply = ERR_NEEDMOREPARAMS(_clients[cid].getSource(), _clients[cid].getNickname(), "MODE");
        send(_clients[cid].getCfd(), reply.c_str(), reply.length(), 0);
        return ;
    }
    //parse param
    while (std::getline(iss, token, ' '))
        tokens.push_back(token);
    for (size_t i = 0; i < tokens.size(); ++i)
    {
        std::cout << "client " << _clients[cid].getNickname() << " - param[" << i << "] is <" << tokens[i] << ">" << std::endl;
    }
    // CHANNEL MODE
    // Do we have to check if the channels modes we indicates follow the same order as mode types (A, B, C and D) ?
    // Or can we modify them in any order ?
    if (param[0] == '#')
    {
        std::cout << "client " << _clients[cid].getNickname() << " - Channel Mode" << std::endl;
        if (tokens[0].size() > CHANNELLEN)
            tokens[0] = tokens[0].substr(0, CHANNELLEN);
        if (existingChannel(tokens[0]) == false) // TO CREATE
        {
            reply = ERR_NOSUCHCHANNEL(_clients[cid].getSource(), _clients[cid].getNickname(), tokens[0].substr(1, tokens[0].size() - 1));
            send(_clients[cid].getCfd(), reply.c_str(), reply.length(), 0);
        }
        else if (tokens.size() < 2)
        {
            reply = RPL_CHANNELMODEIS(_clients[cid].getSource(), _clients[cid].getNickname(), tokens[0], _clients[cid].getChannelModes());
            send(_clients[cid].getCfd(), reply.c_str(), reply.length(), 0);
        }
    }
    // USER MODE
    else
    {
        std::cout << "client " << _clients[cid].getNickname() << " - User Mode" << std::endl;
        if (tokens[0].size() > USERLEN)
            tokens[0] = tokens[0].substr(0, NICKLEN);
        if (existingNick(tokens[0]) == false)
        {
            reply = ERR_NOSUCHNICK(_clients[cid].getSource(), _clients[cid].getNickname());
            send(_clients[cid].getCfd(), reply.c_str(), reply.length(), 0);
        }
        else if (tokens[0].compare(_clients[cid].getNickname()) != 0 || tokens[0].size() != _clients[cid].getNickname().size())
        {
            reply = ERR_USERSDONTMATCH(_clients[cid].getSource(), _clients[cid].getNickname());
            send(_clients[cid].getCfd(), reply.c_str(), reply.length(), 0);
        }
        else if (tokens.size() < 2)
        {
            reply = RPL_UMODEIS(_clients[cid].getSource(), _clients[cid].getNickname(), _clients[cid].getUserModes());
            send(_clients[cid].getCfd(), reply.c_str(), reply.length(), 0);
        }
       else
        {
            bool test = _clients[cid].setUserModes(tokens[1]);
            if (test == true)
            {
                reply = MSG_MODE(_clients[cid].getSource(), _clients[cid].getNickname(), tokens[1].substr(0, 2));
                send(_clients[cid].getCfd(), reply.c_str(), reply.length(), 0);
            }
            if (test == false || tokens.size() > 2 || tokens[1].size() != 2)
            {
                reply = ERR_UMODEUNKNOWNFLAG(_clients[cid].getSource(), _clients[cid].getNickname());
                send(_clients[cid].getCfd(), reply.c_str(), reply.length(), 0);
            }
            std::cout <<  "client " << _clients[cid].getNickname() << " User Mode is: " << _clients[cid].getUserModes() << std::endl;
        }
    }
}

bool		Server::existingChannel(std::string param)
{
    for (std::vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); ++it)
    {
        if (it->getChannelName().compare(param) == 0 && it->getChannelName().size() == param.size())
        return true;
    }
    return false;
}
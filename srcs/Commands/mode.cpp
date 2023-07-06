/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/06 16:05:53 by codespace         #+#    #+#             */
/*   Updated: 2023/07/06 22:16:49 by codespace        ###   ########.fr       */
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
    if (param[0] == '#')
    {
        std::cout << "client " << _clients[cid].getNickname() << " - Channel Mode" << std::endl;
    }
    // USER MODE
    else
    {
        std::cout << "client " << _clients[cid].getNickname() << " - User Mode" << std::endl;
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
            // if (_clients[cid].getUserModes() == "i")
            //     applyMode(cid); // A CREER
            if (test == false || tokens.size() > 2)
            {
                reply = ERR_UMODEUNKNOWNFLAG(_clients[cid].getSource(), _clients[cid].getNickname());
                send(_clients[cid].getCfd(), reply.c_str(), reply.length(), 0);
            }
            std::cout <<  "client " << _clients[cid].getNickname() << " User Mode is: " << _clients[cid].getUserModes() << std::endl;
       }
    }
}
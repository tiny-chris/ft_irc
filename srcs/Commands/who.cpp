/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   who.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmelard <lmelard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/24 18:42:25 by codespace         #+#    #+#             */
/*   Updated: 2023/07/31 15:26:04 by lmelard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Socket.hpp"
#include "utils.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"

/**
 * @brief       This function checks if the nickname matches an existing Client
 */

bool        Server::existingClient(std::string clientName) {
    for (mapClients::iterator it = _clients.begin(); it != _clients.end(); it++) {
       if (it->second.getNickname() == clientName)
        return true;
    }
    return false;
}

/**
 * @brief       This function queries a list of users who match
 *              a channel name. The channel members are listed.
 */

void    Server::handleWhoChannel(int clientSocket, std::string source, std::string nick, std::string target)
{
        Channel *chan = &_channels[target];
        std::string channelName = target;
        for ( mapClientsPtr::iterator it = chan->getChannelMembers().begin(); it != chan->getChannelMembers().end(); it++ ) {
            std::string flag = chan->checkChannelOps( nick ) == true ? "H@" : "H+";
            replyMsg( clientSocket, RPL_WHOREPLY( source, nick, channelName, it->second->getUsername(), SERVERNAME, it->second->getNickname(), flag, it->second->getRealname() ) );
        }
        replyMsg( clientSocket, RPL_ENDOFWHO( source, nick, target ) );
}

/**
 * @brief       This function queries a list of users who match
 *              a nickname. a single user is returned
 */

void    Server::handleWhoClient(int clientSocket, std::string source, std::string nick, std::string target)
{
    Client toDisplay;
    for (mapClients::iterator it = _clients.begin(); it != _clients.end(); it++) {
        if (it->second.getNickname() == target) {
            toDisplay = it->second;
            break ;
        }
    }
    replyMsg(clientSocket, RPL_WHOREPLY(source, nick, "*", toDisplay.getUsername(), SERVERNAME, toDisplay.getNickname(), "H", toDisplay.getRealname()));
    replyMsg(clientSocket, RPL_ENDOFWHO(source, nick, target));
}

/**
 * @brief       WHO commande
 *              WHO <mask>
 * 
 *              This command is used to query a list of users who match
 *              - A channel name, in which case the channel members are listed.
 *              - An exact nickname, in which case a single user is returned.
 *              The WHO reply is always followed by a ENDOFWHO reply
 */

void    Server::handleWho( int clientSocket, std::string param ) {
    std::vector<std::string> tokens = splitString( param, ' ' );
    if (tokens.size() < 1) {
        return ;
    }
    std::string source = _clients.at( clientSocket ).getSource();
    std::string nick = _clients.at( clientSocket ).getNickname();
    std::string target = tokens[0];
    bool    isChannel = target.find('#', 0) != std::string::npos;
    if (isChannel && existingChannel(target)) {
        handleWhoChannel( clientSocket, source, nick, target);
    }
    else if (existingClient( target )) {
        handleWhoClient( clientSocket, source, nick, target );
    }
}

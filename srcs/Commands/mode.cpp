/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmelard <lmelard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/06 16:05:53 by codespace         #+#    #+#             */
/*   Updated: 2023/07/31 11:18:55 by lmelard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Socket.hpp"
#include "utils.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"

/**
 * @brief       The MODE command is used to set or remove options (or modes) from a given target.
 *              2 types of tagets : Channel and User
 */

void		Server::handleMode( int clientSocket, std::string param ) {
    if (param.empty()) {
        replyMsg(clientSocket, ERR_NEEDMOREPARAMS(_clients.at( clientSocket ).getSource(), _clients.at( clientSocket ).getNickname(), "MODE"));
        return ;
    }
    std::vector<std::string>tokens = splitString(param, ' ');
    bool    isChannelMode = tokens[0].find('#', 0) != std::string::npos;
    if (isChannelMode) {
        handleChannelMode(clientSocket, tokens[0], tokens);
    }
    else {
        handleUserMode(clientSocket, tokens);
    }
}

/**
 * @brief       Function that go through the modeString and its arguments 
 *              and set or unset the given channel modes
 */

void    Server::updateChannelMode(Channel *chan, const std::vector<std::string> & tokens, std::string &modeChange, std::string &modeArgs) {
    char modePrefix = getModePrefix(tokens[1]); // checking the mode prefix
    modeChange += modePrefix;
    std::string modeString = tokens[1].substr(1, tokens[1].size() - 1);
    size_t j = 2;
    for (size_t i = 0; i < modeString.size(); i++) { // checking which mode to set or unset depending on the prefix
        char    modeChar = modeString[i];
        if (!chan->isValidModeChar(modeChar)) {
            break;
        }
        if (modePrefix == '+') {
            chan->handleChannelModeSet(modeChar, &modeArgs, &modeChange, tokens, &j);
        }
        else if (modePrefix == '-' && modeString.size() >= 1) {
            chan->handleChannelModeUnset(modeChar, &modeArgs, &modeChange, tokens, &j);
        }
    }
    
}

/**
 * @brief       Set or unset modes from a given channel
 *              If no modestring given the function displays current channel modes
 *              The client need to be chanop to call this function
 *              All changes are displayed to channel members
 */

void		Server::handleChannelMode (int clientSocket, std::string& channelName, const std::vector<std::string> & tokens ) {
    Client *client = &_clients.at( clientSocket );
    std::string source = client->getSource();
    std::string clientName = client->getNickname();
    std::string modeChange, modeArgs;

    channelName = channelName.substr(0, CHANNELLEN);
    if (!existingChannel(channelName)) {
        replyMsg(clientSocket, ERR_NOSUCHCHANNEL(source, clientName, channelName));
        return ;
    }
    if (tokens.size() < 2) { // no param the modes set are displayed
        modeChange = _channels[channelName].getModes();
        modeArgs = _channels[channelName].getModesArgs();
        replyMsg(clientSocket, RPL_CHANNELMODEIS(source, clientName, channelName, modeChange, modeArgs));
        return ;
    }
    Channel *chan = &_channels[channelName];
    if (tokens[1] == "b") {
        replyMsg(clientSocket, RPL_ENDOFBANLIST(source, clientName, channelName));
        return ; 
    }
    if (!chan->checkChannelOps(clientName) && !client->getOperatorMode()) {
        replyMsg(clientSocket, ERR_CHANOPRIVSNEEDED(source, clientName, channelName));
        return ;
    }
    updateChannelMode(chan, tokens, modeChange, modeArgs);
    if (modeChange.size() > 1) { // Displaying channel modes changes to every channel client
        channelMsgToAll(clientSocket, channelName, MSG_MODE_CUSTOM(source, channelName, modeChange + " " + modeArgs));
    }
}

/**
 * @brief       Function that displays user mode updates and errors
 */

void    Server::displayUserModeChanges(Client *client, const std::vector<std::string> & tokens, std::string &modechange) {
     if (modechange.size() > 1) {
        replyMsg(client->getFd(), MSG_MODE(client->getSource(), client->getNickname(), modechange, ""));
        int diff = tokens[1].size() - modechange.size();
        while (diff) {
            replyMsg(client->getFd(), ERR_UMODEUNKNOWNFLAG(client->getSource(), client->getNickname()));
            diff--;
        }
    }
}

/**
 * @brief       Function that go through the modeString  
 *              and set or unset the given user modes
 */

void    Server::updateUserMode(Client *client, const std::vector<std::string> & tokens, std::string &modechange) {
    char modePrefix = getModePrefix(tokens[1]);
    modechange += modePrefix;
    std::string modeString = tokens[1].substr(1, tokens[1].size() - 1);
    for (size_t i = 0; i < modeString.size(); i++) {
        char modeChar = modeString[i];
        if (modePrefix == '+') {
            client->handleUserModeSet(modeChar, &modechange);
        }
        else if (modePrefix == '-') {
            client->handleUserModeUnset(modeChar, &modechange);
        }
    }
}

/**
 * @brief       Set or unset modes from a given user
 *              If no modestring given the function displays current user modes
 *              All changes are displayed to client
 */

void		Server::handleUserMode (int clientSocket, std::vector<std::string> & tokens ) {
    std::string userName = tokens[0].substr(0, NICKLEN);
    std::string modechange;
    Client *client = &_clients.at( clientSocket );
    if (!existingNick(userName)) {
        replyMsg(clientSocket, ERR_NOSUCHNICK( client->getSource(), userName ) );
        return ;
    }
    if (userName != client->getNickname()) {
        replyMsg(clientSocket, ERR_USERSDONTMATCH( client->getSource(), client->getNickname() ) );
        return ;
    }
    if (tokens.size() < 2) { // if not mode string entered the current usermodes are displayed
        modechange = client->getModes();
        replyMsg(clientSocket, RPL_UMODEIS( client->getSource(), client->getNickname(), modechange ));
        return ;
    }
    updateUserMode(client, tokens, modechange);
    if (modechange.size() > 1) {
        replyMsg(clientSocket, MSG_MODE(client->getSource(), client->getNickname(), modechange, ""));
        int diff = tokens[1].size() - modechange.size();
        while (diff) {
            replyMsg(clientSocket, ERR_UMODEUNKNOWNFLAG(client->getSource(), client->getNickname()));
            diff--;
        }
    }
}

/**
 * @brief       Checks if the Channel exists on the server
 */

bool		Server::existingChannel(std::string param) {
    if (_channels.find(param) != _channels.end()) {
        return true;
    }
    return false;
}

/**
 * @brief       Gets the prefix of the modestring
 */

char	Server::getModePrefix( std::string const& token ) {
    char sign = 'O';
    if (token.find('+', 0) != std::string::npos) {
        return (sign = '+');
    }
    if (token.find('-', 0) != std::string::npos) {
        return (sign = '-');
    }
    return (sign);
}

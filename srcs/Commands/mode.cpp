/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmelard <lmelard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/06 16:05:53 by codespace         #+#    #+#             */
/*   Updated: 2023/07/27 17:03:16 by lmelard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Socket.hpp"
#include "utils.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"

void		Server::handleMode( int clientSocket, std::string param )
{
    // if no param are entered then a need more params err msg is displayed
    if (param.empty())
    {
        replyMsg(clientSocket, ERR_NEEDMOREPARAMS(_clients.at( clientSocket ).getSource(), _clients.at( clientSocket ).getNickname(), "MODE"));
        return ;
    }
    // parsing params
    std::vector<std::string>tokens = splitString(param, ' ');
    bool    isChannelMode = tokens[0].find('#', 0) != std::string::npos;
    if (isChannelMode)
        handleChannelMode(clientSocket, tokens[0], tokens);
    else
        handleUserMode(clientSocket, tokens);
}

// CHANGING CHANNEL MODE (type B +k, Type C +l, Type D +it)
// Types B and C modes need a valid mode argument otherwise they are ignored
void		Server::handleChannelMode (int clientSocket, std::string& channelName, const std::vector<std::string> & tokens )
{
    std::string modeChange;
    std::string modeArgs;

    // cropping the first param (channel name) if its length is over the define CHANNELLEN
    if (channelName.size() > CHANNELLEN)
        channelName = channelName.substr(0, CHANNELLEN);
    // if the channel entered doesn't exist no such Channel error displayed
    if (!existingChannel(channelName))
        replyMsg(clientSocket, ERR_NOSUCHCHANNEL(_clients.at( clientSocket ).getSource(), _clients.at( clientSocket ).getNickname(), channelName));
    // else if a valid Channel is entered but no param the modes set are displayed
    else if (tokens.size() < 2)
    {
        modeChange = _channels[channelName].getModes();
        modeArgs = _channels[channelName].getModesArgs();
        replyMsg(clientSocket, RPL_CHANNELMODEIS(_clients.at( clientSocket ).getSource(), _clients.at( clientSocket ).getNickname(), tokens[0], modeChange, modeArgs));
    }
    else
    {
        Channel *chan = &_channels[channelName];
        std::string clientName = _clients.at( clientSocket ).getNickname();
        // if the user is not a channel operator, then an error msg is returned and the command is ignored
        if (tokens[1] == "b")
        {
            replyMsg(clientSocket, RPL_ENDOFBANLIST(_clients.at( clientSocket ).getSource(), _clients.at( clientSocket ).getNickname(), tokens[0]));
            return ; 
        }
        if (!chan->checkChannelOps(clientName))
        {
            replyMsg(clientSocket, ERR_CHANOPRIVSNEEDED(_clients.at( clientSocket ).getSource(), _clients.at( clientSocket ).getNickname(), channelName));
            return ;
        }
        // checking the mode prefix
        char modePrefix = getModePrefix(tokens[1]);
        modeChange += modePrefix;
        std::string modeString = tokens[1].substr(1, tokens[1].size() - 1);
        size_t j = 2;
        // checking which mode to set or unset depending on the prefix
        for (size_t i = 0; i < modeString.size(); i++)
        {
            char    modeChar = modeString[i];
            if (!chan->isValidModeChar(modeChar))
                break;
            if (modePrefix == '+')
                chan->handleChannelModeSet(modeChar, &modeArgs, &modeChange, tokens, &j);
            else if (modePrefix == '-' && modeString.size() >= 1)
                chan->handleChannelModeUnset(modeChar, &modeArgs, &modeChange, tokens, &j);
        }
        // Displaying channel modes changes to every channel client
        if (modeChange.size() > 1)
        {
            int socket;
            for (std::map<std::string, Client *>::iterator it = chan->getChannelMembers().begin(); it != chan->getChannelMembers().end(); it++)
            {
                socket = it->second->getFd();
                replyMsg(socket, MSG_MODE_CUSTOM(_clients.at( socket ).getSource(), channelName, modeChange + " " + modeArgs));

            }
        }
    }
}

// CHANGING USER MODE (only one mode +i)
void		Server::handleUserMode (int clientSocket, std::vector<std::string> & tokens )
{
    std::string userName = tokens[0];
    std::string modechange;
    Client *client = &_clients.at( clientSocket );
    // cropping the first param (nickname) if its length is over the define USERLEN
    if (userName.size() > NICKLEN)
        userName = userName.substr(0, NICKLEN);
     // if the nickname entered doesn't exist no such Channel error displayed
    if (!existingNick(userName))
        replyMsg(clientSocket, ERR_NOSUCHNICK( client->getSource(), client->getNickname()));
    // else if the username doesn't match the client nickname User Don't Match Error
    else if (userName != client->getNickname())
        replyMsg(clientSocket, ERR_USERSDONTMATCH( client->getSource(), client->getNickname()));
    // if not mode string entered the current usermodes are displayed
    else if (tokens.size() < 2)
    {
        // std::string modechange = _clients.at( clientSocket ).getInvisibleMode() == true ? "+i" : "";
        modechange = client->getModes();
        replyMsg(clientSocket, RPL_UMODEIS(client->getSource(), client->getNickname(), modechange));
    }
    else
    {
        char modePrefix = getModePrefix(tokens[1]);
        modechange += modePrefix;
        std::string modeString = tokens[1].substr(1, tokens[1].size() - 1);
        for (size_t i = 0; i < modeString.size(); i++)
        {
            char modeChar = modeString[i];
            if (modePrefix == '+')
                client->handleUserModeSet(modeChar, &modechange);
            else if (modePrefix == '-')
                client->handleUserModeUnset(modeChar, &modechange);
        }
        if (modechange.size() > 1)
        {
            replyMsg(clientSocket, MSG_MODE(client->getSource(), client->getNickname(), modechange, ""));
            int diff = tokens[1].size() - modechange.size();
            while (diff)
            {
                replyMsg(clientSocket, ERR_UMODEUNKNOWNFLAG(client->getSource(), client->getNickname()));
                diff--;
            }
        }
    }
}

// Checks if the Channel exists on the server
bool		Server::existingChannel(std::string param)
{
    if (_channels.find(param) != _channels.end())
        return true;
    return false;
}

// Gets the prefix of the modestring
char	Server::getModePrefix( std::string const& token )
{
    char sign = 'O';

    if (token.find('+', 0) != std::string::npos)
    {
        sign = '+';
        return (sign);
    }
    if (token.find('-', 0) != std::string::npos)
    {
        sign = '-';
        return (sign);
    }
    return (sign);
}

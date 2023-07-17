/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmelard <lmelard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/06 16:05:53 by codespace         #+#    #+#             */
/*   Updated: 2023/07/17 11:59:40 by lmelard          ###   ########.fr       */
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
    // if no param are entered then a need more params err msg is displayed
    if (param.empty())
    {
        replyMsg(cid, ERR_NEEDMOREPARAMS(_clients[cid].getSource(), _clients[cid].getNickname(), "MODE"));
        return ;
    }
    // parsing params
    std::vector<std::string>tokens = splitString(param, ' ');
    bool    isChannelMode = tokens[0].find('#', 0) != std::string::npos;
    if (isChannelMode)
        handleChannelMode(cid, tokens[0], tokens);
    else
        handleUserMode(cid, tokens);
}

// CHANGING CHANNEL MODE (type B +k, Type C +l, Type D +it)
// Types B and C modes need a valid mode argument otherwise they are ignored
void		Server::handleChannelMode (size_t cid, std::string& channelName, const std::vector<std::string> & tokens )
{
    std::string modeChange;
    std::string modeArgs;
    
    // cropping the first param (channel name) if its length is over the define CHANNELLEN
    if (channelName.size() > CHANNELLEN)
        channelName = channelName.substr(0, CHANNELLEN);
    // if the channel entered doesn't exist no such Channel error displayed
    if (!existingChannel(channelName))
        replyMsg(cid, ERR_NOSUCHCHANNEL(_clients[cid].getSource(), _clients[cid].getNickname(), channelName));
    // else if a valid Channel is entered but no param the modes set are displayed 
    else if (tokens.size() < 2)
    {
        modeChange = _channels[channelName].getModes();
        modeArgs = _channels[channelName].getModesArgs();
        replyMsg(cid, RPL_CHANNELMODEIS(_clients[cid].getSource(), _clients[cid].getNickname(), tokens[0], modeChange, modeArgs));
    }
    else
    {
        Channel *chan = &_channels[channelName];
        std::string clientName = _clients[cid].getNickname();
        // if the user is not a channel operator, then an error msg is returned and the command is ignored
        if (!chan->checkChannelOps(clientName))
        {
            replyMsg(cid, ERR_CHANOPRIVSNEEDED(_clients[cid].getSource(), _clients[cid].getNickname(), channelName));
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
            if (!isValidModeChar(modeChar))
                break;
            if (modePrefix == '+')
                handleChannelModeSet(chan, modeChar, &modeArgs, &modeChange, tokens, &j);
            else if (modePrefix == '-' && modeString.size() >= 1)
                handleChannelModeUnset(chan, modeChar, &modeChange);
        }
        // Displaying channel modes changes to every channel client
        if (modeChange.size() > 1) //
            replyMsg(cid, MSG_MODE(_clients[cid].getSource(), _clients[cid].getNickname(), modeChange, modeArgs));
    }
}

// Prefix + -> Sets new channel modes       
void		Server::handleChannelModeSet(Channel *chan, char modeChar, std::string* modeArgs, std::string* modeChange,\
    const std::vector<std::string>& tokens, size_t *j)
{    
    switch (modeChar)
    {
        case 'k':
            handleModeSetKey(chan, modeArgs, modeChange, tokens, j);
            break;
        case 'l':
            handleModeSetLimit(chan, modeArgs, modeChange, tokens, j);
            break;
        case 'i':
            handleModeSetInviteOnly(chan, modeChange);
            break;
        case 't':
            handleModeSetTopicRestriction(chan, modeChange);
            break;
    }
}

// Sets key mode +k <modearg>
void		Server::handleModeSetKey(Channel *chan, std::string* modeArgs, std::string* modeChange,\
    const std::vector<std::string> &tokens, size_t *j)
{
    if (tokens.size() > *j && chan->getKeyStatus() == false && isValidParam(tokens[*j]) == true)
    {
        chan->setKeyStatus(true);
        chan->setKey(tokens[*j]);
        *modeChange += "k";
        *modeArgs += tokens[*j];
    }
    (*j)++;
}

// Sets limit mode +l <modearg>
void 		Server::handleModeSetLimit(Channel *chan, std::string* modeArgs, std::string* modeChange,\
    const std::vector<std::string> &tokens, size_t *j)
{
    if (tokens.size() > *j && chan->getLimitStatus() == false && chan->checkValidLimit(tokens[*j]) == true)
    {
        chan->setLimitStatus(true);
        chan->setLimit(tokens[*j]);
        *modeChange += "l";
        *modeArgs += tokens[*j];
    }
    (*j)++;
}

// Sets invite only mode +i 
void 		Server::handleModeSetInviteOnly(Channel *chan, std::string* modeChange)
{
    if (chan->getInviteOnlyStatus() == false)
    {
        chan->setInviteOnlyStatus(1);
        *modeChange += "i";
    }
}

// Sets topic restriction change mode +t 
void 		Server::handleModeSetTopicRestriction(Channel *chan, std::string* modeChange)
{
    if (chan->getTopicRestrictionStatus() == false)
    {
        chan->setTopicRestrictionStatus(1);
        *modeChange += "t";
    }
}

// Prefix - -> Unsets channel modes 
void		Server::handleChannelModeUnset(Channel *chan, char modeChar, std::string* modeChange)
{
    switch(modeChar)
    {
        case 'k':
            handleModeUnsetKey(chan, modeChange);
            break;
        case 'l':
            handleModeUnsetLimit(chan, modeChange);
            break;
        case 'i':
            handleModeUnsetInviteOnly(chan, modeChange);
            break;
        case 't':
            handleModeUnsetTopicRestriction(chan, modeChange);
            break;
    }
}

// Unsets key mode          
void		Server::handleModeUnsetKey(Channel *chan, std::string* modeChange)
{
    if (chan->getKeyStatus() == true)
    {
        chan->setKeyStatus(false);
        chan->setKey("");
        *modeChange += "k";
    }
}

// Unsets limit mode  
void		Server::handleModeUnsetLimit(Channel *chan, std::string* modeChange)
{
    if (chan->getLimitStatus() == true)
    {
        chan->setLimitStatus(false);
        chan->setLimit("");
        *modeChange += "l";
    }
}

// Unsets invite only mode  
void		Server::handleModeUnsetInviteOnly(Channel *chan, std::string* modeChange)
{
    if (chan->getInviteOnlyStatus() == true)
    {
        chan->setInviteOnlyStatus(false);
        *modeChange += "i";
    }
}

// Unsetstopic restriction mode  
void		Server::handleModeUnsetTopicRestriction(Channel *chan, std::string* modeChange)
{
    if (chan->getTopicRestrictionStatus() == true)
    {
        chan->setTopicRestrictionStatus(false);
        *modeChange += "t";
    }
}

// CHANGING USER MODE (only one mode +i)
void		Server::handleUserMode (size_t cid, std::vector<std::string> & tokens )
{
    std::string userName = tokens[0];
    std::string modechange;
    // cropping the first param (nickname) if its length is over the define USERLEN
    if (userName.size() > NICKLEN)
        userName = userName.substr(0, NICKLEN);
     // if the nickname entered doesn't exist no such Channel error displayed
    if (!existingNick(userName))
        replyMsg(cid, ERR_NOSUCHNICK(_clients[cid].getSource(), _clients[cid].getNickname()));
    // else if the username doesn't match the client nickname User Don't Match Error
    else if (userName != _clients[cid].getNickname())
        replyMsg(cid, ERR_USERSDONTMATCH(_clients[cid].getSource(), _clients[cid].getNickname()));
    // if not mode string entered the current usermodes are displayed
    else if (tokens.size() < 2)
    {
        std::string modechange = _clients[cid].getUserModes() == true ? "+i" : "";
        replyMsg(cid, RPL_UMODEIS(_clients[cid].getSource(), _clients[cid].getNickname(), modechange));
    }
    // else if a mode string is entered the usermode is modified
    else
    {
        if (tokens.size() > 2 || tokens[1].size() != 2)
            replyMsg(cid, ERR_UMODEUNKNOWNFLAG(_clients[cid].getSource(), _clients[cid].getNickname()));
        std::string modeString = tokens[1];
        if ((getModePrefix(modeString) == '+' || getModePrefix(modeString) == '-') && modeString.find('i', 1) != std::string::npos)
        {
            modechange += getModePrefix(modeString);
            if ((_clients[cid].getUserModes() == true && modechange == "-") || \
                 (_clients[cid].getUserModes() == false && modechange == "+"))
                 {
                    replyMsg(cid, MSG_MODE(_clients[cid].getSource(), _clients[cid].getNickname(), modechange + "i", ""));
                    _clients[cid].getUserModes() == true ? _clients[cid].setUserModes(false) : _clients[cid].setUserModes(true);
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

// Checks if the mode is a valid mode k,l,i,t
bool		Server::isValidModeChar( char const modeChar )
{
    if (modeChar == 'k' || modeChar == 'l' || modeChar == 'i' || modeChar == 't')
        return true;
    return false;
}
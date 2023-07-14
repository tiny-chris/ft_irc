/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/06 16:05:53 by codespace         #+#    #+#             */
/*   Updated: 2023/07/14 19:08:33 by codespace        ###   ########.fr       */
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
    std::vector<std::string>  tokens;
    // if no param are entered then a need more params err msg is displayed
    if (param.empty())
    {
        replyMsg(cid, ERR_NEEDMOREPARAMS(_clients[cid].getSource(), _clients[cid].getNickname(), "MODE"));
        return ;
    }
    // parsing params
    tokens = splitString(param, ' ');
    // que se passe t il si juste #
    if (param[0] == '#') // its a channel
    {
        // CHANGING CHANNEL MODE (type B +k, Type C +l, Type D +it)
        // Types B and C modes need a valid mode argument otherwise they are ignored
        std::string modechange;
        std::string modeargs;
        
        // cropping the first param (channel lenght) if its length is over the define CHANNELLEN
        if (tokens[0].size() > CHANNELLEN)
            tokens[0] = tokens[0].substr(0, CHANNELLEN);
        // if the channel entered doesn't exist no such Channel error displayed
        if (existingChannel(tokens[0]) == false)
            replyMsg(cid, ERR_NOSUCHCHANNEL(_clients[cid].getSource(), _clients[cid].getNickname(), tokens[0]));
        // else if a valid Channel is entered but no param the modes set are displayed 
        else if (tokens.size() < 2)
        {
            modechange = _channels[tokens[0]]->getModes();
            modeargs = _channels[tokens[0]]->getModesArgs();
            replyMsg(cid, RPL_CHANNELMODEIS(_clients[cid].getSource(), _clients[cid].getNickname(), tokens[0], modechange, modeargs));
        }
        else
        {
            Channel *chan = _channels[tokens[0]];
            std::string clientName = _clients[cid].getNickname();
            std::cout << "chan->_channelOps.size(): " << chan->_channelOps.size() << std::endl;
            // if the user is not a channel operator, then an error msg is returned and the command is ignored
            if (chan->checkChannelOps(clientName) == false)
            {
                replyMsg(cid, ERR_CHANOPRIVSNEEDED(_clients[cid].getSource(), _clients[cid].getNickname(), tokens[0]));
                return ;
            }
            //  if the modestring starts with a + and has at least a mode letter
            if (tokens[1][0] == '+' && tokens[1].size() >= 2)
            {
                int j = 2;
                modechange += "+";
                // setting the indicated mode if its unset and checking mode args for +k and +l
                // if they are invalid the request is ignored
                for (size_t i = 1; i < tokens[1].size(); i++)
                {
                    if (tokens[1][i] == 'k' && tokens.size() > 2)
                    {
                        if (chan->getKeyStatus() == 0 && isValidParam(tokens[j]) == true)
                        {
                            chan->setKeyStatus(1);
                            chan->setKey(tokens[j]);
                            modechange += "k";
                            modeargs += tokens[j];
                        }
                        j++;
                    }
                    else if (tokens[1][i] == 'l' && tokens.size() > 2)
                    {
                        if (chan->getLimitStatus() == 0 && chan->checkValidLimit(tokens[j]) == true)
                        {
                            chan->setLimitStatus(1);
                            chan->setLimit(tokens[j]);
                            modechange += "l";
                            modeargs += tokens[j];
                        }
                        j++;
                    }
                    else if (tokens[1][i] == 'i')
                    {
                        if (chan->getInviteOnlyStatus() == 0)
                        {
                            chan->setInviteOnlyStatus(1);
                            modechange += "i";
                        }
                    }
                    else if (tokens[1][i] == 't')
                    {
                        if (chan->getTopicRestrictionStatus() == 0)
                        {
                            chan->setTopicRestrictionStatus(1);
                            modechange += "t";
                        }
                    }
                    else
                        break;
                }
                // displaying all mode changes to every channel members
                if (modechange.size() > 1)
                    replyMsg(cid, MSG_MODE(_clients[cid].getSource(), _clients[cid].getNickname(), modechange, modeargs));
            }
            //  if the modestring starts with a - and has at least a mode letter
            else if (tokens[1][0] == '-' && tokens[1].size() >= 2)
            {
                std::cout << "unset mode WIP" << std::endl;
                for (size_t i = 1; i < tokens[1].size(); i++)
                {
                    modechange += "-";
                    if (tokens[1][i] == 'k')
                    {
                        if (chan->getKeyStatus() == 1)
                        {
                            chan->setKeyStatus(0);
                            chan->setKey("");
                            modechange += "k";
                        }
                    }
                    else if (tokens[1][i] == 'l')
                    {
                        if (chan->getLimitStatus() == 1)
                        {
                            chan->setLimitStatus(0);
                            chan->setLimit("");
                            modechange += "l";
                        }
                    }
                    else if (tokens[1][i] == 'i')
                    {
                        if (chan->getInviteOnlyStatus() == 1)
                        {
                            chan->setInviteOnlyStatus(0);
                            modechange += "i";
                        }
                    }
                    else if (tokens[1][i] == 't')
                    {
                        if (chan->getTopicRestrictionStatus() == 1)
                        {
                            chan->setTopicRestrictionStatus(0);
                            modechange += "t";
                        }
                    }
                    else
                        break;
                }
                if (modechange.size() > 1)
                    replyMsg(cid, MSG_MODE(_clients[cid].getSource(), _clients[cid].getNickname(), modechange, ""));
            }
        }
    }
    // CHANGING USER MODE (only one mode +i)
    else
        handleUserMode(cid, tokens);
}

void		Server::handleUserMode (size_t cid, std::vector<std::string> & tokens )
{
        
    std::cout << "client " << _clients[cid].getNickname() << " - User Mode" << std::endl;
    if (tokens[0].size() > USERLEN)
        tokens[0] = tokens[0].substr(0, NICKLEN);
    if (existingNick(tokens[0]) == false)
        replyMsg(cid, ERR_NOSUCHNICK(_clients[cid].getSource(), _clients[cid].getNickname()));
    else if (tokens[0].compare(_clients[cid].getNickname()) != 0 || tokens[0].size() != _clients[cid].getNickname().size())
        replyMsg(cid, ERR_USERSDONTMATCH(_clients[cid].getSource(), _clients[cid].getNickname()));
    else if (tokens.size() < 2)
        replyMsg(cid, RPL_UMODEIS(_clients[cid].getSource(), _clients[cid].getNickname(), _clients[cid].getUserModes()));
    else
    {
        bool test = _clients[cid].setUserModes(tokens[1]);
        if (test == true)
            replyMsg(cid, MSG_MODE(_clients[cid].getSource(), _clients[cid].getNickname(), tokens[1].substr(0, 2), ""));
        if (test == false || tokens.size() > 2 || tokens[1].size() != 2)
            replyMsg(cid, ERR_UMODEUNKNOWNFLAG(_clients[cid].getSource(), _clients[cid].getNickname()));
        std::cout <<  "client " << _clients[cid].getNickname() << " User Mode is: " << _clients[cid].getUserModes() << std::endl;
    }
}

// void		Server::handleChannelMode (size_t cid, std::vector<std::string> & tokens )
// {
//     std::cout << "handle channel mode " << std::endl;
// }

bool		Server::existingChannel(std::string param)
{
    for (std::map<std::string, Channel*>::iterator it = (_channels.begin()); it != _channels.end(); ++it)
    {
        if (it->first.compare(param) == 0 && it->first.size() == param.size())
        return true;
    }
    return false;
}

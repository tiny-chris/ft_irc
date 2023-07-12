/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmelard <lmelard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/06 16:05:53 by codespace         #+#    #+#             */
/*   Updated: 2023/07/12 18:27:07 by lmelard          ###   ########.fr       */
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

    if (param.compare("") == 0)
    {
        replyMsg(cid, ERR_NEEDMOREPARAMS(_clients[cid].getSource(), _clients[cid].getNickname(), "MODE"));
        return ;
    }
    //parse param
    tokens = splitString(param, ' ');
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
        if (existingChannel(tokens[0].substr(1, tokens[0].size() - 1)) == false) // TO CREATE
            replyMsg(cid, ERR_NOSUCHCHANNEL(_clients[cid].getSource(), _clients[cid].getNickname(), tokens[0].substr(1, tokens[0].size() - 1)));
        else if (tokens.size() < 2)
            replyMsg(cid, RPL_CHANNELMODEIS(_clients[cid].getSource(), _clients[cid].getNickname(), tokens[0], _clients[cid].getChannelModes()));
        else
        {
            Channel *chan = _channels[tokens[0].substr(1, tokens[0].size() - 1)];
            std::string clientName = _clients[cid].getNickname();
            // if the user is not a channel operator, then an error msg is returned and the command is ignored
            // if (chan->checkChannelOps(clientName) == false)
            // {
            //     replyMsg(cid, ERR_CHANOPRIVSNEEDED(_clients[cid].getSource(), _clients[cid].getNickname(), tokens[0]));
            //     return ;
            // }
            std::string modechange;
            //  if the modestring starts with a + and has at least a mode letter
            if (tokens[1][0] == '+' && tokens[1].size() >= 2)
            {
                std::cout << "enter +" << std::endl;
                int j = 2;
                modechange.push_back('+');
                for (size_t i = 1; i < tokens[1].size(); i++)
                {
                    if (tokens[1][i] == 'k' && tokens.size() > 2)
                    {
                        // check token[j] est une valid key ->
                        if (chan->getKeyStatus() == false)
                        {
                            chan->setKeyStatus(true);
                            chan->setKey(tokens[j]);
                            modechange.push_back('k');
                            std::cout << "mode";
                        }
                        j++;
                    }
                    else if (tokens[1][i] == 'l' && tokens.size() > 2)
                    {
                        // check token[j] est une valid limit ->
                        if (chan->getLimitStatus() == false)
                        {
                            chan->setLimitStatus(true);
                            chan->setLimit(tokens[j]);
                            modechange.push_back('l');
                        }
                        j++;
                    }
                    else if (tokens[1][i] == 'i')
                    {
                        if (chan->getInviteOnlyStatus() == false)
                        {
                            chan->setInviteOnlyStatus(true);
                            modechange.push_back('i');
                        }
                    }
                    else if (tokens[1][i] == 't')
                    {
                        if (chan->getTopicRestrictionStatus() == false)
                        {
                            chan->setTopicRestrictionStatus(true);
                            modechange.push_back('t');
                        }
                    }
                    else
                        break;
                }
                if (modechange.size() > 1)
                    replyMsg(cid, MSG_MODE(_clients[cid].getSource(), _clients[cid].getNickname(), modechange));
            //        d'un type B ou d'un type C y a t il un autre param derriere ?
            //          - si oui: on change bien le mode
            //          - si non: IGNORER CE MODE
            //        d'un + type D pas besoin d'un autre param derriere
            }
            //  if the modestring starts with a - and has at least a mode letter
            else if (tokens[1][0] == '-' && tokens[1].size() >= 2)
            {
                std::cout << "unset mode WIP" << std::endl;
            }
            // si oui :
            //  - si c'est un -
            //  - si c'est ni un + ni un - : Quel ERR MSG ?
            //  - si il y a d'autre param apres les + et les - : Quel ERR MSG ?
            //  Lorsque les modes ont ete changes alors une commande MODE est envoyee a tous les membres du channel
        }
    }
    // USER MODE
    else
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
                replyMsg(cid, MSG_MODE(_clients[cid].getSource(), _clients[cid].getNickname(), tokens[1].substr(0, 2)));
            if (test == false || tokens.size() > 2 || tokens[1].size() != 2)
                replyMsg(cid, ERR_UMODEUNKNOWNFLAG(_clients[cid].getSource(), _clients[cid].getNickname()));
            std::cout <<  "client " << _clients[cid].getNickname() << " User Mode is: " << _clients[cid].getUserModes() << std::endl;
        }
    }
}

bool		Server::existingChannel(std::string param)
{
    for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
    {
        if (it->first.compare(param) == 0 && it->first.size() == param.size())
        return true;
    }
    return false;
}
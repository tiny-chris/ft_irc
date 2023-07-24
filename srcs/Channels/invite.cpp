/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/24 13:05:13 by codespace         #+#    #+#             */
/*   Updated: 2023/07/24 17:25:51 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Socket.hpp"
#include "utils.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"

/**
   Checks if the client has the rights to invite an other client to a channel, 
   if the channels and the client to add exist, 
   and if the client to add is not already on the channel
   Then adds the client to the invited client vector.
**/
void    Server::handleInvite( int clientSocket, std::string param )
{
    std::string source = _clients.at( clientSocket ).getSource();
	std::string nick = _clients.at( clientSocket ).getNickname();
    std::vector<std::string> tokens = splitString( param, ' ' ); 
    
    if ( param.empty() || tokens.size() < 2 ) // if no param are entered then a need more params err msg is displayed
    {
        replyMsg(clientSocket, ERR_NEEDMOREPARAMS( source, nick, "KICK"));
        return ;
    }
    std::string channelName = tokens[ 1 ];
	if (channelName.size() > CHANNELLEN) // cropping the first param (channel name) if its length is over the define CHANNELLEN
		channelName = channelName.substr(0, CHANNELLEN);
	if (!existingChannel( channelName )) // if the channel entered doesn't exist no such Channel error displayed
    {
		replyMsg(clientSocket, ERR_NOSUCHCHANNEL( source, nick, channelName));
        return ;
    }
	Channel *chan = &_channels[ channelName ];
    if ( !chan->checkChannelMembers( nick ) ) // checking if the client that makes the request is a channel member
    {
        replyMsg(clientSocket, ERR_NOTONCHANNEL( source, nick, channelName ));
        return ;
    }
    if ( !chan->checkChannelOps( nick ) ) // if the client doesn't have chanops privileges 
    {
        replyMsg(clientSocket, ERR_CHANOPRIVSNEEDED( source, nick, channelName ));
        return ;
    }
    std::string toInvite = tokens[ 0 ]; // checks that the name of the client to invite is not too long
    if (toInvite.size() > NICKLEN)
        toInvite.substr(0, NICKLEN);
    if ( chan->checkChannelMembers( toInvite ) ) // checking if the client to invite is not already chanmember
    {
        replyMsg(clientSocket, ERR_USERONCHANNEL(source, nick, toInvite, channelName ));
        return ;
    }
    inviteClientToChannel( clientSocket, nick, toInvite, chan );
}

/* 
    Adding the client to the invited vector if he exists
    Sending an invite reply to the client
*/
void    Server::inviteClientToChannel( int clientSocket, std::string clientNick, std::string nameInvitee, Channel *chan )
{
    Client *toAdd = NULL;
    for ( std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); it++ )
	{
		if (it->second.getNickname() == nameInvitee)
        {
            toAdd = &it->second;
            break;
        }
	}
    if (toAdd == NULL)
        std::cout << "Error: No such Nickname, cannot invite this client" << std::endl;
    else // the invited clients receives an Invite Reply (only him)
    {
        chan->addInvitedMember(nameInvitee);
        replyMsg(clientSocket, RPL_INVITING(_clients.at( clientSocket ).getSource(), clientNick, nameInvitee, chan->getChannelName()));
        replyMsg(toAdd->getFd(), INVITE(clientNick, nameInvitee, chan->getChannelName()));
    }
    
}

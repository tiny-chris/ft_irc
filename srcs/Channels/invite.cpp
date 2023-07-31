/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmelard <lmelard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/24 13:05:13 by codespace         #+#    #+#             */
/*   Updated: 2023/07/31 11:59:38 by lmelard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Socket.hpp"
#include "utils.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"

/**
 * @brief       INVITE command
 *              INVITE <nickname> <channel>
 *             
 *               Invites a client on a Channel
 *              - Checks rights to invite
 *              - Checks on invited client
 *              - Invites client on channel
 */

void    Server::handleInvite( int clientSocket, std::string param ) {
    std::string source = _clients.at( clientSocket ).getSource();
	std::string nick = _clients.at( clientSocket ).getNickname();
    std::vector<std::string> tokens = splitString( param, ' ' ); 
    
    if ( param.empty() || tokens.size() < 2 ) {// if no param are entered then a need more params err msg is displayed 
        replyMsg(clientSocket, ERR_NEEDMOREPARAMS( source, nick, "KICK"));
        return ;
    }
    std::string channelName = tokens[ 1 ].substr(0, CHANNELLEN); // crops the channel lenght if it's too long
	if (!existingChannel( channelName )) { // if the channel entered doesn't exist no such Channel error displayed
		replyMsg(clientSocket, ERR_NOSUCHCHANNEL( source, nick, channelName));
        return ;
    }
	Channel *chan = &_channels[ channelName ];
    if ( !chan->checkChannelMembers( nick ) ) { // checking if the client that makes the request is a channel member
        replyMsg(clientSocket, ERR_NOTONCHANNEL( source, nick, channelName ));
        return ;
    }
    if ( !chan->checkChannelOps( nick ) && !_clients.at( clientSocket ).getOperatorMode() ) { // if the client doesn't have chanops privileges 
        replyMsg(clientSocket, ERR_CHANOPRIVSNEEDED( source, nick, channelName ));
        return ;
    }
    std::string toInvite = tokens[0].substr( 0, NICKLEN ); // crops nickname if too long
    if ( chan->checkChannelMembers( toInvite ) ) { // checking if the client to invite is not already chanmember
        replyMsg(clientSocket, ERR_USERONCHANNEL(source, nick, toInvite, channelName ));
        return ;
    }
    inviteClientToChannel( clientSocket, nick, toInvite, chan );
}

/**
 * @brief       Invites the designted client to the designated channel
 *              - Checks if the client exists
 *              - Adds the client to _invitedMember vector
 *              - Sends an invite reply to the client
 */

void    Server::inviteClientToChannel( int clientSocket, std::string clientNick, std::string nameInvitee, Channel *chan ) {
    Client *toAdd = getClientByNickname( nameInvitee );
    if (toAdd == NULL) {
        std::cout << MSGERROR << " No such Nickname, cannot invite this client" << std::endl;
        return ;
    }
    chan->addInvitedMember(nameInvitee);
    replyMsg(clientSocket, RPL_INVITING(_clients.at( clientSocket ).getSource(), clientNick, nameInvitee, chan->getChannelName()));
    replyMsg(toAdd->getFd(), INVITE(clientNick, nameInvitee, chan->getChannelName()));
}

/**
 * @brief       Function to check if a client exists by nickname
 *              and return a pointer to the client
 */

Client* Server::getClientByNickname(const std::string& nickname) {
    for ( mapClients::iterator it = _clients.begin(); it != _clients.end(); it++ ) {
        if (it->second.getNickname() == nickname) {
            return (&it->second);
        }
    }
    return NULL;
}
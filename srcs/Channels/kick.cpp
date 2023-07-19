/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmelard <lmelard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/19 18:19:57 by lmelard           #+#    #+#             */
/*   Updated: 2023/07/19 18:38:18 by lmelard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Socket.hpp"
#include "utils.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"

void        Server::handleKick( int clientSocket, std::string param )
{
	std::string source = _clients.at( clientSocket ).getSource();
	std::string nick = _clients.at( clientSocket ).getNickname();
	// if no param are entered then a need more params err msg is displayed
    if (param.empty())
    {
        replyMsg(clientSocket, ERR_NEEDMOREPARAMS( source, nick, "MODE"));
        return ;
    }
	// parsing params
    std::vector<std::string> tokens = splitString(param, ' ');
	bool    isChannel = tokens[0].find('#', 0) != std::string::npos;
	if (isChannel)
	{
		std::string channelName = tokens[0];
		// cropping the first param (channel name) if its length is over the define CHANNELLEN
		if (channelName.size() > CHANNELLEN)
			channelName = channelName.substr(0, CHANNELLEN);
		// if the channel entered doesn't exist no such Channel error displayed
		if (!existingChannel(channelName))
			replyMsg(clientSocket, ERR_NOSUCHCHANNEL( source, nick, channelName));
	}
}
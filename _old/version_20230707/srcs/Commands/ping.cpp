/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgaillag <cgaillag@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/30 17:36:40 by cgaillag          #+#    #+#             */
/*   Updated: 2023/07/06 15:30:05 by cgaillag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"
#include "utils.hpp"

/**
 * @brief	PING command to check if the other side of the connection is still connected
 *			to check for connection latency, at the application layer
 *			(from Client)
 *
 * 			syntax:	PING <token>
 *					<token> cannot contain space
 */

void	Server::handlePing( size_t cid, std::string param )
{
	std::string					reply;
	std::vector<std::string>	tokens = splitString(param, ' ');

	if (param.empty())
	{
		replyMsg(cid, ERR_NEEDMOREPARAMS(_serverName, _clients[cid].getNickname(), "USER"));
		// reply = ERR_NEEDMOREPARAMS(_serverName, _clients[cid].getNickname(), "USER");
		// std::cout << "reply:\t " << reply << std::endl;
		// send(_clients[cid].getFd(), reply.c_str(), reply.length(), 0);
	}
	else if (tokens.size() == 1 && param.compare(_clients[cid].getNickname()) == 0)
	{
		replyMsg(cid, _serverName + " PONG " + param + "\r\n");
		// reply = _serverName + " PONG " + param + "\r\n";
		// std::cout << "reply:\t " << reply << std::endl;
		// send(_clients[cid].getFd(), reply.c_str(), reply.length(),0);
	}
	else
		std::cout << "error:\t <" << param << "> is not client's nickname <" << _clients[cid].getNickname() << ">" << std::endl;
}

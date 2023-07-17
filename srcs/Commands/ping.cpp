/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmelard <lmelard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/30 17:36:40 by cgaillag          #+#    #+#             */
/*   Updated: 2023/07/17 17:17:16 by cvidon           ###   ########.fr       */
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

void	Server::handlePing( int clientSocket, std::string param )
{
	std::vector<std::string>	tokens = splitString(param, ' ');

	if (param.empty())
	{
		replyMsg(clientSocket, ERR_NEEDMOREPARAMS(_serverName, _clients.at( clientSocket ).getNickname(), "USER"));
	}
	else if (tokens.size() == 1 && param.compare(_clients.at( clientSocket ).getNickname()) == 0)
	{
		// replyMsg(clientSocket, _serverName + " PONG " + param + "\r\n");
		replyMsg(clientSocket, "PONG " + param + "\r\n");
	}
	else
		std::cout << "error:\t <" << param << "> is not client's nickname <" << _clients.at( clientSocket ).getNickname() << ">" << std::endl;
	return ;
}

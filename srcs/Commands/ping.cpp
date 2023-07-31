/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgaillag <cgaillag@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/30 17:36:40 by cgaillag          #+#    #+#             */
/*   Updated: 2023/07/28 19:54:11 by cgaillag         ###   ########.fr       */
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
 *					<token> cannot contain space (it may be any non-empty string)
 */

void	Server::handlePing( int clientSocket, std::string param )
{
	std::vector<std::string>	tokens = splitString( param, ' ' );

	if ( param.empty() )
	{
		replyMsg( clientSocket, ERR_NEEDMOREPARAMS( _clients.at( clientSocket ).getSource(), _clients.at( clientSocket ).getNickname(), "PING" ) );
	}
	else if ( tokens[ 0 ].empty() )
	{
		replyMsg( clientSocket, ERR_NOORIGIN( _clients.at( clientSocket ).getSource(), _clients.at( clientSocket ).getNickname() ) );
	}
	else
	{
		replyMsg( clientSocket, "PONG " + _serverName + " " + tokens[ 0 ] + CRLF  );
	}
	return ;
}

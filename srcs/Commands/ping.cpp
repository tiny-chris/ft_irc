/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgaillag <cgaillag@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/30 17:36:40 by cgaillag          #+#    #+#             */
/*   Updated: 2023/07/31 14:01:57 by cgaillag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"
#include "utils.hpp"

/**
 * @brief	PING command 
 * 			syntax:			PING <token>
 *							<token> cannot contain space (it may be any non-empty string)
 * 
 * 
 * To check if the other side of the connection is still connected (check for 
 * connection latency, at the application layer)
 *
 */

void	Server::handlePing( int clientSocket, std::string param )
{
	const std::string&			source = _clients.at( clientSocket ).getSource();
	const std::string&			nickname = _clients.at( clientSocket ).getNickname();
	std::vector<std::string>	tokens = splitString( param, ' ' );

	if ( param.empty() )
	{
		replyMsg( clientSocket, ERR_NEEDMOREPARAMS( source, nickname, "PING" ) );
	}
	else if ( tokens[ 0 ].empty() )
	{
		replyMsg( clientSocket, ERR_NOORIGIN( source, nickname ) );
	}
	else
	{
		replyMsg( clientSocket, "PONG " + _serverName + " " + tokens[ 0 ] + CRLF  );
	}
	return ;
}

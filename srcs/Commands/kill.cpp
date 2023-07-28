/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kill.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgaillag <cgaillag@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/27 15:07:17 by cgaillag          #+#    #+#             */
/*   Updated: 2023/07/27 18:44:14 by cgaillag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Socket.hpp"
#include "utils.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"

/*
//	syntax	 KILL <nickname> <comment>

	Numeric Replies:

		ERR_NEEDMOREPARAMS (461)
		ERR_NOSUCHNICK (401)
		ERR_NOPRIVILEGES (481)
		ERR_CANTKILLSERVER (483)

*/

void	Server::handleKill( int clientSocket, std::string param )
{
	Client				*client = &_clients.at( clientSocket );
	const std::string&	source = client->getSource();
	const std::string&	nickname = client->getNickname();

	std::string			nickToKill = "", comment = "";

	// uncomplete command
	if ( param.empty() || splitStringInTwo( param, ' ', &nickToKill, &comment ) == false
		|| nickToKill.empty() || comment.empty() ) {
		replyMsg( clientSocket, ERR_NEEDMOREPARAMS( source, nickname, "KILL" ) );
		return ;
	}

	// try to kill server
	if ( nickToKill == _serverName ) {
		replyMsg( clientSocket, ERR_CANTKILLSERVER );
		return ;
	}

	// there is no client with the provided nickname
	if ( !existingClient( nickToKill ) ) {
		replyMsg( clientSocket, ERR_NOSUCHNICK( source, nickToKill ) );
		return ;
	}

	// client is not a Server Operator (of this server)
	if ( !client->getOperatorMode() ) {
		replyMsg( clientSocket, ERR_NOPRIVILEGES( source ) );
		return ;
	}

	int		socketToKill = findClientFd( nickToKill );
	Client	*clientToKill = &_clients.at( socketToKill );
	std::string	reason = KILL_REASON( nickname, comment );
	// retirer le client de chaque channel (et mettre un message)
	if ( clientToKill->getClientChannels().size() > 0 ) {
		std::vector< std::string > copyClientChannels = client->getClientChannels();

		for ( size_t i = 0; i < copyClientChannels.size(); i++ ) {
			leaveChannel( socketToKill, copyClientChannels[ i ], reason, "QUIT" );
		}
	}

	replyMsg( socketToKill, ERR_KILL( source, _serverName, reason ) );
	disconnectAClient( socketToKill );
}

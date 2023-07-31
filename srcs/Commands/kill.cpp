/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kill.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgaillag <cgaillag@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/27 15:07:17 by cgaillag          #+#    #+#             */
/*   Updated: 2023/07/31 15:34:42 by cgaillag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Socket.hpp"
#include "utils.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"

/**
 * @brief       Pre-check before killing a client: 
 *  			- uncomplete command
 *				- client tries to kill server
 *				- there is no client with the provided target nickname
 *				- client is not a Server Operator (of this server)channel exists and client is joined to it
 *
 */

bool	Server::checkChanPreKill( int clientSocket, const std::string& param, const std::string& nickToKill, const std::string& comment )
{
	Client *client = &_clients.at( clientSocket );
	const std::string&	source = client->getSource();
	const std::string&	nickname = client->getNickname();

	if ( param.empty() || nickToKill.empty() || comment.empty() ) {
		replyMsg( clientSocket, ERR_NEEDMOREPARAMS( source, nickname, "KILL" ) );
		return false ;
	}
	if ( nickToKill == _serverName ) {
		replyMsg( clientSocket, ERR_CANTKILLSERVER );
		return false ;
	}
	if ( !existingClient( nickToKill ) ) {
		replyMsg( clientSocket, ERR_NOSUCHNICK( source, nickToKill ) );
		return false ;
	}
	if ( !client->getOperatorMode() ) {
		replyMsg( clientSocket, ERR_NOPRIVILEGES( source ) );
		return false ;
	}
	return true ;
}

/**
 * @brief       KILL command
 *				syntax:			 KILL <target> <comment>
 *	
 *	Remove a client from the server by removing it from channels and disconnect
 *
 */

void	Server::handleKill( int clientSocket, std::string param )
{
	Client				*client = &_clients.at( clientSocket );
	std::string			nickToKill = "", comment = "";

	splitStringInTwo( param, ' ', &nickToKill, &comment );
	if ( !checkChanPreKill( clientSocket, param, nickToKill, comment ) ) {
		return ;
	}

	int			socketToKill = findClientFd( nickToKill );
	Client		*clientToKill = &_clients.at( socketToKill );
	std::string	reason = KILL_REASON( client->getNickname(), comment );

	if ( clientToKill->getClientChannels().size() > 0 ) {
		std::vector< std::string > copyClientChannels = client->getClientChannels();

		for ( size_t i = 0; i < copyClientChannels.size(); i++ ) {
			leaveChannel( socketToKill, copyClientChannels[ i ], reason, "QUIT" );
		}
	}
	replyMsg( socketToKill, ERR_KILL( client->getSource(), _serverName, reason ) );
	disconnectAClient( socketToKill );
}

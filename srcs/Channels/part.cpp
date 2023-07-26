/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgaillag <cgaillag@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/30 17:36:40 by cgaillag          #+#    #+#             */
/*   Updated: 2023/07/24 13:23:47 by cgaillag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"
#include "utils.hpp"

void	Server::leaveChannel ( int clientSocket, const std::string& channelName, const std::string& reason )
{
	Client		*client = &_clients.at( clientSocket );
	Channel		*channel = &_channels.at( channelName );

	if ( channel->checkChannelOps( client->getNickname() ) == true && channel->getChannelOps().size() == 1 ) {
		if ( channel->getChannelMembers().size() == 1 ) {
			_channels.erase( channelName );
			client->removeClientChannel( channelName );
			// faire un message pour fermeture de channel ???
			replyMsg( clientSocket, RPL_PART( client->getSource(), client->getNickname(), channelName, reason ) );
		}
		else {
			replyMsg( clientSocket, ERR_CANNOTPART( client->getSource(), channelName, " Error: Cannot delete last operator while other members remaining" ) );
		}
	}
	else {// client is not chanOps or is chanOps but there are other chanOps --> remove client from Channel
		channelMsgToAll( clientSocket, channelName, RPL_PART( client->getSource(), client->getNickname(), channelName, reason ) );
		channel->removeChannelOp( client );
		channel->removeChannelMember( client );
		client->removeClientChannel( channelName );
	}
}

/*
For each channel in the parameter of this command, if the channel exists and the client is not joined to it,
they will receive an ERR_NOTONCHANNEL (442) reply and that channel will be ignored.
*/
void	Server::handlePart( int clientSocket, std::string param )
{
	Client&						client = _clients.at( clientSocket );
	std::string 				channelList = "", reason = "";
	std::vector< std::string >	channelNames;

	if ( param.empty() || splitStringInTwo( param, ' ', &channelList, &reason ) == false ) {
		replyMsg( clientSocket, ERR_NEEDMOREPARAMS( client.getSource(), client.getNickname(), "PART" ) );
		return ;
	}

	reason = ( reason.empty() == true ) ? "" : " :" + reason;
	channelNames = splitString( channelList, ',' );
	for ( size_t i = 0; i < channelNames.size(); ++i ) {
		std::string	channelName = channelNames[ i ];

		// faire les checks
		if ( checkChanPrePart( clientSocket, channelName ) == false ) {
			continue ;
		}

		leaveChannel( clientSocket, channelName, reason );
	}
}

bool	Server::checkChanPrePart( int clientSocket, const std::string& channelName )
{
	Client&	client = _clients.at( clientSocket );

	// channel does not exist --> ERR_NOSUCHCHANNEL (403)
	if ( existingChannel( channelName ) == false ) {
		replyMsg( clientSocket, ERR_NOSUCHCHANNEL( client.getSource(), client.getNickname(), channelName ) );
		return false ;
	}

	Channel&	channel = _channels.at( channelName );
	// client is not joined to the channel --> ERR_NOTONCHANNEL (442)
	if ( channel.getChannelMembers().find( client.getNickname() ) == channel.getChannelMembers().end() ) {
		replyMsg( clientSocket, ERR_NOTONCHANNEL( client.getSource(), client.getNickname(), channelName ) );
		return false ;
	}
	return true ;
}

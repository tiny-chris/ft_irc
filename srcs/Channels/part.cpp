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

void	Server::leaveChannel ( int clientSocket, std::string channelName, std::string reason )
{
	Client&		client = _clients.at( clientSocket );
	Channel&	channel = _channels.at( channelName );

	if ( channel.checkChannelOps( client.getNickname() ) == true && channel.getChannelOps().size() == 1 ) {
		if ( channel.getChannelMembers().size() == 1 ) {
			_channels.erase( channelName );
			// TO DO ERASE CHANNEL IN CLIENTS VECTORS
			// client.removeClientChannel( channelName );

			// faire un message pour fermeture de channel ???
			replyMsg( clientSocket, RPL_PART( client.getSource(), client.getNickname(), channelName, reason ) );
		}
		else {
			replyMsg( clientSocket, "Error: Cannot delete last operator while other members remaining\r\n");
		}
	}
	else {// chanOps mais il y en a d'autres ou pas chanOps
		// remove client from channel members and channel operators
		channelMsgToAll( clientSocket, channelName, RPL_PART( client.getSource(), client.getNickname(), channelName, reason ) );
		channel.removeChannelOp( &client );
		channel.removeChannelMember( &client );
		// TO DO ERASE CHANNEL IN CLIENTS VECTORS
		// client.removeClientChannel( channelName );

		// channel.getChannelOps().erase( client.getNickname() );
		// channel.getChannelMembers().erase( client.getNickname() );
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
	std::vector< std::string >	channels;

	if ( param.empty() || splitStringInTwo( param, ' ', &channelList, &reason ) == false ) {
		replyMsg( clientSocket, ERR_NEEDMOREPARAMS( client.getSource(), client.getNickname(), "PART" ) );
		return ;
	}

	reason = ( reason.empty() == true ) ? "" : " :" + reason;
	channels = splitString( channelList, ',' );
	for ( size_t i = 0; i < channels.size(); ++i )
	{
		std::string	channelName = channels[ i ];

		// faire les checks
		if ( checkPrePartChan( clientSocket, channelName ) == false ) {
			continue ;
		}

		leaveChannel( clientSocket, channelName, reason );

		// Channel&	channel = _channels.at( channelName );
		// if ( channel.checkChannelOps( client.getNickname() ) == true && channel.getChannelOps().size() == 1 ) {
		// 	if ( channel.getChannelMembers().size() == 1 ) {
		// 		_channels.erase( channelName );
		// 		// TO DO ERASE CHANNEL IN CLIENTS VECTORS
		// 		// faire un message pour fermeture de channel ???
		// 		replyMsg( clientSocket, RPL_PART( client.getSource(), client.getNickname(), channelName, reason ) );
		// 	}
		// 	else {
		// 		replyMsg( clientSocket, "Error: Cannot delete last operator while other members remaining\r\n");
		// 		continue ;
		// 	}
		// }
		// else {// chanOps mais il y en a d'autres ou pas chanOps
		// 	// remove client from channel members and channel operators
		// 	channelMsgToAll( clientSocket, channelName, RPL_PART( client.getSource(), client.getNickname(), channelName, reason ) );
		// 	channel.removeChannelOp( &client );
		// 	channel.removeChannelMember( &client );
		// 	client.
		// 	// channel.getChannelOps().erase( client.getNickname() );
		// 	// channel.getChannelMembers().erase( client.getNickname() );
		// 	// TO DO ERASE CHANNEL IN CLIENTS VECTORS
		// }
	}
}

bool	Server::checkPrePartChan( int clientSocket, std::string channelName )
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

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmelard <lmelard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/30 17:36:40 by cgaillag          #+#    #+#             */
/*   Updated: 2023/07/27 17:27:09 by lmelard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"
#include "utils.hpp"

void	Server::leaveChannel( int clientSocket, const std::string& channelName, const std::string& reason, std::string cmd )
{
	Client		*client = &_clients.at( clientSocket );
	Channel		*channel = &_channels.at( channelName );

	if ( channel->checkChannelOps( client->getNickname() ) == true && channel->getChannelOps().size() == 1 ) {
		if ( channel->getChannelMembers().size() == 1 ) {
			_channels.erase( channelName );
			client->removeClientChannel( channelName );
			// faire un message pour fermeture de channel ???
			if (cmd == "PART")
				replyMsg( clientSocket, RPL_PART( client->getSource(), client->getNickname(), channelName, reason ) );
			else if (cmd == "QUIT")
				replyMsg( clientSocket, RPL_QUIT( client->getSource(), client->getNickname(), channelName, reason ));
			return ;
		}
		else {//( channel->getChannelMembers().size() > 1 )// but only current client as chanOp
			// replyMsg( clientSocket, ERR_CANNOTPART( client->getSource(), channelName, " Error: Cannot delete last operator while other members remaining" ) );

			// make another client as ChanOp (the first one on the list if not current client)
			// channel->getChannelMembers().begin()
			// 1. récupérer le 1er client de la liste channelMembers

			std::cout << ZZ_MSGTEST << "1. check avant de trouver le client à mettre en chanOp" << std::endl;
			Channel::mapClientsPtr::iterator	it =  channel->getChannelMembers().begin();
			Client								*toBeChanOp = NULL;

			std::cout << ZZ_MSGTEST << "2. check après avoir défini l'itérateur sur les membres du channel" << std::endl;
			for ( ; it != channel->getChannelMembers().end(); it++) {
				if ( it->second != client ) {
					toBeChanOp = it->second;
					break ;
				}
			}

			std::cout << ZZ_MSGTEST << "3. check après avoir récupéré le Client* à mettre en chanOp" << std::endl;

			std::string	toBeChanOpName = toBeChanOp->getNickname();
			// int									toBeChanOpSocket = findClientFd( toBeChanOpName );

			std::cout << ZZ_MSGTEST << "4. check après avoir récup le nom du client futur chanOp : " << toBeChanOpName << std::endl;

			// 2. lui assigner le mode +o de chanOp
			std::string	param = channelName + " +o " + toBeChanOpName;

			std::cout << ZZ_MSGTEST << "5. check après avoir défini le param = channelName + '+o' + nom client futur chanOp : " << param << std::endl;

			handleMode( clientSocket, param );
			std::cout << ZZ_MSGTEST << "6. check après avoir forcé le passage en +o du futur chanOp" << std::endl;
		}
	}
	// else {// client is not chanOps or is chanOps but there are other chanOps --> remove client from Channel
	if (cmd == "PART")
		channelMsgToAll( clientSocket, channelName, RPL_PART( client->getSource(), client->getNickname(), channelName, reason ) );
	else if (cmd == "QUIT")
		channelMsgToAll( clientSocket, channelName, RPL_QUIT( client->getSource(), client->getNickname(), channelName, reason ) );
	channel->removeChannelOp( client );
	channel->removeChannelMember( client );
	client->removeClientChannel( channelName );
	// }
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

	if ( !reason.empty() ) {
		if ( reason.at(0) == ':' )
			reason = " " + reason;
		else
			reason = " :" + reason;
	}
	channelNames = splitString( channelList, ',' );
	for ( size_t i = 0; i < channelNames.size(); ++i ) {
		std::string	channelName = channelNames[ i ];

		// faire les checks
		if ( checkChanPrePart( clientSocket, channelName ) == false ) {
			continue ;
		}

		leaveChannel( clientSocket, channelName, reason, "PART" );
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

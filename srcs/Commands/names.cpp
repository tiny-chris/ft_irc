/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   names.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmelard <lmelard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/30 17:36:40 by cgaillag          #+#    #+#             */
/*   Updated: 2023/07/21 16:40:13 by lmelard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"
#include "utils.hpp"

/**
 * @brief	Display names on one channel
 *
 *			By using RPL_NAMEREPLY + RPL_ENDOFNAMES
 *			/!\ if a user is invisible --> do not show
 *				unless requesting client is also joined to that channel --> show
 *			remark: case 'secret channel' not taken into account
 */

void	Server::displayNames( int clientSocket, Channel& channel )
{
	Channel::mapClientsPtr&					chanMembers = channel.getChannelMembers();
	Channel::mapClientsPtr::const_iterator	it;
	// bool									requestorIsOnChannel = false;
	std::string								listMembers;


/*  ****************************************  */
/*  POUR CHECKER LES CLIENTS DANS CHANNEL !!  */
/*  ****************************************  */

		std::cout << ZZ_MSGTEST << "\n";
		int n = 0;
		for (Channel::mapClientsPtr::const_iterator	testIt = chanMembers.begin(); testIt != chanMembers.end(); testIt++)
		{
			std::cout << "\t\t channel member [" << n << "] named '" << testIt->second->getNickname();
			std::cout << "' with status 'is invisible' = " << testIt->second->getUserModes() << "\n";
			n++;
		}
/*  ****************************************  */
/*  ****************************************  */

	std::cout << ZZ_MSGTEST << "Current channel = '" << channel.getChannelName() << "'" << std::endl;
	bool requestorIsOnChannel = ( chanMembers.find( _clients.at( clientSocket ).getNickname() ) != chanMembers.end() ) ? true : false;
	// if ( chanMembers.find( _clients.at( clientSocket ).getNickname() ) != chanMembers.end() ) {
	// 	requestorIsOnChannel = true ;
	// }
	std::cout << ZZ_MSGTEST << "requestor client = '" << _clients.at( clientSocket ).getNickname() << "' is on channel : " << requestorIsOnChannel << std::endl;
	for ( it = chanMembers.begin(); it != chanMembers.end(); it++ )
	{
		if ( it->second->getUserModes() == true && requestorIsOnChannel == false )
			continue ;
		if ( it != chanMembers.begin() ) {
			listMembers += " ";
		}
//		si le members est un operateur, alors mettre @ sinon mettre +
		if ( channel.checkChannelOps( it->second->getNickname() ) == true ) {
			listMembers += "@";
		}
		// else {
		// 	listMembers += "+";
		// }
		listMembers += it->first;
	}

	replyMsg( clientSocket, RPL_NAMREPLY( _clients.at( clientSocket ).getSource(), _clients.at( clientSocket ).getNickname(), channel.getChannelName(), listMembers ) );
	replyMsg( clientSocket, RPL_ENDOFNAMES( _clients.at( clientSocket ).getSource(), _clients.at( clientSocket ).getNickname(), channel.getChannelName() ) );
}

/**
 * @brief	NAMES command to view the nicknames joined to a channel
 * 			and their channel membership prefixes.
 * 			The param of this command is a list of channel names, delimited by
 * 			a comma (",", 0x2C) character
 *
 * 			syntax:	NAMES <channel>{,<channel>}
 *
 */

void	Server::handleNames( int clientSocket, std::string param )
{
	mapChannels::iterator		chanIt;
	// std::string					chanMembers;
	std::vector<std::string>	tokens = splitString( param, ',' );

	// if there is no param: display all channels' name (one channel by one)
	if ( param.empty() || !tokens.size() )
	{
		for ( chanIt = _channels.begin(); chanIt != _channels.end(); ++chanIt )
		{
			Channel&	channel = chanIt->second;
			displayNames( clientSocket, channel );
		}
	}
	// else: display client names of listed channels that exist (evaluate channels one by one)
	else
	{
		// utiliser TARGMAX pour limiter les multi-ddes ???
		// créer une macro 'TARGMAX' dans defines !!
		// TARGMAX=ACCEPT:,KICK:1,LIST:1,NAMES:1,NOTICE:4,PRIVMSG:4,WHOIS:1
		// créer une std::map<std::string,int> targmaxLimits pour stocker les targmax
		size_t targmax = ( 3 < tokens.size() ) ? 3 : tokens.size() ;
		for ( size_t i = 0; i < targmax; i++ )
		{
			chanIt = _channels.find(tokens[ i ]);
			if ( chanIt != _channels.end() )
			{
				// std::cout << ZZ_MSGTEST << "'" << tokens[i] << "' est bien dans la liste des channels" << std::endl;
				Channel&	channel = chanIt->second;
				displayNames( clientSocket, channel );
			}
			else
			{
				// std::cout << ZZ_MSGTEST << "'" << tokens[i] << "' n'est pas dans la liste des channels" << std::endl;
				replyMsg( clientSocket, RPL_ENDOFNAMES( _clients.at( clientSocket ).getSource(), _clients.at( clientSocket ).getNickname(), tokens[ i ] ) );
			}
		}
	}
	return ;
}

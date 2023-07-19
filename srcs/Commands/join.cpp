/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgaillag <cgaillag@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/30 17:36:40 by cgaillag          #+#    #+#             */
/*   Updated: 2023/07/19 12:14:00 by cgaillag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"
#include "utils.hpp"

/**
 * @brief       JOIN command
 *
 *				spécification IRC (RFC 2812)
 *				http://abcdrfc.free.fr/rfc-vf/rfc2812.html
 *				https://modern.ircdocs.horse/#connection-setup

				syntax:
					JOIN <channel> [key] [invitationkey] [mode]

				Numeric Replies:
					ERR_NEEDMOREPARAMS (461)	x
					ERR_NOSUCHCHANNEL (403)		x
					ERR_TOOMANYCHANNELS (405)	x
					ERR_BADCHANNELKEY (475)		x
					ERR_BANNEDFROMCHAN (474)	x
					ERR_CHANNELISFULL (471)		x
					ERR_INVITEONLYCHAN (473)	x
					ERR_BADCHANMASK (476)		x
					RPL_TOPIC (332)				x
					RPL_TOPICWHOTIME (333)
					RPL_NAMREPLY (353)
					RPL_ENDOFNAMES (366)
 *
 */

void	Server::handleJoin( int clientSocket, std::string param, std::string cmd )
{
	std::vector<std::string>	tokens = splitString(param, ' ');
	Client& 					client = _clients.at( clientSocket );

	std::cout << ZZ_MSGTEST << "JOIN - nombre de tokens: " << tokens.size() << std::endl;

	/* ******************************* ***
	** CHECK 1 - NOT ENOUGH PARAMETERS ***
	** ******************************* ***
	**	check if param is empty --> ERR_NEEDMOREPARAMS
	*/
	if (param.empty() || tokens.size() == 0)
	{
		// replyMsg(cid, ERR_NEEDMOREPARAMS(_serverName, _clients[cid].getNickname(), cmd));
		replyMsg( clientSocket, ERR_NEEDMOREPARAMS(client.getSource(), client.getNickname(), cmd));
		return ;
	}

	/*	count the number parameters
	**	- if tokens size > 15 - 1 (-1 for the command) --> too many parameters
	*/
	// --> revoir car on ne devrait pas avoir autant de tokens !!

	if (tokens.size() > MAXPARAM - 1)
	{
		// check if there is an existing numeric reply
		std::cout << MSGERROR << "too many parameters for this command JOIN\n" << std::endl;
		return ;
	}

	/* *************************************************** ***
	** CHECK 2 - EACH CHANNEL STARTS WITH '#' AND IS VALID ***
	** *************************************************** ***
	**	Let's consider tokens[0] to get the channels
	**	- if there is 0 channel --> reject the request as not aligned with our convention
	**		(there must be at least a channel starting with #)
	**	- if a channel is empty or if it does not start with # --> ERR_BADCHANMASK (476)
	**						+ leave
	**	- or if its content is invalid
	**
	**	"channel" (nom du canal) : Le nom du canal doit commencer par un préfixe spécifique
	**	pour indiquer le type de canal, suivi d'un nom unique. Les préfixes couramment
	**	utilisés sont "#" pour les canaux publics et "&" pour les canaux privés.
	**	Le nom du canal peut contenir des lettres, des chiffres et certains caractères
	**	spéciaux tels que "-", "_", et ".".
	*/
	std::vector<std::string>	channels = splitString(tokens[0], ',');
	std::cout << ZZ_MSGTEST << "nombre de channels (dans tokens[0]): " << channels.size() << std::endl;

	if (validChannelNames( clientSocket, channels ) == false)
		return ;


	std::cout << ZZ_MSGTEST << "Print les valeurs de channelNames :";
	for (std::vector<std::string>::iterator it = channels.begin(); it != channels.end(); it++)
	{
		std::cout << " " << *it;
	}
	std::cout << "\n";

	/* *************************************************** ***
	** CHECK 3 - EACH CHANNEL ALREADY EXISTS               ***
	** *************************************************** ***
	*/
	for ( size_t i = 0; i < channels.size(); ++i )
	{
		if ( existingChannel( channels[ i ] ) == true )
		{
			if ( joinExistingChannel( requestor, channels[i] ) == false )
				return ;
		}
		else
		{
			if ( joinNonExistingChannel( requestor, channels[i] ) == false )
				return ;
			std::map<std::string, Channel>::iterator it;
			for (it = _channels.begin(); it != _channels.end();++it)
			{
				std::cout << "nom du channel '"<< it->first << "'";
				std::cout << " et nom dans le channel '" << it->second.getChannelName() << "'" << std::endl;
			}
		}
		// if client has been added to the channel
		// display RPL_TOPIC and RPL_NAMES
		// RPL_TOPIC
		std::cout << ZZ_MSGTEST << "TO DO: DEAL RPL_TOPIC...\n";
		// NAMES
		std::cout << ZZ_MSGTEST << "TO DO: DEAL RPL_NAMES...\n" << std::endl;
	}
}


/* ************************************ ***
** CHANNEL DOES NOT EXIST YET           ***
** ************************************ ***
**	check if the client is already on too many channels
**	- if so --> ERR_TOOMANYCHANNELS
**	- if not --> follow process:
**		1. create a new Channel with current Client as operator
**			add potential other parameters (TO BE DEFINED)
**		2. add this new channel to the map in the Server '_channels'
**		3. add this new channel to the list of the Client 'vector'
**
*/
bool	Server::joinNonExistingChannel( Client& requestor, std::string chanName )
{
	// penser à recoder avec une fonction sur _channels
	if ( requestor.getClientChannels().size() >= CHANLIMIT )
	{
		replyMsgFd( requestor.getFd(), ERR_TOOMANYCHANNELS( requestor.getSource(), requestor.getNickname(), chanName ) );
		return false ;
	}

	// je crée un nouveau channel
	Channel newChan( chanName );
	std::cout << MSGINFO << "recup nom channel dans joinNonexistingClient '" << newChan.getChannelName() << "'" << std::endl;
	// je lui ajoute le client --> prévoir le chanOp
	newChan.addConnectedClient( requestor );
	// voir pour les paramètres --> opérateur, mode? dans le constructeur ?
	// j'ajoute le channel à la liste des channels
	_channels.insert( std::make_pair( chanName, newChan ) );
	// j'ajoute le nom du channel dans le client (vraiment nécessaire ??)
	requestor.addChannel( chanName );
	std::cout << MSGINFO << "a new channel <" << _channels[ chanName ].getChannelName() << "> is created\n";// << std::endl;
	std::cout << ZZ_MSGTEST << requestor.getNickname() << " has created the channel <" << chanName << "> and is chan op (TO BE CONFIGURED)\n" << std::endl;
	return true ;
}

/* ************************************ ***
** CHANNEL DOES EXIST                   ***
** ************************************ ***
**	check if the client has already joined this channel
**	- if so --> return ; NO ACTION ??? (to check)
**	- if not --> continue
**
**	check if the client is already on too many channels
**	- if so --> ERR_TOOMANYCHANNELS
**	- if not --> continue
**
**	check -----> ERR_NOSUCHCHANNEL ? ? ? ? ? ? ? ? ?
**
**	check if the channel is available upon invitation
**	- if so --> check if client has received an invitation for this channel
**		- if client was not invited --> ERR_INVITEONLYCHAN
**		- if client was invited : continue
**	- if not : continue
**
**	check if client is banned from this channel
**	- if so --> ERR_BANNEDFROMCHAN
**	- if not : continue
**
**	check if the channel required password
**	- if so --> check if client has provided the password
**		- if no: ERR_BADCHANNELKEY
**		- if yes: continue
**	- it not : continue
**
**	check if channel is full
**	- if so : ERR_CHANNELISFULL
**	- if not : follow process:
**		1. add Client to the existing Channel
**			+ provide spec parameters (update ??)
**		2. add Channel to the list of the Client 'vector'
**
*/
bool	Server::joinExistingChannel( Client& requestor, std::string chanName )
{
	// // checkIfClientHasAlreadyThisChannel( )
	// std::vector<Channel>::iterator it = _clients[i]._clientChannels.begin();
	// std::vector<Channel>::iterator ite = _clients[i]._clientChannels.end();
	// for ( ; it != ite; it++ )
	// {
	// 	if ( chanName.compare(it->getChannelName()) == 0 )
	// 	{
	// 		//already exists
	// 		std::cout << MSGINFO << chanName << _clients[cid].getNickname() << " is already on this channel\n" << std::endl;
	// 		return ;//?? true or false... should be false
	// 	}
	// }

	// //
	// if ( _clients[cid]._clientChannels.size() >= CHANLIMIT )
	// {
	// 	replyMsg( cid, ERR_TOOMANYCHANNELS( _serverName, _clients[cid].getNickname(), chanName ) );
	// 	return false ;
	// }

	// Channel newChan( chanName );
	// // voir pour les paramètres --> opérateur, mode? dans le constructeur ?
	// _channels.insert( std::make_pair( chanName, newChan ) );
	// _clients[cid]._clientChannels.push_back( newChan );
	// std::cout << "A new channel " << chanName << " is created\n";// << std::endl;
	// std::cout << _clients[cid].getNickname() << " has created the channel " << chanName << " and is chan op\n" << std::endl;
	( void ) requestor;
	( void ) chanName;
	std::cout << "test:\t case = joinExistingChannel\n";
	return true ;
}

// 	/* ************************************************ ***
// 	** CHECK 1 - CLIENT IS ALREADY ON TOO MANY CHANNELS ***
// 	** ************************************************ ***
// 	*/
// 	if (_clients[cid]._clientChannels.size() >= CHANLIMIT)
// 	{
// 		// RECUP LE CHAN NAME
// 		replyMsg(cid, ERR_TOOMANYCHANNELS(_serverName, _clients[cid].getNickname(), chan));
// 	}
// # define ERR_TOOMANYCHANNELS 405
// //"<nom de canal> :You have joined too many channels"
// //Envoyé à un utilisateur quand il a atteint le nombre maximal de canaux qu'il est autorisé à accéder simultanément, s'il essaie d'en rejoindre un autre.

bool	Server::validChannelNames( int clientSocket, std::vector<std::string>& channelNames )
// bool	Server::validChannelNames( const Client& requestor, std::vector<std::string>& channels )
{
	Client& client = _clients.at( clientSocket );
	// specific case when there is no channel name (only spaces)
	if ( !channelNames.size() )
	{
		replyMsg( clientSocket, ERR_BADCHANMASK( client.getSource(), client.getNickname(), "<empty>" ) );
		return false;
	}

	// check each channel if there is at least one channel
	for ( size_t i = 0; i < channelNames.size(); ++i )
	{
		std::string chanName = "<empty>";
		if ( !channelNames[ i ].empty() && channelNames[ i ].find( CHANTYPES ) != 0 )
		{
			chanName = channelNames[ i ];
		}
		else if ( channelNames[ i ].length() > 1 )
		{
			chanName = channelNames[ i ].substr(1);
		}

		if ( chanName.compare( "<empty>" ) == 0 || channelNames[ i ].find( CHANTYPES ) != 0
			|| ( channelNames[ i ].find( CHANTYPES ) == 0 && channelNames[ i ].length() == 1 )
			|| ( channelNames[ i ].find( CHANTYPES ) == 0 && !isValidToken( chanName ) ) )
		{
			replyMsg( clientSocket, ERR_BADCHANMASK( client.getSource(), client.getNickname(), chanName ) );
			return false ;
		}

		channelNames[ i ] = chanName;
		std::cout << ZZ_MSGTEST << "val de chanName = " << chanName << std::endl;
	}
	return true ;
}

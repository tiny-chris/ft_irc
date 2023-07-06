/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pass.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmelard <lmelard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/03 15:16:32 by lmelard           #+#    #+#             */
/*   Updated: 2023/07/03 15:19:31 by lmelard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Socket.hpp"
#include "utils.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"

/*  ******************************************************************** */
/*  ***** following functions should be put in the Commands folder ***** */
/*  ******************************************************************** */

/* Peut etre mettre des comments dans le serveur (pour indiquer par exemple que le mot de passe est correct....)*/

void	Server::handlePass( size_t cid, std::string param )
{
	std::string	reply;
	// checking if the Client is already registered
	// meaning checking if PASS, NICK, USER are already set
	// if not ERR_ALREADYREGISTERED numeric reply is sent
	if (_clients[cid].getIfRegistered() == true)
	{
		// ERR_ALREADYREGISTERED numeric reply is sent
		reply = ERR_ALREADYREGISTRED(_clients[cid].getSource(), _clients[cid].getNickname());
		std::cout << "print reply: " << reply << std::endl; // to del 
		send(_clients[cid].getCfd(), reply.c_str(), reply.length(), 0);
	}
	// else if there is no param to the PASS command
	// ERR_NEEDMOREPARAMS numeric reply is sent
	else if (param.compare("") == 0)
	{
		std::string command = "pass";
		reply = ERR_NEEDMOREPARAMS (_clients[cid].getSource(), _clients[cid].getNickname(), command);
		std::cout << "print reply: " << reply << std::endl; // to del
		send(_clients[cid].getCfd(), reply.c_str(), reply.length(), 0);
	}
	// else if Pass command's param is different from the password set for the Server
	// then ERR_PASSDMISMATCH error is sent and Client is killed et disconnected
	else if (param.compare(_password) != 0 || param.size() != _password.size())
	{
		reply = ERR_PASSWDMISMATCH(_clients[cid].getSource(), _clients[cid].getNickname());
		std::cout << "print reply: " << reply << std::endl; // to del
		send(_clients[cid].getCfd(), reply.c_str(), reply.length(), 0);
		reply = KILL_MSG(_clients[cid].getSource(), _clients[cid].getNickname());
    send(_clients[cid].getCfd(), reply.c_str(), reply.length(), 0);
   	this->delConnection( cid );
	}
	// else if it's the right password, the client is not yet registered then setPassStatus to true
	else
		_clients[cid].setPassStatus(true);
	return ;
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmelard <lmelard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 10:14:07 by codespace         #+#    #+#             */
/*   Updated: 2023/07/21 18:42:01 by lmelard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Socket.hpp"
#include "utils.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"

// The TOPIC command is used to change or view the topic of the given channel. 
void		Server::handleTopic( int clientSocket, std::string param )
{
    std::string source = _clients.at( clientSocket ).getSource();
	std::string nick = _clients.at( clientSocket ).getNickname();
    // if no param are entered then a need more params err msg is displayed
    if (param.empty())
    {
        replyMsg(clientSocket, ERR_NEEDMOREPARAMS(source, nick, "TOPIC"));
        return ;
    }
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmelard <lmelard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/26 14:14:38 by lmelard           #+#    #+#             */
/*   Updated: 2023/07/26 14:16:20 by lmelard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Socket.hpp"
#include "utils.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"

void	Server::handleQuit( int clientSocket, std::string param )
{
	std::cout << clientSocket << std::endl;
	std::cout << param << std::endl;
}

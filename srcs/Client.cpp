/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgaillag <cgaillag@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/22 15:13:43 by lmelard           #+#    #+#             */
/*   Updated: 2023/06/23 15:33:59 by cgaillag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client( std::string name ) : _name( name ), _cid( -1 ), _passStatus( false )
{}

Client::~Client( void ) {}

std::string& Client::getName( void ) {
	return _name;
}

int& Client::getCid( void ) {
	return _cid;
}

bool& Client::getPassStatus( void ) {
	return _passStatus;
}

void Client::setName( std::string const& name ) {
	_name = name;
}

void Client::setCid( int cid ) {
	_cid = cid;
}

void Client::setPassStatus( bool const& passwordStatus ) {
	_passStatus = passwordStatus;
}

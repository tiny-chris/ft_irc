#include "Server.hpp"

Server::Server(std::size_t port, const char *password) : _port(port), _password(password)
{
	std::cout << "port init = " << this->_port << std::endl;
	return ;
}


Server::~Server(void)
{
	return ;
}

void	Server::run(void)
{
	if (this->_port > 10000)
		throw std::runtime_error("port > 65535");
	std::cout << "server run with:" << std::endl;
	std::cout << "- port = " << this->_port << std::endl;
	std::cout << "- password = " << this->_password << std::endl;
	return ;
}
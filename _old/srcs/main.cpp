#include "Server.hpp"

#include <cstdlib>		// std::atoi() 
#include <iostream>		// std::cout
#include <stdexcept>	// std::invalid_argument, std::runtime_error...


void	checkArgs(int argc, char **argv)
{
	/* check number of arguments */
	if (argc != 3)
		throw std::invalid_argument("incorrect number of arguments");

	/* check <port> is a port number */
	std::string	port(argv[1]);
	if (port.find_first_not_of("0123456789") != std::string::npos \
		|| port.empty())
		throw std::invalid_argument("wrong port number");

	long val = std::atol(argv[1]);
	if (val < 0 || val > 65535)
		throw std::invalid_argument("wrong port number");

	/* check <password> is a port number */
	std::string	password = argv[2];

	if (password.empty())
		throw std::invalid_argument("a password is required");
	
	//	//////////
	//	Y A T IL DES CARACTERES NON AUTORISES DANS LE PWD ??? PAS QUE DES ESPACES PAR EX
	//	//////////

	return ;
}

int	main(int argc, char **argv) 
{
	try
	{
		/* ***** CHECK NB of ARG and VALUES OF ARGV[1] & ARGV[2] ***** */
		checkArgs(argc, argv);
		/* if incorrect --> throw an exception invalid_argument */
		
		/* ***** INIT REQUIRED PARAMETERS FOR THE SERVER CLASS ***** */
		/* Classe Server avec 2 params: port et password, garder password en char* */
		std::size_t	port = std::atoi(argv[1]);
		const char	*password = argv[2];

		Server server(port, password);
		server.run();	
	}
	catch (const std::invalid_argument& ia)
	{
        std::cerr << "Invalid argument: " << ia.what() << std::endl;
		std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
		return EXIT_FAILURE;
	}
	catch(std::runtime_error &e)
	{
		std::cout << "Exception: " << e.what() << std::endl;
	}
	return EXIT_SUCCESS;
}

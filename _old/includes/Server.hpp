#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>	// std::cout

class Server
{
	private:

		std::size_t const	_port;
		std::string const	_password;	


	public:

		Server(std::size_t port, const char *password);
		~Server(void);

		std::size_t	getPort(void) const;
		std::string	getPassword(void) const;

		void	run(void);

};

#endif /* __SERVER_HPP__ */
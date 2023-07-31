/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By:                                            +#+  +:+       +#+        */
/*       lmelard <lmelard@student.42.fr>          +#+#+#+#+#+   +#+           */
/*       cgaillag <cgaillag@student.42.fr>             #+#    #+#             */
/*       cvidon <cvidon@student.42.fr>                ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Server.hpp"
#include "defines.hpp"
#include "utils.hpp"

#include <csignal>
#include <cstdlib>    // std::atoi()
#include <iostream>   // std::cout
#include <stdexcept>  // std::invalid_argument, std::runtime_error...

/**
 * @brief       SINGINT ^-C Handling
 */

void signalHandler( int signal ) {
  if( signal == SIGINT ) {
    throw std::runtime_error( "SIGINT" );
  }
}

void checkArgs( int argc, char** argv ) {
  /* check number of arguments */
  if( argc != 3 )
    throw std::invalid_argument( "incorrect number of arguments" );

  /* check <port> is a port number */
  std::string port( argv[1] );
  if( port.find_first_not_of( "0123456789" ) != std::string::npos
      || port.empty() )
    throw std::invalid_argument( "wrong port number" );

  long val = std::atol( argv[1] );
  if( val < 0 || val > 65535 )
    throw std::invalid_argument( "wrong port number" );

  /* check <password> is existing & valid */
  std::string password = argv[2];

  if( password.empty() )
    throw std::invalid_argument( "a password is required" );
  else if( password.length() < PASSMINLEN || password.length() > PASSMAXLEN )
    throw std::invalid_argument( "password length is out of bounds" );
  else if( !isValidUser( password ) )
    throw std::invalid_argument( "a password contains invalid characters" );

  return;
}

int main( int argc, char** argv ) {
  signal( SIGINT, signalHandler );
  try {
    checkArgs( argc, argv );
    std::size_t port = std::atoi( argv[1] );
    const char* password = argv[2];

    Server server( port, password, "tiny_server" );
    server.start();
  } catch( const std::invalid_argument& ia ) {
    std::cerr << "Invalid argument: " << ia.what() << std::endl;
    std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
    return EXIT_FAILURE;
  } catch( std::runtime_error& e ) {
    std::cout << "Exception: " << e.what() << std::endl;
  }
  return EXIT_SUCCESS;
}

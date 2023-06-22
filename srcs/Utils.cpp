/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmelard <lmelard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/22 18:23:45 by lmelard           #+#    #+#             */
/*   Updated: 2023/06/22 18:27:29 by lmelard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Server.hpp"
#include "Socket.hpp"

/**
 * @brief       Int to String
 */

std::string intToString( int number ) {
  std::ostringstream oss;
  oss << number;
  return oss.str();
}

/**
 * @brief       Forbidden gai_strerror implementation
 */

std::string gaiStrerror( int errorCode ) {
  switch( errorCode ) {
    case 0:
      return "Success";
    case EAI_AGAIN:
      return "Temporary failure in name resolution";
    case EAI_BADFLAGS:
      return "Invalid value for ai_flags";
    case EAI_FAIL:
      return "Non-recoverable failure in name resolution";
    case EAI_FAMILY:
      return "ai_family not supported";
    case EAI_MEMORY:
      return "Out of memory";
    case EAI_NONAME:
      return "Name or service not known";
    case EAI_SERVICE:
      return "Invalid value for service";
    case EAI_SOCKTYPE:
      return "ai_socktype not supported";
    case EAI_SYSTEM:
      return strerror( errno );
    default:
      return "Unknown error";
  }
}
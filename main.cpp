#include "colors.h"
#include <stdlib.h>
#include <iostream> 
#include <signal.h>
#include "Server.hpp"

bool server_run = true;

void	sigint(int sign)
{
	if (sign == SIGINT)
	{
		server_run = false;
		std::cout << COLOR_CYAN << "SIGINT: signal handler called" << std::endl;
	}
}

int main(int argc, char **argv)
{
    if (argc == 3)
	{
		int port = std::atoi(argv[1]);
		if (port < 0 || port > 65535)
		{
			std::cerr << COLOR_RED << "Invalid port number, please select number only between 0 and 65535" << std::endl;
			return (EXIT_FAILURE);
		}
		signal(SIGINT, &sigint);
		std::cout << COLOR_GREEN << "Setting up server..." << std::endl;
		// create server
		Server Server(std::atoi(argv[1]), argv[2]);
		//initialise server TO DO 

		std::cout << COLOR_GREEN << "IRC Server is up and running...\n" << COLOR_DEFAULT;
		
		return (EXIT_SUCCESS);
		}
	std::cerr << COLOR_RED << "Invalid number of arguments: only port number and password pls" << std::endl;
	return (EXIT_FAILURE);
}
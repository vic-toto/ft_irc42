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
		try 
		{
			if (port >= 0 && port <= 65535)
			{
				std::cout << COLOR_GREEN << "Port good, setting up server..." << std::endl;
				std::cout << "- - - - - - - - - - - - - - - - - - - - " << std::endl;
				// create server
				Server server(port, argv[2]);
				//Run server
				try
				{
					if (!(server.init()))
						server.run();
				}
				catch(const std::exception& e)
				{
					std::cerr << e.what() << '\n';
				}
				signal(SIGINT, &sigint);
			}
		
		}
		catch(const std::exception& e)
		{
			std::cerr << COLOR_RED << "Error: " << e.what() << std::endl;
			return (EXIT_FAILURE);
		}
		
		return (EXIT_SUCCESS);
	}
	std::cerr << COLOR_RED << "Invalid number of arguments: only port number and password pls" << std::endl;
	return (EXIT_FAILURE);
}
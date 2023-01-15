#include "Server.hpp"

// STEP 1

Server::Server(int const & port, std::string const & password)
{
	this->__port = port;
	this->__pwd = password;
	std::time_t result = std::time(nullptr);
    this->created_at = std::asctime(std::localtime(&result));
    std::cout << COLOR_YELLOW << "Initialising server - - - created @ " << this->created_at << std::endl << COLOR_DEFAULT;
}

Server & Server::operator=(Server const & rhs)
{
	this->~Server();
	new (this) Server(rhs);
	return (*this);
}

Server::~Server(void)
{
	std::cout << COLOR_GREEN << "IRC Server stopped and shut down...\n" << COLOR_DEFAULT;
}

int	Server::init()
{
	int	server_socket;
	int	yes = 1;

	sockaddr_in sock; 
	sock.sin_family = AF_INET; //AF_INET is constant used to specify the IPv4 family addr when creating socket. 
	sock.sin_port = htons(atoi(__port.c_str()));
	sock.sin_addr.s_addr = INADDR_ANY; // here, we set up the socket address structure used to identify server and port

	if ((server_socket = socket(AF_INET, SOCK_STREAM, 0) < 0)){ //Create fd for server socket 
		throw std::runtime_error(strerror(errno));
		return 1;}
	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)); //with REUSEADDR, we make sure addr stays the same even if the server is restarted
	if (server_socket == -1){
		std::cerr << COLOR_RED << "Cannot create socket, quitting now ---" << std::endl;
		return 1;
	}
	if ((bind(server_socket, (sockaddr*)&sock, sizeof(sock))) < 0) //assigns to serversocket, port specified in socketstructure, returns error if socket in use
	{
		//close (server_socket);
		//throw std::runtime_error(strerror(errno));
		//return 1;
	}
	listen(server_socket, SOMAXCONN); //sets socket in listening state
	fcntl(server_socket, F_SETFL, O_NONBLOCK); //sets socket to non-blocking
    listen_socket = server_socket;
	std::cout << COLOR_GREEN << "IRC Server successfully initialised :) 🎉\n\n" << COLOR_DEFAULT;
	return 0;
}

void	Server::run()
{
	fd_set	ready_sockets;
	FD_ZERO(&current_sockets);
	FD_SET(this->listen_socket, &current_sockets);
	while (true)
	{
		ready_sockets = current_sockets;
		try
		{
			if(select(FD_SETSIZE, &ready_sockets, NULL, NULL, NULL) >= 0){
				for (int i = 0; i < FD_SETSIZE; i++){
					if (FD_ISSET(i, &ready_sockets)){
						if (i == this->listen_socket){
							int client_socket = 1; //here we will add_user from client;
							client_socket = 0;
						}
						else{
							char buff[4096];
							memset(buff, 0, 4096);
							// int num = recv(i, buff, 4096, 0); //function receives data from connected socket or bound connectionless socket
							// controllo su num da fare, se minore di 0 da errore
							// altrimenti prendi user
						}

					}
				}
			}
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
			return ;
		}
		
	}

	std::cout << COLOR_GREEN << "IRC Server is up and running...\n" << COLOR_DEFAULT;
}
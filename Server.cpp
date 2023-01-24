#include "Server.hpp"
#include "User.hpp"


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
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket < 0){ //Create fd for server socket 
		throw std::runtime_error(strerror(errno));
		return 1;}
	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)); //with REUSEADDR, we make sure addr stays the same even if the server is restarted
	if (server_socket == -1){
		std::cerr << COLOR_RED << "Cannot create socket, quitting now ---" << std::endl;
		return 1;
	}
	int ret = bind(server_socket, (sockaddr*)&sock, sizeof(sock));
	if (ret < 0) //assigns to serversocket, port specified in socketstructure, returns error if socket in use
	{
		close (server_socket);
		throw std::runtime_error(strerror(errno));
		return 1;
	}
	printf("server socket %d\n", server_socket);
	int ret2 = listen(server_socket, SOMAXCONN); //sets socket in listening state
	if (ret2 < 0)
	{
	    std::cerr << "Error listening: " << strerror(errno) << std::endl;
	    return 1;
	}
	// listen(server_socket, SOMAXCONN); 
	fcntl(server_socket, F_SETFL, O_NONBLOCK); //sets socket to non-blocking
    this->listen_socket = server_socket;
	std::cout << COLOR_GREEN << "IRC Server successfully initialised :) 🎉\n\n" << COLOR_DEFAULT;
	run();
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
			struct timeval timeout;
			timeout.tv_sec = 5;
			timeout.tv_usec = 0;
			int sel = select(FD_SETSIZE, &ready_sockets, NULL, NULL, &timeout);
						printf("%d this->listen_socket\n\n", this->listen_socket);		
			if(sel >= 0){
				for (int i = 0; i < FD_SETSIZE; i++){
					if (FD_ISSET(i, &ready_sockets)){
						if (i == this->listen_socket){
							printf("\n incoming connection \n");
							int client_socket = accept(this->listen_socket, NULL, NULL); //this is where we connect to client
							if (client_socket >= 0)
							{
								//this is where we connect the client 
								User newUser; // i create an instance of the Userclass
								int check = getUser(client_socket, newUser); // I go get all the info, check password and store data if it passes
								if (check)
									clients_.push_back(newUser); // I add the user to the vector that stores clients
								else
								{
									strerror(errno);
									return ;
								}
							}
							else
								std::cout << COLOR_RED << "Error connecting to client\n" << COLOR_DEFAULT;

							}
						}
					}
				}
			else
				strerror(errno);
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
			return ;
		}
		
	}
}

int Server::getUser(int client_socket, User& user)
{

	//Receive password
	char password_buf[1024];
	int recv_bytes_pass = recv(client_socket, password_buf, sizeof(password_buf), 0);
		std::string password;
	if (recv_bytes_pass > 0){
		password.assign(password_buf, recv_bytes_pass);
		user.setPassword(password);

		//verify password before continuing
		if (password == this->__pwd) 
        std::cout << COLOR_GREEN << "\n PASSWORD OK" << std::endl;
		else {
    	    std::cout << "Error: incorrect password." << std::endl;
			return (1);
    	}

	}
    // Receive username
    char username_buf[1024];
    int recv_bytes = recv(client_socket, username_buf, sizeof(username_buf), 0);
    std::string username;
    if (recv_bytes > 0) {
        username.assign(username_buf, recv_bytes);
        user.setUsername(username);
    } else {
        std::cout << "Error receiving username." << std::endl;
        return 1;
    }

    // Receive nickname
    char nickname_buf[1024];
    recv_bytes = recv(client_socket, nickname_buf, sizeof(nickname_buf), 0);
    std::string nickname;
    if (recv_bytes > 0) {
        nickname.assign(nickname_buf, recv_bytes);
        user.setNickname(nickname);
    } else {
        std::cout << "Error receiving nickname." << std::endl;
        return 1;
    }
	std::cout << COLOR_GREEN << "\n User " << user.getUsername() << " with nickname " << user.getNickname() << " connected successfully 👌🏼" << std::endl;
	return (0);
}

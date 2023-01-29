#include "Server.hpp"
#include "tools.hpp"
// STEP 1

int Server::start(int port)
{
    int server_socket;
    int yes = 1;
    sockaddr_in sock;

	//setting up what IP and port server will listen on
    sock.sin_family = AF_INET;
    sock.sin_port = htons(port);
    int addr = inet_pton(AF_INET, "127.0.0.1", &sock.sin_addr);
    if (addr <= 0) {
    std::cerr << "Invalid IP address" << std::endl;
    return 1;
    }
	//creating server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        throw std::runtime_error(strerror(errno));
        return 1;
    }
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
	//binding server to socket address
    if (bind(server_socket, (sockaddr*)&sock, sizeof(sock)) < 0)
    {
        close(server_socket);
        throw std::runtime_error(strerror(errno));
        return 1;
    }
	//set to listening mode
    if (listen(server_socket, SOMAXCONN) < 0)
    {
        std::cerr << "Error listening: " << strerror(errno) << std::endl;
        return 1;
    }
    std::cout << "IRC Server successfully initialized and listening on " << inet_ntoa(sock.sin_addr) << ":" << ntohs(sock.sin_port) << std::endl;
	setServerSocket(server_socket);
	//now we wait for a client
	return (0);
}



void	Server::handleClientMessage(std::string data, int client_fd) 
{
    if (!(data.compare(0, 4, "PASS"))){ // add if client is authenticated yet to unlock user and other cmds, to do
        if (!(verifyPassword(data))) {
            std::cout << COLOR_RED << "wrong password" << COLOR_DEFAULT << std::endl;
            send(client_fd, "Password rejected\n", 17, 0);
            // close the connection or take additional measures for rejected clients
             }
            std::cout << "Client password accepted" << std::endl;
            std::string message = COLOR_GREEN "Password accepted" COLOR_DEFAULT "\n";
            send(client_fd, message.c_str(), message.length(), 0);
            //send(client_socket, "Create User with USER cmd\n", 26, 0);
            // additional code to handle authenticated client goes here
    }
   // else if (!(data.compare(0, 4, "USER")))
   //     setUsername(client_socket, data);
        

	     else {
        std::cout << "Invalid command" << std::endl;
    }
}



void	Server::go()
{
	std::vector<pollfd> fds;
	struct pollfd pfd;
	pfd.fd = this->server_socket;
	pfd.events = POLLIN;
	pfd.revents = 0;
	fds.push_back(pfd);
    
	while (true)
    {
        int ret = poll(fds.data(), fds.size(), -1);
        if (ret < 0)
        {
            std::cerr << "Error polling: " << strerror(errno) << std::endl;
            continue;
        }
        if (fds[0].revents & POLLIN)
        {
            sockaddr_in client_sock;
            socklen_t client_sock_len = sizeof(client_sock);
            //accept incoming connections
           	int  client_socket = accept(this->server_socket, (sockaddr*)&client_sock, &client_sock_len);
            printf("client socket %d\n\n", client_socket);
            if (client_socket < 0)
            {
                std::cerr << "Error accepting connection: " << strerror(errno) << std::endl;
                continue;
            }
			struct pollfd newClient;
			newClient.fd = client_socket;
			newClient.events = POLLIN;
			newClient.revents = 0;
            //this->addClientFd(client_socket);
			fds.push_back(newClient);
            std::cout << "Incoming connection from " << inet_ntoa(client_sock.sin_addr) << ":" << ntohs(client_sock.sin_port) << std::endl;
            std::cout << "Secure connection and confirm" << std::endl;
           // User User(ntohs(client_sock.sin_port));
        }
        for (std::vector<pollfd>::size_type i = 1; i < fds.size(); i++)
        {
            if (fds[i].revents & POLLIN)
            {
                char buffer[1024];
                int ret = recv(fds[i].fd, buffer, sizeof(buffer), 0);
                if (ret <= 0)
                {
                    std::cerr << "Error receiving data: " << strerror(errno) << std::endl;
                    close(fds[i].fd);
                    fds.erase(fds.begin() + i);
                    i--;
                    continue;
                }
                std::string message(buffer, ret);
				printf("%s client message of %d\n\n", message.c_str(), fds[i].fd);
                this->handleClientMessage(message, fds[i].fd);
            }
		}
	}
}

bool Server::verifyPassword(std::string password)
{
    password.erase(0,4); 
    password = trimWhitespace(password);
    return (password == getPassword());
}

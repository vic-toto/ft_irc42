#include "Server.hpp"
#include "tools.hpp"
#include "clientMessages.hpp"
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
    User user = getUser(client_fd);
    if(is_char_or_digit(data)) {
        if (!(data.compare(0, 6, "PRVMSG")))
        {
            std::string receiverNick = removeLeadingSpace(data.substr(7, (data.size())));
            int endNick = substr_to_first_space_or_end(receiverNick);
            std::cout << "first " << receiverNick << " " << endNick << std::endl;
            if (endNick == 0){
                send(user.getFd(), "Error, missing message \n ", 26, 0);
            }
            else
            {
                std::cout << receiverNick << " " << endNick << std::endl;
                receiverNick = receiverNick.substr(0, endNick);
                std::cout << receiverNick << " " << endNick << std::endl;
                if (isUser(receiverNick)){
                User receiver = getUser(receiverNick);
                std::string message = removeLeadingSpace(data.substr(6, data.size()));
                message.substr(0, receiverNick.size());
                message = cleanString(message);
                send(receiver.getFd(), "\nNew Private Message From ", 27, 0);
                send(receiver.getFd(), user.getNickname().data(), user.getNickname().size(), 0);
                send(receiver.getFd(), "\n", 1, 0);
                send(receiver.getFd(), message.data(), message.size(), 0);
                send(receiver.getFd(), "\n", 1, 0);
                } else {
                    send(user.getFd(), "Error, no user with nick ", 26, 0);
                    send(user.getFd(), receiverNick.data(), receiverNick.size(), 0);
                }
            }
            
        } else {
        std::string cmd = data.substr(0,4);
        std::string message = removeLeadingSpace(data.substr(5, (data.size())));
        message = cleanString(message);
        //CMDS
        if (!(cmd.compare(0, 4, "PASS"))) { // add if client is authenticated yet to unlock user and other cmds, to do
            if (user.getVerification() == 0){
                if (!(verifyPassword(message))) {
                    user.setVerification(1);
                    send(client_fd, PWDACCEPT, 20, 0);
                    updateUser(user);
                } else 
                    send(client_fd, PWDREJECT, 21, 0);
            } 
        } else if (user.getVerification()) {  
            if (!(cmd.compare(0, 4, "USER"))) {
                user.USER(message);
                updateUser(user);
            } else if (!(cmd.compare(0, 4, "NICK"))) {
                user.NICK(message);
                updateUser(user);
            }
            else if (!(cmd.compare(0, 4, "JOIN")) && !(user.getInChannel())) {
                if (message.empty() || message[0] != '#' || message.size() > 200)
                    send(user.getFd(), "Invalid channel name\n", 22, 0);
                if (channelExists(message)){
                    Channel channel = getChannel(message);
                    if (!(channel.getblackList(user.getNickname())))
                    {
                        channel.addUser(user);
                        channel.setChannelOperator(user);
                        user.setInChannel(1);
                        user.setWhatChannel(channel.getName());
                        updateUser(user);
                    }
                    // saves user in channel list
                    // updates the inChannel status
                    //saves in what channel
                } else {
                    Channel channel;
                    channel.setName(message);
                    channel.addUser(user);
                    user.setInChannel(1);
                    user.setWhatChannel(message);
                    updateUser(user);
                    addChannel(channel);
                }
            } else if (user.getInChannel())
            {
                sendMessageToChannel(user, message);
            }
            // }  else if (!(cmd.compare(0, 4, "QUIT"))) {
            // }  else if (!(cmd.compare(0, 4, "KICK"))) {
                else {
            std::cout << "Invalid command" << std::endl;
            send(client_fd, "Invalid command\n", 17, 0);
            // clientConsole(user);
            }
        } else {
            std::cout << "Invalid command" << std::endl;
            send(client_fd, "Invalid command\n", 17, 0);
            // clientConsole(user);
        } 
        }      
    }
    clientConsole(user);
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
            socklen_t client_sock_len = sizeof(client_sock); //accept incoming connections
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
			fds.push_back(newClient);
            std::cout << "Incoming connection from " << inet_ntoa(client_sock.sin_addr) << ":" << ntohs(client_sock.sin_port) << std::endl;
            std::cout << "Secure connection and confirm" << std::endl;
            User user(newClient.fd);
            this->addUser(user);
            if (!(user.getVerification()))
                send(user.getFd(), WELCOME, 84, 0);
        }
        for (std::vector<pollfd>::size_type i = 1; i < fds.size(); i++)
        { 
            if (fds[i].revents & POLLIN)
            {
                std::cout << "no of clients " << getNumberUsers() << std::endl;
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
                removeLeadingSpace(message);
				printf("%s client message of %d\n\n", message.c_str(), fds[i].fd);
                this->handleClientMessage(message, fds[i].fd);
            }
		}
	}
}

void    clientConsole(User user)
{
    if (!(user.getVerification())){
        send(user.getFd(), WELCOME, 84, 0);
        return ;
    } else {
        if (!(user.getUserVerification())){
            send(user.getFd(), "\nPlease set username with USER your_username\n", 46, 0);
            return ; }
        if (!(user.getNickVerification())) {
            send(user.getFd(), "\nPlease set nickname with NICK your_nickname\n", 46, 0);
            return ; 
        }
    }
    send(user.getFd(), user.getNickname().data(), user.getNickname().size(), 0);
    send(user.getFd(), " - ", 3, 0);
    if (user.getInChannel()){
        send(user.getFd(), user.getWhatChannel().data(), user.getWhatChannel().size(), 0);
        send(user.getFd(), " - ", 3, 0);}
}

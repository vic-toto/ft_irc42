#ifndef USER_HPP
#define USER_HPP
#include "Server.hpp"

class User {
    public:
        User() {};
        User(std::string const username, std::string const nickname) : username_(username), nickname_(nickname) {verified = -1;}

        std::string getUsername() const { return username_; }
        std::string getNickname() const { return nickname_; }
        std::string getPassword() const { return password_; }

        void setUsername(int client_socket, std::string username) {
            username.erase(0,4); 
            size_t first = username.find_first_not_of(" \t\r\n");
            if (first == std::string::npos) {
                std::cout << "\x1b[31m" << "Problem in setting User, empty string" << std::endl;
                send(client_socket, "User rejected\n", 13, 0);
                return ;}
            size_t last = username.find_last_not_of(" \t\r\n");
            username.erase(0, first);
            username.erase(last - first + 1);
            printf("ciaoooo\n");
            if (getVerification() == 0){
                username_ = username;
                verified = 1;
                std::cout <<  "\x1b[0m" << "User " << username << " verified" << std::endl;
                std::string message = "\x1b[0m User set up successfully";
                send(client_socket, message.c_str(), message.length(), 0);}
            else
                username_ = username;} // per ora lasciamo cosi ma ci dovrà essere il controllo sugli user esistenti

        void setNickname(std::string nickname) { nickname_ = nickname; }
        void setPassword(std::string password) { password_ = password; }

        void setVerification(int value) {verified = value;}
        int  getVerification() {return (verified);}

    private:
        int verified; 
        std::string username_;
        std::string nickname_;
        std::string password_;
};

#endif

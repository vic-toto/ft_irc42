#ifndef USER_HPP
#define USER_HPP


class User {
    public:
        User() {};
        User(std::string const username, std::string const nickname, std::string const password) : username_(username), nickname_(nickname), password_(password) {}

        std::string getUsername() const { return username_; }
        std::string getNickname() const { return nickname_; }
        std::string getPassword() const { return password_; }

        void setUsername(std::string username) { username_ = username; }
        void setNickname(std::string nickname) { nickname_ = nickname; }
        void setPassword(std::string password) { password_ = password; }

    private:
        std::string username_;
        std::string nickname_;
        std::string password_;
};

#endif

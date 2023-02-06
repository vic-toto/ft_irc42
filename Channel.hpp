#ifndef CHANNEL_HPP
#define CHANNEL_HPP


//CHANNEL - named group of one or more clinets which all receive message addressed to channel 
//        - names are str with & or #
//        - max name len 200
//        - name cannot have white spaces, ascii 7 or ','
//        - max 10 channels per USER
//        - +i means you can join channel only via invite
//        - has 'operator' which is the user owner of channel 
//        - channel has a mode
//        - has a ban list of users that cannot access
//        - can have a password

class Channel
{
private:
    int                 _invitation;
    std::string         _password;
    User                channelOperator;
    std::vector<User>   blackList;
    std::vector<User>   usersInChannel;
    std::string         name;

    /* data */
public:
    Channel() {}
    Channel(std::string name, std::string password, User channelOperator);
    Channel(std::string name, std::string password, User channelOperator, int invite);
    ~Channel() {};

    void    setName(std::string name) {this->name = name;}
    void    addUser(User user) {usersInChannel.push_back(user);}

    std::string getName() const {return this->name;}

    bool	getblackList(std::string nick){
			for (unsigned long i = 0; i < blackList.size(); i++) {
      		if (blackList[i].getNickname() == nick) {
      		  return true;
      		}
			}
			return false;
		}
};




#endif

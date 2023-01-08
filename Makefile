#  _   _                     ____          _  _      _  _               
# | | | |  ___  _ __   ___  | __ )  _   _ (_)| |  __| |(_) _ __    __ _ 
# | |_| | / _ \| '__| / _ \ |  _ \ | | | || || | / _` || || '_ \  / _` |
# |  _  ||  __/| |   |  __/ | |_) || |_| || || || (_| || || | | || (_| |
# |_| |_| \___||_|    \___| |____/  \__,_||_||_| \__,_||_||_| |_| \__, |
#                                                                 |___/ 
# 
#  ___  ____    ____      ____   _  _          _                
# |_ _||  _ \  / ___|    | __ ) (_)| |_   ___ | |__    ___  ___ 
#  | | | |_) || |        |  _ \ | || __| / __|| '_ \  / _ \/ __|
#  | | |  _ < | |___  _  | |_) || || |_ | (__ | | | ||  __/\__ \
# |___||_| \_\ \____|( ) |____/ |_| \__| \___||_| |_| \___||___/
#                    |/                                         

NAME = ircserver

SRCS =  main.cpp \
		# server/*.cpp \
		# client/*.cpp \
		# message/*.cpp \
		# bot/*cpp \

INCLUDES =  color.h \
			# server/*.hpp \
			# client/*.hpp \
			# message/*.hpp \
			# bot/*hpp \

CC = c++

FLAGS = -Wall -Wextra -Werror -std=c++98

OBJS = $(SRCS:.cpp=.o)

$(NAME): $(OBJS)
	$(CC) $(FLAGS) -o $(NAME) $(SRCS) -I$(INCLUDES)

all : $(NAME)

leaks : $(NAME)
		leaks --atExit -- ./$(NAME)

clean :
	rm -rf $(OBJS)

fclean : clean
	rm -rf ${NAME}

re : fclean all
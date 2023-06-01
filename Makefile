################################################################################
########### VARIABLES

SRCS	=	./main.cpp \
			./ConfParser.cpp \
			./Server.cpp \
			./Location.cpp \
			./utils.cpp \

OBJS	=	$(SRCS:%.cpp=%.o)

NAME	=	webserv

CXX		=	c++

CXXFLAGS	=	-Wall -Wextra -Werror -std=c++98

###############################################################################
########### RULES

all:		$(NAME)

$(NAME):	$(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

clean:
	rm -f $(OBJS)

fclean:		clean
	rm -f $(NAME)

re:			fclean all

.PHONY:		all clean fclean re          
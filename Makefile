NAME		:=	ircserv

CC			:=	c++
CFLAGS		:=	-Wall -Wextra -Werror -std=c++98 -MMD -MP -g
CPPFLAGS	:=	-I./includes

# HELP COMMENT for dependencies:
# -MMD	is a compilor option that generates dependencies for object files,
# 		but does not include them in object files
# -MP	creates empty targets for all header files, which prevents an error
# 		message if the header file is deleted

RM 			:=	rm -f
DIR_DUP		= mkdir -p $(@D)

SRCS		:=	\
				Channel.cpp \
				Client.cpp \
				main.cpp \
				registration.cpp \
				Server.cpp \
				Utility.cpp \
				utils.cpp \
				Commands/join.cpp \
				Commands/mode.cpp \
				Commands/names.cpp \
				Commands/nick.cpp \
				Commands/pass.cpp \
				Commands/ping.cpp \
				Commands/user.cpp \
				Channels/kick.cpp \
				Channels/topic.cpp \

SRC_DIR		:=	srcs
OBJ_DIR		:=	objs

SRCS		:=	$(addprefix $(SRC_DIR)/, $(SRCS))

OBJS		:=	$(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

DEPS		:=	$(OBJS:.o=.d)

all:		$(NAME)

$(NAME):	$(OBJS)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(OBJS) -o $(NAME)
	$(info CREATE : $(NAME))

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(DIR_DUP)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@
	$(info CREATE : $@)

-include $(DEPS)

clean:
	$(RM) -r $(OBJ_DIR)
	$(info REMOVED : object files)

fclean:		clean
	$(RM) $(NAME)
	$(info REMOVED : binary file)

re: 		fclean all

.PHONY: 	all clean fclean re
.SILENT:

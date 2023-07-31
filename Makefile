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
MAKEFLAGS   := --no-print-directory --jobs -C

SRCS		:=	\
				Channel.cpp \
				Client.cpp \
				main.cpp \
				registration.cpp \
				Server.cpp \
				Utility.cpp \
				utils.cpp \
				Commands/kick.cpp \
				Commands/join.cpp \
				Commands/part.cpp \
				Commands/privmsg.cpp \
				Commands/topic.cpp \
				Commands/invite.cpp \
				Commands/kill.cpp \
				Commands/mode.cpp \
				Commands/names.cpp \
				Commands/nick.cpp \
				Commands/pass.cpp \
				Commands/ping.cpp \
				Commands/user.cpp \
				Commands/who.cpp \
				Commands/quit.cpp \
				Commands/squit.cpp \

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

re:
	$(MAKE) clean
	$(MAKE) all


# Debug Rules

LEAKS		:= valgrind -q --leak-check=yes --show-leak-kinds=all
CXXFLAGS    := -Wall -Wextra -Werror -std=c++98
DEVFLAGS    := -Wconversion -Wsign-conversion -pedantic
DEVFLAGS    += -fno-omit-frame-pointer -Og -D DEV
.PHONY:		leak
leak:
	$(MAKE) re CXXFLAGS="$(CXXFLAGS)"
	echo "Makefile message: PASS is 12341234"
	-$(LEAKS) ./$(NAME) "6667" "12341234"


.PHONY: 	all clean fclean re
.SILENT:

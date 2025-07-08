# ==============================================================================
# COLORS
# ==============================================================================

BLACK = \001\033[1;30m\002
RED = \001\033[1;31m\002
GREEN = \001\033[1;32m\002
YELLOW = \001\033[1;33m\002
BLUE = \001\033[1;34m\002
MAGENTA = \001\033[1;35m\002
CYAN = \001\033[1;36m\002
WHITE = \001\033[1;37m\002
EOC = \001\033[0m\002

# ==============================================================================
# VARIABLES
# ==============================================================================

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -I include
NAME = ircserv

SRC_DIR = src
OBJ_DIR = obj

SRCS = \
	$(SRC_DIR)/main.cpp				\
	$(SRC_DIR)/Server.cpp			\
	$(SRC_DIR)/Client.cpp			\
	$(SRC_DIR)/Channel.cpp			\
	$(SRC_DIR)/Utils.cpp			\
	$(SRC_DIR)/cmds/nick_cmd.cpp	\
	$(SRC_DIR)/cmds/pass_cmd.cpp	\
	$(SRC_DIR)/cmds/user_cmd.cpp	\
	$(SRC_DIR)/cmds/join_cmd.cpp	\
	$(SRC_DIR)/cmds/cap_cmd.cpp		\
	$(SRC_DIR)/cmds/invite_cmd.cpp	\
	$(SRC_DIR)/cmds/kick_cmd.cpp	\
	$(SRC_DIR)/cmds/list_cmd.cpp	\
	$(SRC_DIR)/cmds/mode_cmd.cpp	\
	$(SRC_DIR)/cmds/names_cmd.cpp	\
	$(SRC_DIR)/cmds/privmsg_cmd.cpp	\
	$(SRC_DIR)/cmds/topic_cmd.cpp


OBJS = \
	$(OBJ_DIR)/main.o				\
	$(OBJ_DIR)/Server.o				\
	$(OBJ_DIR)/Client.o				\
	$(OBJ_DIR)/Channel.o			\
	$(OBJ_DIR)/Utils.o				\
	$(OBJ_DIR)/cmds/nick_cmd.o		\
	$(OBJ_DIR)/cmds/pass_cmd.o		\
	$(OBJ_DIR)/cmds/user_cmd.o		\
	$(OBJ_DIR)/cmds/join_cmd.o		\
	$(OBJ_DIR)/cmds/cap_cmd.o		\
	$(OBJ_DIR)/cmds/invite_cmd.o	\
	$(OBJ_DIR)/cmds/kick_cmd.o		\
	$(OBJ_DIR)/cmds/list_cmd.o		\
	$(OBJ_DIR)/cmds/mode_cmd.o		\
	$(OBJ_DIR)/cmds/names_cmd.o		\
	$(OBJ_DIR)/cmds/privmsg_cmd.o	\
	$(OBJ_DIR)/cmds/topic_cmd.o

# ==============================================================================
# RECIPES
# ==============================================================================

all: $(NAME)

$(NAME): $(OBJS)
	@$(CXX) $(CXXFLAGS) -o $@ $^
	@echo "\n$(GREEN)IRCSERV build completed!$(EOC)\n"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "$(CYAN)Compiling $<...$(EOC)"
	$(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	@echo "\n$(WHITE)Removing .o files...$(EOC)"
	@rm -rf $(OBJ_DIR)
	@echo "$(GREEN)Clean done.$(EOC)"

fclean: clean
	@echo "\n$(WHITE)Removing executable...$(EOC)"
	@rm -f $(NAME)
	@echo "$(GREEN)Fclean done.$(EOC)\n"

re: fclean all

.PHONY: all clean fclean re
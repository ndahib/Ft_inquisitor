# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ndahib <ndahib@student.1337.ma>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/02/04 11:03:01 by ndahib            #+#    #+#              #
#    Updated: 2026/02/16 11:19:51 by ndahib           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME        := inquisitor

#=== Directories ===============================================================
SRC_DIR     := src
OBJ_DIR     := .build
INC_DIR     := src

#=== Files =====================================================================
SRCS        := $(wildcard $(SRC_DIR)/*.c)
HEADERS     := $(wildcard $(INC_DIR)/*.h)
OBJS        := $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

#=== Commands & Flags ==========================================================
CC          := cc
CFLAGS      := -Wall -Wextra -Werror  -I $(INC_DIR) -fsanitize=address
LDFLAGS     := -lpcap
RM          := rm -rf

#=== Colors ====================================================================
DEF         := \033[0m
GREEN       := \033[1;32m
YELLOW      := \033[1;33m
RED         := \033[1;31m
CYAN        := \033[1;36m

#=== Rules =====================================================================
all: $(NAME)


$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) -lpcap -o $(NAME) $(LDFLAGS)
	@printf "$(GREEN)[OK] $(CYAN)$(NAME) linked successfully!$(DEF)\n"


$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	@mkdir -p $(OBJ_DIR)
	@printf "$(YELLOW)Compiling: $(DEF)$< \n"
	@$(CC) $(CFLAGS) -c $< -o $@


deps:
	@printf "$(YELLOW)Checking dependencies...$(DEF)\n"
	@if [ -f /etc/debian_version ]; then \
		sudo apt-get update && sudo apt-get install -y libpcap-dev; \
	elif [ -f /etc/fedora-release ] || [ -f /etc/redhat-release ]; then \
		sudo dnf install -y libpcap-devel; \
	elif [ -f /etc/arch-release ]; then \
		sudo pacman -S --noconfirm libpcap; \
	else \
		echo "$(RED)OS not supported for auto-install.$(DEF)"; \
	fi
	@printf "$(GREEN)Dependencies installed!$(DEF)\n"

clean:
	@$(RM) $(OBJ_DIR)
	@echo "$(RED)Object files removed.$(DEF)"

fclean: clean
	@$(RM) $(NAME)
	@echo "$(RED)Executable removed.$(DEF)"

re: fclean all

.PHONY: all clean fclean re deps
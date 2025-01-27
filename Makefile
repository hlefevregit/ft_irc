# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: hugolefevre <hugolefevre@student.42.fr>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/01/23 13:36:33 by hulefevr          #+#    #+#              #
#    Updated: 2025/01/27 10:46:12 by hugolefevre      ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC		=	c++

CFLAGS	=	-Werror -Wall -Wextra -std=c++98 -Wno-shadow -fsanitize=address

SRC		=	main.cpp \
			./src/Client.cpp \
			./src/Server.cpp \
			./src/Channel.cpp \
			./src/Parsing.cpp \

NAME	=	ircserv

OBJ		=	$(SRC:.cpp=.o)

%.o:%.cpp
	@echo "📦 $(GREEN)Compilation de $<...$(RESET)"
	@$(CC) $(CFLAGS) -c $< -o $@

$(NAME)	: $(OBJ)
	@echo "🔧 $(GREEN)Compilation de $(NAME)...$(RESET)"
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJ)
	@echo "✅ $(GREEN)Compilation terminée avec succès !$(RESET)"

all		:	$(NAME)

clean	:
	@echo "🧹 $(RED)Suppression des fichiers objets...$(RESET)"
	@rm -rf $(OBJ)

fclean	:	clean
	@echo "🗑️ $(RED) Suppression de l'exécutable...$(RESET)"
	@rm -rf $(NAME)

re		:	fclean all

.PHONY	:	all clean fclean re

#	COLOR SETTING	#############################################################
RESET   	=	\033[0m
GREEN     	=	\033[0;32m
RED			=	\033[0;31m
#################################################################################
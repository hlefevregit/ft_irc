# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ldalmass <ldalmass@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/01/23 13:36:33 by hulefevr          #+#    #+#              #
#    Updated: 2025/04/04 17:28:19 by ldalmass         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC		=	c++

CFLAGS	=	-Werror -Wall -Wextra -std=c++98 -Wno-shadow -g

SRC		=	main.cpp \
			./src/Client.cpp \
			./src/Server.cpp \
			./src/Channel.cpp \
			./src/Parsing.cpp \
			./src/cmd/Command.cpp \
			./src/numericalReplies.cpp \
			./src/cmd/joinChannel.cpp \
			./src/cmd/changeID.cpp \
			./src/Grid.cpp \
			./src/Utils.cpp \
			./src/cmd/Authentification.cpp \
			./src/cmd/Privmsg.cpp \
			./src/Bot.cpp \
			./src/Log.cpp \
			./src/printMessage.cpp \
			./src/cmd/Topic.cpp \
			./src/cmd/leaveChannel.cpp \
			./src/cmd/Mode.cpp \
			./src/cmd/kickFromChannel.cpp \
			./src/cmd/Invite.cpp \

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
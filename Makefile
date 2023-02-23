# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/02/08 15:19:33 by llefranc          #+#    #+#              #
#    Updated: 2023/02/22 15:40:45 by llefranc         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		=	taskmaster

SRCS		=	main.cpp ConfigParser.cpp ProgramBlock.cpp ProcInfo.cpp \
			TaskMaster.cpp Logger.cpp Spawner.cpp

PATH_SRCS	=	src/

OBJS		=	$(SRCS:.cpp=.o)

HDRS		=	src/*.hpp

CC		=	g++

FLAGS		=	-MD -std=c++11 -g -fsanitize=address -Wall -Werror -Wextra

all		:	$(NAME)

$(NAME)		:	$(addprefix $(PATH_SRCS), $(OBJS)) $(HDRS)
				@$(CC) -o $(NAME) $(addprefix $(PATH_SRCS), $(OBJS)) $(FLAGS)
				@echo "taskmaster is ready";

clean		:
				@rm -rf $(addprefix $(PATH_SRCS), $(OBJS))

fclean		:	clean
				@rm -rf $(NAME)

re		:	fclean all

.PHONY		:	all clean fclean re

%.o		:	%.cpp
				@$(CC) $(FLAGS) -o $@ -c $<

# To detect changes with header files, works with -MD flasgs for gcc
-include $(OBJS:.o=.d)

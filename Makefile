# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/02/08 15:19:33 by llefranc          #+#    #+#              #
#    Updated: 2023/02/19 17:51:57 by llefranc         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		=	taskmaster

SRCS		=	main.cpp ConfigParser.cpp ProgramBlock.cpp ProcInfo.cpp \
			TaskMaster.cpp Logger.cpp

PATH_SRCS	=	src/

OBJS		=	$(SRCS:.cpp=.o)

HDRS		=	src/*.hpp

CC		=	g++

FLAGS		=	 -std=c++11 -g -fsanitize=address -Wall -Werror -Wextra

all		:	$(NAME)

%.o		:	%.cpp
				@$(CC) $(FLAGS) -o $@ -c $<

$(NAME)		:	$(addprefix $(PATH_SRCS), $(OBJS)) $(HDRS)
				@$(CC) -o $(NAME) $(addprefix $(PATH_SRCS), $(OBJS)) $(FLAGS)
				@echo "taskmaster is ready";

clean		:
				@rm -rf $(addprefix $(PATH_SRCS), $(OBJS))

fclean		:	clean
				@rm -rf $(NAME)

re		:	fclean all

.PHONY		:	all clean fclean re
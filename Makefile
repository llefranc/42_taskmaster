# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/02/08 15:19:33 by llefranc          #+#    #+#              #
#    Updated: 2023/02/08 15:32:08 by llefranc         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		=	taskmaster

SRCS		=	main.cpp ProgramBlock.cpp

PATH_SRCS	=	src/

OBJS		=	$(SRCS:.cpp=.o)

HDRS		=	src/*.hpp

CC		=	g++

FLAGS		=	-g -fsanitize=address -Wall -Werror -Wextra

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
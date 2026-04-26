NAME = codexion

CC = cc
FLAGS = -Wall -Wextra -Werror

SRCS =  edf_helper.c \
        scheduler.c \
        threads.c \
        init_thread.c \
        monitor.c \
        thread_tool.c \
        \
        main.c \
        parser.c \
        thread_helper.c \
		small_helper.c \
		utility.c

OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -o $(NAME)

%.o: %.c
	$(CC) $(FLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
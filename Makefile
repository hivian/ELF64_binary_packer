
NAME 		= packer
CC			= clang
CFLAGS 		= -Wall -Wextra -Werror

LIBS		=

SRC_DIR 	= srcs
INC_DIR		= incs

SRC  		= main.c \
			  run.c \
			  section.c \
			  segment.c \
			  data.c \
			  encrypt.c \
			  error.c \
			  utility.c \

OBJ  		= $(SRC:%.c=objs/%.o)

all: objs $(NAME)

objs:
	@mkdir objs

$(NAME): $(OBJ)
	@printf "Compiling packer ... "
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJ) $(LIBS)
	@echo "Done"

objs/%.o: $(SRC_DIR)/%.c
	@$(CC) $(CFLAGS) -I $(INC_DIR) -o $@ -c $<

clean:
	@rm -rf objs

fclean: clean
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

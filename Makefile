NAME      = webserv

CXX       = c++

#CXXFLAGS  = -Wall -Wextra -Werror -std=c++98

OBJ_DIR   = obj

#HEADERS   = iter.hpp

SRCS      = main.cpp

OBJS      =	$(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRCS))

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR)/%.o: %.cpp $(HEADERS)
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: all clean fclean re
all: $(NAME)

clean:
	@rm -rf $(OBJ_DIR)

fclean: clean
	@rm -f $(NAME)

re: fclean all

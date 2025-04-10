CC = g++
CCFLAGS = -Wall -Wextra -std=c++2a -O0 -g
DIR_OBJ = ./obj/
OBJ_FILES = $(addprefix $(DIR_OBJ), main.o)
LIBRARIES = -lcurl 

main: $(OBJ_FILES)
	$(CC) $(CCFLAGS) $(OBJ_FILES) -o main $(LIBRARIES)

$(DIR_OBJ)%.o: %.cc | $(DIR_OBJ)
	$(CC) $(CCFLAGS) -c $< -o $@

$(DIR_OBJ):
	mkdir -p $(DIR_OBJ)

clean:
	rm -r -f $(DIR_OBJ) main core TEMP_FILE_MADE_BY_INPUT_TESTER

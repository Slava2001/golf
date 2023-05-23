CC=gcc

FREEGLUT_INCLUDE="C:/library/freeglut/include"
FREEGLUT_LIB="C:/library/freeglut/lib"
C_FLAGS=-Wall -Wextra -Wpedantic -Werror -O3 -Wno-comment
LD_FLAGS=-lfreeglut -lopengl32 -lm

INCLUDES=./include

PHYSIC_LIB_SRC= ./src/physic.c ./src/physic-object.c ./src/physic-debug-draw.c ./src/physic-util.c ./src/physic-collider.c
SOURCE=$(PHYSIC_LIB_SRC) ./src/main.c ./src/debug_draw.c ./src/vector.c
OBJECTS=$(SOURCE:.c=.o)

PROG_NAME=example

all: $(PROG_NAME)

.c.o:
	$(CC) -c $(C_FLAGS) -I$(INCLUDES) -I$(FREEGLUT_INCLUDE) $< -o $@

start: clean $(PROG_NAME)
	./$(PROG_NAME)

$(PROG_NAME): $(OBJECTS)
	$(CC) -L$(FREEGLUT_LIB) $(OBJECTS) $(LD_FLAGS) -o ./$(PROG_NAME)

clean:
	del .\src\*.o /s /q
	del .\*.exe /s /q

CC = g++ 


INC = ./include
SFML_INC = /usr/local/Cellar/sfml/2.5.1/include
SFML_LIB = /usr/local/Cellar/sfml/2.5.1/lib
SRC = ./src
OBJ = ./build

CFLAGS = -g -Wall -I${INC} -I${SFML_INC} -std=c++11 
LDFLAGS = -L${SFML_LIB} -lsfml-graphics -lsfml-window -lsfml-system



SOURCES = $(wildcard ${SRC}/*.cpp)

# SOURCES += ${MAIN_FILE}
# $(info $(SOURCES))



OBJS = $(patsubst %.cpp,${OBJ}/%.o,$(notdir ${SOURCES}))

# $(info $(OBJS))
TARGET = NESemu

${TARGET}: ${OBJS}
	$(CC)  ${OBJS} -o $@   $(LDFLAGS)

#生成中间文件
${OBJ}/%.o:${SRC}/%.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY:clean

clean:
	find $(OBJ) -name *.o -exec rm -rf {} \;
	# rm -rf main.o
	rm -rf ${TARGET}



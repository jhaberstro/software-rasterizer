CC=clang++
CFLAGS=-c -Wall -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.7.sdk -std=c++11
LDFLAGS=-framework OpenGL -framework GLUT -Lexternal/libcxx/lib -lc++
INCLUDE_FLAGS=-Iexternal/glm/ -isystem external/libcxx/include/
BIN_DIR=bin
OBJ_DIR=build
SOURCE_DIR=src
SOURCES=src/main.cpp src/Framebuffer.cpp src/Context.cpp src/Pipeline.cpp src/DefaultRasteriser.cpp
OBJECTS= $(addprefix $(OBJ_DIR)/, \
		$(patsubst %.cpp, %.o,  \
		$(patsubst %.c, %.o, $(SOURCES))))
EXECUTABLE=software-rasterizer

CFLAGS += ${INCLUDE_FLAGS}

all: $(EXECUTABLE)

debug: CFLAGS += -DDEBUG -g
debug: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $(BIN_DIR)/$@

$(OBJ_DIR)/%.o: %.cpp 
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $(OBJ_DIR)/$*.o
	@touch $@
			
$(SOURCES):
	@mkdir -p $(dir $@)
	@touch $@

clean:
	rm -r $(OBJ_DIR) $(BIN_DIR)

print:
	@echo $(OBJECTS)
	@echo $(SOURCES)

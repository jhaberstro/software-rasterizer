CC=clang
CXX=clang++
COMMON_FLAGS =-c -Wall -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.7.sdk
CFLAGS=
CXXFLAGS=-std=c++11
LDFLAGS=-framework OpenGL -framework GLUT -Lexternal/libcxx/lib -lc++
INCLUDE_FLAGS=-Iexternal/glm/ -Iexternal/stb_image/ -isystem external/libcxx/include/
RESOURCE_DIR=resources
BIN_DIR=bin
OBJ_DIR=build
SOURCE_DIR=src
SOURCES=src/main.cpp src/Framebuffer.cpp src/Renderer.cpp src/DefaultRasteriser.cpp src/PLYLoader.cpp external/stb_image/stb_image.c
OBJECTS= $(addprefix $(OBJ_DIR)/, \
		$(patsubst %.cpp, %.o,  \
		$(patsubst %.c, %.o, $(SOURCES))))
EXECUTABLE=software-rasterizer

ifeq ($(BUILD), release)
	COMMON_FLAGS += -O2 -s -DNDEBUG
else
	COMMON_FLAGS += -O0 -g -DDEBUG
endif

CFLAGS += ${INCLUDE_FLAGS}
CFLAGS += ${COMMON_FLAGS}
CXXFLAGS += ${INCLUDE_FLAGS}
CXXFLAGS += ${COMMON_FLAGS}	

all: $(EXECUTABLE)

release:
	make "BUILD=release"

$(EXECUTABLE): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	cp ${RESOURCE_DIR}/** ${BIN_DIR}/
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $(BIN_DIR)/$@

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $< -o $(OBJ_DIR)/$*.o
	@touch $@

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $(OBJ_DIR)/$*.o
	@touch $@
			
$(SOURCES):
	@mkdir -p $(dir $@)
	@touch $@

clean:
	rm -r $(OBJ_DIR)/** $(BIN_DIR)/**

print:
	@echo $(OBJECTS)
	@echo $(SOURCES)

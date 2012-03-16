CC=clang++
CFLAGS=-c -Wall -std=c++0x
LDFLAGS=
BIN_DIR=bin
OBJ_DIR=build
SOURCE_DIR=src
SOURCES=src/main.cpp
OBJECTS= $(addprefix $(OBJ_DIR)/, \
		$(patsubst %.cpp, %.o,  \
		$(patsubst %.c, %.o, $(SOURCES))))
EXECUTABLE=software-rasterizer

all: $(EXECUTABLE)

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
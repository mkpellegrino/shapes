CPPFLAGS=-arch x86_64 -m64

DEBUG=-g -DDEBUG
OPT=-O2
OPT_SIZE=-O3 -Os -fno-threadsafe-statics -fno-exceptions -ffunction-sections -fdata-sections -fno-rtti -flto -fvisibility-inlines-hidden

BIN_DIR=./bin/

Shapes: Shapes.cpp
	g++ Shapes.cpp $(CPPFLAGS) $(OPT_SIZE) -o $(BIN_DIR)Shapes
	strip -no_uuid -A -u -S -X -N -x $(BIN_DIR)Shapes
	cp $(BIN_DIR)Shapes ./
	$(BIN_DIR)Shapes > ./Shapes.html

all: clean Shapes Debug 

Debug: Shapes.cpp
	g++ Shapes.cpp $(DEBUG) $(CPPFLAGS) -o $(BIN_DIR)Shapes-debug
	cp $(BIN_DIR)Shapes-debug ./
	$(BIN_DIR)Shapes-debug > ./Shapes.html

clean:
	rm -fR ./bin/Shapes*
	rm -f ./Shapes.html
	rm -f ./Shapes
	rm -f ./Shapes-debug




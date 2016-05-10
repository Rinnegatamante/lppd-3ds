TARGET := lppd-3ds
SOURCES := source
BUILD	:= build

LIBS := -llua -lopengl32 -lSDL -lSDL_image -lSDL_mixer -lpsapi

CFLAGS := -DNO_STDIO_REDIRECT -fpermissive
LFLAGS :=
LDARGS := -DNO_STDIO_REDIRECT

C_FILES	:= $(shell find $(SOURCES) -type f -name '*.cpp')
OBJ_FILES := $(patsubst %.cpp,%.o,$(C_FILES))
CC = $(GCC_PREFIX)g++
LD = $(GCC_PREFIX)g++

all: $(TARGET).exe

$(TARGET).exe: $(OBJ_FILES)
	@echo Linking object files...
	$(LD) -o $@ $(LDARGS) $^ $(LIBS)

%.o: %.cpp
	@echo Creating object file $@...
	$(CC) $(CFLAGS) -c $< -o $@
	
clean:
	@echo Cleaning...
	@rm -f $(OBJ_FILES)
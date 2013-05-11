
ifeq ($(platform),)
platform = unix
ifeq ($(shell uname -a),)
   platform = win
else ifneq ($(findstring MINGW,$(shell uname -a)),)
   platform = win
else ifneq ($(findstring Darwin,$(shell uname -a)),)
   platform = osx
else ifneq ($(findstring win,$(shell uname -a)),)
   platform = win
endif
endif

ifeq ($(platform), unix)
   TARGET := libretro.so
   fpic := -fPIC
   SHARED := -lz -shared -Wl,--version-script=link.T -Wl,--no-undefined
   GL_LIB := -lGL
else ifeq ($(platform), osx)
   TARGET := libretro.dylib
   fpic := -fPIC
   SHARED := -lz -dynamiclib
   GL_LIB := -framework OpenGL
else
   CC = gcc
   CXX = g++
   TARGET := retro.dll
   SHARED := -shared -static-libgcc -static-libstdc++ -s -Wl,--version-script=link.T -Wl,--no-undefined
   GL_LIB := -lopengl32 -lz
endif

ifeq ($(DEBUG), 1)
   CFLAGS += -O0 -g
   CXXFLAGS += -O0 -g
else
   CFLAGS += -O3
   CXXFLAGS += -O3
endif

SOURCES := $(wildcard *.cpp)
CSOURCES := $(wildcard *.c)
OBJECTS := $(SOURCES:.cpp=.o) $(CSOURCES:.c=.o)
CFLAGS += -std=gnu99 -Wall $(fpic)
CXXFLAGS += -std=gnu++03 -Wall $(fpic)

ifeq ($(GLES), 1)
   CFLAGS += -DGLES
   CXXFLAGS += -DGLES
   LIBS += -lGLESv2
else
   LIBS += $(GL_LIB)
endif

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(fpic) $(SHARED) $(INCLUDES) -o $@ $(OBJECTS) $(LIBS) -lm

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: clean


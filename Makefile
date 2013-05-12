
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
   SHARED := -shared -Wl,--version-script=link.T -Wl,--no-undefined
   GL_LIB := -lGL -lz
else ifeq ($(platform), osx)
   TARGET := libretro.dylib
   fpic := -fPIC
   SHARED := -dynamiclib
   GL_LIB := -framework OpenGL -lz
else ifeq ($(platform), qnx)
   TARGET := $(TARGET_NAME)_libretro_qnx.so
   fpic := -fPIC
   SHARED := -lcpp -lm -shared -Wl,-version-script=link.T -Wl,-no-undefined
	CC = qcc -Vgcc_ntoarmv7le
	CXX = QCC -Vgcc_ntoarmv7le_cpp
	AR = QCC -Vgcc_ntoarmv7le
	GLES = 1
	INCFLAGS = -Iinclude/qnx
	LIBS := -lz
else
   CXX = g++
   TARGET := retro.dll
   SHARED := -shared -static-libgcc -static-libstdc++ -s -Wl,--version-script=link.T -Wl,--no-undefined
   GL_LIB := -lopengl32 -lz
   INCFLAGS = -Iinclude/win32
endif

CXXFLAGS += $(INCFLAGS)
CFLAGS += $(INCFLAGS)

ifeq ($(DEBUG), 1)
   CXXFLAGS += -O0 -g
   CFLAGS += -O0 -g
else
   CXXFLAGS += -O3
   CFLAGS += -O3
endif

ifeq ($(INCLUDE_MINIZ), 1)
MINIZ_OBJ := msvc/deps/miniz/miniz.c
endif

SOURCES := $(wildcard *.cpp) $(wildcard *.c)
OBJECTS := $(SOURCES:.cpp=.o) $(MINIZ_OBJ:.c=.o)
CXXFLAGS += -Wall $(fpic)
CFLAGS += -Wall $(fpic)

ifeq ($(GLES), 1)
   CXXFLAGS += -DGLES
   CFLAGS += -DGLES
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


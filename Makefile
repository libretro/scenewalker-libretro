
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

TARGET_NAME := scenewalker

ifeq ($(platform), unix)
   TARGET := $(TARGET_NAME)_libretro.so
   fpic := -fPIC
   SHARED := -shared -Wl,--version-script=link.T -Wl,--no-undefined
   GL_LIB := -lGL
   LIBS := -lz
else ifeq ($(platform), osx)
   TARGET := $(TARGET_NAME)_libretro.dylib
   fpic := -fPIC
   SHARED := -dynamiclib
   GL_LIB := -framework OpenGL
   LIBS += -lz
else ifeq ($(platform), ios)
	TARGET := $(TARGET_NAME)_libretro_ios.dylib
	fpic := -fpic
	SHARED := -dynamiclib
	GL_LIB := -framework OpenGLES
	LIBS += -lz
   GLES = 1
	CXX = clang++ -arch armv7 -isysroot $(IOSSDK)
	DEFINES := -DIOS
	CFLAGS += $(DEFINES)
	CXXFLAGS += $(DEFINES)
else ifeq ($(platform), qnx)
   TARGET := $(TARGET_NAME)_libretro_qnx.so
   fpic := -fPIC
   SHARED := -lcpp -lm -shared -Wl,-version-script=link.T -Wl,-no-undefined
   CXX = QCC -Vgcc_ntoarmv7le_cpp
   AR = QCC -Vgcc_ntoarmv7le
   GLES = 1
   INCFLAGS = -Iinclude/qnx
   LIBS := -lz
else
   CXX = g++
   TARGET := $(TARGET_NAME)_retro.dll
   SHARED := -shared -static-libgcc -static-libstdc++ -s -Wl,--version-script=link.T -Wl,--no-undefined
   GL_LIB := -lopengl32
   LIBS := -lz
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
ifeq ($(platform), ios)
   LIBS += $(GL_LIB)
else
   LIBS += -lGLESv2
endif
else
   LIBS += $(GL_LIB)
endif

all: $(TARGET)

HEADERS := $(wildcard *.hpp) $(wildcard *.h)

$(TARGET): $(OBJECTS)
	$(CXX) $(fpic) $(SHARED) $(INCLUDES) -o $@ $(OBJECTS) $(LIBS) -lm

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: clean


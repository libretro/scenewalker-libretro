
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
   INCFLAGS += -I.
else ifeq ($(platform), pi)
   TARGET := $(TARGET_NAME)_libretro.so
   fpic := -fPIC
   SHARED := -shared -Wl,--version-script=link.T -Wl,--no-undefined
   CXXFLAGS += -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/vmcs_host/linux -DVIDEOCORE
   GLES := 1
   LIBS += -L/opt/vc/lib -lz
else ifeq ($(platform), ios)
   TARGET := $(TARGET_NAME)_libretro_ios.dylib
   fpic := -fpic
   SHARED := -dynamiclib
   GL_LIB := -framework OpenGLES
   LIBS += -lz
   GLES = 1
   CXX = clang++ -arch armv7 -isysroot $(IOSSDK)
   DEFINES := -DIOS
   CXXFLAGS += $(DEFINES)
else ifeq ($(platform), pi)
   TARGET := $(TARGET_NAME)_libretro.so
   fpic := -fPIC
   SHARED := -shared -Wl,--version-script=link.T -Wl,--no-undefined
   CXXFLAGS += -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/vmcs_host/linux
   GLES = 1
   LIBS += -L/opt/vc/lib -lz
else ifeq ($(platform), qnx)
   TARGET := $(TARGET_NAME)_libretro_qnx.so
   fpic := -fPIC
   SHARED := -lcpp -lm -shared -Wl,-version-script=link.T -Wl,-no-undefined
   CXX = QCC -Vgcc_ntoarmv7le_cpp
   AR = QCC -Vgcc_ntoarmv7le
   GLES = 1
   INCFLAGS = -Iinclude/compat
   LIBS := -lz
else ifeq ($(platform), sncps3)
   TARGET := $(TARGET_NAME)_libretro_ps3.a
   CC = $(CELL_SDK)/host-win32/sn/bin/ps3ppusnc.exe
   CXX = $(CELL_SDK)/host-win32/sn/bin/ps3ppusnc.exe
   AR = $(CELL_SDK)/host-win32/sn/bin/ps3snarl.exe
   DEFINES := -D__CELLOS_LV2__
   INCFLAGS = -I. -Iinclude/miniz -Iinclude/compat
   STATIC_LINKING = 1
else
   CXX = g++
   TARGET := $(TARGET_NAME)_retro.dll
   SHARED := -shared -static-libgcc -static-libstdc++ -s -Wl,--version-script=link.T -Wl,--no-undefined
   GL_LIB := -lopengl32
   LIBS := -lz
   INCFLAGS = -I. -Iinclude/win32
endif

CXXFLAGS += $(INCFLAGS)

ifeq ($(DEBUG), 1)
   CXXFLAGS += -O0 -g
else
   CXXFLAGS += -O3
endif

ifeq ($(INCLUDE_MINIZ), 1)
MINIZ_OBJ := msvc/deps/miniz/miniz.c
endif

SOURCES := $(wildcard *.cpp) $(wildcard *.c)
OBJECTS := $(SOURCES:.cpp=.o) $(MINIZ_OBJ:.c=.o)

ifeq ($(platform), sncps3)
CXXFLAGS += $(fpic)
else
CXXFLAGS += -Wall $(fpic)
endif

ifeq ($(GLES), 1)
   CXXFLAGS += -DGLES
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
ifeq ($(STATIC_LINKING), 1)
	$(AR) rcs $@ $(OBJECTS)
else
	$(CXX) $(fpic) $(SHARED) $(INCLUDES) -o $@ $(OBJECTS) $(LIBS) -lm
endif

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: clean


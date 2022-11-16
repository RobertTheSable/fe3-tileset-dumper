CC = x86_64-w64-mingw32-g++
FLAGS = -g -c -std=c++17
LIBS = /usr/x86_64-w64-mingw32/lib/libgdi32.a

BUILDDIR = build
LCDIR = DLLcode

EXECUTABLE = search
SOURCES = $(wildcard *.cpp $(LCDIR)/*.cpp)
OBJECTS = $(patsubst %.cpp,$(BUILDDIR)/%.o,$(SOURCES))

all: dir  $(BUILDDIR)/$(EXECUTABLE)

dir:
	mkdir -p $(BUILDDIR) $(BUILDDIR)/$(LCDIR)
	

$(BUILDDIR)/$(EXECUTABLE): $(OBJECTS)
	$(CC) -static $^ -o $@ -lgdi32

$(OBJECTS): $(BUILDDIR)/%.o : %.cpp
	$(CC) $(FLAGS) $< -o $@

clean:
	rm -f $(BUILDDIR)/$(EXECUTABLE)

CC:=g++
CXXFLAGS = -I ./inc -std=c++17 -O2 -Wall -DDEBUG
LIBS     = -lreadline -lcurses
DEL_FILE = rm -f

SRCDIR := src
OBJDIR := obj
INCDIR := inc
TESTDIR:= Spice_file
SRCS   := $(wildcard $(SRCDIR)/*.cpp)
OBJS   := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))

CASE   := opamp_right
# HEADERS = BTree.h HBTree.h THBTree.h readfile.h

# SOURCES = BTree.cpp HBTree.cpp THBTree.cpp readfile.cpp

# OBJECTS = BTree.o HBTree.o THBTree.o readfile.o

TARGET  = placement

# TOTAL	= g++ -std=c++17 main.cpp readfile.cpp BTree.cpp HBTree.cpp THBTree.cpp FHBTree.cpp SA.cpp

all: $(TARGET)

$(OBJDIR):
	@mkdir -p $(OBJDIR)

$(TARGET): main.cpp $(OBJS)
	$(CC) $(CXXFLAGS) $^ -o $@ $(LIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CC) $(CXXFLAGS) -c $< -o $@ $(LIBS)

run: $(TARGET)
	time ./$(TARGET) $(TESTDIR)/$(CASE).sp $(TESTDIR)/$(CASE)_sym.txt $(TESTDIR)/$(CASE)_proximity.txt $(CASE)_out.txt

clean:
	@$(DEL_FILE) $(OBJS) $(TARGET)

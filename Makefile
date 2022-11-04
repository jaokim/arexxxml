CC     = $(CROSS_COMPILE)g++
STRIP  = $(CROSS_COMPILE)strip
CXX    = $(CROSS_COMPILE)c++
AS     = $(CROSS_COMPILE)as
LD     = $(CROSS_COMPILE)ld
RANLIB = $(CROSS_COMPILE)ranlib
RM     = delete

# Change these as required
OPTIMIZE =
DEBUG =
CFLAGS = -gstabs -Wall $(OPTIMIZE) $(DEBUG)

# Flags passed to gcc during linking
LINK = -gstabs -mcrt=newlib

# Name of the "thing" to build
TARGET = ARexxXML

# Additional linker libraries
LIBS = -athread=native -lauto -lraauto -lexpat

# Source code files used in this project
# Add any additional files to this line

SRCS = $(wildcard *.cpp)

# -------------------------------------------------------------
# Nothing should need changing below this line

 

%.o : %.cpp
	$(CXX) -c -o $@ $< $(CFLAGS)

OBJS = $(SRCS:.cpp=.o)

# Rules for building      $(CC) $(LINK) -o $(TARGET) $(OBJS) $(LIBS)
$(TARGET): $(OBJS)
	$(CC) $(LINK) -o $(TARGET).debug $(OBJS) $(CFLAGS) $(LIBS) -Wl,--cref,-M,-Map=$@.map
	$(STRIP) -R.comment $(TARGET).debug -o $(TARGET)

.PHONY: clean
clean:
	$(RM) $(TARGET) $(TARGET).debug $(OBJS)

.PHONY: revision
revision:
	bumprev $(VERSION) $(TARGET)



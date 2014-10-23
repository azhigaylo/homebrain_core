CC=g++
CFLAGS=-c
LDFLAGS=
SOURCES=test.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=brain_app

LIBS += /pal/libwrapper.a

all: $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@
	
clean:
	rm -rf *.o

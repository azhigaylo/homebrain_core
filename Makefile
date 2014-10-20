CC=gcc
CFLAGS=-c
LDFLAGS=
SOURCES=test.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=test_app

all: $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@
	
clean:
	rm -rf *.o

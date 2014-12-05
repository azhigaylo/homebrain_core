
OBJDIR      := $(patsubst %/,%,$(BUILDDIR)/$(OBJDIR_NAME))
BINDIR      := $(patsubst %/,%,$(BUILDDIR)/$(BINDIR_NAME))

INC_DIRS    := $(patsubst ./%,%,$(addprefix $(patsubst %/,%,$(ROOT))/, $(SOURCE_SUBDIRS)))
SOURCE_DIRS := $(patsubst %/,%,$(ROOT)) $(INC_DIRS)
C_SOURCES   := $(patsubst ./%, %, $(wildcard $(addsuffix /*.c, $(SOURCE_DIRS))))
CPP_SOURCES := $(patsubst ./%, %, $(wildcard $(addsuffix /*.cpp, $(SOURCE_DIRS))))
C_OBJECTS   := $(C_SOURCES:.c=.o)
CPP_OBJECTS := $(CPP_SOURCES:.cpp=.o)

OBJECTS     := $(notdir $(C_OBJECTS) $(CPP_OBJECTS))
VPATH       := $(SOURCE_DIRS) $(OBJDIR) $(BINDIR)

all : $(TARGET_NAME)

$(TARGET_NAME) : $(OBJECTS) | dirs
	$(LINK) -o $(BINDIR)/$@ $(addprefix $(OBJDIR)/,$(OBJECTS)) $(LDFLAGS) $(LDLIBS)

dirs :
	@mkdir -p $(BUILDDIR) $(OBJDIR) $(BINDIR)

%.o : %.cpp | dirs
	$(CXX) -o $(OBJDIR)/$@ -c $< $(CPPFLAGS) -MD -MT $@ $(CXXFLAGS) $(addprefix -I , $(INC_DIRS))
 
%.o : %.c | dirs
	$(CC) -o $(OBJDIR)/$@ -c $< $(CPPFLAGS) -MD -MT $@ $(CFLAGS) $(addprefix -I , $(INC_DIRS))
 
.PHONY : clean
 
clean :
	rm -rf $(BUILDDIR)
 
-include $(addprefix $(OBJDIR)/,$(OBJECTS:.o=.d))


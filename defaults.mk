BUILDDIR    := .build
OBJDIR_NAME := obj
BINDIR_NAME := bin

ROOT        := .

CFLAGS      := -O0 -g3 -D_DEBUG -pthread -lrt
CXXFLAGS    := -O0 -g3 -D_DEBUG -pthread -lrt
CPPFLAGS    := 
LDFLAGS     :=
LDLIBS      :=  -pthread -lrt
#-L/home/azhigaylo/Qt5.4.0/5.4/gcc/lib -lSdb_Connection -lSdb_TraceSender -lQt5Core -lQt5Xml -lQt5Network -Wl,-rpath-link=/home/azhigaylo/Qt5.4.0/5.4/gcc/lib 

COMPILER_PREFIX := 
CC = $(COMPILER_PREFIX)gcc
CXX = $(COMPILER_PREFIX)g++
LINK = $(CC)

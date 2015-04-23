BUILDDIR    := .build
OBJDIR_NAME := obj
BINDIR_NAME := bin

ROOT        := .

CFLAGS      := -O0 -g3 -D_DEBUG -pthread -lrt
CXXFLAGS    := -O0 -g3 -D_DEBUG -pthread -lrt
CPPFLAGS    := 
LDFLAGS     :=
LDLIBS      :=  -pthread -lrt

COMPILER_PREFIX := 
CC = $(COMPILER_PREFIX)gcc
CXX = $(COMPILER_PREFIX)g++
LINK = $(CC)

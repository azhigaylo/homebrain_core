BUILDDIR    := .build
OBJDIR_NAME := obj
BINDIR_NAME := bin

ROOT        := .

CFLAGS      := -O0 -g3 -D_DEBUG
CXXFLAGS    := -O0 -g3 -D_DEBUG
CPPFLAGS    := 
LDFLAGS     := 
LDLIBS      := 

COMPILER_PREFIX := 
CC = $(COMPILER_PREFIX)gcc
CXX = $(COMPILER_PREFIX)g++
LINK = $(CC)

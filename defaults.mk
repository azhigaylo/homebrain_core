BUILDDIR    := .build
OBJDIR_NAME := obj
BINDIR_NAME := bin

ROOT        := .

CFLAGS      := -O0 -g3 -D_DEBUG -pthread -lrt
CXXFLAGS    := -O0 -g3 -D_DEBUG -pthread -lrt
CPPFLAGS    := 
LDFLAGS     :=
LDLIBS      :=  -pthread -lrt 

COMPILER_PREFIX := /home/azhigaylo/Work/HomeControl/buildroot/buildroot/output/host/usr/bin/
CC = $(COMPILER_PREFIX)arm-linux-gcc
CXX = $(COMPILER_PREFIX)arm-linux-g++
LINK = $(CC)

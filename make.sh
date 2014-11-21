#!/bin/bash

cd main

mkdir -p obj
mkdir -p bin

cd .. 

make build_flags="pc-linux" program_name=home_brain


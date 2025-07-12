#!/bin/bash
mkdir ./build && cd ./build && cmake -DENABLE_COVERAGE=ON .. && make && clear
ctest -V -T Test 
ctest -T Coverage
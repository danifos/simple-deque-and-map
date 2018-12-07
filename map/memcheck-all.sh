#!/bin/bash

test_names=("one" "two" "three" "four" "five" "six" "seven")
data_dir="data"

for test in ${test_names[@]}
do
	echo "memcheck "$test":"
	work_dir=$data_dir"/"$test".memcheck/"
	cp -f map.hpp $work_dir
	g++ $work_dir"code.cpp" -g
	valgrind --leak-check=full ./a.out
done

rm -f a.out

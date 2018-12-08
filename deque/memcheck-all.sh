#!/bin/bash

test_names=("one" "two" "three" "four")
file_names=("deque.hpp" "exceptions.hpp" "utility.hpp" "data/class-bint.hpp" "data/class-integer.hpp" "data/class-matrix.hpp")
data_dir="data"

for test in ${test_names[@]}
do
	echo "************************"
	echo "memcheck "$test":"
	work_dir=$data_dir"/"$test".memcheck/"
	for file in ${file_names[@]}
	do
		cp -f $file $work_dir
	done
	g++ $work_dir"code.cpp" -g
	valgrind --leak-check=full ./a.out > temp
	diff $work_dir"answer.txt" temp &>/dev/null
	if [ $? == 0 ]; then
		echo "Accepted"
	else
		echo "Wrong Answer"
	fi
	echo
done

rm -f a.out
rm -f temp

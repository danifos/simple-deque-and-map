#!/bin/bash

test_names=("one" "two" "three" "four" "five" "six")
file_names=("deque.hpp" "exceptions.hpp" "utility.hpp" "data/class-bint.hpp" "data/class-integer.hpp" "data/class-matrix.hpp")
data_dir="data"
num_failure=6

for test in ${test_names[@]}
do
	echo "test "$test":"
	work_dir=$data_dir"/"$test"/"
	for file in ${file_names[@]}
	do
		cp -f $file $work_dir
	done
	g++ -o2 -std=c++14 -o $work_dir"code" $work_dir"code.cpp"
	"./"$work_dir"code" > $work_dir"answer-check.txt"
	diff $work_dir"answer.txt" $work_dir"answer-check.txt" &>/dev/null
	if [ $? == 0 ]; then
		echo "Passed"
		num_failure=$(( $num_failure - 1 ))
	else
		echo "Failed"
		diff $work_dir"answer.txt" $work_dir"answer-check.txt"
	fi
done

if [ $num_failure == 0 ]; then
	echo "Passed all tests. Congratulations!"
else
	echo $num_failure" tests failed"
fi

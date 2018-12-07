#!/bin/bash

test_names=("one" "two" "three" "four" "five" "six" "seven")
data_dir="data"

for test in ${test_names[@]}
do
	echo "test "$test":"
	work_dir=$data_dir"/"$test"/"
	cp -f map.hpp $work_dir
	g++ -o2 -std=c++14 -o $work_dir"code" $work_dir"code.cpp"
	"./"$work_dir"code" > $work_dir"answer-check.txt"
	diff $work_dir"answer.txt" $work_dir"answer-check.txt" &>/dev/null
	if [ $? == 0 ]; then
		echo "Passed"
	else
		echo "Failed"
		diff $work_dir"answer.txt" $work_dir"answer-check.txt"
	fi
done

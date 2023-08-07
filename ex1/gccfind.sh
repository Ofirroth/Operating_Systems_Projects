#!/bin/bash

#if there is not enough parameters
path_of_dir=$1
word=$2
flag=$3
params=("$@")
num_params=${#params[@]}
if [ $num_params -lt 2 ];
  then 
    echo "Not enough parameters"
fi    
#delete all the compile files
#delete_files="*.out" 
rm -rf $path_of_dir/*.out
#current file
c_files=$(find $path_of_dir -maxdepth 1 -type f -name "*.c")
  for  j in $c_files
    do
      if grep -Fqw -i "$word" "$j"; 
        then
  gcc -w "$j" -o "${j:0:-2}.out"
      fi
  done
# Check if the third argument is "-r"
if [[ "$flag" == "-r" ]]; then
# Perform the action you want to take when the third argument is "-r"
    for i in $(ls -A $path_of_dir)
      do
        if [ -d "$path_of_dir/$i" ]; 
          then
          $($0 $path_of_dir/$i $word "-r")
          fi
    done
  # Perform the action you want to take when the third argument is not "-r"
fi


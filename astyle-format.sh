#!/bin/bash
dirs=( "include" "include/exceptions" "src/arma3-unix-launcher-library" "tests" )
astylerc=`realpath astylerc`
for directory in "${dirs[@]}"
do
    pushd $directory
    astyle --options=$astylerc -n *.cpp
    astyle --options=$astylerc -n *.hpp
    popd
done

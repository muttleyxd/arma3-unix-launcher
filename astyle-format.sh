#!/bin/bash
pushd `dirname "$(readlink -f "$0")"`

dirs=( "include" "include/exceptions" "src/arma3-unix-launcher" "src/arma3-unix-launcher-library" "src/arma3-unix-launcher-library/exceptions" "tests/fuzzing" "tests/fuzzing/arma3-unix-launcher-library" "tests/unit/arma3-unix-launcher-library" )
astylerc=`realpath astylerc`
for directory in "${dirs[@]}"
do
    pushd $directory
    astyle --options=$astylerc -n *.cpp
    astyle --options=$astylerc -n *.hpp
    popd
done

popd

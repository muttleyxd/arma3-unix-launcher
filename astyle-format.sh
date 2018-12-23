#!/bin/bash
pushd `dirname "$(readlink -f "$0")"`

dirs=( "include" "include/exceptions" "src/arma3-unix-launcher-library" "tests/include" "tests/src" )
astylerc=`realpath astylerc`
for directory in "${dirs[@]}"
do
    pushd $directory
    astyle --options=$astylerc -n *.cpp
    astyle --options=$astylerc -n *.hpp
    popd
done

popd

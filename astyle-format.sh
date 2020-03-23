#!/bin/bash
pushd "`dirname "$(readlink -f $0)"`" >/dev/null

is_file_ignored()
{
    if [[ $1 == ./external/* ]]; then
        return 1
    fi
    GIT_IGNORED=`git check-ignore -q $1; echo $?`
    if [ $GIT_IGNORED -eq 0 ]; then
        return 1
    fi
    return 0
}

astylerc=`realpath astylerc`
for file in `find . -name "*.cpp" -or -name "*.hpp" -or -name "*.h"`
do
    is_file_ignored $file
    if [ $? -eq 0 ]; then
        astyle --options="$astylerc" -q -n $file
    fi
done

popd >/dev/null

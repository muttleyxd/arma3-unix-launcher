#!/bin/bash

cat /etc/*-release | grep debian > /dev/null
if [ $? -ne 0 ]; then
   echo "You are not Debian-based distro!"
   exit 1
fi

shortCommitHash=`git rev-parse --verify HEAD | cut -c -7`
branchName=`git branch | grep "*" | cut -c 3-`
commitCount=`git rev-list HEAD --count`
distroName=`cat /etc/*-release | grep "^ID=.*" | cut -c 4-`
architecture=`arch`

if [ $architecture == "x86_64" ]; then
    debConfigArchitecture="amd64";
else
    debConfigArchitecture="i386";
fi

dirName="arma3-unix-launcher_$branchName-$commitCount-$shortCommitHash-$distroName-$architecture"

rm -rf build-deb-pkg/
mkdir build-deb-pkg/
cd build-deb-pkg

cmake ..
make -j
make install DESTDIR="$(pwd)/$dirName"

cd $dirName
mkdir DEBIAN

echo "Package: arma3-unix-launcher" > DEBIAN/control
echo "Version: $commitCount-$shortCommitHash" >> DEBIAN/control
echo "Section: base" >> DEBIAN/control
echo "Priority: optional" >> DEBIAN/control
echo "Architecture: $debConfigArchitecture" >> DEBIAN/control
echo "Depends: libgtkmm-3.0-dev (>= 3.14.0)" >> DEBIAN/control
echo "Maintainer: Muttley <muttdini@gmail.com>" >> DEBIAN/control
echo "Description: Advanced launcher Linux and Mac ArmA 3" >> DEBIAN/control

cd ..
dpkg-deb --build $dirName

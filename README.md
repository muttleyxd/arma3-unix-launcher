# ArmA 3 Unix Launcher

ArmA 3 Launcher for Linux and Mac.
Since Bohemia didn't port their launcher to Linux and Mac and existing launcher didn't satisfy my needs I decided to create my own.

Launcher detects and symlinks all mods from Workshop and ArmA's main directory. It also allows to add your own mods from outside ArmA directory.

### Features

* Read location of ArmA 3 (config.vdf parsing)
* Workshop mods support (symlink to ~arma/!workshop)
* Detect @mods (ArmA main dir)
* Add mods from outside ArmA's dir (symlink)
* Launch ArmA with desired options
* Cross platform - Linux and Mac

### TODO

* Steam integration (info about downloading)
* Server browser
* .paa reading - for displaying mod images in launcher
* Mod dependency caching from Steam Workshop
* MacOS binary build
* Code refactoring (logic should be outside GUI code)
* PPA for Ubuntu

### Installing

#### Debian based (Debian, Ubuntu)

There is a .deb package available in releases tab!

[Releases tab](https://github.com/muttleyxd/arma3-unix-launcher/releases)

#### Arch based (Arch, Antergos, Manjaro)

For now there's AUR package available - it's called arma3-linux-launcher-git.

    yaourt -S arma3-linux-launcher-git

#### Mac OS X

Binary package is currently unavailable. Please build from source.

### Building from source

Gtkmm3 is required

#### Debian package
    apt-get install libgtkmm-3.0-1v5

#### Arch Linux
    pacman -S gtkmm3

#### Mac OS X
    brew install gtkmm3

    git clone https://github.com/muttleyxd/arma3_unix_launcher.git
    cd arma3_unix_launcher
    mkdir build
    cd build
    cmake ..
    make

After that you can launch with

    ./arma3_unix_launcher

Or install globally with

    sudo make install

#### Launching on Linux

    arma3_unix_launcher

#### Launching on Mac OS X

Go to Applications and run "arma3-unix-launcher"

### Launch parameters

    --verbose

This enables verbose logging - useful if something works in a wrong way

    --purge

This deletes all files created by the launcher (symlinks and config files)


### Screenshots

Mods tab:
![mods tab](http://i.imgur.com/OmN0IDe.png)

Parameters tab:
![parameters tab](http://i.imgur.com/IseHvUc.png)




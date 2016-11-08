# ArmA 3 Linux Launcher

ArmA 3 Launcher for Linux.
Since Bohemia didn't port their launcher to Linux and existing launcher didn't satisfy my needs I decided to create my own.

Launcher detects and symlinks all mods from Workshop and ArmA's main directory. It also allows to add your own mods from outside ArmA directory.

### Features

* Read location of ArmA 3 (config.vdf parsing)
* Workshop mods support (symlink to ~arma/!workshop)
* Detect @mods (ArmA main dir)
* Add mods from outside ArmA's dir (symlink)
* Launch ArmA with desired options

### TODO

* Steam integration (info about downloading)
* Server browser
* .paa reading - for displaying mod images in launcher
* Mod dependency caching from Steam Workshop
* MacOS port
* Code refactoring (logic should be outside GUI code)
* PPA for Ubuntu

### Installing

#### Debian based (Debian, Ubuntu)

There's a .deb package available in releases tab!

[Releases tab](https://github.com/muttleyxd/arma3-linux-launcher/releases)

#### Arch based (Arch, Antergos, Manjaro)

For now there's AUR package available - it's called arma3-linux-launcher-git.

    yaourt -S arma3-linux-launcher-git

### Building from source

Gtkmm is required

    git clone https://github.com/muttleyxd/arma3_linux_launcher.git
    cd arma3_linux_launcher
    mkdir build
    cd build
    cmake ..
    make

After that you can launch with

    ./arma3_linux_launcher

Or install globally with

    sudo make install
    arma3_linux_launcher

### Launch parameters

    ./arma3_linux_launcher --verbose

This enables verbose logging - useful if something works in a wrong way


    ./arma3_linux_launcher --purge

This deletes all files created by the launcher (symlinks and config files)


### Screenshots

Mods tab:
![mods tab](http://i.imgur.com/OmN0IDe.png)

Parameters tab:
![parameters tab](http://i.imgur.com/IseHvUc.png)




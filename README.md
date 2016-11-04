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
* AUR package for Arch-based, PPA for Ubuntu


### Building

Gtkmm is required

    git clone https://github.com/muttleyxd/arma3_linux_launcher.git
    cd arma3_linux_launcher
    make

After that you can launch with

    ./arma3_linux_launcher

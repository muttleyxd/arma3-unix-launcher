# Arma 3 + DayZ SA Unix Launcher

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/8a144e12d9cc4cde90616f0e3f282322)](https://www.codacy.com/manual/muttleyxd/arma3-unix-launcher?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=muttleyxd/arma3-unix-launcher&amp;utm_campaign=Badge_Grade) [![Build Status](https://img.shields.io/drone/build/muttleyxd/arma3-unix-launcher?label=Linux%20build&logo=drone)](https://cloud.drone.io/muttleyxd/arma3-unix-launcher) [![Actions Status](https://img.shields.io/github/workflow/status/muttleyxd/arma3-unix-launcher/Mac%20OS%20X%20release%20deployment/master?label=Mac%20OS%20X%20build)](https://github.com/muttleyxd/arma3-unix-launcher/actions)

A clean, intuitive Arma 3 + DayZ SA Launcher for GNU/Linux and MacOS.

Since Bohemia Interactive's developers have not ported the official Arma 3 and DayZ Launchers to GNU/Linux and MacOS, and the existing Steam Proton launch options did not satisfy the community's needs, I decided to create a new one. The Arma 3 + DayZ SA Unix Launcher is intuitive, working with all of Bohemia Interactive's launch parameters for each title and integrating neatly with mod subscriptions in the Steam Workshop.

## Table of contents

* [Installing](#installing)
    * [From package](#from-package)
    * [Building from source](#building-from-source)
      * [Arma 3](#build-process-for-arma-3)
      * [DayZ](#build-process-for-dayz)
* [Launch parameters](#launch-parameters)
* [Sceenshots](#screenshots)
* [DayZ Installation Tips](#dayz-installation)

## Installing
### From package

There are GNU/Linux and MacOS packages available in the [releases tab](https://github.com/muttleyxd/arma3-unix-launcher/releases)!

The following package types are available:
- `pkg.tar.xz` (for Arch-based distributions) (arma3-unix-launcher)
- `deb` for Ubuntu 16.04 and 18.04 (arma3-unix-launcher)
- `dmg` for MacOS (arma3-unix-launcher)
- `AppImage` for any GNU/Linux distribution running `glibc 2.23` or newer (both arma3-unix-launcher and dayz-linux-launcher)

For Arch-based distributions, there is also an AUR package available (for Arma 3). It is called `arma3-unix-launcher-bin`.

    yay -S arma3-unix-launcher-bin

### Building from source

Requirements:
* GCC 8 or newer (Clang with C++17 support should work too)
* CMake 3.11
* Qt5 with SVG support, version 5.9 or later
* fmt (optional)

#### Debian-based (Debian, Ubuntu)
    apt install cmake qt5-default libqt5widgets5 libqt5svg5 libqt5svg5-dev libfmt-dev libspdlog-dev

#### Arch-based (Arch Linux, Manjaro)
    pacman -S cmake fmt nlohmann-json pugixml qt5-base qt5-svg spdlog

#### Fedora
    dnf install gcc gcc-c++ cmake libcurl-devel qt5-qtbase qt5-qtsvg qt5-qtbase-devel qt5-qtsvg-devel spdlog-devel

#### MacOS
    brew install gcc cmake qt5

#### Build process for Arma 3
    git clone https://github.com/muttleyxd/arma3-unix-launcher.git
    cd arma3-unix-launcher
    mkdir build
    cd build
    cmake ..
    make

Launch with

    ./src/arma3-unix-launcher/arma3-unix-launcher

You may wish to add a shell alias to .bashrc or similar, such as:

    alias arma="$HOME/arma3-unix-launcher/build/src/arma3-unix-launcher/arma3-unix-launcher"

Then, you simply would type `arma` in order to open the launcher.

#### Build process for DayZ

    git clone https://github.com/muttleyxd/arma3-unix-launcher.git
    cd arma3-unix-launcher
    mkdir build
    cd build
    cmake .. -DBUILD_DAYZ_LAUNCHER=ON
    make

Launch with

    ./src/dayz-linux-launcher/dayz-linux-launcher
    
You may wish to add a shell alias to .bashrc or similar, such as:

    alias dayz="$HOME/arma3-unix-launcher/build/src/dayz-linux-launcher/dayz-linux-launcher"

Then, you simply would type `dayz` in order to open the launcher.

### Launch parameters

```shell
Usage: arma3-unix-launcher [options] 

Optional arguments:
-h --help          	show this help message and exit
-l --list-presets  	list available mod presets
-p --preset-to-run 	preset to run, launcher will start Arma with given mods and exit
--server-ip        	server ip to connect to, usable only with --preset-to-run
--server-port      	server port to connect to, usable only with --preset-to-run
--server-password  	server pasword to connect to, usable only with --preset-to-run
-v --verbose       	verbose mode which enables more logging
```

Example:
```shell
arma3-unix-launcher --preset-to-run testmod --server-ip 127.0.0.1 --server-port 1234 --server-password asdasd
```

## Screenshots

<img src="https://i.imgur.com/t2HXjY5.png" width="400"><img src="https://i.imgur.com/sAetuqr.png" width="400">

## Arma 3 Troubleshooting

### Launch Issues
When directly launching the game if you experience issues getting the game to start, such as a VDF error, a temporary solution can be to start the launcher with the `-d` or `--disable-steam-integration` flag.

### Proton Custom Version (Compatability Tool Not Found)
If using a custom verison of proton, like Glorious Eggroll or a source build, sometimes the launcher cannot find the implementation if its directory is different from what is written in the `config.vdf`. For example, if you're using Arch Linux's `proton-ge-custom-bin` it may install under `/usr/share/steam/compatibilitytools.d/proton-ge-custom`, but the CompatToolMapping for Arma in your `.steam/steam/config/config.vdf` may read as `Proton-GE`. To fix this, create a soft link to the actual installation, using the value in config.vdf as a link.

If your custom tool is tn the user folder, assuming your custom build is in `proton-ge-custom/` but your config.vdf is pointing to `Proton-GE`:
```shell
ln -s [your/steam/path]/compatibilitytools.d/proton-ge-custom [your/steam/path]/compatibilitytools.d/Proton-GE
```

If your custom tool is in the system folder,assuming your custom build is in `proton-ge-custom/` but your config.vdf is pointing to `Proton-GE`:
```shell
sudo ln -s /usr/share/steam/compatibilitytools.d/proton-ge-custom /usr/share/steam/compatibilitytools.d/Proton-GE
```

## DayZ Installation

Before trying to run DayZ via Steam Proton, be sure to increase the max_map_count:

    sudo sysctl -w vm.max_map_count=1048576 

1) Install the game with "Force the use of a specific Steam Play compatibility tool" checked and set to "Proton Experimental."

2) Install "Proton Battleye Runtime."

3) Install the arma3-unix-launcher with the new DayZ SA option ([here](#build-process-for-dayz)).

4) In the DayZ Unix Launcher, go to Parameters. Check "Skip intro" and "Skip logos at startup." 

5) Join a modded server in this way. First, subscribe to the required mods in the Steam Workshop. Second, if possible, ask the server administrator for an .html list that you can simply "open" to check the exact required set of mods at once in muttley's launcher. Third, directly connect to the server via muttley's launcher by entering the IP address, port, and password in Parameters -> Client. Fourth, click "Start."

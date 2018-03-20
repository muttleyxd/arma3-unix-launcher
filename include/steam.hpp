#ifndef __STEAM_HPP
#define __STEAM_HPP

#include <string>
#include <vector>

class Steam
{
    public:
        Steam(std::vector<std::string> search_paths = {"$HOME/.local/share/Steam", "$HOME/.steam"});

        std::vector<std::string> GetInstallPaths();
};

#endif

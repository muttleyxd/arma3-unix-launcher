#ifndef __ARMA3Client_HPP
#define __ARMA3Client_HPP

#include <string>
#include <vector>

class ARMA3Client
{
    public:
        ARMA3Client(std::string path);

        const std::string &GetSteamPath() noexcept;
        std::vector<std::string> GetInstallPaths();
        std::string GetWorkshopPath(std::string appid);

    private:
        std::string steam_path_;
        std::string config_path_ = "/config/config.vdf";
};

#endif

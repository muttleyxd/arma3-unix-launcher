#include "arma_path_chooser_dialog.h"
#include "mainwindow.h"
#include <QApplication>

#include <argparse.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>

#include "arma3client.hpp"
#include "filesystem_utils.hpp"
#include "steam.hpp"
#include "std_utils.hpp"

#include "exceptions/file_not_found.hpp"

#include "settings.hpp"

std::string read_argument(std::string const &argument_name, argparse::ArgumentParser &parser)
{
    try
    {
        return parser.get<std::string>(argument_name);
    }
    catch (std::exception const &)
    {
        return "";
    }
}

void start_arma(std::filesystem::path const &preset_to_run, std::string const &arguments,
                std::filesystem::path const &config_file, ARMA3::Client &client)
{
    auto get_valid_path = [](std::vector<std::filesystem::path> const & paths)
    {
        for (auto const &path : paths)
            if (FilesystemUtils::Exists(path))
                return std::filesystem::path(path);
        return std::filesystem::path();
    };

    std::filesystem::path const config_dir = config_file.parent_path();
    std::filesystem::path const filename_with_extension = preset_to_run.string() + ".a3ulml";
    auto valid_path = get_valid_path({preset_to_run, config_dir / preset_to_run, config_dir / filename_with_extension});
    if (valid_path.empty())
        throw FileNotFoundException(preset_to_run);

    nlohmann::json preset = nlohmann::json::parse(StdUtils::FileReadAllText(valid_path));
    auto const &mods = preset.at("mods");

    std::vector<std::filesystem::path> custom_mods;
    std::vector<std::string> workshop_mods;

    for (auto const &mod : mods["custom"])
        if (mod["enabled"])
            custom_mods.push_back(StringUtils::Replace(mod["path"], "~arma", client.GetPath()));
    for (auto const &mod : mods["workshop"])
        workshop_mods.push_back(mod["id"]);

    fmt::print("Starting Arma with preset {}\n", preset_to_run);
    client.CreateArmaCfg(workshop_mods, custom_mods);
    client.Start(arguments);
}

int main(int argc, char *argv[])
{
    try
    {
        QApplication a(argc, argv);
        argparse::ArgumentParser parser("arma3-unix-launcher");

        parser.add_argument("-l", "--list-presets").help("list available mod presets").default_value(false).implicit_value(
            true);
        parser.add_argument("-p",
                            "--preset-to-run").help("preset to run, launcher will start Arma with given mods and exit").nargs(1);
        parser.add_argument("--server-ip").help("server ip to connect to, usable only with --preset-to-run").nargs(1);
        parser.add_argument("--server-port").help("server port to connect to, usable only with --preset-to-run").nargs(1);
        parser.add_argument("--server-password").help("server pasword to connect to, usable only with --preset-to-run").nargs(
            1);
        parser.add_argument("-v", "--verbose").help("verbose mode which enables more logging").default_value(
            false).implicit_value(true);
        try
        {
            parser.parse_args(argc, argv);
        }
        catch (const std::runtime_error &err)
        {
            fmt::print("{}\n{}\n", err.what(), parser.help().str());
            return 1;
        }

        auto config_file = StdUtils::GetConfigFilePath("launcher.conf");

        if (parser.get<bool>("--list-presets"))
        {
            fmt::print("Available presets:\n");
            for (auto const &file : FilesystemUtils::Ls(config_file.parent_path()))
            {
                if (file == "launcher.conf")
                    continue;
                std::filesystem::path file_path(file);
                fmt::print("{}\n", file_path.stem().string());
            }
            return 0;
        }

        fmt::print("conf file: {}\n", config_file);

        Settings manager(config_file);

        std::string arma_path, workshop_path;

        try
        {
            arma_path = manager.settings.at("paths").at("arma");
            workshop_path = manager.settings.at("paths").at("workshop");
        }
        catch (std::exception const &ex)
        {
            fmt::print("cannot read config file: {}\n", ex.what());
        }

        std::unique_ptr<ARMA3::Client> client;
        if (std::filesystem::exists(arma_path))
            client = std::make_unique<ARMA3::Client>(arma_path, workshop_path);

        if (arma_path.empty() || !std::filesystem::exists(arma_path))
        {
            Steam steam;

            for (auto const &path : steam.GetInstallPaths())
            {
                try
                {
                    fmt::print("Install path: {}\n", path);
                    arma_path = steam.GetGamePathFromInstallPath(path, ARMA3::Definitions::app_id);
                    workshop_path = steam.GetWorkshopPath(path, ARMA3::Definitions::app_id);
                    client = std::make_unique<ARMA3::Client>(arma_path, workshop_path);
                    break;
                }
                catch (std::exception const &e)
                {
                    fmt::print("Didn't find game at {}\nError: {}\n", path, e.what());
                }
            }

            if (arma_path.empty() || workshop_path.empty() || client == nullptr)
            {
                ArmaPathChooserDialog apcd;
                apcd.exec();

                if (apcd.result() != QDialog::Accepted)
                    exit(0);

                fmt::print("Arma3: {}\nWorkshop: {}\n", apcd.arma_path_, apcd.workshop_path_);
                arma_path = apcd.arma_path_;
                workshop_path = apcd.workshop_path_;
                client = std::make_unique<ARMA3::Client>(arma_path, workshop_path);
            }
        }

        manager.settings["paths"]["arma"] = arma_path;
        manager.settings["paths"]["workshop"] = workshop_path;
        manager.save_settings_to_disk();

        auto preset_to_run = read_argument("--preset-to-run", parser);
        if (!preset_to_run.empty())
        {
            std::string arguments = manager.get_launch_parameters();

            auto ip = read_argument("--server-ip", parser);
            auto port = read_argument("--server-port", parser);
            auto password = read_argument("--server-password", parser);
            if (!ip.empty())
                arguments += fmt::format(" -connect={}", ip);
            if (!port.empty())
                arguments += fmt::format(" -port={}", port);
            if (!password.empty())
                arguments += fmt::format(" -password={}", password);

            start_arma(preset_to_run, arguments, config_file, *client);
            return 0;
        }

        MainWindow w(std::move(client), config_file);
        w.show();

        return a.exec();
    }
    catch (std::exception const &ex)
    {
        fmt::print("exception: {}\nshutting down\n", ex.what());
        return 1;
    }
    catch (...)
    {
        fmt::print("unknown exception\n");
        return 1;
    }
}

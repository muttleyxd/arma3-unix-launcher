#ifndef DOCTEST_CONFIG_DISABLE
    #define DOCTEST_CONFIG_IMPLEMENT
    #include "doctest.h"
#endif

#include "arma_path_chooser_dialog.h"
#include "mainwindow.hpp"
#include <QApplication>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include "arma3client.hpp"
#include "steam.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Steam steam;
    std::unique_ptr<ARMA3::Client> client;

    /*for (auto const &path : steam.GetInstallPaths())
    {
        try
        {
            fmt::print("Install path: {}\n", path);
            client = std::make_unique<ARMA3::Client>(steam.GetGamePathFromInstallPath(path, ARMA3::Definitions::app_id),
                     steam.GetWorkshopPath(path, ARMA3::Definitions::app_id));
        }
        catch (std::exception &e)
        {
            fmt::print("Didn't find game at {}\nError: {}\n", path, e.what());
        }
        if (client != nullptr)
            break;
    }*/

    if (client == nullptr)
    {
        ArmaPathChooserDialog apcd;
        apcd.exec();



        fmt::print("{}\n", apcd.result());
    }

    return 0;


    MainWindow w;
    w.show();

    return a.exec();
}

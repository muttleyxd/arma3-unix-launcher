#include "workshop_page.h"

#include "workshop_links.hpp"

bool open_workshop_page(std::string const &workshop_id, std::function<bool(QUrl const &)> const &open_url)
{
    auto const steam_url = WorkshopLinks::SteamClientUrl(workshop_id);
    auto const browser_url = WorkshopLinks::BrowserUrl(workshop_id);
    if (!steam_url || !browser_url)
        return false;

    // openUrl fails when no application is registered for steam://, i.e. Steam is not installed
    if (open_url(QUrl(QString::fromStdString(*steam_url))))
        return true;
    return open_url(QUrl(QString::fromStdString(*browser_url)));
}

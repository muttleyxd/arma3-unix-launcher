#pragma once

#include <functional>
#include <string>

#include <QDesktopServices>
#include <QUrl>

// Opens a Workshop item's page in the Steam client, or in the browser if nothing handles steam:// URLs.
// Returns false if workshop_id is invalid or no page could be opened.
bool open_workshop_page(std::string const &workshop_id,
                        std::function<bool(QUrl const &)> const &open_url = QDesktopServices::openUrl);

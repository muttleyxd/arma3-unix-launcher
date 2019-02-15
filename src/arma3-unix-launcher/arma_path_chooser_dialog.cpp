#include "arma_path_chooser_dialog.h"
#include "ui_arma_path_chooser_dialog.h"

#include <fmt/format.h>
#include <filesystem>

#include <QFile>

#include "exceptions/file_not_found.hpp"

#include "arma3client.hpp"

ArmaPathChooserDialog::ArmaPathChooserDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ArmaPathChooserDialog)
{
    ui->setupUi(this);
    ui->horizontalLayout_2->setAlignment(Qt::AlignTop);
    ui->horizontalLayout->setAlignment(Qt::AlignBottom);
    ui->verticalLayout->setAlignment(Qt::AlignTop);

    this->setMaximumSize(1024000, 1);
    this->setMinimumSize(450, 106);

    QIcon icon_ok = QIcon(":/icons/open-iconic/check.svg");
    pixmap_ok = icon_ok.pixmap(QSize(16, 16));

    QIcon icon_error = QIcon(":/icons/open-iconic/x.svg");
    pixmap_error = icon_error.pixmap(QSize(16, 16));

    on_text_arma_path_textChanged("/invalid-path");

}

ArmaPathChooserDialog::~ArmaPathChooserDialog()
{
    delete ui;
}

void ArmaPathChooserDialog::on_text_arma_path_textChanged(QString const &arg1)
{
    if (!is_arma_path_valid(arg1))
    {
        ui->icon_arma_path->setPixmap(pixmap_error);
        return;
    }

    ui->icon_arma_path->setPixmap(pixmap_ok);
    QString workshop_path = ui->text_arma_path->text() + "/../../workshop/content/107410";
    std::filesystem::path canonical_path = std::filesystem::canonical(workshop_path.toStdString());
    ui->text_workshop_path->setText(QString::fromStdString(canonical_path.string()));
}

void ArmaPathChooserDialog::on_text_workshop_path_textChanged(QString const &arg1)
{
    if (!is_workshop_path_valid(arg1))
    {
        ui->icon_arma_path->setPixmap(pixmap_error);
        return;
    }

    ui->icon_workshop_path->setPixmap(pixmap_ok);
}

bool ArmaPathChooserDialog::is_arma_path_valid(QString const &arg1)
{
    try
    {
        ARMA3::Client client(arg1.toStdString(), "", true);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool ArmaPathChooserDialog::is_workshop_path_valid(QString const &arg1)
{
    using namespace std::filesystem;
    std::error_code ec;
    return is_directory(arg1.toStdString(), ec);
}

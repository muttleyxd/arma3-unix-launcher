#include "arma_path_chooser_dialog.h"
#include "ui_arma_path_chooser_dialog.h"

#include <fmt/ostream.h>
#include <fmt/format.h>
#include <filesystem>

#include <QFile>
#include <QFileDialog>

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
    on_text_workshop_path_textChanged("/invalid-path");
}

ArmaPathChooserDialog::~ArmaPathChooserDialog()
{
    delete ui;
}

void ArmaPathChooserDialog::on_text_arma_path_textChanged(QString const &arg1)
{
    if (!is_arma_path_valid(arg1))
    {
        ui->buttonBox->button(ui->buttonBox->Ok)->setEnabled(false);
        ui->icon_arma_path->setPixmap(pixmap_error);
        return;
    }

    ui->buttonBox->button(ui->buttonBox->Ok)->setEnabled(true);
    ui->icon_arma_path->setPixmap(pixmap_ok);
    QString workshop_path = ui->text_arma_path->text() + "/../../workshop/content/107410";
    try
    {
        std::filesystem::path canonical_path = std::filesystem::canonical(workshop_path.toStdString());
        ui->text_workshop_path->setText(QString::fromStdString(canonical_path.string()));
    }
    catch (std::filesystem::filesystem_error &exception)
    {
        fmt::print("{}\n", exception.what());
    }
}

void ArmaPathChooserDialog::on_text_workshop_path_textChanged(QString const &arg1)
{
    if (!is_workshop_path_valid(arg1))
        ui->icon_workshop_path->setPixmap(pixmap_error);
    else
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

void ArmaPathChooserDialog::on_button_browse_arma_path_clicked()
{
    using namespace std::filesystem;

    std::string format_str = fmt::format("Arma 3 Executable ({})", ARMA3::Definitions::executable_name);

    QFileDialog open_executable_dialog;
    open_executable_dialog.setFilter(QDir::AllDirs | QDir::AllEntries | QDir::Hidden | QDir::System | QDir::NoDotAndDotDot);
    open_executable_dialog.setWindowTitle("Select ArmA 3 executable");
    open_executable_dialog.setFileMode(QFileDialog::ExistingFile);
    open_executable_dialog.setViewMode(QFileDialog::Detail);
    open_executable_dialog.setNameFilter(format_str.c_str());
    open_executable_dialog.setDirectory("/home/mszychow");
    int result = open_executable_dialog.exec();

    if (!result)
        return;
    auto executable_path = open_executable_dialog.selectedFiles()[0];
    ui->text_arma_path->setText(path(executable_path.toStdString()).parent_path().c_str());
}

void ArmaPathChooserDialog::on_button_browse_workshop_path_clicked()
{

}

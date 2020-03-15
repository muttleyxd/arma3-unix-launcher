#include "arma_path_chooser_dialog.h"
#include "ui_arma_path_chooser_dialog.h"

#include <fmt/ostream.h>
#include <fmt/format.h>
#include <filesystem>
#include <utility>

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

    button_ok_ = ui->buttonBox->button(ui->buttonBox->Ok);

    QIcon icon_ok = QIcon(":/icons/open-iconic/check.svg");
    pixmap_ok_ = icon_ok.pixmap(QSize(16, 16));

    QIcon icon_error = QIcon(":/icons/open-iconic/x.svg");
    pixmap_error_ = icon_error.pixmap(QSize(16, 16));

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
        button_ok_->setEnabled(false);
        ui->icon_arma_path->setPixmap(pixmap_error_);
        return;
    }

    button_ok_->setEnabled(true);
    ui->icon_arma_path->setPixmap(pixmap_ok_);
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
        ui->icon_workshop_path->setPixmap(pixmap_error_);
    else
        ui->icon_workshop_path->setPixmap(pixmap_ok_);
}

void ArmaPathChooserDialog::on_button_browse_arma_path_clicked()
{
    using namespace std::filesystem;

    auto open_executable_dialog = get_open_dialog("Select ArmA 3 executable");
    open_executable_dialog->setFileMode(QFileDialog::ExistingFile);
    int result = open_executable_dialog->exec();
    if (!result)
        return;

    auto executable_path = open_executable_dialog->selectedFiles()[0];
    path parent_path = path(executable_path.toStdString()).parent_path();
    ui->text_arma_path->setText(parent_path.c_str());
}

void ArmaPathChooserDialog::on_button_browse_workshop_path_clicked()
{
    auto open_dir_dialog = get_open_dialog("Select ArmA 3 workshop path");
    open_dir_dialog->setOption(QFileDialog::ShowDirsOnly);
    int result = open_dir_dialog->exec();
    if (!result)
        return;

    auto workshop_dir = open_dir_dialog->selectedFiles()[0];
    ui->text_workshop_path->setText(workshop_dir);
}

std::unique_ptr<QFileDialog> ArmaPathChooserDialog::get_open_dialog(QString const &title)
{
    auto dialog = std::make_unique<QFileDialog>();
    dialog->setFilter(QDir::AllDirs | QDir::AllEntries | QDir::Hidden | QDir::System | QDir::NoDotAndDotDot);
    dialog->setWindowTitle(title);
    dialog->setViewMode(QFileDialog::Detail);
    dialog->setOption(QFileDialog::DontResolveSymlinks, false);
    dialog->setDirectory(QDir::homePath());
    return dialog;
}

bool ArmaPathChooserDialog::is_arma_path_valid(QString const &arg1)
{
    try
    {
        ARMA3::Client client(arg1.toStdString(), "");
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

void ArmaPathChooserDialog::on_buttonBox_accepted()
{
    arma_path_ = ui->text_arma_path->text().toStdString();
    workshop_path_ = ui->text_workshop_path->text().toStdString();
}

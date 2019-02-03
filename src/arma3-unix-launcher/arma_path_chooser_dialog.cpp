#include "arma_path_chooser_dialog.h"
#include "ui_arma_path_chooser_dialog.h"

#include <fmt/format.h>

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

    QIcon icon = QIcon::fromTheme("checkmark");
    QPixmap qpixmap = icon.pixmap(icon.actualSize(QSize(32, 32)));
    ui->label_arma_path->setPixmap(qpixmap);
}

ArmaPathChooserDialog::~ArmaPathChooserDialog()
{
    delete ui;
}

void ArmaPathChooserDialog::on_text_arma_path_textChanged(const QString &arg1)
{
    try {
        fmt::print("Trying client at {}\n", arg1.toStdString());
        ARMA3::Client client(arg1.toStdString(), "", true);
        fmt::print("Works\n");
        ui->button_browse_arma_path->setIcon(QIcon::fromTheme("checkmark"));
    } catch (...) {
        fmt::print("Failed\n");
        ui->button_browse_arma_path->setIcon(QIcon::fromTheme("error"));
    }
}

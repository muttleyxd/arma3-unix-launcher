#include "arma_path_chooser_dialog.h"
#include "ui_arma_path_chooser_dialog.h"

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
}

ArmaPathChooserDialog::~ArmaPathChooserDialog()
{
    delete ui;
}

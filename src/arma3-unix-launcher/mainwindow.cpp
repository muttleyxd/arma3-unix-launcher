#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QMessageBox::warning(nullptr, "Clicked", "yay");
}

void MainWindow::on_pushButton_released()
{
    QMessageBox::warning(nullptr, "released", "yay");
}

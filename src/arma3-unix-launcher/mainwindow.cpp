#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <Qt>

#include <QCheckBox>
#include <QMessageBox>
#include <QTabBar>

#include <QResizeEvent>

#include <fmt/format.h>
#include <fmt/ostream.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    init_table();

    //table->item(0, 0)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::set_client(std::unique_ptr<ARMA3::Client> client)
{
    client_ = std::move(client);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    /*
     * Weird behavior regarding font sizes
    int size = (ui->centralWidget->size().width() / 2) - 16;
    QString stylesheet = QString("QTabBar::tab { min-width: %1px; }").arg(size);
    fmt::print("{}\n", ui->tabWidget->styleSheet().toStdString());
    ui->tabWidget->setStyleSheet(stylesheet);
    */
    QMainWindow::resizeEvent(event);
}

void MainWindow::init_table()
{
    /*if (ui->tableWidget->supportedDropActions() == Qt::LinkAction)
    {
        fmt::print("Hi\n");
    }*/
    ui->tableWidget->setRowCount(5);
    return;

    auto table = ui->tableWidget;

    //table->setHorizontalHeaderLabels({"Enabled", "Name", "Workshop ID"});
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    table->setColumnWidth(0, 64);
    table->setColumnWidth(2, 128);

    //table->setSelectionMode(QAbstractItemView::SingleSelection);
    /*table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setDragEnabled(true);
    table->setDragDropMode(QAbstractItemView::InternalMove);
    table->setDragDropOverwriteMode(true);
    table->setDefaultDropAction(Qt::MoveAction);
    table->setDropIndicatorShown(true);
    table->setAcceptDrops(true);
    table->horizontalHeader()->setSectionsMovable(true);
    table->viewport()->setAcceptDrops(true);*/

    auto add_entry = [&](bool enabled, QString const & name, QString const & workshop_id)
    {
        QCheckBox *checkbox = new QCheckBox();
        checkbox->setCheckState(enabled ? Qt::Checked : Qt::Unchecked);
        checkbox->setCheckable(true);
        checkbox->setStyleSheet("margin-left: 16px;");

        QLabel *lb_name = new QLabel();
        lb_name->setText(name);
        lb_name->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        QLabel *lb_workshop_id = new QLabel();
        lb_workshop_id->setText(workshop_id);
        lb_workshop_id->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

        table->insertRow(table->rowCount());
        int index = table->rowCount() - 1;
        table->setCellWidget(index, 0, checkbox);
        table->setCellWidget(index, 1, lb_name);
        table->setCellWidget(index, 2, lb_workshop_id);
    };

    add_entry(true, "Some mod", "463969091");
    add_entry(false, "Some mod two", "463969095");
    add_entry(false, "Some mod three", "463969099");
}

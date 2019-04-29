#ifndef MAINWINDOW_HPP_
#define MAINWINDOW_HPP_

#include <QMainWindow>

#include <arma3client.hpp>

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
        Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

        void set_client(std::unique_ptr<ARMA3::Client> client);

        void resizeEvent(QResizeEvent *event) override;

    private slots:

    private:
        Ui::MainWindow *ui;

        std::unique_ptr<ARMA3::Client> client_;

        void init_table();
};

#endif // MAINWINDOW_HPP_

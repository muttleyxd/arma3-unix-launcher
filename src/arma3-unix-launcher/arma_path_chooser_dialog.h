#ifndef ARMA_PATH_CHOOSER_DIALOG_H
#define ARMA_PATH_CHOOSER_DIALOG_H

#include <QDialog>

namespace Ui
{
    class ArmaPathChooserDialog;
}

class ArmaPathChooserDialog : public QDialog
{
        Q_OBJECT

    public:
        explicit ArmaPathChooserDialog(QWidget *parent = nullptr);
        ~ArmaPathChooserDialog();

    private slots:
        void on_text_arma_path_textChanged(QString const &arg1);
        void on_text_workshop_path_textChanged(QString const &arg1);

    private:
        Ui::ArmaPathChooserDialog *ui;
        QPixmap pixmap_error;
        QPixmap pixmap_ok;

        bool is_arma_path_valid(QString const &arg1);
        bool is_workshop_path_valid(QString const &arg1);
};

#endif // ARMA_PATH_CHOOSER_DIALOG_H

#ifndef ARMA_PATH_CHOOSER_DIALOG_H
#define ARMA_PATH_CHOOSER_DIALOG_H

#include <QDialog>

namespace Ui {
class ArmaPathChooserDialog;
}

class ArmaPathChooserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ArmaPathChooserDialog(QWidget *parent = nullptr);
    ~ArmaPathChooserDialog();

private slots:
    void on_text_arma_path_textChanged(const QString &arg1);

private:
    Ui::ArmaPathChooserDialog *ui;
};

#endif // ARMA_PATH_CHOOSER_DIALOG_H

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

private:
    Ui::ArmaPathChooserDialog *ui;
};

#endif // ARMA_PATH_CHOOSER_DIALOG_H

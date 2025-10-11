#ifndef ABOUTKEYSDIALOG_H
#define ABOUTKEYSDIALOG_H

#include <QDialog>

namespace Ui {
class AboutKeysDialog;
}

class AboutKeysDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutKeysDialog(QWidget *parent = nullptr);
    ~AboutKeysDialog();

private slots:
    void on_pushButtonClose_clicked();

private:
    Ui::AboutKeysDialog *ui;
};

#endif // ABOUTKEYSDIALOG_H

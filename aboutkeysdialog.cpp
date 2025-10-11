#include "aboutkeysdialog.h"
#include "ui_aboutkeysdialog.h"

AboutKeysDialog::AboutKeysDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AboutKeysDialog)
{
    ui->setupUi(this);
}

AboutKeysDialog::~AboutKeysDialog()
{
    delete ui;
}

void AboutKeysDialog::on_pushButtonClose_clicked()
{
    close();
}


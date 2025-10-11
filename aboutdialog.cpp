#include "aboutdialog.h"
#include "ui_aboutdialog.h"

#include "defsui.h"

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    QString m_strAppVersion = APP_NAME_VERSION_1 + QString(NEXTBLOCK_APP_FULL_NAME) + APP_NAME_VERSION_2;

    ui->labelTitle1->setText(m_strAppVersion);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::on_pushButtonClose_clicked()
{
    close();
}


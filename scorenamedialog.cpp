#include "scorenamedialog.h"
#include "ui_scorenamedialog.h"

ScoreNameDialog::ScoreNameDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ScoreNameDialog)
{
    ui->setupUi(this);

    ui->lineEdit->setFocus();
    ui->lineEdit->setText("player");
}

ScoreNameDialog::~ScoreNameDialog()
{
    delete ui;
}

QString ScoreNameDialog::getPlayerName() const
{
    return ui->lineEdit->text();
}

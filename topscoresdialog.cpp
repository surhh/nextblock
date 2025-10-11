#include "topscoresdialog.h"
#include "ui_topscoresdialog.h"

TopScoresDialog::TopScoresDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TopScoresDialog)
{
    ui->setupUi(this);

    ui->tableWidget->setColumnWidth(0, 300);
    ui->tableWidget->setColumnWidth(1, 160);
    ui->tableWidget->setColumnWidth(2, 110);
    ui->tableWidget->setColumnWidth(3, 110);
    ui->tableWidget->setColumnWidth(4, 110);
    ui->tableWidget->setColumnWidth(5, 110);
}

TopScoresDialog::~TopScoresDialog()
{
    delete ui;
}

void TopScoresDialog::on_okButton_clicked()
{
    close();
}

void TopScoresDialog::setPlayersData(const std::vector<Player>& a_data)
{
    int id = 1;

    for (std::vector<Player>::const_iterator it = a_data.begin(); it < a_data.end(); ++it, ++id)
    {
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());

        int columnCount = ui->tableWidget->columnCount();

        QTableWidgetItem* items[columnCount];

        items[0] = new QTableWidgetItem(it->name);
        items[1] = new QTableWidgetItem(QString::number(it->stat.score));
        items[2] = new QTableWidgetItem(QString::number(it->stat.blocks));
        items[3] = new QTableWidgetItem(QString::number(it->stat.delLines));
        items[4] = new QTableWidgetItem(QString::number(it->stat.eff, 'f', 2));
        items[5] = new QTableWidgetItem(QString::number(it->stat.scoreblock, 'f', 2));

        for (int j = 0; j < columnCount; ++j)
        {
            items[j]->setFlags(items[j]->flags() & ~Qt::ItemIsEditable);
            ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, j, items[j]);
        }                
    }
}

void TopScoresDialog::highlightCurrentPlayer(int a_index)
{
    int size = ui->tableWidget->rowCount();
    int columnCount = ui->tableWidget->columnCount();

    for (int i = 0; i < columnCount; ++i)
    {
        ui->tableWidget->item(a_index - 1, i)->setBackground(Qt::green);
    }
}

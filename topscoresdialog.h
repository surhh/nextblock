#ifndef TOPSCORESDIALOG_H
#define TOPSCORESDIALOG_H

#include <QDialog>

#include "scorefile.h"

namespace Ui
{
class TopScoresDialog;
}

class TopScoresDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TopScoresDialog(QWidget *parent = nullptr);
    ~TopScoresDialog();

    void setPlayersData(const std::vector<Player>& a_data);
    void highlightCurrentPlayer(int a_index); /// indexes start from 1

private slots:
    void on_okButton_clicked();

private:
    Ui::TopScoresDialog *ui;
};

#endif // TOPSCORESDIALOG_H

#ifndef SCORENAMEDIALOG_H
#define SCORENAMEDIALOG_H

#include <QDialog>

namespace Ui
{
class ScoreNameDialog;
}

class ScoreNameDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ScoreNameDialog(QWidget *parent = nullptr);
    ~ScoreNameDialog();

    QString getPlayerName() const;

private:
    Ui::ScoreNameDialog *ui;
};

#endif // SCORENAMEDIALOG_H

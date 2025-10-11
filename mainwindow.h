#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts/QLineSeries>
#include <QChart>
#include <QChartView>
#include <QValueAxis>

#include "fieldpainter.h"
#include "nextblockpainter.h"
#include "scorefile.h"
#include "engine/gamefieldthread.h"
#include "updatemanager/filedownloader.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void updateFieldsSignal(const FieldMatrix& a_matrix, const Element& a_element);

private slots:
    void on_startButton_clicked();
    void keyPressEvent(QKeyEvent *a_event);
    void threadFinished();


    void on_checkBoxPreview_stateChanged(int arg1);

    void on_checkBoxRotate_stateChanged(int arg1);

    void on_checkBoxRotateStart_stateChanged(int arg1);


    void updateFieldsSlot(const FieldMatrix& a_matrix, const Element& a_element);

    void updateGameStatSlot(const GameStat& a_stat);


    void on_spinBoxStartSpeed_valueChanged(int arg1);

    void on_actionNew_triggered();

    void on_actionQuit_triggered();

    void on_actionHelpKeys_triggered();

    void on_actionAbout_triggered();

    void on_actionCheckForUpdate_triggered();

    qint32 updateChecked();

    void on_actionHighScores_triggered();

private:
    void checkForUpdate();

private:
    Ui::MainWindow *ui;

    FieldPainter*       m_gameFieldPainter;
    NextBlockPainter*   m_nextBlockPainter;

    GameFieldThread*    m_thread;

    Player              m_player;

    QThread*            m_workerThread;

    ScoreFile           m_scoreFile;
    std::vector<Player> m_scoreData;

    QLineSeries*        m_lineSeries;
    QChart*             m_effChart;
    QChartView*         m_effChartView;
    QValueAxis*         m_axisX;
    QValueAxis*         m_axisY;

    FileDownloader     *m_fileDownloader;
};
#endif // MAINWINDOW_H

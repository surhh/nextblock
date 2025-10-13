#include "mainwindow.h"
#include "./ui_mainwindow.h"


#include "engine/helper.h"
#include "scorenamedialog.h"
#include "topscoresdialog.h"
#include "aboutdialog.h"
#include "aboutkeysdialog.h"
#include "defsui.h"

#include <QKeyEvent>
#include <QMessageBox>
#include <QDesktopServices>

#include <iostream>

/*
#define KEY_LEFT        (Qt::Key_J)
#define KEY_RIGHT       (Qt::Key_L)
#define KEY_DOWN        (Qt::Key_Q)
#define KEY_ROTATE      (Qt::Key_K)
#define KEY_DROP        (Qt::Key_A)
/*/
#define KEY_LEFT        (Qt::Key_Left)
#define KEY_RIGHT       (Qt::Key_Right)
#define KEY_DOWN        (Qt::Key_Down)
#define KEY_ROTATE      (Qt::Key_Up)
#define KEY_DROP        (Qt::Key_Space)
//*/

#define KEY_PAUSE       (Qt::Key_P)

#define SCORE_FILE_PATH     "nextblock.scores"

#define UPDATE_CHECK_URL        "https://raw.githubusercontent.com/surhh/nextblock/main/_lastversion.txt"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initRandom();

    m_scoreFile.init();
    m_scoreFile.setFileName(SCORE_FILE_PATH);
    m_scoreFile.loadFile();

    m_gameFieldPainter = new FieldPainter();
    m_nextBlockPainter = new NextBlockPainter();
    m_thread = new GameFieldThread();
    m_thread->init();
    m_thread->setRotateDirection(ui->checkBoxRotate->isChecked());
    m_thread->setRotateOnStart(ui->checkBoxRotateStart->isChecked());
    m_thread->setStartSpeed(ui->spinBoxStartSpeed->value());


    /// adding chart
    m_lineSeries = new QLineSeries();

    m_axisX = new QValueAxis();
    m_axisY = new QValueAxis();

    m_effChart = new QChart();
    m_effChart->legend()->hide();
    m_effChart->addSeries(m_lineSeries);

    m_effChart->addAxis(m_axisX,Qt::AlignBottom);
    m_effChart->addAxis(m_axisY,Qt::AlignLeft);
    m_lineSeries->attachAxis(m_axisX);
    m_lineSeries->attachAxis(m_axisY);

    m_axisX->setTickCount(10);
    m_axisX->setRange(1, 10);
    m_axisX->setLabelFormat("%i");

    m_axisY->setTickCount(2);
    m_axisY->setRange(0, 100);
    m_axisY->setLabelFormat("%i");

    m_effChart->scroll(m_effChart->plotArea().width() / m_axisX->tickCount(), 0);

    m_effChartView = new QChartView(m_effChart);
    m_effChartView->setFocusPolicy(Qt::NoFocus);
    m_effChartView->setRenderHint(QPainter::Antialiasing);
    m_effChartView->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    /// end of adding chart

    ui->horizontalLayout_1->addWidget(m_gameFieldPainter);
    ui->verticalLayout_5->addWidget(m_nextBlockPainter);
    ui->verticalLayout_7->addWidget(m_effChartView);

    ui->checkBoxPreview->setChecked(true);
    ui->checkBoxSounds->setVisible(false);


    m_gameFieldPainter->setFieldMatrix(m_thread->getField());
    m_gameFieldPainter->update();

    m_nextBlockPainter->update();

    /// meta type registration is needed for Qt::QueuedConnection type of connection
    qRegisterMetaType<const FieldMatrix&>("FieldMatrix&");
    qRegisterMetaType<const Element&>("Element&");
    //////////////////////////////////

    /// connecting signals
    connect(m_thread, SIGNAL(finished()), this, SLOT(threadFinished()));
    connect(m_thread, &GameFieldThread::updateGameStat, this, &MainWindow::updateGameStatSlot, Qt::QueuedConnection);
    connect(m_thread, &GameFieldThread::updateFieldsSignal, this, &MainWindow::updateFieldsSlot, Qt::QueuedConnection);

    //// checking for the new version
    m_fileDownloader = new FileDownloader(QUrl(UPDATE_CHECK_URL));
    if (m_fileDownloader != nullptr)
    {
        connect(m_fileDownloader, SIGNAL(downloaded()),this, SLOT(updateChecked()));
    }
}

MainWindow::~MainWindow()
{
    if (m_thread->isRunning())
    {
        m_thread->stopLoop();
        m_thread->quit();
        m_thread->wait();
    }

    m_lineSeries->clear();


    delete m_fileDownloader;

    delete m_gameFieldPainter;

    delete m_nextBlockPainter;

    delete m_thread;

    delete m_axisX;

    delete m_axisY;

    delete m_lineSeries;

    delete m_effChart;


    delete ui;
}

void MainWindow::on_startButton_clicked()
{
    ui->startButton->clearFocus();
    ui->actionNew->setEnabled(false);
    ui->actionHighScores->setEnabled(false);

    m_lineSeries->clear();
    m_axisX->setTickCount(10);
    m_axisX->setRange(1, 10);

    initRandom();

    m_scoreFile.init();
    m_scoreFile.setFileName(SCORE_FILE_PATH);

    m_thread->init();

    m_thread->setRotateDirection(ui->checkBoxRotate->isChecked());
    m_thread->setRotateOnStart(ui->checkBoxRotateStart->isChecked());
    m_thread->setStartSpeed(ui->spinBoxStartSpeed->value());

    m_thread->start();

    ui->startButton->setEnabled(false);
    ui->checkBoxRotate->setEnabled(false);
    ui->checkBoxRotateStart->setEnabled(false);
    ui->spinBoxStartSpeed->setEnabled(false);
}

void MainWindow::keyPressEvent(QKeyEvent *a_event)
{
    //if(a_event->key() != Qt::Key_Escape)
    //    QDialog::keyPressEvent(a_event);

    int key = a_event->key();

    bool threadRunning = m_thread->isRunning();

    switch (key)
    {
        case KEY_DROP:
        if (threadRunning && m_thread->getGameProgressStatus())
            {
                emit m_thread->dropElementSignal();
            }
            break;
        case KEY_LEFT:
            if (threadRunning && !m_thread->getDroppedStatus() && m_thread->getGameProgressStatus())
            {
                emit m_thread->moveElementSignal(0); /// 0 - means to the left
            }
            break;
        case KEY_ROTATE:
            if (threadRunning && !m_thread->getDroppedStatus() && m_thread->getGameProgressStatus())
            {
                emit m_thread->rotateElementSignal();
            }
            break;
        case KEY_RIGHT:
            if (threadRunning && !m_thread->getDroppedStatus() && m_thread->getGameProgressStatus())
            {
                emit m_thread->moveElementSignal(1); /// 1 - means to the right
            }
            break;
        case KEY_DOWN:
            if (threadRunning)
            {
                emit m_thread->moveElementDownSignal();
            }
            break;
        case KEY_PAUSE:
            if (threadRunning)
            {
                emit m_thread->pauseResumeSignal();
            }
            break;
        default:
            QMainWindow::keyPressEvent(a_event);
    }
}

void MainWindow::threadFinished()
{
    ScoreNameDialog scoreNameDlg;
    TopScoresDialog topScoreDlg;

    ui->startButton->setEnabled(true);
    ui->checkBoxRotate->setEnabled(true);
    ui->checkBoxRotateStart->setEnabled(true);
    ui->spinBoxStartSpeed->setEnabled(true);
    ui->actionNew->setEnabled(true);
    ui->actionHighScores->setEnabled(true);


    m_scoreFile.loadFile();

    quint64 minScore = m_scoreFile.getMinScore();

    qint32 index = -1;

    if (m_player.stat.score > minScore && scoreNameDlg.exec() == QDialog::Accepted)
    {
        QString name = scoreNameDlg.getPlayerName();

        if (!name.isEmpty())
        {
            ///QMessageBox::information(this, "NextBlock", name, QMessageBox::Ok);

            m_player.name = name;

            m_scoreFile.addPlayer(m_player);
            m_scoreFile.saveFile();

            m_scoreFile.loadFile();

            index = m_scoreFile.findPLayerIndex(m_player);
        }
    }

    m_scoreData = m_scoreFile.getPlayersData();

    topScoreDlg.setPlayersData(m_scoreData);

    if (index != -1)
    {
        topScoreDlg.highlightCurrentPlayer(index);
    }

    topScoreDlg.exec();
}

void MainWindow::on_checkBoxPreview_stateChanged(int arg1)
{
    if (arg1 > 0)
    {
        m_nextBlockPainter->setVisible();
    }
    else
    {
        m_nextBlockPainter->setVisible(false);
    }

    update();
}


void MainWindow::on_checkBoxRotate_stateChanged(int arg1)
{
    if (arg1 > 0)
    {
        m_thread->setRotateDirection(true);
    }
    else
    {
        m_thread->setRotateDirection();
    }
}

void MainWindow::on_checkBoxRotateStart_stateChanged(int arg1)
{
    if (arg1 > 0)
    {
        m_thread->setRotateOnStart(true);
    }
    else
    {
        m_thread->setRotateOnStart();
    }
}

void MainWindow::updateFieldsSlot(const FieldMatrix& a_matrix, const Element& a_element)
{
    m_gameFieldPainter->setFieldMatrix(a_matrix);
    m_gameFieldPainter->update();

    m_nextBlockPainter->setNextBlock(a_element);
    m_nextBlockPainter->setColor(m_gameFieldPainter->getElementColor(a_element.getParams().type));
    m_nextBlockPainter->update();

    update();
}

void MainWindow::updateGameStatSlot(const GameStat &a_stat)
{
    m_player.stat.score = a_stat.score;
    m_player.stat.blocks = a_stat.blocks;
    m_player.stat.delLines = a_stat.delLines;
    m_player.stat.deltaScore = a_stat.deltaScore;
    m_player.stat.speed = a_stat.speed;
    m_player.stat.eff = a_stat.eff;
    m_player.stat.scoreblock = a_stat.scoreblock;
    m_player.name = "";

    ui->labelScore->setText("Score: " + QString::number(a_stat.score));
    ui->labelDeltaScore->setText("Score from last block: " + QString::number(a_stat.deltaScore));
    ui->labelSpeed->setText("Speed: " + QString::number(a_stat.speed));
    ui->labelBlocks->setText("Blocks: "  + QString::number(a_stat.blocks));
    ui->labelDelLines->setText("Deleted lines: "  + QString::number(a_stat.delLines));
    ui->labelEfficiency->setText("Efficiency: " + QString::number(a_stat.eff, 'f', 2) + " %");
    ui->labelScorePerBlock->setText("Average score per block: " + QString::number(a_stat.scoreblock, 'f', 2));

    if (a_stat.blocks > 1)
    {
        m_lineSeries->append(a_stat.blocks - 1, a_stat.eff);
    }

    if (a_stat.blocks > 10)
    {
        m_effChart->scroll(m_effChart->plotArea().width() / (m_axisX->tickCount() - 1), 0);
    }
}

void MainWindow::on_spinBoxStartSpeed_valueChanged(int arg1)
{
    m_thread->setStartSpeed(arg1);
}


void MainWindow::on_actionNew_triggered()
{
    on_startButton_clicked();
}


void MainWindow::on_actionQuit_triggered()
{
    qApp->exit();
}


void MainWindow::on_actionHelpKeys_triggered()
{
    AboutKeysDialog dlg;

    dlg.exec();
}


void MainWindow::on_actionAbout_triggered()
{
    AboutDialog dlg;

    dlg.exec();
}


void MainWindow::on_actionCheckForUpdate_triggered()
{
    checkForUpdate();
}

void MainWindow::checkForUpdate()
{
    ui->actionCheckForUpdate->setEnabled(false);

    m_fileDownloader->doRequest();
}

qint32 MainWindow::updateChecked()
{
    ui->actionCheckForUpdate->setEnabled(true);

    QByteArray data = m_fileDownloader->getDownloadedData();

    int32_t newVersion = data.toInt();
    int32_t curVersion = QString(NEXTBLOCK_VERSION_INT).toInt();

    QString text = "";
    QString caption = "Update status";

    if (curVersion < newVersion)
    {
        text = "Your current version of NextBlock is " + QString(TOSTRING(NEXTBLOCK_MAJOR_VERSION)) + "." +
               QString(TOSTRING(NEXTBLOCK_MINOR_VERSION)) + ". The latest version is " +
               QString::number(newVersion / 10) + "." + QString::number(newVersion % 10) +
               " and is available for download. Do you want to upgrade to the latest version?";

        QMessageBox::StandardButton reply = QMessageBox::question(this, caption, text, QMessageBox::Yes|QMessageBox::No);

        if (reply == QMessageBox::Yes)
        {
            QDesktopServices::openUrl(QUrl(UPDATE_DOWNLOAD_URL));
        }
    }
    else
    {
        text = "You already have the latest version of NextBlock!";
        QMessageBox::information(this, caption, text, QMessageBox::Ok);
    }

    return newVersion;
}

void MainWindow::on_actionHighScores_triggered()
{
    TopScoresDialog dlg;

    m_scoreFile.loadFile();

    m_scoreData = m_scoreFile.getPlayersData();

    dlg.setPlayersData(m_scoreData);

    dlg.exec();
}


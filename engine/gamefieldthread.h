#ifndef GAMEFIELDTHREAD_H
#define GAMEFIELDTHREAD_H

#include <QThread>
#include <QMutex>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QUrl>

#include "shape.h"

#define FIELD_GAME_WIDTH            (10)
#define FIELD_GAME_HEIGHT           (20)

#define FIELD_WIDTH_MARGIN          (4)
#define FIELD_HEIGHT_MARGIN_UP      (2)
#define FIELD_HEIGHT_MARGIN         (FIELD_HEIGHT_MARGIN_UP + SIZE_SHAPE)
#define FIELD_WIDTH                 (FIELD_GAME_WIDTH + FIELD_WIDTH_MARGIN)
#define FIELD_HEIGHT                (FIELD_GAME_HEIGHT + FIELD_HEIGHT_MARGIN)

#define FIELD_START_X               (2)
#define FIELD_START_Y               (2)
#define FIELD_SHAPE_START_X         (3)
#define FIELD_SHAPE_START_Y         (-1)
#define MAX_SPEED                   (9)
#define SPEED_INCREASE_INTERVAL     (10)
#define SURFACE_LINE                (FIELD_GAME_WIDTH * 1)
#define MAX_EFFICIENCY_HISTORY      (100)

#define BASE_SPEED_DELAY            (100)
#define MINIMAL_DELAY               (10)
#define BASE_SPEED_DELAY_QUATIENT   (5)
#define SPEED_DELAY                 (20)

#define GENERATED_BLOCKS_NUMBER     (4)

///void initRnd();
uint32_t genRndNumber();

struct FieldMatrix
{
    uint8_t data[FIELD_HEIGHT][FIELD_WIDTH];
};

struct Position
{
    int8_t x;
    int8_t y;
};


struct GameStat
{
    uint32_t speed;
    uint64_t score;
    uint32_t deltaScore;
    uint32_t blocks;
    uint32_t delLines;
    float    eff;
    float    scoreblock;
};


class GameFieldThread : public QThread
{
    Q_OBJECT

private:
    FieldMatrix m_field;

    Element m_currentElement, m_nextElement;

    Element m_elements[GENERATED_BLOCKS_NUMBER];

    Position m_curElemPos;


    uint64_t m_score;
    uint32_t m_deltaScore, m_delLinesCount, m_elementCount;

    uint8_t m_speed, m_startSpeed;

    uint8_t m_curDelLinesCount;


    float m_efficiency, m_maxEfficiency;

    float m_scoreBlock;

    float m_efficiencyHistory[MAX_EFFICIENCY_HISTORY];

    uint8_t m_historyCount;


    bool m_rotateClock;

    bool m_rotateOnStart;

    bool m_dropped;

    bool m_downNext;

    volatile bool m_isPaused, m_isStarted;

    QMutex      m_mutex;

    ///QMediaPlayer *m_player;
    ///QAudioOutput *m_audioOutput;

signals:
    void moveElementSignal(int a_dir);
    void rotateElementSignal();
    void dropElementSignal();
    void moveElementDownSignal();
    void finished();
    void getDroppedStatusSignal();
    void pauseResumeSignal();

    void updateFieldsSignal(const FieldMatrix& a_matrix, const Element& a_element);

    void updateFieldWidgetSignal(const FieldMatrix& a_matrix);
    void updateNextBlockWidgetSignal(const Element &a_element, Qt::GlobalColor a_color);
    void updateGameStat(const GameStat& a_stat);

public slots:
    void moveElementSlot(int a_dir);
    void rotateElementSlot();
    void dropElementSlot();
    void moveElementDownSlot();
    void getDroppedStatusSlot();
    void pauseResumeSlot();

private:
    int8_t findFirstLineToDelete() const;
    bool canRotateElement(bool a_clockwise = false);
    bool canFitElement(const Element& a_element, const Position& a_pos, bool a_clean = true);
    bool canFitElement(const Element& a_element, int8_t a_x, int8_t a_y, bool a_clean = true);
    bool canMoveCurrentElementDownNext();
    void setElementPositionMatrix(const Element& a_element, int8_t a_x, int8_t a_y);
    void setElementPositionMatrix(const Element& a_element, const Position& a_pos);
    void cleanElementPositionMatrix(const Position& a_pos, bool a_allMatrix = true);
    void cleanElementPositionMatrix(FieldMatrix& a_field, const Position& a_pos, bool a_allMatrix = true);
    void copyNextToCurrentElement();
    void copyFieldMatrix(ShapeMatrix &a_dst, int8_t a_x, int8_t a_y);
    void copyFieldMatrix(const FieldMatrix& a_field, ShapeMatrix &a_dst, int8_t a_x, int8_t a_y);
    void deleteLine(uint8_t a_pos);
    void calcScore();
    uint32_t calcDeltaScore();
    void calcEfficiency();
    void deleteLines(uint8_t a_pos, uint8_t a_count);
    bool isGameOver();
    void slideEfficiencyHistoryWindow();
    void setSpeedByVar(uint8_t& a_speedVar, uint8_t a_speedVal);

public:
    explicit GameFieldThread(QObject *parent = nullptr);    
    ~GameFieldThread();

    void init();
    void setRotateDirection(bool a_clockwise = false);
    void setRotateOnStart(bool a_rotate = false);
    void generateElements(bool a_rotate = false);
    void generateElementsN(bool a_rotate = false);
    void setActiveElements(uint8_t a_index);
    void setCurrentElement(uint8_t a_index);
    void setNextElement(uint8_t a_index);
    void rotateElement();
    bool setElementPosition(const Element& a_element, const Position& a_pos, bool a_clean = true);
    bool setElementPosition(const Element& a_element, int8_t a_x, int8_t a_y, bool a_clean = true);
    Position getElementPosition() const;
    bool dropElement();
    uint64_t getScore() const;
    FieldMatrix getField() const;
    const FieldMatrix& getFieldRef() const;
    float getEfficiency() const;
    float getMaxEfficiency() const;
    uint8_t processState();
    uint8_t runStep(bool a_processDelLines = false);
    uint64_t getDeletedLinesCount() const;
    void setStartSpeed(uint8_t a_speed);
    void setSpeed(uint8_t a_speed);
    void increaseSpeed();
    bool moveElementLeft();
    bool moveElementRight();
    bool moveElementDown();
    const float* getEfficiencyHistory() const;
    void run() override;
    void pauseGame();
    void resumeGame();
    void finishGame();
    bool getGameProgressStatus() const;
    bool getDroppedStatus();
    void stopLoop();
};

#endif // GAMEFIELDTHREAD_H

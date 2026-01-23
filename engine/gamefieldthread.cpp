#include "gamefieldthread.h"

#include "shape.h"
#include "helper.h"

///#include <QDebug>

GameFieldThread::GameFieldThread(QObject *parent)
    : QThread{parent}
{
    init();

    connect(this, SIGNAL(moveElementSignal(int)), SLOT(moveElementSlot(int)));
    connect(this, SIGNAL(rotateElementSignal()), SLOT(rotateElementSlot()));
    connect(this, SIGNAL(dropElementSignal()), SLOT(dropElementSlot()));
    connect(this, SIGNAL(moveElementDownSignal()), SLOT(moveElementDownSlot()));
    connect(this, SIGNAL(pauseResumeSignal()), SLOT(pauseResumeSlot()));

    /*
    m_player = new QMediaPlayer(this);
    m_player->setSource(QUrl("qrc:/sounds/line_delete.ogg"));
    //m_player->setVolume(50); /// volume: 0-100
    */

    /*
    m_player = new QMediaPlayer;
    m_audioOutput = new QAudioOutput;
    m_player->setAudioOutput(m_audioOutput);
    m_player->setSource(QUrl("qrc:/sounds/line_delete.ogg"));
    */
}

GameFieldThread::~GameFieldThread()
{
    ///delete m_audioOutput;

    ///delete m_player;
}

void GameFieldThread::init()
{
    ///initRnd();

    std::memset(m_field.data, 0, sizeof(m_field.data));

    for (uint8_t i = 0; i < FIELD_HEIGHT; ++i)
    {
        m_field.data[i][0] = m_field.data[i][1] = 1;
        m_field.data[i][FIELD_WIDTH - FIELD_WIDTH_MARGIN / 2] = m_field.data[i][FIELD_WIDTH - 1] = 1;
    }

    for (uint8_t i = 0; i < FIELD_WIDTH; ++i)
    {
        m_field.data[FIELD_GAME_HEIGHT + FIELD_HEIGHT_MARGIN_UP][i] = 1;
    }

    m_score = m_deltaScore = m_delLinesCount = m_elementCount = 0;

    ///m_speed = 1;

    m_curDelLinesCount = 0;

    m_efficiency = m_maxEfficiency = 0.0;

    m_scoreBlock = 0.0;

    ///m_rotateClock = false;

    m_dropped = false;

    m_downNext = true;

    m_isPaused = false;

    m_isStarted = false;
}

void GameFieldThread::setRotateDirection(bool a_clockwise)
{
    m_rotateClock = a_clockwise;
}

void GameFieldThread::setRotateOnStart(bool a_rotate)
{
    m_rotateOnStart = a_rotate;
}

int8_t GameFieldThread::findFirstLineToDelete() const
{
    int8_t index = -1;

    ///for (uint8_t i = FIELD_START_Y; i < FIELD_START_Y + FIELD_GAME_HEIGHT; ++i) /// original version, starting from the top
    for (int32_t i = FIELD_START_Y + FIELD_GAME_HEIGHT - 1; i >= FIELD_START_Y; --i) /// reverse order
    {
        bool found = true;

        for (uint8_t j = FIELD_START_X; j < FIELD_START_X + FIELD_GAME_WIDTH; ++j)
        {
            if (m_field.data[i][j] == 0)
            {
                found = false;
            }
        }

        if (found)
        {
            index = i;
            break;
        }
    }

    return index;
}

bool GameFieldThread::canRotateElement(bool a_clockwise)
{
    Element tmpElement = m_currentElement;

    tmpElement.rotate(a_clockwise);

    return canFitElement(tmpElement, m_curElemPos);
}

bool GameFieldThread::canFitElement(const Element& a_element, const Position& a_pos, bool a_clean)
{
    return canFitElement(a_element, a_pos.x, a_pos.y, a_clean);
}

bool GameFieldThread::canFitElement(const Element& a_element, int8_t a_x, int8_t a_y, bool a_clean)
{
    bool res = true;

    int8_t x = FIELD_START_X + a_x;
    int8_t y = FIELD_START_Y + a_y;

    ShapeMatrix matrix;

    FieldMatrix field = m_field;

    if (a_clean)
    {
        cleanElementPositionMatrix(field, m_curElemPos, false);
    }

    copyFieldMatrix(field, matrix, x, y);

    if (a_element.isMatrixOverlap(matrix))
    {
        res = false;
    }

    ////cleanElementPositionMatrix(m_curElemPos);

    return res;
}

bool GameFieldThread::canMoveCurrentElementDownNext()
{
    Position tmpPos = m_curElemPos;

    ++tmpPos.y;

    bool b = canFitElement(m_currentElement, tmpPos);

    return b;
}

void GameFieldThread::copyNextToCurrentElement()
{
    m_currentElement.copyElement(m_nextElement);
}

void GameFieldThread::copyFieldMatrix(ShapeMatrix &a_dst, int8_t a_x, int8_t a_y)
{
    copyFieldMatrix(m_field, a_dst, a_x, a_y);
}

void GameFieldThread::copyFieldMatrix(const FieldMatrix& a_field, ShapeMatrix &a_dst, int8_t a_x, int8_t a_y)
{
    for (int8_t i = a_y; i < a_y + SIZE_SHAPE; ++i)
    {
        for (int8_t j = a_x; j < a_x + SIZE_SHAPE; ++j)
        {
            a_dst.data[i - a_y][j - a_x] = a_field.data[i][j];
        }
    }
}
void GameFieldThread::deleteLine(uint8_t a_pos)
{
    QMutexLocker locker(&m_mutex);

    for (int32_t j = a_pos; j >= FIELD_START_Y; --j)
    {
        for (uint8_t i = FIELD_START_X; i < FIELD_START_X + FIELD_GAME_WIDTH; ++i)
        {
            m_field.data[j][i] = m_field.data[j - 1][i];
        }
    }
}

void GameFieldThread::calcScore()
{
    m_score += calcDeltaScore();

    m_scoreBlock = (float)m_score / (m_elementCount + 1); /// +1 is needed as the last block/score is not counted in the
                                                          /// main cycle until is placed in place
}

uint32_t GameFieldThread::calcDeltaScore()
{
    ElementParams params = m_currentElement.getParams();

    float speedQ = 1.0 + (float)m_speed * params.speedQ;

    float elementScore = (float)params.score * speedQ;

    float lineScore = ((1.0 + (float)m_curDelLinesCount)/2.0) * (float)LINE_DEL_SCORE * speedQ;

    if (m_dropped)
    {
        lineScore *= params.dropQ;
        elementScore *= params.dropQ;
    }

    uint32_t deltaScore = std::round(elementScore + lineScore);

    m_deltaScore = deltaScore;

    m_dropped = false;
    m_curDelLinesCount = 0;

    return deltaScore;
}

void GameFieldThread::rotateElement()
{
    QMutexLocker locker(&m_mutex);  /// higher-level mutex checking to avoid race condition

    if (canRotateElement(m_rotateClock))
    {
        cleanElementPositionMatrix(m_curElemPos, false); //// TODO: Fixed - when rotating it may delete the existing blocks

        m_currentElement.rotate(m_rotateClock);

        //// TODO: Fixed - This may cause incorrect behavior when shape reaches existing blocks at bottom
        setElementPositionMatrix(m_currentElement, m_curElemPos);

        m_downNext = canMoveCurrentElementDownNext();
    }
}

bool GameFieldThread::setElementPosition(const Element& a_element, const Position& a_pos, bool a_clean)
{
    QMutexLocker locker(&m_mutex); /// original place for mutex lock was here, now added where m_field is changed

    bool res = canFitElement(a_element, a_pos);

    if (res)
    {
        if (a_clean)
        {
            cleanElementPositionMatrix(m_curElemPos, false);
        }

        m_curElemPos = a_pos;
        setElementPositionMatrix(a_element, a_pos.x, a_pos.y);
    }

    return res;
}

bool GameFieldThread::setElementPosition(const Element& a_element, int8_t a_x, int8_t a_y, bool a_clean)
{
    Position pos{a_x, a_y};

    return setElementPosition(a_element, pos, a_clean);
}

void GameFieldThread::setElementPositionMatrix(const Element& a_element, int8_t a_x, int8_t a_y)
{
    int8_t x = FIELD_START_X + a_x;
    int8_t y = FIELD_START_Y + a_y;

    ShapeMatrix shape = a_element.getShape();

    /// QMutexLocker locker(&m_mutex); /// original place is here, as any action with field change is done here

    for (uint8_t i = 0; i < SIZE_SHAPE; ++i)
    {
        for (uint8_t j = 0; j < SIZE_SHAPE; ++j)
        {
            int8_t Y = y + i;
            int8_t X = x + j;

            if (X >= FIELD_START_X && X <= FIELD_START_X + FIELD_GAME_WIDTH - 1 &&
                Y >= FIELD_START_Y && Y <= FIELD_START_Y + FIELD_GAME_HEIGHT - 1)
            {
                uint8_t tmpVal = shape.data[i][j];

                if (tmpVal)
                {
                    m_field.data[Y][X] = tmpVal;
                }
            }
        }
    }
}

void GameFieldThread::setElementPositionMatrix(const Element& a_element, const Position& a_pos)
{
    setElementPositionMatrix(a_element, a_pos.x, a_pos.y);
}

void GameFieldThread::cleanElementPositionMatrix(const Position& a_pos, bool a_allMatrix)
{
    cleanElementPositionMatrix(m_field, a_pos, a_allMatrix);
}

void GameFieldThread::cleanElementPositionMatrix(FieldMatrix& a_field, const Position& a_pos, bool a_allMatrix)
{
    uint8_t x = FIELD_START_X + a_pos.x;
    uint8_t y = FIELD_START_Y + a_pos.y;

    ShapeMatrix shape = m_currentElement.getShape();

    for (uint8_t i = 0; i < SIZE_SHAPE; ++i)
    {
        for (uint8_t j = 0; j < SIZE_SHAPE; ++j)
        {
            uint8_t Y = y + i;
            uint8_t X = x + j;

            if (X >= FIELD_START_X && X < FIELD_START_X + FIELD_GAME_WIDTH &&
                Y >= FIELD_START_Y && Y < FIELD_START_Y + FIELD_GAME_HEIGHT)
            {
                if (a_allMatrix || shape.data[i][j] > 0)
                {
                    a_field.data[Y][X] = 0;
                }
            }
        }
    }
}

bool GameFieldThread::dropElement()
{    
    bool res = false;

    while (moveElementDown())
    {
        res = true;
    }

    m_dropped = res;

    return res;
}

uint64_t GameFieldThread::getScore() const
{
    return m_score;
}

FieldMatrix GameFieldThread::getField() const
{
    return m_field;
}

const FieldMatrix& GameFieldThread::getFieldRef() const
{
    return m_field;
}

void GameFieldThread::generateElements(bool a_rotate)
{
    m_currentElement.generateRandomShape();
    m_nextElement.generateRandomShape();

    if (a_rotate)
    {
        uint8_t r1 = genRndNumber() / 2; ///% 4; ///std::rand() % 4;
        uint8_t r2 = genRndNumber() / 2; ///% 4; ///std::rand() % 4;

        for (uint8_t i = 0; i < r1; ++i)
        {
            m_currentElement.rotate(m_rotateClock);
        }

        for (uint8_t i = 0; i < r2; ++i)
        {
            m_nextElement.rotate(m_rotateClock);
        }
    }
}

void GameFieldThread::generateElementsN(bool a_rotate)
{
    for (uint8_t i = 0; i < GENERATED_BLOCKS_NUMBER; ++i)
    {
        m_elements[i].generateRandomShape();

        if (a_rotate)
        {
            uint8_t r = genRndNumber() / 2; ///% 4; ///std::rand() % 4;

            for (uint8_t j = 0; j < r; ++j)
            {
                m_elements[i].rotate(m_rotateClock);
            }
        }
    }
}

void GameFieldThread::setActiveElements(uint8_t a_index)
{
    uint8_t index = a_index & (GENERATED_BLOCKS_NUMBER - 1);
    uint8_t indexN = (a_index + 1) & (GENERATED_BLOCKS_NUMBER - 1);

    m_currentElement = m_elements[index];
    m_nextElement = m_elements[indexN];
}

void GameFieldThread::setCurrentElement(uint8_t a_index)
{
    uint8_t index = a_index & (GENERATED_BLOCKS_NUMBER - 1);

    m_currentElement = m_elements[index];
}

void GameFieldThread::setNextElement(uint8_t a_index)
{
    uint8_t index = a_index & (GENERATED_BLOCKS_NUMBER - 1);

    m_nextElement = m_elements[index];
}

uint8_t GameFieldThread::processState()
{
    int8_t index;

    uint8_t delLinesCount = 0;

    while ((index = findFirstLineToDelete()) != -1)
    {
        deleteLine(index);
        ++delLinesCount;
    }

    m_delLinesCount += delLinesCount;

    return delLinesCount;
}

void GameFieldThread::deleteLines(uint8_t a_pos, uint8_t a_count)
{
    for (uint8_t i = FIELD_START_Y + a_pos; i < FIELD_START_Y + a_pos + a_count; ++i)
    {
        deleteLine(i);
    }
}

uint8_t GameFieldThread::runStep(bool a_processDelLines)
{
    if (a_processDelLines)
    {
        m_curDelLinesCount = processState();
    }

    if (m_delLinesCount > 0 && m_delLinesCount / 10 > m_speed - m_startSpeed)
    {
        increaseSpeed();
    }

    return m_curDelLinesCount;
}

bool GameFieldThread::isGameOver()
{
    bool res = false;

    if (!canFitElement(m_currentElement, FIELD_SHAPE_START_X, FIELD_SHAPE_START_Y, false))
    {
        res = true;
        m_isPaused = false;
    }

    m_isStarted = !res;

    return res;
}

uint64_t GameFieldThread::getDeletedLinesCount() const
{
    return m_delLinesCount;
}

void GameFieldThread::setSpeedByVar(uint8_t& a_speedVar, uint8_t a_speedVal)
{
    if (a_speedVal > MAX_SPEED)
    {
        a_speedVar = MAX_SPEED;
    }
    else
    {
        a_speedVar = a_speedVal;
    }
}

void GameFieldThread::setSpeed(uint8_t a_speed)
{
    setSpeedByVar(m_speed, a_speed);
}

void GameFieldThread::setStartSpeed(uint8_t a_speed)
{
    setSpeedByVar(m_speed, a_speed);
    setSpeedByVar(m_startSpeed, a_speed);
}

void GameFieldThread::increaseSpeed()
{
    uint8_t tmpSpeed = m_speed;

    ++tmpSpeed;

    setSpeed(tmpSpeed);
}

void GameFieldThread::slideEfficiencyHistoryWindow()
{
    for (uint8_t i = 1; i < MAX_EFFICIENCY_HISTORY; ++i)
    {
        m_efficiencyHistory[i - 1] = m_efficiencyHistory[i];
    }
}

void GameFieldThread::calcEfficiency()
{
    if (m_delLinesCount == 0)
    {
        m_efficiency = 0.0;
    }
    else
    {
        ///m_efficiency = (float)(m_delLinesCount * SURFACE_LINE * 100) / (m_elementCount * SURFACE_SHAPE);
        /// calculating efficiency formula looks more natural this way
        m_efficiency = ((float)(m_delLinesCount * SURFACE_LINE) / (float)(m_elementCount * SURFACE_SHAPE)) * 100.0;
    }

    if (m_efficiency > m_maxEfficiency)
    {
        m_maxEfficiency = m_efficiency;
    }

    ++m_historyCount;

    if (m_historyCount > MAX_EFFICIENCY_HISTORY)
    {
        slideEfficiencyHistoryWindow();

        m_historyCount = MAX_EFFICIENCY_HISTORY;
    }

    m_efficiencyHistory[m_historyCount - 1] = m_efficiency;
}

float GameFieldThread::getEfficiency() const
{
    return m_efficiency;
}

const float* GameFieldThread::getEfficiencyHistory() const
{
    return m_efficiencyHistory;
}

float GameFieldThread::getMaxEfficiency() const
{
    return m_maxEfficiency;
}

Position GameFieldThread::getElementPosition() const
{
    return m_curElemPos;
}

bool GameFieldThread::moveElementLeft()
{
    Position tmpPos = m_curElemPos;

    --tmpPos.x;

    bool b = setElementPosition(m_currentElement, tmpPos);

    if (b)
    {
        m_downNext = canMoveCurrentElementDownNext();
    }

    return b;
}

bool GameFieldThread::moveElementRight()
{
    Position tmpPos = m_curElemPos;

    ++tmpPos.x;

    bool b = setElementPosition(m_currentElement, tmpPos);

    if (b)
    {
        m_downNext = canMoveCurrentElementDownNext();
    }

    return b;
}

bool GameFieldThread::moveElementDown()
{
    Position tmpPos = m_curElemPos;

    ++tmpPos.y;

    return setElementPosition(m_currentElement, tmpPos);
}

void GameFieldThread::run()
{
    uint64_t timer = 0, timerSpeed = 0;

    bool down = true;

    m_downNext = true;

    m_isStarted = true;
    m_isPaused = false;

    /// generating 3 random elements in advance
    generateElementsN(m_rotateOnStart);
    setActiveElements(0);
    ///////////////////////////////////////////

    emit updateFieldsSignal(m_field, m_nextElement);

    setElementPosition(m_currentElement, FIELD_SHAPE_START_X, FIELD_SHAPE_START_Y);

    ++m_elementCount;

    /////////////////////////// initial stat sending
    GameStat gs;

    gs.speed = m_speed;
    gs.score = m_score;
    gs.deltaScore = m_deltaScore;
    gs.scoreblock = m_scoreBlock;
    gs.eff = m_efficiency;
    gs.delLines = m_delLinesCount;
    gs.blocks = m_elementCount;

    emit updateGameStat(gs);
    ///////////////////////////////////////////////////

    while (true)
    {
        if (!m_isStarted)
        {
            break;
        }

        if (m_isPaused)
        {
            QThread::msleep(BASE_SPEED_DELAY);
            continue;
        }

        timerSpeed = timer % (1 + MAX_SPEED - m_speed);

        ///if (!down && timerSpeed == 0) /// original version
        if (!down && !m_downNext && timerSpeed == 0)
        {
            ///++m_elementCount;

            calcScore();
            ///calcEfficiency();

            /// new algorithm
            uint8_t index = m_elementCount % GENERATED_BLOCKS_NUMBER;

            if (index == GENERATED_BLOCKS_NUMBER - 1)
            {
                setCurrentElement(index);
                generateElementsN(m_rotateOnStart);
                setNextElement(0);
            }
            else
            {
                setActiveElements(index);
            }

            if (isGameOver())
            {
                break;
            }

            setElementPosition(m_currentElement, FIELD_SHAPE_START_X, FIELD_SHAPE_START_Y, false);
            /// end of new algorithm

            ///calcEfficiency(); // original place was here

            ++m_elementCount;

            calcEfficiency();

            ///// sending stat again when block is dropped
            gs.speed = m_speed;
            gs.score = m_score;
            gs.deltaScore = m_deltaScore;
            gs.scoreblock = m_scoreBlock;
            gs.eff = m_efficiency;
            gs.delLines = m_delLinesCount;
            gs.blocks = m_elementCount;

            emit updateGameStat(gs);
            ////////////////////////////////////////////////////////////////
        }

        if (timerSpeed == 0)
        {
            down = moveElementDown();

            m_downNext = canMoveCurrentElementDownNext();
        }

        uint8_t linesDeleted = runStep(!m_downNext); //// originally runStep(!down) is called here. Here was potential bug causing delay

        emit updateFieldsSignal(m_field, m_nextElement);


        if (!m_dropped && down && m_downNext && linesDeleted == 0)
        {
            QThread::msleep(BASE_SPEED_DELAY);
        }
        else
        {
            //// this part is needed in order to quickly move the block left/right when already at bottom
            QThread::msleep(MINIMAL_DELAY);
        }

        /// timer correction algorithm
        /// this block is needed for immediate visualizing of filled lines deletion without delays
        if (linesDeleted)
        {
            uint64_t tmpTimer1 = timer / (1 + MAX_SPEED - m_speed);
            uint64_t tmpTimer2 = (tmpTimer1 + 1) * (1 + MAX_SPEED - m_speed);
            uint64_t deltaTimer = tmpTimer2 - timer;
            timer += deltaTimer;

            ////m_player->play();
        }
        else
        {
            ++timer;
        }
    }

    emit finished();
}

void GameFieldThread::pauseGame()
{
    m_isPaused = true;
}

void GameFieldThread::resumeGame()
{
    m_isPaused = false;
}

bool GameFieldThread::getGameProgressStatus() const
{
    return (!m_isPaused) & m_isStarted;
}

void GameFieldThread::finishGame()
{
    m_isStarted = false;
    m_isPaused = false;
}

void GameFieldThread::stopLoop()
{
    finishGame();
}

void GameFieldThread::moveElementSlot(int a_dir)
{
    a_dir == 0 ? moveElementLeft() : moveElementRight();
}

void GameFieldThread::rotateElementSlot()
{
    rotateElement();
}

void GameFieldThread::dropElementSlot()
{
    dropElement();
}

void GameFieldThread::moveElementDownSlot()
{
    moveElementDown();
}

void GameFieldThread::getDroppedStatusSlot()
{
    m_dropped = getDroppedStatus();
}

void GameFieldThread::pauseResumeSlot()
{
    bool status = getGameProgressStatus();

    if (status)
    {
        pauseGame();
    }
    else
    {
        resumeGame();
    }
}

bool GameFieldThread::getDroppedStatus()
{
    return m_dropped;
}

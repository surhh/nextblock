#include "fieldpainter.h"

#include <QPainterPath>
#include <QPainter>

#include "engine/helper.h"


#define FIELDPAINTER_DUMMY_BORDER_WIDTH      (30)
#define FIELDPAINTER_DUMMY_BORDER_HEIGHT     (20)


#define FIELDPAINTER_Y_X_RATIO_MIN      ((float)FIELD_GAME_HEIGHT/FIELD_GAME_WIDTH)  // (2.1)
#define FIELDPAINTER_Y_X_RATIO_MAX      ((float)FIELD_GAME_HEIGHT/FIELD_GAME_WIDTH)  // (2.3)

FieldPainter::FieldPainter(QWidget *parent)
    : QWidget{parent}
{
    /// initializations
    setFocusPolicy(Qt::NoFocus);

    m_colorMap[0] = Qt::black;
    m_colorMap[SHAPE_SQUARE] = Qt::cyan;
    m_colorMap[SHAPE_LONG] = Qt::darkMagenta;
    m_colorMap[SHAPE_GAMMA] = Qt::green;
    m_colorMap[SHAPE_ANTI_GAMMA] = Qt::red;
    m_colorMap[SHAPE_PODIUM] = Qt::blue;
    m_colorMap[SHAPE_N] = Qt::white;
    m_colorMap[SHAPE_ANTI_N] = Qt::yellow;
}

void FieldPainter::setFieldMatrix(const FieldMatrix& a_field)
{
    m_fieldMatrix = a_field;
}

void FieldPainter::drawCell(int a_x, int a_y, int a_color)
{
    QBrush brush(Qt::SolidPattern);
    QPainter painter(this);

    brush.setColor(m_colorMap[a_color]);

    painter.setPen(Qt::NoPen);
    painter.setBrush(brush);
    painter.drawRect(QRect(a_x, a_y, m_cellSize, m_cellSize));
}

void FieldPainter::paintField()
{
    for (int i = FIELD_START_Y; i < FIELD_START_Y + FIELD_GAME_HEIGHT; ++i)
    {
        for (int j = FIELD_START_X; j < FIELD_START_X + FIELD_GAME_WIDTH; ++j)
        {
            drawCell(m_fieldStartX + m_cellSize * (j - FIELD_START_X), m_fieldStartY + m_cellSize * (i - FIELD_START_Y),
                     m_fieldMatrix.data[i][j]);
        }
    }
}

void FieldPainter::roundSizes()
{
    m_fieldWidth = ((m_fieldWidth / FIELD_GAME_WIDTH) + 1) * FIELD_GAME_WIDTH;
    m_fieldHeight = ((m_fieldHeight / FIELD_GAME_HEIGHT) + 1) * FIELD_GAME_HEIGHT;
}

void FieldPainter::setPen(const QPen& a_pen)
{
    m_pen = a_pen;
    update();
}

void FieldPainter::setBrush(const QBrush& a_brush)
{
    m_brush = a_brush;
    update();
}

void FieldPainter::setAntialiased(bool a_antialiased)
{
    m_antialiased = a_antialiased;
    update();
}

void FieldPainter::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QMutexLocker locker(&m_mutex);
    painter.setPen(m_pen);
    painter.setBrush(m_brush);

    int w = width();
    int h = height();

    m_fieldWidth = w - 2*FIELDPAINTER_DUMMY_BORDER_WIDTH;
    m_fieldHeight = h - 2*FIELDPAINTER_DUMMY_BORDER_HEIGHT;

    float fieldRatio = (float) m_fieldHeight / m_fieldWidth;

    if (fieldRatio < FIELDPAINTER_Y_X_RATIO_MIN)
    {
        m_fieldWidth = m_fieldHeight / FIELDPAINTER_Y_X_RATIO_MIN;
    }
    else
    {
        m_fieldWidth = m_fieldHeight / FIELDPAINTER_Y_X_RATIO_MAX;
    }

    roundSizes();

    m_fieldStartX = (w - m_fieldWidth)/2;
    m_fieldStartY = (h - m_fieldHeight)/2;
    m_cellSize = m_fieldWidth / FIELD_GAME_WIDTH;

    QBrush brush(Qt::SolidPattern);

    painter.setPen(Qt::NoPen);
    painter.setBrush(brush);
    painter.drawRect(QRect(m_fieldStartX, m_fieldStartY, m_fieldWidth, m_fieldHeight));

    paintField();
}

Qt::GlobalColor FieldPainter::getElementColor(int a_shapeType)
{
    return m_colorMap[a_shapeType];
}

void FieldPainter::updateField(const FieldMatrix& a_matrix)
{
    QMutexLocker locker(&m_mutex);

    setFieldMatrix(a_matrix);
    update();
}

#include "nextblockpainter.h"

#include <QPainter>

#include <iostream>

#include "engine/helper.h"

#define PREVIEW_FIELD_WIDTH     (6)
#define PREVIEW_FIELD_HEIGHT    (6)

NextBlockPainter::NextBlockPainter(QWidget *parent)
    : QWidget{parent}
{
    m_visible = true;

    setFocusPolicy(Qt::NoFocus);
}

void NextBlockPainter::setColor(Qt::GlobalColor a_color)
{
    m_color = a_color;
}

void NextBlockPainter::drawCell(int a_x, int a_y, int a_color)
{
    QBrush brush(Qt::SolidPattern);
    QPainter painter(this);

    Qt::GlobalColor col = a_color != 0 ? m_color : Qt::black;
    brush.setColor(a_color != 0 ? m_color : Qt::black);

    painter.setPen(Qt::NoPen);
    painter.setBrush(brush);
    painter.drawRect(QRect(a_x, a_y, m_cellSize, m_cellSize));
}

void NextBlockPainter::setPen(const QPen& a_pen)
{
    m_pen = a_pen;
    update();
}

void NextBlockPainter::setBrush(const QBrush& a_brush)
{
    m_brush = a_brush;
    update();
}

void NextBlockPainter::setAntialiased(bool a_antialiased)
{
    m_antialiased = a_antialiased;
    update();
}

void NextBlockPainter::paintField()
{
    for (int i = 0; i < SIZE_SHAPE; ++i)
    {
        for (int j = 0; j < SIZE_SHAPE; ++j)
        {
            drawCell(m_fieldStartX + m_cellSize * (j+1), m_fieldStartY + m_cellSize * (i+1), m_element.getShape().data[i][j]);
        }
    }
}

void NextBlockPainter::setVisible(bool a_visible)
{
    m_visible = a_visible;

    update();
}

void NextBlockPainter::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setPen(m_pen);
    painter.setBrush(m_brush);

    int w = width();
    int h = height();

    m_fieldWidth = w - 2*PREVIEW_FIELD_WIDTH;
    m_fieldHeight = h - 2*PREVIEW_FIELD_HEIGHT;

    float fieldRatio = (float) m_fieldHeight / m_fieldWidth;

    if (fieldRatio < 1)
    {
        m_fieldWidth = m_fieldHeight / 1;
    }
    else
    {
        m_fieldWidth = m_fieldHeight / 1;
    }

    roundSizes();

    m_fieldStartX = (w - m_fieldWidth)/2;
    m_fieldStartY = (h - m_fieldHeight)/2;
    m_cellSize = m_fieldWidth / PREVIEW_FIELD_WIDTH;

    QBrush brush(Qt::SolidPattern);

    painter.setPen(Qt::NoPen);
    painter.setBrush(brush);
    painter.drawRect(QRect(m_fieldStartX, m_fieldStartY, m_fieldWidth, m_fieldHeight));

    if (m_visible)
    {
        paintField();
    }
}

void NextBlockPainter::roundSizes()
{
    m_fieldWidth = ((m_fieldWidth / PREVIEW_FIELD_WIDTH) + 1) * PREVIEW_FIELD_WIDTH;
    m_fieldHeight = ((m_fieldHeight / PREVIEW_FIELD_HEIGHT) + 1) * PREVIEW_FIELD_HEIGHT;
}

void NextBlockPainter::setNextBlock(const Element& a_element)
{
    m_element = a_element;
}

void NextBlockPainter::updateNextBlock(const Element &a_element, Qt::GlobalColor a_color)
{
    setNextBlock(a_element);
    setColor(a_color);
    update();
}

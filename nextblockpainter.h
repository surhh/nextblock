#ifndef NEXTBLOCKPAINTER_H
#define NEXTBLOCKPAINTER_H

#include <QWidget>
#include <QBrush>
#include <QPen>

#include "engine/shape.h"

class NextBlockPainter : public QWidget
{
    Q_OBJECT

public:
    explicit NextBlockPainter(QWidget *parent = nullptr);
    void setPen(const QPen& a_pen);
    void setBrush(const QBrush& a_brush);
    void setAntialiased(bool a_antialiased);
    void paintField();
    void setNextBlock(const Element& a_element);
    void setColor(Qt::GlobalColor a_color);
    void setVisible(bool a_visible = true);

private:
    QPen        m_pen;
    QBrush      m_brush;

    Element     m_element;

    Qt::GlobalColor   m_color;

    int         m_fieldWidth;
    int         m_fieldHeight;

    int         m_fieldStartX;
    int         m_fieldStartY;

    int         m_cellSize;

    bool        m_antialiased;

    bool        m_visible;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void roundSizes();
    void drawCell(int a_x, int a_y, int a_color);

signals:

public slots:
    void updateNextBlock(const Element& a_element, Qt::GlobalColor a_color);
};

#endif // NEXTBLOCKPAINTER_H

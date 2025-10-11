#ifndef FIELDPAINTER_H
#define FIELDPAINTER_H

#include <QWidget>
#include <QBrush>
#include <QPen>
#include <QMutex>

#include <map>

#include "engine/gamefieldthread.h"

class FieldPainter : public QWidget
{
    Q_OBJECT

public:

    explicit FieldPainter(QWidget *parent = nullptr);
    void setAntialiased(bool a_antialiased);
    void paintField();
    void setFieldMatrix(const FieldMatrix& a_field);
    Qt::GlobalColor getElementColor(int a_shapeType);

private:
    QPen        m_pen;
    QBrush      m_brush;

    FieldMatrix m_fieldMatrix;

    std::map<int, Qt::GlobalColor>   m_colorMap;

    int         m_fieldWidth;
    int         m_fieldHeight;

    int         m_fieldStartX;
    int         m_fieldStartY;

    int         m_cellSize;

    bool        m_antialiased;

    QMutex      m_mutex;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void roundSizes();
    void drawCell(int a_x, int a_y, int a_color);
    void setPen(const QPen& a_pen);
    void setBrush(const QBrush& a_brush);

signals:

public slots:
    void updateField(const FieldMatrix& a_matrix);
};

#endif // FIELDPAINTER_H

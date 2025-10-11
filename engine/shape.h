#ifndef SHAPE_H
#define SHAPE_H

#include <cstdint>

#define SHAPE_SQUARE       (1)      // shape number 0
#define SHAPE_LONG         (3)      // shape number 1
#define SHAPE_GAMMA        (5)      // shape number 2
#define SHAPE_ANTI_GAMMA   (9)      // shape number 3
#define SHAPE_PODIUM       (17)     // shape number 4
#define SHAPE_N            (33)     // shape number 5
#define SHAPE_ANTI_N       (65)     // shape number 6

#define COUNT_SHAPE        (7)
#define SIZE_SHAPE         (4)
#define SURFACE_SHAPE      (4)

#define SCORE_1            (15)
#define SCORE_2            (20)
#define SCORE_3            (25)

#define DROPQ_1            (1.7)
#define DROPQ_2            (1.8)
#define DROPQ_3            (1.9)

#define SPEEDQ_1           (0.01)
#define SPEEDQ_2           (0.02)
#define SPEEDQ_3           (0.03)

#define LINE_DEL_SCORE     (50)

struct ShapeMatrix
{
    uint8_t data[SIZE_SHAPE][SIZE_SHAPE];
};

struct ElementParams
{
    uint8_t     type;
    uint16_t    score;
    float       dropQ;
    float       speedQ;
};

class Element
{
private:
    ShapeMatrix m_shape;

    ElementParams m_params;

    void cleanShape();

    void normalizeX();
    void normalizeY();
    bool canNormalizeX();
    bool canNormalizeY();


public:
    ~Element();
    Element();

    void setShape(uint8_t a_shapeType);
    void setShape(const ShapeMatrix& a_matrix);
    ShapeMatrix getShape() const;

    ElementParams getParams() const;
    void setParams(const ElementParams& a_params);

    void generateRandomShape();
    void rotate(bool a_clockwise = true);
    void copyElement(const Element& a_element);

    bool isMatrixOverlap(const ShapeMatrix& a_matrix) const;

    void normalize();
};


#endif // SHAPE_H

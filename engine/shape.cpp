#include <cstdlib>
#include <cstring>
#include <algorithm>

#include "shape.h"

#include <ctime>
#include <random>

static std::mt19937 rnd;

void initRnd()
{
    rnd.seed(time(nullptr));
}

uint32_t genRndNumber()
{
    return rnd();
}

Element::~Element()
{

}

Element::Element()
{
    for (uint8_t i = 0; i < SIZE_SHAPE; ++i)
    {
        for (uint8_t j = 0; j < SIZE_SHAPE; ++j)
        {
            m_shape.data[i][j] = 0;
        }
    }

    m_params = {0, 0, 0.0, 0.0};
}

void Element::cleanShape()
{
    std::memset(m_shape.data, 0, sizeof(m_shape.data));
}

void Element::setShape(uint8_t a_shapeType)
{
    cleanShape();

    switch (a_shapeType)
    {
        case 0:
            m_shape.data[2][1]=m_shape.data[2][2]=m_shape.data[3][1]=m_shape.data[3][2]=m_params.type = SHAPE_SQUARE;
            m_params.score = SCORE_1;
            m_params.dropQ = DROPQ_1;
            m_params.speedQ = SPEEDQ_1;
            break;
        case 1:
            m_shape.data[0][1]=m_shape.data[1][1]=m_shape.data[2][1]=m_shape.data[3][1]=m_params.type = SHAPE_LONG;
            m_params.score = SCORE_1;
            m_params.dropQ = DROPQ_1;
            m_params.speedQ = SPEEDQ_1;
            break;
        case 2:
            m_shape.data[1][2]=m_shape.data[1][1]=m_shape.data[2][1]=m_shape.data[3][1]=m_params.type = SHAPE_GAMMA;
            m_params.score = SCORE_2;
            m_params.dropQ = DROPQ_2;
            m_params.speedQ = SPEEDQ_2;
            break;
        case 3:
            m_shape.data[1][1]=m_shape.data[1][2]=m_shape.data[2][2]=m_shape.data[3][2]=m_params.type = SHAPE_ANTI_GAMMA;
            m_params.score = SCORE_2;
            m_params.dropQ = DROPQ_2;
            m_params.speedQ = SPEEDQ_2;
            break;
        case 4:
            m_shape.data[2][1]=m_shape.data[3][0]=m_shape.data[3][1]=m_shape.data[3][2]=m_params.type = SHAPE_PODIUM;
            m_params.score = SCORE_3;
            m_params.dropQ = DROPQ_3;
            m_params.speedQ = SPEEDQ_3;
            break;
        case 5:
            m_shape.data[1][1]=m_shape.data[2][1]=m_shape.data[2][2]=m_shape.data[3][2]=m_params.type = SHAPE_N;
            m_params.score = SCORE_3;
            m_params.dropQ = DROPQ_3;
            m_params.speedQ = SPEEDQ_3;
            break;
        case 6:
            m_shape.data[1][2]=m_shape.data[2][2]=m_shape.data[2][1]=m_shape.data[3][1]=m_params.type = SHAPE_ANTI_N;
            m_params.score = SCORE_3;
            m_params.dropQ = DROPQ_3;
            m_params.speedQ = SPEEDQ_3;
            break;
        default:
            break;
    }
}

void Element::setShape(const ShapeMatrix& a_matrix)
{
    std::copy(&a_matrix.data[0][0], &a_matrix.data[0][0] + SIZE_SHAPE * SIZE_SHAPE, &m_shape.data[0][0]);
}

void Element::generateRandomShape()
{
    uint8_t type = genRndNumber() % COUNT_SHAPE; /// std::rand() % COUNT_SHAPE;

    setShape(type);
}

ShapeMatrix Element::getShape() const
{
    return m_shape;
}


ElementParams Element::getParams() const
{
    return m_params;
}

void Element::setParams(const ElementParams& a_params)
{
    m_params = a_params;
}

void Element::rotate(bool a_clockwise)
{
    ShapeMatrix tmpMatrix;

    if (m_params.type == SHAPE_SQUARE)
        return;

    for (uint8_t i = 0; i < SIZE_SHAPE; ++i)
    {
        for (uint8_t j = 0; j < SIZE_SHAPE; ++j)
        {
            if (a_clockwise)
            {
                tmpMatrix.data[j][SIZE_SHAPE - i - 1] = m_shape.data[i][j];
            }
            else
            {
                tmpMatrix.data[SIZE_SHAPE - j - 1][i] = m_shape.data[i][j];
            }
        }
    }

    setShape(tmpMatrix);

    normalize();
}

void Element::copyElement(const Element& a_element)
{
    setShape(a_element.getShape());

    setParams(a_element.getParams());
}

bool Element::isMatrixOverlap(const ShapeMatrix& a_matrix) const
{
    for (uint8_t i = 0; i < SIZE_SHAPE; ++i)
    {
        for (uint8_t j = 0; j < SIZE_SHAPE; ++j)
        {
            if (m_shape.data[i][j] & a_matrix.data[i][j])
            {
                return true;
            }
        }
    }

    return false;
}

bool Element::canNormalizeX()
{
    for (uint8_t i = 0; i < SIZE_SHAPE; ++i)
    {
        if (m_shape.data[i][0] != 0)
        {
            return false;
        }
    }

    return true;
}

bool Element::canNormalizeY()
{
    for (uint8_t i = 0; i < SIZE_SHAPE; ++i)
    {
        if (m_shape.data[SIZE_SHAPE-1][i] != 0)
        {
            return false;
        }
    }

    return true;
}


void Element::normalizeX()
{
    if (canNormalizeX())
    {
        for (uint8_t i = 1; i < SIZE_SHAPE; ++i)
        {
            for (uint8_t j = 0; j < SIZE_SHAPE; ++j)
            {
                m_shape.data[j][i-1] = m_shape.data[j][i];
                m_shape.data[j][i] = 0;
            }
        }
    }
}

void Element::normalizeY()
{
    if (canNormalizeY())
    {
        for (int8_t i = SIZE_SHAPE - 2; i >= 0 ; --i)
        {
            for (uint8_t j = 0; j < SIZE_SHAPE; ++j)
            {
                 m_shape.data[i+1][j] = m_shape.data[i][j];
                 m_shape.data[i][j] = 0;
            }
        }
    }
}

void Element::normalize()
{
    normalizeX();
    normalizeY();
}

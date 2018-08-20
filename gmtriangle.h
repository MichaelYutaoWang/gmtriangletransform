#ifndef GMTRIANGLE_H
#define GMTRIANGLE_H

#include <QLineF>

class GmTriangle
{
public:
    GmTriangle();
    GmTriangle(const QPointF &pa, const QPointF &pb, const QPointF &pc);
    GmTriangle(const GmTriangle &other);

public:
    GmTriangle & operator=(const GmTriangle &other);

    friend bool operator==(const GmTriangle &t1, const GmTriangle &t2);
    friend bool operator!=(const GmTriangle &t1, const GmTriangle &t2);

public:
    void setPoints(const QPointF &pa, const QPointF &pb, const QPointF &pc);
    void setPa(const QPointF &pa);
    void setPb(const QPointF &pb);
    void setPc(const QPointF &pc);

    void clear();

    const QPointF &pa() const;
    const QPointF &pb() const;
    const QPointF &pc() const;

    QLineF edgeAB() const;
    QLineF edgeBC() const;
    QLineF edgeAC() const;

    bool isNull() const;

public:
    bool collinear() const;
    double area() const;

private:
    void assign(const GmTriangle &other);

private:
    QPointF m_pa, m_pb, m_pc;
};

#endif // GMTRIANGLE_H

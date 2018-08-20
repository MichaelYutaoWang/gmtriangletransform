#include "gmtriangle.h"

#include <QtCore>

GmTriangle::GmTriangle()
{
}

GmTriangle::GmTriangle(const QPointF &pa, const QPointF &pb, const QPointF &pc)
{
    setPoints(pa, pb, pc);
}

GmTriangle::GmTriangle(const GmTriangle &other)
{
    assign(other);
}

GmTriangle & GmTriangle::operator=(const GmTriangle &other)
{
    assign(other);
    return *this;
}

void GmTriangle::assign(const GmTriangle &other)
{
    m_pa = other.m_pa;
    m_pb = other.m_pb;
    m_pc = other.m_pc;
}

void GmTriangle::setPoints(const QPointF &pa, const QPointF &pb, const QPointF &pc)
{
    m_pa = pa;
    m_pb = pb;
    m_pc = pc;
}

void GmTriangle::setPa(const QPointF &pa)
{
    m_pa = pa;
}

void GmTriangle::setPb(const QPointF &pb)
{
    m_pb = pb;
}

void GmTriangle::setPc(const QPointF &pc)
{
    m_pc = pc;
}

void GmTriangle::clear()
{
    m_pa = m_pb = m_pc = QPointF();
}

const QPointF &GmTriangle::pa() const
{
    return m_pa;
}

const QPointF &GmTriangle::pb() const
{
    return m_pb;
}

const QPointF &GmTriangle::pc() const
{
    return m_pc;
}

QLineF GmTriangle::edgeAB() const
{
    QLineF lab(m_pa, m_pb);
    return lab;
}

QLineF GmTriangle::edgeBC() const
{
    QLineF lbc(m_pb, m_pc);
    return lbc;
}

QLineF GmTriangle::edgeAC() const
{
    QLineF lac(m_pa, m_pc);
    return lac;
}

bool GmTriangle::isNull() const
{
    bool ok = m_pa.isNull() && m_pb.isNull() && m_pc.isNull();
    return ok;
}

bool GmTriangle::collinear() const
{
    double x1 = m_pb.x() - m_pa.x();
    double y1 = m_pb.y() - m_pa.y();
    double x2 = m_pc.x() - m_pb.x();
    double y2 = m_pc.y() - m_pb.y();

    double ss = x1*y2 + x2*y1;
    bool ok = qFuzzyIsNull(ss);
    return ok;
}

double GmTriangle::area() const
{
    double a = QLineF(m_pa, m_pb).length();
    double b = QLineF(m_pb, m_pc).length();
    double c = QLineF(m_pa, m_pc).length();
    double p = (a + b + c) / 2.0;
    double s = qSqrt(p * (p-a) * (p-b) * (p-c));
    return s;
}

bool operator==(const GmTriangle &t1, const GmTriangle &t2)
{
    bool ok = (t1.m_pa == t2.m_pa && t1.m_pb == t2.m_pb && t1.m_pc == t2.m_pc);
    return ok;
}

bool operator!=(const GmTriangle &t1, const GmTriangle &t2)
{
    bool ok = !(t1 == t2);
    return ok;
}

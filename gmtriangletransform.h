#ifndef GMTRIANGLETRANSFORM_H
#define GMTRIANGLETRANSFORM_H

#include "gmtriangle.h"

#include <QTransform>
#include <QPixmap>

class GmTriangleTransform
{
public:
    GmTriangleTransform();
    GmTriangleTransform(const GmTriangle &st, const GmTriangle &tt);
    GmTriangleTransform(const GmTriangleTransform &other);

public:
    // create transform from 's' space to 't' space
    //   by triangle st in s space and triangle tt in t space.
    // returns true if the transform is ready, otherwise returns false.
    bool setTriangle(const GmTriangle &st, const GmTriangle &tt);

    // return triangle datas
    const GmTriangle & getSTriangle() const;
    const GmTriangle & getTTriangle() const;

    // returns transform state
    bool isReady() const;

    // returns an inverted copy of this transform
    // *pok is set to true if the transform is ready, otherwise it is set to false.
    GmTriangleTransform inverted(bool *pok = 0) const;

    // returns true if this transform is equal to the given transform, otherwise returns false.
    bool operator==(const GmTriangleTransform & other) const;

    // returns true if this transform is not equal to the given transform, otherwise returns false.
    bool operator!=(const GmTriangleTransform & other) const;

    // assigns the given transform's values to this transform.
    GmTriangleTransform & operator=(const GmTriangleTransform & other);

public:
    // map s space data to t space
    // if transform is ok, *pok set to true, otherwise set to false
    GmTriangle map(const GmTriangle & st, bool *pok = 0) const;
    QPointF map(const QPointF & sp, bool *pok = 0) const;
    QLineF map(const QLineF & sl, bool *pok = 0) const;
    QPolygonF map(const QPolygonF & spoly, bool *pok = 0) const;
    QPolygonF mapRect(const QRectF & srect, bool *pok = 0) const;
    QPixmap map(const QPixmap &pixmap, bool *pok = 0) const;
    QImage map(const QImage &image, bool *pok = 0) const;

private:
    void init();
    bool prepare();
    void assign(const GmTriangleTransform & other);
    GmTriangle mapTriangle(const QTransform &tf, const GmTriangle & triangle) const;
    QTransform getImageTransform() const;

private:
    // transform state
    bool m_isReady;
    // tranform parameters
    QTransform m_s2tEdgeAB, m_tMirror, m_sPcMirror, m_sMirrorScale;
    GmTriangle m_ttMirror;
    bool m_sNeedMirror;
    QLineF m_tMirrorEdgeAC;
    qreal m_scalexP2;

private:
    // transform releation triangle data, must contains three points
    GmTriangle m_st, m_tt;
};

#endif // GMTRIANGLETRANSFORM_H

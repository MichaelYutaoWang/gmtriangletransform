#include "gmtriangletransform.h"

GmTriangleTransform::GmTriangleTransform()
{
    init();
}

GmTriangleTransform::GmTriangleTransform(const GmTriangleTransform &other)
{
    assign(other);
}

GmTriangleTransform::GmTriangleTransform(const GmTriangle &st, const GmTriangle &tt)
{
    init();
    setTriangle(st, tt);
}

void GmTriangleTransform::init()
{
    m_isReady = false;
    m_sNeedMirror = false;
    m_scalexP2 = 1.0;
}

void GmTriangleTransform::assign(const GmTriangleTransform & other)
{
    m_isReady = other.m_isReady;

    m_s2tEdgeAB = other.m_s2tEdgeAB;
    m_tMirror = other.m_tMirror;
    m_sPcMirror = other.m_sPcMirror;
    m_sMirrorScale = other.m_sMirrorScale;
    m_ttMirror = other.m_ttMirror;
    m_sNeedMirror = other.m_sNeedMirror;
    m_tMirrorEdgeAC = other.m_tMirrorEdgeAC;
    m_scalexP2 = other.m_scalexP2;

    m_st = other.m_st;
    m_tt = other.m_tt;
}

bool GmTriangleTransform::setTriangle(const GmTriangle & st, const GmTriangle & tt)
{
    m_st = st;
    m_tt = tt;
    bool ok = prepare();
    if (!ok) {
        m_st.clear();
        m_tt.clear();
        init();
    }
    return ok;
}

const GmTriangle & GmTriangleTransform::getSTriangle() const
{
    return m_st;
}

const GmTriangle & GmTriangleTransform::getTTriangle() const
{
    return m_tt;
}

bool GmTriangleTransform::isReady() const
{
    return m_isReady;
}

GmTriangleTransform GmTriangleTransform::inverted(bool *pok) const
{
    GmTriangleTransform ittf;
    if (m_isReady) {
        ittf.setTriangle(m_tt, m_st);
    }
    if (pok) *pok = ittf.isReady();
    return ittf;
}

bool GmTriangleTransform::operator==(const GmTriangleTransform & other) const
{
    if (m_isReady != other.m_isReady) return false;

    if (m_s2tEdgeAB != other.m_s2tEdgeAB) return false;
    if (m_tMirror != other.m_tMirror) return false;
    if (m_sPcMirror != other.m_sPcMirror) return false;
    if (m_sMirrorScale != other.m_sMirrorScale) return false;
    if (m_ttMirror != other.m_ttMirror) return false;
    if (m_sNeedMirror != other.m_sNeedMirror) return false;
    if (m_tMirrorEdgeAC != other.m_tMirrorEdgeAC) return false;
    if (m_scalexP2 != other.m_scalexP2) return false;

    if (m_st != other.m_st) return false;
    if (m_tt != other.m_tt) return false;

    return true;
}

bool GmTriangleTransform::operator!=(const GmTriangleTransform &other) const
{
    bool ok = !(*this == other);
    return ok;
}

GmTriangleTransform & GmTriangleTransform::operator=(const GmTriangleTransform &other)
{
    assign(other);
    return *this;
}

GmTriangle GmTriangleTransform::mapTriangle(const QTransform &tf, const GmTriangle & triangle) const
{
    GmTriangle t2;
    t2.setPa(tf.map(triangle.pa()));
    t2.setPb(tf.map(triangle.pb()));
    t2.setPc(tf.map(triangle.pc()));
    return t2;
}

bool GmTriangleTransform::prepare()
{
    m_isReady = false;
    m_s2tEdgeAB.reset(); m_tMirror.reset(); m_sPcMirror.reset(); m_sMirrorScale.reset();

    if (m_st.collinear() || m_tt.collinear()) return false;
    if (qFuzzyIsNull(m_st.area()) || qFuzzyIsNull(m_tt.area())) return false;

    // translate and scale to edge ab of triangle s
    QLineF sEdgeAB = m_st.edgeAB();
    QLineF tEdgeAB = m_tt.edgeAB();
    qreal scaleb12 = tEdgeAB.length() / sEdgeAB.length();
    m_s2tEdgeAB.translate(m_tt.pa().x(), m_tt.pa().y());
    m_s2tEdgeAB.scale(scaleb12, scaleb12);
    m_s2tEdgeAB.rotate(tEdgeAB.angleTo(sEdgeAB));
    m_s2tEdgeAB.translate(-m_st.pa().x(), -m_st.pa().y());

    // mirror to edge ab of triangle s
    m_tMirror.rotate(tEdgeAB.angle());
    m_tMirror.translate(-m_tt.pa().x(), -m_tt.pa().y());
    m_ttMirror = mapTriangle(m_tMirror, m_tt);
    GmTriangle stMirror = mapTriangle(m_tMirror, mapTriangle(m_s2tEdgeAB, m_st));

    // pc of triangle t mirror to triangle s
    qreal sumAbs = qAbs(m_ttMirror.pc().y() + stMirror.pc().y());
    qreal absSum = qAbs(m_ttMirror.pc().y()) + qAbs(stMirror.pc().y());
    m_sNeedMirror = (sumAbs < absSum);
    if (m_sNeedMirror) {
        m_sPcMirror.scale(1, -1);
        stMirror = mapTriangle(m_sPcMirror, stMirror);
    }

    // parallelogram transform
    // 1. transform to same angle between line13 and line12
    QLineF edgeABMirror = m_ttMirror.edgeAB();
    edgeABMirror.translate(0, stMirror.pc().y());
    QLineF edgeACMirror = m_ttMirror.edgeAC();
    QPointF intersectionPc;
    QLineF::IntersectType itype =  edgeABMirror.intersect(edgeACMirror, &intersectionPc);
    if (itype == QLineF::NoIntersection) return false;
    qreal scalexPcMirror = 1.0;
    if (intersectionPc.x() != 0 && stMirror.pc().x() != 0) {
        scalexPcMirror = intersectionPc.x() / stMirror.pc().x();
    }
    qreal scalexEdgetABMirror = 1.0;
    if (m_ttMirror.pc().x() != 0 && intersectionPc.x() != 0) {
        scalexEdgetABMirror = m_ttMirror.pc().x() / intersectionPc.x();
    }
    qreal scaleyEdgeABMirror = 1.0;
    if (m_ttMirror.pc().y() != 0 && intersectionPc.y() != 0) {
        scaleyEdgeABMirror = m_ttMirror.pc().y() / intersectionPc.y();
    }
    m_sMirrorScale.scale(scalexEdgetABMirror, scaleyEdgeABMirror);
    m_sMirrorScale.scale(scalexPcMirror, 1);
    GmTriangle stMirrorScale = mapTriangle(m_sMirrorScale, stMirror);
    m_tMirrorEdgeAC = m_ttMirror.edgeAC();
    if (m_ttMirror.pb().x() != 0 && stMirrorScale.pb().x() != 0) {
        m_scalexP2 = m_ttMirror.pb().x() / stMirrorScale.pb().x();
    } else {
        m_scalexP2 = 1.0;
    }

    m_isReady = true;
    return m_isReady;
}

GmTriangle GmTriangleTransform::map(const GmTriangle & st, bool *pok) const
{
    if (pok) *pok = m_isReady;
    if (!m_isReady) return GmTriangle();

    GmTriangle tt;
    tt.setPa(map(st.pa()));
    tt.setPb(map(st.pb()));
    tt.setPc(map(st.pc()));
    return tt;
}

QPointF GmTriangleTransform::map(const QPointF & sp, bool *pok) const
{
    if (pok) *pok = m_isReady;
    if (!m_isReady) return QPointF();

    QPointF spMirror = m_tMirror.map(m_s2tEdgeAB.map(sp));
    if (m_sNeedMirror) spMirror = m_sPcMirror.map(spMirror);

    QPointF p = m_sMirrorScale.map(spMirror);
    qreal dx = m_tMirrorEdgeAC.pointAt(p.y() / m_ttMirror.pc().y()).x();
    p.rx() = (p.x() - dx) * m_scalexP2 + dx;

    QPointF tp = m_tMirror.inverted().map(p);
    return tp;
}

QLineF GmTriangleTransform::map(const QLineF & sl, bool *pok) const
{
    if (pok) *pok = m_isReady;
    if (!m_isReady) return QLineF();

    QLineF tl(map(sl.p1()), map(sl.p2()));
    return tl;
}

QPolygonF GmTriangleTransform::map(const QPolygonF & spoly, bool *pok) const
{
    if (pok) *pok = m_isReady;
    if (!m_isReady) return QPolygonF();

    QPolygonF tpoly(spoly.size());
    for (int i = 0; i < spoly.size(); i++) {
        QPointF tp = map(spoly.at(i));
        tpoly[i] = tp;
    }
    return tpoly;
}

QPolygonF GmTriangleTransform::mapRect(const QRectF &srect, bool *pok) const
{
    if (pok) *pok = m_isReady;
    if (!m_isReady) return QPolygonF();

    QPolygonF tpoly = map(QPolygonF(srect));
    return tpoly;
}

QTransform GmTriangleTransform::getImageTransform() const
{
    if (!m_isReady) return QTransform();

    QTransform tf = m_s2tEdgeAB * m_tMirror;
    if (m_sNeedMirror) tf = tf * m_sPcMirror;
    tf = tf * m_sMirrorScale;

    QPointF sp(0, 0);
    QPointF tp = tf.map(sp);
    qreal dx = m_tMirrorEdgeAC.pointAt(tp.y() / m_ttMirror.pc().y()).x();
    QTransform tfx;
    tfx.translate(dx, 0);
    tfx.scale(m_scalexP2, 1.0);
    tfx.translate(-dx, 0);

    tf = tf * tfx;
    tf  = tf * m_tMirror.inverted();

    return tf;
}

QPixmap GmTriangleTransform::map(const QPixmap &pixmap, bool *pok) const
{
    if (pok) *pok = m_isReady;
    if (!m_isReady) return QPixmap();

    QTransform tf = getImageTransform();
    QPixmap pixmaptf = pixmap.transformed(tf, Qt::SmoothTransformation);
    return pixmaptf;
}

QImage GmTriangleTransform::map(const QImage &image, bool *pok) const
{
    if (pok) *pok = m_isReady;
    if (!m_isReady) return QImage();

    QTransform tf = getImageTransform();
    QImage imagetf = image.transformed(tf, Qt::SmoothTransformation);
    return imagetf;
}


#include <QtTest>
#include "RGWebMercatorProjection.h"
#include "RGGoogleMapBounds.h"
#include "RGOsMapBounds.h"

class TestRGWebMercatorProjection : public QObject
{
    Q_OBJECT

private slots:
    void testCenterProjection()
    {
        QPoint topLeft(4334718, 2766702);
        QPoint bottomRight(4335998, 2767422);
        QGeoCoordinate ne(52.110339581205992, 6.0808658203125088);
        QGeoCoordinate sw(52.09135911522506, 6.0259341796875088);
        QGeoCoordinate ctr(52.1008, 6.0534);
        int zoom = 15;

        RGOsMapBounds osmBounds(ctr, zoom);
        RGWebMercatorProjection projOSM(osmBounds, 1280, 720);
        const RGGoogleMapBounds& gmb = projOSM.googleMapBounds();

        RGGoogleMapBounds gmBounds(ne, sw, zoom);
        RGWebMercatorProjection projGM(gmBounds);

        QCOMPARE_EQ(projGM.convert(ne), QPoint(1280, 0));
        QCOMPARE_EQ(projGM.convert(sw), QPoint(0, 720));

        QCOMPARE_EQ(gmb.getNE().latitude(), gmBounds.getNE().latitude());
        QCOMPARE_EQ(gmb.getNE().longitude(), gmBounds.getNE().longitude());
        QCOMPARE_EQ(gmb.getSW().longitude(), gmBounds.getSW().longitude());
        QCOMPARE_EQ(gmb.getSW().longitude(), gmBounds.getSW().longitude());
    }
};

QTEST_MAIN(TestRGWebMercatorProjection)
#include "tst_rgwebmercatorprojection.moc"


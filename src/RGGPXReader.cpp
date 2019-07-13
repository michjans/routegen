#include "RGGPXReader.h"

#include <QXmlStreamReader>
#include <QDebug>
#include <QString>
#include <QFile>

RGGPXReader::RGGPXReader(RGRoute *route, QObject *parent)
    : RGReader(route, parent)
{

}

bool RGGPXReader::readFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "File open error:" << file.errorString();
        return false;
    }
    QXmlStreamReader inputStream(&file);
    m_route->clearPath(true);
    QList<QGeoCoordinate> geoCoordinates;
    while (!inputStream.atEnd() && !inputStream.hasError())
    {
        inputStream.readNext();
        if (inputStream.isStartElement()) {
            QString name = inputStream.name().toString();
            qDebug() << "elementName:" << name;
            //if (name == "wpt")
            if (name == "trkpt")
            {
                geoCoordinates.append(QGeoCoordinate(inputStream.attributes().value("lat").toFloat(), inputStream.attributes().value("lon").toFloat()));
                qDebug() << "lon:" << inputStream.attributes().value("lon").toFloat() << "lat:" << inputStream.attributes().value("lat").toFloat();
            }
        }
    }
    m_route->setGeoCoordinates(geoCoordinates);
    return true;
}




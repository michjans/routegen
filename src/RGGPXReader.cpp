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

    //First collect available data from GPX file
    QStringList trackNames;
    //QStringList routes;
    while (!inputStream.atEnd() && !inputStream.hasError())
    {
        if (inputStream.readNext() == QXmlStreamReader::StartElement && inputStream.name().toString() == "trk")
        {
            if (inputStream.readNextStartElement() && inputStream.name().toString() == "name")
            {
                trackNames << inputStream.readElementText();
            }
        }
    }

    qDebug() << "trackNames:" << trackNames;

    QString selectedTrack = "ACTIVE LOG063900";

    QList<QGeoCoordinate> geoCoordinates;
    file.reset();
    inputStream.clear();
    inputStream.setDevice(&file);
    bool selectionFound = false;
    while (!inputStream.atEnd() && !inputStream.hasError())
    {
        if (selectionFound)
        {
            inputStream.readNext();
            if ( inputStream.isStartElement() && inputStream.name().toString() == "trkpt")
            {
                geoCoordinates.append(QGeoCoordinate(inputStream.attributes().value("lat").toFloat(), inputStream.attributes().value("lon").toFloat()));
                qDebug() << "lon:" << inputStream.attributes().value("lon").toFloat() << "lat:" << inputStream.attributes().value("lat").toFloat();
            }
            else if (inputStream.isEndElement() && inputStream.name().toString() == "trk")
            {
                //End of track (currently all segments within a track are connected together as one track, if we
                //would want to read just one segment, we should check for end element "trkseg" instead)
                m_route->setGeoCoordinates(geoCoordinates);
                break;
            }
        }
        else
        {
            if (inputStream.readNext() == QXmlStreamReader::StartElement && inputStream.name().toString() == "trk")
            {
                if (inputStream.readNextStartElement() && inputStream.name().toString() == "name")
                {
                    if (inputStream.readElementText() == selectedTrack)
                    {
                        selectionFound = true;
                    }
                }
            }
        }
    }


    return selectionFound;

#if 0
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
#endif
}




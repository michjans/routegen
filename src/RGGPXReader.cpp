#include "RGGPXReader.h"

#include <QXmlStreamReader>
#include <QDebug>
#include <QString>
#include <QFile>
#include <QInputDialog>
#include <QMessageBox>
#include <QProgressDialog>

RGGPXReader::RGGPXReader(RGRoute *route, RGMap *map, QWidget *parent)
    : RGReader(route, map, parent)
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

    int progVal = 10;
    QProgressDialog progress("Analyzig gpx...", "GPX import", 0, 100, m_parentWidget);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration (100);
    progress.setValue(progVal);


    //First collect available data from GPX file
    QStringList trackNames;
    //QStringList routes;
    while (!inputStream.atEnd() && !inputStream.hasError())
    {
        if (inputStream.readNext() == QXmlStreamReader::StartElement && inputStream.name().toString() == "trk")
        {
            progress.setValue(progVal++ % 100);
            if (progress.wasCanceled())
            {
                return false;
            }
            if (inputStream.readNextStartElement() && inputStream.name().toString() == "name")
            {
                trackNames << inputStream.readElementText();
            }
        }
    }
    inputStream.clear();
    progress.reset();

    qDebug() << "trackNames:" << trackNames;

    QString selectedTrack;
    if (trackNames.empty())
    {
        QMessageBox::warning (m_parentWidget, "No tracks found", "No tracks found in GPX file!");
        return false;
    }
    else if (trackNames.size() > 1)
    {
        bool ok;
        selectedTrack = QInputDialog::getItem(m_parentWidget, "Track selectionn", "Select track to import", trackNames, 0, false, &ok);
        if (!ok)
        {
            return false;
        }
    }
    else
    {
        selectedTrack = trackNames.first();
    }

    progVal = 10;
    progress.setRange(0, 200);
    progress.setValue(progVal);
    progress.setLabelText(QStringLiteral("Importing route..."));

    m_route->clearPath(true);
    QList<QGeoCoordinate> geoCoordinates;
    file.reset();
    inputStream.setDevice(&file);
    bool selectionFound = false;
    while (!selectedTrack.isEmpty() && !inputStream.atEnd() && !inputStream.hasError())
    {
        if (selectionFound)
        {
            inputStream.readNext();
            if ( inputStream.isStartElement() && inputStream.name().toString() == "trkpt")
            {
                progress.setValue(progVal++ % 100);
                if (progress.wasCanceled())
                {
                    return false;
                }

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
}




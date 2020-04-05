#include "RGGPXReader.h"
#include "GPXSelectionDialog.h"

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

    QProgressDialog progress("Analyzig gpx...", "GPX import", 0, 100, m_parentWidget);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration (100);

    //First collect available data from GPX file
    QStringList routeNames, trackNames;
    if (collectNames(inputStream, progress, routeNames, trackNames) == false)
    {
        return false;
    }

    qDebug() << "trackNames:" << trackNames;
    qDebug() << "routeNames:" << routeNames;

    QString selectedRoute;
    QString waypointTagName;
    QString routeTagName; //trk or rte
    if (trackNames.empty() && routeNames.empty())
    {
        QMessageBox::warning (m_parentWidget, "No routes or tracks found", "No routes or tracks found in GPX file!");
        return false;
    }
    else
    {
        GPXSelectionDialog gpxSel(routeNames, trackNames);
        if (gpxSel.exec() == QDialog::Rejected)
        {
            return false;
        }

        GPXSelectionDialog::GPXItemType gpxItemType;
        selectedRoute = gpxSel.selectedItem(gpxItemType);
        switch (gpxItemType)
        {
        case GPXSelectionDialog::ITEM_TYPE_ROUTE:
            routeTagName = "rte";
            waypointTagName = "rtept";
            break;
        case GPXSelectionDialog::ITEM_TYPE_TRACK:
            routeTagName = "trk";
            waypointTagName = "trkpt";
            break;
        case GPXSelectionDialog::ITEM_TYPE_NONE:
        default:
            return false;
        }
    }

    int progVal = 10;
    progress.setRange(0, 200);
    progress.setLabelText(QStringLiteral("Importing route..."));

    m_route->clearPath(true);
    QList<QGeoCoordinate> geoCoordinates;
    file.reset();
    inputStream.setDevice(&file);
    bool selectionFound = false;
    while (!selectedRoute.isEmpty() && !inputStream.atEnd() && !inputStream.hasError())
    {
        if (selectionFound)
        {
            inputStream.readNext();
            if ( inputStream.isStartElement() && inputStream.name().toString() == waypointTagName)
            {
                progress.setValue(progVal++ % 100);
                if (progress.wasCanceled())
                {
                    return false;
                }

                geoCoordinates.append(QGeoCoordinate(inputStream.attributes().value("lat").toFloat(), inputStream.attributes().value("lon").toFloat()));
                qDebug() << "lon:" << inputStream.attributes().value("lon").toFloat() << "lat:" << inputStream.attributes().value("lat").toFloat();
            }
            else if (inputStream.isEndElement() && inputStream.name().toString() == routeTagName)
            {
                //End of track (currently all segments within a track are connected together as one track, if we
                //would want to read just one segment, we should check for end element "trkseg" instead)
                m_route->setGeoCoordinates(geoCoordinates);
                break;
            }
        }
        else
        {
            if (inputStream.readNext() == QXmlStreamReader::StartElement && inputStream.name().toString() == routeTagName)
            {
                if (inputStream.readNextStartElement() && inputStream.name().toString() == "name")
                {
                    if (inputStream.readElementText() == selectedRoute)
                    {
                        selectionFound = true;
                    }
                }
            }
        }
    }


    return selectionFound;
}

bool RGGPXReader::collectNames(QXmlStreamReader &inputStream, QProgressDialog &pd, QStringList &routeNames, QStringList &trackNames)
{
    int progVal = 10;
    pd.setValue(progVal);
    while (!inputStream.atEnd() && !inputStream.hasError())
    {
        if (inputStream.readNext() == QXmlStreamReader::StartElement)
        {
            pd.setValue(progVal++ % 100);
            if (pd.wasCanceled())
            {
                return false;
            }
            QStringList *namesList = nullptr;
            if (inputStream.name().toString() == "rte")
            {
                namesList = &routeNames;
            }
            else if (inputStream.name().toString() == "trk")
            {
                namesList = &trackNames;
            }
            if (inputStream.readNextStartElement() && inputStream.name().toString() == "name" && namesList)
            {
                *namesList << inputStream.readElementText();
            }
        }
    }
    inputStream.clear();
    pd.reset();

    return true;
}




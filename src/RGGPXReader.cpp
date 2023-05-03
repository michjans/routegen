#include "RGGPXReader.h"
#include "GPXSelectionDialog.h"

#include <QDebug>
#include <QFile>
#include <QInputDialog>
#include <QMessageBox>
#include <QProgressDialog>
#include <QString>
#include <QXmlStreamReader>

namespace
{
const QString rteTagName = QStringLiteral("rte");
const QString rteptTagName = QStringLiteral("rtept");
const QString trkTagName = QStringLiteral("trk");
const QString trkptTagName = QStringLiteral("trkpt");
} // namespace

RGGPXReader::RGGPXReader(RGRoute* route, RGMap* map, QWidget* parent)
    : RGReader(route, map, parent)
{
}

bool RGGPXReader::readFile(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "File open error:" << file.errorString();
        return false;
    }
    QXmlStreamReader inputStream(&file);

    QProgressDialog progress(tr("Analyzig gpx..."), tr("GPX import"), 0, 100, m_parentWidget);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(100);

    //First collect available data from GPX file
    QStringList routeNames, trackNames;
    if (collectNames(inputStream, progress, routeNames, trackNames) == false)
    {
        return false;
    }

    qDebug() << "trackNames:" << trackNames;
    qDebug() << "routeNames:" << routeNames;

    int selectedRouteItemIdx = -1;
    QString waypointTagName;
    QString routeTagName; //trk or rte
    if (trackNames.empty() && routeNames.empty())
    {
        QMessageBox::warning(m_parentWidget, tr("No routes or tracks found"), tr("No routes or tracks found in GPX file!"));
        return false;
    }
    else if (trackNames.empty() && routeNames.size() == 1)
    {
        //Only one route found, just pick it
        routeTagName = rteTagName;
        waypointTagName = rteptTagName;
        selectedRouteItemIdx = 0;
    }
    else if (trackNames.size() == 1 && routeNames.empty())
    {
        //Only one track found, just pick it
        routeTagName = trkTagName;
        waypointTagName = trkptTagName;
        selectedRouteItemIdx = 0;
    }
    else
    {
        GPXSelectionDialog gpxSel(routeNames, trackNames);
        if (gpxSel.exec() == QDialog::Rejected)
        {
            return false;
        }

        GPXSelectionDialog::GPXItemType gpxItemType;
        selectedRouteItemIdx = gpxSel.selectedItem(gpxItemType);
        switch (gpxItemType)
        {
        case GPXSelectionDialog::ITEM_TYPE_ROUTE:
            routeTagName = rteTagName;
            waypointTagName = rteptTagName;
            break;
        case GPXSelectionDialog::ITEM_TYPE_TRACK:
            routeTagName = trkTagName;
            waypointTagName = trkptTagName;
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
    int currentRouteIdx = 0;
    while (selectedRouteItemIdx >= 0 && !inputStream.atEnd() && !inputStream.hasError())
    {
        if (selectionFound)
        {
            inputStream.readNext();
            if (inputStream.isStartElement() && inputStream.name().toString() == waypointTagName)
            {
                progress.setValue(progVal++ % 100);
                if (progress.wasCanceled())
                {
                    return false;
                }

                geoCoordinates.append(QGeoCoordinate(inputStream.attributes().value(QStringLiteral("lat")).toFloat(),
                                                     inputStream.attributes().value(QStringLiteral("lon")).toFloat()));
                qDebug() << "lon:" << inputStream.attributes().value(QStringLiteral("lon")).toFloat()
                         << "lat:" << inputStream.attributes().value(QStringLiteral("lat")).toFloat();
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
                if (currentRouteIdx++ == selectedRouteItemIdx)
                {
                    selectionFound = true;
                }
            }
        }
    }

    return selectionFound;
}

bool RGGPXReader::collectNames(QXmlStreamReader& inputStream, QProgressDialog& pd, QStringList& routeNames, QStringList& trackNames)
{
    int progVal = 10;
    pd.setValue(progVal);
    QStringList* namesList = nullptr;
    QString name;
    while (!inputStream.atEnd() && !inputStream.hasError())
    {
        QXmlStreamReader::TokenType nextTokenType = inputStream.readNext();
        if (nextTokenType == QXmlStreamReader::StartElement)
        {
            if (inputStream.name().toString() == rteTagName)
            {
                namesList = &routeNames;
            }
            else if (inputStream.name().toString() == trkTagName)
            {
                namesList = &trackNames;
            }
            else if (namesList && inputStream.name().toString() == QLatin1String("name"))
            {
                name = inputStream.readElementText();
            }
        }
        else if (nextTokenType == QXmlStreamReader::EndElement && namesList != nullptr &&
                 (inputStream.name().toString() == rteTagName || inputStream.name().toString() == trkTagName))
        {
            if (name.isEmpty())
            {
                //Track or route without name, generate one by number
                int nrIndex = namesList->size() + 1;
                name = QString::number(nrIndex);
            }
            if (pd.wasCanceled())
            {
                return false;
            }
            pd.setValue(progVal++ % 100);
            *namesList << name;
            namesList = nullptr;
            name.clear();
        }
    }
    inputStream.clear();
    pd.reset();

    return true;
}

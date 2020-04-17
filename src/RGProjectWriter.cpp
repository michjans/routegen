#include "RGProjectWriter.h"

RGProjectWriter::RGProjectWriter(RGRoute *route, RGMap *map, QObject *parent)
    : RGWriter(route, map, parent)
{

}

bool RGProjectWriter::writeFile(const QString &fileName)
{
    QFile saveFile(fileName);

    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open save file.");
        return false;
    }

    QJsonObject rgProjObject;

    m_map->write(rgProjObject);
    m_route->write(rgProjObject);

    QJsonDocument saveDoc(rgProjObject);
    return saveFile.write(saveDoc.toJson()) > -1;
}

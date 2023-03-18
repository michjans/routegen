#include "RGProjectReader.h"

RGProjectReader::RGProjectReader(RGRoute* route, RGMap* map, QWidget* parent)
    : RGReader(route, map, parent)
{
}

bool RGProjectReader::readFile(const QString& fileName)
{
    QFile loadFile(fileName);

    if (!loadFile.open(QIODevice::ReadOnly))
    {
        qWarning("Couldn't open save file.");
        return false;
    }

    QByteArray saveData = loadFile.readAll();

    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));

    m_route->clearPath(true);
    m_map->read(loadDoc.object());
    m_route->read(loadDoc.object());

    return true;
}

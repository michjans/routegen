#include "RGProjectReader.h"

RGProjectReader::RGProjectReader(RGRoute *route, QObject *parent)
    : RGReader(route, parent)
{

}

bool RGProjectReader::readFile(const QString &fileName)
{
    return false;
}

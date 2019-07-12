#include "RGProjectWriter.h"

RGProjectWriter::RGProjectWriter(RGRoute *route, QObject *parent)
    : RGWriter(route, parent)
{

}

bool RGProjectWriter::writeFile(const QString &fileName)
{
    return false;
}

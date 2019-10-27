#include "RGWriter.h"

RGWriter::RGWriter(RGRoute *route, RGMap *map, QObject *parent)
    : QObject(parent),
      m_route(route),
      m_map(map)
{

}

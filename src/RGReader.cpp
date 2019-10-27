#include "RGReader.h"

RGReader::RGReader(RGRoute *route, RGMap *map, QWidget *parent)
    : QObject(parent),
      m_route(route),
      m_map(map),
      m_parentWidget(parent)
{

}

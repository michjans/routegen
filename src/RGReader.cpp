#include "RGReader.h"

RGReader::RGReader(RGRoute *route, QWidget *parent)
    : QObject(parent),
      m_parentWidget(parent),
      m_route(route)
{

}

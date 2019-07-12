#include "RGWriter.h"

RGWriter::RGWriter(RGRoute *route, QObject *parent)
    : QObject(parent),
      m_route(route)
{

}

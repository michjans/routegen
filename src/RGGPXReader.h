#ifndef RGGPXREADER_H
#define RGGPXREADER_H

#include "RGReader.h"

class RGGPXReader : public RGReader
{
public:
    RGGPXReader(RGRoute *route, QObject *parent = nullptr);

    virtual bool readFile(const QString &fileName);
};

#endif // RGGPXREADER_H

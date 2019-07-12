#ifndef RGGPXREADER_H
#define RGGPXREADER_H

#include "RGReader.h"

class RGGPXReader : public RGReader
{
public:
    RGGPXReader(RGRoute *route, QObject *parent = nullptr);

    bool readFile(const QString &fileName) override;
};

#endif // RGGPXREADER_H

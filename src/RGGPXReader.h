#ifndef RGGPXREADER_H
#define RGGPXREADER_H

#include "RGReader.h"

class RGGPXReader : public RGReader
{
public:
    RGGPXReader(RGRoute *route, QWidget *parent = nullptr);

    bool readFile(const QString &fileName) override;
};

#endif // RGGPXREADER_H

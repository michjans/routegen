#ifndef RGPROJECTREADER_H
#define RGPROJECTREADER_H

#include "RGReader.h"


class RGProjectReader : public RGReader
{
    Q_OBJECT
public:
    explicit RGProjectReader(RGRoute *route, QObject *parent = 0);

    bool readFile(const QString &fileName) override;

signals:

public slots:
};

#endif // RGPROJECTREADER_H

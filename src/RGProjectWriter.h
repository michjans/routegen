#ifndef RGPROJECTWRITER_H
#define RGPROJECTWRITER_H

#include <RGWriter.h>

class RGProjectWriter : public RGWriter
{
    Q_OBJECT
public:
    explicit RGProjectWriter(RGRoute *route, RGMap *map, QObject *parent = nullptr);

    bool writeFile(const QString &fileName) override;

signals:

public slots:
};

#endif // RGPROJECTWRITER_H

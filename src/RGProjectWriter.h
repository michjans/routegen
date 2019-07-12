#ifndef RGPROJECTWRITER_H
#define RGPROJECTWRITER_H

#include <RGWriter.h>

class RGProjectWriter : public RGWriter
{
    Q_OBJECT
public:
    explicit RGProjectWriter(RGRoute *route, QObject *parent = 0);

    bool writeFile(const QString &fileName) override;

signals:

public slots:
};

#endif // RGPROJECTWRITER_H

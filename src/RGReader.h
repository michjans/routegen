#ifndef RGREADER_H
#define RGREADER_H

#include <QObject>
#include <QString>

#include "RGRoute.h"

class RGReader : public QObject
{
    Q_OBJECT
public:
    explicit RGReader(RGRoute *route, QObject *parent = nullptr);

    virtual bool readFile(const QString &fileName) = 0;

signals:

public slots:

protected:
    RGRoute *m_route;
};

#endif // RGREADER_H

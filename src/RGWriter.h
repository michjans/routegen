#ifndef RGWRITER_H
#define RGWRITER_H

#include <QObject>
#include <QString>

#include "RGRoute.h"

class RGWriter : public QObject
{
    Q_OBJECT
public:
    explicit RGWriter(RGRoute *route, QObject *parent = nullptr);

    virtual bool writeFile(const QString &fileName) = 0;

signals:

public slots:

protected:
    RGRoute *m_route;
};

#endif // RGWRITER_H

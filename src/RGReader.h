#ifndef RGREADER_H
#define RGREADER_H

#include <QObject>
#include <QString>
#include <QWidget>

#include "RGRoute.h"

class RGReader : public QObject
{
    Q_OBJECT
public:
    explicit RGReader(RGRoute *route, QWidget *parent = nullptr);

    virtual bool readFile(const QString &fileName) = 0;

signals:

public slots:

protected:
    RGRoute *m_route;
    QWidget *m_parentWidget;
};

#endif // RGREADER_H

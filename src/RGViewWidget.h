#ifndef RGVIEWWIDGET_H
#define RGVIEWWIDGET_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsTextItem>

class RGViewWidget : public QGraphicsView
{
    Q_OBJECT
public:
    explicit RGViewWidget(QWidget *parent = 0);
    void addItem(QGraphicsItem*);

signals:

public slots:
    void loadImage(const QPixmap &pm);

private:
    QGraphicsScene *mScene;
    QGraphicsTextItem *mWelcomeText;
    QPixmap mImage;

};

#endif // RGVIEWWIDGET_H

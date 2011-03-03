#ifndef RGVIEWWIDGET_H
#define RGVIEWWIDGET_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QTimer>
#include "RGRoute.h"

class RGViewWidget : public QGraphicsView
{
  Q_OBJECT
public:
  explicit RGViewWidget(QWidget *parent = 0);
  QSize   sizeHint () const;
  void addRoute(RGRoute*);
  bool generateMovie(const QString &dirName, const QString &filePrefix, QStringList &generatedBMPs);



signals:
  void playbackStopped(bool);

public slots:
  void loadImage(const QPixmap &pm);
  void play();
  void stop();

private slots:
  void playTimerEvent();

private:
  QGraphicsScene *mScene;
  QGraphicsTextItem *mWelcomeText;
  QPixmap mImage;
  RGRoute *mRoute;
  QTimer        *mPlayTimer;
  int           mTimerCounter;

};

#endif // RGVIEWWIDGET_H

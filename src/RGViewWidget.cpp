#include "RGViewWidget.h"

extern const QString applicationName;

RGViewWidget::RGViewWidget(QWidget *parent) :
    QGraphicsView(parent)
{
  mScene = new QGraphicsScene;
  this->setScene(mScene);

  //init text
  mWelcomeText = mScene->addText(applicationName,QFont("Arial", 30));
  mWelcomeText->setDefaultTextColor(Qt::blue);
}

void RGViewWidget::addItem(QGraphicsItem *item)
{
  mScene->addItem(item);
}

void RGViewWidget::loadImage(const QPixmap &pm)
{
  mImage = pm;
  QGraphicsPixmapItem *image;
  image = mScene->addPixmap(mImage);
  delete mWelcomeText;
}

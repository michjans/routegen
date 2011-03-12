#include "RGViewWidget.h"
#include "RGSettings.h"

//The number of field with of the numbers before the generated file names,
//e.g. if 5 then always 5 digits, e.g. map00001.bmp, etc.
#define FILE_NUMBER_FIELD_WIDTH 5

extern const QString applicationName;

RGViewWidget::RGViewWidget(QWidget *parent) :
    QGraphicsView(parent),
    mPlayTimer(NULL),
    mTimerCounter(0)
{
  mScene = new QGraphicsScene;
  this->setScene(mScene);
  this->setRenderHint(QPainter::Antialiasing);
  this->setRenderHint(QPainter::SmoothPixmapTransform);

  //init text
  mWelcomeText = mScene->addText(applicationName,QFont("Arial", 30));
  mWelcomeText->setDefaultTextColor(Qt::blue);

  //timer
  if (mPlayTimer == NULL) {
    mPlayTimer = new QTimer(this);
    QObject::connect(mPlayTimer, SIGNAL(timeout()), this, SLOT(playTimerEvent()));
  }
}

QSize RGViewWidget::sizeHint () const
{
  return QSize(mScene->width(),mScene->height());
}

void RGViewWidget::addRoute(RGRoute *item)
{
  mRoute=item;
  mScene->addItem(mRoute);
  QObject::connect(this,SIGNAL(sceneRectChanged()),mRoute,SIGNAL(sceneRectChanged()));
}

void RGViewWidget::play()
{
  mTimerCounter = 0;
  mRoute->setCurrentFrame(0);
  mRoute->on_playbackChanged(true);
  mPlayTimer->start((1.0 / (double) RGSettings::getFps()) * 1000);
}

void RGViewWidget::stop()
{
  if (mPlayTimer == NULL || !mPlayTimer->isActive()) return;
  //Finished
  mPlayTimer->stop();
  mRoute->on_playbackChanged(false);
  emit playbackStopped(true);
}

bool RGViewWidget::generateMovie(const QString &dirName, const QString &filePrefix, QStringList &generatedBMPs)
{
  generatedBMPs.clear();
  bool generationOK = true;

  QProgressDialog progress("Generating files...", "Abort", 0, mRoute->countFrames(), this);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration (100);

  QString fileName;
  bool result;

  for (mTimerCounter=0; mTimerCounter < mRoute->countFrames() && generationOK; mTimerCounter++){
    progress.setValue(mTimerCounter);
    mRoute->setCurrentFrame(mTimerCounter);
    QPixmap pixmap(mScene->width(), mScene->height());
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    mScene->render(&painter);
    QString postFix = QString("%1.bmp").arg(mTimerCounter, FILE_NUMBER_FIELD_WIDTH, 10, QChar('0'));
    fileName = dirName + "/" + filePrefix + postFix;
    result = pixmap.save (fileName);
    painter.end();
    if (!result){
      QMessageBox::critical (this, "Oops", "Problems saving file " + fileName);
      break;
    }
    else{
      generatedBMPs.append(fileName);
    }
    if (progress.wasCanceled()) break;
  }
  generationOK = (mTimerCounter == mRoute->countFrames());

  return generationOK;
}

void RGViewWidget::loadImage(const QPixmap &pm)
{
  if(mWelcomeText!=NULL){
    delete mWelcomeText;
    mWelcomeText=NULL;
  }
  mScene->setBackgroundBrush(pm);
  this->setCacheMode(QGraphicsView::CacheBackground);
  mScene->setSceneRect(0,0,pm.width(),pm.height());
  emit sceneRectChanged();
  this->setFrameShape(QFrame::NoFrame);
  viewport()->setMaximumSize(pm.size());
  this->setAlignment((Qt::AlignLeft | Qt::AlignTop));
  updateGeometry();

}

void RGViewWidget::playTimerEvent()
{
  ++mTimerCounter;
  if (mTimerCounter < mRoute->countFrames()){
    mRoute->setCurrentFrame(mTimerCounter);
  }
  else
  {
    //Finished
    this->stop();
  }

}



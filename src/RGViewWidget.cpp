#include "RGViewWidget.h"
#include "RGSettings.h"

//The number of field with of the numbers before the generated file names,
//e.g. if 5 then always 5 digits, e.g. map00001.bmp, etc.
#define FILE_NUMBER_FIELD_WIDTH 5

extern const QString applicationName;

RGViewWidget::RGViewWidget(QWidget *parent) :
    QGraphicsView(parent),
    mEndFrame(1),
    mPlayTimer(NULL),
    mTimerCounter(0)
{
  mScene = new QGraphicsScene;
  this->setScene(mScene);

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

void RGViewWidget::addRoute(RGRoute2 *item)
{
  mRoute=item;
  mScene->addItem(mRoute);
}

void RGViewWidget::play()
{
  mEndFrame=mRoute->countFrames();
  mTimerCounter = 0;
  int time=(1.0 / 25) * 1000;
  mPlayTimer->start((1.0 / (double) RGSettings::getFps()) * 1000);
}

void RGViewWidget::stop()
{
  if (mPlayTimer == NULL || !mPlayTimer->isActive()) return;
  //Finished
  mPlayTimer->stop();
  //emit busy(false);
}

bool RGViewWidget::generateMovie(const QString &dirName, const QString &filePrefix, QStringList &generatedBMPs)
{
  //Disable draw mode automatically
  //if (mInDrawMode)
  //  endDrawMode();

  generatedBMPs.clear();
  //Only usefull when route has more than MIN_PATH_LENGTH points
  /*f (mRgr->userPointCount() < MIN_PATH_LENGTH) {
    QMessageBox::warning (this, "Route too short", "Sorry, this route is too short to generate a valid route.");
    return false;
  }*/

  bool generationOK = true;
  //emit busy(true);

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

  //emit busy(false);

  return generationOK;
}

void RGViewWidget::loadImage(const QPixmap &pm)
{
  if(mWelcomeText!=NULL){
    delete mWelcomeText;
    mWelcomeText=NULL;
  }
  //mImage = pm;
  //QGraphicsPixmapItem *image;
  //image = mScene->addPixmap(mImage);
  mScene->setBackgroundBrush(pm);
  mScene->setSceneRect(0,0,pm.width(),pm.height());
  this->setSceneRect(0,0,pm.width(),pm.height());
  mRoute->sceneRectChanged(QRectF(0,0,pm.width(),pm.height()));
  this->setMaximumSize(pm.size());
  //update();
  updateGeometry();
}

void RGViewWidget::playTimerEvent()
{
  if (mTimerCounter < mEndFrame-1){
    ++mTimerCounter;
  }
  else
  {
    //Finished
    mPlayTimer->stop();
    //emit busy(false);
    qDebug()<<"Last update of mTimerCounter"<<mTimerCounter ;
  }
  mRoute->setCurrentFrame(mTimerCounter);
}

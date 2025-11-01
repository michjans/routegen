/*
    Copyright (C) 2010-2011  Michiel Jansen

  This file is part of Route Generator.

    Route Generator is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "RGGoogleMap.h"
#include "RGSettings.h"

#include <QUrlQuery>
#include <QtWidgets>

#include <QGeoCoordinate>
#include <QGeoRectangle>
#include <QTimer>
#include <QWebEnginePage>
#include <QWebEngineScript>

#if 0
//Debug only
class myWebPage : public QWebEnginePage
{
public:
    myWebPage(QObject* parent)
        : QWebEnginePage(parent)
    {
    }

    ~myWebPage() override
    {
    }

protected:
    void javaScriptConsoleMessage(QWebEnginePage::JavaScriptConsoleMessageLevel level, const QString& message, int lineNumber, const QString& sourceID) override
    {
        QString levelString;
        switch (level)
        {
        case QWebEnginePage::InfoMessageLevel:
            levelString = "info";
            break;
        case QWebEnginePage::WarningMessageLevel:
            levelString = "warning";
            break;
        case QWebEnginePage::ErrorMessageLevel:
            levelString = "error";
            break;
        default:
            levelString = "unknown";
            break;
        }
        qDebug() << "Javascript " << levelString << ": " << sourceID << ":" << lineNumber << ":" << message;
    }
};
#endif

namespace
{
// Example: paste these from gen_obf.cpp API key generator (only example values shown)
//          real key should be pasted here before deployment!
static const unsigned short obf_key[] = { 1, 2, 3, 4, 5, 6 };
static const size_t obf_key_len = sizeof(obf_key)/sizeof(obf_key[0]);

// Use some kind of simple obfuscation, just to prevent that the API key is
// stored here in plain sight.
QString getDecodedApiKeyFromObf() {
    QByteArray ba;
    ba.reserve(static_cast<int>(obf_key_len));

    for (size_t i = 0; i < obf_key_len; ++i) {
        unsigned short val = obf_key[obf_key_len - 1 - i];
        unsigned char ch = static_cast<unsigned char>(val / 42);
        ba.append(char(ch));
    }

    QString key = QString::fromUtf8(ba);
    ba.fill('\0');
    return key;
}

}

RGGoogleMap::RGGoogleMap(QWidget* parent, const QGeoPath& geoPath)
    : QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::Dialog | Qt::WindowMaximizeButtonHint),
      m_geoPath(geoPath)
{
    ui.setupUi(this);

    //TODO: Can we embed this file into the resources, so the api key is not that obvious?
    QFile file(RGSettings::resourceLocation(QStringLiteral("google-maps-template.html")));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    while (!in.atEnd())
    {
        m_html_template.append(in.readLine());
    }

    ui.progressBar->hide();
    ui.progressBar->setRange(0, 100);
    ui.widthScaleSB->setValue(RGSettings::getGMXFactor());
    ui.heightScaleSB->setValue(RGSettings::getGMYFactor());

    ui.webView->setPage(new QWebEnginePage(ui.webView));
    //DEBUG:ui.webView->setPage(new myWebPage(ui.webView));

    //We connect both scale spinboxes to the same slot! So we can't use
    //the passed value, because we don't know the source.
    QObject::connect(ui.widthScaleSB, &QDoubleSpinBox::valueChanged, this, &RGGoogleMap::handleScaleSpinboxChanged);
    QObject::connect(ui.heightScaleSB, &QDoubleSpinBox::valueChanged, this, &RGGoogleMap::handleScaleSpinboxChanged);

    QObject::connect(ui.buttonBox, &QDialogButtonBox::accepted, this, &RGGoogleMap::on_accept);

    ui.mapTypeBox->insertItem(0, QStringLiteral("roadmap"));
    ui.mapTypeBox->insertItem(1, QStringLiteral("terrain"));
    ui.mapTypeBox->insertItem(2, QStringLiteral("hybrid"));
    ui.mapTypeBox->insertItem(3, QStringLiteral("satellite"));
    ui.mapTypeBox->insertItem(3, QStringLiteral("OSM"));
    ui.mapTypeBox->setCurrentIndex(0);

    //Init map resolution
    handleScaleSpinboxChanged(true);

    if (geoPath.size() > 1)
    {
        QGeoRectangle startGeoRect = geoPath.boundingGeoRectangle();
        //Load initial map location from incoming geo rectangle
        qDebug() << "startGeoRect:"
                 << "  topLeft:" << startGeoRect.topLeft() << "  bottomLeft:" << startGeoRect.bottomLeft() << "  topRight:" << startGeoRect.topRight()
                 << "  bottomRight:" << startGeoRect.bottomRight() << "  center:" << startGeoRect.center();

        ui.lineEdit->setEnabled(false);
        ui.goButton->setEnabled(false);

        QString latlon = QString::number(startGeoRect.center().latitude()) + "," + QString::number(startGeoRect.center().longitude());
        ui.webView->setHtml(genHtml(latlon, QStringLiteral("10")));
        //ui.webView->reload();
    }

    setGeometry(RGSettings::getGoogleMapDialogGeometry());
}

void RGGoogleMap::accept()
{
    m_map = QPixmap(ui.webView->size());
    ui.webView->render(&m_map);

    RGSettings::setGMXFactor(ui.widthScaleSB->value());
    RGSettings::setGMYFactor(ui.heightScaleSB->value());
    RGSettings::setGoogleMapDialogGeometry(geometry());

    QDialog::accept();
}

void RGGoogleMap::on_accept()
{
    if (m_geoPath.size() > 1)
    {
        //First delete the drawn rectangle
        ui.webView->page()->runJavaScript(QStringLiteral("deleteGeoRect();"));

        //We need to wait until the georect is deleted
        QTimer::singleShot(1000, this, &RGGoogleMap::continue_Accept);

        setCursor(Qt::WaitCursor);
    }
    else
    {
        continue_Accept();
    }
}

void RGGoogleMap::continue_Accept()
{
    //Now retrieve the map boundaries from google's map
    ui.webView->page()->runJavaScript(QStringLiteral("getBounds();"), QWebEngineScript::MainWorld,
                                      [this](const QVariant& v)
                                      {
                                          qDebug() << "Result of getBounds():" << v.typeName();
                                          if (m_mapBounds.fromQVariant(v))
                                          {
                                              this->accept();
                                          }
                                      });
}

void RGGoogleMap::on_goButton_clicked(bool)
{
    //This is what we still accept:
    //http://maps.google.nl/?ie=UTF8&ll=52.36428,4.847116&spn=0.035902,0.077162&z=14
    //this is what we expect since the new google maps version
    //https://www.google.nl/maps/@52.374716,4.898623,12z

    QString manUrl = ui.lineEdit->text();
    QUrl url(manUrl);
    QUrlQuery urlQuery(url);
    QString latlon;
    QString zoom;
    if (url.hasFragment() || url.host().contains(QLatin1String("google")))
    {
        latlon = urlQuery.queryItemValue(QStringLiteral("ll"));
        zoom = urlQuery.queryItemValue(QStringLiteral("z"));
        if (latlon.isEmpty() || zoom.isEmpty())
        {
            //Now try the new google maps URL format (the construction is not supported by QUrl, so parse the URL manually
            //https://www.google.nl/maps/@52.374716,4.898623,12z
            QStringList part1 = manUrl.split(QStringLiteral("/@"));
            if (part1.size() == 2)
            {
                QStringList locItems = part1[1].split(',');
                if (locItems.size() > 2)
                {
                    latlon = locItems[0] + "," + locItems[1];
                    zoom = locItems[2].mid(0, locItems[2].indexOf('z'));
                }
            }
        }
    }
    if (latlon.isEmpty() || zoom.isEmpty())
    {
        QMessageBox::warning(this, tr("Web Test"),
                             tr("URL should have format similar like this:\n"
                                "https://www.google.nl/maps/@52.374716,4.898623,12z\n"
                                "Copy it from the Google Maps URL line in your browser."));
        return;
    }

    //Zoom should be an integer value, but sometimes url's contain zoomlevels as floats
    int zoomValue = int(zoom.toFloat());
    zoom = QString::number(zoomValue);

    ui.webView->setHtml(genHtml(latlon, zoom));
    //ui.webView->reload();
}

void RGGoogleMap::handleScaleSpinboxChanged(double)
{
    //multiply scale factors by current output resolution width/height
    auto res = RGSettings::getOutputResolution();
    ui.webView->setFixedSize(QSize(ui.widthScaleSB->value() * res.width(), ui.heightScaleSB->value() * res.height()));
}

void RGGoogleMap::on_mapTypeBox_textActivated(const QString& text)
{
    qDebug() << "on_mapTypeBox_textActivated:" << text;
    ui.webView->page()->runJavaScript(QStringLiteral("setMapType(\"") + text + "\");");
}

void RGGoogleMap::on_zoomBox_valueChanged(int zoom)
{
    ui.webView->page()->runJavaScript(QString(QStringLiteral("map.setZoom(")) + QString::number(zoom) + QStringLiteral(");"));
}

void RGGoogleMap::on_webView_loadFinished(bool)
{
    qDebug() << "webView::loadFinished";
    if (m_geoPath.size() > 1)
    {
        QGeoRectangle geoRect = m_geoPath.boundingGeoRectangle();

        QString javaRect = QStringLiteral("[");
        javaRect += "{lat: " + QString::number(geoRect.topLeft().latitude()) + ", lng: " + QString::number(geoRect.topLeft().longitude()) + "},";
        javaRect += "{lat: " + QString::number(geoRect.topRight().latitude()) + ", lng: " + QString::number(geoRect.topRight().longitude()) + "},";
        javaRect += "{lat: " + QString::number(geoRect.bottomRight().latitude()) + ", lng: " + QString::number(geoRect.bottomRight().longitude()) + "},";
        javaRect += "{lat: " + QString::number(geoRect.bottomLeft().latitude()) + ", lng: " + QString::number(geoRect.bottomLeft().longitude()) + "},";
        javaRect += "{lat: " + QString::number(geoRect.topLeft().latitude()) + ", lng: " + QString::number(geoRect.topLeft().longitude()) + "}]";

        qDebug() << "javaRect:" + javaRect;

        ui.webView->page()->runJavaScript(QString(QStringLiteral("drawGeoRect(")) + javaRect + QStringLiteral(");"));
    }

    ui.progressBar->hide();
}

void RGGoogleMap::on_webView_loadProgress(int progress)
{
    ui.progressBar->setValue(progress);
}

void RGGoogleMap::on_webView_loadStarted()
{
    qDebug() << "webView::loadStarted";
    ui.progressBar->show();
}

QString RGGoogleMap::genHtml(const QString& latlon, const QString& zoom) const
{
    qDebug() << "Replace: LATLON = " << latlon << "; zoom = " << zoom;
    QString html = m_html_template;
    html.replace(QLatin1String("LATLON"), latlon);
    html.replace(QLatin1String("ZOOM"), zoom);
    html.replace(QLatin1String("MAPTYPE"), ui.mapTypeBox->currentText());
    html.replace(QLatin1String("XXXXXXXXXXXXX"), getDecodedApiKeyFromObf());

    ui.zoomBox->setValue(zoom.toInt());

#if 0
    QFile file("out.html");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);
        out << html;
    }
#endif

    return html;
}

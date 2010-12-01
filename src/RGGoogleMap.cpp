#include "RGGoogleMap.h"
#include "RGSettings.h"

#include <QtGui>

RGGoogleMap::RGGoogleMap(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	QFile file("google-maps-template.html");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    while (!in.atEnd()) {
        m_html_template.append(in.readLine());
    }

	ui.progressBar->hide();
	ui.progressBar->setRange(0, 100);
  ui.spinBoxX->setValue(RGSettings::getGMXResolution());
  ui.spinBoxY->setValue(RGSettings::getGMYResolution());

	//Init map resolution
	on_fixButton_clicked(true);
}

void RGGoogleMap::accept()
{
	m_map = QPixmap(ui.webView->size());
	ui.webView->render(&m_map);

  RGSettings::setGMXResolution(ui.spinBoxX->value());
  RGSettings::setGMYResolution(ui.spinBoxY->value());

	QDialog::accept();
}


void RGGoogleMap::on_goButton_clicked(bool)
{
	QUrl url = ui.lineEdit->text();

    //This is what we expect:
	//http://maps.google.nl/?ie=UTF8&ll=52.36428,4.847116&spn=0.035902,0.077162&z=14

	QString latlon;
	QString zoom;
	if (url.hasFragment() || url.host().contains("google"))
	{
		latlon = url.queryItemValue ("ll");
		zoom = url.queryItemValue ("z");
	}
	if (latlon.isEmpty() || zoom.isEmpty())
	{
		int ret = QMessageBox::warning(this, tr("Web Test"),
			tr("URL should have format similar like this:\n"
			   "http://maps.google.nl/?ie=UTF8&ll=52.36428,4.847116&spn=0.035902,0.077162&z=14\n"
			   "Copy it from the paste link option from Google Maps in your browser."));
		return;
	}


	ui.webView->setHtml(genHtml(latlon, zoom));
	ui.webView->reload();
}

void RGGoogleMap::on_fixButton_clicked(bool)
{
	ui.webView->setFixedSize(QSize(ui.spinBoxX->value(), ui.spinBoxY->value()));
}

void RGGoogleMap::on_webView_loadFinished ( bool ok )
{
	ui.progressBar->hide();
}

void RGGoogleMap::on_webView_loadProgress ( int progress )
{
	ui.progressBar->setValue(progress);

}

void RGGoogleMap::on_webView_loadStarted ()
{
	ui.progressBar->show();
}

QString RGGoogleMap::genHtml(const QString &latlon, const QString &zoom) const
{
	QString html = m_html_template;
	html.replace("LATLON", latlon);
	html.replace("ZOOM", zoom);

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

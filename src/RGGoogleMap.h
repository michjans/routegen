#include "ui_googlemap.h"

#include <QString>
#include <QPixmap>

class RGGoogleMap : public QDialog
{
	Q_OBJECT

public:
	RGGoogleMap(QWidget *parent);

	QPixmap getMap() const {return m_map;}

public slots:
	void accept();

private slots:
    void on_goButton_clicked(bool);
    void on_fixButton_clicked(bool);

	void on_webView_loadFinished ( bool ok );
	void on_webView_loadProgress ( int progress );
	void on_webView_loadStarted ();

private:
	QString genHtml(const QString &latlon, const QString &zoom) const;

	Ui_googleMap ui;
	QString    m_html_template;
	QPixmap    m_map;
};
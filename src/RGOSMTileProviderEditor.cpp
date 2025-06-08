#include "RGOSMTileProviderEditor.h"
#include "ui_osmtileprovidereditor.h"

RGOSMTileProviderEditor::RGOSMTileProviderEditor(QWidget* parent)
    : QDialog(parent),
      ui(new Ui::OsmProviderEditor)
{
    ui->setupUi(this);
}

RGOSMTileProviderEditor::RGOSMTileProviderEditor(const RGTileProviderManager::TileProvider& tileProvider, QWidget* parent)
    : RGOSMTileProviderEditor(parent)
{
    ui->tileProviderEdit->setText(tileProvider.name);
    ui->tileProviderURL->setText(tileProvider.urlTemplate);
    ui->tileProviderAttribution->setText(tileProvider.attribution);
}

RGOSMTileProviderEditor::~RGOSMTileProviderEditor()
{
    delete ui;
}

RGTileProviderManager::TileProvider RGOSMTileProviderEditor::tileProvider()
{
    RGTileProviderManager::TileProvider tileProvider;
    tileProvider.name = ui->tileProviderEdit->text();
    tileProvider.urlTemplate = ui->tileProviderURL->text();
    tileProvider.attribution = ui->tileProviderAttribution->text();

    return tileProvider;
}

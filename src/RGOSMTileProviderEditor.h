#ifndef RBOSMPROVIDEREDITOR_H
#define RBOSMPROVIDEREDITOR_H

#include <QDialog>

#include "RGOSMTileProviderManager.h"

namespace Ui
{
class OsmProviderEditor;
}

class RGOSMTileProviderEditor : public QDialog
{
    Q_OBJECT

public:
    explicit RGOSMTileProviderEditor(QWidget* parent = nullptr);
    explicit RGOSMTileProviderEditor(const RGTileProviderManager::TileProvider& tileProvider, QWidget* parent = nullptr);
    ~RGOSMTileProviderEditor();

    RGTileProviderManager::TileProvider tileProvider();

private:
    Ui::OsmProviderEditor* ui;
};

#endif // RBOSMPROVIDEREDITOR_H

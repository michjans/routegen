#include "GPXSelectionDialog.h"
#include "ui_gpxselectiondialog.h"

GPXSelectionDialog::GPXSelectionDialog(const QStringList &routeNames, const QStringList &trackNames, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GPXSelectionDialog)
{
    ui->setupUi(this);

    if (!routeNames.empty())
    {
        routeItem = new QTreeWidgetItem(ui->gpxTreeView, QStringList("Routes"));
        for (const QString &routeName : routeNames)
        {
            new QTreeWidgetItem(routeItem, QStringList(routeName));
        }
    }
    if (!trackNames.empty())
    {
        trackItem = new QTreeWidgetItem(ui->gpxTreeView, QStringList("Tracks"));
        for (const QString &trackName : trackNames)
        {
            new QTreeWidgetItem(trackItem, QStringList(trackName));
        }
    }
}

GPXSelectionDialog::~GPXSelectionDialog()
{
    delete ui;
}

QString GPXSelectionDialog::selectedItem(GPXSelectionDialog::GPXItemType &itemType) const
{
    QString selectedItemTxt;
    itemType = GPXItemType::ITEM_TYPE_NONE;
    if (!ui->gpxTreeView->selectedItems().empty())
    {
        QTreeWidgetItem *selectedItem = ui->gpxTreeView->selectedItems().first();
        selectedItemTxt = selectedItem->text(0);
        if (selectedItem->parent() == routeItem)
        {
            itemType = GPXItemType::ITEM_TYPE_ROUTE;
        }
        else if (selectedItem->parent() == trackItem)
        {
            itemType = GPXItemType::ITEM_TYPE_TRACK;
        }
    }
    return selectedItemTxt;
}

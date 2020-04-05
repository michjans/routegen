#ifndef GPXSELECTIONDIALOG_H
#define GPXSELECTIONDIALOG_H

#include <QDialog>
#include <QStringList>

class QTreeWidgetItem;

namespace Ui {
class GPXSelectionDialog;
}

class GPXSelectionDialog : public QDialog
{
    Q_OBJECT

public:
    enum GPXItemType
    {
        ITEM_TYPE_NONE,
        ITEM_TYPE_TRACK,
        ITEM_TYPE_ROUTE
    };

    explicit GPXSelectionDialog(const QStringList &routeNames, const QStringList &trackNames, QWidget *parent = nullptr);
    ~GPXSelectionDialog();

    int selectedItem(GPXItemType &itemType) const;

private:
    Ui::GPXSelectionDialog *ui;

    QTreeWidgetItem *routeItem;
    QTreeWidgetItem *trackItem;
};

#endif // GPXSELECTIONDIALOG_H

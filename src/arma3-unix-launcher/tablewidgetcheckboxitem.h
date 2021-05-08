#ifndef TABLEWIDGETCHECKBOXITEM_H
#define TABLEWIDGETCHECKBOXITEM_H

#include <QTableWidgetItem>

class TableWidgetCheckboxItem : public QTableWidgetItem
{
public:
    explicit TableWidgetCheckboxItem(bool enabled);

    virtual bool operator<(const QTableWidgetItem& other) const override {
        bool selfEnabled = data(Qt::UserRole).toBool();
        bool otherEnabled = other.data(Qt::UserRole).toBool();
        return selfEnabled < otherEnabled;
    }

    void setModEnabled(int checkstate) {
        setData(Qt::UserRole, checkstate == Qt::Checked ? true : false);
    }
};

#endif // TABLEWIDGETCHECKBOXITEM_H

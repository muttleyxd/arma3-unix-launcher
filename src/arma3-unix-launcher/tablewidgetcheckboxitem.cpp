#include "tablewidgetcheckboxitem.h"

TableWidgetCheckboxItem::TableWidgetCheckboxItem(bool enabled)
{
    setData(Qt::UserRole, enabled);
}

bool TableWidgetCheckboxItem::operator<(const QTableWidgetItem &other) const
{
    bool self_enabled = data(Qt::UserRole).toBool();
    bool other_enabled = other.data(Qt::UserRole).toBool();
    return self_enabled < other_enabled;
}

void TableWidgetCheckboxItem::setModEnabled(int checkstate)
{
    setData(Qt::UserRole, checkstate == Qt::Checked ? true : false);
}

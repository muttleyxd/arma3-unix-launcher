#include "tablewidgetcheckboxitem.h"

TableWidgetCheckboxItem::TableWidgetCheckboxItem(bool enabled)
{
    setData(Qt::UserRole, enabled);
}

#include "qmodlist.hpp"

bool QModList::dropMimeData(int row, int, const QMimeData *data, Qt::DropAction action)
{
    std::cout << "dropmeme\n";
    return QTableWidget::dropMimeData(row, 0, data, action);
}

Qt::DropActions QModList::supportedDropActions() const
{
    std::cout << "hi\n";
    return Qt::DropAction::MoveAction;
}

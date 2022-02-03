#pragma once

#include <QTableWidgetItem>

class TableWidgetCheckboxItem : public QTableWidgetItem
{
    public:
        explicit TableWidgetCheckboxItem(bool enabled);

        bool operator<(const QTableWidgetItem &other) const override;

        void setModEnabled(int checkstate);
};

#ifndef QMODVIEW_HPP_
#define QMODVIEW_HPP_

#include <QHeaderView>
#include <QTableView>

#include <iostream>

#include <fmt/format.h>

struct ModEntry
{
    QString name;
    QString path;
};

class ModModel : public QAbstractTableModel
{
        QList<ModEntry> m_data;
    public:
        ModModel(QObject *parent = {}) : QAbstractTableModel{parent} {}
        int rowCount(const QModelIndex &) const override
        {
            return m_data.count();
        }
        int columnCount(const QModelIndex &) const override
        {
            return 2;
        }
        QVariant data(const QModelIndex &index, int role) const override
        {
            const auto &mod = m_data[index.row()];
            if (role == Qt::DisplayRole)
            {
            switch (index.column())
            {
                case 0:
                    return mod.name;
                case 1:
                    return mod.path;
                default:
                    return {};
            }
            }
            return {};
        }

        bool dropMimeData(const QMimeData *data, Qt::DropAction action,
                          int row, int column, const QModelIndex &parent) override
        {
            std::string actionS = "";
            if (action == Qt::DropAction::MoveAction)
                actionS = "move";
            if (action == Qt::DropAction::CopyAction)
                actionS = "copy";

            fmt::print("DropMimeData({}): {} {}, p: {} {}\n", actionS, row, column, parent.row(), parent.column());
            return QAbstractTableModel::dropMimeData(data, action, row, 0, parent);
        }

        void append(const ModEntry & mod) {
              beginInsertRows({}, m_data.count(), m_data.count());
              m_data.append(mod);
              endInsertRows();
           }

        Qt::ItemFlags flags(const QModelIndex &index) const
        {
            Qt::ItemFlags defaultFlags = QAbstractTableModel::flags(index);

            if (index.isValid())
                return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
            else
                return Qt::ItemIsDropEnabled | defaultFlags;
        }

        bool insertRows(int position, int rows, const QModelIndex &parent)
        {
            /*beginInsertRows(QModelIndex(), position, position+rows-1);

             for (int row = 0; row < rows; ++row) {
                 m_data.insert(position,{});
             }

             endInsertRows();*/

            fmt::print("i, p: {} r: {}, parent: r: {}\n", position, rows, parent.row());
            return QAbstractTableModel::insertRows(position, rows, parent);
        }

        bool removeRows(int position, int rows, const QModelIndex &parent)
        {
            /*beginRemoveRows(QModelIndex(), position, position+rows-1);

            for (int row = 0; row < rows; ++row) {
                m_data.removeAt(position);
            }

            endRemoveRows();*/

            fmt::print("r, p: {} r: {}, parent: r: {}\n", position, rows, parent.row());
            return QAbstractTableModel::removeRows(position, rows, parent);
        }

        Qt::DropActions supportedDropActions() const
        {
            return Qt::MoveAction;// | Qt::CopyAction;
        }
};

class QModView : public QTableView
{
    public:
        QModView(QWidget *parent) : QTableView(parent)
        {
            std::cout << "OŁJE\n";
        }
        ~QModView() override = default;

        void setup()
        {
            ModModel* model = new ModModel();
            model->append(ModEntry{"hello", "there"});
            model->append(ModEntry{"general", "kenobi"});
            this->setModel(model);

            this->verticalHeader()->setSectionsMovable(false);
            this->setSelectionBehavior(QTableView::SelectRows);
            this->setSelectionMode(QTableView::SingleSelection);
            this->setShowGrid(false);
            this->setDragDropMode(QTableView::DragDrop);
            this->setDefaultDropAction(Qt::MoveAction);
            this->setDragDropOverwriteMode(false);
            this->setDragEnabled(true);
            this->setAcceptDrops(true);
            this->viewport()->setAcceptDrops(true);
        }
};

#endif

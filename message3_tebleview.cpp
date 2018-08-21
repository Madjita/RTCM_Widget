#include "message3_tebleview.h"


Message3_TebleView::Message3_TebleView()
{
    model.clear();
}

int Message3_TebleView::rowCount(const QModelIndex &parent) const
{
    return model.count() + 1;
}

int Message3_TebleView::columnCount(const QModelIndex &parent) const
{
    return 3;
}

QVariant Message3_TebleView::data(const QModelIndex &index, int role) const
{
    if (index.row() == 0) {
        if (role == Qt::BackgroundRole) {
            return QBrush(Qt::gray);
        } else if (role == Qt::ForegroundRole) {
            return QColor(Qt::black);
        } else if (role == Qt::DisplayRole) {
            if (index.column() == 0) {
                return tr("X");
            } else if (index.column() == 1) {
                return tr("Y");
            } else if (index.column() == 2) {
                return tr("Z");
            }
        }
    } else {

        if (index.column() == 0) {
            if (role == Qt::DisplayRole) {
                return model.at(index.row() - 1).second.value(0);
            }
        } else if (index.column() == 1) {
            if (role == Qt::DisplayRole) {
                return model.at(index.row() - 1).second.value(1);
            }
        } else if (index.column() == 2) {
            if (role == Qt::DisplayRole) {
                return model.at(index.row() - 1).first;
            }
        }

    }

    return QVariant();
}

Qt::ItemFlags Message3_TebleView::flags(const QModelIndex &index) const
{
    if (index.row() == 0)
        return Qt::NoItemFlags;
    else
        return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
}

void Message3_TebleView::addData(QPair<QString,QStringList> data)
{

    beginResetModel();
    if(model.count() == 0 ){
        model.append(data);
    }
    else
    {
       model.replace(0,data);
    }
    endResetModel();

}

void Message3_TebleView::clearData()
{
    beginResetModel();
    model.clear();
    endResetModel();
}

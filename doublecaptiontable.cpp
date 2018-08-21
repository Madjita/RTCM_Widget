#include "doublecaptiontable.h"

DoubleCaptionTable::DoubleCaptionTable()
{
    model.clear();
}

int DoubleCaptionTable::rowCount(const QModelIndex &parent) const
{
    return model.count() + 1;
}

int DoubleCaptionTable::columnCount(const QModelIndex &parent) const
{
    return 6;
}

QVariant DoubleCaptionTable::data(const QModelIndex &index, int role) const
{

    if (index.row() == 0) {
        if (role == Qt::BackgroundRole) {
            return QBrush(Qt::gray);
        } else if (role == Qt::ForegroundRole) {
            return QColor(Qt::black);
        } else if (role == Qt::DisplayRole) {
            if (index.column() == 0) {
                return tr("Масштабный коэффициент");
            } else if (index.column() == 1) {
                return tr("Ошибка дифференциальной дальности пользователя (UDRE)");
            } else if (index.column() == 2) {
                return tr("Номер спутника");
            } else if (index.column() == 3) {
                return tr("Поправка псевдодальности");
            } else if (index.column() == 4) {
                return tr("Скорость приращения поправки псевдодальности");
            } else if (index.column() == 5) {
                return tr("Признак конкретной посылки данных IOD");
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
        } else if (index.column() == 3) {
            if (role == Qt::DisplayRole) {
                return model.at(index.row() - 1).second.value(2);
            }
        } else if (index.column() == 4) {
            if (role == Qt::DisplayRole) {
                return model.at(index.row() - 1).second.value(3);
            }
        } else if (index.column() == 5) {
            if (role == Qt::DisplayRole) {
                return model.at(index.row() - 1).second.value(4);
            }
        }
    }

    return QVariant();
}

Qt::ItemFlags DoubleCaptionTable::flags(const QModelIndex &index) const
{
        if (index.row() == 0)
            return Qt::NoItemFlags;
        else
            return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
}

void DoubleCaptionTable::addData(QPair<QString, QStringList> data)
{

    beginResetModel();
    if(model.count() == 0 ){
        model.append(data);
    }
    else
    {
        int index = -1;
        for (int i=0;i < model.count();i++) {
            if(model.value(i).first == data.first){
                index = i;
            }
        }

        if(index == -1){
            model.append(data);
        }
        else
        {
            model.replace(index,data);
        }
    }
    endResetModel();



}

void DoubleCaptionTable::clearData()
{
    beginResetModel();
    model.clear();
    endResetModel();
}

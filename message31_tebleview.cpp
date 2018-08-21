#include "message31_tebleview.h"

Message31_TebleView::Message31_TebleView()
{
    model.clear();
}

int Message31_TebleView::rowCount(const QModelIndex &parent) const
{
    return model.count() + 1;
}

int Message31_TebleView::columnCount(const QModelIndex &parent) const
{
    return 7;
}

QVariant Message31_TebleView::data(const QModelIndex &index, int role) const
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
                return tr("R");
            } else if (index.column() == 6) {
                return tr("Время данных Tb");
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
        } else if (index.column() == 6) {
            if (role == Qt::DisplayRole) {
                return model.at(index.row() - 1).second.value(5);
            }
        }

    }

    return QVariant();
}

Qt::ItemFlags Message31_TebleView::flags(const QModelIndex &index) const
{
    if (index.row() == 0)
        return Qt::NoItemFlags;
    else
        return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
}

void Message31_TebleView::addData(QPair<QString,QStringList> data)
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

void Message31_TebleView::clearData()
{
    beginResetModel();
    model.clear();
    endResetModel();
}

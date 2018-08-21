#include "message3header_tebleview.h"


Message3Header_TebleView::Message3Header_TebleView()
{
    model.clear();
}

int Message3Header_TebleView::rowCount(const QModelIndex &parent) const
{
    return model.count() + 1;
}

int Message3Header_TebleView::columnCount(const QModelIndex &parent) const
{
    return 6;
}

QVariant Message3Header_TebleView::data(const QModelIndex &index, int role) const
{
    if (index.row() == 0) {
        if (role == Qt::BackgroundRole) {
            return QBrush(Qt::gray);
        } else if (role == Qt::ForegroundRole) {
            return QColor(Qt::black);
        } else if (role == Qt::DisplayRole) {
            if (index.column() == 0) {
                return tr("Тип сообщения");
            } else if (index.column() == 1) {
                return tr("Индекс опорной станции");
            }
            if (index.column() == 2) {
                return tr("Модифицированный Z-счет");
            } else if (index.column() == 3) {
                return tr("№ последовательности");
            } else if (index.column() == 4) {
                return tr("Число слов данных (длина кадра)");
            } else if (index.column() == 5) {
                return tr("Состояние ОС");
            }
        }
    } else {

            if (index.column() == 0) {
                if (role == Qt::DisplayRole) {
                    return model.at(index.row() - 1).first;
                }
            } else if (index.column() == 1) {
                if (role == Qt::DisplayRole) {
                    return model.at(index.row() - 1).second.value(0);
                }
            } else if (index.column() == 2) {
                if (role == Qt::DisplayRole) {
                    return model.at(index.row() - 1).second.value(1);
                }
            } else if (index.column() == 3) {
                if (role == Qt::DisplayRole) {
                    return model.at(index.row() - 1).second.value(2);
                }
            } else if (index.column() == 4) {
                if (role == Qt::DisplayRole) {
                    return model.at(index.row() - 1).second.value(3);
                }
            }else if (index.column() == 5) {
                if (role == Qt::DisplayRole) {
                    return model.at(index.row() - 1).second.value(4);
                }
            }


    }

    return QVariant();
}

Qt::ItemFlags Message3Header_TebleView::flags(const QModelIndex &index) const
{
    if (index.row() == 0)
        return Qt::NoItemFlags;
    else
        return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
}

void Message3Header_TebleView::addData(QPair<QString,QStringList> data)
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

void Message3Header_TebleView::clearData()
{
    beginResetModel();
    model.clear();
    endResetModel();
}

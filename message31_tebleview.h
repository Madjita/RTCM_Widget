#ifndef MESSAGE31_TEBLEVIEW_H
#define MESSAGE31_TEBLEVIEW_H

#include <QAbstractTableModel>
#include <QBrush>

class Message31_TebleView : public QAbstractTableModel
{
public:
    Message31_TebleView();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags ( const QModelIndex & index ) const;

private:
    QVector<QPair<QString,QStringList>> model;

public slots:
    void addData(QPair<QString,QStringList> data);
    void clearData();
};

#endif // MESSAGE31_TEBLEVIEW_H

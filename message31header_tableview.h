#ifndef MESSAGE31HEADER_TABLEVIEW_H
#define MESSAGE31HEADER_TABLEVIEW_H

#include <QAbstractTableModel>
#include <QBrush>

class Message31Header_TableView : public QAbstractTableModel
{
public:
    Message31Header_TableView();

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

#endif // MESSAGE31HEADER_TABLEVIEW_H

#ifndef MESSAGE3_TEBLEVIEW_H
#define MESSAGE3_TEBLEVIEW_H

#include <QAbstractTableModel>
#include <QBrush>
class Message3_TebleView : public QAbstractTableModel
{
public:
    Message3_TebleView();

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

#endif // MESSAGE3_TEBLEVIEW_H

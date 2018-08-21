#ifndef DOUBLECAPTIONTABLE_H
#define DOUBLECAPTIONTABLE_H

#include <QAbstractTableModel>
#include <QBrush>
#include <QPair>

class DoubleCaptionTable : public QAbstractTableModel
{
    Q_OBJECT

public:
    DoubleCaptionTable();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags ( const QModelIndex & index ) const;

private:
  QVector<QPair<QString, QStringList> > model;

public slots:
    void addData(QPair<QString, QStringList> data);
    void clearData();
};

#endif // DOUBLECAPTIONTABLE_H

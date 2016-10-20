#ifndef TEETHFORMULA_H
#define TEETHFORMULA_H

#include <QAbstractTableModel>
#include <QVector>
#include <QPair>
#include <QString>
#include <QColor>

class TeethFormula : public QAbstractTableModel
{
public:
    TeethFormula();
    ~TeethFormula();

    int columnCount(const QModelIndex &parent) const;
    int rowCount(const QModelIndex &parent) const;

    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    Qt::ItemFlags flags(const QModelIndex &index) const;

private:
    QVector<QPair<QString, QString> > m_data;
};

#endif // TEETHFORMULA_H

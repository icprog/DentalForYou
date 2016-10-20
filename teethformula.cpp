#include "teethformula.h"

TeethFormula::TeethFormula()
{
    for (int i = 0; i < 16; i++) {
        QPair<QString, QString> pair;
        pair.first = "";
        pair.second = "";

        m_data.push_back(pair);
    }
}

TeethFormula::~TeethFormula()
{

}

int TeethFormula::columnCount(const QModelIndex &parent) const
{
    return m_data.count();
}

int TeethFormula::rowCount(const QModelIndex &parent) const
{
    return 3;
}

QVariant TeethFormula::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        QPair<QString, QString> pair = m_data.at(index.column());

        if (index.row() == 0) {
            return pair.first;
        }
        else if (index.row() == 1) {
            if (index.column() >= 8) {
                return index.column() - 7;
            }
            else if (index.column() < 8) {
                return 8 - index.column();
            }
        }
        else if (index.row() == 2) {
            return pair.second;
        }
    }
    else if (role == Qt::BackgroundColorRole) {
        if (index.row() == 1) {
            return QColor(Qt::darkGray);
        }
        return QColor(Qt::white);
    }
    else if (role == Qt::ForegroundRole) {
        if (index.row() == 1) return QColor(Qt::black);
    }
    else if (role == Qt::TextAlignmentRole) {
        return Qt::AlignCenter;
    }

    return QVariant();
}

bool TeethFormula::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        int col = index.column();
        int row = index.row();

        QPair<QString, QString> p = m_data.value(col);

        if (row == 0) {
            p.first = value.toString();
        }
        else if (row == 2) {
            p.second = value.toString();
        }
        else {
            return false;
        }

        m_data.replace(col, p);
        emit(dataChanged(index, index));

        return true;
    }


    return false;
}

Qt::ItemFlags TeethFormula::flags(const QModelIndex &index) const
{
    if (!index.isValid() || index.row() == 1) {
        return QAbstractTableModel::flags(index) & Qt::ItemIsSelectable;
    }

    return QAbstractTableModel::flags(index) | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}


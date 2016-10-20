#ifndef TEETHFORMULADIALOG_H
#define TEETHFORMULADIALOG_H

#include <QDialog>
#include <QTableView>
#include <QVBoxLayout>
#include <QHeaderView>

#include "teethformula.h"

class TeethFormulaDialog : public QDialog
{
public:
    TeethFormulaDialog();
    ~TeethFormulaDialog();

    QTableView *table;
};

#endif // TEETHFORMULADIALOG_H

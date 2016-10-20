#include "teethformuladialog.h"

TeethFormulaDialog::TeethFormulaDialog()
{
    QVBoxLayout *mainLayout = new QVBoxLayout();

    TeethFormula *teeth = new TeethFormula();

    table = new QTableView();
    table->setModel(teeth);
    table->verticalHeader()->hide();
    table->horizontalHeader()->hide();
    table->setSelectionMode(QAbstractItemView::SingleSelection);

    for (int i = 0; i < table->model()->columnCount(); i++) {
        table->setColumnWidth(i, 30);
    }

    table->setFixedWidth(table->model()->columnCount() * 30 + 2);
    table->setFixedHeight(table->model()->rowCount() * 35 - 13);

    mainLayout->addWidget(table);
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);

    this->setLayout(mainLayout);
    this->setWindowTitle(tr("Зубная формула"));
}

TeethFormulaDialog::~TeethFormulaDialog()
{

}


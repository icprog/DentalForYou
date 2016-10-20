#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QSqlTableModel>
#include <QListView>
#include <QTableView>
#include <QString>
#include <QEvent>
#include <QKeyEvent>
#include <QTextEdit>
#include <QMap>
#include <QInputDialog>
#include <QTextEdit>
#include <QPushButton>
#include <QSplitter>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolBar>
#include <QPixmap>
#include <QDir>
#include <QFile>
#include <QSqlQuery>
#include <QMessageBox>
#include <QTextStream>
#include <QAction>
#include <QTextDocument>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>
#include <QDesktopWidget>
#include <QRect>
#include <QFont>

#include "preview.h"
#include "teethformuladialog.h"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QString dbFileName, QWidget *parent = 0);
    ~MainWindow();

private:
    QString m_dbFileName;

    QSqlTableModel *m_categoriesModel;
    QSqlTableModel *m_recordsModel;

    QListView *m_categoriesList;
    QListView *m_recordsList;

    QMap<int, QString> m_results;
    QTextEdit *m_resultTextEdit;

    QString m_categories;

    TeethFormulaDialog *m_teethWindow;

    int m_selectedId;

    bool connectDB();
    int createDB();

    void getRecords();
    void getResults();

    bool eventFilter(QObject *, QEvent *);
    void closeEvent(QCloseEvent *);

signals:
    void closed();

private slots:
    void onCategoriesListClicked(const QModelIndex &index);
    void onRecordsListDoubleClicked(const QModelIndex &index);
    void onAddButtonClicked();
    void onDeleteButtonClicked();

    void onResultTextChanged();

    void preview();
    void printBase();
};

#endif // MAINWINDOW_H

#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QLine>
#include <QPushButton>
#include <QFormLayout>
#include <QPalette>
#include <QFont>
#include <QDesktopWidget>
#include <QEvent>
#include <QKeyEvent>
#include <QMessageBox>
#include <QVector>

#include "crypter.h"
#include "mainwindow.h"

class LoginWindow : public QWidget
{
    Q_OBJECT
public:
    explicit LoginWindow(QWidget *parent = 0);
    ~LoginWindow();

private:
    MainWindow *m_mainWindow;

    QLineEdit *m_nameLineEdit;
    QLineEdit *m_passLineEdit;

    QFile *m_loginFile;

    bool eventFilter(QObject *obj, QEvent *evt);

    bool logIn(QString name, QString password);
    bool registerUser(QString name, QString password);

signals:

public slots:
    void onOKButtonClicked();
    void onCancelButtonClicked();
};

#endif // LOGINWINDOW_H

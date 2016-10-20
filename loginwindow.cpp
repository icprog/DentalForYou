#include "loginwindow.h"

LoginWindow::LoginWindow(QWidget *parent) : QWidget(parent)
{
    this->installEventFilter(this);

    m_loginFile = new QFile("logins.ath");

    QVBoxLayout *mainLayout = new QVBoxLayout();

    QLabel *welcomeLabel = new QLabel(tr("Добро пожаловать!"));
    QFont font = welcomeLabel->font();
    font.setPixelSize(20);
    welcomeLabel->setFont(font);
    welcomeLabel->setAlignment(Qt::AlignCenter);

    QFormLayout *passLayout = new QFormLayout();

    m_nameLineEdit = new QLineEdit();
    m_nameLineEdit->setFocus();
    m_passLineEdit = new QLineEdit();
    m_passLineEdit->setEchoMode(QLineEdit::Password);

    passLayout->addRow(tr("Логин"), m_nameLineEdit);
    passLayout->addRow(tr("Пароль"), m_passLineEdit);

    QHBoxLayout *buttonsLayout = new QHBoxLayout();

    QPushButton *okButton = new QPushButton(tr("ОК"));
    connect(okButton, SIGNAL(clicked()),
            this, SLOT(onOKButtonClicked()));
    QPushButton *cancelButton = new QPushButton(tr("Отмена"));
    connect(cancelButton, SIGNAL(clicked()),
            this, SLOT(onCancelButtonClicked()));

    buttonsLayout->addStretch();
    buttonsLayout->addWidget(okButton);
    buttonsLayout->addWidget(cancelButton);

    mainLayout->addWidget(welcomeLabel);
    mainLayout->addLayout(passLayout);
    mainLayout->addLayout(buttonsLayout);

    mainLayout->setSizeConstraint(QLayout::SetFixedSize);

    this->setLayout(mainLayout);

    QPalette pal(palette());
    pal.setColor(QPalette::Background, Qt::white);
    this->setPalette(pal);
    this->setWindowTitle(tr("Авторизация"));

    QDesktopWidget desktop;

    // Получаем прямоугольник с размерами как у экрана
    QRect rect = desktop.availableGeometry(desktop.primaryScreen());

    // Получаем координаты центра экрана
    QPoint center = rect.center();
    center.setX(center.x() - (this->width()/2));
    center.setY(center.y() - (this->height()/2));
    move(center);
}

LoginWindow::~LoginWindow()
{

}

bool LoginWindow::eventFilter(QObject *obj, QEvent *evt)
{
    if (evt->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(evt);

        if (keyEvent->key() == Qt::Key_Escape) {
            onCancelButtonClicked();
        }
        else if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            onOKButtonClicked();
        }
    }

    return QWidget::eventFilter(obj, evt);
}

bool LoginWindow::logIn(QString name, QString password)
{
    if (!m_loginFile->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        m_loginFile->open(QIODevice::WriteOnly | QIODevice::Text);
        m_loginFile->close();
        m_loginFile->open(QIODevice::ReadOnly | QIODevice::Text);
    }

    while (!m_loginFile->atEnd())
    {
        QString str = m_loginFile->readLine();
        QStringList pair = str.split(" ");

        if (pair[0] == name && Crypter::decryptString(pair[1]) == password)
        {
            this->hide();
            m_loginFile->close();

            m_mainWindow = new MainWindow(pair[0] + ".db");
            connect(m_mainWindow, SIGNAL(closed()), this, SLOT(close()));
            m_mainWindow->show();

            return true;
        }
        else if (pair[0] == name && Crypter::decryptString(pair[1]) != password) {
            QMessageBox *errorMB = new QMessageBox();
            errorMB->critical(0, tr("Ошибка"),
                                 tr("Неправильно набран пароль!"));
            m_loginFile->close();
            m_passLineEdit->clear();
            return false;
        }
    }
    m_loginFile->close();

    registerUser(m_nameLineEdit->text(), m_passLineEdit->text());

    m_nameLineEdit->clear();
    m_passLineEdit->clear();

    m_nameLineEdit->setFocus();

    return false;
}

bool LoginWindow::registerUser(QString name, QString password)
{
    if (m_nameLineEdit->text().isEmpty() || m_passLineEdit->text().isEmpty()) {
        return false;
    }

    m_loginFile->open(QIODevice::Append | QIODevice::Text);

    QMessageBox *registerMB = new QMessageBox();
    int answer = registerMB->question(0, tr("Ошибка"),
                                         tr("Пользователь не найден в базе.\nЗарегистрировать?"));

    if (answer == QMessageBox::Yes) {
        m_loginFile->write(m_nameLineEdit->text().toUtf8());
        m_loginFile->write(" ");
        m_loginFile->write(Crypter::cryptString(m_passLineEdit->text()).toUtf8());
        m_loginFile->write(" \n");
        m_loginFile->close();
        return true;
    }

    m_loginFile->close();
    return false;
}

void LoginWindow::onOKButtonClicked()
{
    if (m_nameLineEdit->text().isEmpty() || m_passLineEdit->text().isEmpty()) {
        return;
    }
    logIn(m_nameLineEdit->text(), m_passLineEdit->text());
}

void LoginWindow::onCancelButtonClicked()
{
    this->close();
}


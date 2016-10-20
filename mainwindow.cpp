#include "mainwindow.h"

MainWindow::MainWindow(QString dbFileName, QWidget *parent)
    : QWidget(parent)
{
    m_dbFileName = dbFileName;

    QDesktopWidget desktop;

    // Получаем прямоугольник с размерами как у экрана
    QRect rect = desktop.availableGeometry(desktop.primaryScreen());

    // Получаем координаты центра экрана
    QPoint center = rect.center();
    center.setX(center.x() - (this->width()/2));
    center.setY(center.y() - (this->height()/2));
    move(center);

    this->installEventFilter(this);

    m_selectedId = -1;

    if (!connectDB()) {
        exit(1);
    }
    createDB();

    m_categoriesModel = new QSqlTableModel();
    m_categoriesModel->setTable("Categories");
    m_categoriesModel->select();
    m_categoriesModel->setEditStrategy(QSqlTableModel::OnFieldChange);

    m_categoriesList = new QListView();
    m_categoriesList->setModel(m_categoriesModel);
    m_categoriesList->setModelColumn(1);
    m_categoriesList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(m_categoriesList, SIGNAL(clicked(QModelIndex)),
            this, SLOT(onCategoriesListClicked(QModelIndex)));

    m_recordsModel = new QSqlTableModel();
    m_recordsModel->setTable("Records");
    m_recordsModel->select();
    m_recordsModel->setEditStrategy(QSqlTableModel::OnFieldChange);

    m_recordsList = new QListView();
    m_recordsList->setModel(m_recordsModel);
    m_recordsList->setModelColumn(2);
    m_recordsList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(m_recordsList, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(onRecordsListDoubleClicked(QModelIndex)));

    m_resultTextEdit = new QTextEdit();
    QFont viewFont = m_resultTextEdit->font();
    viewFont.setPointSize(14);
    m_resultTextEdit->setFont(viewFont);
    QSqlQuery *sel = new QSqlQuery();
    sel->prepare("SELECT id FROM Categories");
    sel->exec();
    while(sel->next()) {
        m_results.insert(sel->value(0).toInt(), "");
    }
    connect(m_resultTextEdit, SIGNAL(textChanged()), this, SLOT(onResultTextChanged()));

    QVBoxLayout *mainLayout = new QVBoxLayout();

    QToolBar *toolPanel = new QToolBar();

    QAction *previewAction = new QAction(QIcon(":/preview.png"), tr("Предпро&смотр"), this);
    connect(previewAction, SIGNAL(triggered()), this, SLOT(preview()));

    QAction *printAction = new QAction(QIcon(":/printer.png"), tr("&Печать"), this);
    connect(printAction, SIGNAL(triggered()), this, SLOT(printBase()));

    toolPanel->addAction(previewAction);
    toolPanel->addAction(printAction);

    QSplitter *splitVarsWithCategoriesResult = new QSplitter();

    QSplitter *splitCategoriesWithResult = new QSplitter();
    splitCategoriesWithResult->setOrientation(Qt::Vertical);

    splitCategoriesWithResult->addWidget(m_categoriesList);
    splitCategoriesWithResult->addWidget(m_resultTextEdit);

    splitVarsWithCategoriesResult->addWidget(splitCategoriesWithResult);

    QVBoxLayout *recordsPart = new QVBoxLayout();
    recordsPart->addWidget(m_recordsList);

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    QPushButton *addButton = new QPushButton(tr("Добавить запись"));
    connect(addButton, SIGNAL(clicked()), this, SLOT(onAddButtonClicked()));
    QPushButton *deleteButton = new QPushButton(tr("Удалить запись"));
    connect(deleteButton, SIGNAL(clicked()), this, SLOT(onDeleteButtonClicked()));
    buttonsLayout->addWidget(addButton);
    buttonsLayout->addWidget(deleteButton);

    recordsPart->addLayout(buttonsLayout);

    QWidget *recordsWidget = new QWidget();
    recordsWidget->setLayout(recordsPart);

    splitVarsWithCategoriesResult->addWidget(recordsWidget);

    mainLayout->addWidget(toolPanel);
    mainLayout->addWidget(splitVarsWithCategoriesResult);

    this->setLayout(mainLayout);
    this->setWindowTitle(tr("Денталь4You"));

    m_categoriesList->setCurrentIndex(m_categoriesList->indexAt(QPoint(m_categoriesList->x() + 5, m_categoriesList->y() + 5)));
    m_selectedId = 1;
    getRecords();
    getResults();

    m_teethWindow = new TeethFormulaDialog();
}

bool MainWindow::connectDB()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");

    db.setDatabaseName("databases/" + m_dbFileName);

    // Чистая формальность
    db.setUserName("pharret31");
    db.setHostName("localhost");
    db.setPassword("pharret31");

    if (!db.open())
    {
        QMessageBox *mb = new QMessageBox(QMessageBox::Critical,
                                          tr("Error","Error ConnectDB title"),
                                          tr("Can not connect to database",
                                             "Error ConectDB message")
                                          );
        mb->exec();
        return false;
    }

    return true;
}

int MainWindow::createDB()
{
    const QDir directory(":/scripts");

    // Да, да. Именно такой шаблон должны иметь sql-файлы
    // создания таблиц, чтобы попасть в базу
    const QRegExp regexFileName("[a-zA-Z]+-[0-9]+\\.[0-9]+\\.[Ss][Qq][Ll]");

    int errors = 0;

    QSqlQuery query;
    QString fileName;

    QFile scriptFile;
    QTextStream script;
    QString data;

    // Перебор всех файлов в директории и если файл
    // совпадёт с маской выполнить его как
    // sql-скрипт.
    // Добавление таблиц в базу.
    foreach (fileName, directory.entryList())
    {
        if (fileName.contains(regexFileName))
        {
            scriptFile.setFileName(":scripts/" + fileName);
            scriptFile.open(QIODevice::ReadOnly);
            script.setDevice(&scriptFile);
            script.setCodec("utf-8");
            data = script.readAll();
            scriptFile.close();

            if (!query.exec(data))
            {
                errors++;
            }
        }
    }

    //Заполнение таблицы категорий
    query.clear();
    query.prepare("INSERT INTO Categories (category) VALUES (?)");

    m_categories = "Жалобы;Анамнез;Объективные данные;"
                   "Зубная формула;Прикус;"
                   "Результаты рентгеновского и лабораторных исследований;"
                   "Диагноз;Рекомендованное лечение;Выполненное лечение;Заметки";
    QStringList categoriesList = m_categories.split(";");
    foreach (QString cat, categoriesList) {
        query.addBindValue(cat);
        query.exec();
    }


    return errors;
}

void MainWindow::getRecords()
{
    m_recordsModel->setFilter("categoryId = " + QString::number(m_selectedId));
    m_recordsModel->select();
}

void MainWindow::getResults()
{
    m_resultTextEdit->setText(m_results.value(m_selectedId));
}

bool MainWindow::eventFilter(QObject *obj, QEvent *evt)
{
    if (evt->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(evt);

        if (keyEvent->key() == Qt::Key_Delete) {
            if (m_recordsList->hasFocus()) {
                if (m_recordsList->currentIndex().isValid()) {
                    m_recordsModel->removeRow(m_recordsList->currentIndex().row());
                    m_recordsModel->select();
                }
            }
        }
    }

    return QWidget::eventFilter(obj, evt);
}

void MainWindow::closeEvent(QCloseEvent *)
{
    emit closed();
}

void MainWindow::onCategoriesListClicked(const QModelIndex &index)
{
    if (m_selectedId == -1) {
        return;
    }

    if (index.data().toString() == "Зубная формула") {
        m_teethWindow->show();
    }
    m_selectedId = index.model()->index(index.row(), 0).data().toInt();

    getRecords();
    getResults();
}

void MainWindow::onRecordsListDoubleClicked(const QModelIndex &index)
{
    if (m_selectedId == -1) {
        return;
    }

    m_resultTextEdit->moveCursor(QTextCursor::End);
    if (m_categoriesList->currentIndex().data() == "Выполненное лечение") {
        m_resultTextEdit->insertPlainText(index.data().toString() + "\n");
    }
    else {
        m_resultTextEdit->insertPlainText(index.data().toString() + " ");
    }
}

void MainWindow::onAddButtonClicked()
{
    if (m_selectedId == -1) {
        return;
    }

    QSqlQuery *insert = new QSqlQuery();
    insert->prepare("INSERT INTO Records"
                    "(categoryId, record)"
                    "VALUES"
                    "(?, ?)");

    bool ok = false;
    QString text = QInputDialog::getText(this, tr("Добавление записи"),
                                         tr("Введите запись"), QLineEdit::Normal, tr(""), &ok);

    if (ok && !text.isEmpty()) {
        insert->addBindValue(m_selectedId);
        insert->addBindValue(text);
        insert->exec();
        m_recordsModel->select();
    }
}

void MainWindow::onDeleteButtonClicked()
{
    if (m_selectedId == -1) {
        return;
    }

    m_recordsModel->removeRow(m_recordsList->currentIndex().row());
    m_recordsModel->select();
}

void MainWindow::onResultTextChanged()
{
    if (m_selectedId == -1) {
        return;
    }

    m_results[m_selectedId] = m_resultTextEdit->toPlainText();
}

void MainWindow::preview()
{
    Preview *preview = Preview::Instance();

    QSqlQuery selectCategory;
    selectCategory.prepare("SELECT * FROM Categories");
    selectCategory.exec();

    QString view = "<html><body><table border=\"0\"";
    while (selectCategory.next()) {
        view += "<tr><td width=\"30%\"><b>" + selectCategory.value(1).toString() + "</b></td>";
        view += "<td>";
        if (selectCategory.value(1).toString() == "Зубная формула") {
            view += "<table id=\"teeth\" border=\"1\">";
            for (int i = 0; i < 3; i++) {
                view += "<tr>";
                for (int j = 0; j < 16; j++) {
                    if (i == 1) {
                        view += "<td align=\"center\" bgcolor=\"grey\" width=\"30px\" style=\"color: black; border: 1px solid black;\">";
                    }
                    else {
                        view += "<td align=\"center\" width=\"30px\" style=\"border: 1px solid black;\">";
                    }
                    view += m_teethWindow->table->model()->index(i, j).data().toString();
                    view += "</td>";
                }
                view += "</tr>";
            }
            view += "</table>";
        }
        else {
            QStringList lines = m_results[selectCategory.value(0).toInt()].split("\n");
            lines.removeLast();
            if (lines.size() < 2) {
                view += m_results[selectCategory.value(0).toInt()];
            }
            else {
                for (int i = 0; i < lines.size(); i++) {
                    view += lines[i] + "<br /><br />";
                }
            }
        }
        view += "</td></tr>";
    }
    view += "</table></body></html>";

    preview->setHtmlText(view);
    preview->show();
}

void MainWindow::printBase()
{
    QSqlQuery selectCategory;
    selectCategory.prepare("SELECT * FROM Categories");
    selectCategory.exec();

    QString view = "<html><body><table border=\"0\"";
    while (selectCategory.next()) {
        view += "<tr><td width=\"30%\"><b>" + selectCategory.value(1).toString() + "</b></td>";
        view += "<td>";
        if (selectCategory.value(1).toString() == "Зубная формула") {
            view += "<table id=\"teeth\" border=\"1\">";
            for (int i = 0; i < 3; i++) {
                view += "<tr>";
                for (int j = 0; j < 16; j++) {
                    if (i == 1) {
                        view += "<td align=\"center\" bgcolor=\"grey\" width=\"30px\" style=\"color: black; border: 1px solid black;\">";
                    }
                    else {
                        view += "<td align=\"center\" width=\"30px\" style=\"border: 1px solid black;\">";
                    }
                    view += m_teethWindow->table->model()->index(i, j).data().toString();
                    view += "</td>";
                }
                view += "</tr>";
            }
            view += "</table>";
        }
        else {
            QStringList lines = m_results[selectCategory.value(0).toInt()].split("\n");
            lines.removeLast();
            if (lines.size() < 2) {
                view += m_results[selectCategory.value(0).toInt()];
            }
            else {
                for (int i = 0; i < lines.size(); i++) {
                    view += lines[i] + "<br /><br />";
                }
            }
        }
        view += "</td></tr>";
    }
    view += "</table></body></html>";

    QTextDocument *document = new QTextDocument();
    document->setHtml(view);

    QPrinter *printer = new QPrinter();

    QPrintDialog *dialog = new QPrintDialog(printer, this);
    if (dialog->exec() != QDialog::Accepted)
    {
        return;
    }

    document->print(printer);
}

MainWindow::~MainWindow()
{\
    delete m_categoriesModel;
    delete m_recordsModel;
}

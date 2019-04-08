#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::MainWindow)
{
    _ui->setupUi(this);

    _opt = new Options(this);

    createOptionsFile();
    readOptionsFile();

    connect(_opt,SIGNAL(setPath(QString)),this,SLOT(getPath(QString)));
    connect(_ui->compileButton,SIGNAL(clicked(bool)),this,SLOT(buildGame()));
    connect(_ui->gameId,SIGNAL(returnPressed()),this,SLOT(buildGame()));
}

MainWindow::~MainWindow()
{
    delete _ui;
}

//Слот вызывает окно выбора папки с проектом
void MainWindow::on_actionAdd_path_triggered()
{
    connect(_opt,SIGNAL(optionAlert(QString)),this,SLOT(getOptionAlert(QString)));
    _opt->show();
}

//Создает файл настроек если его нет
void MainWindow::createOptionsFile()
{
    QFile optionsFile(QApplication::applicationDirPath()+"/options.ini");
    if(!optionsFile.exists())
    {
        optionsFile.open(QIODevice::WriteOnly);
        optionsFile.close();
    }
}

//Читает файл настроек
void MainWindow::readOptionsFile()
{
    QFile optionsFile(QApplication::applicationDirPath()+"/options.ini");
    if(!optionsFile.open(QIODevice::ReadOnly))
    {
       _ui->statusBar->setStyleSheet("background: #f44242");
       _ui->statusBar->showMessage("Can't read options.ini");
       return;
    }
    QTextStream stream(&optionsFile);
    _mainDir = stream.readAll();
    _ui->gamePath->setText(_mainDir);
    optionsFile.flush();
    optionsFile.close();
}

//Слот читает из файла настроек в переменную путь к проекту
void MainWindow::getPath(const QString &str)
{
    _ui->gamePath->setText(str);
    _mainDir = str;
}

//Слот выводит сообщения от окна настроек в statusBar
void MainWindow::getOptionAlert(const QString &str)
{
    _ui->gamePath->setText(str);
    _mainDir = str;
}

//Слот выводит сообщения предупреждений в statusBar
void MainWindow::getAlert(const QString &str)
{
    _ui->statusBar->setStyleSheet("background: #f44242");
    _ui->statusBar->showMessage(str);
}

//Слот выводит сообщения о синтаксических ошибках js.list в statusBar
void MainWindow::getJslistAlert(const QString &str)
{
    _ui->statusBar->setStyleSheet("background: #f44242");
    _ui->statusBar->showMessage(str);
    _log = new LogMessage;
    connect(this,SIGNAL(logMessage(QString,QString)),_log,SLOT(message(QString,QString)));
    emit logMessage(str, "");
    _log->show();
    disconnect(this,SIGNAL(logMessage(QString,QString)),_log,SLOT(message(QString,QString)));
}

//Слот выводит ошибки минификации
void MainWindow::getLogAlert(const QString &str, const QString &id)
{
    _ui->statusBar->setStyleSheet("background: #f44242");
    _ui->statusBar->showMessage(str);

    QString logPath = QApplication::applicationDirPath()+"/log/";
    QFile logFile(logPath+id+".log");
    if(!logFile.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        _ui->statusBar->setStyleSheet("background: #f44242");
        _ui->statusBar->showMessage("Can't open log file - "+logPath+id+".log");
        return;
    }
    QStringList buffer;
    QString res;
    QString message;
    while(!logFile.atEnd())
    {
        buffer.append(logFile.readLine());
    }
    for(int i = 0; i < buffer.size(); ++i)
    {
        int counter = 0;
        res = buffer[i];
        if(res.contains("ERROR - "))
        {
            for(int j = i; j < buffer.size(); ++j)
            {
                res = buffer[j];
                if(!res.contains("^"))
                message += res;
                ++counter;
            }
        }
        if(counter > 0)
            break;
    }
    readLog(message, id);
    logFile.flush();
    logFile.close();
}

//Метод выводит ошибки из лог-файла в отдельное окно
void MainWindow::readLog(const QString &message, const QString &id)
{
    _log = new LogMessage;
    connect(this,SIGNAL(logMessage(QString,QString)),_log,SLOT(message(QString,QString)));
    emit logMessage(message, id);
    _log->show();
    disconnect(this,SIGNAL(logMessage(QString,QString)),_log,SLOT(message(QString,QString)));
}

//Слот выводит информационные сообщения в statusBar
void MainWindow::getInfo(const QString &str)
{
    _ui->statusBar->setStyleSheet("background: #f2f291");
    _ui->statusBar->showMessage(str);
}

//Слот выводит сообщения в statusBar
void MainWindow::allOk(const QString &str)
{
    _ui->statusBar->setStyleSheet("background: #64e571");
    _ui->statusBar->showMessage(str);
}

//Слот разблокирует кнопку по окончании компиляции одной игры
void MainWindow::endSingleCompile()
{
    delete _build;
    _ui->compileButton->setEnabled(true);
}

//Слот разблокирует кнопку по окончанию компиляции из диапазона
void MainWindow::endRangeCompile()
{
    delete _build;
    _ui->compileButton->setEnabled(true);
    QMessageBox::information(this, "compiler", "<font color='#051777'>Compilation is complete!!!</font>");
}

//Слот разблокирует кнопку по окончанию пакетной компиляции
void MainWindow::endMultiplyCompile()
{
    delete _build;
    _ui->compileButton->setEnabled(true);
    QMessageBox::information(this, "compiler", "<font color='#051777'>Compilation is complete!!!</font>");
}

//Слот собирает игру
void MainWindow::buildGame()
{
    QDir sourceDir(_mainDir+"/html5");
    if(!sourceDir.exists())
    {
        _ui->statusBar->setStyleSheet("background: #f44242");
        _ui->statusBar->showMessage("Can't find html5 folder!!!");
        return;
    }
    _gameId = _ui->gameId->text();
    if(!_gameId.isEmpty() && !_gameId.contains("-", Qt::CaseSensitive))
    {
        _ui->compileButton->setEnabled(false);
        _thread = new QThread(this);
        connect(this,SIGNAL(destroyed(QObject*)),_thread,SLOT(quit()));

        _build = new Mediator(_mainDir, _gameId);
        connect(this,SIGNAL(startSingleCompile()),_build,SLOT(runSingleCompile()));
        connect(_build,SIGNAL(destroyed(QObject*)),_build,SLOT(deleteLater()));
        connect(_build,SIGNAL(singleCompileComplete()),this,SLOT(endSingleCompile()));
        connect(_build,SIGNAL(buildAlert(QString)),this,SLOT(getAlert(QString)));
        connect(_build,SIGNAL(jslistAlert(QString)),this,SLOT(getJslistAlert(QString)));
        connect(_build,SIGNAL(logAlert(QString, QString)),this,SLOT(getLogAlert(QString, QString)));
        connect(_build,SIGNAL(copyInfo(QString)),this,SLOT(getInfo(QString)));
        connect(_build,SIGNAL(allOk(QString)),this,SLOT(allOk(QString)));
        _build->moveToThread(_thread);
        _thread->start();
        emit startSingleCompile();
    }
    else if(_gameId.contains("-", Qt::CaseSensitive))
    {
        _ui->compileButton->setEnabled(false);
        _thread = new QThread(this);
        connect(this,SIGNAL(destroyed(QObject*)),_thread,SLOT(quit()));

        _build = new Mediator(_mainDir, _gameId);
        connect(this,SIGNAL(startRangeCompile()),_build,SLOT(runRangeCompile()));
        connect(_build,SIGNAL(destroyed(QObject*)),_build,SLOT(deleteLater()));
        connect(_build,SIGNAL(rangeCompileComplete()),this,SLOT(endRangeCompile()));
        connect(_build,SIGNAL(buildAlert(QString)),this,SLOT(getAlert(QString)));
        connect(_build,SIGNAL(jslistAlert(QString)),this,SLOT(getJslistAlert(QString)));
        connect(_build,SIGNAL(logAlert(QString, QString)),this,SLOT(getLogAlert(QString, QString)));
        connect(_build,SIGNAL(copyInfo(QString)),this,SLOT(getInfo(QString)));
        connect(_build,SIGNAL(allOk(QString)),this,SLOT(allOk(QString)));
        _build->moveToThread(_thread);
        _thread->start();
        emit startRangeCompile();
    }
    else
    {
        if(sourceDir.entryList(QDir::Dirs|QDir::NoDotAndDotDot).isEmpty())
        {
            _ui->statusBar->setStyleSheet("background: #f44242");
            _ui->statusBar->showMessage("No games in html5!!!");
            return;
        }

        _ui->compileButton->setEnabled(false);
        _thread = new QThread(this);
        connect(this,SIGNAL(destroyed(QObject*)),_thread,SLOT(quit()));

        _build = new Mediator(_mainDir);
        connect(this,SIGNAL(startMultiplyCompile()),_build,SLOT(runMultiplyCompile()));
        connect(_build,SIGNAL(destroyed(QObject*)),_build,SLOT(deleteLater()));
        connect(_build,SIGNAL(multiplyCompileComplete()),this,SLOT(endMultiplyCompile()));
        connect(_build,SIGNAL(buildAlert(QString)),this,SLOT(getAlert(QString)));
        connect(_build,SIGNAL(jslistAlert(QString)),this,SLOT(getJslistAlert(QString)));
        connect(_build,SIGNAL(logAlert(QString, QString)),this,SLOT(getLogAlert(QString, QString)));
        connect(_build,SIGNAL(copyInfo(QString)),this,SLOT(getInfo(QString)));
        connect(_build,SIGNAL(allOk(QString)),this,SLOT(allOk(QString)));
        _build->moveToThread(_thread);
        _thread->start();
        emit startMultiplyCompile();
    }
    return;
}

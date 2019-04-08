#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "options.h"
#include "builder.h"
#include "mediator.h"
#include "logmessage.h"

#include <QDir>
#include <QFile>
#include <QThread>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QMainWindow>
#include <QMessageBox>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionAdd_path_triggered();
    void getPath(const QString &str);
    void getOptionAlert(const QString &str);
    void getAlert(const QString &str);
    void getJslistAlert(const QString &str);
    void getLogAlert(const QString &str, const QString &id);
    void getInfo(const QString &str);
    void allOk(const QString &str);
    void endSingleCompile();
    void endMultiplyCompile();
    void endRangeCompile();
    void buildGame();

private:
    Ui::MainWindow* _ui;
    LogMessage* _log;
    Options* _opt;
    QString _mainDir;
    QString _gameId;
    QThread* _thread;
    Mediator* _build;

    void createOptionsFile();
    void readOptionsFile();
    void readLog(const QString &message, const QString &id);

signals:
    void logMessage(const QString &message, const QString &id);
    void startSingleCompile();
    void startRangeCompile();
    void startMultiplyCompile();
};

#endif // MAINWINDOW_H

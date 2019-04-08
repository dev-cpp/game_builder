#ifndef MEDIATOR_H
#define MEDIATOR_H

#include "builder.h"

#include <QDir>
#include <QFile>
#include <QObject>
#include <QString>
#include <QStringList>

class Mediator : public QObject
{
    Q_OBJECT

public:
    Mediator();
    Mediator(const QString &path, const QString &id = "");
    ~Mediator();

public slots:
    void runSingleCompile();
    void runRangeCompile();
    void runMultiplyCompile();
    void getAlert(const QString &str);
    void getJslistAlert(const QString &str);
    void getLogAlert(const QString &str, const QString &id);
    void getInfo(const QString &str);
    void allOkSlot(const QString &str);

private:
    QString _basePath;
    QString _gameId;

signals:
    void buildAlert(const QString &str);
    void jslistAlert(const QString &str);
    void logAlert(const QString &str, const QString &id);
    void copyInfo(const QString &str);
    void allOk(const QString &str);
    void singleCompileComplete();
    void rangeCompileComplete();
    void multiplyCompileComplete();
};

#endif // MEDIATOR_H

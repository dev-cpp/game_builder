#ifndef BUILDER_H
#define BUILDER_H

#include <QDir>
#include <QFile>
#include <QObject>
#include <QString>
#include <QProcess>
#include <QTextStream>
#include <QApplication>

class Builder : public QObject
{
    Q_OBJECT

public:
    Builder();
    Builder(const QString &path, const QString &id);
    ~Builder();
    bool getVersion();
    bool copyGameFiles();
    bool copyCommonFiles();
    bool copyDir(const QString &lhs, const QString &rhs);
    bool copyDir(const QString &lhs, const QString &rhs, const QString &filter);
    bool gameMinify();
    void gameCompiler();

private:
    QString _basePath;
    QString _gameId;
    QString _ver;

signals:
    void buildAlert(const QString &str);
    void jslistAlert(const QString &str);
    void logAlert(const QString &str, const QString &id);
    void copyInfo(const QString &str);
    void allOk(const QString &str);
};

#endif // BUILDER_H

#include "mediator.h"

Mediator::Mediator()
{
    _basePath = "";
    _gameId = "";
}

Mediator::Mediator(const QString &path, const QString &id)
{
    _basePath = path;
    _gameId = id;
}

Mediator::~Mediator()
{

}

//Слот запускает компиляцию одной игры
void Mediator::runSingleCompile()
{
    Builder *build = new Builder(_basePath, _gameId);
    connect(build,SIGNAL(buildAlert(QString)),this,SLOT(getAlert(QString)));
    connect(build,SIGNAL(jslistAlert(QString)),this,SLOT(getJslistAlert(QString)));
    connect(build,SIGNAL(logAlert(QString, QString)),this,SLOT(getLogAlert(QString, QString)));
    connect(build,SIGNAL(copyInfo(QString)),this,SLOT(getInfo(QString)));
    connect(build,SIGNAL(allOk(QString)),this,SLOT(allOkSlot(QString)));
    build->gameCompiler();
    delete build;
    emit singleCompileComplete();
}

//Слот запускает компиляцию игр из диапазона
void Mediator::runRangeCompile()
{
    QStringList lst = _gameId.split("-");
    QString str = lst.at(0);
    int start = str.toInt();
    str = lst.at(1);
    int end = str.toInt();
    for(int i = start; i <= end; ++i)
    {
        str = QString::number(i);
        Builder *build = new Builder(_basePath, str);
        connect(build,SIGNAL(buildAlert(QString)),this,SLOT(getAlert(QString)));
        connect(build,SIGNAL(jslistAlert(QString)),this,SLOT(getJslistAlert(QString)));
        connect(build,SIGNAL(logAlert(QString, QString)),this,SLOT(getLogAlert(QString, QString)));
        connect(build,SIGNAL(copyInfo(QString)),this,SLOT(getInfo(QString)));
        connect(build,SIGNAL(allOk(QString)),this,SLOT(allOkSlot(QString)));
        build->gameCompiler();
        delete build;
    }
    emit rangeCompileComplete();
}

//Слот запускает пакетную компиляцию игр
void Mediator::runMultiplyCompile()
{
    QDir sourceDir(_basePath+"/html5");
    foreach(QFileInfo info, sourceDir.entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot))
    {
        if(info.baseName() != "common")
        {
            Builder *build = new Builder(_basePath, info.baseName());
            connect(build,SIGNAL(buildAlert(QString)),this,SLOT(getAlert(QString)));
            connect(build,SIGNAL(jslistAlert(QString)),this,SLOT(getJslistAlert(QString)));
            connect(build,SIGNAL(logAlert(QString, QString)),this,SLOT(getLogAlert(QString, QString)));
            connect(build,SIGNAL(copyInfo(QString)),this,SLOT(getInfo(QString)));
            connect(build,SIGNAL(allOk(QString)),this,SLOT(allOkSlot(QString)));
            build->gameCompiler();
            delete build;
        }
    }
    emit multiplyCompileComplete();
}

void Mediator::getAlert(const QString &str)
{
    emit buildAlert(str);
}

void Mediator::getJslistAlert(const QString &str)
{
    emit jslistAlert(str);
}

void Mediator::getLogAlert(const QString &str, const QString &id)
{
    emit logAlert(str, id);
}

void Mediator::getInfo(const QString &str)
{
    emit copyInfo(str);
}

void Mediator::allOkSlot(const QString &str)
{
    emit allOk(str);
}

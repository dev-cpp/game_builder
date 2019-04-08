#include "builder.h"

Builder::Builder()
{
    _basePath = "";
    _gameId = "";
}

Builder::Builder(const QString &path, const QString &id)
{
    _basePath = path;
    _gameId = id;
}

Builder::~Builder()
{

}

//Метод читает версию игры из файла build.list и присваивает значение переменной _ver
bool Builder::getVersion()
{
    QFile buildFile(_basePath+"/html5/"+_gameId+"/build.list");
    if(!buildFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit buildAlert("Can't open build.list - "+_basePath+"/html5/"+_gameId+"/build.list");
        return false;
    }
    QTextStream stream(&buildFile);
    _ver = stream.readAll();
    buildFile.flush();
    buildFile.close();
    return true;
}

//Метод копирования папок с содержимым
bool Builder::copyDir(const QString &lhs, const QString &rhs)
{
    QDir sourseDir(lhs);
    if(!sourseDir.exists())
    {
        emit buildAlert("Can't find - "+lhs);
        return false;
    }
    QDir destDir(rhs);
    if(!destDir.exists())
    {
        if(!destDir.mkdir(rhs))
        {
            emit buildAlert("Can't create folder - "+rhs);
            return false;
        }
    }
    foreach(QFileInfo info, sourseDir.entryInfoList(QDir::Files|QDir::Dirs|QDir::NoDotAndDotDot, QDir::DirsLast))
    {
        QString copyPath = info.filePath().replace(sourseDir.absolutePath(), destDir.absolutePath());
        if(info.isDir())
        {
            copyDir(info.absoluteFilePath(), copyPath);
        }
        if(info.isFile())
        {
            QFile::remove(copyPath);
            if(!QFile::copy(info.absoluteFilePath(), copyPath))
            {
                emit buildAlert("Can't copy file from: "+info.absoluteFilePath()+" to: "+copyPath);
                return false;
            }
        }
    }
    return true;
}

//Метод копирует папки с файлами по фильтру
bool Builder::copyDir(const QString &lhs, const QString &rhs, const QString &filter)
{
    QDir sourseDir(lhs);
    if(!sourseDir.exists())
    {
        emit buildAlert("Can't find - "+lhs);
        return false;
    }
    QDir destDir(rhs);
    if(!destDir.exists())
    {
        if(!destDir.mkdir(rhs))
        {
            emit buildAlert("Can't create folder - "+rhs);
            return false;
        }
    }
    foreach(QFileInfo info, sourseDir.entryInfoList(QDir::Files|QDir::Dirs|QDir::NoDotAndDotDot, QDir::DirsLast))
    {
        QString copyPath = info.filePath().replace(sourseDir.absolutePath(), destDir.absolutePath());
        if(info.isDir())
        {
            copyDir(info.absoluteFilePath(), copyPath, filter);
        }
        if(info.absoluteFilePath().contains(filter, Qt::CaseInsensitive))
        {
            QFile::remove(copyPath);
            if(!QFile::copy(info.absoluteFilePath(), copyPath))
            {
                emit buildAlert("Can't copy file from: "+info.absoluteFilePath()+" to: "+copyPath);
                return false;
            }
        }
    }
    return true;
}

//Метод создает папки и копирует файлы необходимые для игры
bool Builder::copyGameFiles()
{
    emit copyInfo("Game ID: "+_gameId+"   Copying of files is begun");

    if(!copyDir(_basePath+"/html5/"+_gameId+"/"+_ver+"/images", _basePath+"/html5prod/"+_gameId+"/images"))
        return false;
    if(!copyDir(_basePath+"/html5/"+_gameId+"/"+_ver+"/audio", _basePath+"/html5prod/"+_gameId+"/audio"))
        return false;
    if(!copyDir(_basePath+"/html5/"+_gameId+"/"+_ver+"/fonts", _basePath+"/html5prod/"+_gameId+"/fonts"))
        return false;
    if(!copyDir(_basePath+"/html5/"+_gameId+"/"+_ver+"/style", _basePath+"/html5prod/"+_gameId+"/style", ".css"))
        return false;

    QFile::remove(_basePath+"/html5prod/"+_gameId+"/apple-touch-icon.png");
    if(!QFile::copy(_basePath+"/html5/"+_gameId+"/"+_ver+"/apple-touch-icon.png", _basePath+"/html5prod/"+_gameId+"/apple-touch-icon.png"))
    {
        emit buildAlert("Can't copy file from: "+_basePath+"/html5/"+_gameId+"/"+_ver+"/apple-touch-icon.png"+" to: "+_basePath+"/html5prod/"+_gameId+"/apple-touch-icon.png");
        return false;
    }

    QFile::remove(_basePath+"/html5prod/"+_gameId+"/index.htm");
    if(!QFile::copy(_basePath+"/html5/"+_gameId+"/"+_ver+"/prod.htm", _basePath+"/html5prod/"+_gameId+"/index.htm"))
    {
        emit buildAlert("Can't copy file from: "+_basePath+"/html5/"+_gameId+"/"+_ver+"/prod.htm"+" to: "+_basePath+"/html5prod/"+_gameId+"/index.htm");
        return false;
    }

    emit copyInfo("Game ID: "+_gameId+"   Copying of files is finished");
    return true;
}

bool Builder::copyCommonFiles()
{
    emit copyInfo("Game ID: "+_gameId+"   Copying common files is begun");

    QDir mDir(_basePath+"/html5prod/common");
    if(!mDir.exists())
    {
        if(!mDir.mkdir(_basePath+"/html5prod/common"))
        {
            emit buildAlert("Can't create folder - "+_basePath+"/html5prod/common");
            return false;
        }
    }
    if(!copyDir(_basePath+"/html5/common/images", _basePath+"/html5prod/common/images"))
        return false;
    if(!copyDir(_basePath+"/html5/common/fonts", _basePath+"/html5prod/common/fonts"))
        return false;
    mDir.setPath(_basePath+"/html5prod/common/"+_ver);
    if(!mDir.exists())
    {
        if(!mDir.mkdir(_basePath+"/html5prod/common/"+_ver))
        {
            emit buildAlert("Can't create folder - "+_basePath+"/html5prod/common"+_ver);
            return false;
        }
    }
    mDir.setPath(_basePath+"/html5prod/common/"+_ver+"/vendor");
    if(!mDir.exists())
    {
        if(!mDir.mkdir(_basePath+"/html5prod/common/"+_ver+"/vendor"))
        {
            emit buildAlert("Can't create folder - "+_basePath+"/html5prod/common"+_ver+"/vendor");
            return false;
        }
    }
    if(!copyDir(_basePath+"/common/"+_ver+"/js/vendor", _basePath+"/html5prod/common/"+_ver+"/vendor"))
        return false;
    QDir sDir(_basePath+"/common/"+_ver+"/common_audio");
    if(sDir.exists())
    {
        mDir.setPath(_basePath+"/html5prod/common/"+_ver+"/common_audio");
        if(!mDir.exists())
        {
            if(!mDir.mkdir(_basePath+"/html5prod/common/"+_ver+"/common_audio"))
            {
                emit buildAlert("Can't create folder - "+_basePath+"/html5prod/common"+_ver+"/common_audio");
                return false;
            }
        }
        if(!copyDir(_basePath+"/common/"+_ver+"/common_audio", _basePath+"/html5prod/common/"+_ver+"/common_audio"))
            return false;
    }
    sDir.setPath(_basePath+"/common/"+_ver+"/common_images");
    if(sDir.exists())
    {
        mDir.setPath(_basePath+"/html5prod/common/"+_ver+"/common_images");
        if(!mDir.exists())
        {
            if(!mDir.mkdir(_basePath+"/html5prod/common/"+_ver+"/common_images"))
            {
                emit buildAlert("Can't create folder - "+_basePath+"/html5prod/common"+_ver+"/common_images");
                return false;
            }
        }
        if(!copyDir(_basePath+"/common/"+_ver+"/common_images", _basePath+"/html5prod/common/"+_ver+"/common_images"))
            return false;
    }

    emit copyInfo("Game ID: "+_gameId+"   Copying common files is finished");
    return true;
}

//Метод собирает min.js
bool Builder::gameMinify()
{
    emit copyInfo("Game ID: "+_gameId+"   Start game minify");

    QDir mDir(_basePath+"/html5prod");
    if(!mDir.exists())
    {
        if(!mDir.mkdir(_basePath+"/html5prod"))
        {
            emit buildAlert("Can't create folder - "+_basePath+"/html5prod");
            return false;
        }
    }
    mDir.setPath(_basePath+"/html5prod/"+_gameId);
    if(mDir.exists())
    {
        mDir.removeRecursively();
    }
    if(!mDir.mkdir(_basePath+"/html5prod/"+_gameId))  //создаем папку игры
    {
        emit buildAlert("Can't create folder - "+_basePath+"/html5prod/"+_gameId);
        return false;
    }

    if(!mDir.mkdir(_basePath+"/html5prod/"+_gameId+"/js"))    //создаем папку в которую положим минифицированный js файл
    {
        emit buildAlert("Can't create folder - "+_basePath+"/html5prod/"+_gameId+"/js");
        return false;
    }

    QString compilationLevel = "--compilation_level ADVANCED_OPTIMIZATIONS";
    QString externs = "--externs "+_basePath+"/common/"+_ver+"/js/externs.js";
    QString js = "--js ";
    QString jsOutput = "--js_output_file "+_basePath+"/html5prod/"+_gameId+"/js/min.js";
    QString warningLevel = "--warning_level _verBOSE";
    QFile jsList(_basePath+"/html5/"+_gameId+"/"+_ver+"/js.list");
    if(!jsList.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit buildAlert("Can't open js.list - "+_basePath+"/html5/"+_gameId+"/"+_ver+"/js.list");
        return false;
    }
    QTextStream stream(&jsList);
    js += stream.readAll();
    if(js.contains("/"))
    {
        emit jslistAlert("Game ID: "+_gameId+"   Syntax error in js.list");
        return false;
    }
    js.replace("\\", "/");
    js.remove("^");
    js.replace("%cd%/../../../", _basePath+"/");
    js.replace("%cd%", _basePath+"/html5/"+_gameId+"/"+_ver);
    js = js.simplified();
    jsList.flush();
    jsList.close();

    QString appPath = QApplication::applicationDirPath();
    QString compilerArguments = compilationLevel+" "+externs+" "+js+" "+jsOutput+" "+warningLevel;
    QString javaPath = appPath+"/java/bin/java -jar ";
    QString compilerPath = appPath+"/compiler/closure-compiler.jar ";
    QString compile = javaPath+compilerPath+compilerArguments;

    QDir logDir(appPath+"/log");
    if(!logDir.exists())
    {
        if(!logDir.mkdir(appPath+"/log"))
        {
            emit buildAlert("Can't create folder - "+appPath+"/log");
            return false;
        }
    }
    QFile logFile(appPath+"/log/"+_gameId+".log");
    if(!logFile.open(QIODevice::WriteOnly))
    {
        emit buildAlert("Can't create log file - "+appPath+"/log/"+_gameId+".log");
        return false;
    }
    logFile.close();

    QProcess process(this);
    process.setStandardErrorFile(appPath+"/log/"+_gameId+".log");
    process.start(compile);
    process.waitForFinished();
    int errorCode = process.exitCode();
    QString error = QString::number(errorCode);

    if(errorCode != 0)
    {
        emit logAlert("Can't minify js files in game: "+_gameId+"   Error code: "+error, _gameId);
        mDir.removeRecursively();
        return false;
    }
    if(!QFile::exists(_basePath+"/html5prod/"+_gameId+"/js/min.js"))
    {
        emit logAlert("Can't find min.js in game: "+_gameId, _gameId);
        return false;
    }
    emit copyInfo("Game ID: "+_gameId+"   End game minify");
    return true;
}

//Метод минифицирует и собирает игру
void Builder::gameCompiler()
{
    if(getVersion())
    {
        if(gameMinify())
        {
            if(copyGameFiles())
            {
                if(copyCommonFiles())
                {
                    emit allOk("Game ID: "+_gameId+"    Build complete successfully!!!");
                    return;
                }
            }
        }
    }
}

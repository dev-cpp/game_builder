#include "options.h"
#include "ui_options.h"

Options::Options(QWidget *parent) :
    QDialog(parent),
    _ui(new Ui::Options)
{
    _ui->setupUi(this);

    connect(_ui->pathButton,SIGNAL(clicked(bool)),this,SLOT(openFileDialog()));
    connect(_ui->okButton,SIGNAL(clicked(bool)),this,SLOT(savePath()));
}

Options::~Options()
{
    delete _ui;
}

//Открывает файловый диалог для выбора папки с проектом
void Options::openFileDialog()
{
    //Для линукса меняем "C:/" на "/"
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "C:/", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    _ui->lineEdit->setText(dir);
}

//Сохраняет путь к папке с проектом в файл настроек
void Options::savePath()
{
    QString str = _ui->lineEdit->text();
    if(!str.isEmpty())
    {
        QFile optionsFile(QApplication::applicationDirPath()+"/options.ini");
        if(!optionsFile.open(QIODevice::WriteOnly))
        {
            emit optionAlert("Can't write options.ini");
            return;
        }
        QTextStream stream(&optionsFile);
        stream << str;
        optionsFile.flush();
        optionsFile.close();
        emit setPath(_ui->lineEdit->text());
    }
    close();
}

#ifndef OPTIONS_H
#define OPTIONS_H

#include <QDir>
#include <QFile>
#include <QDialog>
#include <QString>
#include <QFileDialog>
#include <QTextStream>

namespace Ui {
class Options;
}

class Options : public QDialog
{
    Q_OBJECT

public:
    explicit Options(QWidget *parent = nullptr);
    ~Options();

private slots:
    void openFileDialog();
    void savePath();

private:
    Ui::Options* _ui;

signals:
    void setPath(const QString &str);
    void optionAlert(const QString &str);
};

#endif // OPTIONS_H

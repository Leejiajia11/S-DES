#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;

    // 声明 encrypt 函数
    QString encrypt(const QString &plaintext, const QString &key);

    QString applyInitialPermutation(const QString &input);
    QString applyFinalPermutation(const QString &input);
    QString keyGeneration(const QString &key);
    QString leftShift(const QString &key, int shifts);
    QString fFunction(const QString &right, const QString &key);
    QString sBoxSubstitution(const QString &input);
    QString toBinary(const QString &input);
    QString fromBinary(const QString &binary);
    bool isBinaryString(const QString &str);
    QString toBase64(const QString &binary);
    int binaryToDecimal(const QString &binary, bool *ok = nullptr);
    QString toAsciiString(const QString &binary);
};

#endif // MAINWINDOW_H

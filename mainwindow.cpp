#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QChar>
#include <cmath>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//点击按钮，提交8bit的数据和10bit的密钥，开始加密
void MainWindow::on_pushButton_clicked()
{

    QString plaintext = ui->Plaintext->text(); // 获取明文
    QString key = ui->Key->text(); // 获取密钥

    //判断输入
    if(isBinaryString(plaintext)){
        //输入是二进制
        // 检查输入长度
        if (plaintext.length() != 8 || key.length() != 10) {
            ui->tip2->setText("错误：明文必须为8位，密钥必须为10位。");
            return;
        }
        else {
            ui->tip2->setText(" ");
            // 进行加密操作
            QString cipherText = encrypt(plaintext, key); // 加密函数

            // 显示密文
            ui->output->setPlainText(cipherText);
            ui->output->setReadOnly(true); // 使输出框只读

            // 设置 tip QLabel 的文本
              ui->tip->setText("加密完成");

            return;
        }
    }

     //输入是ASCII字符串
    else{
       QString binary = toBinary(plaintext); // 转换为二进制
       QString cipherText;

       //将转换为的二进制每8bit分为一个块
       for (int i = 0; i < binary.length(); i += 8) {
                  QString byteString;
                  if (i + 8 > binary.length()) {
                          byteString = binary.mid(i); // 获取剩余部分
                      } else {
                          byteString = binary.mid(i, 8); // 获取当前的8位二进制块
                      }
          if (byteString.length() < 8) {
           ui->tip2->setText("错误：输入的 ASCII 字符串转换的二进制不应小于 8 位。");
           return;
          }

          //再将每一个8bit的二进制块，依次输入只能处理8bit二进制输入的加密函数encrypt(,key)
          QString encryptedByte = encrypt(byteString, key); // 每个 8 位块进行加密
          cipherText.append(encryptedByte); // 将加密后的结果追加到密文中
          qDebug()<<"二进制："<<cipherText;

      }


       //最后将经过加密后的二进制转换为ASCII，按照顺序输出为ASCII字符串
       QString asciiOutput = toAsciiString(cipherText);

       //在UI中显示
       ui->output->setPlainText(asciiOutput);
       ui->output->setReadOnly(true); // 使输出框只读

       // 设置 tip QLabel 的文本
       ui->tip->setText("加密完成");

    }

}


// 加密函数encrypt
QString MainWindow::encrypt(const QString &plaintext, const QString &key) {
    //读取输入的key，得到k1，k2
    QString k1=keyGeneration(key).mid(0,8);
    QString k2=keyGeneration(key).mid(8,8);
    qDebug() << "k1:" << k1;
   qDebug() << "k2:" << k2;

    //初始置换
    QString permuted = applyInitialPermutation(plaintext);
        qDebug() << "初始置换:" << permuted;
       QString left = permuted.left(4);
       QString right = permuted.mid(4);

       QString R1=right;
       QString output1=fFunction(right,k1);
           qDebug() << "轮函数：" << output1;
       QString L1;
       for (int i=0;i<4;++i) {
           // 确保使用 QChar 的 unicode 方法获取字符的整数值
            int output1Value = output1[i].unicode() - '0'; // 转换为整数
            int leftValue = left[i].unicode() - '0';     // 转换为整数
            // 执行异或并转换回字符
               L1[i]=QChar((output1Value ^ leftValue) + '0');
       }

       //Swap->第二轮
        QString L2=R1;
        QString R2=L1;

        QString output2=fFunction(R2,k2);
        QString L;
        for (int i=0;i<4;++i) {
            // 确保使用 QChar 的 unicode 方法获取字符的整数值
             int output2Value = output2[i].unicode() - '0'; // 转换为整数
             int L2Value = L2[i].unicode() - '0';     // 转换为整数
             // 执行异或并转换回字符
                 L[i]=(QChar((output2Value ^ L2Value) + '0'));

        }
        QString R=R2;
        QString Output=L+R;

        //返回密文
         return applyFinalPermutation( Output);
}

// 密钥生成
QString MainWindow::keyGeneration(const QString &key) {
    // P10置换
    static const int P10[] = {3, 5, 2, 7, 4, 10, 1, 9, 8, 6};
    QString permuted(10, '0');
    for (int i = 0; i < 10; ++i) {
        permuted[i] = key[P10[i] - 1];
    }

    // 左移生成K1和K2(现在是10bit）
    QString k_1 = leftShift(permuted.mid(0, 5), 1) + leftShift(permuted.mid(5, 5), 1);
    QString k_2 = leftShift(permuted.mid(0, 5), 2) + leftShift(permuted.mid(5, 5), 2);

    //P8置换
    static const int P8[]={6,3,7,4,8,5,10,9};
    QString k1(8,'0');
    QString k2(8,'0');
    for(int i=0;i<8;++i){
        k1[i]=k_1[P8[i]-1];
        k2[i]=k_2[P8[i]-1];
    }

    return k1+k2;
}

QString MainWindow::leftShift(const QString &key, int shifts) {
    QString shifted = key.mid(shifts) + key.mid(0, shifts);
    return shifted;
}


// 初始置换
QString MainWindow::applyInitialPermutation(const QString &input) {
    static const int IP[] = {2, 6, 3, 1, 4, 8, 5, 7};
    QString output(8, '0');
    for (int i = 0; i < 8; ++i) {
        output[i] = input[IP[i] - 1];
    }
    return output;
}

// 轮函数F
QString MainWindow::fFunction(const QString &right, const QString &key) {
    //扩展4到8bit
    static const int EP[]={4,1,2,3,2,3,4,1};
    QString R8bit(8,'0');
    for(int i=0;i<8;++i){
         R8bit[i]=right[EP[i]-1];
    }
        qDebug() << "扩展:" << R8bit;

    //与k异或
     QString xored(8,'0');
    for (int i=0;i<8;++i) {
        // 确保使用 QChar 的 unicode 方法获取字符的整数值
         int r8bitValue = R8bit[i].unicode() - '0'; // 转换为整数
         int keyValue = key[i].unicode() - '0';     // 转换为整数
         // 执行异或并转换回字符
            xored[i] = QChar((r8bitValue ^ keyValue) + '0');
            // xored.append(QChar((r8bitValue ^ keyValue) + '0'));
    }
        qDebug() << "异或：" <<  xored;

    // S-Box替换
       QString substituted = sBoxSubstitution(xored);
        qDebug() << "Sbox：" <<  substituted;
       // P-Box置换
       static const int SP[] = {2, 4, 3, 1};
       QString output(4, '0');
       for (int i = 0; i < 4; ++i) {
           output[i] = substituted[SP[i] - 1];
       }
        qDebug() << "置换：" <<  output;
       return output;
}


// S-Box替换
QString MainWindow::sBoxSubstitution(const QString &input) {
    static const int SBox1[4][4] = {
        {1, 0, 3, 2},
        {3, 2, 1, 0},
        {0, 2, 1, 3},
        {3, 1, 0, 2}
    };
    static const int SBox2[4][4] = {
        {0, 1, 2, 3},
        {2, 3, 1, 0},
        {3, 0, 1, 2},
        {2, 1, 0, 3}
    };

    int row1 = (input[0].digitValue() << 1) + input[3].digitValue();
    int col1 = (input.mid(1, 2)).toInt(nullptr, 2);
    int row2 = (input[4].digitValue() << 1) + input[7].digitValue();
    int col2 = (input.mid(5, 2)).toInt(nullptr, 2);

    QString output;
    output.append(QString::number(SBox1[row1][col1], 2).rightJustified(2, '0'));
    output.append(QString::number(SBox2[row2][col2], 2).rightJustified(2, '0'));
    //QString::number(..., 2) 将整数转换为二进制字符串。
    //rightJustified(2, '0') 确保结果为两位，不足的前面补零。

    return output;
}


// 最终置换
QString MainWindow::applyFinalPermutation(const QString &input) {
    static const int IP_inv[] = {4, 1, 3, 5, 7, 2, 8, 6};
    QString output(8, '0');
    for (int i = 0; i < 8; ++i) {
        output[i] = input[IP_inv[i] - 1];
    }
    return output;
}


/***********************扩展功能*********************/
//ASCII转换为二进制
QString MainWindow::toBinary(const QString& input) {
    QString binaryString;
    for (QChar ch : input) {
        binaryString.append(QString::number(ch.unicode(), 2).rightJustified(8, '0'));
    }
    return binaryString;
}

//输出转换为ASCII
QString MainWindow::toAsciiString(const QString &binary) {
    QByteArray byteArray;

    // 确保 binary 的长度是 8 的倍数
    int length = binary.length();
    if (length % 8 != 0) {
        return QString(); // 可以选择抛出异常或返回空字符串
    }

    // 将二进制字符串转换为字节
    for (int i = 0; i < length; i += 8) {
        QString byteString = binary.mid(i, 8);
        bool ok;
        quint8 byteValue = static_cast<quint8>(binaryToDecimal(byteString, &ok));
        if (ok) {
            byteArray.append(static_cast<char>(byteValue)); // 显式转换避免警告
        }
    }

    // 使用 Base64 编码输出
      return QString(byteArray);
}

//判断输入
bool MainWindow::isBinaryString(const QString& str) {
    // 判断字符串是否只包含0和1
    for (QChar ch : str) {
        if (ch != '0' && ch != '1') {
            return false;
        }
    }
    return true;
}

int MainWindow::binaryToDecimal(const QString &binary, bool *ok) {
    if (ok) *ok = true; // 默认设置为成功

    // 检查输入合法性
    if (binary.isEmpty()) {
        if (ok) *ok = false; // 输入为空
        return -1;
    }

    int decimalValue = 0;
    int length = binary.length();

    for (int i = 0; i < length; ++i) {
        QChar bit = binary[length - 1 - i]; // 从右到左读取
        if (bit == '1') {
            decimalValue += (1 << i); // 使用位移操作代替 pow(2, i)
        } else if (bit != '0') {
            if (ok) *ok = false; // 非法字符
            return -1; // 返回错误值
        }
    }
    return decimalValue; // 返回最终的十进制值
}

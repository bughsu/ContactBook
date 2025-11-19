#include "widget.h"
#include "ui_widget.h"
#include <QTableWidgetItem>
#include <QString>
#include <QStringList> // 包含 QStringList
#include <QFile>
#include <QDebug>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
QString mfilename="D:/test/mfile.txt";
void Write(QString filename,QString str)
{
    QFile mfile(filename);
    if(!mfile.open(QFile::WriteOnly | QFile::Text)){
        qDebug()<<"could not op file for write";
        return;
    }
    QTextStream out(&mfile);
    out<<str;
    mfile.flush();
    mfile.close();
}
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    // --- QTableWidget 初始化 ---
    QStringList ColTotle;
    // 設置欄位數為 4
    ui->tableWidget->setColumnCount(4);
    // 設定欄位標題
    ColTotle << QStringLiteral("學號") << QStringLiteral("班級") << QStringLiteral("姓名") << QStringLiteral("電話");
    ui->tableWidget->setHorizontalHeaderLabels(ColTotle);

    // 可選：允許自動調整欄寬
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButton_clicked()
{
    // --- 1. 宣告 QTableWidgetItem 指標 ---
    QTableWidgetItem *inputRow1, *inputRow2, *inputRow3, *inputRow4;

    // --- 2. 從 QLineEdit 中獲取文字並創建 QTableWidgetItem ---
    inputRow1 = new QTableWidgetItem(ui->lineEdit->text());   // 假設對應 學號
    inputRow2 = new QTableWidgetItem(ui->lineEdit_2->text()); // 假設對應 班級
    inputRow3 = new QTableWidgetItem(ui->lineEdit_3->text()); // 假設對應 姓名
    inputRow4 = new QTableWidgetItem(ui->lineEdit_4->text()); // 假設對應 電話

    // --- 3. 將資料插入 QTableWidget (修正部分) ---

    // 獲取當前表格的行數 (這將是新行的索引)
    int currentRow = ui->tableWidget->rowCount();

    // 插入新的一行
    ui->tableWidget->insertRow(currentRow);

    // 將創建的 QTableWidgetItem 設置到新行的各個欄位 (Column 0, 1, 2, 3)
    ui->tableWidget->setItem(currentRow, 0, inputRow1); // 學號 (Column 0)
    ui->tableWidget->setItem(currentRow, 1, inputRow2); // 班級 (Column 1)
    ui->tableWidget->setItem(currentRow, 2, inputRow3); // 姓名 (Column 2)
    ui->tableWidget->setItem(currentRow, 3, inputRow4); // 電話 (Column 3)

    // --- 4. 清空輸入框 (增強功能) ---
    ui->lineEdit->clear();
    ui->lineEdit_2->clear();
    ui->lineEdit_3->clear();
    ui->lineEdit_4->clear();
}

void Widget::on_pushButton_2_clicked()
{
    // 1. 调用 QFileDialog::getSaveFileName 获取用户选择的文件路径
    QString mfilename = QFileDialog::getSaveFileName(
        this,                                          // 父窗口 (this)
        tr("Save Table Data"),                         // 对话框标题
        "",                                            // 默认目录 (空字符串表示当前目录或默认位置)
        tr("CSV Files (*.csv);;All Files (*)")         // 文件过滤器
        );

    // 2. 检查用户是否取消了操作
    if (mfilename.isEmpty()) {
        // 如果文件路径为空，表示用户取消了保存操作，直接返回
        return;
    }

    // 3. 将 tableWidget 中的数据格式化为字符串
    QString savefile = "";
    for(int i = 0; i < ui->tableWidget->rowCount(); i++)
    {
        for(int j = 0; j < ui->tableWidget->columnCount(); j++)
        {
            // 确保单元格不为空，如果为空，则写入空字符串
            QTableWidgetItem* item = ui->tableWidget->item(i, j);
            if (item) {
                savefile += item->text();
            }

            // 在每一列数据后添加逗号分隔符，但避免在最后一列后添加
            if (j < ui->tableWidget->columnCount() - 1) {
                savefile += ",";
            }
        }
        // 在每一行数据后添加换行符
        savefile += "\n";
    }

    // 4. 使用获取到的文件名保存数据
    // 确保 Write 函数是存在的，并且能够处理文件写入操作
    Write(mfilename, savefile);
}


void Widget::on_pushButton_3_clicked()
{
    // 1. 彈出檔案選擇對話框 (使用 QFileDialog::getOpenFileName)
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("開啟通訊錄資料檔案"), // 對話框標題
                                                    "", // 預設路徑 (空字串表示預設)
                                                    tr("資料檔案 (*.txt);;所有檔案 (*.*)") // 檔案過濾器
                                                    );

    // 檢查使用者是否選擇了檔案
    if (fileName.isEmpty()) {
        return; // 如果沒有選擇檔案，則退出函數
    }

    // 2. 準備讀取檔案
    QFile file(fileName);

    // 嘗試以只讀和文本模式開啟檔案
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, tr("檔案錯誤"), tr("無法開啟檔案: %1").arg(file.errorString()));
        return;
    }

    // 3. 讀取並解析檔案內容
    QTextStream in(&file);
    int successfullyLoadedRows = 0;

    // 逐行讀取檔案直到檔案結束
    while (!in.atEnd()) {
        QString line = in.readLine();

        // 使用逗號 (,) 分割字串，假設資料格式為：學號,班級,姓名,電話
        QStringList fields = line.split(',');

        // 確保至少有 4 個欄位 (學號, 班級, 姓名, 電話)
        if (fields.size() >= 4) {
            // 獲取當前表格的行數 (即新行要插入的位置)
            int currentRow = ui->tableWidget->rowCount();

            // 插入新的一行
            ui->tableWidget->insertRow(currentRow);

            // 創建 QTableWidgetItem 並插入到表格中
            // 欄位 0: 學號
            ui->tableWidget->setItem(currentRow, 0, new QTableWidgetItem(fields.at(0).trimmed()));
            // 欄位 1: 班級
            ui->tableWidget->setItem(currentRow, 1, new QTableWidgetItem(fields.at(1).trimmed()));
            // 欄位 2: 姓名
            ui->tableWidget->setItem(currentRow, 2, new QTableWidgetItem(fields.at(2).trimmed()));
            // 欄位 3: 電話
            ui->tableWidget->setItem(currentRow, 3, new QTableWidgetItem(fields.at(3).trimmed()));

            successfullyLoadedRows++;
        }
        // 如果欄位不足 4 個，該行資料將被跳過
    }

    // 4. 關閉檔案
    file.close();

    // 5. 顯示結果訊息
    QMessageBox::information(this, tr("匯入完成"),
                             tr("檔案 %1 已載入，成功新增 %2 筆資料到表格。")
                                 .arg(QFileInfo(fileName).fileName()) // 顯示檔名
                                 .arg(successfullyLoadedRows)
                             );
}


void Widget::on_pushButton_4_clicked()
{
    QApplication::quit();
}

#include "help.h"
#include "ui_help.h"
#include<QDebug>
#include<QMessageBox>

Help::Help(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Help)
{
    ui->setupUi(this);
    this->setWindowTitle("使用说明");
    this->setFixedSize(600,600);
    this->setWindowIcon(QIcon(QPixmap(":/data/icon/help.png")));

    ui->tabWidget->setFixedSize(this->width(),this->height());
    ui->tB_Toolbar->setFixedSize(ui->tabWidget->width(),ui->tabWidget->height());
    ui->tB_View->setFixedSize(ui->tabWidget->width(),ui->tabWidget->height());
    ui->tB_Transfer->setFixedSize(ui->tabWidget->width(),ui->tabWidget->height());
    ui->tB_Addition->setFixedSize(ui->tabWidget->width(),ui->tabWidget->height());
    ui->tabWidget->setCurrentIndex(0);

    QFile open1(":/data/Document/help_Toolbar.html");
    open1.open(QIODevice::ReadOnly);
    if(!open1.isOpen()){
        QMessageBox::critical(this,"错误","帮助文档文件无法打开，请检查");
    }
    ui->tB_Toolbar->setHtml(open1.readAll());

    QFile open2(":/data/Document/help_ViewMap.html");
    open2.open(QIODevice::ReadOnly);
    if(!open2.isOpen()){
        QMessageBox::critical(this,"错误","帮助文档文件无法打开，请检查");
    }
    ui->tB_View->setHtml(open2.readAll());

    QFile open3(":/data/Document/help_Transfer.html");
    open3.open(QIODevice::ReadOnly);
    if(!open3.isOpen()){
        QMessageBox::critical(this,"错误","帮助文档文件无法打开，请检查");
    }
    ui->tB_Transfer->setHtml(open3.readAll());

    QFile open4(":/data/Document/help_Addition.html");
    open4.open(QIODevice::ReadOnly);
    if(!open4.isOpen()){
        QMessageBox::critical(this,"错误","帮助文档文件无法打开，请检查");
    }
    ui->tB_Addition->setHtml(open4.readAll());


}

Help::~Help()
{
    delete ui;
}

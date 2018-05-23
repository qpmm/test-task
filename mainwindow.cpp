#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLibrary>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
}

MainWindow::~MainWindow()
{
    if (reader_cleanup)
    {
        reader_cleanup();
    }

    delete ui;
}

void MainWindow::init()
{
    QLibrary lib("../build-test_task_lib-Desktop_Qt_5_10_1_GCC_64bit-Debug/libtest_task_lib.so");

    if (!lib.load())
    {
        reader_cleanup = nullptr;
        init_error("Ошибка загрузки библиотеки!");
        return;
    }

    func_send_data callback = (func_send_data)&MainWindow::callback_receive;




    func_create reader_create = (func_create)lib.resolve("global_init");
    bool isOk = reader_create("../pulley.txt", this, callback);

    if (!isOk)
    {
        init_error("Ошибка открытия файла!");
        return;
    }

    reader_start = (func_control)lib.resolve("global_start");
    reader_stop = (func_control)lib.resolve("global_stop");
    reader_cleanup = (func_control)lib.resolve("global_cleanup");
    ui->label_info->setText("Программа готова к работе");
}

void MainWindow::on_btn_start_clicked()
{
    reader_start();
    ui->btn_start->setEnabled(false);
    ui->btn_stop->setEnabled(true);
}

void MainWindow::on_btn_stop_clicked()
{
    reader_stop();
    ui->btn_start->setEnabled(true);
    ui->btn_stop->setEnabled(false);
}

void MainWindow::callback_receive(point p)
{
    qDebug() << p.x << " " << p.y;
}

void MainWindow::callback_finished()
{
    init_error("Программа завершила работу");
}

void MainWindow::init_error(const char *message)
{
    ui->btn_start->setEnabled(false);
    ui->btn_stop->setEnabled(false);
    ui->label_info->setText(message);
}

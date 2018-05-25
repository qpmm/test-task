#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLibrary>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    reader_start = nullptr;
    reader_stop = nullptr;
    reader_cleanup = nullptr;
    init();
}

MainWindow::~MainWindow()
{
    if (reader_cleanup)
        reader_cleanup();

    delete ui;
}

void MainWindow::init()
{
    QLibrary lib("../build-test-task-lib-Debug/libtest-task-lib.so");

    if (!lib.load())
    {
        init_error("Ошибка загрузки библиотеки!");
        return;
    }

    fn_init reader_init = (fn_init)lib.resolve("reader_init");
    bool isOk = reader_init(this, "../pulley.txt");

    if (!isOk)
    {
        init_error("Ошибка открытия файла!");
        return;
    }

    fn_set_callbacks set_callbacks = (fn_set_callbacks)lib.resolve("reader_set_callbacks");
    fn_send_data sd = (fn_send_data)&MainWindow::callback_receive;
    fn_finished fn = (fn_finished)&MainWindow::callback_finished;
    set_callbacks(sd, fn);

    reader_start = (fn_control)lib.resolve("reader_start");
    reader_stop = (fn_control)lib.resolve("reader_stop");
    reader_cleanup = (fn_control)lib.resolve("reader_cleanup");

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
    reader_stop();
    init_error("Потоки завершили работу");
}

void MainWindow::init_error(const char *message)
{
    ui->btn_start->setEnabled(false);
    ui->btn_stop->setEnabled(false);
    ui->label_info->setText(message);
}

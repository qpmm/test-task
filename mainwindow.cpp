#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLibrary>
#include <QFile>
#include <QMetaType>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    reader_start = nullptr;
    reader_stop = nullptr;
    reader_cleanup = nullptr;

    qRegisterMetaType<point>("point");
    connect(this, &MainWindow::receive_to_draw, this, &MainWindow::draw_point);
    raw_curve = new QwtPlotCurve();
    flt_curve = new QwtPlotCurve();

    raw_curve->setPen(QPen(Qt::black));
    flt_curve->setPen(QPen(Qt::red));

    init();
}

MainWindow::~MainWindow()
{
    if (reader_cleanup)
        reader_cleanup();

    delete raw_curve;
    delete flt_curve;
    delete ui;
}

void MainWindow::init()
{
    // Загрузка библиотеки

    QLibrary lib("../build-test-task-lib-Debug/libtest-task-lib.so");

    if (!lib.load())
    {
        init_error("Ошибка загрузки библиотеки!");
        return;
    }

    // Инициализация файла

    fn_init reader_init = (fn_init)lib.resolve("reader_init");
    bool isOk = reader_init(this, "../pulley.txt");

    if (!isOk)
    {
        init_error("Ошибка открытия файла!");
        return;
    }

    // Установка callback'ов

    fn_set_callback set_callback = (fn_set_callback)lib.resolve("reader_set_callback");
    set_callback((fn_send_data)&MainWindow::callback_receive);

    reader_start = (fn_control)lib.resolve("reader_start");
    reader_stop = (fn_control)lib.resolve("reader_stop");
    reader_cleanup = (fn_control)lib.resolve("reader_cleanup");

    // Чтение конфига

    QFile config("config.txt");
    config.open(QIODevice::ReadWrite);
    window_size = QString(config.readAll()).toInt();
    config.close();

    // Настройка буферов для данных

    x.reserve(24000);
    y.reserve(24000);
    xmedian.reserve(24000);
    ymedian.reserve(24000);
    xwindow.reserve(window_size);
    ywindow.reserve(window_size);
    window.reserve(window_size);

    xwindow.push_back(0.0);
    ywindow.push_back(0.0);

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

void MainWindow::draw_point(point p)
{
    if (!x.size()) // если получена первая точка
    {
        xmedian.push_back(p.x);
        ymedian.push_back(p.y);
    }

    x.push_back(p.x);
    y.push_back(p.y);

    if (xwindow.size() < window_size) // ждем заполнения окна
    {
        xwindow.push_back(p.x);
        ywindow.push_back(p.y);
    }
    else // медианная фильтрация
    {
        xwindow.pop_front();
        ywindow.pop_front();
        xwindow.push_back(p.x);
        ywindow.push_back(p.y);

        int i = window_size / 2;

        window.clear();
        window.append(xwindow);
        std::sort(window.begin(), window.end());
        xmedian.push_back(window[i]);

        window.clear();
        window.append(ywindow);
        std::sort(window.begin(), window.end());
        ymedian.push_back(window[i]);
    }

    // Отрисовка каждой UPDATE_RATE точки

    if (xmedian.size() % UPDATE_RATE == 0)
    {
        flt_curve->setSamples(xmedian.data(), ymedian.data(), xmedian.size());
        flt_curve->attach(ui->graphic);
        ui->graphic->replot();
    }

    if (x.size() % UPDATE_RATE == 0)
    {
        raw_curve->setSamples(x.data(), y.data(), x.size());
        raw_curve->attach(ui->graphic);
        ui->graphic->replot();
    }
}

void MainWindow::callback_receive(point p)
{
    emit receive_to_draw(p);
}

void MainWindow::init_error(const char *message)
{
    ui->btn_start->setEnabled(false);
    ui->btn_stop->setEnabled(false);
    ui->label_info->setText(message);
}

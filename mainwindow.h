#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QTextStream>
#include "test_task_lib_global.h"
#include <qwt/qwt_plot.h>
#include <qwt/qwt_plot_curve.h>

#define UPDATE_RATE 20

namespace Ui {
class MainWindow;
}

class Logger
{
public:
    Logger();
    ~Logger();

    void init(QString filename);
    void log(QString text);

private:
    QFile output;
    QTextStream writer;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

    void callback_receive(point p); // callback получения точки

private slots:
    void on_btn_start_clicked(); // старт процесса
    void on_btn_stop_clicked(); // стоп процесса
    void draw_point(point p); // отрисовка точки на графике

signals:
    void receive_to_draw(point p); // передать координату функции draw_point

private:
    void init();
    void init_error(const char* message);

    Ui::MainWindow* ui;
    fn_control reader_start;
    fn_control reader_stop;
    fn_control reader_cleanup;

    QwtPlotCurve* raw_curve; // обычный график
    QwtPlotCurve* flt_curve; // график с медианной фильтрацией
    QVector<double> x; // x-координаты обычного графика
    QVector<double> y; // y-координаты обычного графика
    QVector<double> xmedian; // x-координаты фильтрованного графика
    QVector<double> ymedian; // y-координаты фильтрованного графика
    QVector<double> xwindow; // окно x-координат
    QVector<double> ywindow; // окно y-координат
    QVector<double> window; // окно-буфер для проведения фильтрации
    int window_size; // размер окна

    Logger logger;
};

#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "test_task_lib_global.h"
#include <qwt/qwt_plot.h>
#include <qwt/qwt_plot_curve.h>

#define UPDATE_RATE 20

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

    void callback_receive(point p);
    void callback_finished();

private slots:
    void on_btn_start_clicked();
    void on_btn_stop_clicked();
    void draw_point(point p);

signals:
    void receive_to_draw(point p);

private:
    void init();
    void init_error(const char* message);

    Ui::MainWindow* ui;
    fn_control reader_start;
    fn_control reader_stop;
    fn_control reader_cleanup;

    reader* readerer;
    QwtPlotCurve* raw_curve;
    QwtPlotCurve* flt_curve;
    QVector<double> x;
    QVector<double> y;
    QVector<double> xmedian;
    QVector<double> ymedian;
    QVector<double> xwindow;
    QVector<double> ywindow;
    QVector<double> window;
    int window_size;
};

#endif // MAINWINDOW_H

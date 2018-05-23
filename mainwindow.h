#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "test_task_lib_global.h"

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

private:
    void init();
    void init_error(const char* message);

    Ui::MainWindow* ui;
    func_control reader_start;
    func_control reader_stop;
    func_control reader_cleanup;
};

#endif // MAINWINDOW_H

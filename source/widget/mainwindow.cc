#include "mainwindow.h"

namespace xtaro {

    MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    {
        this->setMinimumSize(800, 600);
        this->setWindowTitle("Kiwi GUI");

    }

    MainWindow::~MainWindow() {
        
    }

}
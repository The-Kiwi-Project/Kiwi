#include "./gui.hh"
#include <widget/mainwindow.h>
#include <QApplication>

namespace kiwi {

    auto gui_main(int argc, char** argv) -> int {
        QApplication a(argc, argv);
        xtaro::MainWindow w;
        w.show();
        return a.exec();
    }

}
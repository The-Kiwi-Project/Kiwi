#include "./gui.hh"
#include <widget/window.h>
#include <QApplication>

namespace kiwi {

    auto gui_main(int argc, char** argv) -> int {
        QApplication a(argc, argv);
        // kiwi::MainWindow w;
        auto w = widget::Window{};
        w.show();
        return a.exec();
    }

}
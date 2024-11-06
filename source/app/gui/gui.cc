#include "./gui.hh"
#include <widget/widget.h>
#include <QApplication>

namespace kiwi {

    auto gui_main(int argc, char** argv) -> int {
        QApplication a(argc, argv);
        // kiwi::MainWindow w;
        auto w = widget::Widget{};
        w.show();
        return a.exec();
    }

}
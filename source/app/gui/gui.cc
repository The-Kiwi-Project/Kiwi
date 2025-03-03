#include "./gui.hh"
#include <widget/window.h>
#include <QApplication>

namespace kiwi {

    auto gui_main(int argc, char** argv) -> int {
        auto app = QApplication{argc, argv};
        app.setStyle("Fusion");
        auto w = widget::Window{};
        w.show();
        return app.exec();
    }

}
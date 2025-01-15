#include "./gui.hh"
#include <widget/layout/layoutwidget.h>
#include <widget/schematic/schematicwidget.h>

#include <widget/window.h>

#include <hardware/interposer.hh>
#include <circuit/basedie.hh>

#include <algo/router/route.hh>
#include <algo/router/maze/mazeroutestrategy.hh>

#include <parse/reader/module.hh>
#include <parse/writer/module.hh>

#include <std/utility.hh>
#include <std/range.hh>
#include <std/string.hh>
#include <debug/debug.hh>
#include <std/algorithm.hh>

#include <QApplication>
#include <QDebug>

namespace kiwi {

    auto gui_main(int argc, char** argv) -> int {
        auto app = QApplication{argc, argv};
        app.setStyle("Fusion");
        auto w = widget::Window{};
        w.show();
        return app.exec();
    }

}
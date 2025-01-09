#include "./gui.hh"
#include "widget/schematic/schematicwidget.h"

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

namespace kiwi {

    auto gui_main(int argc, char** argv) -> int {
        auto [interposer, basedie] 
            = kiwi::parse::read_config("../test/config/case3");
    
        // algo::route_nets(interposer.get(), basedie.get(), algo::MazeRouteStrategy{});
        // interposer->randomly_map_remain_indexes();

        auto app = QApplication{argc, argv};
        app.setStyle("Fusion");
        auto w = widget::SchematicWidget{interposer.get(), basedie.get()};
        w.show();
        return app.exec();
    }

}
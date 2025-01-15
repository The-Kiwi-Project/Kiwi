#include <hardware/interposer.hh>
#include <circuit/basedie.hh>

#include <parse/reader/module.hh>

#include <algo/netbuilder/netbuilder.hh>
#include <algo/router/route.hh>
#include <algo/router/maze/mazeroutestrategy.hh>

#include <widget/view3d/view3dwidget.h>

#include <std/utility.hh>
#include <std/range.hh>
#include <std/string.hh>
#include <std/algorithm.hh>
#include <debug/debug.hh>

#include <QApplication>

auto main(int argc, char** argv) -> int {
    if (argc < 2) {
        kiwi::debug::error_fmt("No <config_path> given");
        kiwi::debug::info_fmt("Usage: view3d <config_path>");
        return 0;
    }

    kiwi::debug::set_debug_level(kiwi::debug::DebugLevel::Debug);

    auto config_path = std::StringView{argv[1]};
    auto [interposer, basedie] = kiwi::parse::read_config(config_path);

    kiwi::algo::NetBuilder{basedie.get(), interposer.get()}.build();
    auto len = kiwi::algo::route_nets(interposer.get(), basedie.get(), kiwi::algo::MazeRouteStrategy{});
    kiwi::debug::info_fmt("Length: '{}'", len);

    interposer->randomly_map_remain_indexes();

    auto app = QApplication{argc, argv};
    app.setStyle("Fusion");
    auto w = kiwi::widget::View3DWidget{interposer.get(), basedie.get()};
    w.show();
    w.displayRoutingResult();
    return app.exec();
}
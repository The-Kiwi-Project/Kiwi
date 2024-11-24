#include "./gui.hh"
#include <widget/window.h>
#include <QApplication>
#include "circuit/net/types/btnet.hh"
#include "circuit/net/types/btsnet.hh"
#include "circuit/net/types/tbnet.hh"
#include <std/collection.hh>
#include <std/memory.hh>
#include "debug/debug.hh"
#include "hardware/node/track.hh"
#include "hardware/tob/tobsigdir.hh"
#include <algo/router/maze/mazeroutestrategy.hh>
#include <hardware/interposer.hh>
#include <circuit/net/nets.hh>

namespace kiwi {

    auto gui_main(int argc, char** argv) -> int {
        QApplication a(argc, argv);
        // kiwi::MainWindow w;

        debug::debug("test_route_bump_to_bump_net");

        auto i = hardware::Interposer{};
        auto router = algo::MazeRouteStrategy{};

        auto begin_bump = i.get_bump(0, 0, 45).value();
        auto end_bump = i.get_bump(0, 1, 4).value();
        auto net1 = circuit::BumpToBumpNet{begin_bump, end_bump};

        router.route_bump_to_bump_net(&i, &net1);

        auto w = widget::Window{&i};
        w.show();
        return a.exec();
    }

}
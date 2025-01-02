#include "./gui.hh"

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
#include <widget/frame/graphicsview.h>

namespace kiwi {

    auto gui_main(int argc, char** argv) -> int {
        // auto [interposer, basedie] 
        //     = kiwi::parse::read_config("../config/muyan_no_bus");
    
        // // algo::route_nets(interposer.get(), basedie.get(), algo::MazeRouteStrategy{});
        // // interposer->randomly_map_remain_indexes();

        // QApplication app(argc, argv);
        // auto w = widget::SWindow{interposer.get(), basedie.get()};
        // w.show();
        // return app.exec();

        QApplication app(argc, argv);

        // 创建场景和视图
        QGraphicsScene scene;
        widget::GraphicsView view;
        view.setScene(&scene);

        // 添加示例项
        for (int i = 0; i < 5; ++i) {
            auto* rect = new QGraphicsRectItem(i * 50, i * 50, 40, 40);
            rect->setBrush(Qt::blue);
            rect->setFlag(QGraphicsItem::ItemIsMovable);  // 允许拖动
            scene.addItem(rect);
        }

        // 显示视图
        view.resize(800, 600);
        view.show();

        return app.exec();
    }

}
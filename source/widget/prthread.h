#pragma once

#include <QThread>
#include <debug/debug.hh>
#include <algo/router/route.hh>
#include <algo/netbuilder/netbuilder.hh>
#include <algo/router/maze/mazeroutestrategy.hh>

namespace kiwi::widget {

    class PRThread : public QThread {
        Q_OBJECT

    public:
        PRThread(hardware::Interposer* i, circuit::BaseDie* b) : 
            QThread{},
            _interposer{i},
            _basedie{b} 
        {
        }

    protected:
        void run() override {
            debug::info_fmt("Begin to execute P&R");
            algo::build_nets(this->_basedie, this->_interposer);
            auto l = algo::route_nets(this->_interposer, this->_basedie, algo::MazeRouteStrategy{});
            debug::info_fmt("P&R finished with total path length '{}'", l); 
            emit this->prFinished();
        }

    signals:
        void prFinished();

    protected:
        hardware::Interposer* _interposer;
        circuit::BaseDie* _basedie;
    };

}
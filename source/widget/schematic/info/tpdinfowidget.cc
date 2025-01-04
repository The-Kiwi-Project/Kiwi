#include "./tpdinfowidget.h"
#include "qwidget.h"

#include <debug/debug.hh>

namespace kiwi::widget::schematic {

    TopdieInstInfoWidget::TopdieInstInfoWidget(QWidget* parent) :
        QWidget{parent}
    {
    }

    void TopdieInstInfoWidget::loadInst(TopDieInstanceItem* inst) {
        this->_inst = inst;
        if (this->_inst == nullptr) {
            debug::exception("Load a empty net into TopdieInstInfoWidget");
        }
    }

}
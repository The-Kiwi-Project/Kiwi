#include "./tobitem.h"
#include <QDebug>
#include <hardware/bump/bump.hh>

namespace kiwi::widget::layout {

    const QColor TOBItem::COLOR = Qt::gray;
    const QColor TOBItem::HIGHLIGHT_COLOR = Qt::red;
    const QColor TOBItem::WITH_TOPDIE_INST_COLOR = Qt::black;

    TOBItem::TOBItem(hardware::TOB* tob) :
        _tob{tob}
    {
        this->setRect(-WIDTH / 2., -HEIGHT / 2., WIDTH, HEIGHT);
        this->setBrush(COLOR);
        this->setZValue(Z_VALUE);
    }

    void TOBItem::setTopDieInst(TopDieInstanceItem* topdieinst) {
        this->_topdieInstItem = topdieinst;
        this->updateAppearance();
    }

    void TOBItem::removeTopDieInstance() {
        this->_topdieInstItem = nullptr;
        this->updateAppearance();
    }

    bool TOBItem::hasTopDieInst() const {
        return this->_topdieInstItem != nullptr;
    }

    void TOBItem::highlight(bool active) {
        this->setBrush(active ? HIGHLIGHT_COLOR : COLOR);
    }

    void TOBItem::updateAppearance() {
        this->setBrush(this->_topdieInstItem != nullptr ? WITH_TOPDIE_INST_COLOR : COLOR);
    }

}

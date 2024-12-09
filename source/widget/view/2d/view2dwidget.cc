#include "./view2dwidget.h"
#include "qobject.h"
#include "qpoint.h"
#include "./item/cobitem.hh"

#include "widget/view/2d/item/item.hh"
#include "widget/view/2d/item/tobitem.hh"
#include "widget/view/2d/item/topdieitem.hh"
#include <QPainter>
#include <QBrush>
#include <hardware/interposer.hh>
#include <circuit/basedie.hh>
#include <QDebug>
#include <cmath>
#include <memory>

namespace kiwi::widget {

    View2DWidget::View2DWidget(
        hardware::Interposer* interposer, 
        circuit::BaseDie* basedie,
        QWidget *parent
    ) :
        QWidget{parent},
        _interposer{interposer},
        _basedie{basedie}
    {
        auto cobArrayHeight = static_cast<float>(hardware::Interposer::COB_ARRAY_HEIGHT);
        auto cobArrayWidth  = static_cast<float>(hardware::Interposer::COB_ARRAY_WIDTH);

        this->setMouseTracking(true);
        this->setFocusPolicy(Qt::StrongFocus);
        this->setFocus();

        this->addCOBItems();
        this->addTOBItems();
        this->addTopDieInstItems();
    }

    View2DWidget::~View2DWidget() noexcept {}

    auto View2DWidget::paintEvent(QPaintEvent* event) -> void {
        auto painter = QPainter {this};
        painter.translate(this->_offsetX, this->_offsetY);
        painter.scale(this->_scale, this->_scale);
        for (const auto& item : this->_items) {
            item->paintSelf(painter);
        }
    }

    void View2DWidget::addCOBItems() {
        for (const auto& [coord, _] : this->_interposer->cobs()) {
            auto offset = this->cobPosition(coord);
            this->_items.emplace_back(std::make_unique<COBItem>(offset));
        }
    }

    auto View2DWidget::cobPosition(const hardware::COBCoord& coord) -> QPoint {
        return QPoint{
            static_cast<int>(coord.col) * COB_PITCH, 
            -static_cast<int>(coord.row) * COB_PITCH
        };
    }

    void View2DWidget::addTOBItems() {
        for (const auto& [coord, _] : this->_interposer->tobs()) {
            auto offset = this->tobPosition(coord);
            this->_items.emplace_back(std::make_unique<TOBItem>(offset));
        }
    }

    auto View2DWidget::tobPosition(const hardware::TOBCoord& coord) -> QPoint {
        auto pos = this->cobPosition(hardware::Interposer::TOB_COORD_MAP.at(coord));
        pos.setY(pos.y() - (COB_PITCH / 2));
        return pos;
    }

    void View2DWidget::addTopDieInstItems() {
        for (const auto& [name, topdie] : this->_basedie->topdie_insts()) {
            auto offset = this->topDieInstPosition(topdie.tob()->coord());
            this->_items.emplace_back(std::make_unique<TopDieInstItem>(offset));
        }
    }

    auto View2DWidget::topDieInstPosition(const hardware::TOBCoord& coord) -> QPoint{
        return this->tobPosition(coord);
    }

    void View2DWidget::wheelEvent(QWheelEvent *event) {
        QPointF mousePos = event->position();

        // 将鼠标位置映射到逻辑坐标系（缩放前的位置）
        QPointF beforeScale = (mousePos - QPointF(this->_offsetX, this->_offsetY)) / this->_scale;

        // 根据滚轮事件调整缩放比例
        qreal delta = event->angleDelta().y() / 240.0;
        qreal newScale = _scale * std::pow(1.1, delta);
        newScale = std::clamp(newScale, 0.1, 10.0);

        // 计算鼠标在逻辑坐标系中缩放后的位置
        QPointF afterScale = (mousePos - QPointF(this->_offsetX, this->_offsetY)) / newScale;

        // 调整偏移量，使得鼠标位置保持不变
        this->_offsetX += (afterScale - beforeScale).x() * newScale;
        this->_offsetY += (afterScale - beforeScale).y() * newScale;

        // 更新缩放比例
        this->_scale = newScale;

        this->update();
    }

    void View2DWidget::mousePressEvent(QMouseEvent *event) {
        this->_mouseButton = event->button();
        this->_lastMousePos = event->pos();
    }

    void View2DWidget::mouseReleaseEvent(QMouseEvent *event) {
        Q_UNUSED(event);
        this->_mouseButton = Qt::NoButton;
    }

    void View2DWidget::mouseMoveEvent(QMouseEvent *event) {
        if (this->_mouseButton == Qt::LeftButton) {
            QPoint delta = event->pos() - this->_lastMousePos;
            this->_offsetX += delta.x();
            this->_offsetY += delta.y();
            this->update();
        }

        this->_lastMousePos = event->pos();
    }

}
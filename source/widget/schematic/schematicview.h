#pragma once

#include <widget/frame/graphicsview.h>
#include <QWidget>
#include <QGraphicsView>

namespace kiwi::hardware {
    class Interposer;
    class TOB;
    class COB;
    class Track;
};

namespace kiwi::circuit {
    class BaseDie;
    class TopDieInstance;
};

namespace kiwi::widget {
   
    class SchematicView : public GraphicsView {
    public:
        explicit SchematicView(
            hardware::Interposer* interposer, 
            circuit::BaseDie* basedie,
            QWidget *parent = nullptr);

        ~SchematicView() noexcept;

    protected:
        void drawBackground(QPainter* painter, const QRectF& rect) override;

    public:
        void updateBack();

    public:
        auto backColor() const -> const QColor& { return this->backgroundBrush().color(); }
        auto gridVisible() const -> bool { return this->_gridVisible; }
        auto gridColor() const -> const QColor& { return this->_gridColor; }
        auto gridSize() const -> qreal { return this->_gridSize; }

        void setBackColor(const QColor& color) { this->setBackgroundBrush(color); }
        void setGridVisible(bool visible) { this->_gridVisible = visible; }
        void setGridColor(const QColor& color) { this->_gridColor = color; }
        void setGridSize(qreal size) { this->_gridSize = size; }

    protected:
        hardware::Interposer* _interposer {nullptr};
        circuit::BaseDie* _basedie {nullptr};

        bool _gridVisible {true};
        QColor _gridColor {Qt::lightGray};
        qreal _gridSize {20};
    };

}
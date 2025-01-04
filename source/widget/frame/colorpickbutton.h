#include <QApplication>
#include <QPushButton>
#include <QColorDialog>
#include <QVBoxLayout>
#include <QWidget>

namespace kiwi::widget {

    class ColorPickerButton : public QPushButton {
        Q_OBJECT

    public:
        explicit ColorPickerButton(QWidget *parent = nullptr)
            : QPushButton("Select Color", parent), currentColor(Qt::white) {
            this->updateButtonStyle();
            connect(this, &QPushButton::clicked, this, &ColorPickerButton::onSelectColor);
        }

        QColor getColor() const { return currentColor; }

        void setColor(const QColor &color) {
            if (color.isValid()) {
                currentColor = color;
                this->updateButtonStyle();
            }
        }

    private slots:
        void onSelectColor() {
            QColor color = QColorDialog::getColor(currentColor, this, "Select Color");
            if (color.isValid()) {
                this->setColor(color);
                emit colorChanged(color);
            }
        }

    private:
        void updateButtonStyle() {
            this->setStyleSheet(QString(
                "background-color: %1;"
                    "border-radius: 5px;"
                    "border: 1px solid #A9A9A9;"
                ).arg(currentColor.name()));
            setText(currentColor.name());
        }

    signals:
        void colorChanged(const QColor &color); // 颜色改变信号

    private:
        QColor currentColor;
    };
    
}



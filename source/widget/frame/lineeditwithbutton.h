#pragma once

#include "qsizepolicy.h"
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>

namespace kiwi::widget {

    class LineEditWithButton : public QWidget {
        Q_OBJECT

    public:
        explicit LineEditWithButton(QWidget *parent = nullptr) : QWidget(parent) {
            // 创建子控件
            lineEdit = new QLineEdit(this);
            lineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
            button = new QPushButton("Yes", this);
            button->setMaximumWidth(40);
            button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

            // 设置布局
            QHBoxLayout *layout = new QHBoxLayout(this);
            layout->setContentsMargins(0, 0, 0, 0); // 去除布局的边距
            layout->addWidget(lineEdit);
            layout->addWidget(button);
            setLayout(layout);

            // 连接信号槽
            connect(button, &QPushButton::clicked, this, &LineEditWithButton::onButtonClicked);
        }

        // 提供获取和设置 QLineEditWithButton 内容的接口
        QString text() const { return lineEdit->text(); }
        void setText(const QString &text) { lineEdit->setText(text); }

    signals:
        void textConfirmed(const QString &text); // 确认输入文本的信号

    private slots:
        void onButtonClicked() {
            emit textConfirmed(lineEdit->text()); // 发出确认信号
        }

    private:
        QLineEdit *lineEdit;
        QPushButton *button;
    };

}
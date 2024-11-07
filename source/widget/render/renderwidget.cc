#include "./renderwidget.h"

#include <debug/debug.hh>

#include <QVector>
#include <QMouseEvent>
#include <QtMath>
#include <QWheelEvent>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QFileInfo>
#include <QtDebug>

namespace kiwi::widget {

    float radian2Angle(float radian) {
        return radian / 3.14159f * 180.0f;
    }

    float angle2Radian(float angle) {
        return angle * 3.14159f / 180.0f;
    }

    //! \brief verices date
    float RenderWidget::axisVertices[] = {
        // y axis
        -1 * AXIS_LENGTH / 2.0, 0.0f, 0.0f, AXIS_LENGTH, 0.0f, 0.0f,
        AXIS_LENGTH - ARROWHEAD_SIZE, ARROWHEAD_SIZE, 0.0f, AXIS_LENGTH, 0.0f, 0.0f,
        AXIS_LENGTH - ARROWHEAD_SIZE, -ARROWHEAD_SIZE, 0.0f, AXIS_LENGTH, 0.0f, 0.0f,
        // x axis
        0.0f, 0.0f, -1 * AXIS_LENGTH / 2.0, 0.0f, 0.0f, AXIS_LENGTH,
        ARROWHEAD_SIZE, 0.0f, AXIS_LENGTH - ARROWHEAD_SIZE, 0.0f, 0.0f, AXIS_LENGTH,
        -ARROWHEAD_SIZE, 0.0f, AXIS_LENGTH - ARROWHEAD_SIZE, 0.0f, 0.0f, AXIS_LENGTH,
        // z axis
        0.0f, -1 * AXIS_LENGTH / 2.0, 0.0f, 0.0f, AXIS_LENGTH, 0.0f,
        ARROWHEAD_SIZE, AXIS_LENGTH - ARROWHEAD_SIZE, 0.0f, 0.0f, AXIS_LENGTH, 0.0f,
        -ARROWHEAD_SIZE, AXIS_LENGTH - ARROWHEAD_SIZE, 0.0f, 0.0f, AXIS_LENGTH, 0.0f,
    };

    //! \brief construct function
    RenderWidget::RenderWidget(QWidget *parent) :
        QOpenGLWidget(parent),
        _axisVao{},
        _axisVbo{QOpenGLBuffer::VertexBuffer},
        _axisShader{},
        _currentPointPosition{0.0f, 0.0f, 0.0f},
        _posTheta{RenderWidget::DEFAULT_THETA_VALUE},
        _posPitch{RenderWidget::DEFAULT_PITCH_VALUE},
        _posRadius{RenderWidget::DEFAULT_RADIUS_VALUE},
        _lastMousePos{100, 100},
        _coordbias{RenderWidget::DEFAULT_MODEL_BIAS},
        _mouseButton{Qt::NoButton}
    {
        this->setMouseTracking(true);
        this->setFocusPolicy(Qt::StrongFocus);
        this->setFocus();
    }

    //! \brief destruct function
    RenderWidget::~RenderWidget() noexcept {
        this->makeCurrent();

        this->_axisVao.destroy();
        this->_axisVbo.destroy();

        this->doneCurrent();
    }

    //! \brief iniatil axis object
    void RenderWidget::initAxis(const QMatrix4x4& view, const QMatrix4x4& projection) {
        this->_axisVao.create();
        this->_axisVbo.create();

        this->_axisShader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/shaders/axis.vert");
        this->_axisShader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/shaders/axis.frag");
        if (this->_axisShader.link() == false) {
            // TODO error
            debug::error("Linke the axis shader failed");
            this->close();
        }

        this->_axisVao.bind();
        this->_axisVbo.bind();

        this->_axisVbo.allocate(axisVertices, sizeof(axisVertices));

        this->_axisShader.bind();
        this->_axisShader.setAttributeBuffer(0, GL_FLOAT, 0, 3, 3 * sizeof(float));
        this->_axisShader.enableAttributeArray(0);

        QMatrix4x4 bias;
        bias.translate({0.0f, Z_BIAS, 0.0f});
        this->_axisShader.setUniformValue("bias", bias);
        this->_axisShader.setUniformValue("view", view);
        this->_axisShader.setUniformValue("projection", projection);

        this->_axisShader.release();
        this->_axisVbo.release();
        this->_axisVao.release();
    }

    //! \brief reset uniform view matrix in shaders
    void RenderWidget::updateViewMatrix() {
        this->makeCurrent();

        QMatrix4x4 view = this->getViewMatrix();

        this->_axisShader.bind();
        this->_axisShader.setUniformValue("view", view);
        this->_axisShader.release();

        this->update();

        this->doneCurrent();
    }

    //! \brief update coord bias
    void RenderWidget::updateCoordBias() {
        this->makeCurrent();

        QMatrix4x4 bias;
        bias.translate(this->_coordbias);
        this->_axisShader.bind();
        this->_axisShader.setUniformValue("bias", bias);
        this->_axisShader.release();

        this->update();
        this->doneCurrent();
        this->parentWidget()->update();
    }

    //! \brief reset the view matrix
    void RenderWidget::resetView() {
        this->_posTheta = RenderWidget::DEFAULT_THETA_VALUE;
        this->_posPitch = RenderWidget::DEFAULT_PITCH_VALUE;
        this->_posRadius = RenderWidget::DEFAULT_RADIUS_VALUE;

        this->_coordbias = RenderWidget::DEFAULT_MODEL_BIAS;

        this->updateViewMatrix();
        this->updateCoordBias();
    }

    void RenderWidget::wheelEvent(QWheelEvent *event) {
        this->_posRadius += (event->angleDelta().y() / 100.0);
        this->updateViewMatrix();
        parentWidget()->update();
    }

    void RenderWidget::mousePressEvent(QMouseEvent *event) {
        this->_mouseButton = event->button();
        this->_lastMousePos = event->pos();
    }

    void RenderWidget::mouseReleaseEvent(QMouseEvent *event) {
        Q_UNUSED(event);
        this->_mouseButton = Qt::NoButton;
    }

    void RenderWidget::mouseMoveEvent(QMouseEvent *event) {
        QPoint delta = event->pos() - this->_lastMousePos;

        // rotate model
        if (this->_mouseButton == Qt::LeftButton) {
            this->_posTheta -= (delta.x() / 100.0);
            this->_posPitch += (delta.y() / 100.0);

            // pitch range
            if (this->_posPitch > RenderWidget::MAX_PITCH)
                this->_posPitch = RenderWidget::MAX_PITCH;
            else if (this->_posPitch < RenderWidget::MIN_PITCH)
                this->_posPitch = RenderWidget::MIN_PITCH;

            this->updateViewMatrix();
        }

        // translate model
        else if (this->_mouseButton == Qt::RightButton) {
            float deltaX = delta.x() * -1.0f * qSin(this->_posTheta)
                        + delta.y() * qCos(this->_posTheta);
            float deltaY = delta.x() * qCos(this->_posTheta)
                        + delta.y() * qSin(this->_posTheta);
            this->_coordbias += QVector3D(deltaY / 30.0,
                                        0.0f,
                                        deltaX / 30.0);
            // No move axis
            // this->updateCoordBias();
        }

        this->_lastMousePos = event->pos();

        parentWidget()->update();
    }

    void RenderWidget::dragEnterEvent(QDragEnterEvent *event) {
        if (event->mimeData()->hasUrls()) {
            QString filename = event->mimeData()->urls()[0].fileName();
            QFileInfo info(filename);
            QString ex = info.suffix().toUpper();
            if (ex == "GCODE" || ex == "LC")
                event->acceptProposedAction();
            else
                event->ignore();
        }
        else {
            event->ignore();
        }
    }

    void RenderWidget::initializeGL() {
        // initial OpenGL function pointer
        initializeOpenGLFunctions();

        // enbale depth test
        glEnable(GL_DEPTH_TEST);

        // calculate uniform matrix
        QMatrix4x4 view = this->getViewMatrix();
        QMatrix4x4 projection;
        projection.perspective(45.0f, 1.0f * this->width() / this->height(), 0.1f, 100.0f);

        // create axis opengl object
        this->initAxis(view, projection);
    }

    void RenderWidget::resizeGL(int w, int h) {
        glViewport(0, 0, w, h);
    }

    void RenderWidget::paintGL() {
        // clear screen and buffer
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // render axis
        this->_axisVao.bind();
        this->_axisShader.bind();

        // y axis
        this->_axisShader.setUniformValue("color", QVector3D(1.0f, 0.0f, 0.0f));
        glDrawArrays(GL_LINES, 0, 6);
        // x axis
        this->_axisShader.setUniformValue("color", QVector3D(0.0f, 1.0f, 0.0f));
        glDrawArrays(GL_LINES, 6, 6);
        // z axis
        this->_axisShader.setUniformValue("color", QVector3D(0.0f, 0.0f, 1.0f));
        glDrawArrays(GL_LINES, 12, 6);

        this->_axisVao.release();
        this->_axisShader.release();
    }

}

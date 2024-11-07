#pragma once

#include "qscopedpointer.h"
#include <QVector>
#include <QMouseEvent>
#include <QtMath>
#include <QWheelEvent>
#include <QDragEnterEvent>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QtOpenGL/QOpenGLFunctions_3_3_Core>
#include <QtOpenGL/QOpenGLShaderProgram>
#include <QtOpenGL/QOpenGLBuffer>
#include <QtOpenGL/QOpenGLTexture>
#include <QtOpenGL/QOpenGLVertexArrayObject>
#else
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#endif

namespace kiwi::widget {

    class RenderWidget : public QOpenGLWidget, QOpenGLFunctions_3_3_Core {
        Q_OBJECT

    //! \brief window's constant variable
    private:
        static constexpr float DEFAULT_THETA_VALUE = M_PI / 3.0f + M_PI;
        static constexpr float DEFAULT_PITCH_VALUE = M_PI / 7.0f;
        static constexpr float DEFAULT_RADIUS_VALUE = 30.0f;
        static constexpr float ARROWHEAD_SIZE = 0.3f;
        static constexpr float Z_BIAS = -2.5f;
        static constexpr float MAX_PITCH = M_PI / 2.0 - 0.0001;
        static constexpr float MIN_PITCH = -1 * M_PI / 2.0 + 0.0001;

        static constexpr float AXIS_LENGTH = 10.0f;

        static constexpr float COB_WIDTH = 1.0f;
        static constexpr float COB_HEIGHT = 0.2f;
        static constexpr float COB_INTERAL = 2.0f;

        static constexpr float TRACK_HEIGHT = COB_INTERAL;
        static constexpr float TRACK_WIDTH  = 0.8f;

    //! \group verices date
    private:
        static float axisVertices[];
        static float cobVertices[];
        static int cobIndices[];

    //! \group constructor and desstructor
    public:
        explicit RenderWidget(QWidget *parent = nullptr);
        ~RenderWidget() noexcept;

    //! \group iniatil axis object and model object
    private:
        void initAxis(const QMatrix4x4& view, const QMatrix4x4& projection);
        void initCOB(const QMatrix4x4& view, const QMatrix4x4& projection);

    //! \group calculate current status data
    private:
        QVector3D getViewPos() const;
        QMatrix4x4 getViewMatrix() const;

    //! \group update the current status
    public:
        void updateViewMatrix();
        void updateCoordBias();
        void updateInstantiateMatrices();

    //! \group set the widget's data
    public:
        void drawLineToPoint(const QVector3D& position, bool update = true);
        void jumpToPoint(const QVector3D& position)
        { this->_currentPointPosition = position; }
        void clearModel();
        void resetView();

    //! \group window event callback functions
    protected:
        virtual void wheelEvent(QWheelEvent *event) override;
        virtual void mousePressEvent(QMouseEvent *event) override;
        virtual void mouseReleaseEvent(QMouseEvent *event) override;
        virtual void mouseMoveEvent(QMouseEvent *event) override;
        virtual void dragEnterEvent(QDragEnterEvent *event) override;

    //! \group OpenGL callback functions
    protected:
        virtual void initializeGL() override;
        virtual void resizeGL(int w, int h) override;
        virtual void paintGL() override;

    //! \group data members
    private:
        //! \brief axis
        QOpenGLVertexArrayObject _axisVao {};
        QOpenGLBuffer _axisVbo {QOpenGLBuffer::VertexBuffer};
        QOpenGLShaderProgram _axisShader {};

        //! \brief cob
        QOpenGLVertexArrayObject _cobVao {};
        QOpenGLBuffer _cobVbo {QOpenGLBuffer::VertexBuffer};
        QOpenGLBuffer _cobIndicesVbo {QOpenGLBuffer::IndexBuffer};
        QOpenGLBuffer _cobPositionsVbo {};
        QVector<QVector3D> _cobPositions {QVector3D{0, 0, 0}};
        QScopedPointer<QOpenGLTexture> _cobTexture {nullptr};
        
        QOpenGLShaderProgram _modelShader {};

        //! \brief current point position
        QVector3D _currentPointPosition {0.0f, 0.0f, 0.0f};

        //! \brief camera message
        float _posTheta  {RenderWidget::DEFAULT_THETA_VALUE};
        float _posPitch  {RenderWidget::DEFAULT_PITCH_VALUE};
        float _posRadius {RenderWidget::DEFAULT_RADIUS_VALUE};

        //! \brief last mouse pos
        QPoint _lastMousePos {100, 100};

        //! \brief coordinate position
        QVector3D _coordbias {};
        Qt::MouseButton _mouseButton {Qt::NoButton};
    };

}

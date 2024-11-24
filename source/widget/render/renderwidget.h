#pragma once

#include "qmatrix4x4.h"
#include "qpoint.h"
#include "qvector.h"
#include "qvector3d.h"
#include "std/utility.hh"
#include <QScopedPointer>
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


namespace kiwi::hardware {
    class Interposer;
};

namespace kiwi::widget {

    class RenderWidget : public QOpenGLWidget, QOpenGLFunctions_3_3_Core {
        Q_OBJECT

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
        static constexpr float COB_HEIGHT = 0.4f;
        static constexpr float COB_INTERAL = 3.0f;

        static constexpr float CHANNEL_LENGTH = COB_INTERAL;
        static constexpr float CHANNEL_WIDTH  = 0.8f;
        static constexpr float CHANNEL_HEIGHT  = 0.2f;

        static constexpr float TOB_WIDTH = 1.5f;
        static constexpr float TOB_HEIGHT = 0.6f;

    private:
        static float axisVertices[];
        static int cubeIndices[];

    public:
        explicit RenderWidget(hardware::Interposer* interposer, QWidget *parent = nullptr);
        ~RenderWidget() noexcept;

    private:
        void initAxis(const QMatrix4x4& view, const QMatrix4x4& projection);
        void initCube(const QMatrix4x4& view, const QMatrix4x4& projection);
        void initFrame(const QMatrix4x4& view, const QMatrix4x4& projection);

        struct Cube {
            float length; float width; float height;
            QVector<float> vertices {};
            QOpenGLBuffer verticesVBO {QOpenGLBuffer::VertexBuffer};
            QVector<QVector3D> positions {};
            QOpenGLBuffer positionsVBO {QOpenGLBuffer::VertexBuffer};
            QScopedPointer<QOpenGLTexture> texture {nullptr};
            int textureid;
        };

        auto makeCube(
            float length, float width, float height,
            QVector<QVector3D> positions,
            const QString& texturePath, unsigned int textureid
        ) -> Cube*;
        
        void renderCube(Cube* cube);
        void renderCubes();
        void renderAxis();
        void renderFrame();

        void reRender();

    private:
        QVector3D getViewPos() const;
        QVector3D getScreenViewPos(const QPoint& pos) const;

        QMatrix4x4 getViewMatrix() const;
        QMatrix4x4 getProjection() const;

    public:
        void updateViewMatrix();
        void updateCoordBias();
        void updateInstantiateMatrices();
        void updateProjection();

        void resetView();


    protected:
        virtual void wheelEvent(QWheelEvent *event) override;
        virtual void mousePressEvent(QMouseEvent *event) override;
        virtual void mouseReleaseEvent(QMouseEvent *event) override;
        virtual void mouseMoveEvent(QMouseEvent *event) override;
        virtual void dragEnterEvent(QDragEnterEvent *event) override;
        virtual void resizeEvent(QResizeEvent *event) override;

    protected:
        virtual void initializeGL() override;
        virtual void resizeGL(int w, int h) override;
        virtual void paintGL() override;

    protected:
        auto screenPosToWorldRay(const QPoint& mousePos) -> QVector3D;
        using OneCube = std::Tuple<Cube*, int>;
        auto resetPointedCube(const QPoint& pos) -> void;

    private:
        //! \brief axis
        QOpenGLVertexArrayObject _axisVao {};
        QOpenGLBuffer _axisVbo {QOpenGLBuffer::VertexBuffer};
        QOpenGLShaderProgram _axisShader {};

        //! \brief cubes
        QVector<Cube*> _cubes {};
        QOpenGLVertexArrayObject _cubeVAO;
        QOpenGLBuffer _cubeIndicesVBO {QOpenGLBuffer::IndexBuffer};
        QOpenGLShaderProgram _cubeShader {};

        std::Option<OneCube> _pointedCube {};

        //! \brief pointed
        QOpenGLVertexArrayObject _frameVAO {};
        QOpenGLBuffer _frameVBO {QOpenGLBuffer::VertexBuffer};
        QOpenGLShaderProgram _frameShader {};
        QVector<QVector3D> _frameVertices {24};

        //! \brief camera message
        float _posTheta  {RenderWidget::DEFAULT_THETA_VALUE};
        float _posPitch  {RenderWidget::DEFAULT_PITCH_VALUE};
        float _posRadius {RenderWidget::DEFAULT_RADIUS_VALUE};

        //! \brief last mouse pos
        QPoint _lastMousePos {100, 100};

        //! \brief coordinate position
        QVector3D _coordbias {};
        Qt::MouseButton _mouseButton {Qt::NoButton};

        // Hardware
        hardware::Interposer* _interposer {nullptr};
    };

}

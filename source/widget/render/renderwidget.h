#pragma once

#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QElapsedTimer>
#include <QVector>
#include <QMouseEvent>
#include <QtMath>
#include <QWheelEvent>
#include <QDragEnterEvent>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QtOpenGLWidgets/QOpenGLWidget>
#else
#include <QOpenGLWidget>
#endif

namespace kiwi::widget {

    class RenderWidget : public QOpenGLWidget, QOpenGLFunctions_3_3_Core {
        Q_OBJECT

    //! \brief window's constant variable
    private:
        static constexpr float DEFAULT_THETA_VALUE = M_PI / 3.0f;
        static constexpr float DEFAULT_PITCH_VALUE = M_PI / 7.0f;
        static constexpr float DEFAULT_RADIUS_VALUE = 22.0f;
        static constexpr QVector3D DEFAULT_MODEL_BIAS = QVector3D{0.0f, 0.0f, 0.0f};
        static constexpr float ARROWHEAD_SIZE = 0.3f;
        static constexpr float AXIS_LENGTH = 10.0f;
        static constexpr float Z_BIAS = -2.5f;
        static constexpr float MAX_PITCH = M_PI / 2.0 - 0.0001;
        static constexpr float MIN_PITCH = -1 * M_PI / 2.0 + 0.0001;

    //! \group verices date
    private:
        static float axisVertices[];

    //! \group constructor and desstructor
    public:
        explicit RenderWidget(QWidget *parent = nullptr);
        ~RenderWidget() noexcept;

    //! \group iniatil axis object and model object
    private:
        void initAxis(const QMatrix4x4& view, const QMatrix4x4& projection);

    //! \group calculate current status data
    private:
        QVector3D getViewPos() const
        {
            float horizontalRadius = this->_posRadius * qCos(this->_posPitch);
            return QVector3D(horizontalRadius * qSin(this->_posTheta),
                            this->_posRadius * qSin(this->_posPitch),
                            horizontalRadius * qCos(this->_posTheta));
        }

        QMatrix4x4 getViewMatrix() const
        {
            QMatrix4x4 view;
            view.lookAt(this->getViewPos(),
                        QVector3D(0.0f, 0.0f, 0.0f),
                        QVector3D(0.0f, 1.0f, 0.0f));
            return view;
        }

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
        QOpenGLVertexArrayObject _axisVao;
        QOpenGLBuffer _axisVbo;
        QOpenGLShaderProgram _axisShader;

        //! \brief current point position
        QVector3D _currentPointPosition;

        //! \brief camera message
        float _posTheta;
        float _posPitch;
        float _posRadius;

        //! \brief last mouse pos
        QPoint _lastMousePos;

        //! \brief coordinate position
        QVector3D _coordbias;
        Qt::MouseButton _mouseButton;

        //! \brief _timer
        QElapsedTimer _timer;
    };

}

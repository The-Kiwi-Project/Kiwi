#pragma once

#include "hardware/cob/cob.hh"
#include "hardware/track/trackcoord.hh"
#include "hardware/tob/tobcoord.hh"
#include "std/collection.hh"
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
    class TOB;
    class COB;
    class Track;
};

namespace kiwi::circuit {
    class BaseDie;
    class TopDieInstance;
};

namespace kiwi::widget {

    enum class CubeType {
        TOB,
        COB,
        Channel,
        Topdie,
    };

    struct Cube {
        CubeType type;
        float length; float width; float height;

        QVector<float> vertices {};
        QOpenGLBuffer verticesVBO {QOpenGLBuffer::VertexBuffer};
        
        QVector<QVector3D> positions {};
        QOpenGLBuffer positionsVBO {QOpenGLBuffer::VertexBuffer};
        
        QScopedPointer<QOpenGLTexture> texture {nullptr};
        int textureid;
    };

    class View3DWidget : public QOpenGLWidget, QOpenGLFunctions_3_3_Core {
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

        static constexpr float COB_WIDTH = 2.0f;
        static constexpr float COB_HEIGHT = 0.6f;

        static constexpr float TOB_WIDTH = 2.0f;
        static constexpr float TOB_HEIGHT = 1.0f;

        static constexpr float TOPDIE_WIDTH = 2.5f;
        static constexpr float TOPDIE_HEIGHT = 0.2f;

        static constexpr float COB_INTERAL = 3.0f;
        static constexpr float CHANNEL_LENGTH = 3.0f;
        static constexpr float CHANNEL_WIDTH  = 1.5f;
        static constexpr float CHANNEL_HEIGHT  = 0.2f;
        static constexpr float TRACK_INTERVAL = CHANNEL_WIDTH / (hardware::COB::INDEX_SIZE + 1);

        static_assert(COB_INTERAL > TOB_WIDTH);

    private:
        static float axisVertices[];
        static int cubeIndices[];
        static float trackVertices[];

    public:
        explicit View3DWidget(
            hardware::Interposer* interposer, 
            circuit::BaseDie* basedie,
            QWidget *parent = nullptr);
        ~View3DWidget() noexcept;

    public:
        auto displayRoutingResult() -> void;

    protected:
        void initAxis(const QMatrix4x4& view, const QMatrix4x4& projection, const QMatrix4x4& bias);
        void initCube(const QMatrix4x4& view, const QMatrix4x4& projection, const QMatrix4x4& bias);
        void initFrame(const QMatrix4x4& view, const QMatrix4x4& projection, const QMatrix4x4& bias);
        void initTracks(const QMatrix4x4& view, const QMatrix4x4& projection, const QMatrix4x4& bias);

        void initTOBCube();
        void initCOBCube();
        void initTopdieInstance();
        void initChannelCube();

    protected:
        auto channelPosition(std::i64 row, std::i64 col, hardware::TrackDirection dir) -> QVector3D;
        auto trackPosition(const hardware::TrackCoord& coord) -> std::Tuple<QVector3D, QVector3D>;
        auto cobPosition(const hardware::COBCoord& coord) -> QVector3D;
        auto tobPosition(const hardware::TOBCoord& coord) -> QVector3D;
        auto topdiePosition(const hardware::TOBCoord& coord) -> QVector3D;

    protected:
        auto makeCube(
            CubeType type,
            float length, float width, float height,
            QVector<QVector3D> positions,
            const QString& texturePath, unsigned int textureid
        ) -> Cube*;

        auto addCube(
            CubeType type, float length, float width, float height,
            const QString& texturePath, unsigned int textureid,
            QVector<QVector3D> positions
        ) -> Cube*;

        auto addTrack(const QVector3D &begin, const QVector3D &end, bool update) -> void;
        auto addTrack(const hardware::TrackCoord& coord, bool update) -> void;
        
    protected:
        void renderCubes();
        void renderAxis();
        void renderFrame();
        void renderTracks();

        void renderCube(Cube* cube);

        void reRender();

    private:
        auto getViewPos() const -> QVector3D;
        auto getScreenViewPos(const QPoint& pos) const -> QVector3D;

        auto getViewMatrix() const -> QMatrix4x4;
        auto getProjectionMatrix() const -> QMatrix4x4;
        auto getBiasMatrix() const -> QMatrix4x4;

    public:
        void updateViewMatrix();
        void updateViewMatrix(const QMatrix4x4& view);

        void updateBiasMatrix();
        void updateBiasMatrix(const QMatrix4x4& bias);

        void updateProjectionMatrix();
        void updateProjectionMatrix(const QMatrix4x4& projection);

        void updateTrackInstMatrices();

        void resetView();

    protected:
        auto getTOBByCubeIndeces(int index) -> hardware::TOB*;
        auto getCOBByCubeIndeces(int index) -> hardware::COB*;
        auto getTopdieInstByCubeIndeces(int index) -> circuit::TopDieInstance*;

    protected:
        virtual void wheelEvent(QWheelEvent *event) override;
        virtual void mousePressEvent(QMouseEvent *event) override;
        virtual void mouseReleaseEvent(QMouseEvent *event) override;
        virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
        virtual void mouseMoveEvent(QMouseEvent *event) override;
        virtual void dragEnterEvent(QDragEnterEvent *event) override;
        virtual void resizeEvent(QResizeEvent *event) override;

    protected:
        virtual void initializeGL() override;
        virtual void resizeGL(int w, int h) override;
        virtual void paintGL() override;

    protected:
        using OneCube = std::Tuple<Cube*, int>;
        auto resetPointedCube(const QVector3D& raySource, const QVector3D& rayDirection) -> void;

    private:
        //! \brief axis
        QOpenGLVertexArrayObject _axisVAO {};
        QOpenGLBuffer _axisVBO {QOpenGLBuffer::VertexBuffer};
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

        //! \brief track
        QOpenGLVertexArrayObject _trackVAO;
        QOpenGLBuffer _trackVBO {QOpenGLBuffer::VertexBuffer};
        QOpenGLBuffer _trackInstVBO {QOpenGLBuffer::VertexBuffer};
        QVector<QMatrix4x4> _trackInstMatrices;
        QOpenGLShaderProgram _trackShader;

        //! \brief camera message
        float _posTheta  {View3DWidget::DEFAULT_THETA_VALUE};
        float _posPitch  {View3DWidget::DEFAULT_PITCH_VALUE};
        float _posRadius {View3DWidget::DEFAULT_RADIUS_VALUE};

        //! \brief last mouse pos
        QPoint _lastMousePos {100, 100};

        //! \brief coordinate position
        QVector3D _coordbias {};
        Qt::MouseButton _mouseButton {Qt::NoButton};

    private:
        hardware::Interposer* _interposer {nullptr};
        circuit::BaseDie* _basedie {nullptr};

        std::Vector<hardware::TOB*> _tobs {};
        std::Vector<hardware::COB*> _cobs {};
        std::Vector<circuit::TopDieInstance*> _topdieinsts {};
    };

}

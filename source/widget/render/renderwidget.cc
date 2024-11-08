#include "./renderwidget.h"
#include <hardware/interposer.hh>
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

    // Coord (a, b, c)
    // a => y, b => z, c => x
    // Coord (y, z, x)

    static auto generateCubVertices(float length, float width, float height) -> QVector<float> {
        return QVector<float>{
            0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, length, 0.0f, 1.0f,
            width, 0.0f, 0.0f, 1.0f, 0.0f,
            width, 0.0f, length, 1.0f, 1.0f,

            0.0f, height, 0.0f, 0.0f, 0.0f,
            0.0f, height, length, 0.0f, 1.0f,
            width, height, 0.0f, 1.0f, 0.0, 
            width, height, length, 1.0f, 1.0f,        
        };
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

    int RenderWidget::cubeIndices[] = {
        // Bottom
        0, 1, 3,
        0, 2, 3,

        // Top
        4, 5, 7, 
        4, 6, 7,

        // Left
        0, 4, 5, 
        0, 1, 5,

        // Right
        2, 6, 6,
        2, 3, 6, 

        // Front
        0, 2, 6, 
        0, 4, 6, 

        // Back
        1, 3, 7, 
        1, 5, 7,
    };

    //! \brief construct function
    RenderWidget::RenderWidget(QWidget *parent) :
        QOpenGLWidget(parent)
    {
        auto cob_array_height = static_cast<float>(hardware::Interposer::COB_ARRAY_HEIGHT);
        auto cob_array_width  = static_cast<float>(hardware::Interposer::COB_ARRAY_WIDTH);
        this->_coordbias = QVector3D{
            -1.f * (COB_WIDTH * cob_array_height + COB_INTERAL * (cob_array_height - 1.f)) / 2.f,
            0.0f, 
            -1.f * (COB_WIDTH * cob_array_width + COB_INTERAL * (cob_array_width - 1.f)) / 2.f,
        };

        this->setMouseTracking(true);
        this->setFocusPolicy(Qt::StrongFocus);
        this->setFocus();
    }

    //! \brief destruct function
    RenderWidget::~RenderWidget() noexcept {
        this->makeCurrent();

        this->_axisVao.destroy();
        this->_axisVbo.destroy();
        
        for (auto cube : this->_cubes) {
            cube->positionsVBO.destroy();
            cube->verticesVBO.destroy();
            cube->texture->destroy();
        }

        this->_cubeIndicesVBO.destroy();
        this->_cubeVAO.destroy();

        // ... QScopePointer can't move..... shit!
        for (auto cube : this->_cubes) {
            delete cube;
        }

        this->doneCurrent();
    }

    //! \brief iniatil axis object
    void RenderWidget::initAxis(const QMatrix4x4& view, const QMatrix4x4& projection) {
        this->_axisVao.create();
        this->_axisVao.bind();
        
        // Set axis vertices buffer
        this->_axisVbo.create();
        this->_axisVbo.bind();
        this->_axisVbo.allocate(RenderWidget::axisVertices, sizeof(RenderWidget::axisVertices));

        // Set shader program 
        this->_axisShader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/shaders/axis.vert");
        this->_axisShader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/shaders/axis.frag");
        this->_axisShader.bind();
        this->_axisShader.setAttributeBuffer(0, GL_FLOAT, 0, 3, 3 * sizeof(float));
        this->_axisShader.enableAttributeArray(0);
        this->_axisVbo.release();

        QMatrix4x4 bias;
        bias.translate({0.0f, Z_BIAS, 0.0f});
        this->_axisShader.setUniformValue("bias", bias);
        this->_axisShader.setUniformValue("view", view);
        this->_axisShader.setUniformValue("projection", projection);
        this->_axisShader.release();

        this->_axisVao.release();
    }

    void RenderWidget::initCube(const QMatrix4x4& view, const QMatrix4x4& projection) {
        this->_cubeVAO.create();
        this->_cubeVAO.bind();

        // Shader program
        this->_cubeShader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/shaders/cube.vert");
        this->_cubeShader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/shaders/cube.frag");
        this->_cubeShader.bind();
        QMatrix4x4 bias;
        bias.translate({0.0f, 0.0f, 0.0f});
        this->_cubeShader.setUniformValue("view", view);
        this->_cubeShader.setUniformValue("bias", bias);
        this->_cubeShader.setUniformValue("projection", projection);
        
        // Indices VBO
        this->_cubeIndicesVBO.create();
        this->_cubeIndicesVBO.bind();
        this->_cubeIndicesVBO.allocate(RenderWidget::cubeIndices, sizeof(RenderWidget::cubeIndices));
        this->_cubeIndicesVBO.release();

        //////////////////// Build cob /////////////////////////////////
        auto positions = QVector<QVector3D>{};
        for (int row = 0; row < hardware::Interposer::COB_ARRAY_HEIGHT; ++row) {
            for (int col = 0; col < hardware::Interposer::COB_ARRAY_WIDTH; ++col) {
                auto rowPos = row * (RenderWidget::COB_WIDTH + RenderWidget::COB_INTERAL);
                auto colPos = col * (RenderWidget::COB_WIDTH + RenderWidget::COB_INTERAL);
                positions.push_back(QVector3D{rowPos, 0.f, colPos});
            }
        }
        auto cobCube = this->makeCube(COB_WIDTH, COB_WIDTH, COB_HEIGHT, qMove(positions), ":/textures/textures/cob.jpg", 0);
        this->_cubes.push_back(cobCube);

        /////////////////////// H Channel ///////////////////////////
        auto channelPos = QVector<QVector3D>{};
        
        for (int row = 0; row < hardware::Interposer::COB_ARRAY_HEIGHT; ++row) {
            for (int col = 0; col < hardware::Interposer::COB_ARRAY_WIDTH - 1; ++col) {
                auto rowPos = row * (COB_WIDTH + COB_INTERAL) + (COB_WIDTH - CHANNEL_WIDTH) / 2.f;
                auto colPos = col * (COB_WIDTH + COB_INTERAL) + COB_WIDTH;
                channelPos.push_back(QVector3D{ rowPos, 0.f, colPos });
            }
        }
        auto channelCube = this->makeCube(CHANNEL_LENGTH, CHANNEL_WIDTH, CHANNEL_HEIGHT, qMove(channelPos), ":/textures/textures/channel.jpg", 1);
        this->_cubes.push_back(channelCube);

        ///////////////////// V Channel //////////////////////////
        auto vchannelPos = QVector<QVector3D>{};
        for (int row = 0; row < hardware::Interposer::COB_ARRAY_HEIGHT - 1; ++row) {
            for (int col = 0; col < hardware::Interposer::COB_ARRAY_WIDTH; ++col) {
                auto rowPos = row * (COB_WIDTH + COB_INTERAL) + COB_WIDTH;
                auto colPos = col * (COB_WIDTH + COB_INTERAL) + (COB_WIDTH - CHANNEL_WIDTH) / 2.f;
                channelPos.push_back(QVector3D{ rowPos, 0.f, colPos });
            }
        }
        auto vchannelCube = this->makeCube(CHANNEL_WIDTH, CHANNEL_LENGTH, CHANNEL_HEIGHT, qMove(channelPos), ":/textures/textures/channel.jpg", 2);
        this->_cubes.push_back(vchannelCube);

        //////////////////// TOB /////////////////////////
        auto tobPos = QVector<QVector3D>{};
        for (auto& [_, coord] : hardware::Interposer::TOB_COORD_MAP) {
            auto rowPos = coord.row * (COB_WIDTH + COB_INTERAL) - (COB_INTERAL + TOB_WIDTH) / 2.f;
            auto colPos = coord.col * (COB_WIDTH + COB_INTERAL) - (TOB_WIDTH - COB_WIDTH) / 2.f;
            tobPos.push_back(QVector3D{rowPos, 0.f, colPos});
        }

        auto tobCube = this->makeCube(TOB_WIDTH, TOB_WIDTH, TOB_HEIGHT, qMove(tobPos), ":/textures/textures/tob.jpg", 3);
        this->_cubes.push_back(tobCube);

        this->_cubeVAO.release();
    }

    auto RenderWidget::makeCube(
        float length, float width, float height,
        QVector<QVector3D> positions,
        const QString& texturePath, unsigned int textureid
    ) -> Cube* {
        this->_cubeVAO.bind();

        auto cube = new Cube {};
        cube->vertices = generateCubVertices(length, width, height);

        cube->position = qMove(positions);
        cube->texture.reset(new QOpenGLTexture {QImage(texturePath)});
        cube->texture->setWrapMode(QOpenGLTexture::Repeat);
        cube->texture->setMinificationFilter(QOpenGLTexture::Linear);
        cube->texture->setMagnificationFilter(QOpenGLTexture::Linear);
        cube->texture->generateMipMaps();
        cube->texture->bind(textureid);
        cube->textureid = textureid;

        cube->verticesVBO.create();
        cube->verticesVBO.bind();
        cube->verticesVBO.allocate(cube->vertices.data(), cube->vertices.size() * sizeof(float));
        cube->positionsVBO.create();
        cube->positionsVBO.bind();
        cube->positionsVBO.allocate(cube->position.data(), cube->position.size() * sizeof(QVector3D));

        return cube;
    }

    //! \brief reset uniform view matrix in shaders
    void RenderWidget::updateViewMatrix() {
        this->makeCurrent();

        QMatrix4x4 view = this->getViewMatrix();

        this->_axisShader.bind();
        this->_axisShader.setUniformValue("view", view);
        this->_axisShader.release();

        this->_cubeShader.bind();
        this->_cubeShader.setUniformValue("view", view);
        this->_cubeShader.release();

        this->doneCurrent();
    }

    //! \brief update coord bias
    void RenderWidget::updateCoordBias() {
        this->makeCurrent();

        QMatrix4x4 bias;
        bias.translate(this->_coordbias);

        this->_cubeShader.bind();
        this->_cubeShader.setUniformValue("bias", bias);
        this->_cubeShader.release();

        this->doneCurrent();
    }

    //! \brief reset the view matrix
    void RenderWidget::resetView() {
        this->_posTheta = RenderWidget::DEFAULT_THETA_VALUE;
        this->_posPitch = RenderWidget::DEFAULT_PITCH_VALUE;
        this->_posRadius = RenderWidget::DEFAULT_RADIUS_VALUE;

        this->updateViewMatrix();
        this->updateCoordBias();

        this->reRender();
    }

    void RenderWidget::wheelEvent(QWheelEvent *event) {
        this->_posRadius += (event->angleDelta().y() / 100.0);
        this->updateViewMatrix();
        this->reRender();
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

            this->updateCoordBias();
        }

        this->_lastMousePos = event->pos();

        this->reRender();
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
        this->initializeOpenGLFunctions();

        this->glEnable(GL_DEPTH_TEST);

        // calculate uniform matrix
        QMatrix4x4 view = this->getViewMatrix();
        QMatrix4x4 projection;
        projection.perspective(45.0f, 1.0f * this->width() / this->height(), 0.1f, 100.0f);

        // create axis opengl object
        this->initAxis(view, projection);
        this->initCube(view, projection);

        this->updateViewMatrix();
        this->updateCoordBias();
    }

    void RenderWidget::resizeGL(int w, int h) {
        this->glViewport(0, 0, w, h);
    }

    void RenderWidget::paintGL() {
        // clear screen and buffer
        this->glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        this->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        this->renderAxis();
        this->renderCubes();
    }

    void RenderWidget::renderAxis() {
        this->_axisVao.bind();

        this->_axisShader.bind();
        // y axis
        this->_axisShader.setUniformValue("color", QVector3D(1.0f, 0.0f, 0.0f));
        this->glDrawArrays(GL_LINES, 0, 6);
        // x axis
        this->_axisShader.setUniformValue("color", QVector3D(0.0f, 1.0f, 0.0f));
        this->glDrawArrays(GL_LINES, 6, 6);
        // z axis
        this->_axisShader.setUniformValue("color", QVector3D(0.0f, 0.0f, 1.0f));
        this->glDrawArrays(GL_LINES, 12, 6);
        this->_axisShader.release();

        this->_axisVao.release();
    }

    void RenderWidget::renderCubes() {
        for (auto& cube : this->_cubes) {
            this->renderCube(cube);
        }
    }

    void RenderWidget::renderCube(Cube* cube) {
        this->_cubeVAO.bind();
        this->_cubeShader.bind();

        cube->verticesVBO.bind();
        this->_cubeShader.setAttributeBuffer(0, GL_FLOAT, 0, 3, 5 * sizeof(float));
        this->_cubeShader.setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(float), 2, 5 * sizeof(float));
        cube->verticesVBO.release();
        
        cube->positionsVBO.bind();
        this->_cubeShader.setAttributeBuffer(2, GL_FLOAT, 0, 3, sizeof(QVector3D));
        cube->positionsVBO.release();

        this->_cubeShader.enableAttributeArray(0);
        this->_cubeShader.enableAttributeArray(1);
        this->_cubeShader.enableAttributeArray(2);
        this->glVertexAttribDivisor(2, 1);

        this->_cubeIndicesVBO.bind();
        cube->texture->bind();
        this->_cubeShader.setUniformValue("tex", cube->textureid);
        this->glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, cube->position.size());        

        this->_cubeShader.disableAttributeArray(0);
        this->_cubeShader.disableAttributeArray(1);
        this->_cubeShader.disableAttributeArray(2);

        cube->verticesVBO.release();
        cube->positionsVBO.release();
        this->_cubeShader.release();
        this->_cubeVAO.release();
    }

    void RenderWidget::reRender() {
        this->parentWidget()->update();
    }

    QVector3D RenderWidget::getViewPos() const {
        float horizontalRadius = this->_posRadius * qCos(this->_posPitch);
        return QVector3D(horizontalRadius * qSin(this->_posTheta),
                        this->_posRadius * qSin(this->_posPitch),
                        horizontalRadius * qCos(this->_posTheta));
    }

    QMatrix4x4 RenderWidget::getViewMatrix() const {
        QMatrix4x4 view;
        view.lookAt(this->getViewPos(),
                    QVector3D(0.0f, 0.0f, 0.0f),
                    QVector3D(0.0f, 1.0f, 0.0f));
        return view;
    }

}

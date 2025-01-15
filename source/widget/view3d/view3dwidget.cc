#include "./view3dwidget.h"
#include "./view3dutil.hh"
#include "hardware/cob/cob.hh"
#include "hardware/cob/cobcoord.hh"
#include "hardware/cob/cobdirection.hh"
#include "hardware/cob/cobregister.hh"
#include "hardware/track/trackcoord.hh"

#include "./dialog/tobinfo.h"
#include "./dialog/cobinfo.h"
#include "./dialog/topdieinstinfo.h"
#include "qchar.h"
#include <hardware/interposer.hh>
#include <circuit/basedie.hh>
#include <debug/debug.hh>

#include <QVector>
#include <QMouseEvent>
#include <QtMath>
#include <QWheelEvent>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QFileInfo>
#include <QtDebug>
#include <QTimer>

namespace kiwi::widget {

    // Coord (y, z, x)
    static auto generateCubVertices(float length, float width, float height) -> QVector<float> {
        return QVector<float>{
            0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, length, 1.0f, 0.0f,
            width, 0.0f, 0.0f, 0.0f, 1.0f,
            width, 0.0f, length, 1.0f, 1.0f,

            0.0f, height, 0.0f, 1.0f, 0.0f,
            0.0f, height, length, 0.0f, 1.0f,
            width, height, 0.0f, 1.0f, 1.0f, 
            width, height, length, 0.0f, 0.0f,        
        };
    } 

    //! \brief verices date
    float View3DWidget::axisVertices[] = {
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

    int View3DWidget::cubeIndices[] = {
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
        2, 6, 7,
        2, 3, 7, 

        // Front
        0, 2, 6, 
        0, 4, 6, 

        // Back
        1, 3, 7, 
        1, 5, 7,
    };

    float View3DWidget::trackVertices[] = {
        0.0f, 0.0f, -0.5f, 0.0f, 0.0f, 0.5f,
    };

    //! \brief construct function
    View3DWidget::View3DWidget(
        hardware::Interposer* interposer, 
        circuit::BaseDie* basedie,
        QWidget *parent
    ) :
        QOpenGLWidget(parent),
        _interposer{interposer},
        _basedie{basedie}
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
    View3DWidget::~View3DWidget() noexcept {
        this->makeCurrent();

        this->_axisVAO.destroy();
        this->_axisVBO.destroy();
        
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
    void View3DWidget::initAxis(const QMatrix4x4& view, const QMatrix4x4& projection, const QMatrix4x4& bias) {
        this->_axisVAO.create();
        this->_axisVAO.bind();
        
        // Set axis vertices buffer
        this->_axisVBO.create();
        this->_axisVBO.bind();
        this->_axisVBO.allocate(View3DWidget::axisVertices, sizeof(View3DWidget::axisVertices));

        // Set shader program 
        this->_axisShader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/shader/axis.vert");
        this->_axisShader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/shader/axis.frag");
        this->_axisShader.bind();
        this->_axisShader.setAttributeBuffer(0, GL_FLOAT, 0, 3, 3 * sizeof(float));
        this->_axisShader.enableAttributeArray(0);
        this->_axisVBO.release();

        this->_axisShader.setUniformValue("view", view);
        this->_axisShader.setUniformValue("projection", projection);
        this->_axisShader.release();

        this->_axisVAO.release();
    }

    void View3DWidget::initCube(const QMatrix4x4& view, const QMatrix4x4& projection, const QMatrix4x4& bias) {
        this->_cubeVAO.create();
        this->_cubeVAO.bind();

        // Shader program
        this->_cubeShader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/shader/cube.vert");
        this->_cubeShader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/shader/cube.frag");
        this->_cubeShader.bind();

        this->_cubeShader.setUniformValue("view", view);
        this->_cubeShader.setUniformValue("bias", bias);
        this->_cubeShader.setUniformValue("projection", projection);
        
        // Indices VBO
        this->_cubeIndicesVBO.create();
        this->_cubeIndicesVBO.bind();
        this->_cubeIndicesVBO.allocate(View3DWidget::cubeIndices, sizeof(View3DWidget::cubeIndices));
        this->_cubeIndicesVBO.release();

        // Init cubes
        this->initCOBCube();
        this->initChannelCube();
        this->initTOBCube();
        // this->initTopdieInstance();

        this->_cubeVAO.release();
    }

    void View3DWidget::initFrame(const QMatrix4x4& view, const QMatrix4x4& projection, const QMatrix4x4& bias) {
        this->_frameVAO.create();
        this->_frameVAO.bind();
        
        // Set axis vertices buffer
        this->_frameVBO.create();

        // Set shader program 
        this->_frameShader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/shader/frame.vert");
        this->_frameShader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/shader/frame.frag");
        this->_frameShader.bind();

        this->_frameShader.setUniformValue("bias", bias);
        this->_frameShader.setUniformValue("view", view);
        this->_frameShader.setUniformValue("projection", projection);
        this->_frameShader.release();

        this->_frameVAO.release();
    }

    void View3DWidget::initTracks(const QMatrix4x4& view, const QMatrix4x4& projection, const QMatrix4x4& bias) {
        this->_trackVAO.create();
        this->_trackVBO.create();
        this->_trackInstVBO.create();
        this->_trackShader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/shader/track.vert");
        this->_trackShader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/shader/track.frag");

        this->_trackVAO.bind();

        this->_trackVBO.bind();
        this->_trackVBO.allocate(View3DWidget::trackVertices, sizeof(View3DWidget::trackVertices));

        this->_trackShader.bind();
        this->_trackShader.setAttributeBuffer(0, GL_FLOAT, 0, 3, 3 * sizeof(float));
        this->_trackShader.enableAttributeArray(0);

        this->_frameShader.setUniformValue("bias", bias);
        this->_trackShader.setUniformValue("view", view);
        this->_trackShader.setUniformValue("projection", projection);

        this->_trackVAO.release();
        this->_trackShader.release();
        this->_trackVAO.release();

        this->updateTrackInstMatrices();
    }

    void View3DWidget::initTOBCube() {
        auto positions = QVector<QVector3D>{};
        // MARK: TOB coord should diff type with cob coord...
        for (auto& [tobcoord, basecoord] : hardware::Interposer::TOB_COORD_MAP) {
            auto position = this->tobPosition(basecoord);
            positions.push_back(position);
            this->_tobs.push_back(this->_interposer->get_tob(tobcoord).value());
        }
        auto tobCube = this->addCube(CubeType::TOB, TOB_WIDTH, TOB_WIDTH, TOB_HEIGHT, ":/texture/texture/tob.jpg", 3, qMove(positions));
    }

    void View3DWidget::initCOBCube() {
        auto positions = QVector<QVector3D>{};
        for (int row = 0; row < hardware::Interposer::COB_ARRAY_HEIGHT; ++row) {
            for (int col = 0; col < hardware::Interposer::COB_ARRAY_WIDTH; ++col) {
                auto coord = hardware::COBCoord{row, col};
                auto position = View3DWidget::cobPosition(coord);
                positions.push_back(position);
                this->_cobs.emplace_back(this->_interposer->get_cob(coord).value());
            }
        }
        auto cobCube = this->addCube(CubeType::COB, COB_WIDTH, COB_WIDTH, COB_HEIGHT, ":/texture/texture/cob.jpg", 0, qMove(positions));
    }

    void View3DWidget::initTopdieInstance() {
        auto positions = QVector<QVector3D>{};
        for (auto& [name, topdieinst] : this->_basedie->topdie_insts()) {
            auto tobcoord = topdieinst->tob()->coord();
            auto basecoord = hardware::Interposer::TOB_COORD_MAP.at(tobcoord);
        
            positions.push_back(View3DWidget::topdiePosition(basecoord));
            this->_topdieinsts.push_back(topdieinst.get());
        }

        auto shipCube = this->addCube(CubeType::Topdie, TOPDIE_WIDTH, TOPDIE_WIDTH, TOPDIE_HEIGHT, ":/texture/texture/topdie.jpg", 4, qMove(positions));
    }

    void View3DWidget::initChannelCube() {
        ///////////////////////// H Channel /////////////////////////
        auto channelPos = QVector<QVector3D>{};
        
        for (int row = 0; row < hardware::Interposer::COB_ARRAY_HEIGHT; ++row) {
            for (int col = 0; col < hardware::Interposer::COB_ARRAY_WIDTH - 1; ++col) {
                auto position = this->channelPosition(row, col, hardware::TrackDirection::Horizontal);
                channelPos.push_back(position);
            }
        }
        auto channelCube = this->makeCube(CubeType::Channel, CHANNEL_LENGTH, CHANNEL_WIDTH, CHANNEL_HEIGHT, qMove(channelPos), ":/texture/texture/channel.jpg", 1);
        this->_cubes.push_back(channelCube);

        ///////////////////////// V Channel /////////////////////////
        auto vchannelPos = QVector<QVector3D>{};
        for (int row = 0; row < hardware::Interposer::COB_ARRAY_HEIGHT - 1; ++row) {
            for (int col = 0; col < hardware::Interposer::COB_ARRAY_WIDTH; ++col) {
                auto position = this->channelPosition(row, col, hardware::TrackDirection::Vertical);
                channelPos.push_back(position);
            }
        }
        auto vchannelCube = this->makeCube(CubeType::Channel, CHANNEL_WIDTH, CHANNEL_LENGTH, CHANNEL_HEIGHT, qMove(channelPos), ":/texture/texture/channel.jpg", 2);
        this->_cubes.push_back(vchannelCube);
    }

    auto View3DWidget::channelPosition(std::i64 row, std::i64 col, hardware::TrackDirection dir) -> QVector3D {
        switch (dir) {
            case hardware::TrackDirection::Horizontal: {
                auto rowPos = row * (COB_WIDTH + COB_INTERAL) + (COB_WIDTH - CHANNEL_WIDTH) / 2.f;
                auto colPos = col * (COB_WIDTH + COB_INTERAL) + COB_WIDTH;
                return QVector3D{ rowPos, 0.f, colPos };
            }
            case hardware::TrackDirection::Vertical: {
                auto rowPos = row * (COB_WIDTH + COB_INTERAL) + COB_WIDTH;
                auto colPos = col * (COB_WIDTH + COB_INTERAL) + (COB_WIDTH - CHANNEL_WIDTH) / 2.f;
                return QVector3D{ rowPos, 0.f, colPos };
            }
        }
        debug::unimplement();
    }

    auto View3DWidget::trackPosition(const hardware::TrackCoord& coord) -> std::Tuple<QVector3D, QVector3D> {
        // MARK: Channel position error
        auto channelPos = this->channelPosition(coord.row, coord.col, coord.dir);
        auto begin = QVector3D{};
        auto end = QVector3D{};
        
        switch (coord.dir) {
            case hardware::TrackDirection::Horizontal: {
                auto xpos = channelPos.x() + (coord.index+1) * TRACK_INTERVAL;

                begin.setX(xpos);
                begin.setZ(channelPos.z() - COB_WIDTH - CHANNEL_LENGTH);

                end.setX(xpos);
                end.setZ(channelPos.z() - COB_WIDTH);

                break;
            }
            case hardware::TrackDirection::Vertical: {
                auto zpos = channelPos.z() + (coord.index+1) * TRACK_INTERVAL;

                begin.setZ(zpos);
                begin.setX(channelPos.x() - COB_WIDTH - CHANNEL_LENGTH);

                end.setZ(zpos);
                end.setX(channelPos.x() - COB_WIDTH);

                break;
            }
        }

        begin.setY(channelPos.y() + COB_HEIGHT + 0.05);
        end.setY(channelPos.y() + COB_HEIGHT + 0.05);

        return {begin, end};
    }

    auto View3DWidget::cobPosition(const hardware::Coord& coord) -> QVector3D {
        auto rowPos = coord.row * (View3DWidget::COB_WIDTH + View3DWidget::COB_INTERAL);
        auto colPos = coord.col * (View3DWidget::COB_WIDTH + View3DWidget::COB_INTERAL);
        return QVector3D {rowPos, 0.f, colPos};
    }

    auto View3DWidget::tobPosition(const hardware::TOBCoord& coord) -> QVector3D {
        auto rowPos = coord.row * (COB_WIDTH + COB_INTERAL) - (COB_INTERAL + TOB_WIDTH) / 2.f;
        auto colPos = coord.col * (COB_WIDTH + COB_INTERAL) - (TOB_WIDTH - COB_WIDTH) / 2.f;
        return QVector3D { rowPos, 0.f, colPos };
    }

    auto View3DWidget::topdiePosition(const hardware::TOBCoord& coord) -> QVector3D {
        auto rowPos = coord.row * (COB_WIDTH + COB_INTERAL) - (COB_INTERAL + TOPDIE_WIDTH) / 2.f;
        auto colPos = coord.col * (COB_WIDTH + COB_INTERAL) - (TOPDIE_WIDTH - COB_WIDTH) / 2.f;
        return QVector3D { rowPos, TOB_HEIGHT, colPos };
    }

    auto View3DWidget::makeCube(
        CubeType type,
        float length, float width, float height,
        QVector<QVector3D> positions,
        const QString& texturePath, unsigned int textureid
    ) -> Cube* {
        this->_cubeVAO.bind();

        auto cube = new Cube {};
        cube->type = type;
        cube->length = length;
        cube->width = width;
        cube->height = height;

        cube->vertices = generateCubVertices(length, width, height);

        cube->positions = qMove(positions);
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
        cube->positionsVBO.allocate(cube->positions.data(), cube->positions.size() * sizeof(QVector3D));

        return cube;
    }

    auto View3DWidget::addCube(
        CubeType type, float length, float width, float height,
        const QString& texturePath, unsigned int textureid,
        QVector<QVector3D> positions
    ) -> Cube*
    {
        this->_cubeVAO.bind();

        auto cube = new Cube {};
        cube->type = type;
        cube->length = length;
        cube->width = width;
        cube->height = height;

        cube->vertices = generateCubVertices(length, width, height);
        
        cube->positions = qMove(positions);

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
        cube->positionsVBO.allocate(cube->positions.data(), cube->positions.size() * sizeof(QVector3D));

        this->_cubes.push_back(cube);

        return cube;
    }

    auto View3DWidget::addTrack(const QVector3D &begin, const QVector3D &end, bool update) -> void {
        // qDebug() << "Add track from " << begin << " to " << end;
        QMatrix4x4 model;

        // move to the mid of two position!
        model.translate((begin + end) / 2.);
        // model.translate(QVector3D(0.0f, Z_BIAS, 0.0f));

        // sacle
        float distance = begin.distanceToPoint(end);
        model.scale(distance);

        // roate
        auto direction = (end - begin);
        auto horizontalDiretion = QVector3D{direction.x(), 0.0f, direction.z()}.normalized();

        // vertical ratate
        if (horizontalDiretion.length() != 0) {
            float phi = View3DUtil::radian2Angle(qAsin(direction.y() / direction.length()));
            QVector3D verticalNormal{-1 * horizontalDiretion.z(), 0.0f, horizontalDiretion.x()};
            model.rotate(phi, verticalNormal);

            // horizontal rotate
            float theta = View3DUtil::radian2Angle(qAcos(horizontalDiretion.z()));
            model.rotate(( horizontalDiretion.x() > 0.0f ? theta : -1.0f * theta ), {0.0f, 1.0f, 0.0f});
        }
        else {
            model.rotate(90, {1.0f, 0.0f, 0.0f});
        }

        this->_trackInstMatrices.push_back(model);

        if (update) {
            this->updateTrackInstMatrices();
        }
    }

    auto View3DWidget::addTrack(const hardware::TrackCoord& coord, bool update) -> void {
        auto [begin, end] = this->trackPosition(coord);
        this->addTrack(begin, end, update);
    }

    auto View3DWidget::displayRoutingResult() -> void {
        if (!this->isVisible()) {
            QTimer::singleShot(0, this, &View3DWidget::displayRoutingResult);
            return;
        }

        this->makeCurrent();

        using enum hardware::COBDirection;
        using enum hardware::TrackDirection;
        using hardware::COBSwState;
        for (auto& [coord, cob] : this->_interposer->cobs()) {
            // Left to Up
            for (auto track_index = 0; track_index < hardware::COB::INDEX_SIZE; track_index++) {
                auto from_cob_index = hardware::COB::track_index_to_cob_index(track_index);
                auto to_cob_index = hardware::COB::cob_index_map(Left, from_cob_index, Up);
                auto value = cob->get_sw_resgiter_value(Left, from_cob_index, Up);
                if (value == COBSwState::Connected) {
                    auto [beginl, endl] = this->trackPosition(cob->to_dir_track_coord(Left, from_cob_index));
                    auto [beginu, endu] = this->trackPosition(cob->to_dir_track_coord(Up, to_cob_index));

                    /*
                                           endu
                                            |
                                          beginu
                                       +---------
                        beginl -> endl |
                                       |

                    */

                    this->addTrack(beginl, endl, false);
                    this->addTrack(beginu, endu, false);
                    this->addTrack(endl, beginu, false);
                }
            }

            // Left to Down
            for (auto track_index = 0; track_index < hardware::COB::INDEX_SIZE; track_index++) {
                auto from_cob_index = hardware::COB::track_index_to_cob_index(track_index);
                auto to_cob_index = hardware::COB::cob_index_map(Left, from_cob_index, Down);
                auto value = cob->get_sw_resgiter_value(Left, from_cob_index, Down);
                if (value == COBSwState::Connected) {
                    auto [beginl, endl] = this->trackPosition(cob->to_dir_track_coord(Left, from_cob_index));
                    auto [begind, endd] = this->trackPosition(cob->to_dir_track_coord(Down, to_cob_index));

                    /*
                                       |
                        beginl -> endl |
                                       |
                                       +------------
                                          endd
                                           |
                                         begind 
                    */

                    this->addTrack(beginl, endl, false);
                    this->addTrack(begind, endd, false);
                    this->addTrack(endl, endd, false);
                }
            }

            // Right to Up
            for (auto track_index = 0; track_index < hardware::COB::INDEX_SIZE; track_index++) {
                auto from_cob_index = hardware::COB::track_index_to_cob_index(track_index);
                auto to_cob_index = hardware::COB::cob_index_map(Right, from_cob_index, Up);
                auto value = cob->get_sw_resgiter_value(Right, from_cob_index, Up);
                if (value == COBSwState::Connected) {
                    auto [beginr, endr] = this->trackPosition(cob->to_dir_track_coord(Right, from_cob_index));
                    auto [beginu, endu] = this->trackPosition(cob->to_dir_track_coord(Up, to_cob_index));

                    /*
                                endu
                                  |
                                beginu
                            ---------+
                                     |
                                     | beginr -> endr
                                     |
                    */

                    this->addTrack(beginr, endr, false);
                    this->addTrack(beginu, endu, false);
                    this->addTrack(beginr, beginu, false);
                }
            }

            // Right to Down
            for (auto track_index = 0; track_index < hardware::COB::INDEX_SIZE; track_index++) {
                auto from_cob_index = hardware::COB::track_index_to_cob_index(track_index);
                auto to_cob_index = hardware::COB::cob_index_map(Right, from_cob_index, Down);
                auto value = cob->get_sw_resgiter_value(Right, from_cob_index, Down);
                if (value == COBSwState::Connected) {
                    auto [beginr, endr] = this->trackPosition(cob->to_dir_track_coord(Right, from_cob_index));
                    auto [begind, endd] = this->trackPosition(cob->to_dir_track_coord(Down, to_cob_index));

                    /*
                                        |
                                        | beginr -> endr
                                        |
                            ------------+
                                 endd
                                   |
                                begind 
                    */

                    this->addTrack(beginr, endr, false);
                    this->addTrack(begind, endd, false);
                    this->addTrack(beginr, endd, false);
                }
            }

            // Left to Right
            for (auto track_index = 0; track_index < hardware::COB::INDEX_SIZE; track_index++) {
                auto from_cob_index = hardware::COB::track_index_to_cob_index(track_index);
                auto to_cob_index = hardware::COB::cob_index_map(Left, from_cob_index, Right);
                auto value = cob->get_sw_resgiter_value(Left, from_cob_index, Right);
                if (value == COBSwState::Connected) {
                    auto [beginl, endl] = this->trackPosition(cob->to_dir_track_coord(Left, from_cob_index));
                    auto [beginr, endr] = this->trackPosition(cob->to_dir_track_coord(Right, to_cob_index));

                    /*
                                       |        |
                        beginl -> endl |        | beginr -> endr
                                       |        |

                    */

                    this->addTrack(beginl, endl, false);
                    this->addTrack(beginr, endr, false);
                    this->addTrack(endl, beginr, false);
                }
            }

            // Up to Down
            for (auto track_index = 0; track_index < hardware::COB::INDEX_SIZE; track_index++) {
                auto from_cob_index = hardware::COB::track_index_to_cob_index(track_index);
                auto to_cob_index = hardware::COB::cob_index_map(Up, from_cob_index, Down);
                auto value = cob->get_sw_resgiter_value(Up, from_cob_index, Down);
                if (value == COBSwState::Connected) {
                    auto [beginu, endu] = this->trackPosition(cob->to_dir_track_coord(Up, from_cob_index));
                    auto [begind, endd] = this->trackPosition(cob->to_dir_track_coord(Down, to_cob_index));

                    /*
                                endu
                                  |
                                beginu
                            ------------


                            ------------
                                 endd
                                   |
                                begind 
                    */

                    this->addTrack(beginu, endu, false);
                    this->addTrack(begind, endd, false);
                    this->addTrack(beginu, endd, false);
                }
            }
        }
        this->updateTrackInstMatrices();
        this->updateBiasMatrix();

        this->doneCurrent();
    }

    void View3DWidget::updateViewMatrix() {
        QMatrix4x4 view = this->getViewMatrix();
        this->updateViewMatrix(view);
    }

    void View3DWidget::updateViewMatrix(const QMatrix4x4& view) {
        this->makeCurrent();

        this->_axisShader.bind();
        this->_axisShader.setUniformValue("view", view);
        this->_axisShader.release();

        this->_cubeShader.bind();
        this->_cubeShader.setUniformValue("view", view);
        this->_cubeShader.release();

        this->_frameShader.bind();
        this->_frameShader.setUniformValue("view", view);
        this->_frameShader.release();

        this->_trackShader.bind();
        this->_trackShader.setUniformValue("view", view);
        this->_trackShader.release();

        this->doneCurrent();
    }

    void View3DWidget::updateBiasMatrix() {
        auto bias = this->getBiasMatrix();
        this->updateBiasMatrix(bias);
    }

    void View3DWidget::updateBiasMatrix(const QMatrix4x4& bias) {
        this->makeCurrent();

        this->_cubeShader.bind();
        this->_cubeShader.setUniformValue("bias", bias);
        this->_cubeShader.release();

        this->_frameShader.bind();
        this->_frameShader.setUniformValue("bias", bias);
        this->_frameShader.release();

        this->_trackShader.bind();
        this->_trackShader.setUniformValue("bias", bias);
        this->_trackShader.release();

        this->doneCurrent();
    }

    auto View3DWidget::getTOBByCubeIndeces(int index) -> hardware::TOB* {
        return this->_tobs.at(index);
    }

    auto View3DWidget::getCOBByCubeIndeces(int index) -> hardware::COB* {
        return this->_cobs.at(index);
    }

    auto View3DWidget::getTopdieInstByCubeIndeces(int index) -> circuit::TopDieInstance* {
        return this->_topdieinsts.at(index);
    }

    void View3DWidget::resetView() {
        this->_posTheta = View3DWidget::DEFAULT_THETA_VALUE;
        this->_posPitch = View3DWidget::DEFAULT_PITCH_VALUE;
        this->_posRadius = View3DWidget::DEFAULT_RADIUS_VALUE;

        this->updateViewMatrix();
        this->updateBiasMatrix();

        this->reRender();
    }

    void View3DWidget::wheelEvent(QWheelEvent *event) {
        this->_posRadius += (event->angleDelta().y() / 100.0);
        this->updateViewMatrix();
        this->reRender();
    }

    void View3DWidget::mousePressEvent(QMouseEvent *event) {
        this->_mouseButton = event->button();
        this->_lastMousePos = event->pos();
    }

    void View3DWidget::mouseReleaseEvent(QMouseEvent *event) {
        Q_UNUSED(event);
        this->_mouseButton = Qt::NoButton;
    }

    void View3DWidget::mouseMoveEvent(QMouseEvent *event) {
        QPoint delta = event->pos() - this->_lastMousePos;
        
        int width = this->width();
        int height = this->height();

        QMatrix4x4 projection = this->getProjectionMatrix();
        QMatrix4x4 view = this->getViewMatrix();

        auto raySource = getViewPos();
        auto rayDirection = View3DUtil::screenPosToWorldRay(event->pos(), width, height, projection, view);
        this->resetPointedCube(raySource, rayDirection);

        // rotate track
        if (this->_mouseButton == Qt::LeftButton) {
            this->_posTheta -= (delta.x() / 100.0);
            this->_posPitch += (delta.y() / 100.0);

            // pitch range
            if (this->_posPitch > View3DWidget::MAX_PITCH)
                this->_posPitch = View3DWidget::MAX_PITCH;
            else if (this->_posPitch < View3DWidget::MIN_PITCH)
                this->_posPitch = View3DWidget::MIN_PITCH;

            this->updateViewMatrix(view);
        }

        // translate track
        else if (this->_mouseButton == Qt::RightButton) {
            float deltaX = delta.x() * -1.0f * qSin(this->_posTheta)
                        + delta.y() * qCos(this->_posTheta);
            float deltaY = delta.x() * qCos(this->_posTheta)
                        + delta.y() * qSin(this->_posTheta);
            this->_coordbias += QVector3D(deltaY / 30.0,
                                        0.0f,
                                        deltaX / 30.0);

            this->updateBiasMatrix();
        }

        this->_lastMousePos = event->pos();

        this->reRender();
    }

    void View3DWidget::mouseDoubleClickEvent(QMouseEvent *event) {
        if (!this->_pointedCube.has_value())
            return;

        auto [cube, i] = *(this->_pointedCube);
        switch (cube->type) {
            case CubeType::COB: {
                this->makeCurrent();
                auto cob = this->getCOBByCubeIndeces(i);
                auto dialog = COBInfoDialog{cob};
                dialog.exec();
                this->doneCurrent();
                break;
            }
            case CubeType::TOB: {
                this->makeCurrent();
                auto tob = this->getTOBByCubeIndeces(i);
                auto dialog = TOBInfoDialog{tob};
                dialog.exec();
                this->doneCurrent();
                break;                    
            }
            case CubeType::Topdie: {
                this->makeCurrent();
                auto topdieinst = this->getTopdieInstByCubeIndeces(i);
                auto dialog = TopDieInsDialog{topdieinst};
                dialog.exec();
                this->doneCurrent();
                break;
            }
            default: break;
        }

        this->updateTrackInstMatrices();
    }

    void View3DWidget::dragEnterEvent(QDragEnterEvent *event) {
        // if (event->mimeData()->hasUrls()) {
        //     QString filename = event->mimeData()->urls()[0].fileName();
        //     QFileInfo info(filename);
        //     QString ex = info.suffix().toUpper();
        //     if (ex == "GCODE" || ex == "LC")
        //         event->acceptProposedAction();
        //     else
        //         event->ignore();
        // }
        // else {
        //     event->ignore();
        // }
    }

    void View3DWidget::resizeEvent(QResizeEvent *event) {
        QOpenGLWidget::resizeEvent(event);
        this->updateProjectionMatrix();
        this->reRender();
    }

    void View3DWidget::initializeGL() {
        this->initializeOpenGLFunctions();

        this->glEnable(GL_DEPTH_TEST);

        this->glLineWidth(5.0f);

        // calculate uniform matrix
        auto view = this->getViewMatrix();
        auto projection = this->getProjectionMatrix();
        auto bias = this->getBiasMatrix();

        // create axis opengl object
        this->initAxis(view, projection, bias);
        this->initCube(view, projection, bias);
        this->initFrame(view, projection, bias);
        this->initTracks(view, projection, bias);
    }

    void View3DWidget::resizeGL(int w, int h) {
        this->glViewport(0, 0, w, h);
    }

    void View3DWidget::paintGL() {
        // clear screen and buffer
        this->glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        this->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        this->renderAxis();
        this->renderCubes();
        this->renderFrame();
        this->renderTracks();
    }

    void View3DWidget::renderAxis() {
        this->_axisVAO.bind();

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

        this->_axisVAO.release();
    }

    void View3DWidget::renderCubes() {
        for (auto& cube : this->_cubes) {
            this->renderCube(cube);
        }
    }

    void View3DWidget::renderFrame() {
        if (this->_pointedCube.has_value()) {
            this->_frameVAO.bind();
            this->_frameShader.bind();
    
            this->_frameVBO.bind();

            this->_frameVBO.allocate(this->_frameVertices.data(), this->_frameVertices.size() * sizeof(QVector3D));

            this->_frameShader.bind();
            this->_frameShader.setAttributeBuffer(0, GL_FLOAT, 0, 3, 3 * sizeof(float));
            this->_frameShader.enableAttributeArray(0);

            this->glDrawArrays(GL_LINES, 0, 24);
    
            this->_frameShader.release();
            this->_frameVAO.release();
        }
    }

    void View3DWidget::renderTracks() {
        this->_trackVAO.bind();
        this->_trackShader.bind();

        this->glDrawArraysInstanced(GL_LINES, 0, 2, this->_trackInstMatrices.size());

        this->_trackShader.release();
        this->_trackVAO.release();
    }

    void View3DWidget::renderCube(Cube* cube) {
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
        this->glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, cube->positions.size());        

        this->_cubeShader.disableAttributeArray(0);
        this->_cubeShader.disableAttributeArray(1);
        this->_cubeShader.disableAttributeArray(2);

        cube->verticesVBO.release();
        cube->positionsVBO.release();
        this->_cubeShader.release();
        this->_cubeVAO.release();
    }

    void View3DWidget::reRender() {
        this->update();
        // this->parentWidget()->update();
    }

    QVector3D View3DWidget::getViewPos() const {
        float horizontalRadius = this->_posRadius * qCos(this->_posPitch);
        return QVector3D(horizontalRadius * qSin(this->_posTheta),
                        this->_posRadius * qSin(this->_posPitch),
                        horizontalRadius * qCos(this->_posTheta));
    }

    auto View3DWidget::getViewMatrix() const -> QMatrix4x4 {
        QMatrix4x4 view;
        view.lookAt(this->getViewPos(),
                    QVector3D(0.0f, 0.0f, 0.0f),
                    QVector3D(0.0f, 1.0f, 0.0f));
        return view;
    }

    auto View3DWidget::getProjectionMatrix() const -> QMatrix4x4 {
        QMatrix4x4 projection;
        projection.perspective(45.0f, 1.0f * this->width() / this->height(), 0.1f, 100.0f);
        return projection;
    }

    auto View3DWidget::getBiasMatrix() const -> QMatrix4x4 {
        auto bias = QMatrix4x4{};
        bias.translate(this->_coordbias);
        return bias;
    }

    void View3DWidget::updateProjectionMatrix() {
        auto projection = this->getProjectionMatrix();
        this->updateProjectionMatrix(projection);
    }

    void View3DWidget::updateProjectionMatrix(const QMatrix4x4& projection) {
        this->makeCurrent();
        this->_axisShader.setUniformValue("projection", projection);
        this->_cubeShader.setUniformValue("projection", projection);
        this->_frameShader.setUniformValue("projection", projection);
        this->_trackShader.setUniformValue("projection", projection);
        this->doneCurrent();
    }

    void View3DWidget::updateTrackInstMatrices() {
        this->_trackVAO.bind();

        this->_trackInstVBO.bind();
        this->_trackInstVBO.allocate(
            this->_trackInstMatrices.data(),                                
            this->_trackInstMatrices.size() * sizeof(QMatrix4x4)
        );

        this->_trackShader.bind();
        this->_trackShader.setAttributeBuffer(1, GL_FLOAT, 0, 4, sizeof(QMatrix4x4));
        this->_trackShader.enableAttributeArray(1);
        this->_trackShader.setAttributeBuffer(2, GL_FLOAT, sizeof(QVector4D), 4, sizeof(QMatrix4x4));
        this->_trackShader.enableAttributeArray(2);
        this->_trackShader.setAttributeBuffer(3, GL_FLOAT, 2 * sizeof(QVector4D), 4, sizeof(QMatrix4x4));
        this->_trackShader.enableAttributeArray(3);
        this->_trackShader.setAttributeBuffer(4, GL_FLOAT, 3 * sizeof(QVector4D), 4, sizeof(QMatrix4x4));
        this->_trackShader.enableAttributeArray(4);

        this->glVertexAttribDivisor(1, 1);
        this->glVertexAttribDivisor(2, 1);
        this->glVertexAttribDivisor(3, 1);
        this->glVertexAttribDivisor(4, 1);

        this->_trackShader.release();
        this->_trackInstVBO.release();
        this->_trackVAO.release();
    }

    auto View3DWidget::resetPointedCube(const QVector3D& raySource, const QVector3D& rayDirection) -> void {
        for (auto cube : this->_cubes) {
            if (cube->type == CubeType::Channel) {
                continue;
            }
            auto baseBoxMin = QVector3D{0.f, 0.f, 0.f} + this->_coordbias;
            auto baseBoxMax = QVector3D{cube->width, cube->height, cube->length} + this->_coordbias;
            for (int i = 0; i < cube->positions.size(); ++i) {
                auto& pos = cube->positions[i];
                auto boxMin = baseBoxMin + pos;
                auto boxMax = baseBoxMax + pos;
                if (View3DUtil::doesRayIntersectBox(raySource, rayDirection, boxMin, boxMax)) {
                    auto oneCube = OneCube{cube, i};
                    if (!this->_pointedCube.has_value() || (*this->_pointedCube) != oneCube) {
                        // Create a new pointed cubes!
                        auto& pos = cube->positions[i];

                        auto x = pos.x(), y = pos.y(), z = pos.z();
                        auto width = cube->width, height = cube->height, length = cube->length;

                        auto p1 = QVector3D{x, y, z};
                        auto p2 = QVector3D{x+width, y, z};
                        auto p3 = QVector3D{x, y, z+length};
                        auto p4 = QVector3D{x+width, y, z+length};
                        auto p5 = QVector3D{x, y+height, z};
                        auto p6= QVector3D{x+width, y+height, z};
                        auto p7 = QVector3D{x, y+height, z+length};
                        auto p8 = QVector3D{x+width, y+height, z+length};

                        this->_frameVertices[0] = this->_frameVertices[2] = this->_frameVertices[8] = p1;
                        this->_frameVertices[1] = this->_frameVertices[4] = this->_frameVertices[10] = p2;
                        this->_frameVertices[3] = this->_frameVertices[6] = this->_frameVertices[12] = p3;
                        this->_frameVertices[5] = this->_frameVertices[7] = this->_frameVertices[14] = p4;
                        this->_frameVertices[9] = this->_frameVertices[16] = this->_frameVertices[18] = p5;
                        this->_frameVertices[11] = this->_frameVertices[17] = this->_frameVertices[20] = p6;
                        this->_frameVertices[13] = this->_frameVertices[19] = this->_frameVertices[22] = p7;
                        this->_frameVertices[15] = this->_frameVertices[21] = this->_frameVertices[23] = p8;
                    }
                    this->_pointedCube.emplace(oneCube);
                    return;
                }
            }
        }
        this->_pointedCube.reset();
    }

}

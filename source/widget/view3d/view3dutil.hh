#pragma once

#include "qmatrix4x4.h"
#include "qvector3d.h"
#include <QVector3D>

namespace kiwi::widget {

    struct View3DUtil {

        static auto radian2Angle(float radian) -> float;

        static auto angle2Radian(float angle) -> float;

        static auto doesRayIntersectBox(
            const QVector3D& lightSource, 
            const QVector3D& lightDirection, 
            const QVector3D& boxMin, 
            const QVector3D& boxMax) -> bool;

        static auto screenPosToWorldRay(
            const QPoint& screenPos,
            int width, int height,
            const QMatrix4x4 projection,
            const QMatrix4x4 view) -> QVector3D;
    };

}
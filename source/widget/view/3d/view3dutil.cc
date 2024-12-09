#include "./view3dutil.hh"

namespace kiwi::widget {

    float View3DUtil::radian2Angle(float radian) {
        return radian / 3.14159f * 180.0f;
    }

    float View3DUtil::angle2Radian(float angle) {
        return angle * 3.14159f / 180.0f;
    }

    auto View3DUtil::doesRayIntersectBox(
        const QVector3D& lightSource, 
        const QVector3D& lightDirection, 
        const QVector3D& boxMin, 
        const QVector3D& boxMax) -> bool
    {
        // 光线方向的倒数
        QVector3D invDir(1.0f / lightDirection.x(), 
                        1.0f / lightDirection.y(), 
                        1.0f / lightDirection.z());

        // 计算每个面与光线的交点参数 t
        float t1 = (boxMin.x() - lightSource.x()) * invDir.x();
        float t2 = (boxMax.x() - lightSource.x()) * invDir.x();
        float t3 = (boxMin.y() - lightSource.y()) * invDir.y();
        float t4 = (boxMax.y() - lightSource.y()) * invDir.y();
        float t5 = (boxMin.z() - lightSource.z()) * invDir.z();
        float t6 = (boxMax.z() - lightSource.z()) * invDir.z();

        // 获取进入和离开长方体的 t 值
        float tMin = std::max({std::min(t1, t2), std::min(t3, t4), std::min(t5, t6)});
        float tMax = std::min({std::max(t1, t2), std::max(t3, t4), std::max(t5, t6)});

        // 判断是否相交
        return tMax >= std::max(0.0f, tMin); // 光线与长方体相交（t >= 0 确保相交点在光线前方）
    }

    auto View3DUtil::screenPosToWorldRay(
        const QPoint& screenPos,
        int width, int height,
        const QMatrix4x4 projection,
        const QMatrix4x4 view) -> QVector3D 
    {

        // 归一化屏幕坐标 (从 [0, width] 转为 [-1, 1])
        float x = (2.0f * screenPos.x()) / width - 1.0f;
        float y = 1.0f - (2.0f * screenPos.y()) / height; // OpenGL 的 Y 轴是反的
        float z = 1.0f; // 鼠标在远裁剪面

        // 将屏幕坐标转换为 4D 点 (Clip Space)
        QVector4D rayClip(x, y, -1.0f, 1.0f);

        // 转换为 4D 眼空间坐标 (Eye Space)
        QVector4D rayEye = projection.inverted() * rayClip;
        rayEye.setZ(-1.0f);
        rayEye.setW(0.0f);

        // 转换为 3D 世界空间 (World Space)
        QVector3D rayWorld = (view.inverted() * rayEye).toVector3D();
        rayWorld.normalize();

        return rayWorld; // 返回光线的方向
    }

}
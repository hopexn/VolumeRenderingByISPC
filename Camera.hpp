#ifndef CAMERA_H
#define CAMERA_H

#include <cmath>
#include "util.h"
#include "Vector.hpp"
const float M_PI_F = 3.141592653f;

class Camera {
public:
/**
 * 构造函数
 * 摄像机默认属性
 * posiiton： 0, 0, 2
 * focus: 0, 0, 0
 * right: 0, 1, 0
 * fov: 60°
 */
    Camera() : position(0, 0, 2), focus(0, 0, 0), right_direction(0, 1, 0), fov(30.f) {}

/**
 * 获取摄像机位置
 * @return 摄像机位置向量
 */
    inline const Vector3f &getPosition() const {
        return position;
    }

/**
 * 设置摄像机位置
 * @param position
 */
    inline void setPosition(const Vector3f &_position) {
        position = _position;
    }

/**
 * 获取摄像机焦点位置
 * @return 摄像机焦点位置
 */
    inline Vector3f &getFocus() {
        return focus;
    }

/**
 * 设置摄像机焦点位置
 * @param focus
 */
    inline void setFocus(const Vector3f &_focus) {
        focus = _focus;
    }

/**
 * 获取摄像机右方向向量
 * @return 摄像机右方向向量
 */
    inline Vector3f &getRightDirection() {
        return right_direction;
    }

/**
 * 设置摄像机右方向向量
 * @param rightDirection
 */
    inline void setRightDirection(const Vector3f &_right_direction) {
        right_direction = _right_direction;
    }

/**
 * 获取摄像机视角
 * @return 摄像机视角
 */
    inline float getFov() {
        return fov;
    }

/**
 * 设置摄像机视角
 * @param fov
 */
    inline void setFov(float fov) {
        fov = fov;
    }

/**
 * 获取摄像机视线方向向量
 * @return 摄像机视线方向向量
 */
    inline Vector3f getLookDirection() {
        return (focus - position).normalize();
    }

/**
 * 获取摄像机上方向向量
 * @return 摄像机上方向向量
 */
    inline Vector3f getUpDirection() {
        Vector3f look_direction = getLookDirection();
        return look_direction.cross(right_direction).normalize();
    }

/**
 * 获取摄像机与投影平面的距离
 * @return 摄像机与投影平面的距离
 */
    inline float getPlaneDistance() {
        return 1.f / (2.f * tanf(0.5f * fov * M_PI_F / 180.f));
    }

    inline void rotate(float dx, float dy) {
        if (dx == 0 && dy == 0) return;
        Vector3f up_direction = getUpDirection();
        Vector3f look_direction = getLookDirection();
        float theta = M_PI_F * sqrtf(dx * dx + dy * dy);
        Vector3f vec = up_direction * dy + right_direction * dx;

        Vector3f rotation_axis = vec.cross(-look_direction).normalize();
        if (rotation_axis.zero()) return;
        position = position.rotate(focus, rotation_axis, theta);
        right_direction = right_direction.rotate(rotation_axis, theta).normalize();
    }

    inline void scale(float s) {
        /**
         * 位置变换公式
         * position = position * (1 - s) + focus * s
         */
        position = focus + (position - focus) * s;
    }

    inline void translate(float dx, float dy) {
        Vector3f right = getLookDirection().cross(getUpDirection());
        Vector3f translation = ((right * dx) + (getUpDirection() * dy));
        focus += translation;
        position += translation;
    }

private:
    Vector3f position;          //位置
    Vector3f focus;             //焦点
    Vector3f right_direction;    //右方向
    float fov;                  //视角
};

#endif //CAMERA_H

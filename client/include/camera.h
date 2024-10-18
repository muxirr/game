#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "vector2.h"

class Camera
{
public:
    Camera() = default;
    ~Camera() = default;

    void set_size(const Vector2 &size)
    {
        this->size = size;
    }
    const Vector2 &get_size() const
    {
        return this->size;
    }

    void set_position(const Vector2 &position)
    {
        this->position = position;
    }

    const Vector2 &get_position() const
    {
        return position;
    }

    void look_at(const Vector2 &target)
    {
        this->position = target - this->size / 2.0f;
    }

private:
    Vector2 size;
    Vector2 position;
};

#endif

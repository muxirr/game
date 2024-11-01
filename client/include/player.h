#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "animation.h"

class Player
{
public:
    enum class Facing
    {
        UP,
        DOWN,
        LEFT,
        RIGHT
    };

public:
    Player(Atlas &atlas_idle_up, Atlas &atlas_idle_down, Atlas &atlas_idle_left, Atlas &atlas_idle_right, Atlas &atlas_run_up, Atlas &atlas_run_down, Atlas &atlas_run_left, Atlas &atlas_run_right)
    {
        this->anim_idle_up.set_loop(true);
        this->anim_idle_up.set_interval(0.1f);
        this->anim_idle_up.add_frame(atlas_idle_up);

        this->anim_idle_down.set_loop(true);
        this->anim_idle_down.set_interval(0.1f);
        this->anim_idle_down.add_frame(atlas_idle_down);

        this->anim_idle_left.set_loop(true);
        this->anim_idle_left.set_interval(0.1f);
        this->anim_idle_left.add_frame(atlas_idle_left);

        this->anim_idle_right.set_loop(true);
        this->anim_idle_right.set_interval(0.1f);
        this->anim_idle_right.add_frame(atlas_idle_right);

        this->anim_run_up.set_loop(true);
        this->anim_run_up.set_interval(0.1f);
        this->anim_run_up.add_frame(atlas_run_up);

        this->anim_run_down.set_loop(true);
        this->anim_run_down.set_interval(0.1f);
        this->anim_run_down.add_frame(atlas_run_down);

        this->anim_run_left.set_loop(true);
        this->anim_run_left.set_interval(0.1f);
        this->anim_run_left.add_frame(atlas_run_left);

        this->anim_run_right.set_loop(true);
        this->anim_run_right.set_interval(0.1f);
        this->anim_run_right.add_frame(atlas_run_right);
    }

    void on_update(float delta)
    {
        // update position
        if (!position.approx(pos_target))
        {
            velocity = (pos_target - position).normalize() * SPEED_RUN;
        }
        else
        {
            velocity = Vector2(0, 0);
        }
        if ((pos_target - position).length() <= (velocity * delta).length())
        {
            position = pos_target;
        }
        else
        {
            position += velocity * delta;
        }
        // update animation
        if (velocity.approx(Vector2(0, 0)))
        {
            switch (facing)
            {
            case Player::Facing::UP:
                current_anim = &anim_idle_up;
                break;
            case Player::Facing::DOWN:
                current_anim = &anim_idle_down;
                break;
            case Player::Facing::LEFT:
                current_anim = &anim_idle_left;
                break;
            case Player::Facing::RIGHT:
                current_anim = &anim_idle_right;
                break;
            }
        }
        else
        {
            if (abs(velocity.y) >= 0.000f)
            {
                facing = velocity.y > 0 ? Facing::DOWN : Facing::UP;
            }
            if (abs(velocity.x) >= 0.000f)
            {
                facing = velocity.x > 0 ? Facing::RIGHT : Facing::LEFT;
            }
            switch (facing)
            {
            case Player::Facing::UP:
                current_anim = &anim_idle_up;
                break;
            case Player::Facing::DOWN:
                current_anim = &anim_idle_down;
                break;
            case Player::Facing::LEFT:
                current_anim = &anim_idle_left;
                break;
            case Player::Facing::RIGHT:
                current_anim = &anim_idle_right;
                break;
            }
        }
        if (!current_anim)
        {
            return;
        }
        current_anim->set_position(position);
        current_anim->on_update(delta);
    }

    void on_render(const Camera& camera){
        if(!current_anim){
            return;
        }
        current_anim->on_render(camera);
    }

    void set_position(const Vector2 &position)
    {
        this->position = position;
    }

    const Vector2 &get_position() const
    {
        return position;
    }

    void set_target(const Vector2 &pos_target)
    {
        this->pos_target = pos_target;
    }

    ~Player() = default;

private:
    const float SPEED_RUN = 100.0f;

private:
    Vector2 position;
    Vector2 velocity;
    Vector2 pos_target;

    Animation anim_idle_up;
    Animation anim_idle_down;
    Animation anim_idle_left;
    Animation anim_idle_right;
    Animation anim_run_up;
    Animation anim_run_down;
    Animation anim_run_left;
    Animation anim_run_right;
    Animation *current_anim = nullptr;

    Facing facing = Facing::DOWN;
};

#endif
#ifndef _ANIMATION_H
#define _ANIMATION_H

#include "util.h"
#include "timer.h"
#include "atlas.h"
#include "vector2.h"

#include <vector>
#include <functional>

class Animation
{
public:
    Animation()
    {
        timer.set_one_shot(false);
        timer.set_time_out([&]()
                           { idx_frame++;
                            if(idx_frame >= frame_list.size()){
                                idx_frame = is_loop?0:frame_list.size()-1;
                                if(!is_loop && on_finished)
                                 on_finished();                              
                            } });
    }
    ~Animation() = default;

    void reset()
    {
        timer.restart();
        idx_frame = 0;
    }
    void set_position(const Vector2 &position)
    {
        this->position = position;
    }
    void set_loop(bool is_loop)
    {
        this->is_loop = is_loop;
    }
    void set_interval(float interval)
    {
        timer.set_wait_time(interval);
    }
    bool set_on_finished(std::function<void()> on_finished)
    {
        this->on_finished = on_finished;
    }
    void add_frame(IMAGE *img, int num_h)
    {
        int w = img->getwidth();
        int h = img->getheight();
        int w_frame = w / num_h;
        for (int i = 0; i < num_h; i++)
        {
            Rect rect_src;
            rect_src.x = i * w_frame;
            rect_src.y = 0;
            rect_src.w = w_frame;
            rect_src.h = h;
            frame_list.emplace_back(img, rect_src);
        }
    }
    void add_frame(Atlas &atlas)
    {
        for (int i = 0; i < atlas.get_size(); i++)
        {
            IMAGE *img = atlas.get_image(i);
            Rect rect_src;
            rect_src.x = 0;
            rect_src.y = 0;
            rect_src.w = img->getwidth();
            rect_src.h = img->getheight();
            frame_list.emplace_back(img, rect_src);
        }
    }

    void on_update(float delta)
    {
        timer.on_update(delta);
    }

    void on_render(const Camera &camera)
    {
        const Frame &frame = frame_list[idx_frame];

        Rect rect_dst;
        rect_dst.x = (int)position.x - frame.rect_src.w / 2;
        rect_dst.y = (int)position.y - frame.rect_src.h / 2;
        rect_dst.w = frame.rect_src.w;
        rect_dst.h = frame.rect_src.h;
        putimage_ex(camera, frame.img, &rect_dst, &frame.rect_src);
    }

private:
    struct Frame
    {
        Rect rect_src;
        IMAGE *img = nullptr;

        Frame() = default;
        Frame(IMAGE *img, const Rect &rect_src) : img(img), rect_src(rect_src)
        {
        }
        ~Frame() = default;
    };

private:
    Timer timer;
    Vector2 position;
    bool is_loop = false;
    size_t idx_frame = 0;
    std::vector<Frame> frame_list;
    std::function<void()> on_finished;
};

#endif
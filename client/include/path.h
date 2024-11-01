#ifndef _PATH_H_
#define _PATH_H_

#include "vector2.h"

#include <vector>

class Path
{
public:
    Path(const std::vector<Vector2> &point_list)
    {
        this->point_list = point_list;
        for (int i = 1; i < point_list.size(); i++)
        {
            float segment_len = (point_list[i] - point_list[i - 1]).length();
            this->segment_len_list.push_back(segment_len);
            total_length += segment_len;
        }
    }
    ~Path() = default;
    Vector2 get_postion_at_progress(float progress)
    {
        if (progress >= 1)
        {
            return point_list.back();
        }
        if (progress <= 0)
        {
            return point_list.front();
        }
        // find the segment that the progress is on
        float target_distance = total_length * progress;
        float accumulated_distance = 0;

        for (int i = 1; i < point_list.size(); i++)
        {
            accumulated_distance += segment_len_list[i - 1];
            if (accumulated_distance >= target_distance)
            {
                float segment_progress = (target_distance - (accumulated_distance - segment_len_list[i - 1])) / segment_len_list[i - 1];
                return point_list[i - 1] + (point_list[i] - point_list[i - 1]) * segment_progress;
            }
        }
        return point_list.back();
    }

private:
    float total_length = 0;
    std::vector<Vector2> point_list;
    std::vector<float> segment_len_list;
};

#endif
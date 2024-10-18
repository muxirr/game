#ifndef _ALTAS_H_
#define _ALTAS_H_
#include <vector>
#include <graphics.h>

class Atlas
{
public:
    Atlas() = default;
    ~Atlas() = default;

    void load(LPCTSTR path_template, int num)
    {
        this->img_list.clear();
        this->img_list.resize(num);

        TCHAR path[256];
        for (int i = 0; i < num; i++)
        {
            wsprintf(path, path_template, i + 1);
            loadimage(&img_list[i], path);
        }
    }

    void clear()
    {
        this->img_list.clear();
    }

    int get_size()
    {
        return this->img_list.size();
    }

    IMAGE *get_image(int index)
    {
        if (index < 0 || index >= img_list.size())
            return nullptr;
        return &this->img_list[index];
    }
    void add_image(const IMAGE &img)
    {
        this->img_list.push_back(img);
    }

private:
    std::vector<IMAGE> img_list;
};

#endif
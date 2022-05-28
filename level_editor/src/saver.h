#pragma once
#include <fstream>
#include <string>
#include <vector>
#include "state.h"

class loader
{
    std::vector<state> blocks;
    math::vec<double, 2> spawn;
    math::vec<double, 2> end;

public:
    loader() : blocks{}, spawn{}, end{content_dims - cube_size}
    {
    }

    bool load(std::string_view file_name)
    {
        blocks.clear();

        std::ifstream file(file_name.data(), std::fstream::binary);

        if (!file.read(reinterpret_cast<char *>(&spawn), sizeof(spawn)) ||
            !file.read(reinterpret_cast<char *>(&end), sizeof(end)))
        {
            return false;
        }

        state cur;
        while (file.read(reinterpret_cast<char *>(&cur.file_obj), cur.file_obj.block_size()))
            blocks.push_back(cur);

        return true;
    }

    std::vector<state> &&get_objs()
    {
        return std::move(blocks);
    }

    auto get_spawn() const
    {
        return spawn;
    }

    auto get_end() const
    {
        return end;
    }
};

class save
{
    std::string name;

public:
    save(std::string_view initial_name) : name{initial_name}
    {
        if (name.find(".lvl") == std::string::npos)
            name += ".lvl";
    }

    void mark(math::dvec2 spawn, math::dvec2 end, const std::vector<state> &objs)
    {
        std::vector<game::file_obj> out;
        out.reserve(objs.size());

        std::ofstream file(name, std::fstream::binary | std::fstream::trunc);

        file.write(reinterpret_cast<char *>(&spawn), sizeof(spawn));
        file.write(reinterpret_cast<char *>(&end), sizeof(end));

        for (const auto &obj : objs)
            file.write(reinterpret_cast<const char *>(&obj.file_obj), obj.file_obj.block_size());
    }
};
#pragma once
#include <fstream>
#include <string>
#include <vector>
#include "vec.h"
#include "defs.h"

namespace game
{
    // file layout
    // 8 BYTES (spawn.x), 8 BYTES (spawn.y),
    // 8 BYTES (end.x), 8 BYTES (end.y),
    // blocks...

    // block layout
    // 8 BYTES (min.x), 8 BYTES (min.y), BYTE (block_type), BYTE (block_color), BYTE (direction)

    struct file_obj
    {
        static_assert(sizeof(double) == 8, "double must be 8 bytes to be compatible with levels");

        math::vec<double, 2> min;
        block_color col;
        block_type type;
        direction facing;

        static constexpr size_t block_size()
        {
            return sizeof(math::vec<double, 2>) + sizeof(block_color) + sizeof(block_type) + sizeof(direction);
        }
    };

    class loader
    {
        std::vector<file_obj> blocks;
        math::vec<double, 2> spawn;
        math::vec<double, 2> end;

    public:
        loader() : blocks{}, spawn{}, end{}
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

            file_obj cur;
            while (file.read(reinterpret_cast<char *>(&cur), cur.block_size()))
                blocks.push_back(cur);
            
            return true;
        }

        const std::vector<file_obj> &get_objs() const
        {
            return blocks;
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
}
#pragma once

#include <vector>

#define TILE_SIZE 48

namespace TD
{
struct Tile
{
    enum class Direction
    {
        None,
        Up,
        Down,
        Left,
        Right
    };

    int terrain      = 0;
    int decoration   = 0;
    int special_flag = -1;
    bool has_tower   = false;

    Direction direction = Direction::None;
};

using TileMap = std::vector<std::vector<Tile>>;

} // namespace TD
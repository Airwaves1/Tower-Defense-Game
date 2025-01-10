#pragma once

#include "tile.hpp"
#include <SDL.h>
#include <vector>

namespace TD
{
class Route
{
  public:
    using IdxList = std::vector<SDL_Point>;

  public:
    Route() = default;
    Route(const TileMap &tile_map, const SDL_Point &idx_origin)
    {
        size_t width_map  = tile_map[0].size();
        size_t height_map = tile_map.size();

        SDL_Point idx_next = idx_origin;

        while (true)
        {
            // 检查是否越界
            if (idx_next.x > width_map || idx_next.y >= height_map) break;

            // 避免重复添加导致死循环
            if (check_duplicate_idx(idx_next))
                break;
            else
                m_idx_list.push_back(idx_next);

            bool is_next_dir_exist = true;
            const Tile &tile       = tile_map[idx_next.y][idx_next.x];

            // 如果到达特殊标记点房屋则退出
            if (tile.special_flag == 0) break;

            // 根据方向移动
            switch (tile.direction)
            {
                case Tile::Direction::Up:
                    idx_next.y--;
                    break;
                case Tile::Direction::Down:
                    idx_next.y++;
                    break;
                case Tile::Direction::Left:
                    idx_next.x--;
                    break;
                case Tile::Direction::Right:
                    idx_next.x++;
                    break;
                default:
                    is_next_dir_exist = false;
                    break;
            }

            // 如果下一个方向不存在则退出
            if (!is_next_dir_exist) break;
        }
    }

    const IdxList &get_idx_list() const { return m_idx_list; }

  private:
    bool check_duplicate_idx(const SDL_Point &idx)
    {
        for (const auto &idx_tmp : m_idx_list)
        {
            if (idx_tmp.x == idx.x && idx_tmp.y == idx.y) return true;
        }
        return false;
    }

  private:
    IdxList m_idx_list;
};
} // namespace TD

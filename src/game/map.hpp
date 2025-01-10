#pragma once

#include "tile.hpp"
#include <SDL.h>
#include <fstream>
#include <sstream>
#include <unordered_map>

#include "route.hpp"

namespace TD
{
class Map
{
  public:
    using SpawnerRoutePool = std::unordered_map<int, Route>;

  public:
    Map()  = default;
    ~Map() = default;

    bool load(const std::string &path)
    {
        std::ifstream file(path);
        if (!file.good()) return false;

        TileMap tile_map_tmp;
        int idx_x = -1;
        int idx_y = -1;

        std::string str_line;
        while (std::getline(file, str_line))
        {
            str_line = trim_str(str_line);
            if (str_line.empty()) continue;

            idx_x = -1, idx_y++;
            tile_map_tmp.emplace_back();

            std::string str_tile;
            std::istringstream iss(str_line);
            while (std::getline(iss, str_tile, ','))
            {
                idx_x++;
                tile_map_tmp[idx_y].emplace_back();
                Tile &tile = tile_map_tmp[idx_y].back();
                load_tile_from_string(tile, str_tile);
            }
        }

        file.close();
        if (tile_map_tmp.empty() || tile_map_tmp[0].empty()) return false;

        m_tile_map = tile_map_tmp;

        generate_map_cache();

        return true;
    }

    size_t get_width() const
    {
        if (m_tile_map.empty()) return 0;
        return m_tile_map[0].size();
    }

    size_t get_height() const { return m_tile_map.size(); }

    const TileMap &get_tile_map() const { return m_tile_map; }

    const SDL_Point &get_home_idx() const { return idx_honme; }

    const SpawnerRoutePool &get_spawner_route_pool() const { return m_spawner_route_pool; }

    void place_tower(const SDL_Point &idx, bool has_tower)
    {
        if (idx.x < 0 || idx.y < 0 || idx.x >= get_width() || idx.y >= get_height()) return;

        m_tile_map[idx.y][idx.x].has_tower = has_tower;
    }

  private:
    // 截去字符串两端的空格和制表符
    std::string trim_str(const std::string &str)
    {
        size_t begin_idx = str.find_first_not_of(" \t");
        if (begin_idx == std::string::npos) return "";

        size_t end_idx = str.find_last_not_of(" \t");
        return str.substr(begin_idx, end_idx - begin_idx + 1);
    }

    // 从字符串中加载地图块
    void load_tile_from_string(Tile &tile, const std::string &str)
    {
        std::string str_tidy = trim_str(str);

        std::string str_value;
        std::vector<int> values;
        std::stringstream iss(str_tidy);

        while (std::getline(iss, str_value, '\\'))
        {
            int value;
            try
            {
                value = std::stoi(str_value);
            }
            catch (const std::invalid_argument &e)
            {
                value = -1;
            }

            values.push_back(value);
        }

        tile.terrain    = (values.size() < 1 || values[0] < 0) ? 0 : values[0];
        tile.decoration = (values.size() < 2) ? -1 : values[1];
        tile.direction  = (Tile::Direction)((values.size() < 3 || values[2] < 0) ? 0 : values[2]);
        tile.has_tower  = (values.size() <= 3) ? -1 : values[3];
    }

    void generate_map_cache()
    {
        for (int y = 0; y < get_height(); y++)
        {
            for (int x = 0; x < get_width(); x++)
            {
                const Tile &tile = m_tile_map[y][x];
                if (tile.special_flag < 0) continue;

                if (tile.special_flag == 0)
                    idx_honme = {x, y};
                else // 刷怪点
                {
                    m_spawner_route_pool[tile.special_flag] = Route(m_tile_map, {x, y});
                }
            }
        }
    }

  private:
    TileMap m_tile_map;
    SDL_Point idx_honme = {0};
    SpawnerRoutePool m_spawner_route_pool; // 生成怪物的路径池
};
} // namespace TD
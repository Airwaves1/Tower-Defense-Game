#pragma once

#include "manager.hpp"

#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <fstream>
#include <json.hpp>
#include <iostream>

#include "game/map.hpp"
#include "game/wave.hpp"

namespace TD
{
class ConfigManager : public Manager<ConfigManager>
{
    friend class Manager<ConfigManager>;

  public:
    struct BasicTemplate
    {
        uint16_t window_width    = 1200;
        uint16_t window_height   = 800;
        std::string window_title = "Tower Defense";
    };

    struct PlayerTemplate
    {
        double speed                  = 3;
        double normal_attack_interval = 0.5;
        double normal_attack_damage   = 0;
        double skill_interval         = 10;
        double skill_damage           = 1;
    };

    struct TowerTemplate
    {
        double interval[10]    = {1};
        double damage[10]      = {25};
        double view_range[10]  = {5};
        double cost[10]        = {50};
        double upgrade_cost[9] = {75};
    };

    struct EnemyTemplate
    {
        double hp                = 100;
        double speed             = 1;
        double damage            = 1;
        double reward_ratio      = 0.5;
        double recover_interval  = 10;
        double recover_range     = 0;
        double recover_intensity = 0;
    };

  public:
    Map map;
    std::vector<Wave> wave_list;

    int level_archer = 0;
    int level_axeman = 0;
    int level_gunner = 0;

    bool is_game_win  = false;
    bool is_game_over = false;

    SDL_Rect rect_tile_map = {0, 0, 0, 0};

    BasicTemplate basic_template;

    PlayerTemplate player_template;

    TowerTemplate tower_archer_template;
    TowerTemplate tower_axeman_template;
    TowerTemplate tower_gunner_template;

    EnemyTemplate enemy_slim_template;
    EnemyTemplate enemy_king_slim_template;
    EnemyTemplate enemy_skeleton_template;
    EnemyTemplate enemy_goblin_template;
    EnemyTemplate enemy_goblin_priest_template;

    const double num_initial_hp    = 10;
    const double num_initial_coin  = 100;
    const double num_coin_per_prop = 10;

  public:
    bool load_level_config(const std::string &path)
    {
        std::ifstream file(path);
        if (!file.good())
        {
            std::cerr << "Failed to open file " << path << std::endl;
            return false;
        }

        std::stringstream str_stream;
        str_stream << file.rdbuf();
        file.close();

        nlohmann::json json_data = nlohmann::json::parse(str_stream.str());
        if (json_data.is_null())
        {
            std::cerr << "Failed to parse json data from " << path << std::endl;
            return false;
        }

        for (const auto &json_wave : json_data)
        {
            wave_list.emplace_back();
            Wave &wave = wave_list.back();

            // 解析rewards和interval
            wave.reward   = json_wave["rewards"].get<double>();
            wave.interval = json_wave["interval"].get<double>();

            // spawn_list
            for (const auto &json_spawn : json_wave["spawn_list"])
            {
                wave.spawn_list.emplace_back();
                Wave::SpawnEvent &spawn_event = wave.spawn_list.back();

                spawn_event.interval    = json_spawn["interval"].get<double>();
                spawn_event.spawn_point = json_spawn["spawn_point"].get<int>();

                // 验证枚举类型
                std::string enemy_type = json_spawn["enemy_type"].get<std::string>();
                if (enemy_type == "Slim")
                    spawn_event.enemy_type = EnemyType::Slim;
                else if (enemy_type == "KingSlim")
                    spawn_event.enemy_type = EnemyType::KingSlim;
                else if (enemy_type == "Skeleton")
                    spawn_event.enemy_type = EnemyType::Skeleton;
                else if (enemy_type == "Goblin")
                    spawn_event.enemy_type = EnemyType::Goblin;
                else if (enemy_type == "GoblinPriest")
                    spawn_event.enemy_type = EnemyType::GoblinPriest;
                else
                    spawn_event.enemy_type = EnemyType::Slim;
            }

            if (wave.spawn_list.empty())
            {
                std::cerr << "Failed to parse spawn_list from " << path << std::endl;
                wave_list.pop_back(); // 回滚
                return false;
            }
        }

        if (wave_list.empty())
        {
            std::cerr << "Failed to parse wave_list from " << path << std::endl;
            return false;
        }

        return true;
    }

    bool load_game_config(const std::string &path)
    {
        std::ifstream file(path);

        if (!file.good())
        {
            std::cerr << "Failed to open file " << path << std::endl;
            return false;
        }

        std::stringstream str_stream;
        str_stream << file.rdbuf();
        file.close();

        nlohmann::json json_data = nlohmann::json::parse(str_stream.str());
        if (json_data.is_null())
        {
            std::cerr << "Failed to parse json data from " << path << std::endl;
            return false;
        }


        // 解析基础配置 ------------start
        if(!json_data.contains("basic"))
        {
            std::cerr << "Failed to parse basic from " << path << std::endl;
            return false;
        }
        const auto &basic = json_data["basic"];
        basic_template.window_width  = basic.value("window_width", 1200);
        basic_template.window_height = basic.value("window_height", 800);
        basic_template.window_title  = basic.value("window_title", std::string("Tower Defense"));
        // 解析基础配置 ------------end


        // 解析玩家配置 ------------start
        if(!json_data.contains("player"))
        {
            std::cerr << "Failed to parse player from " << path << std::endl;
            return false;
        }
        const auto &player = json_data["player"];
        player_template.speed                  = player.value("speed", 3.0);
        player_template.normal_attack_interval = player.value("normal_attack_interval", 0.5);
        player_template.normal_attack_damage   = player.value("normal_attack_damage", 0.0);
        player_template.skill_interval         = player.value("skill_interval", 10.0);
        player_template.skill_damage           = player.value("skill_damage", 1.0);
        // 解析玩家配置 ------------end


        // 解析塔配置 ------------start
        if(!json_data.contains("tower"))
        {
            std::cerr << "Failed to parse tower from " << path << std::endl;
            return false;
        }
        auto parse_tower = [](const nlohmann::json &json_tower, TowerTemplate &tower)
        {
            auto interval_vec = json_tower.value("interval", std::vector<double>(10, 1));
            std::copy(interval_vec.begin(), interval_vec.end(), tower.interval);

            auto damage_vec = json_tower.value("damage", std::vector<double>(10, 25));
            std::copy(damage_vec.begin(), damage_vec.end(), tower.damage);

            auto view_range_vec = json_tower.value("view_range", std::vector<double>(10, 5));
            std::copy(view_range_vec.begin(), view_range_vec.end(), tower.view_range);

            auto cost_vec = json_tower.value("cost", std::vector<double>(10, 50));
            std::copy(cost_vec.begin(), cost_vec.end(), tower.cost);

            auto upgrade_cost_vec = json_tower.value("upgrade_cost", std::vector<double>(9, 75));
            std::copy(upgrade_cost_vec.begin(), upgrade_cost_vec.end(), tower.upgrade_cost);
        };

        const auto &tower = json_data["tower"];
        if(!tower.contains("archer") || !tower.contains("axeman") || !tower.contains("gunner"))
        {
            std::cerr << "Failed to parse tower from " << path << std::endl;
            return false;
        }
        parse_tower(tower["archer"], tower_archer_template);
        parse_tower(tower["axeman"], tower_axeman_template);
        parse_tower(tower["gunner"], tower_gunner_template);
        // 解析塔配置 ------------end


        // 解析敌人配置 ------------start
        if(!json_data.contains("enemy"))
        {
            std::cerr << "Failed to parse enemy from " << path << std::endl;
            return false;
        }
        auto parse_enemy = [](const nlohmann::json &json_enemy, EnemyTemplate &enemy)
        {
            enemy.hp                = json_enemy.value("hp", 100.0);
            enemy.speed             = json_enemy.value("speed", 1.0);
            enemy.damage            = json_enemy.value("damage", 1.0);
            enemy.reward_ratio      = json_enemy.value("reward_ratio", 0.5);
            enemy.recover_interval  = json_enemy.value("recover_interval", 10.0);
            enemy.recover_range     = json_enemy.value("recover_range", 0.0);
            enemy.recover_intensity = json_enemy.value("recover_intensity", 0.0);
        };

        const auto &enemy = json_data["enemy"];
        if(!enemy.contains("slim") || !enemy.contains("king_slim") || !enemy.contains("skeleton") || !enemy.contains("goblin") || !enemy.contains("goblin_priest"))
        {
            std::cerr << "Failed to parse enemy from " << path << std::endl;
            return false;
        }
        parse_enemy(enemy["slim"], enemy_slim_template);
        parse_enemy(enemy["king_slim"], enemy_king_slim_template);
        parse_enemy(enemy["skeleton"], enemy_skeleton_template);
        parse_enemy(enemy["goblin"], enemy_goblin_template);
        parse_enemy(enemy["goblin_priest"], enemy_goblin_priest_template);
        // 解析敌人配置 ------------end

        return true;

    }

   

  protected:
    ConfigManager()          = default;
    virtual ~ConfigManager() = default;

  private:
};
} // namespace TD

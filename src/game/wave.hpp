#pragma once

#include "game/enemy/enemy_type.hpp"
#include <vector>

namespace TD
{

struct Wave
{
    struct SpawnEvent
    {
        double interval      = 0;
        int spawn_point      = 1;
        EnemyType enemy_type = EnemyType::Slim;
    };
    
    double reward   = 0;
    double interval = 0;

    std::vector<SpawnEvent> spawn_list;
};

} // namespace TD

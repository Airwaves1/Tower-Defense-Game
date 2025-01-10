#include "utils/commn.hpp"
#include "manager/game_manager.hpp"

int main()
{
    TD::GameManager::get_instance().init({1200, 800, "Tower Defense"});
    return TD::GameManager::get_instance().run();
}

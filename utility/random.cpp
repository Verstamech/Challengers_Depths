#include "random.h"
#include <random>

int randint(int min, int max) {
    static std::random_device rd;
    static std::default_random_engine ran{rd()};
    return std::uniform_int_distribution<int> {min, max}(ran);
};

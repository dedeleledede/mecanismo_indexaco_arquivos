#pragma once

#include <string>
#include "index.h"

class Serializer {
public:
    static bool save(const Index& index, const std::string& filename);

    static bool load(Index& index, const std::string& filename);
};

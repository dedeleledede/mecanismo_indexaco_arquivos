#pragma once

#include <string>
#include "index.h"
#include "text_processor.h"

class Indexer {
private:
    Index& index;
    TextProcessor& text_processor;

    void process_file(const std::string& filepath);

public:
    Indexer(Index& idx, TextProcessor& tp) : index(idx), text_processor(tp) {}

    void build(const std::string& root_dir);
};
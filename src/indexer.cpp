#include "indexer.h"
#include <fstream>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

void Indexer::process_file(const string& filepath) {
    ifstream file(filepath);
    if (!file.is_open()) {
        return;
    }

    string content( (istreambuf_iterator<char>(file)), istreambuf_iterator<char>() );

    auto words = text_processor.process(content);

    for (const auto& word : words) {
        index.add_word(word, filepath);
    }
}

void Indexer::build(const string& root_dir) {
    for (const auto& entry : fs::recursive_directory_iterator(root_dir)) {
        if (entry.is_regular_file()) {
            string path = entry.path().string();
            process_file(path);
        }
    }
}
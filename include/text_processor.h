#pragma once
#include <string>
#include <vector>
#include <unordered_set>

class TextProcessor {
private:
    std::unordered_set<std::string> stopwords;

    std::string normalize(const std::string& text) const;
    std::vector<std::string> split_words(const std::string& normalized) const;
    bool is_stopword(const std::string& word) const;

public:
    TextProcessor() = default;

    void load_stopwords(const std::string& filepath);
    std::vector<std::string> process(const std::string& text) const;
};

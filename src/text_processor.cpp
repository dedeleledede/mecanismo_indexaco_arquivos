#include "text_processor.h"

#include <fstream>
#include <cctype>

using namespace std;

string TextProcessor::normalize(const string& text) const{
    string resultado = {};
    for (int i = 0; i < text.length(); i++){
        unsigned char c = tolower(text[i]);

        if (isalnum(c)){
            resultado.push_back(c);
        }

        else{
            resultado.push_back(' ');
        }
    }

    return resultado;
}

bool TextProcessor::is_stopword(const string& word) const{
    return stopwords.find(word) != stopwords.end();
}

vector<string> TextProcessor::split_words(const string& normalized) const{
    vector<string> words = {};
    string current = {};

    for (int i = 0; i < normalized.length(); i++){
        unsigned char c = normalized[i];
        if (isspace(c)){
            if (!current.empty()){
                words.push_back(current);
                current.clear();
            }
        }
        else{
            current.push_back(c);
        }
    }

    if (!current.empty())
        words.push_back(current);

    return words;
}

void TextProcessor::load_stopwords(const string& filepath){
    ifstream file(filepath);
    if (!file.is_open()) return;

    string word;
    while (file >> word) {
        stopwords.insert(word);
    }
}

vector<string> TextProcessor::process(const string& text) const{
    vector<string> result = {};
    string normalized = normalize(text);
    auto splitted = split_words(normalized);
    
    for (const auto& word : splitted){
        if (!is_stopword(word))
            result.push_back(word);
    }
    return result;
}
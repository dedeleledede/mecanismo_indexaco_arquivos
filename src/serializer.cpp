#include "serializer.h"

#include <fstream>
#include <unordered_map>
#include <vector>
#include <algorithm>

using namespace std;

bool Serializer::save(const Index& index, const string& filename) {
    ofstream out(filename);
    if (!out.is_open()) {
        return false;
    }

    const auto& id_to_filepath = index.get_id_to_filepath();
    const auto& inverted = index.get_inverted_index();

    // documentos
    out << id_to_filepath.size() << '\n';

    // escrever os docs em ordem de id
    vector<pair<int, string>> docs(id_to_filepath.begin(), id_to_filepath.end());
    sort(docs.begin(), docs.end(), [](const auto& a, const auto& b) { return a.first < b.first; });

    for (const auto& p : docs) {
        out << p.first << ' ' << p.second << '\n';
    }

    // ondice invertido
    out << inverted.size() << '\n';

    for (const auto& entry : inverted) {
        const string& word = entry.first;
        const Index::doc_set_id& docset = entry.second;

        out << word << ' ' << docset.size();
        for (int id : docset) {
            out << ' ' << id;
        }
        out << '\n';
    }

    return true;
}

bool Serializer::load(Index& index, const string& filename) {
    ifstream in(filename);
    if (!in.is_open()) {
        return false;
    }

    size_t doc_count;
    if (!(in >> doc_count)) {
        return false;
    }

    unordered_map<string, int> filepath_to_id;
    unordered_map<int, string> id_to_filepath;

    // le documentos
    for (size_t i = 0; i < doc_count; ++i) {
        int id;
        string path;
        if (!(in >> id >> path)) {
            return false;
        }
        filepath_to_id[path] = id;
        id_to_filepath[id] = path;
    }

    size_t term_count;
    if (!(in >> term_count)) {
        return false;
    }

    unordered_map<string, Index::doc_set_id> inverted;

    // le linhas de termos
    for (size_t i = 0; i < term_count; ++i) {
        string word;
        size_t qty;
        if (!(in >> word >> qty)) {
            return false;
        }

        Index::doc_set_id docs;
        for (size_t j = 0; j < qty; ++j) {
            int id;
            if (!(in >> id)) {
                return false;
            }
            docs.insert(id);
        }

        inverted[word] = std::move(docs);
    }

    index.set_doc_maps(filepath_to_id, id_to_filepath);
    index.set_inverted_index(inverted);

    return true;
}
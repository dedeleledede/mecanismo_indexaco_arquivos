#include "index.h"

using namespace std;

int Index::create_doc(const std::string& filepath) {
  auto it = path_to_id.find(filepath);

  if (it != path_to_id.end())
    return it->second; //ja existe

  //criando (mesmo tamanho)
  int new_id = static_cast<int>(path_to_id.size());
  
  // att id to path e path to id
  path_to_id[filepath] = new_id;
  id_to_path[new_id] = filepath; 
  
  return new_id; //retorna o novo id
}

void Index::add_word(const std::string& word, const std::string& filepath) {
  int doc_id = create_doc(filepath);
  inverted_index_map[word].insert(doc_id);
}

// getters

string Index::get_filepath(int doc_id) const {
  auto it = id_to_path.find(doc_id);
  if (it != id_to_path.end())
    return it->second;

  return string{}; //return empty string, chamador checar
}

const Index::doc_set_id& Index::get_docs_from_word(const std::string& word) const{
  auto it = inverted_index_map.find(word);

  if (it != inverted_index_map.end())
    return it->second;

  static const doc_set_id empty_set;
  return empty_set;
}

const unordered_map<string, Index::doc_set_id>& Index::get_inverted_index() const {
  return inverted_index_map;
}

const unordered_map<string, int>& Index::get_filepath_to_id() const {
  return path_to_id;
}

const unordered_map<int, string>& Index::get_id_to_filepath() const {
  return id_to_path;
}

// setters

void Index::set_inverted_index(const unordered_map<string, Index::doc_set_id>& mapa) {
  inverted_index_map = mapa;
}

void Index::set_doc_maps(const unordered_map<string, int>& mapa_filepath_id, const unordered_map<int, string>& mapa_id_filepath) {
  path_to_id = mapa_filepath_id;
  id_to_path = mapa_id_filepath;
}
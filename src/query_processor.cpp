#include "query_processor.h"
#include <algorithm>

using namespace std;

QueryProcessor::QueryProcessor(const Index& idx) : index(idx) {}

Index::doc_set_id QueryProcessor::process_terms(const vector<string>& terms) const {
    Index::doc_set_id result;

    if (terms.empty())
        return result;

    bool first = true;

    for (const auto& term: terms){
        const auto& docs = index.get_docs_from_word(term);
        if (first){
            result = docs;
            first = false;
        }
        else{
            Index::doc_set_id temp;
            // calcula a intersecao entre result e docs e coloca em temp
            set_intersection(result.begin(), result.end(), docs.begin(), docs.end(), std::inserter(temp, temp.begin()));
            result = temp; 
        }

        if (result.empty())
            break;
    }
    return result;
}
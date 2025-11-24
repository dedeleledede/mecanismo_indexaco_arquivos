#include "query_processor.h"

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
            
            auto it1 = result.begin();
            auto end1 = result.end();
            auto it2 = docs.begin();
            auto end2 = result.end();

            while (it1 != end1 && it2 != end2){
                if (*it1 < *it2){
                    it1++;
                }
                else if (*it2 < *it1){
                    it2++;
                }
                else{
                    temp.insert(*it1);
                    it1++;
                    it2++;
                }
            }
            result = temp;
        }

        if (result.empty())
            break;
    }
    return result;
}
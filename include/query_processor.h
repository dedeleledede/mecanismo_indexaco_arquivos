#pragma once

#include <vector>
#include <string>
#include "index.h"

class QueryProcessor {
private:
    const Index& index;

public:
    QueryProcessor(const Index& idx);

    Index::doc_set_id process_terms(const std::vector<std::string>& terms) const;
};

#pragma once
#include "compiler.hpp"
#include <vector>

class Parser {
public:
    bool parse(std::vector<Stmt*>& program);
};

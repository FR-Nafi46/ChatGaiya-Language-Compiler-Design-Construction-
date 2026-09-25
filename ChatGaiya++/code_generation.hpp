#pragma once
#include "compiler.hpp"
#include <string>
#include <vector>

class CodeGenerator {
public:
    std::string generate(const std::vector<Stmt*>& program) const;
private:
    mutable int indent_ = 0;
    mutable bool usesIndexing_ = false;
    std::string expression(const Expr* expr) const;
    void statements(const std::vector<Stmt*>& list, std::vector<std::string>& out) const;
    void emit(const std::string& line, std::vector<std::string>& out) const;
};

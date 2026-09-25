#pragma once
#include "compiler.hpp"
#include "symbol_table.hpp"
#include <vector>

class SemanticAnalyzer {
public:
    bool analyze(const std::vector<Stmt*>& program);
    const SymbolTable& symbols() const;
private:
    SymbolTable symbols_;
    int loopDepth_ = 0;
    ValueType analyzeExpr(Expr* expr);
    void analyzeStatements(const std::vector<Stmt*>& statements);
    bool assignable(ValueType target, ValueType source) const;
};

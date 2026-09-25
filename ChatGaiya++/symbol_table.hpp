#pragma once
#include "compiler.hpp"
#include <string>
#include <unordered_map>

class SymbolTable {
public:
    bool declare(const std::string& name, ValueType type);
    bool contains(const std::string& name) const;
    ValueType lookup(const std::string& name) const;
    const std::unordered_map<std::string, ValueType>& entries() const;
private:
    std::unordered_map<std::string, ValueType> symbols_;
};

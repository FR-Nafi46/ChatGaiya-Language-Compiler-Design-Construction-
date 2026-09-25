#include "symbol_table.hpp"

bool SymbolTable::declare(const std::string& name, ValueType type) {
    return symbols_.emplace(name, type).second;
}

bool SymbolTable::contains(const std::string& name) const {
    return symbols_.find(name) != symbols_.end();
}

ValueType SymbolTable::lookup(const std::string& name) const {
    const auto it = symbols_.find(name);
    return it == symbols_.end() ? ValueType::UNKNOWN : it->second;
}

const std::unordered_map<std::string, ValueType>& SymbolTable::entries() const {
    return symbols_;
}

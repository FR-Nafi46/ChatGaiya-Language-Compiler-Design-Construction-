#include "error_reporting.hpp"
#include <iostream>

ErrorReporter g_errors;

void ErrorReporter::add(int line, int column, const std::string& message) {
    diagnostics_.push_back({line, column, message});
}

bool ErrorReporter::hasErrors() const { return !diagnostics_.empty(); }

void ErrorReporter::print(const std::string& file) const {
    for (const auto& d : diagnostics_) {
        std::cerr << file << ':' << d.line << ':' << d.column << ": error: " << d.message << '\n';
    }
}

void ErrorReporter::clear() { diagnostics_.clear(); }

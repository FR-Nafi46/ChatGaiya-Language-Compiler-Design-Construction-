#pragma once
#include <string>
#include <vector>

struct Diagnostic {
    int line;
    int column;
    std::string message;
};

class ErrorReporter {
public:
    void add(int line, int column, const std::string& message);
    bool hasErrors() const;
    void print(const std::string& file) const;
    void clear();
private:
    std::vector<Diagnostic> diagnostics_;
};

extern ErrorReporter g_errors;

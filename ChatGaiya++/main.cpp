#include "code_generation.hpp"
#include "error_reporting.hpp"
#include "parser.hpp"
#include "semantic_analysis.hpp"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

extern FILE* yyin;
extern void yyrestart(FILE* input_file);

int main(int argc, char** argv) {
    if (argc != 2 && argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <source.cg> [-o output.py]\n";
        return 2;
    }
    const std::string input = argv[1];
    std::string output;
    if (argc == 4) {
        if (std::string(argv[2]) != "-o") {
            std::cerr << "Expected -o before output file\n";
            return 2;
        }
        output = argv[3];
    } else {
        output = input;
        const std::size_t slash = output.find_last_of("/\\");
        const std::size_t dot = output.find_last_of('.');
        if (dot != std::string::npos && (slash == std::string::npos || dot > slash)) output.resize(dot);
        output += ".py";
    }

    FILE* source = std::fopen(input.c_str(), "rb");
    if (!source) {
        std::cerr << "Cannot open source file: " << input << '\n';
        return 1;
    }
    yyin = source;
    yyrestart(source);
    std::vector<Stmt*> program;
    Parser parser;
    const bool parsed = parser.parse(program);
    std::fclose(source);
    if (!parsed || g_errors.hasErrors()) {
        g_errors.print(input);
        std::cerr << "Compilation failed; no output file generated.\n";
        return 1;
    }

    SemanticAnalyzer analyzer;
    if (!analyzer.analyze(program)) {
        g_errors.print(input);
        std::cerr << "Compilation failed; no output file generated.\n";
        return 1;
    }
    CodeGenerator generator;
    const std::string python = generator.generate(program);
    std::ofstream file(output, std::ios::binary);
    if (!file) {
        std::cerr << "Cannot write output file: " << output << '\n';
        return 1;
    }
    file << python;
    if (!file) {
        std::cerr << "Failed while writing output file: " << output << '\n';
        return 1;
    }
    std::cout << "Compilation successful: " << output << '\n';
    return 0;
}

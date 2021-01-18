//
// Created by 李宇轩 on 2018-12-10.
//

#ifndef PARSER_TYPEDEFS_H
#define PARSER_TYPEDEFS_H

class Symbol;

using Sentence = std::vector<Symbol>;
using Production = std::pair<Sentence, Sentence>;
using ProductionSet = std::unordered_set<Production>;
using ProductionMap = std::unordered_multimap<Sentence, Sentence>;
using Table = std::unordered_map<std::pair<Symbol, Symbol>, Production>;

#endif //PARSER_TYPEDEFS_H

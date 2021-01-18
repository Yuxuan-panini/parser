//
// Created by 李宇轩 on 2018-12-06.
//

#ifndef PARSER_LL1_H
#define PARSER_LL1_H

#include <stack>
#include "Symbol.h"
#include "TypeDefs.h"

class LL1
{
public:
	static std::unordered_map<Symbol, std::unordered_set<Symbol>> getFirsts(const std::unordered_set<Symbol> &T,
			const std::unordered_set<Symbol> &N, const ProductionMap &P);
	static std::unordered_map<Symbol, std::unordered_set<Symbol>> getFollows(const std::unordered_set<Symbol> &T,
			const std::unordered_set<Symbol> &N, const ProductionMap &P, const Symbol &S,
			const std::unordered_map<Symbol, std::unordered_set<Symbol>> &FIRST);
	static Table buildTable(const std::unordered_set<Symbol> &T, const std::unordered_set<Symbol> &N,
			const ProductionSet &P, const Symbol &S, const std::unordered_map<Symbol, std::unordered_set<Symbol>> &FIRST,
			const std::unordered_map<Symbol, std::unordered_set<Symbol>> &FOLLOW);
	static std::vector<Production> parse(const Symbol &S, std::vector<Symbol> in, Table table);
};

#endif //PARSER_LL1_H

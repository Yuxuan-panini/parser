//
// Created by 李宇轩 on 2018-12-05.
//

#ifndef PARSER_SYMBOL_H
#define PARSER_SYMBOL_H

#include <string>
#include <vector>
#include <numeric>
#include <unordered_map>
#include <unordered_set>
#include "TypeDefs.h"

class Symbol
{
	friend bool operator==(const Symbol &lhs, const Symbol &rhs);
public:
	enum class SymbolType { Terminal, NonTerminal, Epsilon, Dollar };

	Symbol();
	Symbol(const char &c, SymbolType type);
	Symbol(const std::string &label, SymbolType type);
	Symbol(const Symbol &s) = default;
	Symbol(Symbol &&s) noexcept;
	Symbol &operator=(const Symbol &rhs) = default;
	Symbol &operator=(Symbol &&rhs) noexcept;
	inline bool isTerminal() const { return type == SymbolType::Terminal; }
	inline bool isNonTerminal() const { return type == SymbolType::NonTerminal; }
	inline bool isEpsilon() const { return type == SymbolType::Epsilon; }
	inline bool isDollar() const { return type == SymbolType::Dollar; }

	static Symbol &Epsilon();
	static Symbol &Dollar();

	std::string label;
	SymbolType type;

private:
	static Symbol *epsilon;
	static Symbol *dollar;
};

bool operator==(const Symbol &lhs, const Symbol &rhs);
bool operator!=(const Symbol &lhs, const Symbol &rhs);
bool operator==(const Sentence &lhs, const Sentence &rhs);
bool operator==(const Production &lhs, const Production &rhs);
bool operator==(const std::pair<Symbol, Symbol> &lhs, const std::pair<Symbol, Symbol> &rhs);

namespace std
{
	template<>
	struct hash<Symbol>
	{
		typedef size_t result_type;
		typedef Symbol argument_type;

		size_t operator()(const Symbol &s) const;
	};

	template<>
	struct hash<Sentence>
	{
		typedef size_t result_type;
		typedef Sentence argument_type;

		size_t operator()(const Sentence &s) const;
	};

	template <>
	struct hash<Production>
	{
		typedef size_t result_type;
		typedef Production argument_type;
		size_t operator()(const Production &p) const;
	};

	template <>
	struct hash<std::pair<Symbol, Symbol>>
	{
		typedef size_t result_type;
		typedef std::pair<Symbol, Symbol> argument_type;
		size_t operator()(const std::pair<Symbol, Symbol> &p) const;
	};
}

#endif //PARSER_SYMBOL_H

//
// Created by 李宇轩 on 2018-12-05.
//
#include "Symbol.h"
#include "TypeDefs.h"

using namespace std;

Symbol *Symbol::epsilon = nullptr;
Symbol *Symbol::dollar = nullptr;

Symbol::Symbol():
	label(""), type(SymbolType::Dollar)
{
}

Symbol::Symbol(const char &c, Symbol::SymbolType type):
	label({c}), type(type)
{
}

Symbol::Symbol(const string &label, Symbol::SymbolType type):
	label(label), type(type)
{
}

Symbol::Symbol(Symbol &&s) noexcept :
	label(std::move(s.label)), type(s.type)
{
}

Symbol &Symbol::operator=(Symbol &&rhs) noexcept
{
	label = std::move(rhs.label);
	type = rhs.type;

	return *this;
}

Symbol &Symbol::Epsilon()
{
	if (!epsilon) {
		epsilon = new Symbol("", SymbolType::Epsilon);
	}

	return *epsilon;
}

Symbol &Symbol::Dollar()
{
	if (!dollar) {
		dollar = new Symbol("", SymbolType::Dollar);
	}

	return *dollar;
}

bool operator==(const Symbol &lhs, const Symbol &rhs)
{
	if (lhs.type == rhs.type) {
		return lhs.label == rhs.label;
	}

	return false;
}

bool operator!=(const Symbol &lhs, const Symbol &rhs)
{
	return !(lhs == rhs);
}

bool operator==(const Sentence &lhs, const Sentence &rhs)
{
	bool rst = true;

	if (lhs.size() != rhs.size()) {
		rst = false;
	}

	for (auto lbeg = lhs.begin(), rbeg = rhs.begin(), lend = lhs.end(), rend = rhs.end(); lbeg != lend && rbeg != rend;
		lbeg++, rbeg++) {
		if (*lbeg != *rbeg) {
			rst = false;
		}
	}

	return rst;
}

bool operator==(const Production &lhs, const Production &rhs)
{
	return lhs.first == rhs.first && lhs.second == rhs.second;
}

bool operator==(const std::pair<Symbol, Symbol> &lhs, const std::pair<Symbol, Symbol> &rhs)
{
	return lhs.first == rhs.first && lhs.second == rhs.second;
}

namespace std
{
	size_t hash<Symbol>::operator()(const Symbol &s) const
	{
		return hash<string>()(s.label) ^
		       hash<int>()(static_cast<int>(s.type));
	}

	size_t hash<Sentence>::operator()(const Sentence &s) const
	{
		result_type result = 0;
		for (const auto &symbol : s) {
			result ^= hash<string>()(symbol.label) ^
			          hash<int>()(static_cast<int>(symbol.type));
		}

		return result;
	}

	size_t hash<Production>::operator()(const Production &p) const
	{
		return hash<Sentence>()(p.first) ^
		       hash<Sentence>()(p.second);
	}

	size_t hash<std::pair<Symbol, Symbol>>::operator()(const std::pair<Symbol, Symbol> &p) const
	{
		return hash<Symbol>()(p.first) ^
		       hash<Symbol>()(p.second);
	}
}
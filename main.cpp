#include <iostream>
#include <stack>
#include "Symbol.h"
#include "LL1.h"
#include "TypeDefs.h"

using namespace std;

void getGrammar(unordered_set<Symbol> &T, unordered_set<Symbol> &N, ProductionSet &pSet, Symbol &S);
void getInput(vector<Symbol> &in);
ProductionMap transferProductions(ProductionSet raw);
void showSymbols(const unordered_set<Symbol> &T, const unordered_set<Symbol> &N);
void showProductionSet(const ProductionSet &productions);
void showProductionMap(const unordered_set<Symbol> &N, const ProductionMap &pMap);
void showFirstsAndFollows(const unordered_set<Symbol> &N,
                          const std::unordered_map<Symbol, std::unordered_set<Symbol>> &FIRST,
                          const std::unordered_map<Symbol, std::unordered_set<Symbol>> &FOLLOW);
void showResult(const std::vector<Production> &result);

int main()
{
	unordered_set<Symbol> T, N;
	Symbol S;
	ProductionSet pSet;
	ProductionMap pMap;
	std::unordered_map<Symbol, std::unordered_set<Symbol>> FIRST, FOLLOW;
	Table table;
	vector<Symbol> in;
	std::vector<Production> result;

	getGrammar(T, N, pSet, S); cout << endl;
	getInput(in); cout << endl;
	pMap = transferProductions(pSet);
	FIRST = LL1::getFirsts(T, N, pMap);
	FOLLOW = LL1::getFollows(T, N, pMap, S, FIRST);
	table = LL1::buildTable(T, N, pSet, S, FIRST, FOLLOW);
	result = LL1::parse(S, in, table);

//	showSymbols(T, N); cout << endl;
//	showProductionSet(pSet); cout << endl;
//	showProductionMap(N, pMap); cout << endl;
//	showFirstsAndFollows(N, FIRST, FOLLOW); cout << endl;
	showResult(result);

	return 0;
}

void getGrammar(unordered_set<Symbol> &T, unordered_set<Symbol> &N, ProductionSet &pSet, Symbol &S)
{
	string buffer;

	cout << "Enter the grammar:" << endl;

	while (getline(cin, buffer)) {
		// an empty line means exit
		if (buffer.empty()) {
			break;
		}

		string lhs, rhs;
		Symbol lSymbol, rSymbol;
		Sentence lSentence, rSentence;

		// get and sanitize lhs and rhs
		lhs = buffer.substr(0, buffer.find_first_of("->"));
		rhs = buffer.substr(buffer.find_first_of("->") + 2);
		lhs.erase(remove_if(lhs.begin(), lhs.end(), ::isspace), lhs.end());
		rhs.erase(remove_if(rhs.begin(), rhs.end(), ::isspace), rhs.end());

		// error report
		if (lhs.size() != 1) {
			cout << "*Left-hand side sentence has move than one symbol*" << endl;
			continue;
		}
		if (::islower(lhs[0])) {
			cout << "*Left-hand side is not a non-terminal symbol (Use upper characters to represent non-terminals*"
				 << endl;
			continue;
		}

		// build symbol(s) according to characters
		lSymbol = Symbol(lhs[0], Symbol::SymbolType::NonTerminal);
		// add it into the set of terminals
		N.insert(lSymbol);
		// build the left-hand side sentence
		lSentence = {lSymbol};

		if (rhs == "ε") {
			rSentence.push_back(Symbol::Epsilon());
		}
		else {
			for (const auto &c : rhs) {
				// build symbols according to characters
				if (::isupper(c)) {
					rSymbol = Symbol(c, Symbol::SymbolType::NonTerminal);
				}
				else {
					rSymbol = Symbol(c, Symbol::SymbolType::Terminal);
				}

				// add it into the set of terminals or non-terminals
				if (rSymbol.isTerminal()) {
					T.insert(rSymbol);
				}
				else if (rSymbol.isNonTerminal()) {
					N.insert(rSymbol);
				}

				// build the right-hand side sentence
				rSentence.push_back(rSymbol);
			}
		}

		// build production according to symbols
		pSet.insert({lSentence, rSentence});
	}

	while (true) {
		cout << "Start Symbol: ";
		getline(cin, buffer);
		buffer.erase(remove_if(buffer.begin(), buffer.end(), ::isspace), buffer.end());

		if (buffer.size() != 1) {
			cout << "*Could only accept one start symbol*" << endl;
		}
		else if (::islower(buffer[0])) {
			cout << "*Start symbol must be a terminal symbol*" << endl;
		}
		else {
			S = Symbol(buffer[0], Symbol::SymbolType::NonTerminal);
			break;
		}
	}
}

void getInput(vector<Symbol> &in)
{
	string buffer;

	cout << "Enter input string: " << endl;
	getline(cin, buffer);

	in.clear();
	buffer.erase(remove_if(buffer.begin(), buffer.end(), ::isspace), buffer.end());

	for (const auto &b : buffer) {
		in.emplace_back(b, Symbol::SymbolType::Terminal);
	}
	in.push_back(Symbol::Dollar());
}

ProductionMap transferProductions(ProductionSet raw)
{
	ProductionMap map;

	for (const auto &sentencePair : raw) {
		const auto &lhs = sentencePair.first;
		const auto &rhs = sentencePair.second;

		map.insert({lhs, rhs});
	}

	return map;
}

void showProductionSet(const ProductionSet &productions)
{
	for (const auto &p : productions) {
		for (const auto &s : p.first) {
			cout << s.label;
		}
		cout << " -> ";
		for (const auto &s : p.second) {
			cout << s.label;
		}
		cout << endl;
	}
}

void showSymbols(const unordered_set<Symbol> &T, const unordered_set<Symbol> &N)
{
	cout << "Terminals: ";
	for (const auto &t : T) {
		cout << t.label;
	}
	cout << "\nNon-Terminals: ";
	for (const auto &n : N) {
		cout << n.label;
	}
	cout << endl;
}

void showProductionMap(const unordered_set<Symbol> &N, const ProductionMap &pMap)
{
	for (const auto &n : N) {
		if (pMap.find(vector<Symbol>({n})) != pMap.end()) {
			// traverse all right-hand side sentences of n
			for (auto pos = pMap.equal_range(vector<Symbol>({n})); pos.first != pos.second; ++pos.first) {
				// a right-hand side sentences of n
				const auto &rhs = pos.first->second;

				cout << n.label << " -> ";
				for (const auto &s : rhs) {
					cout << s.label;
				}
				cout << endl;
			}
		}
	}
}

void showFirstsAndFollows(const unordered_set<Symbol> &N,
                          const std::unordered_map<Symbol, std::unordered_set<Symbol>> &FIRST,
                          const std::unordered_map<Symbol, std::unordered_set<Symbol>> &FOLLOW)
{
	cout << "FIRST: " << endl;
	for (const auto &n : N) {
		const auto &ts = FIRST.find(n)->second;

		cout << n.label << ": ";
		for (const auto &t : ts) {
			if (t.isEpsilon()) {
				cout << "ε" << " ";
			}
			else if (t.isDollar()) {
				cout << "$" << " ";
			}
			else {
				cout << t.label << " ";
			}
		}
		cout << endl;
	}

	cout << "\nFOLLOW: " << endl;
	for (const auto &n : N) {
		const auto &ts = FOLLOW.find(n)->second;

		cout << n.label << ": ";
		for (const auto &t : ts) {
			if (t.isEpsilon()) {
				cout << "ε" << " ";
			}
			else if (t.isDollar()) {
				cout << "$" << " ";
			}
			else {
				cout << t.label << " ";
			}
		}
		cout << endl;
	}
}

void showResult(const std::vector<Production> &result)
{
	cout << "Result: " << endl;

	for (const auto &p : result) {
		for (const auto &s : p.first) {
			if (s.isEpsilon()) {
				cout << "ε" << " ";
			}
			else if (s.isDollar()) {
				cout << "$" << " ";
			}
			else {
				cout << s.label << " ";
			}
		}
		cout << " -> ";
		for (const auto &s : p.second) {
			if (s.isEpsilon()) {
				cout << "ε" << " ";
			}
			else if (s.isDollar()) {
				cout << "$" << " ";
			}
			else {
				cout << s.label << " ";
			}
		}
		cout << endl;
	}
}

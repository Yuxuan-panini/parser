//
// Created by 李宇轩 on 2018-12-06.
//
#include "LL1.h"
#include "TypeDefs.h"

using namespace std;

unordered_map<Symbol, unordered_set<Symbol>> LL1::getFirsts(const unordered_set<Symbol> &T,
		const unordered_set<Symbol> &N, const ProductionMap &P)
{
	unordered_map<Symbol, unordered_set<Symbol>> FIRST;
	unordered_map<Symbol, unordered_set<Symbol>> pFIRST;

	// initialize
	for (const auto &t : T) {
		FIRST[t] = {t};
	}
	for (const auto &n : N) {
		FIRST[n] = {};
	}

	// while FIRST sets are still changing
	while (FIRST != pFIRST) {
		pFIRST = FIRST;

		for (const auto &n : N) {
			if (P.find(vector<Symbol>({n})) != P.end()) {
				// traverse all right-hand side sentences of X
				for (auto pos = P.equal_range(vector<Symbol>({n})); pos.first != pos.second; ++pos.first) {
					// a right-hand side sentences of X
					const auto &rhs = pos.first->second;

					if (rhs.empty()) {
						throw runtime_error("Invalid Production");
					}

					// X -> a...
					const auto &t = rhs[0];
					if (t.isTerminal()) {
						FIRST[n].insert(t);
					}
					// X -> e
					else if (t.isEpsilon() && rhs.size() == 1) {
						FIRST[n].insert(t);
					}

					// X -> Y, X -> Y1Y2...Yk
					int i = 0;
					while (i < rhs.size() && rhs[i].isNonTerminal()) {
						FIRST[n].insert(FIRST[rhs[i]].begin(), FIRST[rhs[i]].end());

						if (FIRST[rhs[i]].find(Symbol::Epsilon()) == FIRST[rhs[i]].end()) {
							break;
						}

						i++;
					}

					// X -> ...e
					if (i == rhs.size() && FIRST[rhs[i-1]].find(Symbol::Epsilon()) != FIRST[rhs[i-1]].end()) {
						FIRST[n].insert(Symbol::Epsilon());
					}
				}
			}
		}
	}

	return FIRST;
}

unordered_map<Symbol, unordered_set<Symbol>> LL1::getFollows(const unordered_set<Symbol> &T,
		const unordered_set<Symbol> &N, const ProductionMap &P, const Symbol &S,
		const unordered_map<Symbol, unordered_set<Symbol>> &FIRST)
{
	unordered_map<Symbol, unordered_set<Symbol>> FOLLOW;
	unordered_map<Symbol, unordered_set<Symbol>> pFOLLOW;
	unordered_set<Symbol> trailer;

	// initialize
	for (const auto &n : N) {
		FOLLOW[n] = {};
	}

	FOLLOW[S] = {Symbol::Dollar()};

	// while FOLLOW sets are still changing
	while (FOLLOW != pFOLLOW) {
		pFOLLOW = FOLLOW;

		for (const auto &n : N) {
			if (P.find(vector<Symbol>({n})) != P.end()) {
				// traverse all right-hand side sentences of X
				for (auto pos = P.equal_range(vector<Symbol>({n})); pos.first != pos.second; ++pos.first) {
					// a right-hand side sentences of X
					const auto &rhs = pos.first->second;

					trailer = FOLLOW[n];

					// traverse this sentence from the tail to the head
					for (auto rbeg = rhs.rbegin(), rend = rhs.rend(); rbeg != rend; rbeg++) {
						const auto &s = *rbeg;
						const auto &firstOfS = FIRST.find(s)->second;

						// is non-terminal
						if (s.isNonTerminal()) {
							FOLLOW[s].insert(trailer.begin(), trailer.end());
							if (firstOfS.find(Symbol::Epsilon()) != firstOfS.end()) {
								trailer.insert(firstOfS.begin(), firstOfS.end());
								trailer.erase(Symbol::Epsilon());
							}
							else {
								trailer = firstOfS;
							}
						}
						// is terminal
						else if (s.isTerminal()){
							trailer = firstOfS;
						}
					}
				}
			}
		}
	}

	return FOLLOW;
}

Table LL1::buildTable(const unordered_set<Symbol> &T, const unordered_set<Symbol> &N, const ProductionSet &P,
					  const Symbol &S, const unordered_map<Symbol, unordered_set<Symbol>> &FIRST,
					  const unordered_map<Symbol, unordered_set<Symbol>> &FOLLOW)
{
	Table table;

	for (const auto &n : N) {
		for (const auto &t : T) {
			// use empty production
			table[{n, t}] = {{}, {}};
		}

		// traverse each production A -> a
		for (const auto &p : P) {
			unordered_set<Symbol> firstPlus;
			const auto &lhs = p.first[0];
			const auto &rhs = p.second;

			// get FIRST[a]
			auto beg = rhs.begin(), end = rhs.end();
			for (; beg != end; beg++) {
				const auto &s = *beg;
				if (s == Symbol::Epsilon()) {
					break;
				}

				const auto &firstOfS = FIRST.find(s)->second;
				firstPlus.insert(firstOfS.begin(), firstOfS.end());
				if (firstOfS.find(Symbol::Epsilon()) == firstOfS.end()) {
					break;
				}
			}

			// for each x in FIRST[a], put A -> a into M[A, x]
			for (const auto &x : firstPlus) {
				table[{lhs, x}] = p;
			}

			// if FIRST[a] has an epsilon, then for each y in FOLLOW[A], put A -> a into M[A, y]
			if (beg == end || *beg == Symbol::Epsilon()) {
				const auto &followOfA = FOLLOW.find(lhs)->second;

				for (const auto &y : followOfA) {
					table[{lhs, y}] = p;
				}
			}
		}
	}

	return table;
}

vector<Production> LL1::parse(const Symbol &S, vector<Symbol> in, Table table)
{
	stack<Symbol> stk;
	Symbol word;
	Symbol focus;
	vector<Production> result;

	// return empty vector when failed
	if (in.empty()) {
		return {};
	}

	if (!in.back().isDollar()) {
		in.push_back(Symbol::Dollar());
	}

	stk.push(Symbol::Dollar());
	stk.push(S);
	word = in.front();
	in.erase(in.begin());

	while (true) {
		focus = stk.top();

		if (word.isDollar() && focus.isDollar()) {
			break;
		}
		else if (focus.isTerminal()) {
			if (focus == word) {
				stk.pop();
				word = in.front();
				in.erase(in.begin());
			}
			else {
				// parse failed
				result = {};
				break;
			}
		}
		// focus is a non-terminal symbol
		else {
			const auto &p = table.find({focus, word})->second;

			// M[focus, word] is A -> B1B2...Bk
			if (p.first[0] == focus) {
				const auto &rhs = p.second;

				stk.pop();
				// push Bi into stack from k to 1
				for (auto rbeg = rhs.rbegin(), rend = rhs.rend(); rbeg != rend; rbeg++) {
					if (!rbeg->isEpsilon()) {
						stk.push(*rbeg);
					}
				}

				// produce a production
				result.push_back(p);
			}
			else {
				result = {};
				break;
			}
		}
	}

	return result;
}

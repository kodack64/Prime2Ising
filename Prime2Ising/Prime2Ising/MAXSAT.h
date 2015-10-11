
#pragma once

#include <vector>
#include <string>
#include <algorithm>

class SATInstance;

using namespace std;

typedef vector<pair<bool, string>> LabeledClause;
typedef vector<pair<bool, int>> Clause;
typedef vector<LabeledClause> LabeledClauses;
typedef vector<Clause> Clauses;

class MaxSATInstance {
public:
	Clauses eqs;
	int variableCount;
	int clauseCount;

	int bitSize;
	int composite;
	int answer;
	int maxSatisfied;
	vector<string> variableList;
	vector<int> answerBits;

	MaxSATInstance(SATInstance* si);
};
#pragma once


#include <vector>
#include <string>
#include <algorithm>

class MaxSATInstance;
using namespace std;

typedef vector<pair<bool, string>> LabeledClause;
typedef vector<pair<bool, int>> Clause;
typedef vector<LabeledClause> LabeledClauses;
typedef vector<Clause> Clauses;
typedef pair<double, vector<pair<bool, int>>> HamiltonianClause;
typedef vector<HamiltonianClause> HamiltonianClauses;

class QUBOInstance {
public:
	vector<int> singleTerm;
	vector < vector<int> > quadTerm;
	vector<string> variableList;
	vector<int> answerBits;

	int variableCount;
	int clauseCount;
	int maxQUBO;

	int bitSize;
	int composite;
	int answer;

	QUBOInstance(MaxSATInstance* mi);
};
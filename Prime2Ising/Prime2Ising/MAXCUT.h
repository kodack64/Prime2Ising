#pragma once


#include <vector>
#include <string>
#include <algorithm>

class QUBOInstance;
using namespace std;

typedef vector<pair<bool, string>> LabeledClause;
typedef vector<pair<bool, int>> Clause;
typedef vector<LabeledClause> LabeledClauses;
typedef vector<Clause> Clauses;
typedef pair<double, vector<pair<bool, int>>> HamiltonianClause;
typedef vector<HamiltonianClause> HamiltonianClauses;

class MAXCUTInstance {
public:
	vector < vector<int> > weight;
	vector<string> variableList;

	int variableCount;
	int clauseCount;
	int maxcutValue;
	vector<int> answerBits;

	int bitSize;
	int composite;
	int answer;

	MAXCUTInstance(QUBOInstance* mi);
	void outputGraph(string filename);
};

#pragma once

#include <vector>
#include <string>
#include <algorithm>

class CircuitElement;
class CircuitInstance;

using namespace std;

typedef vector<pair<bool,string>> LabeledClause;
typedef vector<pair<bool,int>> Clause;
typedef vector<LabeledClause> LabeledClauses;
typedef vector<Clause> Clauses;

class SATInstance{
public:
	Clauses eqs;
	int variableCount;
	int clauseCount;

	int bitSize;
	int composite;
	int answer;
	vector<int> answerBits;
	vector<string> variableList;

	SATInstance(CircuitInstance* ci);
	LabeledClauses getCNFFormula(CircuitElement* ce);
	void outputCNF(string file);
	void outputVariable(string file);
	void simplify(); 
	void calcAnswerBits();
	void calcSubAnswerBits();
};

#pragma once

#include <algorithm>
#include <vector>

using namespace std;

class SATInstance;
class MAXCUTInstance;
typedef pair<double, vector<pair<bool,int>>> HamiltonianClause;
typedef vector<HamiltonianClause> HamiltonianClauses;

class HamiltonianInstance{
public:
	int bitSize;
	int answer;
	int composite;
	vector<int> answerBits;
	double minimumEnergy;
	HamiltonianInstance(string file);
	HamiltonianInstance(SATInstance* si);
	HamiltonianInstance(MAXCUTInstance* si);
	int getVariableCount();
	HamiltonianClauses clauses;
	vector< HamiltonianClauses > relation;
	int variableCount;
	int clauseCount;
	void outputClause(string file);
private:
	void createRelation();
};

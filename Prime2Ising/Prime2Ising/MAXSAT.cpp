
#include "SAT.h"
#include "MAXSAT.h"

#include <fstream>
#include <iostream>

MaxSATInstance::MaxSATInstance(SATInstance* si) {
	eqs.clear();

	bitSize = si->bitSize;
	composite = si->composite;
	answer = si->answer;
	variableList = si->variableList;
	variableCount = si->variableCount;
	answerBits = si->answerBits;

	// convert 3-SAT Instance to MAX-2-SAT Instance
	// x v y v z -> x , y , z , w , ^x v ^y , ^y v ^z , ^z v ^x, x v ^w, y v ^w, z v ^w
	// this is satisfied 7/10 clause when original satisfied
	// this is satisfied 6/10 clause when original not satisfied
	// 1,2 clause "x" and "x v y" is kept, so weight of 3term and 2term is equal
	// we need new sites, their number is equal to number of 3term clause
	// we add new clause, number of new clauses are equal to 9*(#3term clause)
	eqs.clear();
	maxSatisfied = 0;
	int addVar = 0;
	for (auto clause : si->eqs) {
		int num = clause.size();
		if (num == 3) {
			int addVarInd = variableCount + addVar;
			variableList.push_back("_addVar"+to_string(addVar));
			vector<pair<bool, int>> teqs;
			
			// x,y,z,w
			teqs.clear(); teqs.push_back(clause[0]);
			eqs.push_back(teqs);
			teqs.clear(); teqs.push_back(clause[1]);
			eqs.push_back(teqs);
			teqs.clear(); teqs.push_back(clause[2]);
			eqs.push_back(teqs);
			teqs.clear(); teqs.push_back(make_pair(false,addVarInd));
			eqs.push_back(teqs);

			// ^x v ^y and other
			teqs.clear(); 
			teqs.push_back(make_pair(!clause[0].first,clause[0].second));
			teqs.push_back(make_pair(!clause[1].first,clause[1].second));
			eqs.push_back(teqs);
			teqs.clear();
			teqs.push_back(make_pair(!clause[1].first, clause[1].second));
			teqs.push_back(make_pair(!clause[2].first, clause[2].second));
			eqs.push_back(teqs);
			teqs.clear();
			teqs.push_back(make_pair(!clause[2].first, clause[2].second));
			teqs.push_back(make_pair(!clause[0].first, clause[0].second));
			eqs.push_back(teqs);

			// x v ^w and other
			teqs.clear();
			teqs.push_back(clause[0]);
			teqs.push_back(make_pair(true, addVarInd));
			eqs.push_back(teqs);
			teqs.clear();
			teqs.push_back(clause[1]);
			teqs.push_back(make_pair(true, addVarInd));
			eqs.push_back(teqs);
			teqs.clear();
			teqs.push_back(clause[2]);
			teqs.push_back(make_pair(true, addVarInd));
			eqs.push_back(teqs);

			addVar++;
			maxSatisfied += 7;

			int satisfiedVariable = 0;
			for (int i = 0; i < 3; i++) {
				if (answerBits[clause[i].second] == 1 && clause[i].first == false) satisfiedVariable++;
				if (answerBits[clause[i].second] == 0 && clause[i].first == true) satisfiedVariable++;
			}
			if (satisfiedVariable == 1) {
				answerBits.push_back(0);
			}else if(satisfiedVariable>1) {
				answerBits.push_back(1);
			}else {
				cout << "stacked" << endl;
			}
		}
		else {
			eqs.push_back(clause);
			maxSatisfied++;
		}
	}

	variableCount = variableList.size();
	clauseCount = eqs.size();
	int sc = 0;
	for (auto clause : eqs) {
		int sv = 0;
		for (int i = 0; i < clause.size(); i++) {
			int v = answerBits[clause[i].second];
			if (clause[i].first == true && v == 0 || clause[i].first == false && v == 1) sv++;
		}
		if (sv > 0) sc++;
	}
	if (sc != maxSatisfied) {
		cout << "stacked" << endl;
	}
}


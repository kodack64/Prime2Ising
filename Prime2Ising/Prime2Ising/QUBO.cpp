
#include "SAT.h"
#include "QUBO.h"
#include "MaxSAT.h"

#include <fstream>
#include <iostream>

QUBOInstance::QUBOInstance(MaxSATInstance* mi) {
	answer = mi->answer;
	bitSize = mi->bitSize;
	composite = mi->composite;
	variableList = mi->variableList;
	answerBits = mi->answerBits;

	singleTerm.resize(mi->variableCount);
	quadTerm.resize(mi->variableCount);
	for (int i = 0; i < quadTerm.size(); i++) {
		singleTerm[i] = 0;
		quadTerm[i].resize(mi->variableCount);
		for (int j = 0; j < quadTerm[i].size(); j++) {
			quadTerm[i][j] = 0;
		}
	}
	maxQUBO = 0;

	int satisfied = 0;
	for (int i = 0; i < mi->eqs.size(); i++) {
		Clause cl = mi->eqs[i];
		if (cl.size() == 1) {
			if (!cl[0].first) {
				// x 1/0
				singleTerm[cl[0].second] += 1;
				if (answerBits[cl[0].second] == 1) maxQUBO++;
			} else {
				// -x 0/-1
				singleTerm[cl[0].second] -= 1;
				if (!(answerBits[cl[0].second] == 0)) maxQUBO--;
			}
		}
		else {
			if ((!cl[0].first) && (!cl[1].first)) {
				// x+y-xy 1/0
				singleTerm[cl[0].second] += 1;
				singleTerm[cl[1].second] += 1;
				quadTerm[cl[0].second][cl[1].second] -= 1;
				quadTerm[cl[1].second][cl[0].second] -= 1;

				if (answerBits[cl[0].second] == 1 || answerBits[cl[1].second]==1) maxQUBO++;
			}
			if ((!cl[0].first) && (cl[1].first)) {
				// -y+xy 0/-1
				singleTerm[cl[1].second] -= 1;
				quadTerm[cl[0].second][cl[1].second] += 1;
				quadTerm[cl[1].second][cl[0].second] += 1;

				if ( !(answerBits[cl[0].second] == 1 || answerBits[cl[1].second] == 0)) maxQUBO--;
			}
			if ((cl[0].first) && (!cl[1].first)) {
				// -x+xy 0/-1
				singleTerm[cl[0].second] -= 1;
				quadTerm[cl[0].second][cl[1].second] += 1;
				quadTerm[cl[1].second][cl[0].second] += 1;

				if (!(answerBits[cl[0].second] == 0 || answerBits[cl[1].second] == 1)) maxQUBO--;
			}
			if ((cl[0].first) && (cl[1].first)) {
				// -xy 0/-1
				quadTerm[cl[0].second][cl[1].second] -= 1;
				quadTerm[cl[1].second][cl[0].second] -= 1;
				if (!(answerBits[cl[0].second] == 0 || answerBits[cl[1].second] == 0)) maxQUBO--;
			}
		}
	}
	variableCount = mi->variableCount;
	clauseCount = 0;
	for (int i = 0; i < variableCount; i++) {
		for (int j = 0; j < i; j++) {
			if (quadTerm[j][i] != 0)clauseCount++;
		}
		if (singleTerm[i] != 0)clauseCount ++;
	}

	int check = 0;
	for (int i = 0; i < variableCount; i++) {
		check += singleTerm[i] * answerBits[i];
		for (int j = 0; j < i; j++) {
			check += quadTerm[j][i] * answerBits[i] * answerBits[j];
		}
	}

	/*
	for (int i = 0; i < (1 << answerBits.size()); i++) {
		int tcheck = 0;
		vector<int> ta;
		for (int j = 0; j < answerBits.size(); j++) {
			ta.push_back((i>>j)%2);
		}
		for (int k = 0; k < variableCount; k++) {
			tcheck += singleTerm[k] * ta[k];
			for (int l = 0; l < k; l++) {
				tcheck += quadTerm[l][k] * ta[k] * ta[l];
			}
		}
		if (tcheck >= check) {
			for (int j = 0; j < answerBits.size(); j++) {
				cout << ta[j];
			}
			cout << ":" << tcheck << endl;
		}
	}
	*/

	if (check != maxQUBO) {
		cout << "stacked" << endl;
	}

}


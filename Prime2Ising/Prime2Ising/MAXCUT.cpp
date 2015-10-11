

#include "SAT.h"
#include "QUBO.h"
#include "MAXCUT.h"

#include <fstream>
#include <iostream>

MAXCUTInstance::MAXCUTInstance(QUBOInstance* qi) {
	answer = qi->answer;
	bitSize = qi->bitSize;
	composite = qi->composite;
	variableCount = qi->variableCount+ 1;
	answerBits = qi->answerBits;
	variableList = qi->variableList;
	variableList.push_back("__supernode");

	// initialize weight graph
	weight.resize(variableCount);
	for (int i = 0; i < variableCount; i++) {
		weight[i].resize(variableCount);
		for (int j = 0; j < weight[i].size(); j++) {
			weight[i][j] = 0;
		}
	}

	// pre-calculation for sq_i = sum_j q_ij
	vector<int> sumQuad;
	sumQuad.resize(variableCount - 1);
	for (int i = 0; i < variableCount-1; i++) {
		sumQuad[i] = 0;
	}
	for (int i = 0; i < variableCount - 1; i++) {
		for (int j = 0; j < variableCount-1; j++) {
			sumQuad[i] += qi->quadTerm[j][i];
		}
	}

	// calculation weight
	for (int i = 0; i < variableCount-1; i++) {
		// instead of dividing q_ij -> q_ij + q_ji,
		// doubling s_i
		weight[i][variableCount - 1] += qi->singleTerm[i]*2 + sumQuad[i];
		for (int j = 0; j < i; j++) {
			weight[j][i] -= qi->quadTerm[j][i];
		}
	}

	clauseCount = 0;
	for (int i = 0; i < variableCount; i++) {
		for (int j = 0; j < i; j++) {
			if (weight[j][i] != 0)clauseCount++;
		}
	}

	answerBits.push_back(0);
	maxcutValue = 0;
	for (int i = 0; i < variableCount; i++) {
		for (int j = 0; j < i; j++) {
			if (answerBits[i] != answerBits[j]) {
				maxcutValue += weight[j][i];
			}
		}
	}

	/*
	for (int i = 0; i < (1 << answerBits.size()); i++) {
		int tcheck = 0;
		vector<int> ta;
		for (int j = 0; j < answerBits.size(); j++) {
			ta.push_back((i >> j) % 2);
		}
		for (int k = 0; k < variableCount; k++) {
			for (int l = 0; l < k; l++) {
				if (ta[k] != ta[l]) {
					tcheck += weight[l][k];
				}
			}
		}
		if (tcheck >= maxcutValue) {
			for (int j = 0; j < answerBits.size(); j++) {
				cout << ta[j];
			}
			cout << ":" << tcheck << endl;
		}
	}
	*/

}

void MAXCUTInstance::outputGraph(string filename) {
	ofstream ofs(filename);
	ofs << variableCount << " " << clauseCount << endl;
	for (int i = 0; i < variableCount; i++) {
		for (int j = 0; j < i; j++) {
			if (weight[j][i]!=0)
				ofs << j+1 << " " << i+1 << " " << weight[j][i] << endl;
		}
	}
	ofs.close();
}

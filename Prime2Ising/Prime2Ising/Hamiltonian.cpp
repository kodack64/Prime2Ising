
#include "Hamiltonian.h"
#include "SAT.h"
#include "MAXCUT.h"

#include <fstream>
#include <algorithm>
#include <iterator>
#include <iostream>

HamiltonianInstance::HamiltonianInstance(string file){
	ifstream fs(file);
	if (!fs)return; 

	fs >> variableCount >> clauseCount;
	for (int i = 0; i < clauseCount; i++){
		HamiltonianClause cl;
		fs >> cl.first;
		int count;
		fs >> count;
		for (int j = 0; j < count; j++){
			int temp;
			fs >> temp;
			cl.second.push_back(make_pair(true,temp));
		}
		clauses.push_back(cl);
	}
	fs.close();
	clauseCount = clauses.size();
	createRelation();
}
HamiltonianInstance::HamiltonianInstance(SATInstance* si){
	bitSize = si->bitSize;
	answer = si->answer;
	composite = si->composite;

	variableCount = si->variableCount;

	// decompose SAT clause to Hamiltonian term
	// e.g. satisfy (x or y or  not(z))?True:False is equal to minimize (1-x)(1-y)(1+z)-1?-1:7 (x,y,z={-1,1})
	// (1-x)(1-y)(1-z)-1 = -x-y-z + xy+yz+zx - xyz
	// satisfy (x or y) is equal to minimize (1-x)(1-y)-1 = -x-y+xy
	for (auto clause : si->eqs){
		int num = clause.size();

		// variable "i"'s n-th bit means (this term use n-th variable or not)
		// constant value i=0 is passed
		for (int i = 1; i < (1 << num); i++){
			HamiltonianClause hcl;
			bool isNegative = false;
			for (int j = 0; j < num; j++){

				// index starts from 1 @SAT , index starts from 0 @Ham, so clause[].second is decreased

				{
					// use absolute : force all terms to the same dimension
	//				hcl.second.push_back(make_pair(((i>>j)%2 == 1),clause[j].second)); 
					// not use absolute
					if ((i >> j) % 2 == 1) hcl.second.push_back(make_pair(true, clause[j].second));
				}

				// if useing variable dont has neg flag(clause[].fisrt == false) , term is inverted
				if ((i >> j) % 2 == 1) {
					if (!clause[j].first)isNegative = !isNegative;
				}
			}
			hcl.first = isNegative ? -1 : 1;
//			sort(hcl.second.begin(), hcl.second.end());

/*			cout << hcl.first;
			for (int k = 0; k < hcl.second.size(); k++)cout << hcl.second[k].second;
			cout << endl;*/

			clauses.push_back(hcl);
		}
	}

	// gather duplicated clause
	for (unsigned int i = 0; i < clauses.size(); i++){
		for (unsigned int j = i+1; j < clauses.size(); j++){
			if (clauses[i].second == clauses[j].second){
				clauses[i].first += clauses[j].first;
				clauses.erase(clauses.begin() + j);
				j--;
			}
		}
	}
	clauses.erase(remove_if(clauses.begin(), clauses.end(), [](HamiltonianClause hc)->bool{return fabs(hc.first) < 0.5; }),clauses.end());

	clauseCount = clauses.size();

	createRelation();
}


HamiltonianInstance::HamiltonianInstance(MAXCUTInstance* mci) {
	bitSize = mci->bitSize;
	answer = mci->answer;
	composite = mci->composite;
	answerBits = mci->answerBits;
	variableCount = mci->variableCount;

	for (int i = 0; i < variableCount; i++) {
		for (int j = 0; j < i; j++) {
			if (mci->weight[j][i] != 0) {
				HamiltonianClause hcl;
				hcl.first = mci->weight[j][i];
				hcl.second.push_back(make_pair(true, i));
				hcl.second.push_back(make_pair(true, j));
				clauses.push_back(hcl);
			}
		}
	}
	clauseCount = clauses.size();
	createRelation();

	minimumEnergy = 0;
	for (auto hcl : clauses) {
		minimumEnergy += hcl.first*((answerBits[hcl.second[0].second] == answerBits[hcl.second[1].second]) ? 1 : -1);
	}

	/*
	for (int i = 0; i < (1 << answerBits.size()); i++) {
		double checkEne = 0;
		vector<int> ta;
		for (int j = 0; j < answerBits.size(); j++) {
			ta.push_back((i >> j) % 2);
		}
		for (auto hcl : clauses) {
			checkEne += hcl.first*((ta[hcl.second[0].second] == ta[hcl.second[1].second]) ? 1 : -1);
		}
		if (checkEne <= minimumEnergy) {
			for (int j = 0; j < ta.size(); j++) {
				cout << ta[j];
			}
			cout << ":" << checkEne << endl;
		}
	}
	*/
}

// initialize relation from clauses
void HamiltonianInstance::createRelation(){
	relation.resize(variableCount);
	// create relation about i-th site
	for (int i = 0; i < variableCount; i++){
		for (auto cl : clauses){
			if (find_if(cl.second.begin(), cl.second.end(), [i](pair<bool, int> x)->bool{return x.second == i && x.first==true; }) != cl.second.end()){
				vector<pair<bool,int>> rev;
				copy_if(cl.second.begin(), cl.second.end(), back_inserter(rev), [i](pair<bool,int> x)->bool{ return x.second != i; });
				relation[i].push_back(make_pair(cl.first, rev));
			}
		}
	}
}

int HamiltonianInstance::getVariableCount(){
	return variableCount;
}

void HamiltonianInstance::outputClause(string file){
	ofstream ofs(file);
	ofs << variableCount << " " << clauseCount << endl;
	for (auto clause : clauses){
		ofs << clause.first << " " << clause.second.size();
		for (auto label : clause.second){
			if(label.first)ofs << " " << label.second;
			else ofs << " |" << label.second << "|";
		}
		ofs << endl;
	}
	ofs.close();
}


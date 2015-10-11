
#include "SAT.h"
#include "Circuit.h"

#include <fstream>

SATInstance::SATInstance(CircuitInstance* ci){
	eqs.clear();

	bitSize = ci->bitSize;
	composite = ci->correctOutput;
	answer = ci->correctInput1;

	// convert CircuitElement to CNF Formula
	LabeledClauses seqs;
	for(unsigned int i=0;i<ci->elements.size();i++){
		LabeledClauses lc;
		lc = getCNFFormula(ci->elements[i]);
		seqs.insert(seqs.end(),lc.begin(),lc.end());
	}

	// gather variable list of CNF Formula and assign number
	variableList.clear();
	for(unsigned int i=0;i<seqs.size();i++){
		for(unsigned int j=0;j<seqs[i].size();j++){
			variableList.push_back(seqs[i][j].second);
		}
	}
	sort(variableList.begin(), variableList.end());
	variableList.erase(unique(variableList.begin(), variableList.end()), variableList.end());

	// create nodeMap (name of node, index of variableList+1)
	// to fit CNF formula, variable index start from 0
	map<string, int> nodeMap;
	for(unsigned int i=0;i<variableList.size();i++){
		nodeMap.insert(make_pair(variableList[i],i));
	}
	for(unsigned int i=0;i<seqs.size();i++){
		vector<pair<bool,int>> teqs;
		for(unsigned int j=0;j<seqs[i].size();j++){
			teqs.push_back(make_pair(seqs[i][j].first,nodeMap[seqs[i][j].second]));
		}
		eqs.push_back(teqs);
	}

	// add clause which force multiply CircuitAnswer must be composite
	for(int i=0;i<(ci->bitSize-1)*2;i++){
		vector<pair<bool,int>> teqs;
		string nodes = ci->st_output+"_"+CircuitNode::myToString(0)+"_"+CircuitNode::myToString(i);
		if(i<ci->bitSize)teqs.push_back(make_pair((ci->correctOutput>>i)%2==0,nodeMap[nodes]));
		else teqs.push_back(make_pair(true,nodeMap[nodes]));
		eqs.push_back(teqs);
	}

	variableCount = variableList.size();
	clauseCount = eqs.size();

}

LabeledClauses SATInstance::getCNFFormula(CircuitElement* ce){
	LabeledClauses res;
	// decompose each element to 2in1out and convert to SAT
	if(ce->type==AND){
		LabeledClause clause;
		/*
		clause.push_back(make_pair(true,ce->inputNodes[0].toString()));
		clause.push_back(make_pair(true,ce->inputNodes[1].toString()));
		clause.push_back(make_pair(false,ce->outputNodes[0].toString()));
		clause[0].first=true;clause[1].first=true;clause[2].first=!(clause[0].first&clause[1].first);res.push_back(clause);
		clause[0].first=true;clause[1].first=false;clause[2].first=!(clause[0].first&clause[1].first);res.push_back(clause);
		clause[0].first=false;clause[1].first=true;clause[2].first=!(clause[0].first&clause[1].first);res.push_back(clause);
		clause[0].first=false;clause[1].first=false;clause[2].first=!(clause[0].first&clause[1].first);res.push_back(clause);
		*/
		clause.clear();
		clause.push_back(make_pair(true,ce->inputNodes[0].toString()));
		clause.push_back(make_pair(true,ce->inputNodes[1].toString()));
		clause.push_back(make_pair(false,ce->outputNodes[0].toString()));
		res.push_back(clause);
		clause.clear();
		clause.push_back(make_pair(false,ce->inputNodes[0].toString()));
		clause.push_back(make_pair(true,ce->outputNodes[0].toString()));
		res.push_back(clause);
		clause.clear();
		clause.push_back(make_pair(false,ce->inputNodes[1].toString()));
		clause.push_back(make_pair(true,ce->outputNodes[0].toString()));
		res.push_back(clause);

	}else if(ce->type==XOR){
		LabeledClause clause;
		clause.push_back(make_pair(false,ce->inputNodes[0].toString()));
		clause.push_back(make_pair(false,ce->inputNodes[1].toString()));
		clause.push_back(make_pair(false,ce->outputNodes[0].toString()));		
		clause[0].first=true;clause[1].first=true;clause[2].first=!(clause[0].first^clause[1].first);res.push_back(clause);
		clause[0].first=true;clause[1].first=false;clause[2].first=!(clause[0].first^clause[1].first);res.push_back(clause);
		clause[0].first=false;clause[1].first=true;clause[2].first=!(clause[0].first^clause[1].first);res.push_back(clause);
		clause[0].first=false;clause[1].first=false;clause[2].first=!(clause[0].first^clause[1].first);res.push_back(clause);
	}else if(ce->type==HALF_ADDER){
		LabeledClauses cnf1 = getCNFFormula(&C_XOR(ce->inputNodes[0],ce->inputNodes[1],ce->outputNodes[0]));
		LabeledClauses cnf2 = getCNFFormula(&C_AND(ce->inputNodes[0],ce->inputNodes[1],ce->outputNodes[1]));
		res.insert(res.end(),cnf1.begin(),cnf1.end());
		res.insert(res.end(),cnf2.begin(),cnf2.end());
	}else if(ce->type==FULL_ADDER){
		vector<string> s;s.push_back(ce->inputNodes[0].toString());s.push_back(ce->inputNodes[1].toString());s.push_back(ce->inputNodes[2].toString());
		sort(s.begin(),s.end());
		CircuitNode intermediate0(string("inter_")+s[0]+"_"+s[1]+"_"+s[2]+"_xor",0,0);
		CircuitNode intermediate1(string("inter_")+s[0]+"_"+s[1]+"_"+s[2]+"_inc1",0,0);
		CircuitNode intermediate2(string("inter_")+s[0]+"_"+s[1]+"_"+s[2]+"_inc2",0,0);
		
		LabeledClauses cnf1 = getCNFFormula(&C_HALF_ADDER(ce->inputNodes[0],ce->inputNodes[1],intermediate0,intermediate1));
		LabeledClauses cnf2 = getCNFFormula(&C_HALF_ADDER(ce->inputNodes[2],intermediate0,ce->outputNodes[0],intermediate2));
		LabeledClauses cnf3 = getCNFFormula(&C_XOR(intermediate1,intermediate2,ce->outputNodes[1]));
		res.insert(res.end(),cnf1.begin(),cnf1.end());
		res.insert(res.end(),cnf2.begin(),cnf2.end());
		res.insert(res.end(),cnf3.begin(),cnf3.end());
	}
	return res;
}

void SATInstance::outputCNF(string file){
	fstream fs(file,ios::out);
	fs << "c This CNF is created by " << bitSize << "bit number : " << composite << " = " << answer << " * " << composite/answer << endl;
	fs << "c This eq is satisfied when var(1-"+to_string(bitSize-1)+") * var("
		+to_string(bitSize)+"-"+to_string((bitSize-1)*2)+") = var("
		+to_string((bitSize-1)*2+1)+"-"+to_string((bitSize-1)*4)+") = "
		+to_string(composite) << endl;
	fs << "p cnf " << variableCount << " " << clauseCount << endl;
	for(unsigned int i=0;i<eqs.size();i++){
		for(unsigned int j=0;j<eqs[i].size();j++){
			fs << (eqs[i][j].first?"-":"") << (eqs[i][j].second+1) << " ";
		}
		fs << "0" << endl;
	}
	fs.close();
}
void SATInstance::outputVariable(string file){
	fstream fs(file,ios::out);
	for(unsigned int i=0;i<variableList.size();i++){
		fs << i << " " << variableList[i] << endl;
	}
	fs.close();
}


// solve 3-SAT with answer for debug
void SATInstance::calcAnswerBits(){

	// make deep copy
	Clauses testEqs = eqs;

	// fill all bits unknown(-1)
	answerBits.resize(variableCount);
	for(int i=0;i<variableCount;i++)answerBits[i]=-1;

	// first bits are always answer and composite/answer
	for(int i=0;i+1<bitSize;i++){
		answerBits[i]=(answer>>i)%2;
		answerBits[i+bitSize-1]=((composite/answer)>>i)%2;
	}

	// repeat until all clauses are satisfied
	while(testEqs.size()>0){

		// write status
/*		cout << testEqs.size() << "clauses" << endl;
		for(int i=1;i<variableCount+1;i++){
			cout << (answerBits[i]==-1?"*":to_string(answerBits[i]));
		}
		cout << endl;*/

		int currentAssigned = 0;
		for(unsigned int i=0;i<testEqs.size();i++){
			Clause cl = testEqs[i];
			int ok=0;
			int ng=0;
			int und=0;

			// check 
			// ok = satisfied
			// ng = not satisfied
			// und = this bit is undefined
			// answerBits ==0 -> false
			// answerBits ==1 -> true
			// cl[].first == true -> this variable has negative flag
			for(unsigned int j=0;j<cl.size();j++){
				if(answerBits[cl[j].second]==-1)und++;
				else if(answerBits[cl[j].second]==0 && cl[j].first) ok++;
				else if(answerBits[cl[j].second]==1 && (!cl[j].first)) ok++;
				else ng++;
			}


			// if at least one variable is satisfied, this is satisfied
			if(ok!=0){
				testEqs.erase(testEqs.begin()+i);
				i--;
			}
			// if only one term is satisfied, variable is assigned
			else if(und==1){
				for(unsigned int j=0;j<cl.size();j++){
					if(answerBits[cl[j].second]==-1){
						answerBits[cl[j].second]=cl[j].first?0:1;
					}
				}
				testEqs.erase(testEqs.begin()+i);
				i--;
				currentAssigned++;
			}
			// if all variables are not satisfied, output error
			if(cl.size()==ng){
				cout << "stacked" << endl;
				for (unsigned int j = 0; j<cl.size(); j++){
					cout << (cl[j].first?"-":"")<< cl[j].second << " ";
				}
				cout << endl;
				for (unsigned int j = 0; j<cl.size(); j++){
					cout << answerBits[cl[j].second] << " ";
				}
				cout << endl;
			}
		}
		if (currentAssigned == 0) {
			cout << "stacked" << endl;
		}
	}

	/*
	cout << "answer = ";
	for(int i=0;i<variableCount;i++){
		cout << (answerBits[i]==-1?"*":to_string(answerBits[i]));
	}
	cout << endl;
	*/
//	cout << "all satisfied" << endl;
}
// solve 3-SAT with answer for debug
void SATInstance::calcSubAnswerBits() {

	// make deep copy
	Clauses testEqs = eqs;

	vector<int> subAnswerBits;

	// fill all bits unknown(-1)
	subAnswerBits.resize(variableCount);
	for (int i = 0; i<variableCount; i++)subAnswerBits[i] = -1;

	// first bits are always answer and composite/answer
	for (int i = 0; i + 1<bitSize; i++) {
		subAnswerBits[i] = ((composite/answer) >> i) % 2;
		subAnswerBits[i + bitSize - 1] = ((answer) >> i) % 2;
	}

	// repeat until all clauses are satisfied
	while (testEqs.size()>0) {

		// write status
		/*		cout << testEqs.size() << "clauses" << endl;
		for(int i=1;i<variableCount+1;i++){
		cout << (subAnswerBits[i]==-1?"*":to_string(subAnswerBits[i]));
		}
		cout << endl;*/

		int currentAssigned = 0;
		for (unsigned int i = 0; i<testEqs.size(); i++) {
			Clause cl = testEqs[i];
			int ok = 0;
			int ng = 0;
			int und = 0;

			// check 
			// ok = satisfied
			// ng = not satisfied
			// und = this bit is undefined
			// subAnswerBits ==0 -> false
			// subAnswerBits ==1 -> true
			// cl[].first == true -> this variable has negative flag
			for (unsigned int j = 0; j<cl.size(); j++) {
				if (subAnswerBits[cl[j].second] == -1)und++;
				else if (subAnswerBits[cl[j].second] == 0 && cl[j].first) ok++;
				else if (subAnswerBits[cl[j].second] == 1 && (!cl[j].first)) ok++;
				else ng++;
			}


			// if at least one variable is satisfied, this is satisfied
			if (ok != 0) {
				testEqs.erase(testEqs.begin() + i);
				i--;
			}
			// if only one term is satisfied, variable is assigned
			else if (und == 1) {
				for (unsigned int j = 0; j<cl.size(); j++) {
					if (subAnswerBits[cl[j].second] == -1) {
						subAnswerBits[cl[j].second] = cl[j].first ? 0 : 1;
					}
				}
				testEqs.erase(testEqs.begin() + i);
				i--;
				currentAssigned++;
			}
			// if all variables are not satisfied, output error
			if (cl.size() == ng) {
				cout << "stacked" << endl;
				for (unsigned int j = 0; j<cl.size(); j++) {
					cout << (cl[j].first ? "-" : "") << cl[j].second << " ";
				}
				cout << endl;
				for (unsigned int j = 0; j<cl.size(); j++) {
					cout << subAnswerBits[cl[j].second] << " ";
				}
				cout << endl;
			}
		}
		if (currentAssigned == 0) {
			cout << "stacked" << endl;
		}
	}

	cout << "answer = ";
	for (int i = 0; i<variableCount; i++) {
		cout << (subAnswerBits[i] == -1 ? "*" : to_string(subAnswerBits[i]));
	}
	cout << endl;
//	cout << "all satisfied" << endl;
}

// remove 1-variable clause in 3SAT
void SATInstance::simplify(){
	map<int, bool> trivialVariables;
	// gather trivial variables
	for (unsigned int i = 0; i < eqs.size(); i++){
		Clause cl = eqs[i];
		if (cl.size() == 1){
			trivialVariables.insert(make_pair(cl[0].second,cl[0].first));
			eqs.erase(eqs.begin()+i);
			i--;
		}
	}

	// remove trivial clauses
	for (unsigned int i = 0; i < eqs.size(); i++){
		for (unsigned int j = 0; j < eqs[i].size(); j++){
			auto ite = trivialVariables.find(eqs[i][j].second);
			// if current clause has trivial variable
			if (ite != trivialVariables.end()){
				// if var is true, delete clause
				if (ite->second == eqs[i][j].first){
					eqs.erase(eqs.begin() + i);
					i--;
					break;
				}
				// if var is false, delete variable
				else{
					eqs[i].erase(eqs[i].begin() + j);
					j--;
					break;
				}
			}
		}
	}

	clauseCount = eqs.size();
	variableCount -= trivialVariables.size();

	// relabel so as to compensate removed trivial labels
	vector<int> trivialLabels;
	for (auto label : trivialVariables){
		trivialLabels.push_back(label.first);
	}

	for (int i = 0; i < variableCount ; i++){
		unsigned int j;
		auto ite = find(trivialLabels.begin(), trivialLabels.end(), i);

		// if find trivial variable < variableCount
		if (ite != trivialLabels.end()){

			// move id > variableCount , move to position of trivial variable
			for (j = variableCount; j < variableCount + trivialLabels.size(); j++){
				if (find(trivialLabels.begin(), trivialLabels.end(), j) == trivialLabels.end())break;
			}
			// replace id j to id i
			for (auto &clause : eqs){
				for (auto &term : clause){
					if (term.second == j) term.second = i;
				}
			}
//			cout << i << ":" << variableList[i] << " is trivial, assign" << j << ":" << variableList[j] <<  "instead" << endl;
			variableList[i] = variableList[j];

			(*ite) = j;
		}
	}
	variableList.resize(variableCount);
}




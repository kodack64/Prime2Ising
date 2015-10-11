
#include <iostream>
#include <fstream>
#include "Factoring.h"
#include "Circuit.h"
#include "SAT.h"
#include "Hamiltonian.h"
#include "Anealing.h"
#include "MAXSAT.h"
#include "QUBO.h"
#include "MAXCUT.h"

using namespace std;

void exec() {
	int minPrime, maxPrime;
	cout << "ranmin>";
	cin >> minPrime;
	cout << "ranmax>";
	cin >> maxPrime;

	FactoringInstance fi(minPrime, maxPrime);
	cout << "choose " << fi.bitSize << " bit number " << fi.composite << "=" << fi.answer << "*" << fi.composite / fi.answer << endl;

	cout << "generate circuitSAT with size " << fi.bitSize << endl;
	CircuitInstance ci(&fi);
	cout << " #Element : " << ci.elements.size() << endl;
	cout << " #Node : " << ci.nodeCount << endl;

	cout << "generate CNF-3-SAT" << endl;
	SATInstance si(&ci);
	cout << " #Variable : " << si.variableCount << endl;
	cout << " #Clause : " << si.clauseCount << endl;
	cout << "remove trivial clauses and variables" << endl;
	si.simplify();
	cout << " #Variable : " << si.variableCount << endl;
	cout << " #Clause : " << si.clauseCount << endl;
	int answer = -si.clauseCount;
	si.calcAnswerBits();
	//	si.calcSubAnswerBits();

	cout << "generate CNF-MAX-2-SAT" << endl;
	MaxSATInstance mi(&si);
	cout << " #Variable : " << mi.variableCount << endl;
	cout << " #Clause : " << mi.clauseCount << endl;
	cout << " #Satisfiable : " << mi.maxSatisfied << endl;

	cout << "generate Quadratic Unconstrained Binary Optimization" << endl;
	QUBOInstance qi(&mi);
	cout << " #Variable : " << qi.variableCount << endl;
	cout << " #Clause : " << qi.clauseCount << endl;
	cout << " #MAXQUBO : " << qi.maxQUBO << endl;

	cout << "generate MAXCUT" << endl;
	MAXCUTInstance mci(&qi);
	cout << " #Variable : " << mci.variableCount << endl;
	cout << " #Clause : " << mci.clauseCount << endl;
	cout << " #MAXCUT : " << mci.maxcutValue << endl;

	cout << "generate Hamiltonian Minimum Eigenvalue " << endl;
	//	HamiltonianInstance hi(&si);
	HamiltonianInstance hi(&mci);
	cout << " #Variable : " << hi.variableCount << endl;
	cout << " #Clause : " << hi.clauseCount << endl;
	cout << " #Minimum : " << hi.minimumEnergy << endl;
	cout << " Answer = ";
	for (int i = 0; i < hi.answerBits.size(); i++) {
		cout << (hi.answerBits[i] == 1 ? "+" : "-");
	}
	cout << endl;

	cout << "output result to file" << endl;
	ci.outputCircuit("_circuit.txt");
	ci.outputLabel("_label.txt");
	si.outputCNF("_sat.txt");
	si.outputVariable("_var.txt");
	mci.outputGraph("_graph.txt");
	hi.outputClause("_ham.txt");

	cout << "start classical anealing" << endl;
	ClassicalAnealer ca;
	ca.answer = hi.minimumEnergy;
	ca.load("_ham.txt");
	ca.aneal();
	ca.output("_ca.txt");

	cout << "start quantum anealing" << endl;
	QuantumAnealer qa;
	qa.answer = hi.minimumEnergy;
	qa.imaginaryStepCount = 20;
	qa.load("_ham.txt");
	qa.aneal();
	ca.output("_qa.txt");

	ofstream ofs("_result.txt", ios::trunc);
	ofs << "composite " << hi.composite << endl;
	ofs << "prime1 " << hi.answer << endl;
	ofs << "prime2 " << hi.composite / hi.answer << endl;
	ofs << "#clause " << hi.clauseCount << endl;
	ofs << "#variable " << hi.variableCount << endl;
	ofs << "minimumEnergy " << hi.minimumEnergy << endl;
	ofs << "SA_Answer " << ca.minimumEnergy << endl;
	ofs << "QA_Answer " << qa.minimumEnergy << endl;

	ofs << "SA_Step " << ca.step << endl;
	ofs << "QA_Step " << qa.step << endl;

	ofs << "correctSpins ";
	for (int i = 0; i < hi.answerBits.size(); i++) ofs << ((hi.answerBits[i] == 1) ? "+" : "-");
	ofs << endl;
	ofs << "SA_Spins     ";
	for (int i = 0; i < ca.minimumSites.size(); i++) ofs << ((ca.minimumSites[i] == 1) ? "+" : "-");
	ofs << endl;
	ofs << "QA_Spins     ";
	for (int i = 0; i < qa.minimumSites.size(); i++) ofs << ((qa.minimumSites[i] == 1) ? "+" : "-");
	ofs << endl;
	ofs.close();
}
int main(){
	exec();
	return 0;
}


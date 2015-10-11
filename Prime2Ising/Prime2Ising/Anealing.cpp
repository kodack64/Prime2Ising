
#include "Anealing.h"
#include <conio.h>

Anealer::Anealer()
:rand(0, 1){
	random_device rd;
	mtgen.seed(rd());
	dt = 0.0001;
}

void Anealer::load(string name){
	ifstream ifs(name);
	ifs >> variableCount >> clauseCount;
	for (uint i = 0; i < clauseCount; i++){
		double coef;
		uint termCount;
		ifs >> coef >> termCount;
		vector<int> clause;
		for (uint j = 0; j < termCount; j++){
			int t;
			ifs >> t;
			clause.push_back(t);
		}
		clauses.push_back(make_pair(coef,clause));
	}
	for (uint i = 0; i < variableCount; i++){
		Relation rel;
		for (uint j = 0; j < clauses.size(); j++){
			if (find(clauses[j].second.begin(), clauses[j].second.end(), i) != clauses[j].second.end()){
				Interact hin;
				hin.first = clauses[j].first;
				for (uint k = 0; k < clauses[j].second.size(); k++){
					if (clauses[j].second[k] != i){
						hin.second.push_back(clauses[j].second[k]);
					}
				}
				rel.push_back(hin);
			}
		}
		relations.push_back(rel);
	}
}

void Anealer::consoleOutput(){
	cout << "step=" << step+1 << " energy=" << currentEnergy << " rest=" << currentEnergy-answer<< endl;
/*	for (uint i = 0; i <variableCount; i++){
		cout << (sites[i] == 1 ? "+" : "-");
	}
	cout << endl;*/
}

void Anealer::checkMinimum(){
	double mene;
	long mset = 0;
	for (long i = 0; i < (1 << variableCount); i++){
		double ene = 0;
		for (uint j = 0; j < clauses.size(); j++){
			double tene = clauses[j].first;
			for (uint k = 0; k < clauses[j].second.size(); k++){
				tene *= (i >> (clauses[j].second[k])) % 2 == 1 ? 1.0 : -1.0;
			}
			ene += tene;
		}
		if (i == 0 || ene<mene){
			mene = ene;
			mset = i;
		}
	}
	for (uint i = 0; i < variableCount; i++){
		cout << ((mset >> i) % 2 == 1 ? "+" : "-");
	}
	cout << endl << "ene=" << mene << endl;
}


void ClassicalAnealer::aneal(){
	sites.resize(variableCount);
	for (uint i = 0; i < variableCount; i++){
		sites[i] = rand(mtgen) >0.5 ? 1 : -1;
	}
	double temp;
	maxStep = 10000;
	currentEnergy = minimumEnergy = calcEnergy();
	minimumSites.resize(sites.size());
	consoleOutput();
	for (step = 0; step < maxStep && minimumEnergy>answer+0.5; step++){
		time = step*dt;
		temp = 1.0*exp(-time);
		for (uint i = 0; i < sites.size(); i++){
			double dif = calcEnergyDif(i);
			if (rand(mtgen) < 1.0/(1+exp(dif / temp))){
				sites[i] = -sites[i];
//				cout << "spin" << i << "  dif: " << dif << "  prob:" << 1.0 / (1 + exp(dif / temp)) << " flipped"<< endl;
			}
		}
		currentEnergy = calcEnergy();
		if (currentEnergy < minimumEnergy){
			minimumEnergy = currentEnergy;
			for (uint i = 0; i < sites.size(); i++){
				minimumSites[i] = sites[i];
			}
			consoleOutput();
		}
		energyLog.push_back(currentEnergy);
	}
	cout << "end" << endl;
}

double ClassicalAnealer::calcEnergy(){
	double ene = 0;
	for (uint i = 0; i < clauses.size(); i++){
		double tene = clauses[i].first;
		for (uint j = 0; j < clauses[i].second.size(); j++){
			tene *= sites[clauses[i].second[j]];
		}
		ene += tene;
	}
	return ene;
}
double ClassicalAnealer::calcEnergyDif(int ind){
	double ene = 0;
	for (uint i = 0; i < relations[ind].size(); i++){
		double tene = relations[ind][i].first;
		for (uint j = 0; j < relations[ind][i].second.size(); j++){
			tene *= sites[relations[ind][i].second[j]];
		}
		ene += -2 * sites[ind] * tene;
	}
	return ene;
}


void QuantumAnealer::aneal(){
	sites.resize(variableCount*imaginaryStepCount);
	for (uint i = 0; i < sites.size(); i++){
		sites[i] = rand(mtgen)>0.5 ? 1 : -1;
	}
	maxStep = 10000;
	currentEnergy = minimumEnergy = calcEnergy();
	minimumSites.resize(variableCount);
	consoleOutput();
	for (step = 0; step < maxStep && minimumEnergy>answer+0.5; step++){
		time = step*dt;
		for (uint i = 0; i < sites.size(); i++){
			double dif = calcEnergyDif(i);
			if (rand(mtgen) < 1.0 / (1 + exp(dif / 1.0))){
				sites[i] = -sites[i];
//				cout << "spin" << i << "  dif: " << dif << "  prob:" << 1.0 / (1 + exp(dif / temp)) << " flipped"<< endl;
			}
		}
		currentEnergy = calcEnergy();
		if (currentEnergy < minimumEnergy){
			minimumEnergy = currentEnergy;
			for (uint i = 0; i < variableCount; i++){
				minimumSites[i] = sites[i];
			}
			consoleOutput();
		}
		energyLog.push_back(currentEnergy);
	}
	cout << "end" << endl;
}

double QuantumAnealer::calcEnergy(){
	double minene = 0;
//	for (uint k = 0; k < imaginaryStepCount; k++){
	for (uint k = 0; k <=0; k++){

		double ene = 0;
		for (uint i = 0; i < clauses.size(); i++){
			double tene = clauses[i].first;
			for (uint j = 0; j < clauses[i].second.size(); j++){
				tene *= sites[clauses[i].second[j]+k*variableCount];
			}
			ene += tene;
		}

		if (k == 0) minene = ene;
		else minene = min(ene, minene);
	}
	return minene;
}
double QuantumAnealer::calcEnergyDif(int ind){
	double ene = 0;
	uint me = ind%variableCount;
	uint im = ind / variableCount;
	for (uint i = 0; i < relations[me].size(); i++){
		double tene = relations[me][i].first;
		for (uint j = 0; j < relations[me][i].second.size(); j++){
			tene *= sites[relations[me][i].second[j]];
		}
		ene += -2*sites[ind] * tene;
	}

	double temp = 1.0;
//	double itemp = -(signed)imaginaryStepCount*temp / 2.0 * log(tanh(exp(-time)/imaginaryStepCount/temp));
	double itemp = exp(-time);
	if(im>0)					ene += -2*itemp*sites[me + im*variableCount] * sites[me + (im - 1)*variableCount];
	if(im+1<imaginaryStepCount)	ene += -2*itemp*sites[me + im*variableCount] * sites[me + (im + 1)*variableCount];

	return ene;
}

void Anealer::output(string filename) {
	ofstream ofs(filename, ios::trunc);
	for (int i = 0; i < energyLog.size(); i++) {
		ofs << i << " " << energyLog[i] << " " << energyLog[i]-answer << endl;
	}
	ofs.close();
}
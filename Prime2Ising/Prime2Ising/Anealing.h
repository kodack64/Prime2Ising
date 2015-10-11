
#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <random>

using namespace std;
typedef pair<double, vector<int>> AniClause;
typedef vector<AniClause> AniClauses;
typedef pair<double, vector<int>> Interact;
typedef vector<Interact> Relation;
typedef vector<Relation> Relations;
typedef unsigned int uint;

class Anealer{
public:
	Anealer();
	virtual void load(string filename);
	uint variableCount;
	uint clauseCount;
	uint maxStep;
	double answer;
	virtual void aneal() = 0;
	virtual void checkMinimum();
	virtual void consoleOutput();
	virtual void output(string filename);
	double  minimumEnergy;
	vector<int> minimumSites;
	uint step;
protected:
	mt19937 mtgen;
	uniform_real_distribution<double> rand;
	AniClauses clauses;
	Relations relations;
	vector<int> sites;
	double currentEnergy;
	double dt;
	double time;
	virtual double calcEnergy()=0;
	virtual double calcEnergyDif(int ind) = 0;
	vector<double> energyLog;
};
class ClassicalAnealer : public Anealer{
public:
	virtual void aneal();
private:
	virtual double calcEnergy();
	virtual double calcEnergyDif(int ind);
};

class QuantumAnealer : public Anealer{
public:
	virtual void aneal();
	uint imaginaryStepCount = 10;
private:
	virtual double calcEnergy();
	virtual double calcEnergyDif(int ind);
	int siteNum;
};

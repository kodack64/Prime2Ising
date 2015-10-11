

#pragma once

#include <vector>
#include <cmath>
#include <algorithm>
#include <random>
#include <iostream>

using namespace std;

class FactoringInstance{
public:
	int composite;
	int bitSize;
	int answer;
	FactoringInstance(int minPrime,int maxPrime);
	static vector<int> factoring(int number);
	static bool checkPrime(int number);
	static vector<int> primeList(int minNumber,int maxNumber);
	static pair<int,int> getRandomPrimePair(int minPrime,int maxPrime);
	static int getRandomComposite(int minFactorNumber,int maxFactorNumber);

};

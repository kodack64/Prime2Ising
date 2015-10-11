

#include "Factoring.h"

FactoringInstance::FactoringInstance(int minPrime,int maxPrime){
	// get 2 prime and composite number
	auto pair = getRandomPrimePair(minPrime, maxPrime);
	answer = pair.first;
	composite = answer*pair.second;
	bitSize = (int)ceil(log((double)composite+1)/log(2.0));
}

vector<int> FactoringInstance::factoring(int number){

	int range = (int)sqrt((double)(number))+1;
	vector<int> factors;

	bool* box = new bool[range];
	for(int i=0;i<range;i++)box[i]=true;
	for(int ind=2;ind<range;ind++){
		if(box[ind]){

			while(number%ind==0){
				factors.push_back(ind);
				number/=ind;
			}

			for(int t=2;t*ind<range;t++){
				box[t*ind]=false;
			}
		}
		if(number==1)break;
	}
	if(number!=1)factors.push_back(number);
	delete[] box;
	return factors;
}

bool FactoringInstance::checkPrime(int number){
	if(number%2==0)return true;
	for(int i=3;i<(int)sqrt((double)number)+1;i+=2){
		if(number%i==0)return true;
	}
	return false;
}
vector<int> FactoringInstance::primeList(int minNumber,int maxNumber){
	int range = maxNumber+1;
	vector<int> primes;
	bool* box = new bool[range];
	for(int i=0;i<range;i++)box[i]=true;
	for(int ind=2;ind<range;ind++){
		if(box[ind]){
			if(ind>=minNumber)primes.push_back(ind);
			for(int t=2;t*ind<range;t++){
				box[t*ind]=false;
			}
		}
	}
	delete[] box;
	return primes;
}
pair<int,int> FactoringInstance::getRandomPrimePair(int minPrime,int maxPrime){
	vector<int> list = primeList(minPrime,maxPrime);
	random_device r;
	mt19937 mt(r());
	if(list.size()==0){
		cout << "There is no prime in range" << endl;
		return make_pair(2,2);
	}
	return make_pair(list[mt()%list.size()],list[mt()%list.size()]);
}
int FactoringInstance::getRandomComposite(int minFactorNumber,int maxFactorNumber){
	vector<int> list = primeList(minFactorNumber,maxFactorNumber);
	if(list.size()==0){
		cout << "There is no prime in range" << endl;
		return 2;
	}
	random_device myRandomGen;
	return list[myRandomGen()%list.size()]*list[myRandomGen()%list.size()];
}

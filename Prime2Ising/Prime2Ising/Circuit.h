
#pragma once

#include <vector>
#include <cmath>
#include <string>
#include <map>
#include <algorithm>
#include <iostream>

class FactoringInstance;

using namespace std;

class CircuitNode{
private:
	string state;
	int id1;
	int id2;
public:
	static int maxid;
	CircuitNode()
	:id1(-1),id2(-1),state(""){}
	CircuitNode(string _state, int _id1,int _id2)
	:state(_state),id1(_id1),id2(_id2){}
	bool isNull(){return id1==-1;}


	// for sort in debug solve, force names of input bits are ordered by number (9,10,11... -> 09,10,11...)
	static string myToString(int val){
		unsigned int ind = 1+(unsigned int)floor(log((double)maxid)/log(10.0));
		string str = to_string(val);
		while(str.length()<ind)str=string("0")+str;
		return str;
	}
	string toString(){
		return state+"_"+myToString(id1)+"_"+myToString(id2);
	}
};

enum ElementType{
	AND,
	XOR,
	FULL_ADDER,
	HALF_ADDER
};

// super class of circuit element
class CircuitElement{
public:
	ElementType type;
	vector<CircuitNode> inputNodes;
	vector<CircuitNode> outputNodes;
	int getInputNumber(){return inputNodes.size();};
	int getOutputNumber(){return outputNodes.size();};
	virtual string toString()=0;
	virtual vector<string> getNodeList()=0;
};

// subclasses
class C_2in1out: public CircuitElement{
public:
	C_2in1out(CircuitNode in1,CircuitNode in2,CircuitNode out1) : CircuitElement(){
		inputNodes.push_back(in1);inputNodes.push_back(in2);
		outputNodes.push_back(out1);
	}
	vector<string> getNodeList(){
		vector<string> s;
		s.push_back(inputNodes[0].toString());
		s.push_back(inputNodes[1].toString());
		s.push_back(outputNodes[0].toString());
		return s;
	}
};
class C_AND : public C_2in1out{
public:
	C_AND(CircuitNode in1,CircuitNode in2,CircuitNode out1):C_2in1out(in1,in2,out1){
		type=AND;
	}
	string toString(){
		return outputNodes[0].toString()+" <- "+inputNodes[0].toString()+" * "+inputNodes[1].toString();
	}
};
class C_XOR : public C_2in1out{
public:
	C_XOR(CircuitNode in1,CircuitNode in2,CircuitNode out1):C_2in1out(in1,in2,out1){
		type=XOR;
	}
	string toString(){
		return outputNodes[0].toString()+" <- "+inputNodes[0].toString()+" ^ "+inputNodes[1].toString();
	}
};
class C_2in2out: public CircuitElement{
protected:
	C_2in2out(CircuitNode in1,CircuitNode in2,CircuitNode out1,CircuitNode out2) : CircuitElement(){
		inputNodes.push_back(in1);inputNodes.push_back(in2);
		outputNodes.push_back(out1);outputNodes.push_back(out2);
	}
	vector<string> getNodeList(){
		vector<string> s;
		s.push_back(inputNodes[0].toString());
		s.push_back(inputNodes[1].toString());
		s.push_back(outputNodes[0].toString());
		s.push_back(outputNodes[1].toString());
		return s;
	}
};
class C_HALF_ADDER : public C_2in2out{
public:
	C_HALF_ADDER(CircuitNode in1,CircuitNode in2,CircuitNode out1,CircuitNode out2):C_2in2out(in1,in2,out1,out2){
		type=HALF_ADDER;
	}
	string toString(){
		return string("(")+outputNodes[0].toString()+","+outputNodes[1].toString()+") <- "+inputNodes[0].toString()+" + "+inputNodes[1].toString();
	}
};
class C_3in2out: public CircuitElement{
protected:
	C_3in2out(CircuitNode in1,CircuitNode in2,CircuitNode in3,CircuitNode out1,CircuitNode out2) : CircuitElement(){
		inputNodes.push_back(in1);inputNodes.push_back(in2);inputNodes.push_back(in3);
		outputNodes.push_back(out1);outputNodes.push_back(out2);
	}
	vector<string> getNodeList(){
		vector<string> s;
		s.push_back(inputNodes[0].toString());
		s.push_back(inputNodes[1].toString());
		s.push_back(inputNodes[2].toString());
		s.push_back(outputNodes[0].toString());
		s.push_back(outputNodes[1].toString());
		return s;
	}
};
class C_FULL_ADDER : public C_3in2out{
public:
	C_FULL_ADDER(CircuitNode in1,CircuitNode in2,CircuitNode in3,CircuitNode out1,CircuitNode out2):C_3in2out(in1,in2,in3,out1,out2){
		type=FULL_ADDER;
	}
	string toString(){
		return string("(")+outputNodes[0].toString()+","+outputNodes[1].toString()+") <- "+inputNodes[0].toString()+" + "+inputNodes[1].toString()+" + "+inputNodes[2].toString();
	}
};



class CircuitInstance{
public:
	int bitSize;
	int nodeCount;
	vector<string> nodeList;

	string st_input;
	string st_output;
	string st_matrix;
	string st_current;
	string st_increment;

	int correctInput1;
	int correctInput2;
	int correctOutput;
	vector<CircuitElement*> elements;
	CircuitInstance(FactoringInstance* fi);
	virtual ~CircuitInstance(){
		for(unsigned int i=0;i<elements.size();i++)delete elements[i];
	}
	virtual void outputCircuit(string file);
	virtual void outputLabel(string file);
};



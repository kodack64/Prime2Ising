
#include "Circuit.h"
#include "Factoring.h"

#include <fstream>

int CircuitNode::maxid=0;

CircuitInstance::CircuitInstance(FactoringInstance* fi){

	elements.clear();

	// prefix of NodeName
	// *** for debug solve, st_input must be alphabetically first
	st_input = "___in";
	st_output = "___out";
	st_matrix = "__mat";
	st_current = "_c";
	st_increment = "_cinc";

	bitSize=fi->bitSize;
	if(bitSize<=1){
		cout << "Circuit size must be larger than 1" << endl;
		return;
	}
	CircuitNode::maxid = 2*bitSize-1;

	// input bit is (bit of N - 1) for eliminating trivial answer N = 1 * N
	int bits= bitSize-1;

	// (mat_i_i+j = in1_j and in2_i)  mat_i_j ‚ÍŠ|‚¯ŽZ‚Ì•MŽZ‚ð‘‚¢‚½Žž‚Ìisj—ñ–Ú
	// there is no mat_0_0 because (mat_0_0=out_0_0)
	for(int i=0;i<bits;i++){
		for(int j=0;j<bits;j++){
			if(i+j==0)
				elements.push_back(new C_AND(CircuitNode(st_input,1,i),CircuitNode(st_input,0,j),CircuitNode(st_output,0,0)));
			else
				elements.push_back(new C_AND(CircuitNode(st_input,1,i),CircuitNode(st_input,0,j),CircuitNode(st_matrix,i,i+j)));
		}
	}

	for(int d=1;d<bits;d++){
		// if bitSize=2
		if(d==1 && d+1==bits){
			for(int i=d;i<bits+d;i++){
				if(i==d)
					elements.push_back(new C_HALF_ADDER(CircuitNode(st_matrix,d-1,i),CircuitNode(st_matrix,d,i),CircuitNode(st_output,0,d),CircuitNode(st_increment,d,i+1)));
				else if(i+1==bits+d)
					elements.push_back(new C_HALF_ADDER(CircuitNode(st_matrix,d,i),CircuitNode(st_increment,d,i),CircuitNode(st_output,0,i),CircuitNode(st_output,0,i+1)));
				else
					elements.push_back(new C_FULL_ADDER(CircuitNode(st_matrix,d-1,i),CircuitNode(st_matrix,d,i),CircuitNode(st_increment,d,i),CircuitNode(st_output,0,i),CircuitNode(st_increment,d,i+1)));
			}
		}

		// first line
		else if(d==1){
			for(int i=d;i<bits+d;i++){
				if(i==d)
					elements.push_back(new C_HALF_ADDER(CircuitNode(st_matrix,d-1,i),CircuitNode(st_matrix,d,i),CircuitNode(st_output,0,d),CircuitNode(st_increment,d,i+1)));
				else if(i+1==bits+d)
					elements.push_back(new C_HALF_ADDER(CircuitNode(st_matrix,d,i),CircuitNode(st_increment,d,i),CircuitNode(st_current,d,i),CircuitNode(st_increment,d,i+1)));
				else
					elements.push_back(new C_FULL_ADDER(CircuitNode(st_matrix,d-1,i),CircuitNode(st_matrix,d,i),CircuitNode(st_increment,d,i),CircuitNode(st_current,d,i),CircuitNode(st_increment,d,i+1)));
			}
		}
		// last line
		else if(d+1==bits){
			for(int i=d;i<bits+d;i++){
				if(i==d) //Å‰‚Ì—ñ‚Íd-1s‚Ü‚Å‚Ì˜a‚Ì’l‚ÆAds‚Ì’l‚Ì”¼‰ÁŽZ
					elements.push_back(new C_HALF_ADDER(CircuitNode(st_current,d-1,i),CircuitNode(st_matrix,d,i),CircuitNode(st_output,0,d),CircuitNode(st_increment,d,i+1)));
				else if(i+1==bits+d) //ÅŒã‚Ì—ñ‚Íd-1s‚Ü‚Å‚ÌÅŒã‚ÌŒJ‚èã‚ª‚è‚ÆAds–Ú‚Ì’l‚ÆA‘O—ñ‚©‚ç‚ÌŒJ‚èã‚ª‚è‚Ì‘S‰ÁŽZ
					elements.push_back(new C_FULL_ADDER(CircuitNode(st_increment,d-1,i),CircuitNode(st_matrix,d,i),CircuitNode(st_increment,d,i),CircuitNode(st_output,0,i),CircuitNode(st_output,0,i+1)));
				else // Å‰‚ÆÅŒãˆÈŠO‚Ì—ñ‚Íd-1s‚Ü‚Å‚Ì˜a‚Ì’l‚ÆAds–Ú‚Ì’l‚ÆA‘O—ñ‚©‚ç‚ÌŒJ‚èã‚ª‚è‚Ì‘S‰ÁŽZ
					elements.push_back(new C_FULL_ADDER(CircuitNode(st_current,d-1,i),CircuitNode(st_matrix,d,i),CircuitNode(st_increment,d,i),CircuitNode(st_output,0,i),CircuitNode(st_increment,d,i+1)));
			}
		}
		// other lines
		// c_d_i is i-th bit of (sum of mat_a_b (a<d)) , there is no c_d_d because "c_d_d = out_d" , there is node c_0_i because (c_0_i=mat_0_i)
		// cinc_d_i carry from i-1 th digit , there is no c_d_(i+bits) because "c_d_(i+bits)=cinc_d_(i+bits)"
		else{
			for(int i=d;i<bits+d;i++){
				if(i==d) //first column is half_adder of current,mat -> out,carry
					elements.push_back(new C_HALF_ADDER(CircuitNode(st_current,d-1,i),CircuitNode(st_matrix,d,i),CircuitNode(st_output,0,d),CircuitNode(st_increment,d,i+1)));
				else if(i+1==bits+d) //last column is full_adder of carry,mat,carry -> current,carry
					elements.push_back(new C_FULL_ADDER(CircuitNode(st_increment,d-1,i),CircuitNode(st_matrix,d,i),CircuitNode(st_increment,d,i),CircuitNode(st_current,d,i),CircuitNode(st_increment,d,i+1)));
				else // other columns are full_adder of current,mat,carry -> current,carry
					elements.push_back(new C_FULL_ADDER(CircuitNode(st_current,d-1,i),CircuitNode(st_matrix,d,i),CircuitNode(st_increment,d,i),CircuitNode(st_current,d,i),CircuitNode(st_increment,d,i+1)));
			}
		}
	}

	// count unique number of nodes
	nodeList.clear();
	for(unsigned int i=0;i<elements.size();i++){
		auto nodes = elements[i]->getNodeList();
		nodeList.insert(nodeList.end(),nodes.begin(),nodes.end());
	}
	sort(nodeList.begin(),nodeList.end());
	nodeList.erase(unique(nodeList.begin(),nodeList.end()),nodeList.end());
	nodeCount = nodeList.size();


	correctInput1 = fi->answer;
	correctInput2 = fi->composite/fi->answer;
	correctOutput = fi->composite;

}

void CircuitInstance::outputCircuit(string file){
	fstream fs(file,ios::out);
	for(unsigned int i=0;i<elements.size();i++){
		fs << elements[i]->toString() << endl;
	}
	fs.close();
}

void CircuitInstance::outputLabel(string file){
	fstream fs(file,ios::out);
	for(unsigned int i=0;i<nodeList.size();i++){
		fs << i+1 << " " << nodeList[i] << endl;
	}
	fs.close();
}
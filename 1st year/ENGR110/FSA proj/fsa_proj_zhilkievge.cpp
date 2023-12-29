#include <fstream> 
#include <iostream> 
#include <sstream> 
#include <algorithm>
#include <vector>
#include <functional>
#include <string>

std::vector<std::string> code;
std::string line;

bool isError = false;

struct Transition{
  std::string input_signal;
  std::string dest_state;
};
struct State{
  std::string name;
  std::vector<Transition> transitions;
};
struct FSM_out{
   std::vector<State> states;
   std::string accepting_name;
   std::string initial_name;
   State state_in_progress;
};
FSM_out fsm;
Transition curTrans;
//state declaration
void root(char input);
void error(char input);

void accept_0(char input);
void init_0(char input);
void state_0(char input);

void accept_1(char input);
void init_1(char input);
void state_1(char input);

void name_0(char input);
void trans_0(char input);

void name_1(char input);
void trans_1(char input);

void in_0(char input);
void dest_0(char input);

void in_1(char input);
void dest_1(char input);

std::function <void(char)> cur_state;

void root(char input) {
	if (input == 'a') {cur_state = accept_0;}
	else if (input == 'i'){cur_state = init_0;}
	else if (input == 's') {cur_state = state_0;}
	else {cur_state = error;
		std::cout<<"Error did not find 'a', 'i' or 's'!"<<std::endl;
		isError = true;}
}
void error(char input) {cur_state = error;}

/**
 * 1st layer
 **/
void accept_0(char input) {
	if (input == '{') {cur_state = accept_1;
		fsm.accepting_name = "";}
	else {cur_state = error;
		std::cout<<"Error 'a' is not followed by '{'!"<<std::endl;
		isError = true;}
}
void init_0(char input){
	if (input == '{') {cur_state = init_1;
		fsm.initial_name = "";}
	else {cur_state = error;
		std::cout<<"Error 'i' is not followed by '{'!"<<std::endl;
		isError = true;}
}
void state_0(char input){
	if (input == '{') {cur_state = state_1;}
	else {cur_state = error;
		std::cout<<"Error 's' is not followed by '{'!"<<std::endl;
		isError = true;}
}

/**
 * 2nd layer
 **/
void accept_1(char input) {
	if (input == '}') {
		cur_state = root;
		}
	else {
		fsm.accepting_name += input;
	}
}
void init_1(char input){
	if (input == '}') {
		cur_state = root;
		}
	else {
		fsm.initial_name += input;
	}
}
void state_1(char input){
	if (input == '}') {
		cur_state = root;
		fsm.states.push_back(fsm.state_in_progress);
		fsm.state_in_progress.transitions.clear();
	}
	else if (input == 'n') {cur_state = name_0;}
	else if (input == 't') {cur_state = trans_0;}
	else {cur_state = error;
		std::cout<<"Error did not find 'n', 't' or '}'!"<<std::endl;
		isError = true;}
}
/**
 * 1st state layer
 **/
void name_0(char input){
	if (input == '{') {cur_state = name_1;
		fsm.state_in_progress.name = "";}
	else {cur_state = error;
		std::cout<<"Error 'n' is not followed by '{'!"<<std::endl;
		isError = true;}
}
void trans_0(char input){
	if (input == '{') {cur_state = trans_1;
		curTrans.input_signal = "";
		curTrans.dest_state = "";}
	else {cur_state = error;
		std::cout<<"Error 't' is not followed by '{'!"<<std::endl;
		isError = true;}
}

/**
 * 2nd state layer
 **/
void name_1(char input){
	if (input == '}') {cur_state = state_1;}
	else {
		fsm.state_in_progress.name += input;
	}
}
void trans_1(char input){
	if (input == '}') {
		cur_state = state_1;
		fsm.state_in_progress.transitions.push_back(curTrans);
	}
	else if (input == 'i') {cur_state = in_0;}
	else if (input == 'd') {cur_state = dest_0;}
	else {
		std::cout<<"Error did not find 'i', 'd' or '}'!"<<std::endl;
		cur_state = error;
		isError = true;}
}

/**
 * 1st transition layer
 **/
void in_0(char input){
		
	curTrans.input_signal = "";
	if (input == '{') {cur_state = in_1;}
	else {cur_state = error;
		std::cout<<"Error 'i' is not followed by '{'!"<<std::endl;
		isError = true;}
}
void dest_0(char input){
	curTrans.dest_state = "";
	if (input == '{') {cur_state = dest_1;}
	else {cur_state = error;
		std::cout<<"Error 'd' is not followed by '{'!"<<std::endl;
		isError = true;}
}

/**
 * 2nd transition layer
 **/
void in_1(char input){
	if (input == '}') {cur_state = trans_1;}
	else {     
		curTrans.input_signal += input;
	}
}
void dest_1(char input){
	if (input == '}') {cur_state = trans_1;}
	else {
		curTrans.dest_state += input;
	}
}



// returns whole specification file as one string
// whitespaces and line ending characters are removed
std::string ReadSpecFile(std::string fileName){
  std::cout<<"Reading specifications file "<<fileName<<std::endl;
  std::string out;
  std::ifstream inputFile( fileName );
  std::string line;
  out = "";
  std::string n1;
  while(getline(inputFile,line)) { 
     //std::cout<<"from file="<<line<<std::endl;
     // remove all new line characters;
     line.erase(std::remove(line.begin(), line.end(),'\r'), line.end());
     line.erase(std::remove(line.begin(), line.end(),'\n'), line.end());
     line.erase(std::remove(line.begin(), line.end(),' '), line.end());
     out.append(line);
  }
  //cout<<" input="<< input<<endl;
  return out;
}
int SaveCodeFile(std::string fileName){
  std::ofstream codeOutFile( fileName );
  // write whole code to the file
   for( unsigned int i = 0 ; i < code.size(); i++ ) {
      //cout<<code.at(i)<<endl;
      codeOutFile<<code.at(i)<<std::endl;
    } 
    codeOutFile.close();
    return 0;
}
void code_init_push() {
	code.push_back("#include <iostream>");
	code.push_back("using namespace std;");
	code.push_back("string state;");
	code.push_back("string input;");
	code.push_back("int accept = 0;");
	code.push_back("int Proc(string input) {");
}
int main() {
	cur_state = root;
	std::string fsm_fname;
	getline(std::cin,fsm_fname);
	std::string fsm_string = ReadSpecFile(fsm_fname + ".txt");
	
	for (char c:fsm_string){
		//std::cout<<c<<std::endl;
		cur_state(c);
		if  (isError) {
			std::cout<<"Terminating program!";
			return -1;
		}
	}
	
	code_init_push();
	bool init = true;
	bool hasAccept = false;
	bool hasInit = false;
	for (State s: fsm.states){
		if (s.name == fsm.accepting_name) {hasAccept = true;}
		if (s.name == fsm.initial_name) {hasInit = true;}
		if (init) {
			line = "	if (state ==\"" ;
			init = false;
		}
		else {
			line = "	else if (state ==\"";
		}
		line += s.name + "\") {";
		code.push_back(line);
		line = "";
		for (Transition t : s.transitions){
			bool inStates = false;
			if (t.dest_state  == "" || t.input_signal == "") {
				std::cout<<"Need both a destination and an input!";
				return -1;
			}
			for (State n: fsm.states){
				if (t.dest_state == n.name) {
					inStates = true;
				}
			}
			if (!inStates) {
				std::cout<<"Transition state does not exist!";
				return -1;
			} else {
				bool isSame = false;
				for (Transition t2 : s.transitions){
					if ( t.input_signal == t2.input_signal && t.dest_state !=  t2.dest_state) {isSame = true;}
				}
				if (!isSame) {
					code.push_back("		if ( input==\"" + t.input_signal +"\"){");
					code.push_back("			state=\"" + t.dest_state + "\"; } \n");
				}
			}
		}
		code.push_back("}");
	}
	if (hasAccept) {
		line = "	if (state ==\"" + fsm.accepting_name + "\") { ";
		code.push_back(line);
		code.push_back("		cout<<\"In accepting state! \"<<state<<endl; \n			return 1;}");
		code.push_back("		return 0; }");
	} else { std::cout<<"No accept name does not match any state!";return -1;}
	
	if (hasInit) {
		code.push_back("int main(){");
		code.push_back("	state = \"" + fsm.initial_name + "\";");
		
	}	else  {std::cout<<"No initial state"; return -1;}
	
	code.push_back("	while(1){");
	code.push_back("		cout<<\" state=\"<<state<<\" enter signal\"<<endl;");
	code.push_back("	    cin>>input;");
	code.push_back("		Proc(input);");
	code.push_back("	    cout<<\" new state=\"<<state<<endl; // p");
	code.push_back("	} \n}");
	std::cout<<"filename: ";
	
	getline(std::cin,fsm_fname);
	SaveCodeFile(fsm_fname + ".cpp");
	return 0;
}

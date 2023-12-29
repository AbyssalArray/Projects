#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include <math.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h> 
#include <chrono>
//#include <curses.h>
#include <unistd.h>
#include <iomanip>
#include <vector>
#include <string>

using namespace std;

#define TYPE double

//debugging functions
void stop(std::string message){
	std::cout<<message<<std::endl;
	int d;
	std::cin>>d;
}

void save_vector_to_file(std::string file_name,std::vector<TYPE> v){
	std::ofstream ofs;
    std::cout<<"Saving vector "<<file_name<<" ..."<<std::endl;
    ofs.open(file_name.c_str(), std::ofstream::out);//, std::ofstream::out | std::ofstream::trunc);
	if (ofs.is_open()){
		int count =0 ;
		for(TYPE ve:v){
			ofs<<count<<" "<<ve<<std::endl;
			count++;
		}
		ofs.close();
	}
}

bool string_contains(std::string str, std::string to_find){
	return (str.find(to_find) != str.npos);
}

// some common function not specific for neural nets
// sigmoid activation function
double activation(double x)
{
    return 1.0/(1.0 + exp(-x));
}

// returns derivative of sigmoid activation function
double d_activation(double x){
    return (1.0 - activation(x))*activation(x);
 }  
 

// takes array a of size n and sets
// its elements to random values 
// in min-max range
void Randomize(double* a, int n, double min, double max){
	//cout<<"RAND_MAX="<<RAND_MAX<<endl;
    srand (time(NULL));
    for ( int i = 0 ; i < n ; i++){
        double f = (double)rand() / RAND_MAX;
        a[i]= min + f * (max - min);
    }
}

// prints n elements of array a on the screen
void PrintVector(double* a, int n){
    for ( int i = 0 ; i < n ; i++){
        std::cout<<std::fixed<<std::setprecision(3)<<a[i]<<" ";
    }
    cout<<endl;
}

struct data_point_t{
	std::vector<TYPE> inputs;
	std::vector<TYPE> expected_outputs;
};

struct data_set_t{
  std::vector<data_point_t> data_points;	
};

std::vector<std::string> parse_string_by_char(std::string in, char ch){
	std::vector<std::string> out;
    std::stringstream s_stream(in); 
    while(s_stream.good()) {
      std::string substr;
      getline(s_stream, substr, ch); //get first string delimited by char
      if ((substr.size() > 0)){
          out.push_back(substr);
	  }
    }
    return out;
}

data_set_t read_train_file(std::string file_name,int n_points){
	
	data_set_t ts;
    std::ifstream ifs(file_name);
    std::cout<<"Loading training set "<<file_name<<" ..."<<std::endl;
    std::string in_line; 
    data_point_t tp;
    if (ifs.is_open()) {
		int line_count = 0;
	    while(getline(ifs,in_line)){
	       std::stringstream ss(in_line); //read one line
	       tp.expected_outputs.clear();
           for (int i = 0; i <10; i++) tp.expected_outputs.push_back(0.0);
           tp.inputs.clear(); 
		    // parse comma-separated string
		   for (int i = 0, count = 0 ; ss >> i; count++) {
               if (count == 0) { // first is expected answer
		          tp.expected_outputs[i] = 1.0;	   
		       } else { // inputs normalize input to 0..1
			      tp.inputs.push_back(((TYPE)i/255.0 - 0.137)/0.3081);
		       }
               if (ss.peek() == ',') ss.ignore(); //comma - do nothing
 	        }   
            ts.data_points.push_back(tp);
            line_count++;
            if ( (n_points != -1) && (line_count>=n_points)){
				 break;
			 }     
        }  // while line in the file
	}  else { 
		  std::cout<<" Can not open "<<file_name<<" file"<<std::endl;
   }
   std::cout<<ts.data_points.size()<<" training point loaded"<<std::endl;
   ifs.close();   
   return ts;

}

void draw_inputs(std::vector<TYPE> in){
		int count = 0 ;
		for ( int i = 0 ; i< 28;i++){
			for ( int j = 0 ; j <28;j++){
				if (in.at(count)>0.5) std::cout<<"X"; else std::cout<<"_";
				count++;
			}
			std::cout<<std::endl;
		}
		std::cout<<"***************"<<std::endl;
}

void display_train_entry(data_set_t ds){
	uint iImage = 0;
	cout<<" There are "<< ds.data_points.size()<<" entries "<<endl;
	cout<<" Enter number of the entry to display"<<endl;
	cin>>iImage;
	while (iImage < ds.data_points.size()) {
      draw_inputs(ds.data_points[iImage].inputs);
      cin>>iImage;
   }
}

// class contains variables and functions
// of the neural net
class NN{
	private:
	  int nInputs; // number of inputs
	  int nOutputs; //number of outputs
	  int nHiddenNeurons; // number of neurons in hidden layer
	 
	  // search parameters
	  double dw;   // bias and weights step for gradient estimation
	  double learningRate; // hmm, it is learning  rate
	  int nStepsMax;
	  
	  // whole training set 
	  double* inputsTraining;   // inputs - column, pattern - row 
	  double* answersTraining;  // outputs - column, pattern - row
	  int nTrainingEntries;  // number of rows in training set
	  
	  // current NN situation
	  double* currentInputs;  // row picked from inputsTraining
	  double* currentAnswers; // ditto for training answers 
	  double* currentOutputs; // guess what? 
	  double* currentError;   // current differnce between answers and output
	  //output error for current dataset row
	  double sumOfOutputErrors;
	  // sum of errors for all dataset entries
	  double totalDatasetError;
	  
	  // input to hidden layer
	  double* weightsHidden;  // hidden layer weights matrix Wh
	  double* biasHidden;     // hidden layer bias vector bh
	  
	  // state of the hidden layer
	  double* netHidden;     // yh = Wh*z + bh - renaming 
	  double* outHidden;   //  zh = activ(Wh*x + bh) - layer output
	  
      // hidden layer to output layer
      double* weightsOutput;  // Wh
      double* biasOutput;     // bh
 
	  // state of the output layer
      double* nnSumOutput;  
      double* netOutput;  // yo = Wo*zh + bo
      double* nnBiaOutput;
	  double* nnOutOutput; // zo = activ(yo)
	  
	  
	public:
	  NN(){ }; // constructor
	  int InitNet(int nInp,int nHid , int nOut);
	  void GetTrainingEntry(data_point_t dp);
	  void ForwardProp();
	  //double GetTotalError(){ return sumOfOutputErrors;};
	  void PrintErr(){ cout<<" Error: "<<sumOfOutputErrors<<endl;};
	  double GetOutputError(){ return sumOfOutputErrors;};
	  double TotalDatasetError(data_set_t ds); // sum of errors for all rows of train data
	  void PrintOutputs();
      void DisplayResults(data_set_t ds);	
      void load_neurons(std::string);
      int index_of_max_output();
};


// reserves the memory and puts
//random values (inside min-max range) into weights  and biases
int NN::InitNet(int nInp,int nHid , int nOut){
	nInputs = nInp;
	nHiddenNeurons = nHid;
	nOutputs = nOut;
	cout<<" InitNet: nInputs="<<nInputs<<" nHiddenNeurons=";
	cout<<nHiddenNeurons<<" nOutputs="<<nOutputs<<endl; 
	// reserve the memory for weights and biases
	// hidden layer
	weightsHidden = new double[nHiddenNeurons*nInputs];
	biasHidden = new double[nHiddenNeurons];
	// output layer
	weightsOutput = new double[nHiddenNeurons*nOutputs];
	biasOutput = new double[nOutputs];
	
	// current input and output vector, answers and error	
	currentInputs = new double[nInputs];
	currentOutputs = new double[nOutputs];
	currentAnswers = new double[nOutputs];
	currentError =  new double[nOutputs];
	
	// reserve memory for current net levels
	netHidden = new double[nHiddenNeurons];
	outHidden = new double[nHiddenNeurons];
	netOutput = new double[nOutputs];
	
   
  	return 0;
}

// loads row of dataset into the net for estimation
void NN::GetTrainingEntry(data_point_t dp){
	for ( int i = 0 ; i<nInputs;i++)
	   currentInputs[i] = dp.inputs[i];
	for (int i = 0 ; i < nOutputs;i++)   
	  currentAnswers[i]= dp.expected_outputs[i];
	
}

// direct calculation of forward propagation
// takes input values and calculates the outputs
void NN::ForwardProp(){
	//  inputs ->  hidden layer
	// for each neuron in hidden layer
	for ( int hid = 0 ;hid < nHiddenNeurons ; hid++){
		// combine input values and add bias
        netHidden[hid] = biasHidden[hid]; 
        int n = hid*nInputs;
        for (int inp = 0 ; inp < nInputs ; inp++){
		   netHidden[hid] = netHidden[hid] + currentInputs[inp]* weightsHidden[n+ inp];
	    }
	    outHidden[hid] = activation(netHidden[hid]);
	}	
	sumOfOutputErrors = 0.0;
	
	// for each neuron in output layer
	for ( int out = 0 ; out < nOutputs ; out++){
		// combine hidden layer neuron outputs and add bias 
		netOutput[out] = biasOutput[out];
		int n = out*nHiddenNeurons;
		for (int hid = 0 ; hid < nHiddenNeurons ; hid++){
			netOutput[out] = netOutput[out] + outHidden[hid]* weightsOutput[n+hid];
		}
		currentOutputs[out] = activation(netOutput[out]);
		currentError[out] = currentOutputs[out] - currentAnswers[out];
		sumOfOutputErrors = sumOfOutputErrors + currentError[out]*currentError[out];
	}
}




// calculates combined error for all entries in the dataset
// for current values of weights and biases
double NN::TotalDatasetError(data_set_t ds){ // sum of errors for all rows of train data
	totalDatasetError = 0.0;
	for ( uint entry = 0 ; entry < ds.data_points.size(); entry++){
		GetTrainingEntry(ds.data_points[entry]);
	    ForwardProp();
	    totalDatasetError = totalDatasetError + GetOutputError();
	}
	return totalDatasetError/ds.data_points.size();
}


void NN::PrintOutputs(){
	cout<<" Net outputs: ";
	for (int out = 0 ; out < nOutputs ; out++){
		cout<<currentOutputs[out]<<"  ";
	}
	cout<<endl;
}


void NN::DisplayResults(data_set_t ds){
	uint iImage = 0;
	cout<<" There are "<< ds.data_points.size()<<" entries "<<endl;
	cout<<" Enter number of the entry to display"<<endl;
	cin>>iImage;
	while (iImage < ds.data_points.size()) {
	  // copy inputs and outputs from big matrix
	  GetTrainingEntry(ds.data_points[iImage]);
	  ForwardProp();
      draw_inputs(ds.data_points[iImage].inputs);
      PrintVector(currentOutputs, nOutputs);
      cin>>iImage;
   }
    
}

void NN::load_neurons(std::string file_name){
    std::ifstream ifs;
    std::cout<<"Opening neural net into "<<file_name<<" ..."<<std::endl;
    ifs.open(file_name.c_str(), std::ifstream::in);//, std::ofstream::out | std::ofstream::trunc);
	if (ifs.is_open()){
		std::string in_line;
		// number of hidden and output later neurons
		int nH = 0;
		int nO =0;
		while(getline(ifs,in_line)){
			std::vector<std::string> parsed = parse_string_by_char(in_line,',');
			//std::cout<<"at 0"<<parsed[0]<<" lenght="<<parsed.size()<<std::endl;
			nH = nH + string_contains(parsed[0],"hidden");
			nO = nO + string_contains(parsed[0],"output");
		}
		std::cout<<" nH="<<nH<<" nO="<<nO<<std::endl;
		InitNet(28*28,nH,nO);
		// rewind file back
		ifs.clear();
        ifs.seekg(0);
        // read the file, fill biases and weights
        int hid = 0;
        int out = 0;
        while(getline(ifs,in_line)){
			std::vector<std::string> parsed = parse_string_by_char(in_line,',');
			if( string_contains(parsed[0],"hidden") ){
				biasHidden[hid] = stod(parsed[1]);
                for (int inp = 0 ; inp < nInputs ; inp++){
					weightsHidden[hid*nInputs + inp] = stod(parsed[2+inp]);
	           }
	           hid++;
			}
			if ( string_contains(parsed[0],"output") ) {
				biasOutput[out] = stod(parsed[1]);
				for (int hid = 0 ; hid < nHiddenNeurons ; hid++){
			      weightsOutput[out*nHiddenNeurons+hid] = stod(parsed[2+hid]);
		        }
		        out++;
			}
		}
		ifs.close();
		std::cout<<" Net loaded"<<std::endl;
	} else {
		std::cout<<" Can not open "<<file_name<<std::endl;
	}
	

}

int NN::index_of_max_output(){
	TYPE max = 0.0;
	int ind_max = -1;
	for (int i = 0 ; i < nOutputs;i++){
		if (currentOutputs[i]>max){
			max = currentOutputs[i];
			ind_max = i;
		}
	}
	return ind_max;
}


int main(int argc, char **argv){

	NN neuralNet;
    std::string wb_file_name;
    std::cout<<" main() arguments are:\n"<<argc<<std::endl;
    int argcc = argc; 
    while(argcc--)
    {
        std::cout<<"argc="<<argcc<<"    "<<argv[argcc]<<std::endl;
    }
    if (argc == 2){
        wb_file_name = argv[argc-1];
    } else {
	    std::cout<<"Enter file name for biases and weights: ";
	    std::cin>>wb_file_name;
    }

    
    neuralNet.load_neurons(wb_file_name);
    data_set_t test_set=read_train_file("mnist_test.csv",10000);
	
	std::cout<<" Total error="<<neuralNet.TotalDatasetError(test_set)<<std::endl;
	int n_correct = 0;
	for (uint entry = 0 ; entry < test_set.data_points.size() ; entry++) {
	   neuralNet.GetTrainingEntry(test_set.data_points[entry]);
       neuralNet.ForwardProp();
       if (test_set.data_points[entry].expected_outputs[neuralNet.index_of_max_output()]==1){
		   n_correct++;
		 //  std::cout<<"correct"<<std::endl;
	   }
       //stop(" next data point");
    }
	std::cout<<" Rate="<<(double)n_correct/test_set.data_points.size();
	neuralNet.DisplayResults(test_set);
}

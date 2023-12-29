#pragma GCC optimize("Ofast")

#include <iostream>
#include <vector>
#include <array>
#include <algorithm>
#include <math.h>
#include <numeric>
#include <fstream> 
#include <sstream>
#include <chrono>
#include <random>

#define DEBUG 1
#define double double

int lr;
int dev;
//debugging functions
void stop(std::string message){
	std::cout<<message<<std::endl;
	int d;
	std::cin>>d;
}

void save_vector_to_file(std::vector<double> v){
	std::string file_name;
	std::cout<<" Enter file name to save the vector: ";
	std::cin>>file_name;
	std::ofstream ofs;
    std::cout<<"Saving vector "<<file_name<<" ..."<<std::endl;
    ofs.open(file_name.c_str(), std::ofstream::out);//, std::ofstream::out | std::ofstream::trunc);
	if (ofs.is_open()){
		int count =0 ;
		for(double ve:v){
			ofs<<count<<" "<<ve<<std::endl;
			count++;
		}
		ofs.close();
	}
}


double sigmoid(double x){
    return 1.0 / (1.0 + exp(-x));
}

double d_sigmoid(double x){
    return (1.0-sigmoid(x))*sigmoid(x);
}

// one training point - set of input values and set of targets
struct train_point_t{
	std::vector<double> inputs;
	std::vector<double> targets;
};

struct train_set_t{
  std::vector<train_point_t> train_points;	
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



//read train set from the sile
train_set_t read_train_file(std::string file_name,int n_points){
	
	train_set_t ts;
    std::ifstream ifs(file_name);
    std::cout<<"Loading training set "<<file_name<<" ..."<<std::endl;
    std::string in_line; 
    train_point_t tp;
    if (ifs.is_open()) {
		int line_count = 0;
	    while(getline(ifs,in_line)){
	       std::stringstream ss(in_line); //read one line
	       tp.targets.clear();
           for (int i = 0; i <10; i++) tp.targets.push_back(0.0);
           tp.inputs.clear(); 
		    // parse comma-separated string
		   for (int i = 0, count = 0 ; ss >> i; count++) {
               if (count == 0) { // first is expected answer
		          tp.targets[i] = 1.0;	   
		       } else { // inputs normalize input to 0..1
			      tp.inputs.push_back(((double)i/255.0 - 0.137)/0.3081);
		       }
               if (ss.peek() == ',') ss.ignore(); //comma - do nothing
 	        }   
            ts.train_points.push_back(tp);
            line_count++;
            if ( (n_points != -1) && (line_count>=n_points)){
				 break;
			 }     
        }  // while line in the file
	}  else { 
		  std::cout<<" Can not open "<<file_name<<" file"<<std::endl;
   }
   std::cout<<ts.train_points.size()<<" training point loaded"<<std::endl;
   ifs.close();   
   return ts;

}


void print_train_point(train_point_t tp){
	std::cout<<"Training point. Inputs: ";
	for (double inp:tp.inputs) { std::cout<<inp<<" ";};
	//std::cout<<std::endl;
	std::cout<<" Ref Outputs: ";
	for (double out:tp.targets) { std::cout<<out<<" ";};
	std::cout<<std::endl;
}


struct Neuron{
	std::vector<double> weights;   // weights
	std::vector<double> d_weights; // weight derivatives
	double bias;
	double d_bias; // bias derivative
	double z; // before sigmoid
	double y; // current output
	double delta;
    double calculate(const std::vector<double>& inputs);
    void set_neuron(std::vector<double> wi, double bi);
	double error(double t){return (y-t);};
	void print();
};

void Neuron::set_neuron(std::vector<double> wi, double bi){
	weights = wi; //std::transform(wi.begin,wi.end(),w.begin(),[](double in){return out; });  
	bias = bi;
}

double Neuron::calculate(const std::vector<double>& inputs){
	z = std::inner_product(weights.begin(), weights.end(),inputs.begin(),bias);
	y =  sigmoid(z);
	return y;
}

void Neuron::print(){
	std::cout<<"Neuron:\tdelta="<<delta<<std::endl;
	std::cout<<"       \tbias="<<bias<<" d_bias="<<d_bias<<std::endl;
	std::cout<<"       \tWeights: ";
	std::for_each(weights.begin(),weights.end(),[](double w){std::cout<<w<<" ";});
	std::cout<<std::endl;
	std::cout<<"        \tDerivatives: ";
	std::for_each(d_weights.begin(),d_weights.end(),[](double dw){std::cout<<dw<<" ";});
	std::cout<<std::endl;
	std::cout<<"        \tz="<<z<<" y="<<y<<std::endl;
}


struct Net{
	std::vector<Neuron> hidden_layer;
	std::vector<Neuron> output_layer;
	std::vector<double> hidden_outputs;
	std::vector<double> output_errors;
	// net metaparameters
	double in_median;
	double in_deviation;
	double learning_rate;
	int n_batch;
	int n_hidden;
	int n_epoch;  // maximum number of training epochs
	
	double train_set_error;
    std::vector<double> convergence;
	void init(uint n_inputs, uint n_hidden,uint n_outputs);
    void print();
    void forward_prop(const std::vector<double>& inputs);
    double train_point_error(const train_point_t& tp);
    double calc_train_set_error(const train_set_t& ts);
    double element_gradient(double e0, double& e,const train_point_t& tp);
    void calculate_all_gradients(const train_point_t& tp);
    void backprop(const train_point_t& tp);
    void backprop_accumulate(const train_point_t& tp);
    void step_by_gradient(double lr);
    void train(train_set_t ts);
    void print_outputs();
    void display_result(train_set_t ts, int i);
    void save_neurons();
};


// reserve memory
// put random values into vectors for weights and biases
void Net::init( uint n_inputs, uint n_hidden,uint n_outputs){
	
	std::cout<<" Initializing: n_inputs="<<n_inputs<<" n_hidden="<<n_hidden;
	std::cout<<" n_outputs="<<n_outputs<<std::endl;
	//srand (time(NULL));
	std::default_random_engine generator;
    std::normal_distribution<double> distribution(in_median,in_deviation);
	
	hidden_layer.reserve(n_hidden);
	for (uint i = 0 ; i < n_hidden ; i++){
	   Neuron n1;
	   n1.bias = distribution(generator);
	   n1.d_bias = 0.0;
	   n1.delta = 0.0;
	   n1.weights.reserve(n_inputs);
	   n1.d_weights.reserve(n_inputs);
	   for (uint j = 0; j < n_inputs ; j++){
		   n1.weights.emplace_back(distribution(generator));
		   n1.d_weights.emplace_back(0.0);
	   }
	   n1.y = n1.z = 0.0;
	   hidden_layer.emplace_back(n1);
    }
    
    output_layer.reserve(n_outputs);
    output_errors.reserve(n_outputs);
	for (uint i = 0 ; i < n_outputs ; i++){
	   Neuron n;
	   n.bias = distribution(generator);
	   n.d_bias = 0.0;
	   n.delta =0.0;
	   n.weights.reserve(n_hidden);
	   n.d_weights.reserve(n_hidden);
	   for (uint j = 0; j < n_hidden ; j++){
		   n.weights.emplace_back(distribution(generator));
		   n.d_weights.emplace_back(0.0);
	   }
	   n.y = n.z = 0.0;
	   output_layer.emplace_back(n);
	   output_errors.emplace_back( 0.0);
    }
}

void Net::print(){
	std::cout<<"***HIDDEN***"<<std::endl;
	for (uint i = 0 ; i < hidden_layer.size() ; i++){
	   hidden_layer.at(i).print();
    }
    std::cout<<"****OUTPUT******"<<std::endl;
	for (uint i = 0 ; i < output_layer.size() ; i++){
	   output_layer.at(i).print();
    }

}

void Net::forward_prop(const std::vector<double>& inputs){
	
	hidden_outputs.clear();
	for(Neuron& nhl:hidden_layer) { //each neuron in hidden layer (nhl)
		hidden_outputs.emplace_back(nhl.calculate(inputs));
	}
	for(Neuron& nol:output_layer) {
		nol.calculate(hidden_outputs);
    }
}


// 
double Net::train_point_error(const train_point_t& tp){
   double error = 0.0;
   forward_prop(tp.inputs);
   // for all neutons in output layer
   for (uint i=0; i< output_layer.size() ; i++){
	    //double y = output_layer.at(i).y;
	    //double t = tp.targets[i];
	    //std::cout<<" out_size="<<output_layer.size()<<" y="<<y<<" t="<<t<<std::endl;
		double err = output_layer.at(i).y-tp.targets[i];
		error = error + err*err;
	}
	//std::cout<<" error="<<error<<std::endl;
	return error;
}

double Net::calc_train_set_error(const train_set_t& ts){
	//std::cout<<" Calculating total error..."<<std::endl;
	double tse = 0.0;
	for (uint i = 0 ; i < ts.train_points.size() ; i++){
		tse = tse + train_point_error(ts.train_points.at(i));
	}
	return tse/ ts.train_points.size();
}


void Net::print_outputs(){
	std::cout<<" Outputs of the Network: ";
	double max = 0.0;
	int index_max = -1;
    for (uint i=0; i< output_layer.size() ; i++){
       std::cout<<output_layer.at(i).y<<" ";
       if (output_layer.at(i).y>max){ 
		   max = output_layer.at(i).y;
		   index_max = i;
	   }
    }
    std::cout<<" Index of the max. output: "<<index_max<<std::endl; 
    std::cout<<std::endl;    
}

// calculate gradient of one element
double Net::element_gradient(double e0, double& e, const train_point_t& tp){
	double d = 0.0001;
	double temp = 0.0;
	temp = e;
    e = e + d;
    forward_prop(tp.inputs);
  	double e1 = train_point_error(tp);
	e = temp;
	return (e1 - e0)/d;
}

void Net::calculate_all_gradients(const train_point_t& tp){

	forward_prop(tp.inputs);
    double e0 = train_point_error(tp);
	for(Neuron& nhl:hidden_layer){
        nhl.d_bias = element_gradient(e0, nhl.bias,tp);
	    int count = 0;
	    for (double& w:nhl.weights){
			nhl.d_weights.at(count) = element_gradient(e0, w,tp);
			count++;
	    }
	}
    for(Neuron& nol:output_layer){
	  	nol.d_bias = element_gradient(e0, nol.bias,tp);
		//forward_prop(tp.inputs);
	    int count = 0;
	    for (double& w:nol.weights){
			nol.d_weights.at(count) = element_gradient(e0, w,tp);
			count++;
	    }	
	} // hidden
}

//v1 = v1+sca*v2
void modify_vector(std::vector<double>& v1, const std::vector<double>& v2, double sca){
	
	for (uint i = 0 ; i < v1.size(); i++){
	   v1[i] = v1[i] + sca*v2[i];
	}
}


void Net::backprop(const train_point_t& tp){
	forward_prop(tp.inputs);
    //for (uint i=0; i< output_layer.size() ; i++){
	//	output_errors.at(i) = (output_layer.at(i).y-tp.targets[i]);
	//}
	// for all neurons in output layer - calculate delta
	int count =0;
	for(Neuron& nol:output_layer){
		nol.delta = d_sigmoid(nol.z)*(output_layer.at(count).y-tp.targets[count]);
		nol.d_bias = 2.0*nol.delta;
		int count1 =0;
		for(Neuron& nhl:hidden_layer){
			//if (fabs(nol.delta) >0.005)
			nol.d_weights.at(count1) = 2.0*nol.delta*nhl.y;
			count1++;
		}
		count++;
	}
	
	//for all neurons in hidden layer - calculate delta
	int count_hidden = 0;
	for (Neuron& nhl:hidden_layer){
		// sum of weighted deltas from output layer
		nhl.delta = 0.0;
		//int count1 = 0;
		for(uint i = 0 ; i <  output_layer.size() ; i++){
			nhl.delta = nhl.delta + output_layer.at(i).weights.at(count_hidden)*output_layer.at(i).delta;
		}
		nhl.delta = nhl.delta*d_sigmoid(nhl.z);
		count_hidden++;
	}
	
	// derivaties of output layer
	
	for(Neuron& nol:output_layer){
		nol.d_bias = 2.0*nol.delta;
		int count =0;
		for(Neuron& nhl:hidden_layer){
			//if (fabs(nol.delta) >0.005)
			nol.d_weights.at(count) = 2.0*nol.delta*nhl.y;
			count++;
		}
    }
     
    // deivatives of hidden layer
	for(Neuron& nhl:hidden_layer){
		nhl.d_bias = 2.0 * nhl.delta;
		for ( uint i = 0 ; i < nhl.weights.size(); i++){
			//if (fabs(nhl.delta) >0.005)
			    nhl.d_weights.at(i) = 2.0*nhl.delta*tp.inputs.at(i);
		}
	}
}



void Net::backprop_accumulate(const train_point_t& tp){
	//double numeric = 0.0000001;
	forward_prop(tp.inputs);
    for (uint i=0; i< output_layer.size() ; i++){
		output_errors.at(i) = (output_layer.at(i).y-tp.targets[i]);
	}
	// for all neurons in output layer - calculate delta
	int count = 0;
	for(Neuron& nol:output_layer){
		nol.delta = d_sigmoid(nol.z)*output_errors.at(count);
		count++;
	}
	//for all neurons in hidden layer - calculate delta
	for (Neuron& nhl:hidden_layer){
		// sum of weighted deltas from output layer
		nhl.delta = 0.0 ;
		int count1 = 0;
		for (uint i=0; i< output_layer.size();i++){
			//if (fabs(output_layer.at(i).delta) > numeric)
			nhl.delta = nhl.delta + 
			      output_layer[i].weights.at(count1)*output_layer[i].delta;
			count1++;
		}
		nhl.delta = - nhl.delta*d_sigmoid(nhl.z);
	}
	
	// derivaties of output layer
	for(Neuron& nol:output_layer){
		nol.d_bias = nol.d_bias + 2.0*nol.delta;
		int count =0;
		for(Neuron& nhl:hidden_layer){
			//if (fabs(nhl.delta) > numeric)
		    nol.d_weights[count] = nol.d_weights[count] + 2.0*nol.delta*hidden_outputs[count];
			count++;
		}
    }
    // deivatives of hidden layer
	for(Neuron& nhl:hidden_layer){
		nhl.d_bias = nhl.d_bias + 2.0 * nhl.delta;
		//if (fabs(nhl.delta) > numeric)
        modify_vector(nhl.d_weights,tp.inputs,2.0*nhl.delta);
		//for ( uint i = 0 ; i < nhl.weights.size(); i++){
	  	//	nhl.d_weights.at(i) = nhl.d_weights.at(i) + 2.0*nhl.delta*tp.inputs.at(i);
		//}
	}
}

/*
double adjust_lr(double error){
	if (error>0.5) return 3.0;
	if (error>0.3) return 1.5;
	if (error>0.1) return 1.0;
	if (error>0.05) return 0.5;
	if (error>0.03) return 0.1;
	return 0.005;
}
*/

void Net::step_by_gradient(double lr){
	for(Neuron& nhl:hidden_layer){
		nhl.bias = nhl.bias - lr * nhl.d_bias;
		modify_vector(nhl.weights,nhl.d_weights,-lr);
    }
	for(Neuron& nol:output_layer){
		nol.bias = nol.bias - lr * nol.d_bias;
		modify_vector(nol.weights,nol.d_weights,-lr);
    }
}



void draw_inputs(std::vector<double> in){
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

void Net::display_result(train_set_t ts, int i){
	std::cout<<" Entry "<<i<<" from dataset"<<std::endl;
	draw_inputs(ts.train_points.at(i).inputs);
	std::cout<<" Expected Outputs: ";
	for (double out:ts.train_points.at(i).targets) { std::cout<<out<<" ";};
  	std::cout<<std::endl;
		
	forward_prop(ts.train_points.at(i).inputs);
	print_outputs();
	std::cout<<"**************"<<std::endl;
	//stop(" outputs");
}


void Net::train(train_set_t ts){
	//int timing = 1;
	std::cout<<"Starting training..."<<std::endl;
	int epoch = 0;
	double dataset_error = calc_train_set_error(ts);
	std::cout<<" Before training error="<<dataset_error<<std::endl;
	convergence.push_back(dataset_error);
	
	while ((dataset_error>0.0001)&&(epoch<n_epoch)){
		for (uint i = 0; i < ts.train_points.size() - n_batch ; i = i + n_batch){
		    backprop( ts.train_points.at(i) );
		    for ( int j = 1 ; j < n_batch ; j++){
		      backprop_accumulate( ts.train_points.at(i+j) );
		    }
		   step_by_gradient(learning_rate /(double)n_batch);
	    } 
	    // for display only - training can run without
	    dataset_error = calc_train_set_error(ts);
	    std::cout<<" epoch="<<epoch<<"/"<<n_epoch<<" error="<<dataset_error<<std::endl;
	    convergence.push_back(dataset_error);
	    
	    epoch++;
    } //epoch
    
    //save_vector_to_file(convergence);
}

void Net::save_neurons(){

	std::string file_name = "50_temp/N" + std::to_string(this -> n_hidden) + "wbls" + std::to_string(lr)+ "ds" + std::to_string(dev);
	//std::cout<<" Enter file name for weights and biases: ";
	//std::cin>>file_name;  
    std::ofstream ofs;
    std::cout<<"Saving neural net into "<<file_name<<" ..."<<std::endl;
    ofs.open(file_name.c_str(), std::ofstream::out);//, std::ofstream::out | std::ofstream::trunc);
	if (ofs.is_open()){
		for(Neuron& nhl:hidden_layer){
			ofs<<"hidden,"<<nhl.bias<<',';
			//std::cout<<"h,"<<nhl.bias<<',';
			for(double w:nhl.weights) ofs<<w<<',';
			ofs<<std::endl;
			//std::cout<<std::endl;
		}
		for(Neuron& nol:output_layer){
			ofs<<"output,"<<nol.bias<<',';
			//std::cout<<"output,"<<nol.bias<<',';
			for(double w:nol.weights) ofs<<w<<',';
			ofs<<std::endl;
		}
		ofs.close();
		std::cout<<" Net saved"<<std::endl;
	} else {
		std::cout<<" Can not open "<<file_name<<std::endl;
	}
	
}

bool string_contains(std::string s,std::string to_find){
	return (s.find(to_find) != std::string::npos);
}

// read network metaparameters from file
int read_meta(Net& net,int& n_samples){
	std::ifstream ifs;
	ifs.open("metas.txt");
	if (ifs.is_open()){
		std::string in_line;
		// get all lines
		while(std::getline(ifs,in_line)){
			// parse string if it is not empty
			//std::cout<<"in_line="<<in_line<<"  n="<<in_line.size()<<std::endl;
			if (in_line.size()>0){
			  std::vector<std::string> parsed_line = parse_string_by_char(in_line,'=');
			  //std::cout<<parsed_line[0]<<" "<<parsed_line[1]<<std::endl;
			  if (string_contains(parsed_line[0],"in_median")) net.in_median = stod(parsed_line[1]);
			  if (string_contains(parsed_line[0],"in_deviation")) net.in_deviation = stod(parsed_line[1]);
			  if (string_contains(parsed_line[0],"learning_rate")) net.learning_rate = stod(parsed_line[1]);
			  if (string_contains(parsed_line[0],"n_batch")) net.n_batch = stoi(parsed_line[1]);
			  if (string_contains(parsed_line[0],"n_hidden")) net.n_hidden = stoi(parsed_line[1]);
			  if (string_contains(parsed_line[0],"n_epoch")) net.n_epoch = stoi(parsed_line[1]);
			  if (string_contains(parsed_line[0],"n_samples")) n_samples = stoi(parsed_line[1]);
		    }
		}
		std::cout<<" Metaparameters read"<<std::endl;
		return 0;
	} else {
		std::cout<<" Can not open file with metaparameters"<<std::endl;
		return -1;
	}
}


int main(){
	for (int i = 20; i > 0; i--) {
		for (int j = 20; j > 0; j--) {
			Net net;
			int n_samples = 0;
			lr = i;
			dev = j;
			if (read_meta(net,n_samples) != 0) return -1; 

			train_set_t ts = read_train_file("mnist_train.csv",n_samples);
			net.in_deviation = j * 0.1;
			net.learning_rate = i * 0.1;
			net.in_median = 0.0;
			net.n_batch = 1;
			std::cout<<" Dev="<<net.in_deviation;
			std::cout<<" lr="<<net.learning_rate;
			std::cout<<" n_epoch="<<net.n_epoch;
			std::cout<<" n_hid="<<net.n_hidden;
			std::cout<<" n_batch="<<net.n_batch<<std::endl;
			net.init(ts.train_points[0].inputs.size(),net.n_hidden,ts.train_points[0].targets.size());
			net.train(ts);
			net.save_neurons();
		}
	}
}

#include <iostream>
using namespace std;
string state;
string input;
int accept = 0;
int Proc(string input) {
	if (state =="S0") {
		if ( input=="c"){
			state="S1"; } 

		if ( input=="a"){
			state="S0"; } 

		if ( input=="t"){
			state="S0"; } 

		if ( input=="n"){
			state="S0"; } 

}
	else if (state =="S1") {
		if ( input=="c"){
			state="S1"; } 

		if ( input=="a"){
			state="S2"; } 

		if ( input=="n"){
			state="S2"; } 

}
	else if (state =="S2") {
		if ( input=="t"){
			state="S2"; } 

		if ( input=="n"){
			state="S0"; } 

}
	else if (state =="S3") {
}
	if (state =="S3") return 1;
		return 0; }
int main(){
	state = "S0";
	while(1){
		cout<<" state="<<state<<" enter signal"<<endl;
	    cin>>input;
		Proc(input);
	    cout<<" new state="<<state<<endl; // p
	} 
}

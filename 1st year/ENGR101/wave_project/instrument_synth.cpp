#include <iostream> // input-output library 
#include <fstream>

#include <math.h>  // library for sin function 
#include "wav.hpp" // make sure to include this helper library 
// " " instead of <> means that library is in the same folder as your program 
 
using namespace std; 

double calc_volume(int vol_change, double samples) {
	if (vol_change == 0) {
		return 0;
	}
	else {
		double vol_rate = vol_change / samples ;
		return vol_rate;
	}
}

vector<double> readNotes(string filename) {
	vector<double> file_notes;
    double note;

    ifstream input_file(filename);

    while (input_file >> note) {
		file_notes.push_back(note);
    }
    input_file.close();
	return file_notes;
}  
   
int main(){ 
   const int SAMPLE_FREQ = 44100; // samples per second, select value which provides good quality sound  
   const double SAMPLE_PRD = (double)1/SAMPLE_FREQ;
   double harm_coef[] = {1, 1, 1, 0.250, 0.4, 0.167, 0.286, 0.125, 0.222, 0.1, 0.182};
   double sub_tempo[] = {0.9, 0.05, 0.0, 0.05}; // duration (attack, decay, sustain, release) (seconds)
   double tempo;
   for (int i = 0; i < 4; i++) {
	   sub_tempo[i] /= 2;
		tempo += sub_tempo[i];
	}
	
	vector<double> notes = readNotes("anotherbrickinthewall.txt"); // odetojoy , anotherbrickinthewall
	
	const int SMPLS_PER_NOTE = tempo * SAMPLE_FREQ;
	const int N_SAMPLES = SMPLS_PER_NOTE * notes.size();
	int* waveform = new int[N_SAMPLES]; // creates the array 
	
	int chng_in_vol[] = {0,0,0,0};
	int rng_in_vol[] = {0, 3000, 1500, 1500, 0};
	for (int i = 0; i < 4; i++) {
		chng_in_vol[i] = rng_in_vol[i + 1] - rng_in_vol[i];
	}
   int t = 0;
	for (vector<double>::iterator tone = notes.begin(); tone != notes.end(); ++tone){
		int cur_volume = 0;
		for (int sub = 0; sub < 4; sub++) {
			int sub_samples = sub_tempo[sub] * SAMPLE_FREQ;
			
			double v_rate = calc_volume(chng_in_vol[sub], sub_samples);

			if (sub_samples > 0) {
				for (int i = 0; i < sub_samples; i++) {
					t++;
					cur_volume = v_rate * i + rng_in_vol[sub];
					int temp =  0;
					for (int h = 0; h < 11; h++) {
						temp += (cur_volume * harm_coef[h]) * sin(2 * M_PI * ((h + 1) * (*tone)) * t * SAMPLE_PRD);
					}
					waveform[t] = temp;
				}
			}
		}
	} 

   MakeWavFromInt("final.wav",SAMPLE_FREQ, waveform, N_SAMPLES); //file name can be changed but keep extension .wav 
   notes.clear();	
   delete(waveform); 
   return 0; 
}
			


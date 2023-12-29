#include <iostream> // input-output library 
#include <math.h>  // library for sin function 

#include "wav.hpp" // make sure to include this helper library 
// " " instead of <> means that library is in the same folder as your program 
 
using namespace std; 

// takes current frequency 
int change_f(int poss_freq[], size_t tones_amt, int in_freq) {
	for (size_t i = 0; i < tones_amt; i++) {
		// If got to end then freq must match and begin again
		if (i == tones_amt-1) {}
		// If current frequency matches switch to next
		else if (poss_freq[i] == in_freq) {
			return poss_freq[i+1];
		}
	}
	return poss_freq[0];
}

int main(){ 
   int sample_rate = 44100; // samples per second, select value which provides good quality sound  
   double duration = 5.0; // how long [seconds] it will sound 
   int n_samples = duration * sample_rate; 
   
   const double TONE_DURATION = 1.0; // duration each tone plays for (seconds)
   const int TONE_SAMPLES = TONE_DURATION * sample_rate;
   
   double sample_period = (double)1/sample_rate;
   int* waveform = new int[n_samples]; // creates the array 
   int tones[] = {1319, 1047}; // E6, C6;
   size_t n_tones = sizeof(tones)/ sizeof(tones[0]);
   int volume = 3000;
   
   int freq = tones[0];
   for ( int i_sample = 0; i_sample < n_samples ; i_sample++){
	   
	   // switches frequency when TONE_DURATION has passed
	   if (i_sample % TONE_SAMPLES == 0) {
		   freq = change_f(tones, n_tones, freq);
	   }
       waveform[i_sample] = volume*sin(2 * M_PI * freq * i_sample * sample_period);
      // Asin(2*pi*f*dt)
   } 
   // generates sound file from waveform array, writes n_samples numbers  
   // into sound wav file 
   // should know sample_rate for sound timing 
   MakeWavFromInt("double_tone.wav",sample_rate, waveform, n_samples); //file name can be changed but keep extension .wav 
   delete(waveform); 
   return 0; 
}
			


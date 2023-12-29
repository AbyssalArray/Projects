#include <iostream> // input-output library 
#include <math.h>  // library for sin function 
#include "wav.hpp" // make sure to include this helper library 
// " " instead of <> means that library is in the same folder as your program 
 
using namespace std; 
 
int main(){ 
	
   int sample_rate = 44100; // samples per second, select value which provides good quality sound  
   double duration = 5.0; // how long [seconds] it will sound 
   int n_samples = duration * sample_rate; // if sound is "duration" seconds long and there are "sample_rate" samples per second 
                        // - how many samples are there altogether? What type should this variable be? 
   int* waveform = new int[n_samples]; // creates the array 
   double frequency = 523; // pitch of the sound, C
   int volume = 6000;// 6000 is loud enough 
   double sample_period = (double)1/sample_rate;
   
   for ( int i_sample = 0; i_sample < n_samples; i_sample++){  
       waveform[i_sample] = volume*sin(2 * M_PI * frequency * i_sample * sample_period);
      // Asin(2*pi*f*dt)
   } 
   // generates sound file from waveform array, writes n_samples numbers  
   // into sound wav file 
   // should know sample_rate for sound timing 
   MakeWavFromInt("singletone.wav",sample_rate, waveform, n_samples); //file name can be changed but keep extension .wav 
   delete(waveform); 
   return 0; 
}
			


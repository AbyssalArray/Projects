#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <vector>

using namespace std;

struct WavSound{
	//
//private:
   // wav waveform parameters as defined by the standard
   unsigned int      data_length;
   short	audio_format;
   short	num_channels;
   int	    sample_rate;
   int	    byte_rate;
   int  	block_align;
   int  	bits_per_sample;	
   int      n_samples;
   char*    wave;	    // raw byte data
   int*     wave_int;   // CORRESPONDING array of samples
   //vector<int> waveform;
	
//public:	
	WavSound(); //constructor
	//int ReadWavFile(string,int* &wf, int& ns,int& sr);
	//int SaveWavFile(string);
	//int SaveAsText(string file_name);
	//int MakeWavFromInt(string file_name,int sample_rate, int* wf, int n);
   // void AddSample(short new_sample);
   //int MakeWavFromVector(string file_name,int sample_rate_arg,vector<int> sound);
};

// constructor
WavSound::WavSound(){
	//waveform.clear();
}

int ExtractIntFrom2Chars(char* p,int addr){
	int extr;
	extr =  (((unsigned char)p[addr]));
	extr =  extr + (((unsigned char)p[addr+1])<<8);
	return extr;
}

int ExtractIntFrom4Chars(char* p,int addr){
	int extr;
	extr =  (((unsigned char)p[addr]));
	extr =  extr + (((unsigned char)p[addr+1])<<8);
	extr =  extr + (((unsigned char)p[addr+2])<<16);
	extr =  extr + (((unsigned char)p[addr+3])<<24);
	return extr;
}

void Set4CharsSEndFromInt(int n, char* a,int addr){
	a[addr] = (n)& 0xFF;
    a[addr+1] = (n>>8)& 0xFF;
    a[addr+2] = (n>>16)& 0xFF;
    a[addr+3] = (n>>24)& 0xFF;
    
}

// takes array of integers and makes wav 
int MakeWavFromInt(string file_name,int sample_rate_arg, int* wf, int n){
	WavSound ws;
	// compute wav file parameters
	cout<<"Making wav file"<<endl;
	ws.sample_rate = sample_rate_arg;
	ws.n_samples = n;
	///cout<<"sample rate = "<<sample_rate<<endl;
	//cout<<"n_samples = "<<n_samples<<endl;
	ws.bits_per_sample = 16;
	int n_chars = n*(ws.bits_per_sample/8); //wav file data chaunk size
	//cout<<"n_chars(data) = "<<n_chars<<endl;
	char* raw = new char[n_chars+44];
	// prepare header
    raw[0]='R';
    raw[1]='I';
    raw[2]='F';
    raw[3]='F';
    unsigned int chunk_size = 36+n_chars;
    Set4CharsSEndFromInt(chunk_size, raw , 4);
	raw[8]='W'; raw[9]='A'; raw[10]='V';  raw[11]='E';
    raw[12] = 'f';raw[13] = 'm';raw[14] = 't';raw[15] = ' ';
    unsigned int r = 16;
    Set4CharsSEndFromInt(r, raw , 16);
    
    raw[20] = 1;  raw[21] = 0; // PCM
    ws.num_channels = 1;
    raw[22] = 1; // number of channels
    raw[23] = 0;
    
    Set4CharsSEndFromInt(ws.sample_rate, raw , 24);
    raw[27] = (ws.sample_rate>>24)& 0xFF;
    raw[26] = (ws.sample_rate>>16)& 0xFF;
    raw[25] = (ws.sample_rate>>8)& 0xFF;
    raw[24] = (ws.sample_rate)& 0xFF;
    
    ws.bits_per_sample = 16;
    ws.byte_rate = ws.sample_rate * ws.num_channels*(ws.bits_per_sample/8);
    //cout<<" sample_rate="<<sample_rate<<endl;
    //cout<<" num_channels="<<num_channels<<endl;
    //cout<<" bits_per_sample="<<bits_per_sample<<endl;
    
    //cout<<" byte_rate="<<byte_rate<<endl;
    raw[31] = (ws.byte_rate>>24)& 0xFF;
    raw[30] = (ws.byte_rate>>16)& 0xFF;
    raw[29] = (ws.byte_rate>>8)& 0xFF;
    raw[28] = (ws.byte_rate)& 0xFF;
    
    ws.block_align = ws.num_channels*(ws.bits_per_sample/8);
    //cout<<" block_align="<<block_align<<endl;
    raw[33] = (ws.block_align>>8)& 0xFF;
    raw[32] = (ws.block_align)& 0xFF;
    
    raw[35] = (ws.bits_per_sample>>8)& 0xFF;
    raw[34] = (ws.bits_per_sample)& 0xFF;
    // subchunk2
    raw[36] = 'd';
    raw[37] = 'a';
    raw[38] = 't';
    raw[39] = 'a';
    // subchunk2 size
    raw[43] = (n_chars>>24)& 0xFF;
    raw[42] = (n_chars>>16)& 0xFF;
    raw[41] = (n_chars>>8)& 0xFF;
    raw[40] = (n_chars)& 0xFF;
    // save samples data
    int pos= 0;
    for ( int iint = 0 ; iint < ws.n_samples ; iint++){
        raw[44+pos] = (wf[iint])& 0xFF;
        raw[44+pos+1] = (wf[iint]>>8)& 0xFF;
        pos = pos + 2;
    }
     // check wav_int. If not NULL, delete and make new one
    // copying from input argument
    //if (wave_int != NULL){
	//	delete(wave_int);
	//	wave_int = new int[n_samples];
	//} else {
	//	wave_int = new int[n_samples];
	//}
    //for ( int i = 0 ; i < n_samples ; i++){
	//	wave_int[i] = wf[i];
	//	if ( i < 20 ){
	//		cout<<"wav.hpp, copying: i="<<i<<" w[i]="<<wave_int[i]<<endl;
	//	}

	//}
    
	// save raw data to the wav. file
	ofstream ofs;
	ofs.open (file_name, ofstream::out | ofstream::binary);
	if (ofs.is_open()){
		ofs.write(raw,n_chars+44);
		ofs.close();
		cout<<" Waveform data copied to wav sound"<<endl;
		delete raw;
		return 0;
	} else {
        cout<<" Waveform data can not be copied to wav sound file"<<file_name<<endl;
        delete raw;
		return -1;
	}
	return 1;
}

/*
void WavSound::AddSample(short new_sample){
	waveform.push_back(new_sample);
}
*/

int MakeWavFromVector(string file_name,int sample_rate_arg,vector<int> sound){
	WavSound ws;
	
	// compute wav file parameters
	cout<<"Making wav file from vector"<<endl;
	ws.sample_rate = sample_rate_arg;
	ws.n_samples = sound.size();
	//cout<<"sample rate = "<<ws.sample_rate<<endl;
	//cout<<"n_samples = "<<ws.n_samples<<endl;
	ws.bits_per_sample = 16;
	int n_chars = ws.n_samples*(ws.bits_per_sample/8); //wav file data chaunk size
	//cout<<"n_chars(data) = "<<n_chars<<endl;
	char* raw = new char[n_chars+44];
	// prepare header
    raw[0]='R';
    raw[1]='I';
    raw[2]='F';
    raw[3]='F';
    unsigned int chunk_size = 36+n_chars;
    Set4CharsSEndFromInt(chunk_size, raw , 4);
	raw[8]='W'; raw[9]='A'; raw[10]='V';  raw[11]='E';
    raw[12] = 'f';raw[13] = 'm';raw[14] = 't';raw[15] = ' ';
    unsigned int r = 16;
    Set4CharsSEndFromInt(r, raw , 16);
    
    raw[20] = 1;  raw[21] = 0; // PCM
    ws.num_channels = 1;
    raw[22] = 1; // number of channels
    raw[23] = 0;
    
    Set4CharsSEndFromInt(ws.sample_rate, raw , 24);
    raw[27] = (ws.sample_rate>>24)& 0xFF;
    raw[26] = (ws.sample_rate>>16)& 0xFF;
    raw[25] = (ws.sample_rate>>8)& 0xFF;
    raw[24] = (ws.sample_rate)& 0xFF;
    
    ws.bits_per_sample = 16;
    ws.byte_rate = ws.sample_rate * ws.num_channels*(ws.bits_per_sample/8);
    //cout<<" sample_rate="<<ws.sample_rate<<endl;
    //cout<<" num_channels="<<ws.num_channels<<endl;
    //cout<<" bits_per_sample="<<ws.bits_per_sample<<endl;
    
    cout<<" byte_rate="<<ws.byte_rate<<endl;
    raw[31] = (ws.byte_rate>>24)& 0xFF;
    raw[30] = (ws.byte_rate>>16)& 0xFF;
    raw[29] = (ws.byte_rate>>8)& 0xFF;
    raw[28] = (ws.byte_rate)& 0xFF;
    
    ws.block_align = ws.num_channels*(ws.bits_per_sample/8);
    //cout<<" block_align="<<ws.block_align<<endl;
    raw[33] = (ws.block_align>>8)& 0xFF;
    raw[32] = (ws.block_align)& 0xFF;
    
    raw[35] = (ws.bits_per_sample>>8)& 0xFF;
    raw[34] = (ws.bits_per_sample)& 0xFF;
    // subchunk2
    raw[36] = 'd';
    raw[37] = 'a';
    raw[38] = 't';
    raw[39] = 'a';
    // subchunk2 size
    raw[43] = (n_chars>>24)& 0xFF;
    raw[42] = (n_chars>>16)& 0xFF;
    raw[41] = (n_chars>>8)& 0xFF;
    raw[40] = (n_chars)& 0xFF;
    // save samples data
    int pos= 0;
    for ( int iint = 0 ; iint < ws.n_samples ; iint++){
        raw[44+pos] = (sound.at(iint))& 0xFF;
        raw[44+pos+1] = (sound.at(iint)>>8)& 0xFF;
        pos = pos + 2;
    }
    // check wav_int. If not NULL, delete and make new one
    // copying from input argument
    //if (wave_int != NULL){
	//	delete(wave_int);
	//	wave_int = new int[n_samples];
	//} else {
	//	wave_int = new int[n_samples];
	//}
    //for ( int i = 0 ; i < n_samples ; i++){
	//	wave_int[i] = wf[i];
	//	if ( i < 20 ){
	//		cout<<"wav.hpp, copying: i="<<i<<" w[i]="<<wave_int[i]<<endl;
	//	}

	//}
    
	// save raw data to the wav. file
	ofstream ofs;
	ofs.open (file_name, ofstream::out | ofstream::binary);
	if (ofs.is_open()){
		ofs.write(raw,n_chars+44);
		ofs.close();
        cout<<" Waveform data copied to wav sound"<<endl;
        delete raw;
        return 0;
	} else{
        cout<<" Waveform data can not be copied to wav sound file"<<file_name<<endl;
        delete raw;
	    return -1;
	}


	return 0;
}

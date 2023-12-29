/*
 * 
 */ 

#include "image_pr4.h" 
#include <thread>
#include <chrono>
#include <cmath>


int rCount = 0;
int prevTime = 0;
int sunCenter = 0;
int x, y;

double a11,a12,a13,a21,a22,a23,a31,a32,a33,b1,b2,b3;

// you can use that
struct Orbit {
	// logged position and time
	std::vector<int> x;
	std::vector<int> y;
	std::vector<int> t;
	int xc,yc,r;  // center and radius
	int x_sunrise,y_sunrise;
	double omega = 0.1;
} orbit;
//Positons of the red pixels
std::vector<int> xPos;
std::vector<int> yPos;


struct Sun{
	int xRight;
	int xLeft;
	int yTop;
	int yBot;
	} sun;
	
bool contains(int xValue, int  yValue){
	for(unsigned int i = 0; i <xPos.size(); i++){
		if(xValue == xPos.at(i) && yValue == yPos.at(i) ){
			return true;
		} 
	}
	return false;
	
}
	
bool isRedPix(ImagePPM img, int xCord, int yCord) {
	int pixRed = get_pixel(img, yCord,xCord, 0); // getting colour values of pixel
	int pixLum = get_pixel(img, yCord,xCord, 3); // getting luminosity values of pixel
	const int ACCURACY = 20;		//how much leway is given when comparing to red
	// returning the true if red is above luminosity
	return (pixRed + ACCURACY > pixLum);
}

void recursiveHelper(int row, int col, Sun& t){
	//adjusting for bounderies 0 so it wont go back with -1
	int rStart = (row == 0) ? 0 : -1;
	int cStart = (col == 0) ? 0 : -1;
	//adjusting for max by making the index never reach 1
	int rEnd = (row == image.height-1) ? 1 : 2;
	int cEnd = (col == image.width-1) ? 1 : 2;
			

			if(isRedPix(image, col, row)){
		
				if(!contains(col,row)){
						// std::cout<<"Wow"<<std::endl;
					xPos.push_back(col);
					yPos.push_back(row);
					if(col < t.xLeft){
						
						t.xLeft = col;
						}
					if(row < t.yTop){
						t.yTop = row;
						}
					//right most pixel
					if(col > t.xRight){
						t.xRight = col;
						}
					//bottomest pos
					if(row > t.yBot){
						t.yBot = row;
					}
			for (int r = rStart; r < rEnd; r++)
				for (int c = cStart; c < cEnd; c++) 
							recursiveHelper(row + r, col + c, t);


				}
	}
}

bool locateSun(int time){
	int count = 0;
	for(int row = 0; row<image.height; row++){
		for(int col = 0; col<image.width; col++){
			if(isRedPix(image, col, row)) { 
					Sun tempSun;
					tempSun.xLeft = image.width;
					tempSun.yTop = image.height;
					tempSun.xRight = 0;
					tempSun.yBot = 0; 
					
					if(!contains(col, row)){
						recursiveHelper(row, col, tempSun);
						int xDiff = tempSun.xRight-tempSun.xLeft;
						int yDiff = tempSun.yBot - tempSun.yTop;

						if((xDiff - yDiff) *(xDiff - yDiff) <=  1 && yDiff == 26) {
							sun = tempSun;
							return true;
						}
				    count++;
					}  
				}
			}
		}

	return false;
	}
	

//Orbit caluclator
void calulateOrbit(int time){
	x = sun.xRight-((sun.xRight-sun.xLeft)/2);
	y = sun.yBot - ((sun.yBot-sun.yTop)/2);
	std::cout<<x<<", "<<y<<std::endl;
	std::cout<<scene.x_sun<<", "<<scene.y_sun<<std::endl;

	orbit.x.push_back(x);
	orbit.y.push_back(y);
	orbit.t.push_back(time);
	
	a11 = orbit.x.size();
    a12 =0;
    a21 = 0;
    a22 = orbit.x.size();
    a33 = orbit.t.size();
    a13 = 0.0;
    a23 = 0.0;
    a31 = 0.0;
    a32 = 0.0;
    b1 = 0.0;
    b2 = 0.0;
    b3 = 0.0;

    for(unsigned int i =0; i < orbit.t.size(); i++){
        a13 +=cos(orbit.omega*orbit.t.at(i));

        a23 += sin(orbit.omega*orbit.t.at(i));

        a31+= cos(orbit.omega*orbit.t.at(i));

        a32 += sin(orbit.omega*orbit.t.at(i));
 
        b1 += orbit.x.at(i);

        b2 += orbit.y.at(i);

        b3 += orbit.x.at(i) * cos(orbit.omega*orbit.t.at(i)) + orbit.y.at(i) * sin(orbit.omega*orbit.t.at(i));

    }


    double det = a11 *(a22 * a33 - a23 * a32) - a12* (a21 * a33 - a23 * a31) + a13 *(a21 * a32 - a22 * a31);

    double detx = b1 *(a22 * a33 - a23 * a32) - a12* (b2 * a33 - a23 * b3) + a13 *(b2 * a32 - a22 * b3);

    double dety = a11 *(b2 * a33 - a23 * b3) - b1* (a21 * a33 - a23 * a31) + a13 *(a21 * b3 - b2 * a31);

    double detr = a11 *(a22 * b3 - b2 * a32) - a12* (a21 * b3 - b2 * a31) + b1 *(a21 * a32 - a22 * a31);



    orbit.xc = detx/det;
    orbit.yc = dety/det;
    orbit.r = detr/det;
    std::cout<<orbit.r<<", "<<orbit.xc<<", "<<orbit.yc<<std::endl;

	}


int main()
{        
	std::cout<<"start..."<<std::endl;
	init(2);//0 - core, 1- completion, 2-challenge
	//int x_sun, y_sun; // current position of the sun
    for ( int time = 0 ; time < 950; time++){
       draw_all(time);  // image is ready,
       xPos.clear();
       yPos.clear();
       bool sunCheck = locateSun(time);
       if(sunCheck ){
		   //centerXandY(time);
		   calulateOrbit(time);
	   }
	   
		   int boxX, boxY;
		   int xSun = orbit.xc + orbit.r * cos(orbit.omega*(time+1));
	       int ySun = orbit.yc + orbit.r * sin(orbit.omega*(time+1));
		   get_aim(boxX, boxY);
           double angle = atan2(ySun - boxY, xSun - boxX);
           move_aim(angle);
		   
   
       //int xSun = orbit.xc + orbit.r * cos(orbit.omega*(time+1));
	   //int ySun = orbit.yc + orbit.r * sin(orbit.omega*(time+1));
       std::cout<<" time="<<time<<std::endl;
       //std::cout<<"________________________________________"<<std::endl;
       //std::cout<<"xc: "<<orbit.xc<<" || yc: "<<orbit.yc<<std::endl;
       //std::cout<<"________________________________________"<<std::endl;
       std::this_thread::sleep_for(std::chrono::milliseconds(50));
       
   }
    return 0;
}

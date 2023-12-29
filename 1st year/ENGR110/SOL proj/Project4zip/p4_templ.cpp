#include "image_pr4.h" 
#include <thread>
#include <chrono>
#include <algorithm>
#include <vector>


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
struct Pos {
	int x;
	int y;
};

std::vector<Pos> v;
struct Obj {
	std::vector<Pos> p;
	int yl = 100000;
	int yr = 0;
	int xl = 100000;
	int xr = 0;
} sun;

bool isRedPix(ImagePPM img, int xCord, int yCord) {
	int pixRed = get_pixel(img, yCord,xCord, 0); // getting colour values of pixel
	int pixLum = get_pixel(img, yCord,xCord, 3); // getting luminosity values of pixel
	const int ACCURACY = 20;		//how much leway is given when comparing to red
	// returning the true if red is above luminosity
	return (pixRed + ACCURACY > pixLum);
}

bool contains(Pos point) {
	for (Pos pnt : v) {
		if (point.x == pnt.x && point.y == pnt.y)  {
			return true;
		}  
	}
	return false;
}

void checkAt(ImagePPM& img, int row, int col, Obj& tmp){
	//adjusting for bounderies 0 so it wont go back with -1
	int rStart = (row == 0) ? 0 : -1;
	int cStart = (col == 0) ? 0 : -1;
	//adjusting for max by making the index never reach 1
	int rEnd = (row == img.height-1) ? 1 : 2;
	int cEnd = (col == img.width-1) ? 1 : 2;
	
	Pos p;
	p.x = col;
	p.y = row;
		if (isRedPix(img, row, col) && !contains(p))     {
			
			v.push_back(p);
			tmp.p.push_back(p);
			tmp.xl = (tmp.xl > col) ? col : tmp.xl;
			tmp.xr = (tmp.xr < col) ? col : tmp.xr;
			tmp.yl = (tmp.yl > row) ? row : tmp.yl;
			tmp.yr = (tmp.yr < row) ? row : tmp.yr;
			for (int r = rStart; r < rEnd; r++)
				for (int c = cStart; c < cEnd; c++) 
					checkAt(img, row + r, col + c,tmp);   // row +- 1, col +- 1 unless border
	}
}
bool findSun(ImagePPM& img) {
	for (int col = 0; col < img.width; col++)  {
		for (int row = 0; row < img.height; row++)  {
			if (isRedPix(img, row, col))     {
				Pos cp;
				cp.x = col;
				cp.y = row;
				if(!contains(cp)) {
					Obj tmp_sun;
					tmp_sun.xl = img.width +1;
					tmp_sun.yl = img.height +1;
					checkAt(img, row, col, tmp_sun);
					int width =	tmp_sun.xr - tmp_sun.xl;
					int height = tmp_sun.yr - tmp_sun.yl;
					if (height < 30 && height > 20 && ( width-height ) * ( width-height ) <= 1)  {
						sun = tmp_sun;
						return true;
					}
				}
			}
		}
	}
	return false;
}
double det2(double c11, double c12, double c21, double c22) {
	return c11*c22 - c12 * c21;
}
int main()
{
	std::cout<<"start..."<<std::endl;
	init(2);
	//int x_sun, y_sun; // current position of the sun
    for ( int time = 0 ; time < 900; time++){
		draw_all(time);  // image is ready, 
		v.clear();
		   
	   if(findSun(image)) {
			orbit.t.push_back(time);
			
			int y = sun.xr - ((sun.xr - sun.xl)/2);
			int x = sun.yr - ((sun.yr - sun.yl)/2);
			orbit.x.push_back(x);
			orbit.y.push_back(y);

			if (orbit.t.size() > 10 && orbit.t.size() < 30) {
				int n = orbit.t.size();
				double b[3] = {0.0,0.0,0.0};
				double a[3][3] = {{0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}};
				a[0][0] =  (double)n;
				a[0][1] = 0.0;
				a[1][0] = 0.0;
				a[1][1] = (double)n;
				a[2][2] = (double)n;
				double b3l = 0.0;
				double b3r = 0.0;
				for (int t = 0; t < n; t++) {
					a[0][2] += cos(orbit.omega * orbit.t.at(t));
					a[2][0] += cos(orbit.omega * orbit.t.at(t));
					a[1][2] += sin(orbit.omega * orbit.t.at(t));
					a[2][1] += sin(orbit.omega * orbit.t.at(t));
					b[0] += orbit.x.at(t);
					b[1] += orbit.y.at(t);
					b3l +=  orbit.x.at(t) * cos(orbit.omega * orbit.t.at(t));
					b3r +=  orbit.y.at(t) * sin(orbit.omega * orbit.t.at(t));
				}
				b[2] = b3l + b3r;

				double d1 = b[0] * det2(a[1][1], a[1][2], a[2][1], a[2][2]);
				d1 += -1 * (a[0][1] * det2(b[1], a[1][2], b[2], a[2][2]));
				d1 += a[0][2] * det2(b[1], a[1][1], b[2], a[2][1]);
				
				double d2 = a[0][0] * det2(a[1][1], a[1][2], a[2][1], a[2][2]);
				d2 += -1 * (a[0][1] * det2(a[1][0], a[1][2], a[2][0], a[2][2]));
				d2 += a[0][2] * det2(a[1][0], a[1][1], a[2][0], a[2][1]);
				
				double d3 = a[0][0] * det2(b[1], a[1][2], b[2], a[2][2]);
				d3 += -1 * (b[0] * det2(a[1][0], a[1][2], a[2][0], a[2][2]));
				d3 += a[0][2] * det2(a[1][0], b[1], a[2][0], b[2]);

				double d4 = a[0][0] * det2(a[1][1], b[1], a[2][1], b[2]);
				d4 += -1 * (a[0][1] * det2(a[1][0], b[1], a[2][0], b[2]));
				d4 += b[0] * det2(a[1][0], a[1][1], a[2][0], a[2][1]);			
				orbit.xc = d1/d2;
				orbit.yc = d3/d2;
				orbit.r = d4/d2;
				orbit.y_sunrise = image.height;
				orbit.x_sunrise = orbit.xc - sqrt(orbit.r * orbit.r - (image.height - orbit.yc)* (image.height - orbit.yc));
			}
		}
	   
		if (orbit.t.size() > 10) {
			int trkX, trkY;
			get_aim(trkX, trkY);
		   
			int xPos = orbit.xc + orbit.r * cos(orbit.omega * (time+1));
			int yPos = orbit.yc + orbit.r * sin(orbit.omega *  (time+1));
			
			if (yPos > orbit.y_sunrise) {
				yPos = orbit.y_sunrise;
				xPos = orbit.x_sunrise;
			}
			move_aim(atan2( yPos - trkY, xPos - trkX));
	   }
       std::cout<<" time="<<time<<std::endl;
       std::this_thread::sleep_for(std::chrono::milliseconds(500));
   }
    return 0;
}


#include <iostream>
namespace Complex{
	class complex{
	private:
	int a;
	int b;
	
	public:
	complex() {
		a = 1;
		b = 1;
	}
	complex(int ia, int ib) {
		a = ia;
		b = ib;
	}
	int geta() {
		return a;
	}
	int getb() {
		return b;
	}
};
}


int main() {
	Complex::complex c1;
	Complex::complex c2(5,10);
	std::cout << "Complex number 1:" << c1.geta() <<" + " << c1.getb() << "i\n"<<std::endl;
	std::cout << "Complex number 2:" << c2.geta() <<" + " << c2.getb() << "i\n"<<std::endl;
	return 0;
}

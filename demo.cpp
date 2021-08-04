#include <iostream>
#include <vector>
#include <fstream>

int main (){

	std::vector<std::vector<std::vector<int> > > v;

	std::ifstream s;
	s.open("CosineLSH/data/hash_table");
	int a, b, c;
	s >> a >> b >> c;
	std::cout << a << " " << b << " " << c << std::endl;
	v.resize(a, std::vector<std::vector<int>>(b, std::vector<int>(c)));

	int data;

	for (int i = 0; i < a; i++) {
		for (int j = 0; j < b; j++) {
			for (int k = 0; k < c; k++) {
				s >> data;
				v[i][j][k] = data;
				std::cout << v[i][j][k] << " ";
			}
		}
	}
	s.close();

	// for (int i = 0; i < 5; i++) {
	// 	for (int j = 0; j < 3; j++) {
	// 		for (int k = 0; k < 7; k++) {
	// 			v[i][j][k] = 2;
	// 		}
	// 	}
	// }
	
	// std::ofstream oFile;
	// oFile.open("CosineLSH/data/hash_table");

	// oFile << 5 << " " << 3 << " " << 7 << std::endl;

	// for (int i = 0; i < 5; i++) {
	// 	for (int j = 0; j < 3; j++) {
	// 		for (int k = 0; k < 7; k++) {
	// 			if (k == 0)
	// 				oFile << v[i][j][k];
	// 			else
	// 				oFile << " " << v[i][j][k];
	// 		}
	// 		if (i == 4 && j == 2)
	// 			continue;
	// 		oFile << std::endl;
	// 	}
	// }

	// oFile.close();

  return 0;
}
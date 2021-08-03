#include<iostream>
#include<vector>
#include<algorithm>
using namespace std;

int main() {
    
	vector<int> nums = {10, 5, 40, 10, 5, 20, 10, 10, 30 };
	
  random_shuffle(nums.begin(), nums.end());
	for (const auto &num : nums) {
		cout << num << " ";
	}
  cout << endl;

	random_shuffle(nums.begin(), nums.end());
	for (const auto &num : nums) {
		cout << num << " ";
	}
	cout << endl;
	return 0;
}
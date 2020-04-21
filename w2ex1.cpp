
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <iomanip>
#include <bitset>

typedef uint64_t u64;

using namespace std;

class BitArray {
private:
	u64* arr;

public:
	int size;

	BitArray(int n) {
		size = n / 64;
		if (n % 64 != 0) size++;

		arr = new u64[size]();
	}

	int get(int i) {
		u64 num = arr[i / 64];
		u64 mask = 1;
		mask <<= (i % 64);

		return ((num & mask) == 0) ? 0 : 1;
	}

	void set(int i, int b) {
		u64 num = arr[i / 64];
		u64 mask = 1;
		mask <<= (i % 64);

		if (b == 0) {
			num &= ~mask;
		} else {
			num |= mask;
		}

		arr[i / 64] = num;
	}

	void dealloc() {
		delete[] arr;
	}

	void print() {
		for (int i = 0; i < size; i++) {
			bitset<64> b(arr[i]);
			cout << b << "\n";
		}
	}
};

int main() {
	int n = 64;
	int m = 32;

	BitArray ba(n);

	vector<int> ints;
	mt19937 g(random_device{}());
	uniform_int_distribution<int> d(0, n - 1);
	for (int i = 0; i < m; i++) {
		ints.push_back(d(g));
	}

	chrono::high_resolution_clock::time_point setStart = chrono::high_resolution_clock::now();
	for (int i : ints) {
		ba.set(i, 1);
	}
	chrono::high_resolution_clock::time_point setEnd = chrono::high_resolution_clock::now();

	chrono::high_resolution_clock::time_point getStart = chrono::high_resolution_clock::now();
	for (int i : ints) {
		ba.get(i);
	}
	chrono::high_resolution_clock::time_point getEnd = chrono::high_resolution_clock::now();

	chrono::duration<double> setTime = chrono::duration_cast<chrono::duration<double>>(setEnd - setStart);
	chrono::duration<double> getTime = chrono::duration_cast<chrono::duration<double>>(getEnd - getStart);

	cout << fixed << setprecision(16);
	cout << "Setting values took: " << setTime.count() << " seconds.\n";
	cout << "Getting values took: " << getTime.count() << " seconds.\n";

	ba.dealloc();
	return 0;
}


#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <iomanip>

typedef uint64_t u64;

using namespace std;

class BitArray {
private:
	u64* arr;

public:
	BitArray(u64 n) {
		u64 size = n / 64;
		if (n % 64 != 0) size++;
		arr = new u64[size]();
	}

	u64 get(u64 i) {
		// u64 containing value to get
		u64 num = arr[i / 64];
		// read the value with a bit mask and return it
		u64 mask = 1;
		mask <<= (i % 64);
		return ((num & mask) == 0) ? 0 : 1;
	}

	void set(u64 i, int b) {
		// u64 containing the position to write to
		u64 num = arr[i / 64];
		// set the bit to 0 or 1 with a bit mask
		u64 mask = 1;
		mask <<= (i % 64);

		if (b == 0) {
			num &= ~mask;
		} else {
			num |= mask;
		}

		// save the new u64 to the array
		arr[i / 64] = num;
	}

	void dealloc() {
		delete[] arr;
	}
};

void measureTime(BitArray ba, vector<u64> ints, int mode) {
	chrono::high_resolution_clock::time_point start = chrono::high_resolution_clock::now();

	if (mode == 0) {
		for (u64 i : ints) {
			ba.set(i, 1);
		}
	} else {
		for (u64 i : ints) {
			ba.get(i);
		}
	}
	
	chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();
	chrono::duration<double> dur = chrono::duration_cast<chrono::duration<double>>(end - start);

	string op = (mode == 0) ? "Setting" : "Getting";
	cout << fixed << setprecision(9);
	cout << op << " values took: " << dur.count() << " seconds.\n";
}

int main() {
	u64 n, m;
	cout << "enter number of bits in array (n): ";
	cin >> n;
	cout << "enter number of random positions to generate (m): ";
	cin >> m;

	BitArray ba(n);

	vector<u64> ints;
	mt19937 g(random_device{}());
	uniform_int_distribution<u64> d(0, n - 1);
	for (u64 i = 0; i < m; i++) {
		ints.push_back(d(g));
	}

	measureTime(ba, ints, 0); // set
	measureTime(ba, ints, 1); // get

	ints.clear();
	for (u64 i = 0; i < m; i++) {
		ints.push_back(d(g));
	}

	measureTime(ba, ints, 1); // get

	ba.dealloc();
	return 0;
}

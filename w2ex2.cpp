
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <iomanip>
#include <intrin.h>
#include <bitset>

typedef uint64_t u64;

// visual studio
//#define popcnt __popcnt64
// gcc
#define popcnt __builtin_popcountll 

using namespace std;

class SumBitArray {
private:
	u64* arr;
	u64* sums;

public:
	u64 size;

	SumBitArray(u64 n) {
		size = n / 64;
		if (n % 64 != 0) size++;

		arr = new u64[size]();
		sums = new u64[size]();
	}

	u64 get(u64 i) {
		u64 num = arr[i / 64];
		u64 mask = 1;
		mask <<= (i % 64);

		return ((num & mask) == 0) ? 0 : 1;
	}

	void set(u64 i, u64 b) {
		u64 num = arr[i / 64];
		u64 mask = 1;
		mask <<= (i % 64);

		if (b == 0) {
			num &= ~mask;
		}
		else {
			num |= mask;
		}

		arr[i / 64] = num;
	}

	u64 naiveSum(u64 i) {
		u64 s = 0;

		for (u64 j = 0; j <= i; j++) {
			s += this->get(j);
		}
		
		return s;
	}

	void generateSums() {
		u64 total = 0;

		for (u64 i = 0; i < size; i++) {
			total += popcnt(arr[i]);
			sums[i] = total;
		}
	}

	u64 sum(u64 i) {
		u64 div = (i + 1) / 64;
		u64 mod = (i + 1) % 64;
		u64 sum = 0;

		if (div > 0) {
			sum = sums[div - 1];
		}

		if (mod != 0) {
			u64 num = arr[div];
			u64 mask = 0;
			mask = (~mask) >> (64 - mod);
			sum += popcnt(num & mask);
		}

		return sum;
	}

	void dealloc() {
		delete[] arr;
		delete[] sums;
	}
};

void measureTime(SumBitArray ba, u64 r, int mode) {
	u64 sum = 0;
	chrono::high_resolution_clock::time_point start = chrono::high_resolution_clock::now();

	if (mode == 0) {
		sum = ba.naiveSum(r);
	} else if (mode == 1) {
		ba.generateSums();
	} else {
		sum = ba.sum(r);
	}

	chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();
	chrono::duration<double> dur = chrono::duration_cast<chrono::duration<double>>(end - start);

	cout << fixed << setprecision(9);
	cout << "time taken: " << dur.count() << " seconds.\n";

	if (mode != 1) {
		cout << "sum: " << sum << "\n";
	}
}

void benchmark() {
	u64 n, m, r;
	cout << "enter number of bits: ";
	cin >> n;
	cout << "enter number of bits to set to 1: ";
	cin >> m;
	cout << "enter upper limit of range to sum: ";
	cin >> r;

	SumBitArray ba(n);

	vector<int> ints;
	mt19937 g(random_device{}());
	uniform_int_distribution<int> d(0, n - 1);
	for (u64 i = 0; i < m; i++) {
		ints.push_back(d(g));
	}

	for (int i : ints) {
		ba.set(i, 1);
	};

	cout << "Generating sum support structure\n";
	measureTime(ba, r, 1);

	cout << "Calculating sum with simple algorithm\n";
	measureTime(ba, r, 0);

	cout << "Calculating sum with better algorithm\n";
	measureTime(ba, r, 2);

	ba.dealloc();
}

int main() {
	benchmark();

	return 0;
}

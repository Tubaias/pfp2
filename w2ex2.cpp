
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <iomanip>

typedef uint64_t u64;

// gcc
#define popcnt __builtin_popcountll 

using namespace std;

class SumBitArray {
private:
	u64* arr;
	u64* sums;
	u64 size;
	u64 suminterval;

public:
	SumBitArray(u64 n, u64 t) {
		// size of bit array
		size = n / 64;
		if (n % 64 != 0) size++;
		arr = new u64[size]();

		// size of sum support structure
		u64 sumsize = size / t;
		sums = new u64[sumsize]();

		suminterval = t;
	}

	u64 get(u64 i) {
		// u64 containing value to get
		u64 num = arr[i / 64];
		// read the value with a bit mask and return it
		u64 mask = 1;
		mask <<= (i % 64);
		return ((num & mask) == 0) ? 0 : 1;
	}

	void set(u64 i, u64 b) {
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

	// sum function using the simple and slow code from the spec
	u64 naiveSum(u64 i) {
		u64 s = 0;

		for (u64 j = 0; j <= i; j++) {
			s += this->get(j);
		}
		
		return s;
	}

	// generating support strucure for better sum function
	void generateSums() {
		u64 total = 0;

		for (u64 i = 0; i < size; i++) {
			// number of 1-bits in arr[i]
			total += popcnt(arr[i]);

			// if enough u64's have been summed, save current total
			if ((i + 1) % suminterval == 0) {
				sums[((i + 1) / suminterval) - 1] = total;
			}
		}
	}

	u64 sum(u64 i) {
		// helper values
		u64 bigdiv = (i + 1) / (suminterval * 64);
		u64 bigmod = (i + 1) % (suminterval * 64);
		u64 div = (i + 1) / 64;
		u64 mod = (i + 1) % 64;
		u64 sum = 0;

		// use support structure if range to sum is big enough
		if (bigdiv > 0) {
			sum = sums[bigdiv - 1];
		}

		// add remaining 1-bits to the sum
		if (bigmod != 0) {
			// whole u64's
			for (u64 j = (bigdiv * suminterval); j < div; j++) {
				u64 num = arr[j];
				sum += popcnt(num);
			}
			
			// last non-whole u64
			if (mod != 0) {
				u64 num = arr[div];
				u64 mask = 0;
				mask = (~mask) >> (64 - mod);
				sum += popcnt(num & mask);
			}
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

void hundredSums(SumBitArray ba, u64 n) {
	vector<u64> ints;
	mt19937 g(random_device{}());
	uniform_int_distribution<u64> d(0, n - 1);
	for (u64 i = 0; i < 100; i++) {
		ints.push_back(d(g));
	}

	chrono::high_resolution_clock::time_point start = chrono::high_resolution_clock::now();

	for (u64 i : ints) {
		ba.sum(i);
	}

	chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();
	chrono::duration<double> dur = chrono::duration_cast<chrono::duration<double>>(end - start);

	cout << fixed << setprecision(9);
	cout << "time taken: " << dur.count() << " seconds.\n";
}

int main() {
	u64 n, m, t, r;
	cout << "enter number of bits (n): ";
	cin >> n;
	cout << "enter number of bits to set to 1 (m): ";
	cin >> m;
	cout << "enter interval of sum pre-generation (t / 64): ";
	cin >> t;
	cout << "enter range to sum (i), max n - 1, 0 for a benchmark of 100 random values: ";
	cin >> r;

	SumBitArray ba(n, t);

	vector<u64> ints;
	mt19937 g(random_device{}());
	uniform_int_distribution<u64> d(0, n - 1);
	for (u64 i = 0; i < m; i++) {
		ints.push_back(d(g));
	}

	for (u64 i : ints) {
		ba.set(i, 1);
	};

	cout << "\nGenerating sum support structure\n";
	measureTime(ba, r, 1);

	if (r == 0) {
		cout << "\nCalculating 100 sums\n";
		hundredSums(ba, n);
	} else {
		cout << "\nCalculating sum with simple algorithm\n";
		measureTime(ba, r, 0);

		cout << "\nCalculating sum with better algorithm\n";
		measureTime(ba, r, 2);
	}

	ba.dealloc();
	return 0;
}

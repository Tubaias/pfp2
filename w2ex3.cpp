
#include <iostream>
#include <vector>
#include <random>

typedef uint64_t u64;

using namespace std;

class PackedIntegerArray {
private:
	u64* arr;
	u64 size;
	u64 width;

public:
	PackedIntegerArray(u64 n, u64 k) {
		width = k;
		size = k * n / 64;
		if ((k * n) % 64 != 0) size++;

		arr = new u64[size]();
	}

	u64 get(u64 i) {
		// helper values
		u64 start = i * width;
		u64 end = (i + 1) * width - 1;
		u64 startdiv = start / 64;
		u64 enddiv = end / 64;
		start %= 64;
		end %= 64;

		// u64 containing desired bits
		u64 num = arr[startdiv];

		if (startdiv == enddiv) {
			// remove trailing bits
			num <<= 63 - end;
			// remove leading bits
			num >>= 63 - end + start;
			return num;
		} else {
			// second u64 containing desired bits
			u64 num2 = arr[enddiv];
			// remove leading bits from first u64
			num >>= start;
			// remove tailing bits and offset second u64
			num2 <<= 63 - end;
			num2 >>= start - end - 1;
			// return sum of numbers
			return num + num2;
		}
	}

	void set(u64 i, u64 val) {
		// helper values
		u64 start = i * width;
		u64 end = (i + 1) * width - 1;
		u64 startdiv = start / 64;
		u64 enddiv = end / 64;
		start %= 64;
		end %= 64;

		// initialize bit mask
		u64 mask = 0;
		mask = ~mask;

		// u64 containing block to write
		u64 num = arr[startdiv];

		if (startdiv == enddiv) {
			// set block to zeros using the bit mask
			mask = ~((mask >> 64 - width) << start);
			num &= mask;
			// offset value and write it to block
			val <<= start;
			arr[startdiv] = num | val;
		} else {
			// second u64 containing block to write
			u64 num2 = arr[enddiv];
			// set block to zeros using two bit masks
			u64 mask2 = mask << (end + 1);
			mask >>= 64 - start;
			num &= mask;
			num2 &= mask2;
			// write less significant part of the value
			u64 valcpy = val;
			valcpy <<= start;
			arr[startdiv] = num | valcpy;
			// write more significant part of the value
			val >>= (64 - start);
			arr[enddiv] = num2 | val;
		}
	}

	void dealloc() {
		delete[] arr;
	}
};

int main() {
	u64 n, k;
	cout << "enter number of integers: ";
	cin >> n;
	cout << "enter number of bits per integer (<=64): ";
	cin >> k;

	u64 max = 2;
	max <<= k - 1;

	PackedIntegerArray a(n, k);

	vector<u64> ints;
	mt19937 g(random_device{}());
	uniform_int_distribution<u64> d(0, max - 1);

	for (int x = 0; x < 2; x++) {
		ints.clear();

		cout << "generating random numbers\n\n";
		for (u64 i = 0; i < n; i++) {
			ints.push_back(d(g));
		}

		for (u64 i = 0; i < n; i++) {
			a.set(i, ints[i]);
		}

		cout << "original number : number retrieved from integer array\n";
		for (u64 i = 0; i < n; i++) {
			u64 original = ints[i];
			u64 fromarray = a.get(i);
			cout << original << " : " << fromarray << "\n";

			if (original != fromarray) {
				cout << "ERROR, numbers did not match\n";
				return 1;
			}
		}

		cout << "-----\n";
	}

	return 0;
}

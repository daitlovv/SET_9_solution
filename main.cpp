#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <numeric>
#include <functional>

class StringGenerator {
    std::mt19937 gen;
    std::uniform_int_distribution<> lengthDist;
    std::uniform_int_distribution<> charDist;
    std::string chars;

    void initCharSet() {
        for (char c = 'A'; c <= 'Z'; c++) {
	        chars += c;
        }

        for (char c = 'a'; c <= 'z'; c++) {
	        chars += c;
        }

        for (char c = '0'; c <= '9'; c++) {
	        chars += c;
        }

    	chars += "!@#%:;^&*()-.";
    }

    std::string generateRandomString() {
        int len = lengthDist(gen);
        std::string result;

        for (int i = 0; i < len; i++) {
            result += chars[charDist(gen)];
        }

        return result;
    }

public:
    StringGenerator(int minLen = 10, int maxLen = 200) : gen(std::random_device{}()), lengthDist(minLen, maxLen), charDist(0, 74) {
        initCharSet();
    }

    std::vector<std::string> generateRandomArray(int size) {
        std::vector<std::string> arr(size);

        for (int i = 0; i < size; i++) {
            arr[i] = generateRandomString();
        }

        return arr;
    }

    std::vector<std::string> generateReverseSortedArray(int size) {
        std::vector<std::string> arr = generateRandomArray(size);
        std::sort(arr.begin(), arr.end());

        std::reverse(arr.begin(), arr.end());
        return arr;
    }

    std::vector<std::string> generateNearlySortedArray(int size, int swaps) {
        std::vector<std::string> arr = generateRandomArray(size);
        std::sort(arr.begin(), arr.end());

        for (int i = 0; i < swaps; i++) {
            int pos1 = std::uniform_int_distribution<>(0, size - 1)(gen);
            int pos2 = std::uniform_int_distribution<>(0, size - 1)(gen);
            std::swap(arr[pos1], arr[pos2]);
        }

        return arr;
    }

    std::vector<std::string> generateWithCommonPrefix(int size, const std::string& prefix) {
        std::vector<std::string> arr = generateRandomArray(size);

        for (size_t i = 0; i < arr.size(); i++) {
            arr[i] = prefix + arr[i];
        }
        return arr;
    }
};

long long comparisons = 0;

void resetComparisons() {
	comparisons = 0;
}


class StringSortTester {
	int runs;
public:
	StringSortTester(int runs = 3) : runs(runs) {}

	struct Result {
		long long time_us;
		long long comparisons;
	};

	Result measure(std::vector<std::string> data, std::function<void(std::vector<std::string>&)> sortFn) {
		std::vector<long long> times, comps;
		for (int r = 0; r < runs; r++) {
			std::vector<std::string> copy = data;
			resetComparisons();
			auto start = std::chrono::high_resolution_clock::now();
			sortFn(copy);
			auto end = std::chrono::high_resolution_clock::now();
			times.push_back(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());
			comps.push_back(comparisons);
		}
		return {
			std::accumulate(times.begin(), times.end(), 0LL) / runs,
			std::accumulate(comps.begin(), comps.end(), 0LL) / runs
		};
	}
};


bool compareStrings(const std::string& a, const std::string& b) {
    size_t i = 0;
    size_t minLen = std::min(a.length(), b.length());

    while (i < minLen && a[i] == b[i]) {
        comparisons++;
        i++;
    }

    if (i < minLen) {
        comparisons++;
        return a[i] < b[i];
    }

    comparisons++;

    return a.length() < b.length();
}

void quickSort(std::vector<std::string>& arr, int left, int right) {
    if (left < right) {
    	int mid = (left + right) / 2;
    	std::swap(arr[static_cast<size_t>(left)], arr[static_cast<size_t>(mid)]);
    	std::string pivot = arr[static_cast<size_t>(left)];

    	int i = left;
    	int j = right;

    	while (i <= j) {
    		while (compareStrings(arr[static_cast<size_t>(i)], pivot)) {
    			i++;
    		}
    		while (compareStrings(pivot, arr[static_cast<size_t>(j)])) {
    			j--;
    		}
    		if (i <= j) {
    			std::swap(arr[static_cast<size_t>(i)], arr[static_cast<size_t>(j)]);
    			i++;
    			j--;
    		}
    	}

    	quickSort(arr, left, j);
    	quickSort(arr, i, right);
    }
}

void merge(std::vector<std::string>& arr, std::vector<std::string>& temp, int left, int mid, int right) {
    int i = left;
    int j = mid + 1;
    int k = left;

    while (i <= mid && j <= right) {
        if (compareStrings(arr[static_cast<size_t>(i)], arr[static_cast<size_t>(j)])) {
            temp[static_cast<size_t>(k)] = arr[static_cast<size_t>(i)];
            i++;
            k++;
        } else {
            temp[static_cast<size_t>(k)] = arr[static_cast<size_t>(j)];
            j++;
            k++;
        }
    }

    while (i <= mid) {
        temp[static_cast<size_t>(k)] = arr[static_cast<size_t>(i)];
        i++;
        k++;
    }

    while (j <= right) {
        temp[static_cast<size_t>(k)] = arr[static_cast<size_t>(j)];
        j++;
        k++;
    }

    for (i = left; i <= right; i++) {
        arr[static_cast<size_t>(i)] = temp[static_cast<size_t>(i)];
    }
}

void mergeSort(std::vector<std::string>& arr, std::vector<std::string>& temp, int left, int right) {
    if (left < right) {
    	int mid = (left + right) / 2;
    	mergeSort(arr, temp, left, mid);
    	mergeSort(arr, temp, mid + 1, right);
    	merge(arr, temp, left, mid, right);
    }
}

void stringQuickSort(std::vector<std::string>& arr, int left, int right, int depth) {
    if (left < right) {
        int mid = (left + right) / 2;
        std::swap(arr[static_cast<size_t>(left)], arr[static_cast<size_t>(mid)]);
        std::string pivot = arr[static_cast<size_t>(left)];

        int lt = left;
        int gt = right;
        int i = left + 1;

        while (i <= gt) {
            char c1 = static_cast<size_t>(depth) < arr[static_cast<size_t>(i)].length() ? arr[static_cast<size_t>(i)][static_cast<size_t>(depth)] : 0;
            char c2 = static_cast<size_t>(depth) < pivot.length() ? pivot[static_cast<size_t>(depth)] : 0;

        	comparisons++;

            if (c1 < c2) {
                std::swap(arr[static_cast<size_t>(lt)], arr[static_cast<size_t>(i)]);
                lt++;
                i++;
            } else if (c1 > c2) {
                std::swap(arr[static_cast<size_t>(i)], arr[static_cast<size_t>(gt)]);
                gt--;
            } else {
                i++;
            }
        }

        stringQuickSort(arr, left, lt - 1, depth);

        if (depth < static_cast<int>(pivot.length())) {
            stringQuickSort(arr, lt, gt, depth + 1);
        }

        stringQuickSort(arr, gt + 1, right, depth);
    }
}

int lcpLength(const std::string& a, const std::string& b, int k) {
    size_t i = static_cast<size_t>(k);
    size_t minLen = std::min(a.length(), b.length());

    while (i < minLen && a[i] == b[i]) {
        comparisons++;
        i++;
    }

    return static_cast<int>(i);
}

std::pair<int, int> lcpCompare(const std::string& a, const std::string& b, int k) {
    int l = lcpLength(a, b, k);

    if (l < static_cast<int>(std::min(a.length(), b.length()))) {
        if (a[static_cast<size_t>(l)] < b[static_cast<size_t>(l)]) {
            return { -1, l };
        }
        return { 1, l };
    }

    if (a.length() == b.length()) {
        return { 0, l };
    }

    if (a.length() < b.length()) {
        return { -1, l };
    }

    return { 1, l };
}

void lcpMerge(std::vector<std::pair<std::string, int>>& arr, std::vector<std::pair<std::string, int>>& temp, int left, int mid, int right) {
    int i = left;
    int j = mid + 1;
    int k = left;

    while (i <= mid && j <= right) {
        if (arr[static_cast<size_t>(i)].second > arr[static_cast<size_t>(j)].second) {
            temp[static_cast<size_t>(k)] = arr[static_cast<size_t>(i)];
            i++;
            k++;
        } else if (arr[static_cast<size_t>(i)].second < arr[static_cast<size_t>(j)].second) {
            temp[static_cast<size_t>(k)] = arr[static_cast<size_t>(j)];
            j++;
            k++;
        } else {
            std::pair<int, int> res = lcpCompare(arr[static_cast<size_t>(i)].first, arr[static_cast<size_t>(j)].first, arr[static_cast<size_t>(i)].second);
            if (res.first == -1) {
                temp[static_cast<size_t>(k)] = arr[static_cast<size_t>(i)];
                i++;
                k++;
                arr[static_cast<size_t>(j)].second = res.second;
            } else {
                temp[static_cast<size_t>(k)] = arr[static_cast<size_t>(j)];
                j++;
                k++;
                arr[static_cast<size_t>(i)].second = res.second;
            }
        }
    }

    while (i <= mid) {
        temp[static_cast<size_t>(k)] = arr[static_cast<size_t>(i)];
        i++;
        k++;
    }

    while (j <= right) {
        temp[static_cast<size_t>(k)] = arr[static_cast<size_t>(j)];
        j++;
        k++;
    }

    for (i = left; i <= right; i++) {
        arr[static_cast<size_t>(i)] = temp[static_cast<size_t>(i)];
    }
}

void lcpMergeSort(std::vector<std::pair<std::string, int>>& arr, std::vector<std::pair<std::string, int>>& temp, int left, int right) {
    if (left < right) {
        int mid = (left + right) / 2;
        lcpMergeSort(arr, temp, left, mid);
        lcpMergeSort(arr, temp, mid + 1, right);
        lcpMerge(arr, temp, left, mid, right);
    }
}

void msdRadixSort(std::vector<std::string>& arr, int pos, int alphabetSize, bool switchToQuick) {
    if (arr.size() <= 1) {
        return;
    }

    if (switchToQuick && static_cast<int>(arr.size()) < alphabetSize) {
        stringQuickSort(arr, 0, static_cast<int>(arr.size()) - 1, pos);
        return;
    }

    std::vector<int> count(alphabetSize + 1, 0);

    for (size_t i = 0; i < arr.size(); i++) {
        int index = static_cast<size_t>(pos) < arr[i].length() ? static_cast<int>(arr[i][static_cast<size_t>(pos)]) % alphabetSize : 0;
        count[static_cast<size_t>(index + 1)]++;
    }

    for (int i = 1; i <= alphabetSize; i++) {
        count[static_cast<size_t>(i)] += count[static_cast<size_t>(i - 1)];
    }

    std::vector<std::string> output(arr.size());
    for (size_t i = 0; i < arr.size(); i++) {
        int index = static_cast<size_t>(pos) < arr[i].length() ? static_cast<int>(arr[i][static_cast<size_t>(pos)]) % alphabetSize : 0;
        output[static_cast<size_t>(count[static_cast<size_t>(index)]++)] = arr[i];
    }

    arr = output;

    int start = 0;
    for (int i = 0; i <= alphabetSize; i++) {
        if (count[static_cast<size_t>(i)] - start > 1) {
            std::vector<std::string> sub(arr.begin() + start, arr.begin() + count[static_cast<size_t>(i)]);
            msdRadixSort(sub, pos + 1, alphabetSize, switchToQuick);
            std::copy(sub.begin(), sub.end(), arr.begin() + start);
        }
        start = count[static_cast<size_t>(i)];
    }
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    StringGenerator generator;

    int alphabetSize = 74;

	StringSortTester tester(3);

    std::ofstream results("results.csv");
    results << "size,type,algorithm,time_us,comparisons\n";

    std::vector<int> sizes;
    for (int i = 100; i <= 3000; i += 100) {
        sizes.push_back(i);
    }

    std::vector<std::string> fullRandom = generator.generateRandomArray(3000);
    std::vector<std::string> fullReverse = generator.generateReverseSortedArray(3000);
    std::vector<std::string> fullNearly = generator.generateNearlySortedArray(3000, 50);
    std::vector<std::string> fullPrefix = generator.generateWithCommonPrefix(3000, "prefix_");

    std::vector<std::string> types = {"random", "reverse", "nearly", "prefix"};
    std::vector<std::vector<std::string>> datasets = {fullRandom, fullReverse, fullNearly, fullPrefix};

    for (size_t t = 0; t < types.size(); t++) {
        for (int size : sizes) {
            std::vector<std::string> data(datasets[t].begin(), datasets[t].begin() + size);


        	auto qsRes = tester.measure(data, [&](std::vector<std::string>& arr) {
				quickSort(arr, 0, static_cast<int>(arr.size()) - 1);
			});
        	results << size << "," << types[t] << ",quicksort," << qsRes.time_us << "," << qsRes.comparisons << "\n";

        	auto msRes = tester.measure(data, [&](std::vector<std::string>& arr) {
				std::vector<std::string> temp(arr.size());
				mergeSort(arr, temp, 0, static_cast<int>(arr.size()) - 1);
			});
        	results << size << "," << types[t] << ",mergesort," << msRes.time_us << "," << msRes.comparisons << "\n";

        	auto sqsRes = tester.measure(data, [&](std::vector<std::string>& arr) {
				stringQuickSort(arr, 0, static_cast<int>(arr.size()) - 1, 0);
			});
        	results << size << "," << types[t] << ",string_quicksort," << sqsRes.time_us << "," << sqsRes.comparisons << "\n";

        	auto smsRes = tester.measure(data, [&](std::vector<std::string>& arr) {
				std::vector<std::pair<std::string, int>> pairs(arr.size());

				for (int i = 0; i < static_cast<int>(arr.size()); i++) {
					pairs[i] = { arr[i], 0 };
				}
				std::vector<std::pair<std::string, int>> temp(arr.size());
				lcpMergeSort(pairs, temp, 0, static_cast<int>(arr.size()) - 1);

				for (int i = 0; i < static_cast<int>(arr.size()); i++) {
					arr[i] = pairs[i].first;
				}
			});
        	results << size << "," << types[t] << ",string_mergesort," << smsRes.time_us << "," << smsRes.comparisons << "\n";

        	auto msdRes = tester.measure(data, [&](std::vector<std::string>& arr) {
				msdRadixSort(arr, 0, alphabetSize, false);
			});
        	results << size << "," << types[t] << ",msd_radix," << msdRes.time_us << "," << msdRes.comparisons << "\n";

        	auto msdSwRes = tester.measure(data, [&](std::vector<std::string>& arr) {
				msdRadixSort(arr, 0, alphabetSize, true);
			});
        	results << size << "," << types[t] << ",msd_radix_switch," << msdSwRes.time_us << "," << msdSwRes.comparisons << "\n";
        }
    }

    results.close();
    return 0;
}
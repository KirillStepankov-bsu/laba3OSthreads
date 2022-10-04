#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <thread>
#include <map>

void
multiplyBlocks(const std::vector<int> &a, const std::vector<int> &b, std::vector<int> &c, int n, int r, int I, int J) {
    for (int i = I; i < std::min(I + r, n); ++i) {
        for (int j = J; j < std::min(J + r, n); ++j) {
            c[i * n + j] = 0;
            for (int k = 0; k < n; k++) {
                c[i * n + j] += a[i * n + k] * b[k * n + j];
            }
        }
    }
}

void multiplyMatrixSeq(const std::vector<int> &a, const std::vector<int> &b, std::vector<int> &c, int n, int r) {
    for (int I = 0; I < n; I += r) {
        for (int J = 0; J < n; J += r) {
            multiplyBlocks(a, b, c, n, r, I, J);
        }
    }
}

void multiplyMatrixThread(const std::vector<int> &a, const std::vector<int> &b, std::vector<int> &c, int n, int r) {
    std::vector<std::thread> threads;
    for (int I = 0; I < n; I += r) {
        for (int J = 0; J < n; J += r) {
            threads.emplace_back(multiplyBlocks, std::cref(a), std::cref(b), std::ref(c), n, r, I, J);
        }
    }
    for (std::thread &thread: threads) {
        thread.join();
    }
}

std::vector<int> generateMatrix(int n) {
    std::vector<int> result(n * n);
    std::mt19937 rnd(std::chrono::system_clock::now().time_since_epoch().count());
    for (int i = 0; i < n * n; i++) {
        result[i] = static_cast<int>(rnd()) % 201 - 100;
    }
    return result;
}

std::map<int, std::pair<double, double>>
generateTimes(const std::vector<int> &matrixA, const std::vector<int> &matrixB, std::vector<int> &matrixC, int n) {
    std::map<int, std::pair<double, double>> times;
    for (int i = 1; i < n; ++i) {
        auto start = std::chrono::system_clock::now();
        multiplyMatrixSeq(matrixA, matrixB, matrixC, n, i);
        auto end = std::chrono::system_clock::now();
        auto durSeq = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        start = std::chrono::system_clock::now();
        multiplyMatrixThread(matrixA, matrixB, matrixC, n, i);
        end = std::chrono::system_clock::now();
        auto durThread = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        times[i] = std::pair<double, double>(durSeq, durThread);
    }
    return times;
}

int main() {
    int n = 300;
    std::vector<int> matrixA = generateMatrix(n);
    std::vector<int> matrixB = generateMatrix(n);
    std::vector<int> matrixC(n * n, 0);
    std::map<int, std::pair<double, double>> times = generateTimes(matrixA, matrixB, matrixC, n);

    for (int i = 0; i < times.size(); ++i) {
        std::cout << "Amount of blocks: " << i + 1 << "; Duration for sequential: " << times[i + 1].first
                  << "; Duration for thread: " << times[i + 1].second << "; Speed up: " << 1.*times[i+1].first/times[i+1].second << std::endl;
    }
}
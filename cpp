#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <string>
#include <limits>

using Matrix = std::vector<std::vector<long long>>; // supports big ints comfortably

// ---------- I/O helpers ----------
void printMatrix(const Matrix& M, const std::string& title = "Matrix") {
    if (M.empty()) { std::cout << title << " (empty)\n"; return; }
    std::cout << title << " (" << M.size() << "x" << M[0].size() << "):\n";
    // Find max width for nice alignment
    int w = 1;
    for (const auto& row : M)
        for (auto v : row) {
            std::ostringstream oss;
            oss << v;
            w = std::max(w, (int)oss.str().size());
        }
    w = std::max(w, 2); // at least 2 like "01" in sample
    for (const auto& row : M) {
        for (auto v : row) std::cout << std::setw(w + 1) << v; // +1 for spacing
        std::cout << "\n";
    }
}

bool loadMatricesFromFile(const std::string& path, Matrix& A, Matrix& B) {
    std::ifstream fin(path);
    if (!fin) {
        std::cerr << "Error: cannot open file '" << path << "'.\n";
        return false;
    }
    long long N;
    if (!(fin >> N) || N <= 0) {
        std::cerr << "Error: first value must be a positive integer N.\n";
        return false;
    }
    A.assign((size_t)N, std::vector<long long>((size_t)N));
    B.assign((size_t)N, std::vector<long long>((size_t)N));

    for (long long i = 0; i < N; ++i)
        for (long long j = 0; j < N; ++j)
            if (!(fin >> A[i][j])) { std::cerr << "Error: not enough values for matrix A.\n"; return false; }

    for (long long i = 0; i < N; ++i)
        for (long long j = 0; j < N; ++j)
            if (!(fin >> B[i][j])) { std::cerr << "Error: not enough values for matrix B.\n"; return false; }

    return true;
}

// ---------- Operations ----------
Matrix add(const Matrix& A, const Matrix& B) {
    size_t n = A.size();
    Matrix C(n, std::vector<long long>(n, 0));
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < n; ++j)
            C[i][j] = A[i][j] + B[i][j];
    return C;
}

Matrix multiply(const Matrix& A, const Matrix& B) {
    size_t n = A.size();
    Matrix C(n, std::vector<long long>(n, 0));
    for (size_t i = 0; i < n; ++i)
        for (size_t k = 0; k < n; ++k) {
            long long aik = A[i][k];
            for (size_t j = 0; j < n; ++j)
                C[i][j] += aik * B[k][j];
        }
    return C;
}

long long mainDiagonalSum(const Matrix& M) {
    long long s = 0;
    for (size_t i = 0; i < M.size(); ++i) s += M[i][i];
    return s;
}

long long secondaryDiagonalSum(const Matrix& M) {
    long long s = 0;
    size_t n = M.size();
    for (size_t i = 0; i < n; ++i) s += M[i][n - 1 - i];
    return s;
}

bool swapRows(Matrix& M, size_t r1, size_t r2) {
    size_t n = M.size();
    if (r1 >= n || r2 >= n) return false;
    std::swap(M[r1], M[r2]);
    return true;
}

bool swapCols(Matrix& M, size_t c1, size_t c2) {
    size_t n = M.size();
    if (n == 0 || c1 >= n || c2 >= n) return false;
    for (size_t i = 0; i < n; ++i) std::swap(M[i][c1], M[i][c2]);
    return true;
}

bool updateCell(Matrix& M, size_t r, size_t c, long long newVal) {
    size_t n = M.size();
    if (r >= n || c >= n) return false;
    M[r][c] = newVal;
    return true;
}

// ---------- Menu ----------
void clearStdin() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int main(int argc, char* argv[]) {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string path;
    if (argc >= 2) {
        path = argv[1];
    } else {
        std::cout << "Enter input filename: ";
        std::getline(std::cin, path);
    }

    Matrix A, B;
    if (!loadMatricesFromFile(path, A, B)) return 1;

    printMatrix(A, "Matrix A");
    printMatrix(B, "Matrix B");

    while (true) {
        std::cout << "\n--- Menu ---\n"
                  << "1) Add A + B\n"
                  << "2) Multiply A * B\n"
                  << "3) Sum diagonals of A\n"
                  << "4) Sum diagonals of B\n"
                  << "5) Swap rows in A\n"
                  << "6) Swap columns in A\n"
                  << "7) Update a cell in A\n"
                  << "8) Print A\n"
                  << "9) Print B\n"
                  << "0) Exit\n"
                  << "Choice: ";

        int choice;
        if (!(std::cin >> choice)) { clearStdin(); continue; }

        if (choice == 0) break;

        switch (choice) {
            case 1: {
                Matrix C = add(A, B);
                printMatrix(C, "A + B");
                break;
            }
            case 2: {
                Matrix C = multiply(A, B);
                printMatrix(C, "A * B");
                break;
            }
            case 3: {
                long long m = mainDiagonalSum(A);
                long long s = secondaryDiagonalSum(A);
                std::cout << "A main diagonal sum    = " << m << "\n";
                std::cout << "A secondary diagonal sum = " << s << "\n";
                break;
            }
            case 4: {
                long long m = mainDiagonalSum(B);
                long long s = secondaryDiagonalSum(B);
                std::cout << "B main diagonal sum    = " << m << "\n";
                std::cout << "B secondary diagonal sum = " << s << "\n";
                break;
            }
            case 5: {
                size_t r1, r2;
                std::cout << "Enter two row indices (0-based): ";
                if (!(std::cin >> r1 >> r2)) { clearStdin(); break; }
                if (swapRows(A, r1, r2)) printMatrix(A, "A after row swap");
                else std::cout << "Invalid row indices.\n";
                break;
            }
            case 6: {
                size_t c1, c2;
                std::cout << "Enter two column indices (0-based): ";
                if (!(std::cin >> c1 >> c2)) { clearStdin(); break; }
                if (swapCols(A, c1, c2)) printMatrix(A, "A after column swap");
                else std::cout << "Invalid column indices.\n";
                break;
            }
            case 7: {
                size_t r, c; long long val;
                std::cout << "Enter row, col (0-based) and new value: ";
                if (!(std::cin >> r >> c >> val)) { clearStdin(); break; }
                if (updateCell(A, r, c, val)) printMatrix(A, "A after update");
                else std::cout << "Invalid indices.\n";
                break;
            }
            case 8: printMatrix(A, "Matrix A"); break;
            case 9: printMatrix(B, "Matrix B"); break;
            default: std::cout << "Invalid choice.\n";
        }
    }

    return 0;
}
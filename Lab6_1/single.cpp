#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <chrono>
#include <boost/program_options.hpp>

#define EPS 1e-6
#define MAX_ITER 1000000

using namespace std;
namespace po = boost::program_options;

const float values[4] = {10.0f, 20.0f, 30.0f, 40.0f};
float interpolate(float x, float y) {
    float top = values[0] * (1 - x) + values[1] * x;
    float bottom = values[2] * (1 - x) + values[3] * x;
    return bottom * (1 - y) + top * y;
}

int main(int argc, char *argv[]) {
    int rows, cols;
    double precision;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("rows", po::value<int>(&rows)->default_value(128), "number of rows")
        ("cols", po::value<int>(&cols)->default_value(128), "number of columns")
        ("precision", po::value<double>(&precision)->default_value(EPS), "precision for convergence");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << desc << endl;
        return 1;
    }

    vector<double> grid(rows * cols, 0.0);
    vector<double> newGrid(rows * cols, 0.0);

    for (int j = 0; j < cols; j++) {
        grid[j] = interpolate(static_cast<float>(j) / (cols - 1), 0);  
        grid[(rows - 1) * cols + j] = interpolate(static_cast<float>(j) / (cols - 1), 1);  
    }

    for (int i = 0; i < rows; i++) {
        grid[i * cols] = interpolate(0, static_cast<float>(i) / (rows - 1));  
        grid[i * cols + (cols - 1)] = interpolate(1, static_cast<float>(i) / (rows - 1));  
    }

    int niters = 0;
    double maxDiff;

    auto start = chrono::high_resolution_clock::now();

    do {
        niters++;
        maxDiff = 0.0;

        for (int i = 1; i < rows - 1; i++) {
            for (int j = 1; j < cols - 1; j++) {
                newGrid[i * cols + j] = (grid[(i - 1) * cols + j] +
                                          grid[(i + 1) * cols + j] +
                                          grid[i * cols + (j - 1)] +
                                          grid[i * cols + (j + 1)]) * 0.25;

                maxDiff = max(maxDiff, fabs(grid[i * cols + j] - newGrid[i * cols + j]));
            }
        }

        swap(grid, newGrid);

    } while (maxDiff >= precision && niters < MAX_ITER);

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    cout << "# Heat 2D: grid: rows " << rows << ", cols " << cols << endl;
    cout << "# niters " << niters << ", max difference " << fixed << setprecision(6) << maxDiff << endl;
    cout << "# elapsed time: " << elapsed.count() << " seconds" << endl;

    if (rows >= 13 && cols >= 13) {
        cout << "# Final grid values (13x13):" << endl;
        for (int i = rows / 2 - 6; i < rows / 2 + 7; i++) { 
            for (int j = cols / 2 - 6; j < cols / 2 + 7; j++) {
                cout << fixed << setprecision(6) << grid[i * cols + j] << " ";
            }
            cout << endl;
        }
    }

    return 0;
}

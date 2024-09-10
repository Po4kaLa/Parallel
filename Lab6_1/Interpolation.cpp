#include <iostream>
#include <vector>
#include <fstream>

const int N = 128;
const float values[4] = {10.0f, 20.0f, 30.0f, 40.0f}; 

float interpolate(float x, float y) {
    float top = values[0] * (1 - x) + values[1] * x;
    float bottom = values[2] * (1 - x) + values[3] * x;
    return bottom * (1 - y) + top * y;
}

int main() {
    std::vector<std::vector<float>> grid(N, std::vector<float>(N, 0.0f));

    for (int i = 0; i < N; ++i) {
        grid[0][i] = interpolate(static_cast<float>(i) / (N - 1), 0); 
        grid[N - 1][i] = interpolate(static_cast<float>(i) / (N - 1), 1); 
        grid[i][0] = interpolate(0, static_cast<float>(i) / (N - 1)); 
        grid[i][N - 1] = interpolate(1, static_cast<float>(i) / (N - 1)); 
    }

    std::ofstream outFile("grid.txt");
    if (outFile.is_open()) {
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                outFile << grid[i][j] << " ";
            }
            outFile << std::endl;
        }
        outFile.close();
        std::cout << "Сетка успешно сохранена в grid.txt" << std::endl;
    } else {
        std::cerr << "Не удалось открыть файл для записи." << std::endl;
    }

    return 0;
}

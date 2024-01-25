#include <iostream>

int main() {
    vector<int> pixels = {1, 2, 3, 4, 5, 6};
    vector<int> tempMatrix = {};
    int height = 2;
    int width = 3;
    int index = 0;
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            tempMatrix[j][i] = pixels[index++];
        }
    }
    for(int i = 0; i<8; i++)
}
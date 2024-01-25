#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>

using namespace std;

#pragma pack(1)
struct BMPHeader {
    uint16_t signature;
    uint32_t fileSize;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t dataOffset;
    uint32_t headerSize;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bitsPerPixel;
    uint32_t compression;
    uint32_t dataSize;
    int32_t horizontalRes;
    int32_t verticalRes;
    uint32_t colors;
    uint32_t importantColors;
};
#pragma pack(pop)

struct Pixel {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

double applyGaussianBlur(const vector<vector<Pixel>>& tempMatrix, int x, int y, int width, int height, int radius, int channel) {
    double result = 0.0;
    double totalWeight = 0.0;

    double weights[5] = { 0.003, 0.013, 0.022, 0.013, 0.003 };

    for (int i = -radius; i <= radius; i++) {
        int neighborX = x + i;

        if (neighborX >= 0 && neighborX < width) {
            double weight = weights[i + radius];
            switch (channel) {
            case 0: // Red channel
                result += weight * tempMatrix[neighborX][y].red;
                break;
            case 1: // Green channel
                result += weight * tempMatrix[neighborX][y].green;
                break;
            case 2: // Blue channel
                result += weight * tempMatrix[neighborX][y].blue;
                break;
            }
            totalWeight += weight;
        }
    }

    return result / totalWeight;
}

Pixel applyGaussianBlurToPixel(const vector<vector<Pixel>>& tempMatrix, int x, int y, int width, int height, int radius) {
    Pixel blurredPixel;
    blurredPixel.red = static_cast<uint8_t>(applyGaussianBlur(tempMatrix, x, y, width, height, radius, 0));
    blurredPixel.green = static_cast<uint8_t>(applyGaussianBlur(tempMatrix, x, y, width, height, radius, 1));
    blurredPixel.blue = static_cast<uint8_t>(applyGaussianBlur(tempMatrix, x, y, width, height, radius, 2));
    return blurredPixel;
}

vector<vector<Pixel>> rotateImage(const vector<Pixel>& pixels, int width, int height) {
    vector<vector<Pixel>> tempMatrix(width, vector<Pixel>(height));

    int index = 0;
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            tempMatrix[j][i] = pixels[index++];
        }
    }

    return tempMatrix;
}

void applyGaussianBlurToImage(vector<Pixel>& pixels, const vector<vector<Pixel>>& tempMatrix, int width, int height, int radius) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            pixels[y * width + x] = applyGaussianBlurToPixel(tempMatrix, x, y, width, height, radius);
        }
    }
}

void processImage(const char* inputFileName, const char* outputFileName) {
    ifstream img(inputFileName, ios::binary);
    ofstream new_img(outputFileName, ios::binary | ios::trunc);

    if (!img || !new_img) {
        cerr << "Error: Unable to open input or output file." << endl;
        exit(1);
    }

    BMPHeader header;
    img.read(reinterpret_cast<char*>(&header), sizeof(header));

    int width = header.width;
    int height = header.height;

    vector<Pixel> pixels(width * height);
    img.read(reinterpret_cast<char*>(pixels.data()), pixels.size() * sizeof(Pixel));
    if (!img) {
        cerr << "Error: Unable to read pixel data from input file." << endl;
        exit(1);
    }

    // Rotate the image
    vector<vector<Pixel>> tempPixels = rotateImage(pixels, width, height);

    // Apply Gaussian Blur
    applyGaussianBlurToImage(pixels, tempPixels, width, height, 5);  // Adjust the radius as needed

    new_img.write(reinterpret_cast<char*>(&header), sizeof(header));
    new_img.write(reinterpret_cast<char*>(pixels.data()), pixels.size() * sizeof(Pixel));

    img.close();
    new_img.close();

    cout << "Files successfully processed." << endl;
}

int main() {
    processImage("sun.bmp", "new_img.bmp");

    return 0;
}
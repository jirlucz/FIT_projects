/**
 * @file LineMandelCalculator.h
 * @author Jiri Kotek <xkotek09@stud.fit.vutbr.cz>
 * @brief Implementation of Mandelbrot calculator that uses SIMD paralelization over lines
 * @date 24.10.2025
 */

#include <BaseMandelCalculator.h>


class LineMandelCalculator : public BaseMandelCalculator
{
public:
    LineMandelCalculator(unsigned matrixBaseSize, unsigned limit);
    ~LineMandelCalculator();
    int *calculateMandelbrot();

private:
    int *data;

    const int BLOCK_SIZE = 256;
    // const int BLOCK_SIZE = 4096;

    float *zReal_vec;
    float *zImag_vec;
    float *x_vec;
    int *iter_vec;

    void initializeVectors(int i);
};
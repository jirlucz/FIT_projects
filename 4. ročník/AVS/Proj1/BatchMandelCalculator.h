/**
 * @file BatchMandelCalculator.h
 * @author Jiri Kotek <xkotek09@stud.fit.vutbr.cz>
 * @brief Implementation of Mandelbrot calculator that uses SIMD paralelization over small batches
 * @date 24.10.2025
 */
#ifndef BATCHMANDELCALCULATOR_H
#define BATCHMANDELCALCULATOR_H

#include <BaseMandelCalculator.h>

class BatchMandelCalculator : public BaseMandelCalculator
{
public:
    BatchMandelCalculator(unsigned matrixBaseSize, unsigned limit);
    ~BatchMandelCalculator();
    int * calculateMandelbrot();

private:
    int *data;

    const int BLOCK_SIZE_Y = 8; 
    const int BLOCK_SIZE_X = 128;  

    // Pomocná pole pro vektorizaci
    float *zReal_vec;   // Závislá hodnota mezi iteracemi 
    float *zImag_vec;   // Závislá hodnota mezi iteracemi 
    float *x_vec;       // Uloží konstantní reálné části 'c',       x je stejné pro pro každý řádek ve sloupci
    float *y_vec;       // Uloží konstantní imaginární části 'c',   y je stejné pro každý sloupec na řádku
    int *iter_vec;      // Určuje, které body nepřesáhly prahovou hodnotu

    
    
};

#endif
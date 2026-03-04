/**
 * @file BatchMandelCalculator.cc
 * @author Jiri Kotek <xkotek09@stud.fit.vutbr.cz>
 * @brief Implementation of Mandelbrot calculator that uses SIMD paralelization over small batches
 * @date 24.10.2025
 */

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include <stdlib.h>
#include <stdexcept>
#include <cstring>
#include <immintrin.h>

#include "BatchMandelCalculator.h"

BatchMandelCalculator::BatchMandelCalculator (unsigned matrixBaseSize, unsigned limit) :
    BaseMandelCalculator(matrixBaseSize, limit, "BatchMandelCalculator")
{
    data = (int *) aligned_alloc(64, height * width * sizeof(int));

    zReal_vec = (float *)aligned_alloc(64, BLOCK_SIZE_X  * sizeof(float));
    zImag_vec = (float *)aligned_alloc(64, BLOCK_SIZE_X  * sizeof(float));
    x_vec = (float *)aligned_alloc(64,BLOCK_SIZE_X  * sizeof(float));
    y_vec = (float *)aligned_alloc(64, BLOCK_SIZE_X  * sizeof(float));
    iter_vec = (int *)aligned_alloc(64, BLOCK_SIZE_X  * sizeof(int));

    memset(data, 0, width * height * sizeof(int));
}


BatchMandelCalculator::~BatchMandelCalculator() {
    free(data);
    free(zReal_vec);
    free(zImag_vec);
    free(x_vec);
    free(y_vec);
    free(iter_vec);

    data = NULL;
    zReal_vec = NULL;
    zImag_vec = NULL;
    x_vec = NULL;
    y_vec = NULL;
    iter_vec = NULL;
}


int * BatchMandelCalculator::calculateMandelbrot () {
    const int half_height = (height + 1) / 2;

    // 1. Vnější smyčka přes BLOKY řádků (i_block)
    for (int i_block = 0; i_block < half_height; i_block += BLOCK_SIZE_Y) 
    {
        // 2. Vnější smyčka přes BLOKY sloupců (j_block)
        for (int j_block = 0; j_block < width; j_block += BLOCK_SIZE_X)
        {
            int i_end = std::min(i_block + BLOCK_SIZE_Y, half_height);
            int j_end = std::min(j_block + BLOCK_SIZE_X, width);
            const int block_width = j_end - j_block; // počet platných prvků v tomto bloku

            // Inicializace x_vec - konstantní pro celý blok sloupců
            #pragma omp simd aligned(x_vec:64)
            for (int j = 0; j < block_width; j++)
            {
                x_vec[j] = x_start + (j_block + j) * dx;
            }

            // 3. Smyčka přes řádky v bloku
            for (int i = i_block; i < i_end; i++)
            {
                float y = y_start + i * dy;
                int* prow = data + i * width;
                int* prow_offset = prow + j_block; // začátek pro zápis tohoto bloku
                int sym_i = height - 1 - i;
                int* psym = data + sym_i * width;

                // Inicializace bloku
                #pragma omp simd aligned(x_vec, zReal_vec, zImag_vec, iter_vec:64)
                for (int j = 0; j < block_width; j++)
                {
                    zReal_vec[j] = x_vec[j];
                    zImag_vec[j] = y;
                    iter_vec[j] = limit;
                }

                // Výpočetní smyčka přes přes prvky v bloku
                for (int iter = 0; iter < limit; ++iter)
                {
                    int all_escaped = 1;

                    #pragma omp simd reduction(&:all_escaped) aligned(x_vec, zReal_vec, zImag_vec, iter_vec:64)
                    for (int j = 0; j < block_width; j++)
                    {
                        // Kontrola, zda je bod má smysl stále počítat
                        if (iter_vec[j] == limit)
                        {
                            float real_pow2 = zReal_vec[j] * zReal_vec[j];
                            float imaginary_pow2 = zImag_vec[j] * zImag_vec[j];

                            if ((real_pow2 + imaginary_pow2) > 4.0f)
                            {
                                // Uložení počtu iterací
                                iter_vec[j] = iter;
                            }
                            else
                            {
                                zImag_vec[j] = 2.0f * zReal_vec[j] * zImag_vec[j] + y;
                                zReal_vec[j] = real_pow2 - imaginary_pow2 + x_vec[j];
                                all_escaped = 0;
                            }
                        }
                    }

                    if (all_escaped) break;
                }

                // Ukládání výsledků a zrcadlení
                #pragma omp simd aligned(iter_vec:64)
                for (int j = 0; j < block_width; j += 16)
                {
                    __m512i vec = _mm512_load_si512((__m512i*)&iter_vec[j]);
                    
                    // zápis do hlavního pole
                    _mm512_stream_si512((__m512i*)&prow_offset[j], vec);
                    
                    if (i != sym_i)
                    {
                        _mm512_stream_si512((__m512i*)&psym[j_block + j], vec);
                    }
                }
            } // konec smyčky i
            
        } // konec smyčky j_block
    } // konec smyčky i_block

    return data;
}
/**
 * @file LineMandelCalculator.cc
 * @author Jiri Kotek <xkotek09@stud.fit.vutbr.cz>
 * @brief Implementation of Mandelbrot calculator that uses SIMD paralelization over lines
 * @date 24.10.2025
 */
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include <stdlib.h>
#include <cstring>
#include <immintrin.h>


#include "LineMandelCalculator.h"


LineMandelCalculator::LineMandelCalculator (unsigned matrixBaseSize, unsigned limit) :
    BaseMandelCalculator(matrixBaseSize, limit, "LineMandelCalculator")
{
    data =          (int *) aligned_alloc(64, height * width * sizeof(int));
    memset(data, 0, width * height * sizeof(int));

    zReal_vec =     (float *)aligned_alloc(64, width * sizeof(float));
    zImag_vec =     (float *)aligned_alloc(64, width * sizeof(float));
    x_vec =         (float *)aligned_alloc(64,width * sizeof(float));
    iter_vec =      (int *)aligned_alloc(64, width * sizeof(int));
    
}

LineMandelCalculator::~LineMandelCalculator() {
    free(data);
    free(zReal_vec);
    free(zImag_vec);
    free(x_vec);
    free(iter_vec);

    data = NULL;
    zReal_vec = NULL;
    zImag_vec = NULL;
    x_vec = NULL;
    iter_vec = NULL;
}


int * LineMandelCalculator::calculateMandelbrot() {
    
    // Vnější smyčka přes řádky (i)
    for (int i = 0; i < (height + 1) / 2; i++)
    {
        float y = y_start + i * dy; 
        int* prow = data + i * width; // Ukazatel na začátek aktuálního řádku

        // Iterace přes bloky velikosti BLOCK_SIZE v rámci řádku
        for (int j_block = 0; j_block < width; j_block += BLOCK_SIZE)
        {
            int j_end = std::min(j_block + BLOCK_SIZE, width);

            // Inicializace vektorů pro aktuální blok
            #pragma omp simd aligned(x_vec, zReal_vec, zImag_vec, iter_vec : 64)
            for (int j = j_block; j < j_end; j++) {
                float x = x_start + j * dx;  // c_real
                x_vec[j] = x;
                zReal_vec[j] = x;   // z_real = c_real
                zImag_vec[j] = y;   // z_imag = c_imag 
                iter_vec[j] = limit; // Maska aktivních bodů
            }

            // 2. výpočetní smyčka přes počet iterací
            for (int iter = 0; iter < limit; ++iter)
            {
                int all_escaped = 1; 

                // 3. Vnitřní smyčka přes sloupce 'j' v BLOKU)
                #pragma omp simd reduction(&:all_escaped) aligned(x_vec, zReal_vec, zImag_vec, iter_vec : 64)
                for (int j = j_block; j < j_end; j++)
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
                            float tmp = 2.0f * zReal_vec[j] * zImag_vec[j] + y; 
                            zReal_vec[j] = real_pow2 - imaginary_pow2 + x_vec[j];
                            zImag_vec[j] = tmp;
                            
                            all_escaped = 0; // Alespoň jeden bod je stále aktivní
                        }
                    }
                } // Konec smyčky 'j' (v rámci bloku)

                // 4. Kontrola redukce v rámci bloku
                if (all_escaped)
                {
                    break;
                }

            } // Konec smyčky 'iter'
        } // Konec smyčky 'j_block'

        // 5. Uložení výsledků
        #pragma omp simd aligned(iter_vec: 64)
        for (int j = 0; j < width; j++)
        {
            prow[j] = iter_vec[j];
        }

        int sym_i = height - 1 - i; // Index symetrického řádku

        if (i != sym_i) 
        {
            int* psym = data + sym_i * width; // Ukazatel na symetrický řádek
            memcpy(psym, prow, width * sizeof(int));
        }

    } // Konec smyčky 'i' (řádky)

    return data;
}

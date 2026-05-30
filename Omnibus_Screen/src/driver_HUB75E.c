/**
 ******************************************************************************
 * @file           : driver_HUB75E.c
 * @brief          : Screen driver
 * @author         : Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 ******************************************************************************
 * @details
 *
 *
 * @attention
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "hub75.pio.h"
#include "driver_HUB75E.h"
#include "service_scheduler.h"
#include "frame_assembly.h"

//Images
//#include "mountains_128x64_rgb565.h"
//#include "color_matrix.h"
// #include "smpte.h"
// #include "test_corners.h"
// #include "image3.h"
// #include "image2.h"

/* Defines -------------------------------------------------------------------*/
#define WIDTH 128
#define HEIGHT 64

/* Variables ---------------------------------------------------------------- */
DRIVERHUB75E driver_HUB75E;

/* Functions -----------------------------------------------------------------*/

//==============================================================================
void driver_HUB75E_init(void)
//==============================================================================
{
    driver_HUB75E.pio = pio0;
    driver_HUB75E.sm_data = 0;
    driver_HUB75E.sm_row = 1;
    driver_HUB75E.data_prog_offs = pio_add_program(driver_HUB75E.pio, &hub75_data_rgb888_program);
    driver_HUB75E.row_prog_offs = pio_add_program(driver_HUB75E.pio, &hub75_row_program);

    hub75_data_rgb888_program_init(driver_HUB75E.pio, driver_HUB75E.sm_data, driver_HUB75E.data_prog_offs, DATA_BASE_PIN, CLK_PIN);
    hub75_row_program_init(driver_HUB75E.pio, driver_HUB75E.sm_row, driver_HUB75E.row_prog_offs, ROWSEL_BASE_PIN, ROWSEL_N_PINS, STROBE_PIN);

    scheduler_phase_array[PHASE_DRIVER_HUB75E_OUT] = do_nothing;
}

//==============================================================================
static inline uint32_t gamma_correct_565_888(uint16_t pix)
//==============================================================================
{
    uint32_t r_gamma = pix & 0xf800u;
    r_gamma *= r_gamma;
    uint32_t g_gamma = pix & 0x07e0u;
    g_gamma *= g_gamma;
    uint32_t b_gamma = pix & 0x001fu;
    b_gamma *= b_gamma;
    return (b_gamma >> 2 << 0) | (g_gamma >> 14 << 16) | (r_gamma >> 24 << 8);
}

//==============================================================================
void driver_HUB75E_run(void)
//==============================================================================
{
    const uint16_t *img;
    static uint32_t gc_row[2][WIDTH]; 

    img = (const uint16_t *)frame_buffer_1;

    for (int rowsel = 0; rowsel < (1 << ROWSEL_N_PINS); rowsel++)
    {
        for (int columnsel = 0; columnsel < WIDTH; columnsel++)
        {
            gc_row[0][columnsel] = gamma_correct_565_888(img[rowsel * WIDTH + columnsel]);
            gc_row[1][columnsel] = gamma_correct_565_888(img[((1u << ROWSEL_N_PINS) + rowsel) * WIDTH + columnsel]);
        }
        for (int bit = 0; bit < 8; ++bit)
        {
            hub75_data_rgb888_set_shift(driver_HUB75E.pio,
                                        driver_HUB75E.sm_data,
                                        driver_HUB75E.data_prog_offs,
                                        bit);
            for (int columnsel = 0; columnsel < WIDTH; columnsel++)
            {
                pio_sm_put_blocking(driver_HUB75E.pio,
                                    driver_HUB75E.sm_data,
                                    gc_row[0][columnsel]);
                pio_sm_put_blocking(driver_HUB75E.pio,
                                    driver_HUB75E.sm_data,
                                    gc_row[1][columnsel]);
            }
            // Dummy pixel per lane
            pio_sm_put_blocking(driver_HUB75E.pio, driver_HUB75E.sm_data, 0);
            pio_sm_put_blocking(driver_HUB75E.pio, driver_HUB75E.sm_data, 0);
            // SM is finished when it stalls on empty TX FIFO
            hub75_wait_tx_stall(driver_HUB75E.pio, driver_HUB75E.sm_data);
            // Also check that previous OEn pulse is finished, else things can get out of sequence
            hub75_wait_tx_stall(driver_HUB75E.pio, driver_HUB75E.sm_row);

            // Latch row data, pulse output enable for new row.
            pio_sm_put_blocking(driver_HUB75E.pio, driver_HUB75E.sm_row, rowsel | (100u * (1u << bit) << 5));
        }
    }
}

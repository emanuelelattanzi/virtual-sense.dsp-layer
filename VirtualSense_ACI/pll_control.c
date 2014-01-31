/*
 * $$$MODULE_NAME pll_control.c
 *
 * $$$MODULE_DESC pll_control.c
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

/** @file Pll_control.c
 *
 *  @brief PLL functional layer sample source file
 *
 *
 * \page    page12  CSL PLL EXAMPLE DOCUMENTATION
 *
 * \section PLL   PLL EXAMPLE
 *
 * \subsection PLLx    TEST DESCRIPTION:
 *        This test code verifies the functionality of CSL PLL module. PLL module
 * on the C5505/C5515 DSP is used to generate clock for CPU and peripherals.
 * A 32KHz input clock is supplied to the PLL using which different system clock
 * values are generated. Different system clock values are generated by
 * configuring the PLL to different divider values.
 *
 * During the test PLL module will be configured to the 60MHz clock frequency
 * using PLL_config() API. Configured values are read back and verified using
 * PLL_getConfig() APIs. All the values should match the configured values
 * except the test lock mon value which will reflect on the registers only
 * after the PLL is up. Values read from the PLL are displayed on the CCS
 * "stdout" window. Manual inspection is required to verify the test success.
 *
 * C5515 DSP PLL register bit fileds are little different than that of C5505 DSP.
 * Use the 'PLL_Config' values defined 12.288MHz - 120MHz to verify PLL
 * configuration.
 *
 * NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5505 AND
 * C5515. MAKE SURE THAT PROPER CHIP VERSION MACRO CHIP_5505/CHIP_5515 IS
 * DEFINED IN THE FILE c55xx_csl\inc\csl_general.h.
 *
 * \subsection PLLy    TEST PROCEDURE:
 *  @li Open the CCS and connect the target (C5505/C5515 EVM)
 *  @li Open the project "CSL_PLL_Example.pjt" and build it
 *  @li Load the program on to the target
 *  @li Run the program and observe the test result
 *  @li Repeat the test in Release mode
 *
 * \subsection PLLz    TEST RESULT:
 *  @li All the CSL APIs should return success
 *  @li Configuration values read from the PLL should match with the actual
 *      configured values except the test lock mon value.
 *
 */

/* ============================================================================
 * Revision History
 * ================
 * 15-Sept-2008 Created
 * ============================================================================
 */

//#include<stdio.h>
#include "VirtualSense_ACIcfg.h"

#include "csl_pll.h"
#include "csl_general.h"
#include "csl_pllAux.h"

PLL_Obj pllObj;
PLL_Config pllCfg1;

PLL_Handle hPll;

#if ((defined(CHIP_5515)) || (defined(CHIP_5514)))

//const PLL_Config pllCfg_12p288MHz   = {0x8173, 0x8000, 0x0806, 0x0000};
//const PLL_Config pllCfg_40MHz       = {0x8988, 0x8000, 0x0806, 0x0201};
//const PLL_Config pllCfg_50MHz       = {0x8BE8, 0x8000, 0x0806, 0x0201};
//const PLL_Config pllCfg_60MHz       = {0x8724, 0x8000, 0x0806, 0x0000};
//const PLL_Config pllCfg_75MHz       = {0x88ED, 0x8000, 0x0806, 0x0000};
const PLL_Config pllCfg_100MHz      = {0x8BE8, 0x8000, 0x0806, 0x0000};
const PLL_Config pllCfg_120MHz      = {0x8E4A, 0x8000, 0x0806, 0x0000};
//const PLL_Config pllCfg_100MHz_ExtClk12Mhz  = {0x8645, 0x00BD, 0x0806, 0x0000};
//const PLL_Config pllCfg_120MHz_ExtClk12Mhz  = {0x87CC, 0x00C4, 0x0806, 0x0000};
//const PLL_Config pllCfg_144MHz_ExtClk12Mhz  = {0x8960, 0x00C4, 0x0806, 0x0000}; // debug

#else

const PLL_Config pllCfg_12p288MHz = {0x82ED, 0x8000, 0x0806, 0x0200};
const PLL_Config pllCfg_40MHz     = {0x8262, 0x8000, 0x0806, 0x0300};
const PLL_Config pllCfg_60MHz     = {0x81C8, 0xB000, 0x0806, 0x0000};
const PLL_Config pllCfg_75MHz     = {0x823B, 0x9000, 0x0806, 0x0000};
const PLL_Config pllCfg_100MHz    = {0x82FA, 0x8000, 0x0806, 0x0000};
const PLL_Config pllCfg_120MHz    = {0x8392, 0xA000, 0x0806, 0x0000};

#endif


PLL_Config *pConfigInfo;

CSL_Status pll_sample()
{
    CSL_Status status;
	volatile int i;

    status = PLL_init(&pllObj, CSL_PLL_INST_0);
    if (status != CSL_SOK)
    {
        return status;
    }

    hPll = (PLL_Handle)(&pllObj);

    PLL_reset(hPll);

    status = PLL_bypass(hPll);
    if (status != CSL_SOK)
    {
        return status;
    }

    /* Configure the PLL */
    //pConfigInfo = (PLL_Config *)&pllCfg_40MHz;
    pConfigInfo = (PLL_Config *)&pllCfg_100MHz;
    //pConfigInfo = (PLL_Config *)&pllCfg_120MHz;
    //pConfigInfo = (PLL_Config *)&pllCfg_100MHz_ExtClk12Mhz;
    //pConfigInfo = (PLL_Config *)&pllCfg_120MHz_ExtClk12Mhz;

    status = PLL_config (hPll, pConfigInfo);
    if (status != CSL_SOK)
    {
        return(status);
    }

    status = PLL_getConfig(hPll, &pllCfg1);
    if (status != CSL_SOK)
    {
        return status;
    }

    /* Wait for PLL to stabilize */
    for (i=0; i<100; i++);

    status = PLL_enable(hPll);
    if (status != CSL_SOK)
    {
        return status;
    }

    // set DSP_LDO to 1.05V
    //*(volatile ioport unsigned int *)(0x7004) |= 0x0002;

    return CSL_SOK;
}


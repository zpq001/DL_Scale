#ifndef _SCALE_CONFIG_H
#define _SCALE_CONFIG_H

// will move to RB_Config.h
#define CONFIG_MELSI_SAMPLING_FREQ      80 //366.0
#define CONFIG_WEIGHT_CYCLES_PER_SEC    25//32
#define CONFIG_MAX_UPSCALE_TEST_POINT   3
#define CONFIG_MAX_WT_DIGITS            8
#define CONFIG_MIN_INCR_INDEX           0
#define CONFIG_MAX_INCR_INDEX           19
#define CONFIG_MAX_DP                   4

#define clearButton                     0x0217
#define pushButtonTare                  0x0213
#define keyboardEnteredTare             0x0214

#define NEAR_ZERO                       0.000001
#define INTERAL_DIVISION                1000000.0

#define SCALECAPACITYLIMIT              999980.00

#endif // #ifndef _SCALE_CONFIG_H

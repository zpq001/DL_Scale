#ifndef  _MOTION_H
#define  _MOTION_H

//#include "RB_Type.h"

//#include "IDNet.h"
#include "comm.h"
#include "SetupParameterTable.h"
#define MOTION_ENTRIES	50


// class MOTION
struct MotionData
{
  SETUP_MOTIONRANGE  sensitivityInD;                     // motion sensitivity (Divisions)
  long            sensitivityInCounts;                // converted to raw counts
  long            periodInCycles;                     // number of readings required to detect "no motion".It can vary from 3 to 10.
  bool          inMotionFlag;                       // TRUE  = motion,FALSE = no motion
  bool          motionChangedFlag;                  // TRUE  = motion changed,FALSE = motion stayed the same
  long            readingsBuffer[MOTION_ENTRIES];     // last twenty raw weight counts
  unsigned short  bufferWritePointer;                 // pointer to last entry
  SETUP_NOMOTIONINTERVAL   sensitivityInterval;
  //! Callback function for re-initialize data members
  // void (*reInitializeDataMembers)(void *);
};

typedef struct MotionData MOTION;
extern MOTION g_motiondata;
extern int32_t m_motionCounter;
// extern void (*MOTION_ReInitializeDataMembers)(MOTION *this);

void MOTION_Init(MOTION *this);
// void MOTION_InstallReInitialization(MOTION *this, void (*pCallbackFunction)(void *));
float MOTION_GetMotionSensitivityD(MOTION *this);
void MOTION_SetMotionSensitivityD(MOTION *this, unsigned short D);
void MOTION_SetStabilityTimePeriod(MOTION *this, int cycles);
long MOTION_GetStabilityTimePeriod(MOTION *this);
bool MOTION_GetMotion(MOTION *this);
bool MOTION_GetMotionChanged(MOTION *this);
void MOTION_Calibrate(MOTION *this, float countsPerD);
void MOTION_ProcessMotion(MOTION *this, long counts);
bool MOTION_Detect(int32_t currentCounts, int32_t *pPreviousCounts, int32_t motionRange, int32_t settlingCounter);


#endif

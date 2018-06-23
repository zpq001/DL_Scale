#ifndef  _UNIT_H
#define  _UNIT_H

typedef	enum
{
    UNIT_none = 0,   
    UNIT_g,
    UNIT_kg,
    UNIT_lb,
    UNIT_oz,
    UNIT_t,
    UNIT_ton,
    UNIT_newton,
}UNIT_tSymbol;

//! WARNING C-Syntax: NO character is allowed after the backspace '\' !!!
//! Parameter definitions, which are used in the parameter table
#define UNIT_STRING_LIST		   	               \
    "None",                                        \
    "g",                                           \
    "kg",                                          \
    "lb",                                          \
    "oz",                                          \
    "t",                                           \
    "ton",                                         \
    "n",                                           \


typedef enum
{
    PRIME_UNIT = 0,
    SECOND_UNIT
} UNIT_tType;

// class UNIT
struct UnitData
{
    UNIT_tType currUnitType;        // this LEGAL_UNIT represents the
									// primary or secondary units for
									// scale.
    UNIT_tType calUnitType;       // calibration Units -- either
									// primary or secondary units.
    UNIT_tSymbol priUnits;			// 1 = lb, 2 = kg, 3 = g
    UNIT_tSymbol secUnits;			// 1 = lb, 2 = kg, 3 = g

//    float customUnitFactor;         // Custom Units Conversion Factor
    bool bPowerUpUnit;              // false = start up at primary weigh unit, true = start up at current weigh unit
    
    //! Callback function for re-initialize data members
    // void (*reInitializeDataMembers)(void *);
};

typedef struct UnitData UNIT;

extern UNIT g_unitdata;

void UNIT_Init(UNIT *this);
// void UNIT_InstallReInitialization(UNIT *this, void (*pCallbackFunction)(void *));
//float UNIT_ConvertUnits(UNIT *this, UNIT_tSymbol calUnit, UNIT_tSymbol srcUnit, UNIT_tSymbol dstUnit, float weight);
double UNIT_ConvertUnits(UNIT_tSymbol srcUnit, UNIT_tSymbol dstUnit, double weight);
double UNIT_ConvertUnitType(UNIT *this, UNIT_tType srcType, UNIT_tType dstType, double weight);
double UNIT_RoundToNearest1_2_5(double inc, UNIT_tSymbol dstUnit);
UNIT_tType UNIT_GetCurrentUnitType(UNIT *this);
UNIT_tSymbol UNIT_GetUnit(UNIT *this, UNIT_tType unitType);
// int UNIT_GetIncrIndex(float incr);
char * UNIT_GetUnitStringbyUnitType(UNIT *this, UNIT_tType unitType);
char * UNIT_GetUnitStringbyUnit(UNIT_tSymbol unit); 
extern const char *unitString[];
#endif

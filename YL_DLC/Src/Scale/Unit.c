/*-----------------------------------------------------------------------*
 * Filename....: Unit.c
 * Scope.......: Unit switch implementation
 * Last Change.:
 *
 * Version      Date                      Description
 *   1.0        7/6/07                    New
 *
 *          COPYRIGHT (C) 1993.  All Rights Reserved.
 *       Mettler Toledo Corporation, MTCZ.
 *------------------------------------------------------------------------*/
#include <math.h>
#include <string.h>
#include "ScaleConfig.h"
#include "Scale.h"
#include "Unit.h"

UNIT g_unitdata;

const double gramsPerUnit[] =
{
	1.0,			// none
	1.0,			// g
	1000.0,			// GRAMS PER KILOGRAM, kg
	453.59237, 		// GRAMS PER POUND, lb
	28.349523125,	// GRAMS PER OUNCE, oz
	1000000.0,		// GRAMS PER METRIC TON, t
	907184.74, 		// GRAMS PER SHORT (ADVOIR) TON, tons
	1000.0,            // Custom (assume equal to 1 gram)
//	28.349523125,	// GRAMS PER OUNCE, oz
//	0.885922597654,	// P  (1 / 1.12876678239)
//	31.1034768,		// GRAMS PER TROY OUNCE, ozt
//	1.555173843,	// GRAMS PER PENNY WEIGHT, dwt
//	28.349523125,	// GRAMS PER LB-OUNCE
//	101.97162,		// GRAMS PER NEWTON
};

const char *unitString[] = {
    UNIT_STRING_LIST
};
//extern SCALE scale;

/*---------------------------------------------------------------------*
 * Name         : UNIT_Init
 * Prototype in : Unit.h
 * Description  : constructor
 * Return value : none
 * Unit Testing Complete Date:   Tested By:  Test Unit Used:
 *---------------------------------------------------------------------*/
void UNIT_Init(UNIT *this)
{
    // this->reInitializeDataMembers(this);
//    this->customUnitFactor = 1.0;
}

/*---------------------------------------------------------------------*
 * Name         : UNIT_InstallReInitialization
 * Prototype in : Unit.h
 * Description  : Callback function for re-initialize UNIT data members.
 *                This function will be installed from application.
 * Return value : none
 * Unit Testing Complete Date:   Tested By:  Test Unit Used:
 *---------------------------------------------------------------------*/
//void UNIT_InstallReInitialization(UNIT *this, void (*pCallbackFunction)(void *))
//{
//	this->reInitializeDataMembers = pCallbackFunction;
//}

//float UNIT_ConvertUnits(UNIT *this, UNIT_tSymbol calUnit, UNIT_tSymbol srcUnit, UNIT_tSymbol dstUnit, float weight)
double UNIT_ConvertUnits(UNIT_tSymbol srcUnit, UNIT_tSymbol dstUnit, double weight)
{
    double gramsPerSrcUnit, gramsPerDstUnit,value;
    
	if (srcUnit != dstUnit)
	{
        gramsPerSrcUnit = gramsPerUnit[srcUnit];
        gramsPerDstUnit = gramsPerUnit[dstUnit];
	    value = weight * gramsPerSrcUnit / gramsPerDstUnit;
       
	    return value;
	}
	else
		return weight;
}

double UNIT_ConvertUnitType(UNIT *this, UNIT_tType srcType, UNIT_tType dstType, double weight)
{
    UNIT_tSymbol  srcUnit, dstUnit;
    if (srcType == PRIME_UNIT)
        srcUnit = this->priUnits;
    else
        srcUnit = this->secUnits;

    if (dstType == PRIME_UNIT)
        dstUnit = this->priUnits;
    else
        dstUnit = this->secUnits;
    return UNIT_ConvertUnits(srcUnit, dstUnit, weight);
}

double UNIT_RoundToNearest1_2_5(double inc, UNIT_tSymbol dstUnit)
{
	double new_inc[4];
	double diff[4];
//	float diff_x;
	int i, j;
	
	if (inc < NEAR_ZERO)
		new_inc[0] = 0.0;
	else
	{
		new_inc[0] = 100000.0;
		while (new_inc[0] > inc)
			new_inc[0] *= .1;
	}

	new_inc[1] = new_inc[0] * 2.0;
	new_inc[2] = new_inc[0] * 5.0;
	new_inc[3] = new_inc[0] * 10.0;

	diff[0]	   = fabs(new_inc[0] - inc);
	diff[1]	   = fabs(new_inc[1] - inc);
	diff[2]	   = fabs(new_inc[2] - inc);
	diff[3]	   = fabs(new_inc[3] - inc);

	for (i = 0; i < 4; i++) 
	{
		for (j = 0; j < 4; j++)
			if (diff[i] > diff[j]) break;
		if ( j == 4 ) break;
	}

	// handle 4oz, 8oz, and 6ozt increments. dpb 06-apr-95


	return new_inc[i];
}

UNIT_tType UNIT_GetCurrentUnitType(UNIT *this)
{
    return this->currUnitType;
}

UNIT_tSymbol UNIT_GetUnit(UNIT *this, UNIT_tType unitType)
{
    UNIT_tSymbol unit;
    
    switch (unitType)
    {
        case PRIME_UNIT:
            unit = this->priUnits;
            break;
            
        case SECOND_UNIT:
            unit = this->secUnits;
            break;
            
        default:
            unit = this->priUnits;
            break;
    }
    return unit;
}



char * UNIT_GetUnitStringbyUnitType(UNIT *this, UNIT_tType unitType)
{
    UNIT_tSymbol unit;
    
    unit = UNIT_GetUnit(this, unitType);
    return (char *)unitString[unit];
}

char * UNIT_GetUnitStringbyUnit(UNIT_tSymbol unit)
{
    return (char *)unitString[unit];
}

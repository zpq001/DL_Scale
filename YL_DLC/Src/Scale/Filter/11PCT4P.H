/***********************************************************
;Filter Filename:   11pct4p.flt
;11.00%, 4 Pole Mayer2 IIR Filter, Filename: 11pct4p.flt


;	2nd Order Cell #1 Amplification =  1.14273793e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -6.68202290e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  1.25297463e-01
;	2nd Order Cell #2 Amplification =  1.44133806e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -4.91163079e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  6.76983034e-02
;*/

FILT_COEF filter_11_4 = { /* Form I Mayer Filter,  c language Coefficients */
		    2,	/* # of cells */
		    {	/*    amplif        den coef 2      den coef 3 */
			{ 1.14273793e-01,-6.68202290e-01, 1.25297463e-01},
			{ 1.44133806e-01,-4.91163079e-01, 6.76983034e-02}
		    }
		 };


/*        Filter Step Response
;
;     Overshoot is       12 ppm or  0.00120 %
;     Samples to settle to 1/1000         11
;     Samples to settle to 1/5000         12
;     Samples to settle to 1/10000        13
;     Samples to settle to 1/50000        13
;     Samples to settle to 1/100000       15
;*/

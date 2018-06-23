/***********************************************************
;Filter Filename:   14pct4p.flt
;14.00%, 4 Pole Mayer2 IIR Filter, Filename: 14pct4p.flt


;	2nd Order Cell #1 Amplification =  1.94881686e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -2.36118669e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  1.56454122e-02
;	2nd Order Cell #2 Amplification =  2.09491327e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.70160030e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  8.12533850e-03
;*/

FILT_COEF filter_14_4 = { /* Form I Mayer Filter,  c language Coefficients */
			2,	/* # of cells */
			{	/*    amplif        den coef 2      den coef 3 */
			{ 1.94881686e-01,-2.36118669e-01, 1.56454122e-02},
			{ 2.09491327e-01,-1.70160030e-01, 8.12533850e-03}
			}
		 };


/*        Filter Step Response
;
;     Overshoot is        0 ppm or  0.00000 %
;     Samples to settle to 1/1000          8
;     Samples to settle to 1/5000          9
;     Samples to settle to 1/10000         9
;     Samples to settle to 1/50000        10
;     Samples to settle to 1/100000       10
;*/

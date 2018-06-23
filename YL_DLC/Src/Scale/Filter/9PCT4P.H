/***********************************************************
;Filter Filename:   9pct4p.flt
;9.00%, 4 Pole Mayer2 IIR Filter, Filename: 9pct4p.flt


;	2nd Order Cell #1 Amplification =  7.58281588e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -9.49895955e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  2.53208590e-01
;	2nd Order Cell #2 Amplification =  1.07001435e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -7.15763212e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  1.43768951e-01
;*/

FILT_COEF filter_9_4 = { /* Form I Mayer Filter,  c language Coefficients */
			2,	/* # of cells */
			{	/*    amplif        den coef 2      den coef 3 */
			{ 7.58281588e-02,-9.49895955e-01, 2.53208590e-01},
			{ 1.07001435e-01,-7.15763212e-01, 1.43768951e-01}
			}
		 };


/*        Filter Step Response
;
;     Overshoot is      671 ppm or  0.06707 %
;     Samples to settle to 1/1000         13
;     Samples to settle to 1/5000         17
;     Samples to settle to 1/10000        18
;     Samples to settle to 1/50000        20
;     Samples to settle to 1/100000       21
;*/

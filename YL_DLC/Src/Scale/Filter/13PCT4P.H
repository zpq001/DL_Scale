/***********************************************************
;Filter Filename:   13pct4p.flt
;13.00%, 4 Pole Mayer2 IIR Filter, Filename: 13pct4p.flt


;	2nd Order Cell #1 Amplification =  1.64944276e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -3.80947471e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  4.07245737e-02
;	2nd Order Cell #2 Amplification =  1.86397872e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -2.75746087e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  2.13375757e-02
;*/

FILT_COEF filter_13_4 = { /* Form I Mayer Filter, c language Coefficients */
		    2,	/* # of cells */
		    {	/*    amplif        den coef 2      den coef 3 */
			{ 1.64944276e-01,-3.80947471e-01, 4.07245737e-02},
			{ 1.86397872e-01,-2.75746087e-01, 2.13375757e-02}
		    }
		 };


/*        Filter Step Response
;
;     Overshoot is        0 ppm or  0.00000 %
;     Samples to settle to 1/1000          9
;     Samples to settle to 1/5000         10
;     Samples to settle to 1/10000        10
;     Samples to settle to 1/50000        11
;     Samples to settle to 1/100000       12
;*/

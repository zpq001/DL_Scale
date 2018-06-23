/***********************************************************
;Filter Filename:   11pct6p.flt
;11.00%, 6 Pole Mayer2 IIR Filter, Filename: 11pct6p.flt


;	2nd Order Cell #1 Amplification =  1.05139838e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -7.28283101e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  1.48842455e-01
;	2nd Order Cell #2 Amplification =  1.33812965e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -5.49475319e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  8.47271774e-02
;	2nd Order Cell #3 Amplification =  1.45934560e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -4.81256780e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  6.49950198e-02
;*/

FILT_COEF filter_11_6 = {      /* Form I Mayer Filter,  c language Coefficients */
		    3,	/* # of cells */
		    {	/*    amplif        den coef 2      den coef 3 */
			{ 1.05139838e-01,-7.28283101e-01, 1.48842455e-01},
			{ 1.33812965e-01,-5.49475319e-01, 8.47271774e-02},
			{ 1.45934560e-01,-4.81256780e-01, 6.49950198e-02}
		    }
		 };


/*        Filter Step Response
;
;     Overshoot is        9 ppm or  0.00092 %
;     Samples to settle to 1/1000         14
;     Samples to settle to 1/5000         15
;     Samples to settle to 1/10000        16
;     Samples to settle to 1/50000        17
;     Samples to settle to 1/100000       17
;*/

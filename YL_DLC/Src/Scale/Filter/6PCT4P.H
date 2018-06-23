/***********************************************************
;Filter Filename:   6pct4p.flt
;6.00%, 4 Pole Mayer1 IIR Filter, Filename: 6pct4p.flt


;	2nd Order Cell #1 Amplification =  4.56384729e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.25785503e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  4.40408926e-01
;	2nd Order Cell #2 Amplification =  3.52016304e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.26491632e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  4.05722842e-01
;*/

FILT_COEF filter_6_4 = { /* Form I Mayer Filter,  c language Coefficients */
		    2,	/* # of cells */
		    {	/*    amplif        den coef 2      den coef 3 */
			{ 4.56384729e-02,-1.25785503e+00, 4.40408926e-01},
			{ 3.52016304e-02,-1.26491632e+00, 4.05722842e-01}
		    }
		 };


/*        Filter Step Response
;
;     Overshoot is      391 ppm or  0.03909 %
;     Samples to settle to 1/1000         19
;     Samples to settle to 1/5000         25
;     Samples to settle to 1/10000        26
;     Samples to settle to 1/50000        28
;     Samples to settle to 1/100000       28
;*/

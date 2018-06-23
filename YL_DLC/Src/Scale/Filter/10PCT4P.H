/***********************************************************
;Filter Filename:   10pct4p.flt
;10.00%, 4 Pole Mayer2 IIR Filter, Filename: 10pct4p.flt


;	2nd Order Cell #1 Amplification =  9.35362924e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -8.09950299e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  1.84095468e-01
;	2nd Order Cell #2 Amplification =  1.24919410e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -6.02033202e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  1.01710841e-01
;*/

FILT_COEF filter_10_4 = { /* Form I Mayer Filter,  c language Coefficients */
			2,	/* # of cells */
			{	/*    amplif        den coef 2      den coef 3 */
			{ 9.35362924e-02,-8.09950299e-01, 1.84095468e-01},
			{ 1.24919410e-01,-6.02033202e-01, 1.01710841e-01}
			}
		 };


/*        Filter Step Response
;
;     Overshoot is      110 ppm or  0.01104 %
;     Samples to settle to 1/1000         12
;     Samples to settle to 1/5000         13
;     Samples to settle to 1/10000        15
;     Samples to settle to 1/50000        18
;     Samples to settle to 1/100000       19
;*/

/***********************************************************
;Filter Filename:   10pct6p.flt
;10.00%, 6 Pole Mayer2 IIR Filter, Filename: 10pct6p.flt


;	2nd Order Cell #1 Amplification =  8.43966348e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -8.79465601e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  2.17052141e-01
;	2nd Order Cell #2 Amplification =  1.13832686e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -6.71029091e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  1.26359836e-01
;	2nd Order Cell #3 Amplification =  1.26877713e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -5.90261232e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  9.77720854e-02
;*/

FILT_COEF filter_10_6 = {      /* Form I Mayer Filter,  c language Coefficients */
		    3,	/* # of cells */
		    {	/*    amplif        den coef 2      den coef 3 */
			{ 8.43966348e-02,-8.79465601e-01, 2.17052141e-01},
			{ 1.13832686e-01,-6.71029091e-01, 1.26359836e-01},
			{ 1.26877713e-01,-5.90261232e-01, 9.77720854e-02}
		    }
		 };


/*        Filter Step Response
;
;     Overshoot is      108 ppm or  0.01083 %
;     Samples to settle to 1/1000         15
;     Samples to settle to 1/5000         16
;     Samples to settle to 1/10000        19
;     Samples to settle to 1/50000        22
;     Samples to settle to 1/100000       22
;*/

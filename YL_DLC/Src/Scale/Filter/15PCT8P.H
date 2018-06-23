/***********************************************************
;Filter Filename:   15pct8p.flt
;15.00%, 8 Pole Mayer2 IIR Filter, Filename: 15pct8p.flt


;	2nd Order Cell #1 Amplification =  2.24496499e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.05114653e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  3.10065097e-03
;	2nd Order Cell #2 Amplification =  2.30261166e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -8.07868409e-02
;Numerator(3) =  1.00000000e+00  Denominator(3) =  1.83150290e-03
;	2nd Order Cell #3 Amplification =  2.33172957e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -6.86299354e-02
;Numerator(3) =  1.00000000e+00  Denominator(3) =  1.32176284e-03
;	2nd Order Cell #4 Amplification =  2.34417139e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -6.34616286e-02
;Numerator(3) =  1.00000000e+00  Denominator(3) =  1.13018304e-03
;*/

FILT_COEF filter_15_8 = {      /* Form I Mayer Filter,  c language Coefficients */
		    4,	/* # of cells */
		    {	/*    amplif        den coef 2      den coef 3 */
			{ 2.24496499e-01,-1.05114653e-01, 3.10065097e-03},
			{ 2.30261166e-01,-8.07868409e-02, 1.83150290e-03},
			{ 2.33172957e-01,-6.86299354e-02, 1.32176284e-03},
			{ 2.34417139e-01,-6.34616286e-02, 1.13018304e-03}
		    }
		 };


/*        Filter Step Response
;
;     Overshoot is        0 ppm or  0.00000 %
;     Samples to settle to 1/1000         10
;     Samples to settle to 1/5000         11
;     Samples to settle to 1/10000        11
;     Samples to settle to 1/50000        12
;     Samples to settle to 1/100000       12
;*/

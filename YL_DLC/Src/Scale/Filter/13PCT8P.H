/***********************************************************
;Filter Filename:   13pct8p.flt
;13.00%, 8 Pole Mayer2 IIR Filter, Filename: 13pct8p.flt


;	2nd Order Cell #1 Amplification =  1.53770069e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -4.39002725e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  5.40830021e-02
;	2nd Order Cell #2 Amplification =  1.73198517e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -3.39562811e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  3.23568770e-02
;	2nd Order Cell #3 Amplification =  1.83527397e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -2.89392139e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  2.35017293e-02
;	2nd Order Cell #4 Amplification =  1.88046320e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -2.67965076e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  2.01503573e-02
;*/

FILT_COEF filter_13_8 = {      /* Form I Mayer Filter,  c language Coefficients */
		    4,	/* # of cells */
		    {	/*    amplif        den coef 2      den coef 3 */
			{ 1.53770069e-01,-4.39002725e-01, 5.40830021e-02},
			{ 1.73198517e-01,-3.39562811e-01, 3.23568770e-02},
			{ 1.83527397e-01,-2.89392139e-01, 2.35017293e-02},
			{ 1.88046320e-01,-2.67965076e-01, 2.01503573e-02}
		    }
		 };


/*        Filter Step Response
;
;     Overshoot is        0 ppm or  0.00000 %
;     Samples to settle to 1/1000         14
;     Samples to settle to 1/5000         15
;     Samples to settle to 1/10000        15
;     Samples to settle to 1/50000        16
;     Samples to settle to 1/100000       17
;*/

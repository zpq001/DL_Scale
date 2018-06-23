/***********************************************************
;Filter Filename:   10pct8p.flt
;10.00%, 8 Pole Mayer2 IIR Filter, Filename: 10pct8p.flt


;	2nd Order Cell #1 Amplification =  7.93687275e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -9.20095302e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  2.37570211e-01
;	2nd Order Cell #2 Amplification =  1.04964370e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -7.29470869e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  1.49328350e-01
;	2nd Order Cell #3 Amplification =  1.20404577e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -6.29631495e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  1.11249802e-01
;	2nd Order Cell #4 Amplification =  1.27556099e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -5.86210278e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  9.64346739e-02
;*/

FILT_COEF filter_10_8 = {      /* Form I Mayer Filter,  c language Coefficients */
		    4,	/* # of cells */
		    {	/*    amplif        den coef 2      den coef 3 */
			{ 7.93687275e-02,-9.20095302e-01, 2.37570211e-01},
			{ 1.04964370e-01,-7.29470869e-01, 1.49328350e-01},
			{ 1.20404577e-01,-6.29631495e-01, 1.11249802e-01},
			{ 1.27556099e-01,-5.86210278e-01, 9.64346739e-02}
		    }
		 };


/*        Filter Step Response
;
;     Overshoot is       99 ppm or  0.00987 %
;     Samples to settle to 1/1000         18
;     Samples to settle to 1/5000         19
;     Samples to settle to 1/10000        20
;     Samples to settle to 1/50000        25
;     Samples to settle to 1/100000       26
;*/

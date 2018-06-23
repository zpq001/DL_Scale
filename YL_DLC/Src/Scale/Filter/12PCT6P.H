/***********************************************************
;Filter Filename:   12pct6p.flt
;12.00%, 6 Pole Mayer2 IIR Filter, Filename: 12pct6p.flt


;	2nd Order Cell #1 Amplification =  1.29612364e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -5.74014380e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  9.24638353e-02
;	2nd Order Cell #2 Amplification =  1.55616348e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -4.29238570e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  5.17039635e-02
;	2nd Order Cell #3 Amplification =  1.66179112e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -3.74678888e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  3.93953355e-02
;*/

FILT_COEF filter_12_6 = {      /* Form I Mayer Filter,  c language Coefficients */
		    3,	/* # of cells */
		    {	/*    amplif        den coef 2      den coef 3 */
			{ 1.29612364e-01,-5.74014380e-01, 9.24638353e-02},
			{ 1.55616348e-01,-4.29238570e-01, 5.17039635e-02},
			{ 1.66179112e-01,-3.74678888e-01, 3.93953355e-02}
		    }
		 };


/*        Filter Step Response
;
;     Overshoot is        0 ppm or  0.00004 %
;     Samples to settle to 1/1000         13
;     Samples to settle to 1/5000         14
;     Samples to settle to 1/10000        15
;     Samples to settle to 1/50000        16
;     Samples to settle to 1/100000       16
;*/

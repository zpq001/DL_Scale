/***********************************************************
;Filter Filename:   14pct8p.flt
;14.00%, 8 Pole Mayer2 IIR Filter, Filename: 14pct8p.flt


;	2nd Order Cell #1 Amplification =  1.87027715e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -2.72768363e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  2.08792240e-02
;	2nd Order Cell #2 Amplification =  2.00571395e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -2.10102010e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  1.23875886e-02
;	2nd Order Cell #3 Amplification =  2.07569183e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.78682949e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  8.95968175e-03
;	2nd Order Cell #4 Amplification =  2.10590913e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.65304599e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  7.66824940e-03
;*/

FILT_COEF filter_14_8 = {      /* Form I Mayer Filter,  c language Coefficients */
		    4,	/* # of cells */
		    {	/*    amplif        den coef 2      den coef 3 */
			{ 1.87027715e-01,-2.72768363e-01, 2.08792240e-02},
			{ 2.00571395e-01,-2.10102010e-01, 1.23875886e-02},
			{ 2.07569183e-01,-1.78682949e-01, 8.95968175e-03},
			{ 2.10590913e-01,-1.65304599e-01, 7.66824940e-03}
		    }
		 };


/*        Filter Step Response
;
;     Overshoot is        0 ppm or  0.00000 %
;     Samples to settle to 1/1000         12
;     Samples to settle to 1/5000         13
;     Samples to settle to 1/10000        13
;     Samples to settle to 1/50000        14
;     Samples to settle to 1/100000       15
;*/

/***********************************************************
;Filter Filename:   2pct6p.flt
;2.00%, 6 Pole Mayer1 IIR Filter, Filename: 2pct6p.flt


;	2nd Order Cell #1 Amplification =  6.36922088e-03
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.74924726e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  7.74724141e-01
;	2nd Order Cell #2 Amplification =  4.71478418e-03
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.75590942e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  7.74768561e-01
;	2nd Order Cell #3 Amplification =  4.37637073e-03
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.73885780e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  7.56363286e-01
;*/

FILT_COEF filter_2_6 = {      /* Form I Mayer Filter,  c language Coefficients */
		    3,	/* # of cells */
		    {	/*    amplif        den coef 2      den coef 3 */
			{ 6.36922088e-03,-1.74924726e+00, 7.74724141e-01},
			{ 4.71478418e-03,-1.75590942e+00, 7.74768561e-01},
			{ 4.37637073e-03,-1.73885780e+00, 7.56363286e-01}
		    }
		 };


/*        Filter Step Response
;
;     Overshoot is      273 ppm or  0.02727 %
;     Samples to settle to 1/1000         77
;     Samples to settle to 1/5000         94
;     Samples to settle to 1/10000       100
;     Samples to settle to 1/50000       109
;     Samples to settle to 1/100000      112
;*/

/***********************************************************
;Filter Filename:   2pct4p.flt
;2.00%, 4 Pole Mayer1 IIR Filter, Filename: 2pct4p.flt


;	2nd Order Cell #1 Amplification =  6.32611614e-03
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.73740894e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  7.62713406e-01
;	2nd Order Cell #2 Amplification =  4.43082300e-03
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.74153139e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  7.59254686e-01
;*/

FILT_COEF filter_2_4 = {   /* Form I Mayer Filter,  c language Coefficients */
		    2,	/* # of cells */
		    {	/*    amplif        den coef 2      den coef 3 */
			{ 6.32611614e-03,-1.73740894e+00, 7.62713406e-01},
			{ 4.43082300e-03,-1.74153139e+00, 7.59254686e-01}
		    }
		 };


/*        Filter Step Response
;
;     Overshoot is       75 ppm or  0.00754 %
;     Samples to settle to 1/1000         62
;     Samples to settle to 1/5000         66
;     Samples to settle to 1/10000        68
;     Samples to settle to 1/50000        87
;     Samples to settle to 1/100000       92
;*/

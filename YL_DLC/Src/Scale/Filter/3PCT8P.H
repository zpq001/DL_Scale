/***********************************************************
;Filter Filename:   3pct8p.flt
;3.00%, 8 Pole Mayer1 IIR Filter, Filename: 3pct8p.flt


;	2nd Order Cell #1 Amplification =  1.36087869e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.63594517e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  6.90380316e-01
;	2nd Order Cell #2 Amplification =  1.32422239e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.59187974e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  6.44848639e-01
;	2nd Order Cell #3 Amplification =  9.90657039e-03
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.62530882e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  6.64935103e-01
;	2nd Order Cell #4 Amplification =  9.50234887e-03
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.61271016e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  6.50719557e-01
;*/

FILT_COEF filter_3_8 = {      /* Form I Mayer Filter,  c language Coefficients */
		    4,	/* # of cells */
		    {	/*    amplif        den coef 2      den coef 3 */
			{ 1.36087869e-02,-1.63594517e+00, 6.90380316e-01},
			{ 1.32422239e-02,-1.59187974e+00, 6.44848639e-01},
			{ 9.90657039e-03,-1.62530882e+00, 6.64935103e-01},
			{ 9.50234887e-03,-1.61271016e+00, 6.50719557e-01}
		    }
		 };


/*        Filter Step Response
;
;     Overshoot is       10 ppm or  0.00102 %
;     Samples to settle to 1/1000         64
;     Samples to settle to 1/5000         69
;     Samples to settle to 1/10000        70
;     Samples to settle to 1/50000        73
;     Samples to settle to 1/100000       79
;*/

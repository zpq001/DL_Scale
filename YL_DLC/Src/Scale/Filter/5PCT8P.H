/***********************************************************
;Filter Filename:   5pct8p.flt
;5.00%, 8 Pole Mayer1 IIR Filter, Filename: 5pct8p.flt


;	2nd Order Cell #1 Amplification =  3.40906795e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.40277680e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  5.39139517e-01
;	2nd Order Cell #2 Amplification =  3.27416269e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.34726545e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  4.78231957e-01
;	2nd Order Cell #3 Amplification =  2.58060043e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.38924436e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  4.92468375e-01
;	2nd Order Cell #4 Amplification =  2.48242116e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.37332574e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  4.72622584e-01
;*/

FILT_COEF filter_5_8 = {      /* Form I Mayer Filter,  c language Coefficients */
		    4,	/* # of cells */
		    {	/*    amplif        den coef 2      den coef 3 */
			{ 3.40906795e-02,-1.40277680e+00, 5.39139517e-01},
			{ 3.27416269e-02,-1.34726545e+00, 4.78231957e-01},
			{ 2.58060043e-02,-1.38924436e+00, 4.92468375e-01},
			{ 2.48242116e-02,-1.37332574e+00, 4.72622584e-01}
		    }
		 };


/*        Filter Step Response
;
;     Overshoot is       35 ppm or  0.00349 %
;     Samples to settle to 1/1000         37
;     Samples to settle to 1/5000         39
;     Samples to settle to 1/10000        40
;     Samples to settle to 1/50000        46
;     Samples to settle to 1/100000       48
;*/

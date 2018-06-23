/***********************************************************
;Filter Filename:   8pct8p.flt
;8.00%, 8 Pole Mayer1 IIR Filter, Filename: 8pct8p.flt


;	2nd Order Cell #1 Amplification =  7.60397609e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.06878581e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  3.72944856e-01
;	2nd Order Cell #2 Amplification =  7.20031545e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.01204882e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  3.00061434e-01
;	2nd Order Cell #3 Amplification =  6.04186477e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.05480584e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  2.96480432e-01
;	2nd Order Cell #4 Amplification =  5.82911102e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.03848435e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  2.71648790e-01
;*/

FILT_COEF filter_8_8 = {      /* Form I Mayer Filter,  c language Coefficients */
		    4,	/* # of cells */
		    {	/*    amplif        den coef 2      den coef 3 */
			{ 7.60397609e-02,-1.06878581e+00, 3.72944856e-01},
			{ 7.20031545e-02,-1.01204882e+00, 3.00061434e-01},
			{ 6.04186477e-02,-1.05480584e+00, 2.96480432e-01},
			{ 5.82911102e-02,-1.03848435e+00, 2.71648790e-01}
		    }
		 };


/*        Filter Step Response
;
;     Overshoot is      265 ppm or  0.02652 %
;     Samples to settle to 1/1000         22
;     Samples to settle to 1/5000         25
;     Samples to settle to 1/10000        26
;     Samples to settle to 1/50000        28
;     Samples to settle to 1/100000       28
;*/

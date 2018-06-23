/***********************************************************
;Filter Filename:   11pct8p.flt
;11.00%, 8 Pole Mayer2 IIR Filter, Filename: 11pct8p.flt


;	2nd Order Cell #1 Amplification =  1.00005837e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -7.63608522e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  1.63631869e-01
;	2nd Order Cell #2 Amplification =  1.25387981e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -5.99205808e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  1.00757733e-01
;	2nd Order Cell #3 Amplification =  1.39957400e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -5.14436331e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  7.42659298e-02
;	2nd Order Cell #4 Amplification =  1.46556891e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -4.77850714e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  6.40782786e-02
;*/

FILT_COEF filter_11_8 = {      /* Form I Mayer Filter,  c language Coefficients */
		    4,	/* # of cells */
		    {	/*    amplif        den coef 2      den coef 3 */
			{ 1.00005837e-01,-7.63608522e-01, 1.63631869e-01},
			{ 1.25387981e-01,-5.99205808e-01, 1.00757733e-01},
			{ 1.39957400e-01,-5.14436331e-01, 7.42659298e-02},
			{ 1.46556891e-01,-4.77850714e-01, 6.40782786e-02}
		    }
		 };


/*        Filter Step Response
;
;     Overshoot is        6 ppm or  0.00064 %
;     Samples to settle to 1/1000         17
;     Samples to settle to 1/5000         18
;     Samples to settle to 1/10000        19
;     Samples to settle to 1/50000        20
;     Samples to settle to 1/100000       20
;*/

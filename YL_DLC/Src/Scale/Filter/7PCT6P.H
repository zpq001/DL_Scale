/***********************************************************
;Filter Filename:   7pct6p.flt
;7.00%, 6 Pole Mayer1 IIR Filter, Filename: 7pct6p.flt


;	2nd Order Cell #1 Amplification =  6.02395922e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.16685680e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  4.07815172e-01
;	2nd Order Cell #2 Amplification =  4.91012237e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.17861192e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  3.75016812e-01
;	2nd Order Cell #3 Amplification =  4.60562924e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.14881330e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  3.33038474e-01
;*/

FILT_COEF filter_7_6 = {      /* Form I Mayer Filter,  c language Coefficients */
			3,	/* # of cells */
		    {	/*    amplif        den coef 2      den coef 3 */
			{ 6.02395922e-02,-1.16685680e+00, 4.07815172e-01},
			{ 4.91012237e-02,-1.17861192e+00, 3.75016812e-01},
			{ 4.60562924e-02,-1.14881330e+00, 3.33038474e-01}
		    }
		 };


/*        Filter Step Response
;
;     Overshoot is      761 ppm or  0.07610 %
;     Samples to settle to 1/1000         20
;     Samples to settle to 1/5000         26
;     Samples to settle to 1/10000        27
;     Samples to settle to 1/50000        29
;     Samples to settle to 1/100000       29
;*/

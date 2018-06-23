/***********************************************************
;Filter Filename:   6pct6p.flt
;6.00%, 6 Pole Mayer1 IIR Filter, Filename: 6pct6p.flt


;	2nd Order Cell #1 Amplification =  4.63786825e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.27825616e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  4.63770894e-01
;	2nd Order Cell #2 Amplification =  3.71891893e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.28990566e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  4.38662416e-01
;	2nd Order Cell #3 Amplification =  3.48307259e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.26033157e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  3.99654478e-01
;*/

FILT_COEF filter_6_6 = {      /* Form I Mayer Filter,  c language Coefficients */
			3,	/* # of cells */
			{	/*    amplif        den coef 2      den coef 3 */
			{ 4.63786825e-02,-1.27825616e+00, 4.63770894e-01},
			{ 3.71891893e-02,-1.28990566e+00, 4.38662416e-01},
			{ 3.48307259e-02,-1.26033157e+00, 3.99654478e-01}
		    }
		 };


/*        Filter Step Response
;
;     Overshoot is      577 ppm or  0.05774 %
;     Samples to settle to 1/1000         24
;     Samples to settle to 1/5000         31
;     Samples to settle to 1/10000        32
;     Samples to settle to 1/50000        34
;     Samples to settle to 1/100000       35
;*/

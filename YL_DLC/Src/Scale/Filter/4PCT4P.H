/***********************************************************
;Filter Filename:   4pct4p.flt
;4.00%, 4 Pole Mayer1 IIR Filter, Filename: 4pct4p.flt


;	2nd Order Cell #1 Amplification =  2.25269048e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.49046537e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  5.80572992e-01
;	2nd Order Cell #2 Amplification =  1.66312072e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.49672791e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  5.63252740e-01
;*/

FILT_COEF filter_4_4 = { /* Form I Mayer Filter,  c language Coefficients */
		    2,	/* # of cells */
		    {	/*    amplif        den coef 2      den coef 3 */
			{ 2.25269048e-02,-1.49046537e+00, 5.80572992e-01},
			{ 1.66312072e-02,-1.49672791e+00, 5.63252740e-01}
		    }
		 };


/*        Filter Step Response
;
;     Overshoot is      172 ppm or  0.01718 %
;     Samples to settle to 1/1000         30
;     Samples to settle to 1/5000         32
;     Samples to settle to 1/10000        38
;     Samples to settle to 1/50000        43
;     Samples to settle to 1/100000       44
;*/

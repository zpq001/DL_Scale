/***********************************************************
;Filter Filename:   12pct8p.flt
;12.00%, 8 Pole Mayer2 IIR Filter, Filename: 12pct8p.flt


;	2nd Order Cell #1 Amplification =  1.24772062e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -6.02923708e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  1.02011957e-01
;	2nd Order Cell #2 Amplification =  1.48136449e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -4.69245364e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  6.17911588e-02
;	2nd Order Cell #3 Amplification =  1.60998439e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -4.01169085e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  4.51628432e-02
;	2nd Order Cell #4 Amplification =  1.66715536e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -3.71964442e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  3.88265864e-02
;*/

FILT_COEF filter_12_8 = {      /* Form I Mayer Filter,  c language Coefficients */
		    4,	/* # of cells */
		    {	/*    amplif        den coef 2      den coef 3 */
			{ 1.24772062e-01,-6.02923708e-01, 1.02011957e-01},
			{ 1.48136449e-01,-4.69245364e-01, 6.17911588e-02},
			{ 1.60998439e-01,-4.01169085e-01, 4.51628432e-02},
			{ 1.66715536e-01,-3.71964442e-01, 3.88265864e-02}
		    }
		 };


/*        Filter Step Response
;
;     Overshoot is        0 ppm or  0.00002 %
;     Samples to settle to 1/1000         15
;     Samples to settle to 1/5000         17
;     Samples to settle to 1/10000        17
;     Samples to settle to 1/50000        18
;     Samples to settle to 1/100000       19
;*/

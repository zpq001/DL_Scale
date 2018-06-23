/***********************************************************
;Filter Filename:   5pct6p.flt
;5.00%, 6 Pole Mayer1 IIR Filter, Filename: 5pct6p.flt


;	2nd Order Cell #1 Amplification =  3.38312225e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.39210056e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  5.27425448e-01
;	2nd Order Cell #2 Amplification =  2.66532379e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.40326945e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  5.09882403e-01
;	2nd Order Cell #3 Amplification =  2.49202809e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.37486817e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  4.74549296e-01
;*/

FILT_COEF filter_5_6 = {      /* Form I Mayer Filter,  c language Coefficients */
			3,	/* # of cells */
			{	/*    amplif        den coef 2      den coef 3 */
			{ 3.38312225e-02,-1.39210056e+00, 5.27425448e-01},
			{ 2.66532379e-02,-1.40326945e+00, 5.09882403e-01},
			{ 2.49202809e-02,-1.37486817e+00, 4.74549296e-01}
		    }
		 };


/*        Filter Step Response
;
;     Overshoot is      455 ppm or  0.04551 %
;     Samples to settle to 1/1000         30
;     Samples to settle to 1/5000         37
;     Samples to settle to 1/10000        39
;     Samples to settle to 1/50000        42
;     Samples to settle to 1/100000       43
;*/

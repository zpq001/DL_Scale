/***********************************************************
;Filter Filename:   5pct4p.flt
;5.00%, 4 Pole Mayer1 IIR Filter, Filename: 5pct4p.flt


;	2nd Order Cell #1 Amplification =  3.33544650e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.37248275e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  5.05900611e-01
;	2nd Order Cell #2 Amplification =  2.51940387e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.37928133e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  4.80057488e-01
;*/

FILT_COEF filter_5_4 = {  /* Form I Mayer Filter,  c language Coefficients */
			2,	/* # of cells */
			{	/*    amplif        den coef 2      den coef 3 */
			{ 3.33544650e-02,-1.37248275e+00, 5.05900611e-01},
			{ 2.51940387e-02,-1.37928133e+00, 4.80057488e-01}
			}
		 };


/*        Filter Step Response
;
;     Overshoot is      264 ppm or  0.02645 %
;     Samples to settle to 1/1000         24
;     Samples to settle to 1/5000         29
;     Samples to settle to 1/10000        31
;     Samples to settle to 1/50000        34
;     Samples to settle to 1/100000       35
;*/

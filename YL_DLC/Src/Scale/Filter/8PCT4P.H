/***********************************************************
;Filter Filename:   8pct4p.flt
;8.00%, 4 Pole Mayer1 IIR Filter, Filename: 8pct4p.flt


;	2nd Order Cell #1 Amplification =  7.38227050e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.03762372e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  3.32914544e-01
;	2nd Order Cell #2 Amplification =  5.90886692e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.04457512e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  2.80929801e-01
;*/

FILT_COEF filter_8_4 = { /* Form I Mayer Filter,  c language Coefficients */
			2,	/* # of cells */
			{	/*    amplif        den coef 2      den coef 3 */
			{ 7.38227050e-02,-1.03762372e+00, 3.32914544e-01},
			{ 5.90886692e-02,-1.04457512e+00, 2.80929801e-01}
			}
		 };


/*        Filter Step Response
;
;     Overshoot is      902 ppm or  0.09023 %
;     Samples to settle to 1/1000         14
;     Samples to settle to 1/5000         18
;     Samples to settle to 1/10000        19
;     Samples to settle to 1/50000        20
;     Samples to settle to 1/100000       24
;*/

/***********************************************************
;Filter Filename:   12pct4p.flt
;12.00%, 4 Pole Mayer2 IIR Filter, Filename: 12pct4p.flt


;	2nd Order Cell #1 Amplification =  1.38072678e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -5.25080180e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  7.73708931e-02
;	2nd Order Cell #2 Amplification =  1.64623816e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -3.82578860e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  4.10741226e-02
;*/

FILT_COEF filter_12_4 = { /* Form I Mayer Filter,  c language Coefficients */
		    2,	/* # of cells */
		    {	/*    amplif        den coef 2      den coef 3 */
			{ 1.38072678e-01,-5.25080180e-01, 7.73708931e-02},
			{ 1.64623816e-01,-3.82578860e-01, 4.10741226e-02}
		    }
		 };


/*        Filter Step Response
;
;     Overshoot is        1 ppm or  0.00007 %
;     Samples to settle to 1/1000         10
;     Samples to settle to 1/5000         11
;     Samples to settle to 1/10000        12
;     Samples to settle to 1/50000        13
;     Samples to settle to 1/100000       13
;*/

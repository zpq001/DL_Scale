/***********************************************************
;Filter Filename:   3pct4p.flt
;3.00%, 4 Pole Mayer1 IIR Filter, Filename: 3pct4p.flt


;	2nd Order Cell #1 Amplification =  1.34099321e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.61204035e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  6.65680080e-01
;	2nd Order Cell #2 Amplification =  9.65533009e-03
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.61743471e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  6.56056035e-01
;*/

FILT_COEF filter_3_4 = {  /* Form I Mayer Filter,  c language Coefficients */
			2,	/* # of cells */
			{	/*    amplif        den coef 2      den coef 3 */
			{ 1.34099321e-02,-1.61204035e+00, 6.65680080e-01},
			{ 9.65533009e-03,-1.61743471e+00, 6.56056035e-01}
		    }
		 };


/*        Filter Step Response
;
;     Overshoot is      113 ppm or  0.01127 %
;     Samples to settle to 1/1000         41
;     Samples to settle to 1/5000         43
;     Samples to settle to 1/10000        50
;     Samples to settle to 1/50000        58
;     Samples to settle to 1/100000       60
;*/

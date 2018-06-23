/***********************************************************
;Filter Filename:   14pct6p.flt
;14.00%, 6 Pole Mayer2 IIR Filter, Filename: 14pct6p.flt


;	2nd Order Cell #1 Amplification =  1.89933568e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -2.59105701e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  1.88399738e-02
;	2nd Order Cell #2 Amplification =  2.04675938e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.91597927e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  1.03016780e-02
;	2nd Order Cell #3 Amplification =  2.10309569e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.66545536e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  7.78381227e-03
;*/

FILT_COEF filter_14_6 = {      /* Form I Mayer Filter,  c language Coefficients */
		    3,	/* # of cells */
		    {	/*    amplif        den coef 2      den coef 3 */
			{ 1.89933568e-01,-2.59105701e-01, 1.88399738e-02},
			{ 2.04675938e-01,-1.91597927e-01, 1.03016780e-02},
			{ 2.10309569e-01,-1.66545536e-01, 7.78381227e-03}
		    }
		 };


/*        Filter Step Response
;
;     Overshoot is        0 ppm or  0.00000 %
;     Samples to settle to 1/1000         10
;     Samples to settle to 1/5000         11
;     Samples to settle to 1/10000        11
;     Samples to settle to 1/50000        12
;     Samples to settle to 1/100000       12
;*/

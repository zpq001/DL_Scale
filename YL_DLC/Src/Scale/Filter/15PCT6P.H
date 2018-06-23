/***********************************************************
;Filter Filename:   15pct6p.flt
;15.00%, 6 Pole Mayer2 IIR Filter, Filename: 15pct6p.flt


;	2nd Order Cell #1 Amplification =  2.25748409e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -9.98014847e-02
;Numerator(3) =  1.00000000e+00  Denominator(3) =  2.79511939e-03
;	2nd Order Cell #2 Amplification =  2.31974318e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -7.36238464e-02
;Numerator(3) =  1.00000000e+00  Denominator(3) =  1.52111961e-03
;	2nd Order Cell #3 Amplification =  2.34301623e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -6.39408215e-02
;Numerator(3) =  1.00000000e+00  Denominator(3) =  1.14731529e-03
;*/

FILT_COEF filter_15_6 = {      /* Form I Mayer Filter,  c language Coefficients */
		    3,	/* # of cells */
		    {	/*    amplif        den coef 2      den coef 3 */
			{ 2.25748409e-01,-9.98014847e-02, 2.79511939e-03},
			{ 2.31974318e-01,-7.36238464e-02, 1.52111961e-03},
			{ 2.34301623e-01,-6.39408215e-02, 1.14731529e-03}
		    }
		 };


/*        Filter Step Response
;
;     Overshoot is        0 ppm or  0.00000 %
;     Samples to settle to 1/1000          8
;     Samples to settle to 1/5000          9
;     Samples to settle to 1/10000         9
;     Samples to settle to 1/50000        10
;     Samples to settle to 1/100000       10
;*/

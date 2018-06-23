/***********************************************************
;Filter Filename:   13pct6p.flt
;13.00%, 6 Pole Mayer2 IIR Filter, Filename: 13pct6p.flt


;	2nd Order Cell #1 Amplification =  1.57874071e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -4.17393441e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  4.88897254e-02
;	2nd Order Cell #2 Amplification =  1.79233626e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -3.10040628e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  2.69751318e-02
;	2nd Order Cell #3 Amplification =  1.87624104e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -2.69954208e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  2.04506239e-02
;*/

FILT_COEF  filter_13_6 = {      /* Form I Mayer Filter,  c language Coefficients */
		    3,	/* # of cells */
		    {	/*    amplif        den coef 2      den coef 3 */
			{ 1.57874071e-01,-4.17393441e-01, 4.88897254e-02},
			{ 1.79233626e-01,-3.10040628e-01, 2.69751318e-02},
			{ 1.87624104e-01,-2.69954208e-01, 2.04506239e-02}
		    }
		 };


/*        Filter Step Response
;
;     Overshoot is        0 ppm or  0.00000 %
;     Samples to settle to 1/1000         11
;     Samples to settle to 1/5000         12
;     Samples to settle to 1/10000        13
;     Samples to settle to 1/50000        14
;     Samples to settle to 1/100000       14
;*/

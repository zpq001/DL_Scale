/***********************************************************
;Filter Filename:   8pct6p.flt
;8.00%, 6 Pole Mayer1 IIR Filter, Filename: 8pct6p.flt


;	2nd Order Cell #1 Amplification =  7.52544955e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.05774842e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  3.58766404e-01
;	2nd Order Cell #2 Amplification =  6.22816422e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.06929659e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  3.18423161e-01
;	2nd Order Cell #3 Amplification =  5.84978493e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.04006002e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  2.74051414e-01
;*/

FILT_COEF filter_8_6 = {      /* Form I Mayer Filter,  c language Coefficients */
		    3,	/* # of cells */
		    {	/*    amplif        den coef 2      den coef 3 */
			{ 7.52544955e-02,-1.05774842e+00, 3.58766404e-01},
			{ 6.22816422e-02,-1.06929659e+00, 3.18423161e-01},
			{ 5.84978493e-02,-1.04006002e+00, 2.74051414e-01}
		    }
		 };


/*        Filter Step Response
;
;     Overshoot is     1042 ppm or  0.10423 %
;     Samples to settle to 1/1000         20
;     Samples to settle to 1/5000         23
;     Samples to settle to 1/10000        24
;     Samples to settle to 1/50000        25
;     Samples to settle to 1/100000       28
;*/

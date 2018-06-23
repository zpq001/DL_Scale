/***********************************************************
;Filter Filename:   15pct4p.flt
;15.00%, 4 Pole Mayer2 IIR Filter, Filename: 15pct4p.flt


;	2nd Order Cell #1 Amplification =  2.27860910e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -9.08737737e-02
;Numerator(3) =  1.00000000e+00  Denominator(3) =  2.31741325e-03
;	2nd Order Cell #2 Amplification =  2.33965287e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -6.53368104e-02
;Numerator(3) =  1.00000000e+00  Denominator(3) =  1.19795973e-03
;*/

FILT_COEF filter_15_4 = {      /* Form I Mayer Filter,  c language Coefficients */
		    2,	/* # of cells */
		    {	/*    amplif        den coef 2      den coef 3 */
			{ 2.27860910e-01,-9.08737737e-02, 2.31741325e-03},
			{ 2.33965287e-01,-6.53368104e-02, 1.19795973e-03}
		    }
		 };


/*        Filter Step Response
;
;     Overshoot is        0 ppm or  0.00000 %
;     Samples to settle to 1/1000          7
;     Samples to settle to 1/5000          7
;     Samples to settle to 1/10000         7
;     Samples to settle to 1/50000         8
;     Samples to settle to 1/100000        8
;*/

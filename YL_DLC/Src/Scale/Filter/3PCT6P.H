/***********************************************************
;Filter Filename:   3pct6p.flt
;3.00%, 6 Pole Mayer1 IIR Filter, Filename: 3pct6p.flt


;	2nd Order Cell #1 Amplification =  1.35390289e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.62755938e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  6.81715499e-01
;	2nd Order Cell #2 Amplification =  1.02504426e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.63633112e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  6.77332887e-01
;	2nd Order Cell #3 Amplification =  9.54217142e-03
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.61393504e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  6.52103727e-01
;*/

FILT_COEF filter_3_6 = {      /* Form I Mayer Filter,  c language Coefficients */
		    3,	/* # of cells */
		    {	/*    amplif        den coef 2      den coef 3 */
			{ 1.35390289e-02,-1.62755938e+00, 6.81715499e-01},
			{ 1.02504426e-02,-1.63633112e+00, 6.77332887e-01},
			{ 9.54217142e-03,-1.61393504e+00, 6.52103727e-01}
		    }
		 };


/*        Filter Step Response
;
;     Overshoot is      308 ppm or  0.03084 %
;     Samples to settle to 1/1000         51
;     Samples to settle to 1/5000         63
;     Samples to settle to 1/10000        66
;     Samples to settle to 1/50000        72
;     Samples to settle to 1/100000       73
;*/

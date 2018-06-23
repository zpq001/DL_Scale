/***********************************************************
;Filter Filename:   9pct6p.flt
;9.00%, 6 Pole Mayer2 IIR Filter, Filename: 9pct6p.flt


;	2nd Order Cell #1 Amplification =  6.72706374e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.02676579e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  2.95848344e-01
;	2nd Order Cell #2 Amplification =  9.57055791e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -7.94169191e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  1.76991508e-01
;	2nd Order Cell #3 Amplification =  1.09025687e-01
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -7.02314193e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  1.38416941e-01
;*/

FILT_COEF filter_9_6 = {      /* Form I Mayer Filter,  c language Coefficients */
			3,	/* # of cells */
		    {	/*    amplif        den coef 2      den coef 3 */
			{ 6.72706374e-02,-1.02676579e+00, 2.95848344e-01},
			{ 9.57055791e-02,-7.94169191e-01, 1.76991508e-01},
			{ 1.09025687e-01,-7.02314193e-01, 1.38416941e-01}
		    }
		 };


/*        Filter Step Response
;
;     Overshoot is      818 ppm or  0.08178 %
;     Samples to settle to 1/1000         16
;     Samples to settle to 1/5000         21
;     Samples to settle to 1/10000        22
;     Samples to settle to 1/50000        24
;     Samples to settle to 1/100000       25
;*/

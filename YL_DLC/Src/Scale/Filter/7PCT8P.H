/***********************************************************
;Filter Filename:   7pct8p.flt
;7.00%, 8 Pole Mayer1 IIR Filter, Filename: 7pct8p.flt


;	2nd Order Cell #1 Amplification =  6.08196357e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.17809240e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  4.21370941e-01
;	2nd Order Cell #2 Amplification =  5.78280730e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.12014504e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  3.51457332e-01
;	2nd Order Cell #3 Amplification =  4.76050096e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.16385835e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  3.54278390e-01
;	2nd Order Cell #4 Amplification =  4.58892366e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.14720376e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  3.30760709e-01
;*/

FILT_COEF filter_7_8 = {      /* Form I Mayer Filter,  c language Coefficients */
		    4,	/* # of cells */
		    {	/*    amplif        den coef 2      den coef 3 */
			{ 6.08196357e-02,-1.17809240e+00, 4.21370941e-01},
			{ 5.78280730e-02,-1.12014504e+00, 3.51457332e-01},
			{ 4.76050096e-02,-1.16385835e+00, 3.54278390e-01},
			{ 4.58892366e-02,-1.14720376e+00, 3.30760709e-01}
		    }
		 };


/*        Filter Step Response
;
;     Overshoot is      146 ppm or  0.01455 %
;     Samples to settle to 1/1000         25
;     Samples to settle to 1/5000         26
;     Samples to settle to 1/10000        30
;     Samples to settle to 1/50000        33
;     Samples to settle to 1/100000       33
;*/

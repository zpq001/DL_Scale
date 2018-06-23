/***********************************************************
;Filter Filename:   2pct8p.flt
;2.00%, 8 Pole Mayer1 IIR Filter, Filename: 2pct8p.flt


;	2nd Order Cell #1 Amplification =  6.39241292e-03
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.75561674e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  7.81186387e-01
;	2nd Order Cell #2 Amplification =  6.26975820e-03
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.72193076e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  7.47009789e-01
;	2nd Order Cell #3 Amplification =  4.55120289e-03
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.74753456e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  7.65739374e-01
;	2nd Order Cell #4 Amplification =  4.35717397e-03
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.73792126e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  7.55349951e-01
;*/

FILT_COEF filter_2_8 = {      /* Form I Mayer Filter,  c language Coefficients */
		    4,	/* # of cells */
		    {	/*    amplif        den coef 2      den coef 3 */
			{ 6.39241292e-03,-1.75561674e+00, 7.81186387e-01},
			{ 6.26975820e-03,-1.72193076e+00, 7.47009789e-01},
			{ 4.55120289e-03,-1.74753456e+00, 7.65739374e-01},
			{ 4.35717397e-03,-1.73792126e+00, 7.55349951e-01}
		    }
		 };


/*        Filter Step Response
;
;     Overshoot is        7 ppm or  0.00075 %
;     Samples to settle to 1/1000         97
;     Samples to settle to 1/5000        105
;     Samples to settle to 1/10000       107
;     Samples to settle to 1/50000       112
;     Samples to settle to 1/100000      113
;*/

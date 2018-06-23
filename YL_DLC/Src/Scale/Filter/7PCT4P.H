/***********************************************************
;Filter Filename:   7pct4p.flt
;7.00%, 4 Pole Mayer1 IIR Filter, Filename: 7pct4p.flt


;	2nd Order Cell #1 Amplification =  5.91796004e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.14632448e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  3.83042882e-01
;	2nd Order Cell #2 Amplification =  4.65333159e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.15342344e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  3.39556708e-01
;*/

FILT_COEF filter_7_4 = { /* Form I Mayer Filter,  c language Coefficients */
			2,	/* # of cells */
			{	/*    amplif        den coef 2      den coef 3 */
			{ 5.91796004e-02,-1.14632448e+00, 3.83042882e-01},
			{ 4.65333159e-02,-1.15342344e+00, 3.39556708e-01}
			}
		 };


/*        Filter Step Response
;
;     Overshoot is      602 ppm or  0.06021 %
;     Samples to settle to 1/1000         16
;     Samples to settle to 1/5000         21
;     Samples to settle to 1/10000        22
;     Samples to settle to 1/50000        23
;     Samples to settle to 1/100000       26
;*/

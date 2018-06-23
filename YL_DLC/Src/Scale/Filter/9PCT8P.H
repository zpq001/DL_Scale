/***********************************************************
;Filter Filename:   9pct8p.flt
;9.00%, 8 Pole Mayer1 IIR Filter, Filename: 9pct8p.flt


;	2nd Order Cell #1 Amplification =  9.23162313e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -9.61345852e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  3.30610778e-01
;	2nd Order Cell #2 Amplification =  8.71034079e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -9.07061508e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  2.55475140e-01
;	2nd Order Cell #3 Amplification =  7.43896223e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -9.47934120e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  2.45492609e-01
;	2nd Order Cell #4 Amplification =  7.18260950e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -9.32305954e-01
;Numerator(3) =  1.00000000e+00  Denominator(3) =  2.19610334e-01
;*/

FILT_COEF filter_9_8 = {      /* Form I Mayer Filter,  c language Coefficients */
		    4,	/* # of cells */
		    {	/*    amplif        den coef 2      den coef 3 */
			{ 9.23162313e-02,-9.61345852e-01, 3.30610778e-01},
			{ 8.71034079e-02,-9.07061508e-01, 2.55475140e-01},
			{ 7.43896223e-02,-9.47934120e-01, 2.45492609e-01},
			{ 7.18260950e-02,-9.32305954e-01, 2.19610334e-01}
		    }
		 };


/*        Filter Step Response
;
;     Overshoot is      514 ppm or  0.05140 %
;     Samples to settle to 1/1000         19
;     Samples to settle to 1/5000         23
;     Samples to settle to 1/10000        23
;     Samples to settle to 1/50000        24
;     Samples to settle to 1/100000       27
;*/

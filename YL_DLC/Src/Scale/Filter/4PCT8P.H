/***********************************************************
;Filter Filename:   4pct8p.flt
;4.00%, 8 Pole Mayer1 IIR Filter, Filename: 4pct8p.flt


;	2nd Order Cell #1 Amplification =  2.29475808e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.51829889e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  6.10089212e-01
;	2nd Order Cell #2 Amplification =  2.21745802e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.46715424e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  5.55852565e-01
;	2nd Order Cell #3 Amplification =  1.70482927e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.50588848e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  5.74081651e-01
;	2nd Order Cell #4 Amplification =  1.63782728e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.49124240e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  5.56755493e-01
;*/

FILT_COEF filter_4_8 = {      /* Form I Mayer Filter,  c language Coefficients */
		    4,	/* # of cells */
		    {	/*    amplif        den coef 2      den coef 3 */
			{ 2.29475808e-02,-1.51829889e+00, 6.10089212e-01},
			{ 2.21745802e-02,-1.46715424e+00, 5.55852565e-01},
			{ 1.70482927e-02,-1.50588848e+00, 5.74081651e-01},
			{ 1.63782728e-02,-1.49124240e+00, 5.56755493e-01}
		    }
		 };


/*        Filter Step Response
;
;     Overshoot is       18 ppm or  0.00175 %
;     Samples to settle to 1/1000         47
;     Samples to settle to 1/5000         50
;     Samples to settle to 1/10000        51
;     Samples to settle to 1/50000        53
;     Samples to settle to 1/100000       60
;*/

/***********************************************************
;Filter Filename:   4pct6p.flt
;4.00%, 6 Pole Mayer1 IIR Filter, Filename: 4pct6p.flt


;	2nd Order Cell #1 Amplification =  2.27996456e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.50851093e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  5.99709515e-01
;	2nd Order Cell #2 Amplification =  1.76226864e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.51875000e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  5.89240744e-01
;	2nd Order Cell #3 Amplification =  1.64440403e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.49266378e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  5.58439940e-01
;*/

FILT_COEF filter_4_6 = {      /* Form I Mayer Filter,  c language Coefficients */
		    3,	/* # of cells */
		    {	/*    amplif        den coef 2      den coef 3 */
			{ 2.27996456e-02,-1.50851093e+00, 5.99709515e-01},
			{ 1.76226864e-02,-1.51875000e+00, 5.89240744e-01},
			{ 1.64440403e-02,-1.49266378e+00, 5.58439940e-01}
		    }
		 };


/*        Filter Step Response
;
;     Overshoot is      368 ppm or  0.03676 %
;     Samples to settle to 1/1000         38
;     Samples to settle to 1/5000         47
;     Samples to settle to 1/10000        49
;     Samples to settle to 1/50000        53
;     Samples to settle to 1/100000       54
;*/

/***********************************************************
;Filter Filename:   6pct8p.flt
;6.00%, 8 Pole Mayer1 IIR Filter, Filename: 6pct8p.flt


;	2nd Order Cell #1 Amplification =  4.67826952e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.28939128e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  4.76522056e-01
;	2nd Order Cell #2 Amplification =  4.46910957e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.23174410e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  4.10508484e-01
;	2nd Order Cell #3 Amplification =  3.60329947e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.27528110e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  4.19413081e-01
;	2nd Order Cell #4 Amplification =  3.47007097e-02
;Numerator(1) =  1.00000000e+00  Denominator(1) =  1.00000000e+00
;Numerator(2) =  2.00000000e+00  Denominator(2) = -1.25873009e+00
;Numerator(3) =  1.00000000e+00  Denominator(3) =  3.97532931e-01
;*/

FILT_COEF filter_6_8 = {      /* Form I Mayer Filter,  c language Coefficients */
		    4,	/* # of cells */
		    {	/*    amplif        den coef 2      den coef 3 */
			{ 4.67826952e-02,-1.28939128e+00, 4.76522056e-01},
			{ 4.46910957e-02,-1.23174410e+00, 4.10508484e-01},
			{ 3.60329947e-02,-1.27528110e+00, 4.19413081e-01},
			{ 3.47007097e-02,-1.25873009e+00, 3.97532931e-01}
		    }
		 };


/*        Filter Step Response
;
;     Overshoot is       72 ppm or  0.00719 %
;     Samples to settle to 1/1000         30
;     Samples to settle to 1/5000         32
;     Samples to settle to 1/10000        32
;     Samples to settle to 1/50000        38
;     Samples to settle to 1/100000       39
;*/

# 1 "rc4-ia64.S"
.ident "rc4-ia64.s, version 3.0"
.ident "Copyright (c) 2005 Hewlett-Packard Development Company, L.P."

# 146
	.text

	.align	32

	.type	RC4, @function
	.global	RC4

	.proc	RC4
	.prologue

RC4:
	{
	  	.mmi
		alloc	r2 = ar.pfs, 4 , (24 - 4 - 0) , 0 , 24 

		.rotr Data[4], I[2], IPr[3], SI[3], JP[2], SJ[2], T[2], 		      OutWord[2]

		.rotp pPhase[4]

		add 		r18  = 0, in2 
		add 		r14  = 0, in0 
	}
	{
		.mmi
		add 		r16  = 0, in2 
		add 		r17  = 0, in3 
		mov		r8  = r0
	}
	;;
	{
		.mmi
		lfetch.nt1	[r18 ], 0x80
		add 		r19  = 0, in3 
	}
	{
        	.mib
		add 		in0  = 0, in0 
        	cmp.ge.unc  	p8 , p7  = r0, in1 
	(p8 ) br.ret.sptk.few rp
	}
	;;
	{
        	.mib
        	cmp.eq.or  	p8 , p7  = r0, r16 
        	cmp.eq.or  	p8 , p7  = r0, r17 
		nop		0x0
	}
	{
		.mib
        	cmp.eq.or  	p8 , p7  = r0, in0 
		nop		0x0
	(p8 ) br.ret.sptk.few rp
	}
	;;
		ld4 		I[1] = [r14 ], 4 
# 209
		add 		r25  = 7*128, in0 
		add 		r26  = 6*128, in0 




		;;
# 227
		lfetch.fault.nt1		[r25 ], -256
		lfetch.fault.nt1		[r26 ], -256;;
		lfetch.fault.nt1		[r25 ], -256
		lfetch.fault.nt1		[r26 ], -256;;


		lfetch.fault.nt1		[r25 ], -256
		lfetch.fault.nt1		[r26 ], -256;;

	{
		.mii
		lfetch.fault.nt1		[r25 ]
		add		I[1]=1,I[1];;
		zxt1		I[1]=I[1]
	}
	{
		.mmi
		lfetch.nt1	[r18 ], 0x80
		lfetch.excl.nt1	[r19 ], 0x80
		.save		pr, r9 
		mov		r9  = pr
	} ;;
	{
		.mmi
		lfetch.excl.nt1	[r19 ], 0x80
		ld4 		r15  = [r14 ], 4 
		add 		r24  = in1 , r16 
	}  ;;
	{
		.mmi
		add 		r24  = -1, r24 

		mov		r20  = 1
		.save		ar.lc, r8 
		mov		r8  = ar.lc
		.body
	} ;;
	{
		.mmb
		sub		r22  = 0, r17 
		cmp.gtu		p8 , p0 = 91, in1 
(p8 )	br.cond.dpnt	.rc4Remainder

	} ;;
	{
		.mmi
		and		r22  = 0x7, r22 
		;;
		cmp.eq		p9 , p10  = r22 , r0
		nop		0x0
	} ;;
	{
		.mmb
.pred.rel	"mutex",p10 ,p9 
(p10 )	add		r22  = -1, r22 
(p9 )	sub		r22  = r24 , r16 
(p9 )	br.cond.dptk.many .rc4Aligned
	} ;;
	{
		.mmi
		nop		0x0
		nop		0x0
		mov.i		ar.lc = r22 
	}




		{ ld1 Data[0] = [r16], 1; add r23 = 1, I[1]; shladd IPr[0] = I[1], 2, r14; } ;; { ld4 SI[0] = [IPr[0]]; mov pr.rot = 0x10000; mov ar.ec = 4; } ;; { add r15 = r15, SI[0]; zxt1 I[0] = r23; br.cexit.spnt.few .+16; } ;; 
		.RC4AlignLoop: { .mmi; (pPhase[0]) ld1 Data[0] = [r16], 1; (pPhase[0]) add r23 = 1, I[1]; (pPhase[1]) zxt1 r15 = r15; }{ .mmi; (pPhase[3]) ld4 T[1] = [T[1]]; (pPhase[2]) add T[0] = SI[2], SJ[1]; (pPhase[0]) shladd IPr[0] = I[1], 2, r14; } ;; { .mmi; (pPhase[2]) st4 [IPr[2]] = SJ[1]; (pPhase[2]) st4 [JP[1]] = SI[2]; (pPhase[2]) zxt1 T[0] = T[0]; }{ .mmi; (pPhase[0]) ld4 SI[0] = [IPr[0]]; (pPhase[1]) shladd JP[0] = r15, 2, r14; (pPhase[0]) cmp.eq.unc p6, p0 = I[1], r15; } ;; { .mmi; (pPhase[1]) ld4 SJ[0] = [JP[0]]; (pPhase[3]) xor Data[3] = Data[3], T[1]; nop 0x0; }{ .mmi; (pPhase[2]) shladd T[0] = T[0], 2, r14; (p6) mov SI[0] = SI[1]; (pPhase[0]) zxt1 I[0] = r23; } ;; { .mmb; (pPhase[3]) st1 [r17] = Data[3], 1; (pPhase[0]) add r15 = r15, SI[0]; br.ctop.sptk.few .RC4AlignLoop; } ;; 

	{
		.mib
		sub		r22  = r24 , r16 
		zxt1		r23  = r23 
		clrrrb
		;;

	}
	{
		.mmi
		mov		I[1] = r23 
		nop		0x0
		nop		0x0
	} ;;


.rc4Aligned:





	{
		.mlx
		add	r21  = 1 - (6 + 1)*4, r22 
		movl		r22  = 0xaaaaaaaaaaaaaaab
	} ;;
	{
		.mmi
		setf.sig	f6 = r21 
		setf.sig	f7 = r22 
		nop		0x0
	} ;;
	{
		.mfb
		nop		0x0
		xmpy.hu		f6 = f6, f7
		nop		0x0
	} ;;
	{
		.mmi
		getf.sig	r21  = f6;;
		nop		0x0
		shr.u		r21  = r21 , 4
	} ;;
	{
		.mmi
		nop		0x0
		nop		0x0
		mov.i		ar.lc = r21 
	} ;;



.rc4Prologue:

		ld1    Data[0] = [r16 ], 1
		padd1  I[0] = r20 , I[1]
		shladd IPr[0] = I[1], 2, r14 
		;;
		ld4    SI[0] = [IPr[0]]
		;;
		add    r15  = r15 , SI[0]
		;;

		ld1    Data[1] = [r16 ], 1
		padd1  I[1] = r20 , I[0]
		zxt1   r15  = r15 
		shladd IPr[1] = I[0], 2, r14 
		;;
		ld4    SI[1] = [IPr[1]]
		shladd JP[1] = r15, 2, r14 
		;;
		ld4    SJ[1] = [JP[1]]
		cmp.eq p6 , p0 = I[0], r15 
		add    r15  = r15 , SI[1]
(p6 )	br.cond.spnt.many .rc4Bypass2
		;;
.rc4Resume2:

		ld1    Data[2] = [r16 ], 1
		padd1  I[0] = r20 , I[1]
		zxt1   r15  = r15 
		add    T[0] = SI[0], SJ[1]
		shladd IPr[2] = I[1], 2, r14 
		;;
		st4    [IPr[0]] = SJ[1]
		st4    [JP[1]] = SI[0]
		zxt1   T[0] = T[0]
		ld4    SI[2] = [IPr[2]]
		shladd JP[0] = r15, 2, r14 
		;;
		ld4    SJ[0] = [JP[0]]
		cmp.eq p6 , p0 = I[1], r15 
		add    r15  = r15 , SI[2]
		shladd T[0] = T[0], 2, r14 
(p6 )	br.cond.spnt.many .rc4Bypass3
		;;
.rc4Resume3:

		{ .mmi
		ld1    Data[3] = [r16 ], 1
		padd1  I[1] = r20 , I[0]
		zxt1   r15  = r15 
		}
		{ .mmi
		ld4    T[0] = [T[0]]
		add    T[1] = SI[1], SJ[0]
		shladd IPr[0] = I[0], 2, r14 
		}
		;;
		{ .mmi
		st4    [IPr[1]] = SJ[0]
		st4    [JP[0]] = SI[1]
		zxt1   T[1] = T[1]
		}
		{ .mmi
		ld4    SI[0] = [IPr[0]]
		shladd JP[1] = r15, 2, r14 
		xor    Data[0] = Data[0], T[0]
		}
		;;
		{ .mmi
		ld4    SJ[1] = [JP[1]]
		cmp.eq p6 , p0 = I[0], r15 
		dep OutWord[0] = Data[0], OutWord[1], (56 - (8 * (0))) , 8
		}
		{ .mmb
		add    r15  = r15 , SI[0]
		shladd T[1] = T[1], 2, r14 
(p6 )	br.cond.spnt.many .rc4Bypass4
		}
		;;
.rc4Resume4:
.rc4Loop:

		{ .mmi
		ld1    Data[0] = [r16 ], 1
		padd1  I[0] = r20 , I[1]
		zxt1   r15  = r15 
		}
		{ .mmi
		ld4    T[1] = [T[1]]
		add    T[0] = SI[2], SJ[1]
		shladd IPr[1] = I[1], 2, r14 
		}
		;;
		{ .mmi
		st4    [IPr[2]] = SJ[1]
		st4    [JP[1]] = SI[2]
		zxt1   T[0] = T[0]
		}
		{ .mmi
		ld4    SI[1] = [IPr[1]]
		shladd JP[0] = r15, 2, r14 
		xor    Data[1] = Data[1], T[1]
		}
		;;
		{ .mmi
		ld4    SJ[0] = [JP[0]]
		cmp.eq p6 , p0 = I[1], r15 
		dep OutWord[0] = Data[1], OutWord[0], (56 - (8 * (1))) , 8
		}
		{ .mmb
		add    r15  = r15 , SI[1]
		shladd T[0] = T[0], 2, r14 
(p6 )	br.cond.spnt.many .rc4Bypass5
		}
		;;
.rc4Resume5:

		{ .mmi
		ld1    Data[1] = [r16 ], 1
		padd1  I[1] = r20 , I[0]
		zxt1   r15  = r15 
		}
		{ .mmi
		ld4    T[0] = [T[0]]
		add    T[1] = SI[0], SJ[0]
		shladd IPr[2] = I[0], 2, r14 
		}
		;;
		{ .mmi
		st4    [IPr[0]] = SJ[0]
		st4    [JP[0]] = SI[0]
		zxt1   T[1] = T[1]
		}
		{ .mmi
		ld4    SI[2] = [IPr[2]]
		shladd JP[1] = r15, 2, r14 
		xor    Data[2] = Data[2], T[0]
		}
		;;
		{ .mmi
		ld4    SJ[1] = [JP[1]]
		cmp.eq p6 , p0 = I[0], r15 
		dep OutWord[0] = Data[2], OutWord[0], (56 - (8 * (2))) , 8
		}
		{ .mmb
		add    r15  = r15 , SI[2]
		shladd T[1] = T[1], 2, r14 
(p6 )	br.cond.spnt.many .rc4Bypass6
		}
		;;
.rc4Resume6:

		{ .mmi
		ld1    Data[2] = [r16 ], 1
		padd1  I[0] = r20 , I[1]
		zxt1   r15  = r15 
		}
		{ .mmi
		ld4    T[1] = [T[1]]
		add    T[0] = SI[1], SJ[1]
		shladd IPr[0] = I[1], 2, r14 
		}
		;;
		{ .mmi
		st4    [IPr[1]] = SJ[1]
		st4    [JP[1]] = SI[1]
		zxt1   T[0] = T[0]
		}
		{ .mmi
		ld4    SI[0] = [IPr[0]]
		shladd JP[0] = r15, 2, r14 
		xor    Data[3] = Data[3], T[1]
		}
		;;
		{ .mmi
		ld4    SJ[0] = [JP[0]]
		cmp.eq p6 , p0 = I[1], r15 
		dep OutWord[0] = Data[3], OutWord[0], (56 - (8 * (3))) , 8
		}
		{ .mmb
		add    r15  = r15 , SI[0]
		shladd T[0] = T[0], 2, r14 
(p6 )	br.cond.spnt.many .rc4Bypass7
		}
		;;
.rc4Resume7:

		{ .mmi
		ld1    Data[3] = [r16 ], 1
		padd1  I[1] = r20 , I[0]
		zxt1   r15  = r15 
		}
		{ .mmi
		ld4    T[0] = [T[0]]
		add    T[1] = SI[2], SJ[0]
		shladd IPr[1] = I[0], 2, r14 
		}
		;;
		{ .mmi
		st4    [IPr[2]] = SJ[0]
		st4    [JP[0]] = SI[2]
		zxt1   T[1] = T[1]
		}
		{ .mmi
		ld4    SI[1] = [IPr[1]]
		shladd JP[1] = r15, 2, r14 
		xor    Data[0] = Data[0], T[0]
		}
		;;
		{ .mmi
		ld4    SJ[1] = [JP[1]]
		cmp.eq p6 , p0 = I[0], r15 
		dep OutWord[0] = Data[0], OutWord[0], (56 - (8 * (4))) , 8
		}
		{ .mmb
		add    r15  = r15 , SI[1]
		shladd T[1] = T[1], 2, r14 
(p6 )	br.cond.spnt.many .rc4Bypass8
		}
		;;
.rc4Resume8:

		{ .mmi
		ld1    Data[0] = [r16 ], 1
		padd1  I[0] = r20 , I[1]
		zxt1   r15  = r15 
		}
		{ .mmi
		ld4    T[1] = [T[1]]
		add    T[0] = SI[0], SJ[1]
		shladd IPr[2] = I[1], 2, r14 
		}
		;;
		{ .mmi
		st4    [IPr[0]] = SJ[1]
		st4    [JP[1]] = SI[0]
		zxt1   T[0] = T[0]
		}
		{ .mmi
		ld4    SI[2] = [IPr[2]]
		shladd JP[0] = r15, 2, r14 
		xor    Data[1] = Data[1], T[1]
		}
		;;
		{ .mmi
		ld4    SJ[0] = [JP[0]]
		cmp.eq p6 , p0 = I[1], r15 
		dep OutWord[0] = Data[1], OutWord[0], (56 - (8 * (5))) , 8
		}
		{ .mmb
		add    r15  = r15 , SI[2]
		shladd T[0] = T[0], 2, r14 
(p6 )	br.cond.spnt.many .rc4Bypass9
		}
		;;
.rc4Resume9:

		{ .mmi
		ld1    Data[1] = [r16 ], 1
		padd1  I[1] = r20 , I[0]
		zxt1   r15  = r15 
		}
		{ .mmi
		ld4    T[0] = [T[0]]
		add    T[1] = SI[1], SJ[0]
		shladd IPr[0] = I[0], 2, r14 
		}
		;;
		{ .mmi
		st4    [IPr[1]] = SJ[0]
		st4    [JP[0]] = SI[1]
		zxt1   T[1] = T[1]
		}
		{ .mmi
		ld4    SI[0] = [IPr[0]]
		shladd JP[1] = r15, 2, r14 
		xor    Data[2] = Data[2], T[0]
		}
		;;
		{ .mmi
		ld4    SJ[1] = [JP[1]]
		cmp.eq p6 , p0 = I[0], r15 
		dep OutWord[0] = Data[2], OutWord[0], (56 - (8 * (6))) , 8
		}
		{ .mmb
		add    r15  = r15 , SI[0]
		shladd T[1] = T[1], 2, r14 
(p6 )	br.cond.spnt.many .rc4Bypass10
		}
		;;
.rc4Resume10:

		{ .mmi
		ld1    Data[2] = [r16 ], 1
		padd1  I[0] = r20 , I[1]
		zxt1   r15  = r15 
		}
		{ .mmi
		ld4    T[1] = [T[1]]
		add    T[0] = SI[2], SJ[1]
		shladd IPr[1] = I[1], 2, r14 
		}
		;;
		{ .mmi
		st4    [IPr[2]] = SJ[1]
		st4    [JP[1]] = SI[2]
		zxt1   T[0] = T[0]
		}
		{ .mmi
		ld4    SI[1] = [IPr[1]]
		shladd JP[0] = r15, 2, r14 
		xor    Data[3] = Data[3], T[1]
		}
		;;
		{ .mmi
		ld4    SJ[0] = [JP[0]]
		cmp.eq p6 , p0 = I[1], r15 
		dep OutWord[0] = Data[3], OutWord[0], (56 - (8 * (7))) , 8
		}
		{ .mmb
		add    r15  = r15 , SI[1]
		shladd T[0] = T[0], 2, r14 
(p6 )	br.cond.spnt.many .rc4Bypass11
		}
		;;
.rc4Resume11:

		{ .mmi
		ld1    Data[3] = [r16 ], 1
		padd1  I[1] = r20 , I[0]
		zxt1   r15  = r15 
		}
		{ .mmi
		ld4    T[0] = [T[0]]
		add    T[1] = SI[0], SJ[0]
		shladd IPr[2] = I[0], 2, r14 
		}
		;;
		{ .mmi
		st4    [IPr[0]] = SJ[0]
		st4    [JP[0]] = SI[0]
		zxt1   T[1] = T[1]
		}
		{ .mmi
		ld4    SI[2] = [IPr[2]]
		shladd JP[1] = r15, 2, r14 
		xor    Data[0] = Data[0], T[0]
		}
		;;
		{ .mmi
		ld4    SJ[1] = [JP[1]]
		cmp.eq p6 , p0 = I[0], r15 
		dep OutWord[1] = Data[0], OutWord[0], (56 - (8 * (0))) , 8
		}
		{ .mmb
		add    r15  = r15 , SI[2]
		shladd T[1] = T[1], 2, r14 
(p6 )	br.cond.spnt.many .rc4Bypass12
		}
		;;
.rc4Resume12:
		st8 [r17 ] = OutWord[0], 8

		{ .mmi
		ld1    Data[0] = [r16 ], 1
		padd1  I[0] = r20 , I[1]
		zxt1   r15  = r15 
		}
		{ .mmi
		ld4    T[1] = [T[1]]
		add    T[0] = SI[1], SJ[1]
		shladd IPr[0] = I[1], 2, r14 
		}
		;;
		{ .mmi
		st4    [IPr[1]] = SJ[1]
		st4    [JP[1]] = SI[1]
		zxt1   T[0] = T[0]
		}
		{ .mmi
		ld4    SI[0] = [IPr[0]]
		shladd JP[0] = r15, 2, r14 
		xor    Data[1] = Data[1], T[1]
		}
		;;
		{ .mmi
		ld4    SJ[0] = [JP[0]]
		cmp.eq p6 , p0 = I[1], r15 
		dep OutWord[1] = Data[1], OutWord[1], (56 - (8 * (1))) , 8
		}
		{ .mmb
		add    r15  = r15 , SI[0]
		shladd T[0] = T[0], 2, r14 
(p6 )	br.cond.spnt.many .rc4Bypass13
		}
		;;
.rc4Resume13:

		{ .mmi
		ld1    Data[1] = [r16 ], 1
		padd1  I[1] = r20 , I[0]
		zxt1   r15  = r15 
		}
		{ .mmi
		ld4    T[0] = [T[0]]
		add    T[1] = SI[2], SJ[0]
		shladd IPr[1] = I[0], 2, r14 
		}
		;;
		{ .mmi
		st4    [IPr[2]] = SJ[0]
		st4    [JP[0]] = SI[2]
		zxt1   T[1] = T[1]
		}
		{ .mmi
		ld4    SI[1] = [IPr[1]]
		shladd JP[1] = r15, 2, r14 
		xor    Data[2] = Data[2], T[0]
		}
		;;
		{ .mmi
		ld4    SJ[1] = [JP[1]]
		cmp.eq p6 , p0 = I[0], r15 
		dep OutWord[1] = Data[2], OutWord[1], (56 - (8 * (2))) , 8
		}
		{ .mmb
		add    r15  = r15 , SI[1]
		shladd T[1] = T[1], 2, r14 
(p6 )	br.cond.spnt.many .rc4Bypass14
		}
		;;
.rc4Resume14:

		{ .mmi
		ld1    Data[2] = [r16 ], 1
		padd1  I[0] = r20 , I[1]
		zxt1   r15  = r15 
		}
		{ .mmi
		ld4    T[1] = [T[1]]
		add    T[0] = SI[0], SJ[1]
		shladd IPr[2] = I[1], 2, r14 
		}
		;;
		{ .mmi
		st4    [IPr[0]] = SJ[1]
		st4    [JP[1]] = SI[0]
		zxt1   T[0] = T[0]
		}
		{ .mmi
		ld4    SI[2] = [IPr[2]]
		shladd JP[0] = r15, 2, r14 
		xor    Data[3] = Data[3], T[1]
		}
		;;
		{ .mmi
		ld4    SJ[0] = [JP[0]]
		cmp.eq p6 , p0 = I[1], r15 
		dep OutWord[1] = Data[3], OutWord[1], (56 - (8 * (3))) , 8
		}
		{ .mmb
		add    r15  = r15 , SI[2]
		shladd T[0] = T[0], 2, r14 
(p6 )	br.cond.spnt.many .rc4Bypass15
		}
		;;
.rc4Resume15:

		{ .mmi
		ld1    Data[3] = [r16 ], 1
		padd1  I[1] = r20 , I[0]
		zxt1   r15  = r15 
		}
		{ .mmi
		ld4    T[0] = [T[0]]
		add    T[1] = SI[1], SJ[0]
		shladd IPr[0] = I[0], 2, r14 
		}
		;;
		{ .mmi
		st4    [IPr[1]] = SJ[0]
		st4    [JP[0]] = SI[1]
		zxt1   T[1] = T[1]
		}
		{ .mmi
		ld4    SI[0] = [IPr[0]]
		shladd JP[1] = r15, 2, r14 
		xor    Data[0] = Data[0], T[0]
		}
		;;
		{ .mmi
		ld4    SJ[1] = [JP[1]]
		cmp.eq p6 , p0 = I[0], r15 
		dep OutWord[1] = Data[0], OutWord[1], (56 - (8 * (4))) , 8
		}
		{ .mmb
		add    r15  = r15 , SI[0]
		shladd T[1] = T[1], 2, r14 
(p6 )	br.cond.spnt.many .rc4Bypass16
		}
		;;
.rc4Resume16:

		{ .mmi
		ld1    Data[0] = [r16 ], 1
		padd1  I[0] = r20 , I[1]
		zxt1   r15  = r15 
		}
		{ .mmi
		ld4    T[1] = [T[1]]
		add    T[0] = SI[2], SJ[1]
		shladd IPr[1] = I[1], 2, r14 
		}
		;;
		{ .mmi
		st4    [IPr[2]] = SJ[1]
		st4    [JP[1]] = SI[2]
		zxt1   T[0] = T[0]
		}
		{ .mmi
		ld4    SI[1] = [IPr[1]]
		shladd JP[0] = r15, 2, r14 
		xor    Data[1] = Data[1], T[1]
		}
		;;
		{ .mmi
		ld4    SJ[0] = [JP[0]]
		cmp.eq p6 , p0 = I[1], r15 
		dep OutWord[1] = Data[1], OutWord[1], (56 - (8 * (5))) , 8
		}
		{ .mmb
		add    r15  = r15 , SI[1]
		shladd T[0] = T[0], 2, r14 
(p6 )	br.cond.spnt.many .rc4Bypass17
		}
		;;
.rc4Resume17:

		{ .mmi
		ld1    Data[1] = [r16 ], 1
		padd1  I[1] = r20 , I[0]
		zxt1   r15  = r15 
		}
		{ .mmi
		ld4    T[0] = [T[0]]
		add    T[1] = SI[0], SJ[0]
		shladd IPr[2] = I[0], 2, r14 
		}
		;;
		{ .mmi
		st4    [IPr[0]] = SJ[0]
		st4    [JP[0]] = SI[0]
		zxt1   T[1] = T[1]
		}
		{ .mmi
		ld4    SI[2] = [IPr[2]]
		shladd JP[1] = r15, 2, r14 
		xor    Data[2] = Data[2], T[0]
		}
		;;
		{ .mmi
		ld4    SJ[1] = [JP[1]]
		cmp.eq p6 , p0 = I[0], r15 
		dep OutWord[1] = Data[2], OutWord[1], (56 - (8 * (6))) , 8
		}
		{ .mmb
		add    r15  = r15 , SI[2]
		shladd T[1] = T[1], 2, r14 
(p6 )	br.cond.spnt.many .rc4Bypass18
		}
		;;
.rc4Resume18:

		{ .mmi
		ld1    Data[2] = [r16 ], 1
		padd1  I[0] = r20 , I[1]
		zxt1   r15  = r15 
		}
		{ .mmi
		ld4    T[1] = [T[1]]
		add    T[0] = SI[1], SJ[1]
		shladd IPr[0] = I[1], 2, r14 
		}
		;;
		{ .mmi
		st4    [IPr[1]] = SJ[1]
		st4    [JP[1]] = SI[1]
		zxt1   T[0] = T[0]
		}
		{ .mmi
		ld4    SI[0] = [IPr[0]]
		shladd JP[0] = r15, 2, r14 
		xor    Data[3] = Data[3], T[1]
		}
		;;
		{ .mmi
		ld4    SJ[0] = [JP[0]]
		cmp.eq p6 , p0 = I[1], r15 
		dep OutWord[1] = Data[3], OutWord[1], (56 - (8 * (7))) , 8
		}
		{ .mmb
		add    r15  = r15 , SI[0]
		shladd T[0] = T[0], 2, r14 
(p6 )	br.cond.spnt.many .rc4Bypass19
		}
		;;
.rc4Resume19:

		{ .mmi
		ld1    Data[3] = [r16 ], 1
		padd1  I[1] = r20 , I[0]
		zxt1   r15  = r15 
		}
		{ .mmi
		ld4    T[0] = [T[0]]
		add    T[1] = SI[2], SJ[0]
		shladd IPr[1] = I[0], 2, r14 
		}
		;;
		{ .mmi
		st4    [IPr[2]] = SJ[0]
		st4    [JP[0]] = SI[2]
		zxt1   T[1] = T[1]
		}
		{ .mmi
		ld4    SI[1] = [IPr[1]]
		shladd JP[1] = r15, 2, r14 
		xor    Data[0] = Data[0], T[0]
		}
		;;
		{ .mmi
		ld4    SJ[1] = [JP[1]]
		cmp.eq p6 , p0 = I[0], r15 
		dep OutWord[0] = Data[0], OutWord[1], (56 - (8 * (0))) , 8
		}
		{ .mmb
		add    r15  = r15 , SI[1]
		shladd T[1] = T[1], 2, r14 
(p6 )	br.cond.spnt.many .rc4Bypass20
		}
		;;
.rc4Resume20:
		st8 [r17 ] = OutWord[1], 8

		{ .mmi
		ld1    Data[0] = [r16 ], 1
		padd1  I[0] = r20 , I[1]
		zxt1   r15  = r15 
		}
		{ .mmi
		ld4    T[1] = [T[1]]
		add    T[0] = SI[0], SJ[1]
		shladd IPr[2] = I[1], 2, r14 
		}
		;;
		{ .mmi
		st4    [IPr[0]] = SJ[1]
		st4    [JP[1]] = SI[0]
		zxt1   T[0] = T[0]
		}
		{ .mmi
		ld4    SI[2] = [IPr[2]]
		shladd JP[0] = r15, 2, r14 
		xor    Data[1] = Data[1], T[1]
		}
		;;
		{ .mmi
		ld4    SJ[0] = [JP[0]]
		cmp.eq p6 , p0 = I[1], r15 
		dep OutWord[0] = Data[1], OutWord[0], (56 - (8 * (1))) , 8
		}
		{ .mmb
		add    r15  = r15 , SI[2]
		shladd T[0] = T[0], 2, r14 
(p6 )	br.cond.spnt.many .rc4Bypass21
		}
		;;
.rc4Resume21:

		{ .mmi
		ld1    Data[1] = [r16 ], 1
		padd1  I[1] = r20 , I[0]
		zxt1   r15  = r15 
		}
		{ .mmi
		ld4    T[0] = [T[0]]
		add    T[1] = SI[1], SJ[0]
		shladd IPr[0] = I[0], 2, r14 
		}
		;;
		{ .mmi
		st4    [IPr[1]] = SJ[0]
		st4    [JP[0]] = SI[1]
		zxt1   T[1] = T[1]
		}
		{ .mmi
		ld4    SI[0] = [IPr[0]]
		shladd JP[1] = r15, 2, r14 
		xor    Data[2] = Data[2], T[0]
		}
		;;
		{ .mmi
		ld4    SJ[1] = [JP[1]]
		cmp.eq p6 , p0 = I[0], r15 
		dep OutWord[0] = Data[2], OutWord[0], (56 - (8 * (2))) , 8
		}
		{ .mmb
		add    r15  = r15 , SI[0]
		shladd T[1] = T[1], 2, r14 
(p6 )	br.cond.spnt.many .rc4Bypass22
		}
		;;
.rc4Resume22:

		{ .mmi
		ld1    Data[2] = [r16 ], 1
		padd1  I[0] = r20 , I[1]
		zxt1   r15  = r15 
		}
		{ .mmi
		ld4    T[1] = [T[1]]
		add    T[0] = SI[2], SJ[1]
		shladd IPr[1] = I[1], 2, r14 
		}
		;;
		{ .mmi
		st4    [IPr[2]] = SJ[1]
		st4    [JP[1]] = SI[2]
		zxt1   T[0] = T[0]
		}
		{ .mmi
		ld4    SI[1] = [IPr[1]]
		shladd JP[0] = r15, 2, r14 
		xor    Data[3] = Data[3], T[1]
		}
		;;
		{ .mmi
		ld4    SJ[0] = [JP[0]]
		cmp.eq p6 , p0 = I[1], r15 
		dep OutWord[0] = Data[3], OutWord[0], (56 - (8 * (3))) , 8
		}
		{ .mmb
		add    r15  = r15 , SI[1]
		shladd T[0] = T[0], 2, r14 
(p6 )	br.cond.spnt.many .rc4Bypass23
		}
		;;
.rc4Resume23:

		{ .mmi
		ld1    Data[3] = [r16 ], 1
		padd1  I[1] = r20 , I[0]
		zxt1   r15  = r15 
		}
		{ .mmi
		ld4    T[0] = [T[0]]
		add    T[1] = SI[0], SJ[0]
		shladd IPr[2] = I[0], 2, r14 
		}
		;;
		{ .mmi
		st4    [IPr[0]] = SJ[0]
		st4    [JP[0]] = SI[0]
		zxt1   T[1] = T[1]
		}
		{ .mmi
		ld4    SI[2] = [IPr[2]]
		shladd JP[1] = r15, 2, r14 
		xor    Data[0] = Data[0], T[0]
		}
		;;
		{ .mmi
		ld4    SJ[1] = [JP[1]]
		cmp.eq p6 , p0 = I[0], r15 
		dep OutWord[0] = Data[0], OutWord[0], (56 - (8 * (4))) , 8
		}
		{ .mmb
		add    r15  = r15 , SI[2]
		shladd T[1] = T[1], 2, r14 
(p6 )	br.cond.spnt.many .rc4Bypass24
		}
		;;
.rc4Resume24:

		{ .mmi
		ld1    Data[0] = [r16 ], 1
		padd1  I[0] = r20 , I[1]
		zxt1   r15  = r15 
		}
		{ .mmi
		ld4    T[1] = [T[1]]
		add    T[0] = SI[1], SJ[1]
		shladd IPr[0] = I[1], 2, r14 
		}
		;;
		{ .mmi
		st4    [IPr[1]] = SJ[1]
		st4    [JP[1]] = SI[1]
		zxt1   T[0] = T[0]
		}
		{ .mmi
		ld4    SI[0] = [IPr[0]]
		shladd JP[0] = r15, 2, r14 
		xor    Data[1] = Data[1], T[1]
		}
		;;
		{ .mmi
		ld4    SJ[0] = [JP[0]]
		cmp.eq p6 , p0 = I[1], r15 
		dep OutWord[0] = Data[1], OutWord[0], (56 - (8 * (5))) , 8
		}
		{ .mmb
		add    r15  = r15 , SI[0]
		shladd T[0] = T[0], 2, r14 
(p6 )	br.cond.spnt.many .rc4Bypass25
		}
		;;
.rc4Resume25:

		{ .mmi
		ld1    Data[1] = [r16 ], 1
		padd1  I[1] = r20 , I[0]
		zxt1   r15  = r15 
		}
		{ .mmi
		ld4    T[0] = [T[0]]
		add    T[1] = SI[2], SJ[0]
		shladd IPr[1] = I[0], 2, r14 
		}
		;;
		{ .mmi
		st4    [IPr[2]] = SJ[0]
		st4    [JP[0]] = SI[2]
		zxt1   T[1] = T[1]
		}
		{ .mmi
		ld4    SI[1] = [IPr[1]]
		shladd JP[1] = r15, 2, r14 
		xor    Data[2] = Data[2], T[0]
		}
		;;
		{ .mmi
		ld4    SJ[1] = [JP[1]]
		cmp.eq p6 , p0 = I[0], r15 
		dep OutWord[0] = Data[2], OutWord[0], (56 - (8 * (6))) , 8
		}
		{ .mmb
		add    r15  = r15 , SI[1]
		shladd T[1] = T[1], 2, r14 
(p6 )	br.cond.spnt.many .rc4Bypass26
		}
		;;
.rc4Resume26:

		{ .mmi
		ld1    Data[2] = [r16 ], 1
		padd1  I[0] = r20 , I[1]
		zxt1   r15  = r15 
		}
		{ .mmi
		ld4    T[1] = [T[1]]
		add    T[0] = SI[0], SJ[1]
		shladd IPr[2] = I[1], 2, r14 
		}
		;;
		{ .mmi
		st4    [IPr[0]] = SJ[1]
		st4    [JP[1]] = SI[0]
		zxt1   T[0] = T[0]
		}
		{ .mmi
		ld4    SI[2] = [IPr[2]]
		shladd JP[0] = r15, 2, r14 
		xor    Data[3] = Data[3], T[1]
		}
		;;
		{ .mmi
		ld4    SJ[0] = [JP[0]]
		cmp.eq p6 , p0 = I[1], r15 
		dep OutWord[0] = Data[3], OutWord[0], (56 - (8 * (7))) , 8
		}
		{ .mmb
		add    r15  = r15 , SI[2]
		shladd T[0] = T[0], 2, r14 
(p6 )	br.cond.spnt.many .rc4Bypass27
		}
		;;
.rc4Resume27:

		{ .mmi
		ld1    Data[3] = [r16 ], 1
		padd1  I[1] = r20 , I[0]
		zxt1   r15  = r15 
		}
		{ .mmi
		ld4    T[0] = [T[0]]
		add    T[1] = SI[1], SJ[0]
		shladd IPr[0] = I[0], 2, r14 
		}
		;;
		{ .mmi
		st4    [IPr[1]] = SJ[0]
		st4    [JP[0]] = SI[1]
		zxt1   T[1] = T[1]
		}
		{ .mmi
		ld4    SI[0] = [IPr[0]]
		shladd JP[1] = r15, 2, r14 
		xor    Data[0] = Data[0], T[0]
		}
		;;
		{ .mmi
		ld4    SJ[1] = [JP[1]]
		cmp.eq p6 , p0 = I[0], r15 
		dep OutWord[1] = Data[0], OutWord[0], (56 - (8 * (0))) , 8
		}
		{ .mmb
		add    r15  = r15 , SI[0]
		shladd T[1] = T[1], 2, r14 
(p6 )	br.cond.spnt.many .rc4Bypass28
		}
		;;
.rc4Resume28:
		st8 [r17 ] = OutWord[0], 8
		mov OutWord[0] = OutWord[1]
		lfetch.nt1 [r18 ], 24
		lfetch.excl.nt1 [r19 ], 24
		br.cloop.sptk.few .rc4Loop
.rc4Epilogue:

		zxt1   r15  = r15 
		ld4    T[1] = [T[1]]
		add    T[0] = SI[2], SJ[1]
		;;
		st4    [IPr[2]] = SJ[1]
		st4    [JP[1]] = SI[2]
		zxt1   T[0] = T[0]
		shladd JP[0] = r15, 2, r14 
		xor    Data[1] = Data[1], T[1]
		;;
		ld4    SJ[0] = [JP[0]]
		dep OutWord[1] = Data[1], OutWord[1], (56 - (8 * (1))) , 8
		shladd T[0] = T[0], 2, r14 
		;;

		ld4    T[0] = [T[0]]
		add    T[1] = SI[0], SJ[0]
		;;
		st4    [IPr[0]] = SJ[0]
		st4    [JP[0]] = SI[0]
		zxt1   T[1] = T[1]
		xor    Data[2] = Data[2], T[0]
		;;
		dep OutWord[1] = Data[2], OutWord[1], (56 - (8 * (2))) , 8
		shladd T[1] = T[1], 2, r14 
		;;

		ld4    T[1] = [T[1]]
		;;
		xor    Data[3] = Data[3], T[1]
		;;
		dep OutWord[1] = Data[3], OutWord[1], (56 - (8 * (3))) , 8
		;;


		shr.u	OutWord[1] = OutWord[1], 32;;

		st4 [r17 ] = OutWord[1], 4
	{
		.mmi
		lfetch.nt1	[r24 ]
		mov		r23  = I[1]
		nop		0x0
	}

.rc4Remainder:
	{
		.mmi
		sub		r22  = r24 , r16 


		nop		0x0
		nop		0x0
	} ;;
	{
		.mib
		cmp.eq		p7 , p0 = -1, r22 
		mov.i		ar.lc = r22 
(p7 )		br.cond.dptk.few .rc4Complete
	}



		{ ld1 Data[0] = [r16], 1; add r23 = 1, I[1]; shladd IPr[0] = I[1], 2, r14; } ;; { ld4 SI[0] = [IPr[0]]; mov pr.rot = 0x10000; mov ar.ec = 4; } ;; { add r15 = r15, SI[0]; zxt1 I[0] = r23; br.cexit.spnt.few .+16; } ;; 
		.RC4RestLoop: { .mmi; (pPhase[0]) ld1 Data[0] = [r16], 1; (pPhase[0]) add r23 = 1, I[1]; (pPhase[1]) zxt1 r15 = r15; }{ .mmi; (pPhase[3]) ld4 T[1] = [T[1]]; (pPhase[2]) add T[0] = SI[2], SJ[1]; (pPhase[0]) shladd IPr[0] = I[1], 2, r14; } ;; { .mmi; (pPhase[2]) st4 [IPr[2]] = SJ[1]; (pPhase[2]) st4 [JP[1]] = SI[2]; (pPhase[2]) zxt1 T[0] = T[0]; }{ .mmi; (pPhase[0]) ld4 SI[0] = [IPr[0]]; (pPhase[1]) shladd JP[0] = r15, 2, r14; (pPhase[0]) cmp.eq.unc p6, p0 = I[1], r15; } ;; { .mmi; (pPhase[1]) ld4 SJ[0] = [JP[0]]; (pPhase[3]) xor Data[3] = Data[3], T[1]; nop 0x0; }{ .mmi; (pPhase[2]) shladd T[0] = T[0], 2, r14; (p6) mov SI[0] = SI[1]; (pPhase[0]) zxt1 I[0] = r23; } ;; { .mmb; (pPhase[3]) st1 [r17] = Data[3], 1; (pPhase[0]) add r15 = r15, SI[0]; br.ctop.sptk.few .RC4RestLoop; } ;; 

.rc4Complete:
	{
		.mmi
		add		r14  = - 4 , r14 
		add		r23  = -1, r23 
		mov		ar.lc = r8 
	} ;;
	{
		.mii
		st4 		[r14 ] = r15 ,- 4 
		zxt1		r23  = r23 
		mov		pr = r9 , 0x1FFFF
	} ;;
	{
		.mib
		st4 		[r14 ] = r23 
		add		r8  = 1, r0
		br.ret.sptk.few	rp
	} ;;
.rc4Bypass2:
		sub r15  = r15 , SI[1]
		nop 0
		nop 0
		;;
		add r15  = r15 , SI[0]
		mov SI[1] = SI[0]
		br.sptk.many .rc4Resume2

		;;
.rc4Bypass3:
		sub r15  = r15 , SI[2]
		nop 0
		nop 0
		;;
		add r15  = r15 , SI[1]
		mov SI[2] = SI[1]
		br.sptk.many .rc4Resume3

		;;
.rc4Bypass4:
		sub r15  = r15 , SI[0]
		nop 0
		nop 0
		;;
		add r15  = r15 , SI[2]
		mov SI[0] = SI[2]
		br.sptk.many .rc4Resume4

		;;
.rc4Bypass5:
		sub r15  = r15 , SI[1]
		nop 0
		nop 0
		;;
		add r15  = r15 , SI[0]
		mov SI[1] = SI[0]
		br.sptk.many .rc4Resume5

		;;
.rc4Bypass6:
		sub r15  = r15 , SI[2]
		nop 0
		nop 0
		;;
		add r15  = r15 , SI[1]
		mov SI[2] = SI[1]
		br.sptk.many .rc4Resume6

		;;
.rc4Bypass7:
		sub r15  = r15 , SI[0]
		nop 0
		nop 0
		;;
		add r15  = r15 , SI[2]
		mov SI[0] = SI[2]
		br.sptk.many .rc4Resume7

		;;
.rc4Bypass8:
		sub r15  = r15 , SI[1]
		nop 0
		nop 0
		;;
		add r15  = r15 , SI[0]
		mov SI[1] = SI[0]
		br.sptk.many .rc4Resume8

		;;
.rc4Bypass9:
		sub r15  = r15 , SI[2]
		nop 0
		nop 0
		;;
		add r15  = r15 , SI[1]
		mov SI[2] = SI[1]
		br.sptk.many .rc4Resume9

		;;
.rc4Bypass10:
		sub r15  = r15 , SI[0]
		nop 0
		nop 0
		;;
		add r15  = r15 , SI[2]
		mov SI[0] = SI[2]
		br.sptk.many .rc4Resume10

		;;
.rc4Bypass11:
		sub r15  = r15 , SI[1]
		nop 0
		nop 0
		;;
		add r15  = r15 , SI[0]
		mov SI[1] = SI[0]
		br.sptk.many .rc4Resume11

		;;
.rc4Bypass12:
		sub r15  = r15 , SI[2]
		nop 0
		nop 0
		;;
		add r15  = r15 , SI[1]
		mov SI[2] = SI[1]
		br.sptk.many .rc4Resume12

		;;
.rc4Bypass13:
		sub r15  = r15 , SI[0]
		nop 0
		nop 0
		;;
		add r15  = r15 , SI[2]
		mov SI[0] = SI[2]
		br.sptk.many .rc4Resume13

		;;
.rc4Bypass14:
		sub r15  = r15 , SI[1]
		nop 0
		nop 0
		;;
		add r15  = r15 , SI[0]
		mov SI[1] = SI[0]
		br.sptk.many .rc4Resume14

		;;
.rc4Bypass15:
		sub r15  = r15 , SI[2]
		nop 0
		nop 0
		;;
		add r15  = r15 , SI[1]
		mov SI[2] = SI[1]
		br.sptk.many .rc4Resume15

		;;
.rc4Bypass16:
		sub r15  = r15 , SI[0]
		nop 0
		nop 0
		;;
		add r15  = r15 , SI[2]
		mov SI[0] = SI[2]
		br.sptk.many .rc4Resume16

		;;
.rc4Bypass17:
		sub r15  = r15 , SI[1]
		nop 0
		nop 0
		;;
		add r15  = r15 , SI[0]
		mov SI[1] = SI[0]
		br.sptk.many .rc4Resume17

		;;
.rc4Bypass18:
		sub r15  = r15 , SI[2]
		nop 0
		nop 0
		;;
		add r15  = r15 , SI[1]
		mov SI[2] = SI[1]
		br.sptk.many .rc4Resume18

		;;
.rc4Bypass19:
		sub r15  = r15 , SI[0]
		nop 0
		nop 0
		;;
		add r15  = r15 , SI[2]
		mov SI[0] = SI[2]
		br.sptk.many .rc4Resume19

		;;
.rc4Bypass20:
		sub r15  = r15 , SI[1]
		nop 0
		nop 0
		;;
		add r15  = r15 , SI[0]
		mov SI[1] = SI[0]
		br.sptk.many .rc4Resume20

		;;
.rc4Bypass21:
		sub r15  = r15 , SI[2]
		nop 0
		nop 0
		;;
		add r15  = r15 , SI[1]
		mov SI[2] = SI[1]
		br.sptk.many .rc4Resume21

		;;
.rc4Bypass22:
		sub r15  = r15 , SI[0]
		nop 0
		nop 0
		;;
		add r15  = r15 , SI[2]
		mov SI[0] = SI[2]
		br.sptk.many .rc4Resume22

		;;
.rc4Bypass23:
		sub r15  = r15 , SI[1]
		nop 0
		nop 0
		;;
		add r15  = r15 , SI[0]
		mov SI[1] = SI[0]
		br.sptk.many .rc4Resume23

		;;
.rc4Bypass24:
		sub r15  = r15 , SI[2]
		nop 0
		nop 0
		;;
		add r15  = r15 , SI[1]
		mov SI[2] = SI[1]
		br.sptk.many .rc4Resume24

		;;
.rc4Bypass25:
		sub r15  = r15 , SI[0]
		nop 0
		nop 0
		;;
		add r15  = r15 , SI[2]
		mov SI[0] = SI[2]
		br.sptk.many .rc4Resume25

		;;
.rc4Bypass26:
		sub r15  = r15 , SI[1]
		nop 0
		nop 0
		;;
		add r15  = r15 , SI[0]
		mov SI[1] = SI[0]
		br.sptk.many .rc4Resume26

		;;
.rc4Bypass27:
		sub r15  = r15 , SI[2]
		nop 0
		nop 0
		;;
		add r15  = r15 , SI[1]
		mov SI[2] = SI[1]
		br.sptk.many .rc4Resume27

		;;
.rc4Bypass28:
		sub r15  = r15 , SI[0]
		nop 0
		nop 0
		;;
		add r15  = r15 , SI[2]
		mov SI[0] = SI[2]
		br.sptk.many .rc4Resume28

		;;
	.endp RC4

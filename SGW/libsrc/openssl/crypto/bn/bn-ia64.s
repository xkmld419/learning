# 1 "asm/ia64.S"
.explicit
.text
.ident	"ia64.S, Version 2.1"
.ident	"IA-64 ISA artwork by Andy Polyakov <appro@fy.chalmers.se>"

# 168
.global	bn_add_words#
.proc	bn_add_words#
.align	64
.skip	32
bn_add_words:
	.prologue
	.save	ar.pfs,r2
{ .mii;	alloc		r2=ar.pfs,4,12,0,16
	cmp4.le		p6,p0=r35,r0	};;
{ .mfb;	mov		r8=r0
(p6)	br.ret.spnt.many	b0	};;

{ .mib;	sub		r10=r35,r0,1
	.save	ar.lc,r3
	mov		r3=ar.lc
	brp.loop.imp	.L_bn_add_words_ctop,.L_bn_add_words_cend-16
					}
{ .mib;	add 		r14=0,r32
	.save	pr,r9
	mov		r9=pr		};;
	.body
{ .mii;	add 		r15=0,r33
	mov		ar.lc=r10
	mov		ar.ec=6		}
{ .mib;	add 		r16=0,r34
	mov		pr.rot=1<<16	};;

.L_bn_add_words_ctop:
{ .mii;	(p16)	ld8		r32=[r16],8
	(p18)	add		r39=r37,r34
	(p19)	cmp.ltu.unc	p56,p0=r40,r38	}
{ .mfb;	(p0)	nop.m		0x0
	(p0)	nop.f		0x0
	(p0)	nop.b		0x0		}
{ .mii;	(p16)	ld8		r35=[r15],8
	(p58)	cmp.eq.or	p57,p0=-1,r41
	(p58)	add		r41=1,r41	}
{ .mfb;	(p21)	st8		[r14]=r42,8
	(p0)	nop.f		0x0
	br.ctop.sptk	.L_bn_add_words_ctop	};;
.L_bn_add_words_cend:

{ .mii;
(p59)	add		r8=1,r8
	mov		pr=r9,0x1ffff
	mov		ar.lc=r3	}
{ .mbb;	nop.b		0x0
	br.ret.sptk.many	b0	};;
.endp	bn_add_words#




.global	bn_sub_words#
.proc	bn_sub_words#
.align	64
.skip	32
bn_sub_words:
	.prologue
	.save	ar.pfs,r2
{ .mii;	alloc		r2=ar.pfs,4,12,0,16
	cmp4.le		p6,p0=r35,r0	};;
{ .mfb;	mov		r8=r0
(p6)	br.ret.spnt.many	b0	};;

{ .mib;	sub		r10=r35,r0,1
	.save	ar.lc,r3
	mov		r3=ar.lc
	brp.loop.imp	.L_bn_sub_words_ctop,.L_bn_sub_words_cend-16
					}
{ .mib;	add 		r14=0,r32
	.save	pr,r9
	mov		r9=pr		};;
	.body
{ .mii;	add 		r15=0,r33
	mov		ar.lc=r10
	mov		ar.ec=6		}
{ .mib;	add 		r16=0,r34
	mov		pr.rot=1<<16	};;

.L_bn_sub_words_ctop:
{ .mii;	(p16)	ld8		r32=[r16],8
	(p18)	sub		r39=r37,r34
	(p19)	cmp.gtu.unc	p56,p0=r40,r38	}
{ .mfb;	(p0)	nop.m		0x0
	(p0)	nop.f		0x0
	(p0)	nop.b		0x0		}
{ .mii;	(p16)	ld8		r35=[r15],8
	(p58)	cmp.eq.or	p57,p0=0,r41
	(p58)	add		r41=-1,r41	}
{ .mbb;	(p21)	st8		[r14]=r42,8
	(p0)	nop.b		0x0
	br.ctop.sptk	.L_bn_sub_words_ctop	};;
.L_bn_sub_words_cend:

{ .mii;
(p59)	add		r8=1,r8
	mov		pr=r9,0x1ffff
	mov		ar.lc=r3	}
{ .mbb;	nop.b		0x0
	br.ret.sptk.many	b0	};;
.endp	bn_sub_words#
# 280
.global	bn_mul_words#
.proc	bn_mul_words#
.align	64
.skip	32
bn_mul_words:
	.prologue
	.save	ar.pfs,r2



{ .mfi;	alloc		r2=ar.pfs,4,12,0,16	};;

{ .mib;	mov		r8=r0
	cmp4.le		p6,p0=r34,r0
(p6)	br.ret.spnt.many	b0		};;

{ .mii;	sub	r10=r34,r0,1
	.save	ar.lc,r3
	mov	r3=ar.lc
	.save	pr,r9
	mov	r9=pr			};;

	.body
{ .mib;	setf.sig	f8=r35
	mov		pr.rot=0x800001<<16

	brp.loop.imp	.L_bn_mul_words_ctop,.L_bn_mul_words_cend-16
					}



{ .mmi;	add 		r14=0,r32
	add 		r15=0,r33
	mov		ar.lc=r10	}
{ .mmi;	mov		r40=0
	mov		ar.ec=13	};;
# 328
.L_bn_mul_words_ctop:
{ .mfi;	(p25)	getf.sig	r36=f52
	(p21)	xmpy.lu		f48=f37,f8
	(p28)	cmp.ltu		p54,p50=r41,r39	}
{ .mfi;	(p16)	ldf8		f32=[r15],8
	(p21)	xmpy.hu		f40=f37,f8
	(p0)	nop.i		0x0		};;
{ .mii;	(p25)	getf.sig	r32=f44
	.pred.rel	"mutex",p50,p54
	(p50)	add		r40=r38,r35
	(p54)	add		r40=r38,r35,1	}
{ .mfb;	(p28)	st8		[r14]=r41,8
	(p0)	nop.f		0x0
	br.ctop.sptk	.L_bn_mul_words_ctop	};;
.L_bn_mul_words_cend:

{ .mii;	nop.m		0x0
.pred.rel	"mutex",p51,p55
(p51)	add		r8=r36,r0
(p55)	add		r8=r36,r0,1	}
{ .mfb;	nop.m	0x0
	nop.f	0x0
	nop.b	0x0			}
# 381
{ .mii;	nop.m		0x0
	mov		pr=r9,0x1ffff
	mov		ar.lc=r3	}
{ .mfb;	rum		1<<5
	nop.f		0x0
	br.ret.sptk.many	b0	};;
.endp	bn_mul_words#
# 394
.global	bn_mul_add_words#
.proc	bn_mul_add_words#
.align	64
.skip	48
bn_mul_add_words:
	.prologue
	.save	ar.pfs,r2
{ .mmi;	alloc		r2=ar.pfs,4,4,0,8
	cmp4.le		p6,p0=r34,r0
	.save	ar.lc,r3
	mov		r3=ar.lc	};;
{ .mib;	mov		r8=r0
	sub		r10=r34,r0,1
(p6)	br.ret.spnt.many	b0	};;

{ .mib;	setf.sig	f8=r35
	.save	pr,r9
	mov		r9=pr
	brp.loop.imp	.L_bn_mul_add_words_ctop,.L_bn_mul_add_words_cend-16
					}
	.body
{ .mmi;	add 		r14=0,r32
	add 		r15=0,r33
	mov		ar.lc=r10	}
{ .mii;	add 		r16=0,r32
	mov		pr.rot=0x2001<<16

	mov		ar.ec=11	};;
# 432
.L_bn_mul_add_words_ctop:
.pred.rel	"mutex",p40,p42
{ .mfi;	(p23)	getf.sig	r36=f45
	(p20)	xma.lu		f42=f36,f8,f50
	(p40)	add		r39=r39,r35	}
{ .mfi;	(p16)	ldf8		f32=[r15],8
	(p20)	xma.hu		f36=f36,f8,f50
	(p42)	add		r39=r39,r35,1	};;
{ .mmi;	(p24)	getf.sig	r32=f40
	(p16)	ldf8		f46=[r16],8
	(p40)	cmp.ltu		p41,p39=r39,r35	}
{ .mib;	(p26)	st8		[r14]=r39,8
	(p42)	cmp.leu		p41,p39=r39,r35
	br.ctop.sptk	.L_bn_mul_add_words_ctop};;
.L_bn_mul_add_words_cend:

{ .mmi;	.pred.rel	"mutex",p40,p42
(p40)	add		r8=r35,r0
(p42)	add		r8=r35,r0,1
	mov		pr=r9,0x1ffff	}
{ .mib;	rum		1<<5
	mov		ar.lc=r3
	br.ret.sptk.many	b0	};;
.endp	bn_mul_add_words#
# 462
.global	bn_sqr_words#
.proc	bn_sqr_words#
.align	64
.skip	32
bn_sqr_words:
	.prologue
	.save	ar.pfs,r2
{ .mii;	alloc		r2=ar.pfs,3,0,0,0
	sxt4		r34=r34		};;
{ .mii;	cmp.le		p6,p0=r34,r0
	mov		r8=r0		}
{ .mfb;	add 		r32=0,r32
	nop.f		0x0
(p6)	br.ret.spnt.many	b0	};;

{ .mii;	sub	r10=r34,r0,1
	.save	ar.lc,r3
	mov	r3=ar.lc
	.save	pr,r9
	mov	r9=pr			};;

	.body
{ .mib;	add 		r33=0,r33
	mov		pr.rot=1<<16
	brp.loop.imp	.L_bn_sqr_words_ctop,.L_bn_sqr_words_cend-16
					}
{ .mii;	add		r34=8,r32
	mov		ar.lc=r10
	mov		ar.ec=18	};;
# 499
.L_bn_sqr_words_ctop:
{ .mfi;	(p16)	ldf8		f32=[r33],8
	(p25)	xmpy.lu		f42=f41,f41
	(p0)	nop.i		0x0		}
{ .mib;	(p33)	stf8		[r32]=f50,16
	(p0)	nop.i		0x0
	(p0)	nop.b		0x0		}
{ .mfi;	(p0)	nop.m		0x0
	(p25)	xmpy.hu		f52=f41,f41
	(p0)	nop.i		0x0		}
{ .mib;	(p33)	stf8		[r34]=f60,16
	(p0)	nop.i		0x0
	br.ctop.sptk	.L_bn_sqr_words_ctop	};;
.L_bn_sqr_words_cend:

{ .mii;	nop.m		0x0
	mov		pr=r9,0x1ffff
	mov		ar.lc=r3	}
{ .mfb;	rum		1<<5
	nop.f		0x0
	br.ret.sptk.many	b0	};;
.endp	bn_sqr_words#
# 542
.global	bn_sqr_comba8#
.proc	bn_sqr_comba8#
.align	64
bn_sqr_comba8:
	.prologue
	.save	ar.pfs,r2
# 554
{ .mii;	alloc	r2=ar.pfs,2,1,0,0

	mov	r34=r33
	add	r14=8,r33		};;
	.body
{ .mii;	add	r17=8,r34
	add	r15=16,r33
	add	r18=16,r34		}
{ .mfb;	add	r16=24,r33
	br	.L_cheat_entry_point8	};;
.endp	bn_sqr_comba8#
# 613
.global	bn_mul_comba8#
.proc	bn_mul_comba8#
.align	64
bn_mul_comba8:
	.prologue
	.save	ar.pfs,r2
# 625
{ .mii;	alloc   r2=ar.pfs,3,0,0,0

	add	r14=8,r33
	add	r17=8,r34		}
	.body
{ .mii;	add	r15=16,r33
	add	r18=16,r34
	add	r16=24,r33		}
.L_cheat_entry_point8:
{ .mmi;	add	r19=24,r34

	ldf8	f32=[r33],32		};;

{ .mmi;	ldf8	f120=[r34],32
	ldf8	f121=[r17],32		}
{ .mmi;	ldf8	f122=[r18],32
	ldf8	f123=[r19],32		};;
{ .mmi;	ldf8	f124=[r34]
	ldf8	f125=[r17]		}
{ .mmi;	ldf8	f126=[r18]
	ldf8	f127=[r19]		}

{ .mmi;	ldf8	f33=[r14],32
	ldf8	f34=[r15],32		}
{ .mmi;	ldf8	f35=[r16],32;;
	ldf8	f36=[r33]		}
{ .mmi;	ldf8	f37=[r14]
	ldf8	f38=[r15]		}
{ .mfi;	ldf8	f39=[r16]




		xma.hu	f41=f32,f120,f0		}
{ .mfi;		xma.lu	f40=f32,f120,f0		};;
{ .mfi;		xma.hu	f51=f32,f121,f0		}
{ .mfi;		xma.lu	f50=f32,f121,f0		};;
{ .mfi;		xma.hu	f61=f32,f122,f0		}
{ .mfi;		xma.lu	f60=f32,f122,f0		};;
{ .mfi;		xma.hu	f71=f32,f123,f0		}
{ .mfi;		xma.lu	f70=f32,f123,f0		};;
{ .mfi;		xma.hu	f81=f32,f124,f0		}
{ .mfi;		xma.lu	f80=f32,f124,f0		};;
{ .mfi;		xma.hu	f91=f32,f125,f0		}
{ .mfi;		xma.lu	f90=f32,f125,f0		};;
{ .mfi;		xma.hu	f101=f32,f126,f0	}
{ .mfi;		xma.lu	f100=f32,f126,f0	};;
{ .mfi;		xma.hu	f111=f32,f127,f0	}
{ .mfi;		xma.lu	f110=f32,f127,f0	};;
# 685
{ .mfi;	getf.sig	r16=f40
		xma.hu	f42=f33,f120,f41
	add		r33=8,r32		}
{ .mfi;		xma.lu	f41=f33,f120,f41	};;
{ .mfi;	getf.sig	r24=f50
		xma.hu	f52=f33,f121,f51	}
{ .mfi;		xma.lu	f51=f33,f121,f51	};;
{ .mfi;	st8		[r32]=r16,16
		xma.hu	f62=f33,f122,f61	}
{ .mfi;		xma.lu	f61=f33,f122,f61	};;
{ .mfi;		xma.hu	f72=f33,f123,f71	}
{ .mfi;		xma.lu	f71=f33,f123,f71	};;
{ .mfi;		xma.hu	f82=f33,f124,f81	}
{ .mfi;		xma.lu	f81=f33,f124,f81	};;
{ .mfi;		xma.hu	f92=f33,f125,f91	}
{ .mfi;		xma.lu	f91=f33,f125,f91	};;
{ .mfi;		xma.hu	f102=f33,f126,f101	}
{ .mfi;		xma.lu	f101=f33,f126,f101	};;
{ .mfi;		xma.hu	f112=f33,f127,f111	}
{ .mfi;		xma.lu	f111=f33,f127,f111	};;

{ .mfi;	getf.sig	r25=f41
		xma.hu	f43=f34,f120,f42	}
{ .mfi;		xma.lu	f42=f34,f120,f42	};;
{ .mfi;	getf.sig	r16=f60
		xma.hu	f53=f34,f121,f52	}
{ .mfi;		xma.lu	f52=f34,f121,f52	};;
{ .mfi;	getf.sig	r17=f51
		xma.hu	f63=f34,f122,f62
	add		r25=r25,r24		}
{ .mfi;		xma.lu	f62=f34,f122,f62
	mov		r14 =0		};;
{ .mfi;	cmp.ltu		p6,p0=r25,r24
		xma.hu	f73=f34,f123,f72	}
{ .mfi;		xma.lu	f72=f34,f123,f72	};;
{ .mfi;	st8		[r33]=r25,16
		xma.hu	f83=f34,f124,f82
(p6)	add		r14 =1,r14 		}
{ .mfi;		xma.lu	f82=f34,f124,f82	};;
{ .mfi;		xma.hu	f93=f34,f125,f92	}
{ .mfi;		xma.lu	f92=f34,f125,f92	};;
{ .mfi;		xma.hu	f103=f34,f126,f102	}
{ .mfi;		xma.lu	f102=f34,f126,f102	};;
{ .mfi;		xma.hu	f113=f34,f127,f112	}
{ .mfi;		xma.lu	f112=f34,f127,f112	};;

{ .mfi;	getf.sig	r18=f42
		xma.hu	f44=f35,f120,f43
	add		r17=r17,r16		}
{ .mfi;		xma.lu	f43=f35,f120,f43	};;
{ .mfi;	getf.sig	r24=f70
		xma.hu	f54=f35,f121,f53	}
{ .mfi;	mov		r15 =0
		xma.lu	f53=f35,f121,f53	};;
{ .mfi;	getf.sig	r25=f61
		xma.hu	f64=f35,f122,f63
	cmp.ltu		p7,p0=r17,r16		}
{ .mfi;	add		r18=r18,r17
		xma.lu	f63=f35,f122,f63	};;
{ .mfi;	getf.sig	r26=f52
		xma.hu	f74=f35,f123,f73
(p7)	add		r15 =1,r15 		}
{ .mfi;	cmp.ltu		p7,p0=r18,r17
		xma.lu	f73=f35,f123,f73
	add		r18=r18,r14 		};;
{ .mfi;
		xma.hu	f84=f35,f124,f83
(p7)	add		r15 =1,r15 		}
{ .mfi;	cmp.ltu		p7,p0=r18,r14 
		xma.lu	f83=f35,f124,f83	};;
{ .mfi;	st8		[r32]=r18,16
		xma.hu	f94=f35,f125,f93
(p7)	add		r15 =1,r15 		}
{ .mfi;		xma.lu	f93=f35,f125,f93	};;
{ .mfi;		xma.hu	f104=f35,f126,f103	}
{ .mfi;		xma.lu	f103=f35,f126,f103	};;
{ .mfi;		xma.hu	f114=f35,f127,f113	}
{ .mfi;	mov		r14 =0
		xma.lu	f113=f35,f127,f113
	add		r25=r25,r24		};;

{ .mfi;	getf.sig	r27=f43
		xma.hu	f45=f36,f120,f44
	cmp.ltu		p6,p0=r25,r24		}
{ .mfi;		xma.lu	f44=f36,f120,f44
	add		r26=r26,r25		};;
{ .mfi;	getf.sig	r16=f80
		xma.hu	f55=f36,f121,f54
(p6)	add		r14 =1,r14 		}
{ .mfi;		xma.lu	f54=f36,f121,f54	};;
{ .mfi;	getf.sig	r17=f71
		xma.hu	f65=f36,f122,f64
	cmp.ltu		p6,p0=r26,r25		}
{ .mfi;		xma.lu	f64=f36,f122,f64
	add		r27=r27,r26		};;
{ .mfi;	getf.sig	r18=f62
		xma.hu	f75=f36,f123,f74
(p6)	add		r14 =1,r14 		}
{ .mfi;	cmp.ltu		p6,p0=r27,r26
		xma.lu	f74=f36,f123,f74
	add		r27=r27,r15 		};;
{ .mfi;	getf.sig	r19=f53
		xma.hu	f85=f36,f124,f84
(p6)	add		r14 =1,r14 		}
{ .mfi;		xma.lu	f84=f36,f124,f84
	cmp.ltu		p6,p0=r27,r15 	};;
{ .mfi;	st8		[r33]=r27,16
		xma.hu	f95=f36,f125,f94
(p6)	add		r14 =1,r14 		}
{ .mfi;		xma.lu	f94=f36,f125,f94	};;
{ .mfi;		xma.hu	f105=f36,f126,f104	}
{ .mfi;	mov		r15 =0
		xma.lu	f104=f36,f126,f104
	add		r17=r17,r16		};;
{ .mfi;		xma.hu	f115=f36,f127,f114
	cmp.ltu		p7,p0=r17,r16		}
{ .mfi;		xma.lu	f114=f36,f127,f114
	add		r18=r18,r17		};;

{ .mfi;	getf.sig	r20=f44
		xma.hu	f46=f37,f120,f45
(p7)	add		r15 =1,r15 		}
{ .mfi;	cmp.ltu		p7,p0=r18,r17
		xma.lu	f45=f37,f120,f45
	add		r19=r19,r18		};;
{ .mfi;	getf.sig	r24=f90
		xma.hu	f56=f37,f121,f55	}
{ .mfi;		xma.lu	f55=f37,f121,f55	};;
{ .mfi;	getf.sig	r25=f81
		xma.hu	f66=f37,f122,f65
(p7)	add		r15 =1,r15 		}
{ .mfi;	cmp.ltu		p7,p0=r19,r18
		xma.lu	f65=f37,f122,f65
	add		r20=r20,r19		};;
{ .mfi;	getf.sig	r26=f72
		xma.hu	f76=f37,f123,f75
(p7)	add		r15 =1,r15 		}
{ .mfi;	cmp.ltu		p7,p0=r20,r19
		xma.lu	f75=f37,f123,f75
	add		r20=r20,r14 		};;
{ .mfi;	getf.sig	r27=f63
		xma.hu	f86=f37,f124,f85
(p7)	add		r15 =1,r15 		}
{ .mfi;		xma.lu	f85=f37,f124,f85
	cmp.ltu		p7,p0=r20,r14 	};;
{ .mfi;	getf.sig	r28=f54
		xma.hu	f96=f37,f125,f95
(p7)	add		r15 =1,r15 		}
{ .mfi;	st8		[r32]=r20,16
		xma.lu	f95=f37,f125,f95	};;
{ .mfi;		xma.hu	f106=f37,f126,f105	}
{ .mfi;	mov		r14 =0
		xma.lu	f105=f37,f126,f105
	add		r25=r25,r24		};;
{ .mfi;		xma.hu	f116=f37,f127,f115
	cmp.ltu		p6,p0=r25,r24		}
{ .mfi;		xma.lu	f115=f37,f127,f115
	add		r26=r26,r25		};;

{ .mfi;	getf.sig	r29=f45
		xma.hu	f47=f38,f120,f46
(p6)	add		r14 =1,r14 		}
{ .mfi;	cmp.ltu		p6,p0=r26,r25
		xma.lu	f46=f38,f120,f46
	add		r27=r27,r26		};;
{ .mfi;	getf.sig	r16=f100
		xma.hu	f57=f38,f121,f56
(p6)	add		r14 =1,r14 		}
{ .mfi;	cmp.ltu		p6,p0=r27,r26
		xma.lu	f56=f38,f121,f56
	add		r28=r28,r27		};;
{ .mfi;	getf.sig	r17=f91
		xma.hu	f67=f38,f122,f66
(p6)	add		r14 =1,r14 		}
{ .mfi;	cmp.ltu		p6,p0=r28,r27
		xma.lu	f66=f38,f122,f66
	add		r29=r29,r28		};;
{ .mfi;	getf.sig	r18=f82
		xma.hu	f77=f38,f123,f76
(p6)	add		r14 =1,r14 		}
{ .mfi;	cmp.ltu		p6,p0=r29,r28
		xma.lu	f76=f38,f123,f76
	add		r29=r29,r15 		};;
{ .mfi;	getf.sig	r19=f73
		xma.hu	f87=f38,f124,f86
(p6)	add		r14 =1,r14 		}
{ .mfi;		xma.lu	f86=f38,f124,f86
	cmp.ltu		p6,p0=r29,r15 	};;
{ .mfi;	getf.sig	r20=f64
		xma.hu	f97=f38,f125,f96
(p6)	add		r14 =1,r14 		}
{ .mfi;	st8		[r33]=r29,16
		xma.lu	f96=f38,f125,f96	};;
{ .mfi;	getf.sig	r21=f55
		xma.hu	f107=f38,f126,f106	}
{ .mfi;	mov		r15 =0
		xma.lu	f106=f38,f126,f106
	add		r17=r17,r16		};;
{ .mfi;		xma.hu	f117=f38,f127,f116
	cmp.ltu		p7,p0=r17,r16		}
{ .mfi;		xma.lu	f116=f38,f127,f116
	add		r18=r18,r17		};;

{ .mfi;	getf.sig	r22=f46
		xma.hu	f48=f39,f120,f47
(p7)	add		r15 =1,r15 		}
{ .mfi;	cmp.ltu		p7,p0=r18,r17
		xma.lu	f47=f39,f120,f47
	add		r19=r19,r18		};;
{ .mfi;	getf.sig	r24=f110
		xma.hu	f58=f39,f121,f57
(p7)	add		r15 =1,r15 		}
{ .mfi;	cmp.ltu		p7,p0=r19,r18
		xma.lu	f57=f39,f121,f57
	add		r20=r20,r19		};;
{ .mfi;	getf.sig	r25=f101
		xma.hu	f68=f39,f122,f67
(p7)	add		r15 =1,r15 		}
{ .mfi;	cmp.ltu		p7,p0=r20,r19
		xma.lu	f67=f39,f122,f67
	add		r21=r21,r20		};;
{ .mfi;	getf.sig	r26=f92
		xma.hu	f78=f39,f123,f77
(p7)	add		r15 =1,r15 		}
{ .mfi;	cmp.ltu		p7,p0=r21,r20
		xma.lu	f77=f39,f123,f77
	add		r22=r22,r21		};;
{ .mfi;	getf.sig	r27=f83
		xma.hu	f88=f39,f124,f87
(p7)	add		r15 =1,r15 		}
{ .mfi;	cmp.ltu		p7,p0=r22,r21
		xma.lu	f87=f39,f124,f87
	add		r22=r22,r14 		};;
{ .mfi;	getf.sig	r28=f74
		xma.hu	f98=f39,f125,f97
(p7)	add		r15 =1,r15 		}
{ .mfi;		xma.lu	f97=f39,f125,f97
	cmp.ltu		p7,p0=r22,r14 	};;
{ .mfi;	getf.sig	r29=f65
		xma.hu	f108=f39,f126,f107
(p7)	add		r15 =1,r15 		}
{ .mfi;	st8		[r32]=r22,16
		xma.lu	f107=f39,f126,f107	};;
{ .mfi;	getf.sig	r30=f56
		xma.hu	f118=f39,f127,f117	}
{ .mfi;		xma.lu	f117=f39,f127,f117	};;



{ .mii;	getf.sig	r31=f47
	add		r25=r25,r24
	mov		r14 =0		};;
{ .mii;		getf.sig	r16=f111
	cmp.ltu		p6,p0=r25,r24
	add		r26=r26,r25		};;
{ .mfb;		getf.sig	r17=f102	}
{ .mii;
(p6)	add		r14 =1,r14 
	cmp.ltu		p6,p0=r26,r25
	add		r27=r27,r26		};;
{ .mfb;	nop.m	0x0				}
{ .mii;
(p6)	add		r14 =1,r14 
	cmp.ltu		p6,p0=r27,r26
	add		r28=r28,r27		};;
{ .mii;		getf.sig	r18=f93
		add		r17=r17,r16
		mov		r34 =0	}
{ .mii;
(p6)	add		r14 =1,r14 
	cmp.ltu		p6,p0=r28,r27
	add		r29=r29,r28		};;
{ .mii;		getf.sig	r19=f84
		cmp.ltu		p7,p0=r17,r16	}
{ .mii;
(p6)	add		r14 =1,r14 
	cmp.ltu		p6,p0=r29,r28
	add		r30=r30,r29		};;
{ .mii;		getf.sig	r20=f75
		add		r18=r18,r17	}
{ .mii;
(p6)	add		r14 =1,r14 
	cmp.ltu		p6,p0=r30,r29
	add		r31=r31,r30		};;
{ .mfb;		getf.sig	r21=f66		}
{ .mii;	(p7)	add		r34 =1,r34 
		cmp.ltu		p7,p0=r18,r17
		add		r19=r19,r18	}
{ .mfb;	nop.m	0x0				}
{ .mii;
(p6)	add		r14 =1,r14 
	cmp.ltu		p6,p0=r31,r30
	add		r31=r31,r15 		};;
{ .mfb;		getf.sig	r22=f57		}
{ .mii;	(p7)	add		r34 =1,r34 
		cmp.ltu		p7,p0=r19,r18
		add		r20=r20,r19	}
{ .mfb;	nop.m	0x0				}
{ .mii;
(p6)	add		r14 =1,r14 
	cmp.ltu		p6,p0=r31,r15 	};;
{ .mfb;		getf.sig	r23=f48		}
{ .mii;	(p7)	add		r34 =1,r34 
		cmp.ltu		p7,p0=r20,r19
		add		r21=r21,r20	}
{ .mii;
(p6)	add		r14 =1,r14 		}
{ .mfb;	st8		[r33]=r31,16		};;

{ .mfb;	getf.sig	r24=f112		}
{ .mii;	(p7)	add		r34 =1,r34 
		cmp.ltu		p7,p0=r21,r20
		add		r22=r22,r21	};;
{ .mfb;	getf.sig	r25=f103		}
{ .mii;	(p7)	add		r34 =1,r34 
		cmp.ltu		p7,p0=r22,r21
		add		r23=r23,r22	};;
{ .mfb;	getf.sig	r26=f94			}
{ .mii;	(p7)	add		r34 =1,r34 
		cmp.ltu		p7,p0=r23,r22
		add		r23=r23,r14 	};;
{ .mfb;	getf.sig	r27=f85			}
{ .mii;	(p7)	add		r34 =1,r34 
		cmp.ltu		p7,p8=r23,r14 };;
{ .mii;	getf.sig	r28=f76
	add		r25=r25,r24
	mov		r14 =0		}
{ .mii;		st8		[r32]=r23,16
	(p7)	add		r15 =1,r34 
	(p8)	add		r15 =0,r34 	};;

{ .mfb;	nop.m	0x0				}
{ .mii;	getf.sig	r29=f67
	cmp.ltu		p6,p0=r25,r24
	add		r26=r26,r25		};;
{ .mfb;	getf.sig	r30=f58			}
{ .mii;
(p6)	add		r14 =1,r14 
	cmp.ltu		p6,p0=r26,r25
	add		r27=r27,r26		};;
{ .mfb;		getf.sig	r16=f113	}
{ .mii;
(p6)	add		r14 =1,r14 
	cmp.ltu		p6,p0=r27,r26
	add		r28=r28,r27		};;
{ .mfb;		getf.sig	r17=f104	}
{ .mii;
(p6)	add		r14 =1,r14 
	cmp.ltu		p6,p0=r28,r27
	add		r29=r29,r28		};;
{ .mfb;		getf.sig	r18=f95		}
{ .mii;
(p6)	add		r14 =1,r14 
	cmp.ltu		p6,p0=r29,r28
	add		r30=r30,r29		};;
{ .mii;		getf.sig	r19=f86
		add		r17=r17,r16
		mov		r34 =0	}
{ .mii;
(p6)	add		r14 =1,r14 
	cmp.ltu		p6,p0=r30,r29
	add		r30=r30,r15 		};;
{ .mii;		getf.sig	r20=f77
		cmp.ltu		p7,p0=r17,r16
		add		r18=r18,r17	}
{ .mii;
(p6)	add		r14 =1,r14 
	cmp.ltu		p6,p0=r30,r15 	};;
{ .mfb;		getf.sig	r21=f68		}
{ .mii;	st8		[r33]=r30,16
(p6)	add		r14 =1,r14 		};;

{ .mfb;	getf.sig	r24=f114		}
{ .mii;	(p7)	add		r34 =1,r34 
		cmp.ltu		p7,p0=r18,r17
		add		r19=r19,r18	};;
{ .mfb;	getf.sig	r25=f105		}
{ .mii;	(p7)	add		r34 =1,r34 
		cmp.ltu		p7,p0=r19,r18
		add		r20=r20,r19	};;
{ .mfb;	getf.sig	r26=f96			}
{ .mii;	(p7)	add		r34 =1,r34 
		cmp.ltu		p7,p0=r20,r19
		add		r21=r21,r20	};;
{ .mfb;	getf.sig	r27=f87			}
{ .mii;	(p7)	add		r34 =1,r34 
		cmp.ltu		p7,p0=r21,r20
		add		r21=r21,r14 	};;
{ .mib;	getf.sig	r28=f78
	add		r25=r25,r24		}
{ .mib;	(p7)	add		r34 =1,r34 
		cmp.ltu		p7,p8=r21,r14 };;
{ .mii;		st8		[r32]=r21,16
	(p7)	add		r15 =1,r34 
	(p8)	add		r15 =0,r34 	}

{ .mii;	mov		r14 =0
	cmp.ltu		p6,p0=r25,r24
	add		r26=r26,r25		};;
{ .mfb;		getf.sig	r16=f115	}
{ .mii;
(p6)	add		r14 =1,r14 
	cmp.ltu		p6,p0=r26,r25
	add		r27=r27,r26		};;
{ .mfb;		getf.sig	r17=f106	}
{ .mii;
(p6)	add		r14 =1,r14 
	cmp.ltu		p6,p0=r27,r26
	add		r28=r28,r27		};;
{ .mfb;		getf.sig	r18=f97		}
{ .mii;
(p6)	add		r14 =1,r14 
	cmp.ltu		p6,p0=r28,r27
	add		r28=r28,r15 		};;
{ .mib;		getf.sig	r19=f88
		add		r17=r17,r16	}
{ .mib;
(p6)	add		r14 =1,r14 
	cmp.ltu		p6,p0=r28,r15 	};;
{ .mii;	st8		[r33]=r28,16
(p6)	add		r14 =1,r14 		}

{ .mii;		mov		r15 =0
		cmp.ltu		p7,p0=r17,r16
		add		r18=r18,r17	};;
{ .mfb;	getf.sig	r24=f116		}
{ .mii;	(p7)	add		r15 =1,r15 
		cmp.ltu		p7,p0=r18,r17
		add		r19=r19,r18	};;
{ .mfb;	getf.sig	r25=f107		}
{ .mii;	(p7)	add		r15 =1,r15 
		cmp.ltu		p7,p0=r19,r18
		add		r19=r19,r14 	};;
{ .mfb;	getf.sig	r26=f98			}
{ .mii;	(p7)	add		r15 =1,r15 
		cmp.ltu		p7,p0=r19,r14 };;
{ .mii;		st8		[r32]=r19,16
	(p7)	add		r15 =1,r15 	}

{ .mfb;	add		r25=r25,r24		};;

{ .mfb;		getf.sig	r16=f117	}
{ .mii;	mov		r14 =0
	cmp.ltu		p6,p0=r25,r24
	add		r26=r26,r25		};;
{ .mfb;		getf.sig	r17=f108	}
{ .mii;
(p6)	add		r14 =1,r14 
	cmp.ltu		p6,p0=r26,r25
	add		r26=r26,r15 		};;
{ .mfb;	nop.m	0x0				}
{ .mii;
(p6)	add		r14 =1,r14 
	cmp.ltu		p6,p0=r26,r15 	};;
{ .mii;	st8		[r33]=r26,16
(p6)	add		r14 =1,r14 		}

{ .mfb;		add		r17=r17,r16	};;
{ .mfb;	getf.sig	r24=f118		}
{ .mii;		mov		r15 =0
		cmp.ltu		p7,p0=r17,r16
		add		r17=r17,r14 	};;
{ .mii;	(p7)	add		r15 =1,r15 
		cmp.ltu		p7,p0=r17,r14 };;
{ .mii;		st8		[r32]=r17
	(p7)	add		r15 =1,r15 	};;
{ .mfb;	add		r24=r24,r15 		};;
{ .mib;	st8		[r33]=r24		}

{ .mib;	rum		1<<5
	br.ret.sptk.many	b0	};;
.endp	bn_mul_comba8#
# 1170
.global	bn_sqr_comba4#
.proc	bn_sqr_comba4#
.align	64
bn_sqr_comba4:
	.prologue
	.save	ar.pfs,r2
# 1182
{ .mii;	alloc	r2=ar.pfs,2,1,0,0

	mov	r34=r33
	add	r14=8,r33		};;
	.body
{ .mii;	add	r17=8,r34
	add	r15=16,r33
	add	r18=16,r34		}
{ .mfb;	add	r16=24,r33
	br	.L_cheat_entry_point4	};;
.endp	bn_sqr_comba4#
# 1202
.global	bn_mul_comba4#
.proc	bn_mul_comba4#
.align	64
bn_mul_comba4:
	.prologue
	.save	ar.pfs,r2
# 1214
{ .mii;	alloc	r2=ar.pfs,3,0,0,0

	add	r14=8,r33
	add	r17=8,r34		}
	.body
{ .mii;	add	r15=16,r33
	add	r18=16,r34
	add	r16=24,r33		};;
.L_cheat_entry_point4:
{ .mmi;	add	r19=24,r34

	ldf8	f32=[r33]		}

{ .mmi;	ldf8	f120=[r34]
	ldf8	f121=[r17]		};;
{ .mmi;	ldf8	f122=[r18]
	ldf8	f123=[r19]		}

{ .mmi;	ldf8	f33=[r14]
	ldf8	f34=[r15]		}
{ .mfi;	ldf8	f35=[r16]

		xma.hu	f41=f32,f120,f0		}
{ .mfi;		xma.lu	f40=f32,f120,f0		};;
{ .mfi;		xma.hu	f51=f32,f121,f0		}
{ .mfi;		xma.lu	f50=f32,f121,f0		};;
{ .mfi;		xma.hu	f61=f32,f122,f0		}
{ .mfi;		xma.lu	f60=f32,f122,f0		};;
{ .mfi;		xma.hu	f71=f32,f123,f0		}
{ .mfi;		xma.lu	f70=f32,f123,f0		};;


{ .mfi;	getf.sig	r16=f40
		xma.hu	f42=f33,f120,f41
	add		r33=8,r32		}
{ .mfi;		xma.lu	f41=f33,f120,f41	};;
{ .mfi;	getf.sig	r24=f50
		xma.hu	f52=f33,f121,f51	}
{ .mfi;		xma.lu	f51=f33,f121,f51	};;
{ .mfi;	st8		[r32]=r16,16
		xma.hu	f62=f33,f122,f61	}
{ .mfi;		xma.lu	f61=f33,f122,f61	};;
{ .mfi;		xma.hu	f72=f33,f123,f71	}
{ .mfi;		xma.lu	f71=f33,f123,f71	};;

{ .mfi;	getf.sig	r25=f41
		xma.hu	f43=f34,f120,f42	}
{ .mfi;		xma.lu	f42=f34,f120,f42	};;
{ .mfi;	getf.sig	r16=f60
		xma.hu	f53=f34,f121,f52	}
{ .mfi;		xma.lu	f52=f34,f121,f52	};;
{ .mfi;	getf.sig	r17=f51
		xma.hu	f63=f34,f122,f62
	add		r25=r25,r24		}
{ .mfi;	mov		r14 =0
		xma.lu	f62=f34,f122,f62	};;
{ .mfi;	st8		[r33]=r25,16
		xma.hu	f73=f34,f123,f72
	cmp.ltu		p6,p0=r25,r24		}
{ .mfi;		xma.lu	f72=f34,f123,f72	};;

{ .mfi;	getf.sig	r18=f42
		xma.hu	f44=f35,f120,f43
(p6)	add		r14 =1,r14 		}
{ .mfi;	add		r17=r17,r16
		xma.lu	f43=f35,f120,f43
	mov		r15 =0		};;
{ .mfi;	getf.sig	r24=f70
		xma.hu	f54=f35,f121,f53
	cmp.ltu		p7,p0=r17,r16		}
{ .mfi;		xma.lu	f53=f35,f121,f53	};;
{ .mfi;	getf.sig	r25=f61
		xma.hu	f64=f35,f122,f63
	add		r18=r18,r17		}
{ .mfi;		xma.lu	f63=f35,f122,f63
(p7)	add		r15 =1,r15 		};;
{ .mfi;	getf.sig	r26=f52
		xma.hu	f74=f35,f123,f73
	cmp.ltu		p7,p0=r18,r17		}
{ .mfi;		xma.lu	f73=f35,f123,f73
	add		r18=r18,r14 		};;

{ .mii;	st8		[r32]=r18,16
(p7)	add		r15 =1,r15 
	cmp.ltu		p7,p0=r18,r14 	};;

{ .mfi;	getf.sig	r27=f43
(p7)	add		r15 =1,r15 		};;
{ .mii;		getf.sig	r16=f71
	add		r25=r25,r24
	mov		r14 =0		};;
{ .mii;		getf.sig	r17=f62
	cmp.ltu		p6,p0=r25,r24
	add		r26=r26,r25		};;
{ .mii;
(p6)	add		r14 =1,r14 
	cmp.ltu		p6,p0=r26,r25
	add		r27=r27,r26		};;
{ .mii;
(p6)	add		r14 =1,r14 
	cmp.ltu		p6,p0=r27,r26
	add		r27=r27,r15 		};;
{ .mii;		getf.sig	r18=f53
(p6)	add		r14 =1,r14 
	cmp.ltu		p6,p0=r27,r15 	};;
{ .mfi;	st8		[r33]=r27,16
(p6)	add		r14 =1,r14 		}

{ .mii;		getf.sig	r19=f44
		add		r17=r17,r16
		mov		r15 =0	};;
{ .mii;	getf.sig	r24=f72
		cmp.ltu		p7,p0=r17,r16
		add		r18=r18,r17	};;
{ .mii;	(p7)	add		r15 =1,r15 
		cmp.ltu		p7,p0=r18,r17
		add		r19=r19,r18	};;
{ .mii;	(p7)	add		r15 =1,r15 
		cmp.ltu		p7,p0=r19,r18
		add		r19=r19,r14 	};;
{ .mii;	getf.sig	r25=f63
	(p7)	add		r15 =1,r15 
		cmp.ltu		p7,p0=r19,r14 };;
{ .mii;		st8		[r32]=r19,16
	(p7)	add		r15 =1,r15 	}

{ .mii;	getf.sig	r26=f54
	add		r25=r25,r24
	mov		r14 =0		};;
{ .mii;		getf.sig	r16=f73
	cmp.ltu		p6,p0=r25,r24
	add		r26=r26,r25		};;
{ .mii;
(p6)	add		r14 =1,r14 
	cmp.ltu		p6,p0=r26,r25
	add		r26=r26,r15 		};;
{ .mii;		getf.sig	r17=f64
(p6)	add		r14 =1,r14 
	cmp.ltu		p6,p0=r26,r15 	};;
{ .mii;	st8		[r33]=r26,16
(p6)	add		r14 =1,r14 		}

{ .mii;	getf.sig	r24=f74
		add		r17=r17,r16
		mov		r15 =0	};;
{ .mii;		cmp.ltu		p7,p0=r17,r16
		add		r17=r17,r14 	};;

{ .mii;	(p7)	add		r15 =1,r15 
		cmp.ltu		p7,p0=r17,r14 };;
{ .mii;		st8		[r32]=r17,16
	(p7)	add		r15 =1,r15 	};;

{ .mii;	add		r24=r24,r15 		};;
{ .mii;	st8		[r33]=r24		}

{ .mib;	rum		1<<5
	br.ret.sptk.many	b0	};;
.endp	bn_mul_comba4#
# 1398
cont=p16
break=p0
equ=p24


.global	abort#
.global	bn_div_words#
.proc	bn_div_words#
.align	64
bn_div_words:
	.prologue
	.save	ar.pfs,r2
{ .mii;	alloc		r2=ar.pfs,3,5,0,8
	.save	b0,r3
	mov		r3=b0
	.save	pr,r10
	mov		r10=pr		};;
{ .mmb;	cmp.eq		p6,p0=r34,r0
	mov		r8=-1
(p6)	br.ret.spnt.many	b0	};;

	.body
{ .mii;	mov		r16 =r32
	mov		ar.ec=0
	mov		pr.rot=0	}
{ .mii;	mov		r17 =r33
	mov		r36=r0		};;

.L_divw_shift:
{ .mfi;	(p0)	cmp.lt		p16,p0=r0,r34
	(p0)	shladd		r33=r34,1,r0	}
{ .mfb;	(p0)	add		r35=1,r36
	(p0)	nop.f		0x0
(p16)	br.wtop.dpnt		.L_divw_shift	};;

{ .mii;	mov		r18 =r34
	shr.u		r22 =r34,32
	sub		r35=64,r36		};;
{ .mii;	setf.sig	f7=r22 
	shr.u		r14 =r16 ,r35
	mov		r21 =r36			};;
{ .mib;	cmp.ne		p6,p0=r0,r14 
	shl		r16 =r16 ,r36
(p6)	br.call.spnt.clr	b0=abort	};;

{ .mfi;	fcvt.xuf.s1	f7=f7
	shr.u		r14 =r17 ,r35		};;
{ .mii;	shl		r17 =r17 ,r36
	or		r16 =r16 ,r14 			};;

{ .mii;	nop.m		0x0
	cmp.leu		p6,p0=r18 ,r16 ;;
(p6)	sub		r16 =r16 ,r18 			}

{ .mlx;	setf.sig	f14=r18 
	movl		r14 =0xffffffff		};;

{ .mii;	setf.sig	f6=r16 
	shr.u		r20 =r16 ,32;;
	cmp.eq		p6,p7=r20 ,r22 		};;
{ .mfb;
(p6)	setf.sig	f8=r14 
(p7)	fcvt.xuf.s1	f6=f6
(p7)	br.call.sptk	b6=.L_udiv64_32_b6	};;

{ .mfi;	getf.sig	r33=f8
	xmpy.lu		f9=f8,f14		}
{ .mfi;	xmpy.hu		f10=f8,f14
	shrp		r16 =r16 ,r17 ,32		};;

{ .mmi;	getf.sig	r35=f9
	getf.sig	r31=f10			};;

.L_divw_1st_iter:
{ .mii;	(p0)	add		r32=-1,r33
	(p0)	cmp.eq		equ,cont=r20 ,r31		};;
{ .mii;	(p0)	cmp.ltu		p8,p0=r35,r18 
	(p0)	sub		r34=r35,r18 
	(equ)	cmp.leu		break,cont=r35,r16 	};;
{ .mib;	(cont)	cmp.leu		cont,break=r20 ,r31
	(p8)	add		r31=-1,r31
(cont)	br.wtop.spnt		.L_divw_1st_iter	};;

{ .mii;	sub		r16 =r16 ,r35
	shl		r8=r33,32
	shl		r17 =r17 ,32			};;

{ .mii;	setf.sig	f6=r16 
	shr.u		r20 =r16 ,32;;
	cmp.eq		p6,p7=r20 ,r22 		};;
{ .mfb;
(p6)	setf.sig	f8=r14 
(p7)	fcvt.xuf.s1	f6=f6
(p7)	br.call.sptk	b6=.L_udiv64_32_b6	};;

{ .mfi;	getf.sig	r33=f8
	xmpy.lu		f9=f8,f14		}
{ .mfi;	xmpy.hu		f10=f8,f14
	shrp		r16 =r16 ,r17 ,32		};;

{ .mmi;	getf.sig	r35=f9
	getf.sig	r31=f10			};;

.L_divw_2nd_iter:
{ .mii;	(p0)	add		r32=-1,r33
	(p0)	cmp.eq		equ,cont=r20 ,r31		};;
{ .mii;	(p0)	cmp.ltu		p8,p0=r35,r18 
	(p0)	sub		r34=r35,r18 
	(equ)	cmp.leu		break,cont=r35,r16 	};;
{ .mib;	(cont)	cmp.leu		cont,break=r20 ,r31
	(p8)	add		r31=-1,r31
(cont)	br.wtop.spnt		.L_divw_2nd_iter	};;

{ .mii;	sub	r16 =r16 ,r35
	or	r8=r8,r33
	mov	ar.pfs=r2		};;
{ .mii;	shr.u	r9=r16 ,r21 
	mov	pr=r10,0x1ffff		}
{ .mfb;	br.ret.sptk.many	b0	};;
# 1524
pred=p15
# 1537
.align	32
.skip	16
.L_udiv64_32_b6:
	frcpa.s1	f8,pred=f6,f7;;

(pred)	fnma.s1		f9=f7,f8,f1
(pred)	fmpy.s1		f10=f6,f8;;
(pred)	fmpy.s1		f11=f9,f9
(pred)	fma.s1		f10=f9,f10,f10;;
(pred)	fma.s1		f8=f9,f8,f8
(pred)	fma.s1		f9=f11,f10,f10;;
(pred)	fma.s1		f8=f11,f8,f8
(pred)	fnma.s1		f10=f7,f9,f6;;
(pred)	fma.s1		f8=f10,f8,f9;;

	fcvt.fxu.trunc.s1	f8=f8
	br.ret.sptk.many	b6;;
.endp	bn_div_words#

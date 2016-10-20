.ident  "../sha512-ia64.s, version 1.1"
.ident  "IA-64 ISA artwork by Andy Polyakov <appro@fy.chalmers.se>"
.explicit
.text

pfssave=r2;
lcsave=r3;
prsave=r14;
K=r15;
A=r16;	B=r17;	C=r18;	D=r19;
E=r20;	F=r21;	G=r22;	H=r23;
T1=r24;	T2=r25;
s0=r26;	s1=r27;	t0=r28;	t1=r29;
Ktbl=r30;
ctx=r31;	// 1st arg
input=r48;	// 2nd arg
num=r49;	// 3rd arg
sgm0=r50;	sgm1=r51;	// small constants
A_=r54;	B_=r55;	C_=r56;	D_=r57;
E_=r58;	F_=r59;	G_=r60;	H_=r61;

// void sha512_block_data_order (SHA_CTX *ctx, const void *in,size_t num[,int host])
.global	sha512_block_data_order#
.proc	sha512_block_data_order#
.align	32
sha512_block_data_order:
	.prologue
	.save	ar.pfs,pfssave
{ .mmi;	alloc	pfssave=ar.pfs,3,27,0,16
	add	ctx=0,r32		// 1st arg
	.save	ar.lc,lcsave
	mov	lcsave=ar.lc	}
{ .mmi;	add	input=0,r33		// 2nd arg
	mov	num=r34			// 3rd arg
	.save	pr,prsave
	mov	prsave=pr	};;

	.body
{ .mib;	add	r8=0*8,ctx
	add	r9=1*8,ctx
	brp.loop.imp	.L_first16,.L_first16_end-16	}
{ .mib;	add	r10=2*8,ctx
	add	r11=3*8,ctx
	brp.loop.imp	.L_rest,.L_rest_end-16		};;

// load A-H
.Lpic_point:
{ .mmi;	ld8	A_=[r8],4*8
	ld8	B_=[r9],4*8
	mov	Ktbl=ip		}
{ .mmi;	ld8	C_=[r10],4*8
	ld8	D_=[r11],4*8
	mov	sgm0=7	};;
{ .mmi;	ld8	E_=[r8]
	ld8	F_=[r9]
	add	Ktbl=(K512#-.Lpic_point),Ktbl		}
{ .mmi;	ld8	G_=[r10]
	ld8	H_=[r11]
	cmp.ne	p0,p16=0,r0	};;	// used in sha256_block
{ .mii;	and	r8=7,input
	and	input=~7,input;;
	cmp.eq	p9,p0=1,r8	}
{ .mmi;	cmp.eq	p10,p0=2,r8
	cmp.eq	p11,p0=3,r8
	cmp.eq	p12,p0=4,r8	}
{ .mmi;	cmp.eq	p13,p0=5,r8
	cmp.eq	p14,p0=6,r8
	cmp.eq	p15,p0=7,r8	};;
.L_outer:
.rotr	X[16]
{ .mmi;	mov	A=A_
	mov	B=B_
	mov	ar.lc=14	}
{ .mmi;	mov	C=C_
	mov	D=D_
	mov	E=E_		}
{ .mmi;	mov	F=F_
	mov	G=G_
	mov	ar.ec=2		}
{ .mmi;	nop.m	 0x0		// eliminated in 64-bit
	mov	H=H_
	mov	sgm1=6	};;

// in 64-bit mode I load whole X[16] at once and take care of alignment...
{ .mmi;	add	r8=1*8,input
	add	r9=2*8,input
	add	r10=3*8,input		};;
{ .mmb;	ld8	X[15]=[input],4*8
	ld8	X[14]=[r8],4*8
(p9)	br.cond.dpnt.many	.L1byte	};;
{ .mmb;	ld8	X[13]=[r9],4*8
	ld8	X[12]=[r10],4*8
(p10)	br.cond.dpnt.many	.L2byte	};;
{ .mmb;	ld8	X[11]=[input],4*8
	ld8	X[10]=[r8],4*8
(p11)	br.cond.dpnt.many	.L3byte	};;
{ .mmb;	ld8	X[ 9]=[r9],4*8
	ld8	X[ 8]=[r10],4*8
(p12)	br.cond.dpnt.many	.L4byte	};;
{ .mmb;	ld8	X[ 7]=[input],4*8
	ld8	X[ 6]=[r8],4*8
(p13)	br.cond.dpnt.many	.L5byte	};;
{ .mmb;	ld8	X[ 5]=[r9],4*8
	ld8	X[ 4]=[r10],4*8
(p14)	br.cond.dpnt.many	.L6byte	};;
{ .mmb;	ld8	X[ 3]=[input],4*8
	ld8	X[ 2]=[r8],4*8
(p15)	br.cond.dpnt.many	.L7byte	};;
{ .mmb;	ld8	X[ 1]=[r9],4*8
	ld8	X[ 0]=[r10],4*8
	br.many	.L_first16		};;
.L1byte:
{ .mmi;	ld8	X[13]=[r9],4*8
	ld8	X[12]=[r10],4*8
	shrp	X[15]=X[15],X[14],56	};;
{ .mmi;	ld8	X[11]=[input],4*8
	ld8	X[10]=[r8],4*8
	shrp	X[14]=X[14],X[13],56	}
{ .mmi;	ld8	X[ 9]=[r9],4*8
	ld8	X[ 8]=[r10],4*8
	shrp	X[13]=X[13],X[12],56	};;
{ .mmi;	ld8	X[ 7]=[input],4*8
	ld8	X[ 6]=[r8],4*8
	shrp	X[12]=X[12],X[11],56	}
{ .mmi;	ld8	X[ 5]=[r9],4*8
	ld8	X[ 4]=[r10],4*8
	shrp	X[11]=X[11],X[10],56	};;
{ .mmi;	ld8	X[ 3]=[input],4*8
	ld8	X[ 2]=[r8],4*8
	shrp	X[10]=X[10],X[ 9],56	}
{ .mmi;	ld8	X[ 1]=[r9],4*8
	ld8	X[ 0]=[r10],4*8
	shrp	X[ 9]=X[ 9],X[ 8],56	};;
{ .mii;	ld8	T1=[input]
	shrp	X[ 8]=X[ 8],X[ 7],56
	shrp	X[ 7]=X[ 7],X[ 6],56	}
{ .mii;	shrp	X[ 6]=X[ 6],X[ 5],56
	shrp	X[ 5]=X[ 5],X[ 4],56	};;
{ .mii;	shrp	X[ 4]=X[ 4],X[ 3],56
	shrp	X[ 3]=X[ 3],X[ 2],56	}
{ .mii;	shrp	X[ 2]=X[ 2],X[ 1],56
	shrp	X[ 1]=X[ 1],X[ 0],56	}
{ .mib;	shrp	X[ 0]=X[ 0],T1,56
	br.many	.L_first16		};;
.L2byte:
{ .mmi;	ld8	X[11]=[input],4*8
	ld8	X[10]=[r8],4*8
	shrp	X[15]=X[15],X[14],48	}
{ .mmi;	ld8	X[ 9]=[r9],4*8
	ld8	X[ 8]=[r10],4*8
	shrp	X[14]=X[14],X[13],48	};;
{ .mmi;	ld8	X[ 7]=[input],4*8
	ld8	X[ 6]=[r8],4*8
	shrp	X[13]=X[13],X[12],48	}
{ .mmi;	ld8	X[ 5]=[r9],4*8
	ld8	X[ 4]=[r10],4*8
	shrp	X[12]=X[12],X[11],48	};;
{ .mmi;	ld8	X[ 3]=[input],4*8
	ld8	X[ 2]=[r8],4*8
	shrp	X[11]=X[11],X[10],48	}
{ .mmi;	ld8	X[ 1]=[r9],4*8
	ld8	X[ 0]=[r10],4*8
	shrp	X[10]=X[10],X[ 9],48	};;
{ .mii;	ld8	T1=[input]
	shrp	X[ 9]=X[ 9],X[ 8],48
	shrp	X[ 8]=X[ 8],X[ 7],48	}
{ .mii;	shrp	X[ 7]=X[ 7],X[ 6],48
	shrp	X[ 6]=X[ 6],X[ 5],48	};;
{ .mii;	shrp	X[ 5]=X[ 5],X[ 4],48
	shrp	X[ 4]=X[ 4],X[ 3],48	}
{ .mii;	shrp	X[ 3]=X[ 3],X[ 2],48
	shrp	X[ 2]=X[ 2],X[ 1],48	}
{ .mii;	shrp	X[ 1]=X[ 1],X[ 0],48
	shrp	X[ 0]=X[ 0],T1,48	}
{ .mfb;	br.many	.L_first16		};;
.L3byte:
{ .mmi;	ld8	X[ 9]=[r9],4*8
	ld8	X[ 8]=[r10],4*8
	shrp	X[15]=X[15],X[14],40	};;
{ .mmi;	ld8	X[ 7]=[input],4*8
	ld8	X[ 6]=[r8],4*8
	shrp	X[14]=X[14],X[13],40	}
{ .mmi;	ld8	X[ 5]=[r9],4*8
	ld8	X[ 4]=[r10],4*8
	shrp	X[13]=X[13],X[12],40	};;
{ .mmi;	ld8	X[ 3]=[input],4*8
	ld8	X[ 2]=[r8],4*8
	shrp	X[12]=X[12],X[11],40	}
{ .mmi;	ld8	X[ 1]=[r9],4*8
	ld8	X[ 0]=[r10],4*8
	shrp	X[11]=X[11],X[10],40	};;
{ .mii;	ld8	T1=[input]
	shrp	X[10]=X[10],X[ 9],40
	shrp	X[ 9]=X[ 9],X[ 8],40	}
{ .mii;	shrp	X[ 8]=X[ 8],X[ 7],40
	shrp	X[ 7]=X[ 7],X[ 6],40	};;
{ .mii;	shrp	X[ 6]=X[ 6],X[ 5],40
	shrp	X[ 5]=X[ 5],X[ 4],40	}
{ .mii;	shrp	X[ 4]=X[ 4],X[ 3],40
	shrp	X[ 3]=X[ 3],X[ 2],40	}
{ .mii;	shrp	X[ 2]=X[ 2],X[ 1],40
	shrp	X[ 1]=X[ 1],X[ 0],40	}
{ .mib;	shrp	X[ 0]=X[ 0],T1,40
	br.many	.L_first16		};;
.L4byte:
{ .mmi;	ld8	X[ 7]=[input],4*8
	ld8	X[ 6]=[r8],4*8
	shrp	X[15]=X[15],X[14],32	}
{ .mmi;	ld8	X[ 5]=[r9],4*8
	ld8	X[ 4]=[r10],4*8
	shrp	X[14]=X[14],X[13],32	};;
{ .mmi;	ld8	X[ 3]=[input],4*8
	ld8	X[ 2]=[r8],4*8
	shrp	X[13]=X[13],X[12],32	}
{ .mmi;	ld8	X[ 1]=[r9],4*8
	ld8	X[ 0]=[r10],4*8
	shrp	X[12]=X[12],X[11],32	};;
{ .mii;	ld8	T1=[input]
	shrp	X[11]=X[11],X[10],32
	shrp	X[10]=X[10],X[ 9],32	}
{ .mii;	shrp	X[ 9]=X[ 9],X[ 8],32
	shrp	X[ 8]=X[ 8],X[ 7],32	};;
{ .mii;	shrp	X[ 7]=X[ 7],X[ 6],32
	shrp	X[ 6]=X[ 6],X[ 5],32	}
{ .mii;	shrp	X[ 5]=X[ 5],X[ 4],32
	shrp	X[ 4]=X[ 4],X[ 3],32	}
{ .mii;	shrp	X[ 3]=X[ 3],X[ 2],32
	shrp	X[ 2]=X[ 2],X[ 1],32	}
{ .mii;	shrp	X[ 1]=X[ 1],X[ 0],32
	shrp	X[ 0]=X[ 0],T1,32	}
{ .mfb;	br.many	.L_first16		};;
.L5byte:
{ .mmi;	ld8	X[ 5]=[r9],4*8
	ld8	X[ 4]=[r10],4*8
	shrp	X[15]=X[15],X[14],24	};;
{ .mmi;	ld8	X[ 3]=[input],4*8
	ld8	X[ 2]=[r8],4*8
	shrp	X[14]=X[14],X[13],24	}
{ .mmi;	ld8	X[ 1]=[r9],4*8
	ld8	X[ 0]=[r10],4*8
	shrp	X[13]=X[13],X[12],24	};;
{ .mii;	ld8	T1=[input]
	shrp	X[12]=X[12],X[11],24
	shrp	X[11]=X[11],X[10],24	}
{ .mii;	shrp	X[10]=X[10],X[ 9],24
	shrp	X[ 9]=X[ 9],X[ 8],24	};;
{ .mii;	shrp	X[ 8]=X[ 8],X[ 7],24
	shrp	X[ 7]=X[ 7],X[ 6],24	}
{ .mii;	shrp	X[ 6]=X[ 6],X[ 5],24
	shrp	X[ 5]=X[ 5],X[ 4],24	}
{ .mii;	shrp	X[ 4]=X[ 4],X[ 3],24
	shrp	X[ 3]=X[ 3],X[ 2],24	}
{ .mii;	shrp	X[ 2]=X[ 2],X[ 1],24
	shrp	X[ 1]=X[ 1],X[ 0],24	}
{ .mib;	shrp	X[ 0]=X[ 0],T1,24
	br.many	.L_first16		};;
.L6byte:
{ .mmi;	ld8	X[ 3]=[input],4*8
	ld8	X[ 2]=[r8],4*8
	shrp	X[15]=X[15],X[14],16	}
{ .mmi;	ld8	X[ 1]=[r9],4*8
	ld8	X[ 0]=[r10],4*8
	shrp	X[14]=X[14],X[13],16	};;
{ .mii;	ld8	T1=[input]
	shrp	X[13]=X[13],X[12],16
	shrp	X[12]=X[12],X[11],16	}
{ .mii;	shrp	X[11]=X[11],X[10],16
	shrp	X[10]=X[10],X[ 9],16	};;
{ .mii;	shrp	X[ 9]=X[ 9],X[ 8],16
	shrp	X[ 8]=X[ 8],X[ 7],16	}
{ .mii;	shrp	X[ 7]=X[ 7],X[ 6],16
	shrp	X[ 6]=X[ 6],X[ 5],16	}
{ .mii;	shrp	X[ 5]=X[ 5],X[ 4],16
	shrp	X[ 4]=X[ 4],X[ 3],16	}
{ .mii;	shrp	X[ 3]=X[ 3],X[ 2],16
	shrp	X[ 2]=X[ 2],X[ 1],16	}
{ .mii;	shrp	X[ 1]=X[ 1],X[ 0],16
	shrp	X[ 0]=X[ 0],T1,16	}
{ .mfb;	br.many	.L_first16		};;
.L7byte:
{ .mmi;	ld8	X[ 1]=[r9],4*8
	ld8	X[ 0]=[r10],4*8
	shrp	X[15]=X[15],X[14],8	};;
{ .mii;	ld8	T1=[input]
	shrp	X[14]=X[14],X[13],8
	shrp	X[13]=X[13],X[12],8	}
{ .mii;	shrp	X[12]=X[12],X[11],8
	shrp	X[11]=X[11],X[10],8	};;
{ .mii;	shrp	X[10]=X[10],X[ 9],8
	shrp	X[ 9]=X[ 9],X[ 8],8	}
{ .mii;	shrp	X[ 8]=X[ 8],X[ 7],8
	shrp	X[ 7]=X[ 7],X[ 6],8	}
{ .mii;	shrp	X[ 6]=X[ 6],X[ 5],8
	shrp	X[ 5]=X[ 5],X[ 4],8	}
{ .mii;	shrp	X[ 4]=X[ 4],X[ 3],8
	shrp	X[ 3]=X[ 3],X[ 2],8	}
{ .mii;	shrp	X[ 2]=X[ 2],X[ 1],8
	shrp	X[ 1]=X[ 1],X[ 0],8	}
{ .mib;	shrp	X[ 0]=X[ 0],T1,8
	br.many	.L_first16		};;

.align	32
.L_first16:
{ .mmi;		ld8	K=[Ktbl],8
		and	T1=F,E
		and	T2=A,B		}
{ .mmi;		//ld8	X[15]=[input],8	// X[i]=*input++
		andcm	r8=G,E
		and	r9=A,C		};;
{ .mmi;		xor	T1=T1,r8		//T1=((e & f) ^ (~e & g))
		and	r10=B,C
		shrp	r11=E,E,14 }	// ROTR(e,14)
{ .mmi;		xor	T2=T2,r9
		nop.i	 0x0 };;	// eliminated in big-endian
{ .mib;		add	T1=T1,H			// T1=Ch(e,f,g)+h
		shrp	r8=E,E,18 }	// ROTR(e,18)
{ .mib;		xor	T2=T2,r10		// T2=((a & b) ^ (a & c) ^ (b & c))
		mov	H=G		};;
{ .mib;		xor	r11=r8,r11
		shrp	r9=E,E,41 }	// ROTR(e,41)
{ .mib;		mov	G=F
		mov	F=E		};;
{ .mib;		xor	r9=r9,r11		// r9=Sigma1(e)
		shrp	r10=A,A,28 }	// ROTR(a,28)
{ .mib;		add	T1=T1,K			// T1=Ch(e,f,g)+h+K512[i]
		mov	E=D		};;
{ .mib;		add	T1=T1,r9		// T1+=Sigma1(e)
		shrp	r11=A,A,34 }	// ROTR(a,34)
{ .mib;		mov	D=C
		mov	C=B		};;
{ .mib;		add	T1=T1,X[15]		// T1+=X[i]
		shrp	r8=A,A,39 }	// ROTR(a,39)
{ .mib;		xor	r10=r10,r11
		nop.i	 0x0 };;	// eliminated in 64-bit
{ .mmi;		xor	r10=r8,r10		// r10=Sigma0(a)
		mov	B=A
		add	A=T1,T2		};;
{ .mib;		add	E=E,T1
		add	A=A,r10			// T2=Maj(a,b,c)+Sigma0(a)
	br.ctop.sptk	.L_first16	};;
.L_first16_end:

{ .mii;	mov	ar.lc=80-17
	mov	ar.ec=1			};;

.align	32
.L_rest:
.rotr	X[16]
{ .mib;		ld8	K=[Ktbl],8
		shrp	r8=X[15-1],X[15-1],1 }	// ROTR(s0,1)
{ .mib; 	add	X[15]=X[15],X[15-9]	// X[i&0xF]+=X[(i+9)&0xF]
		shr.u	s0=X[15-1],sgm0	};;	// s0=X[(i+1)&0xF]>>7
{ .mib;		and	T1=F,E
		shrp	r9=X[15-1],X[15-1],8 }	// ROTR(s0,8)
{ .mib;		andcm	r10=G,E
		shr.u	s1=X[15-14],sgm1 };;	// s1=X[(i+14)&0xF]>>6
{ .mmi;		xor	T1=T1,r10		// T1=((e & f) ^ (~e & g))
		xor	r9=r8,r9
		shrp	r10=X[15-14],X[15-14],19 };;// ROTR(s1,19)
{ .mib;		and	T2=A,B		
		shrp	r11=X[15-14],X[15-14],61 }// ROTR(s1,61)
{ .mib;		and	r8=A,C		};;
{ .mib;		xor	s0=s0,r9		// s0=sigma0(X[(i+1)&0xF])
		shrp	r9=E,E,14 }	// ROTR(e,14)
{ .mib;		xor	r10=r11,r10
		xor	T2=T2,r8	};;
{ .mib;		xor	s1=s1,r10		// s1=sigma1(X[(i+14)&0xF])
		add	T1=T1,H		}
{ .mib;		and	r10=B,C
		add	X[15]=X[15],s0	};;	// X[i&0xF]+=sigma0(X[(i+1)&0xF])
{ .mmi;		xor	T2=T2,r10		// T2=((a & b) ^ (a & c) ^ (b & c))
		mov	H=G
		shrp	r8=E,E,18 };;	// ROTR(e,18)
{ .mmi;		xor	r11=r8,r9
		add	X[15]=X[15],s1		// X[i&0xF]+=sigma1(X[(i+14)&0xF])
		shrp	r9=E,E,41 }	// ROTR(e,41)
{ .mmi;		mov	G=F
		mov	F=E		};;
{ .mib;		xor	r9=r9,r11		// r9=Sigma1(e)
		shrp	r10=A,A,28 }	// ROTR(a,28)
{ .mib;		add	T1=T1,K			// T1=Ch(e,f,g)+h+K512[i]
		mov	E=D		};;
{ .mib;		add	T1=T1,r9		// T1+=Sigma1(e)
		shrp	r11=A,A,34 }	// ROTR(a,34)
{ .mib;		mov	D=C
		mov	C=B		};;
{ .mmi;		add	T1=T1,X[15]		// T1+=X[i]
		xor	r10=r10,r11
		shrp	r8=A,A,39 };;	// ROTR(a,39)
{ .mmi;		xor	r10=r8,r10		// r10=Sigma0(a)
		mov	B=A
		add	A=T1,T2		};;
{ .mib;		add	E=E,T1
		add	A=A,r10			// T2=Maj(a,b,c)+Sigma0(a)
	br.ctop.sptk	.L_rest	};;
.L_rest_end:

{ .mmi;	add	A_=A_,A
	add	B_=B_,B
	add	C_=C_,C			}
{ .mmi;	add	D_=D_,D
	add	E_=E_,E
	cmp.ltu	p16,p0=1,num		};;
{ .mmi;	add	F_=F_,F
	add	G_=G_,G
	add	H_=H_,H			}
{ .mmb;	add	Ktbl=-8*80,Ktbl
(p16)	add	num=-1,num
(p16)	br.dptk.many	.L_outer	};;

{ .mib;	add	r8=0*8,ctx
	add	r9=1*8,ctx		}
{ .mib;	add	r10=2*8,ctx
	add	r11=3*8,ctx		};;
{ .mmi;	st8	[r8]=A_,4*8
	st8	[r9]=B_,4*8
	mov	ar.lc=lcsave		}
{ .mmi;	st8	[r10]=C_,4*8
	st8	[r11]=D_,4*8
	mov	pr=prsave,0x1ffff	};;
{ .mmb;	st8	[r8]=E_
	st8	[r9]=F_			}
{ .mmb;	st8	[r10]=G_
	st8	[r11]=H_
	br.ret.sptk.many	b0	};;
.endp	sha512_block_data_order#
.align	64
.type	K512#,@object
K512:	data8	0x428a2f98d728ae22,0x7137449123ef65cd
	data8	0xb5c0fbcfec4d3b2f,0xe9b5dba58189dbbc
	data8	0x3956c25bf348b538,0x59f111f1b605d019
	data8	0x923f82a4af194f9b,0xab1c5ed5da6d8118
	data8	0xd807aa98a3030242,0x12835b0145706fbe
	data8	0x243185be4ee4b28c,0x550c7dc3d5ffb4e2
	data8	0x72be5d74f27b896f,0x80deb1fe3b1696b1
	data8	0x9bdc06a725c71235,0xc19bf174cf692694
	data8	0xe49b69c19ef14ad2,0xefbe4786384f25e3
	data8	0x0fc19dc68b8cd5b5,0x240ca1cc77ac9c65
	data8	0x2de92c6f592b0275,0x4a7484aa6ea6e483
	data8	0x5cb0a9dcbd41fbd4,0x76f988da831153b5
	data8	0x983e5152ee66dfab,0xa831c66d2db43210
	data8	0xb00327c898fb213f,0xbf597fc7beef0ee4
	data8	0xc6e00bf33da88fc2,0xd5a79147930aa725
	data8	0x06ca6351e003826f,0x142929670a0e6e70
	data8	0x27b70a8546d22ffc,0x2e1b21385c26c926
	data8	0x4d2c6dfc5ac42aed,0x53380d139d95b3df
	data8	0x650a73548baf63de,0x766a0abb3c77b2a8
	data8	0x81c2c92e47edaee6,0x92722c851482353b
	data8	0xa2bfe8a14cf10364,0xa81a664bbc423001
	data8	0xc24b8b70d0f89791,0xc76c51a30654be30
	data8	0xd192e819d6ef5218,0xd69906245565a910
	data8	0xf40e35855771202a,0x106aa07032bbd1b8
	data8	0x19a4c116b8d2d0c8,0x1e376c085141ab53
	data8	0x2748774cdf8eeb99,0x34b0bcb5e19b48a8
	data8	0x391c0cb3c5c95a63,0x4ed8aa4ae3418acb
	data8	0x5b9cca4f7763e373,0x682e6ff3d6b2b8a3
	data8	0x748f82ee5defb2fc,0x78a5636f43172f60
	data8	0x84c87814a1f0ab72,0x8cc702081a6439ec
	data8	0x90befffa23631e28,0xa4506cebde82bde9
	data8	0xbef9a3f7b2c67915,0xc67178f2e372532b
	data8	0xca273eceea26619c,0xd186b8c721c0c207
	data8	0xeada7dd6cde0eb1e,0xf57d4f7fee6ed178
	data8	0x06f067aa72176fba,0x0a637dc5a2c898a6
	data8	0x113f9804bef90dae,0x1b710b35131c471b
	data8	0x28db77f523047d84,0x32caab7b40c72493
	data8	0x3c9ebe0a15c9bebc,0x431d67c49c100d4c
	data8	0x4cc5d4becb3e42b6,0x597f299cfc657e2a
	data8	0x5fcb6fab3ad6faec,0x6c44198c4a475817
.size	K512#,8*80
stringz	"SHA512 block transform for IA64, CRYPTOGAMS by <appro@openssl.org>"

.ident  "../sha256-ia64.s, version 1.1"
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

// void sha256_block_data_order (SHA_CTX *ctx, const void *in,size_t num[,int host])
.global	sha256_block_data_order#
.proc	sha256_block_data_order#
.align	32
sha256_block_data_order:
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
{ .mib;	add	r8=0*4,ctx
	add	r9=1*4,ctx
	brp.loop.imp	.L_first16,.L_first16_end-16	}
{ .mib;	add	r10=2*4,ctx
	add	r11=3*4,ctx
	brp.loop.imp	.L_rest,.L_rest_end-16		};;

// load A-H
.Lpic_point:
{ .mmi;	ld4	A_=[r8],4*4
	ld4	B_=[r9],4*4
	mov	Ktbl=ip		}
{ .mmi;	ld4	C_=[r10],4*4
	ld4	D_=[r11],4*4
	mov	sgm0=3	};;
{ .mmi;	ld4	E_=[r8]
	ld4	F_=[r9]
	add	Ktbl=(K256#-.Lpic_point),Ktbl		}
{ .mmi;	ld4	G_=[r10]
	ld4	H_=[r11]
	cmp.ne	p0,p16=0,r0	};;	// used in sha256_block
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
{ .mmi;	ld1	X[15]=[input],4		// eliminated in 64-bit
	mov	H=H_
	mov	sgm1=10	};;

.align	32
.L_first16:
{ .mmi;		add	r9=1-4,input
		add	r10=2-4,input
		add	r11=3-4,input	};;
{ .mmi;		ld1	r9=[r9]
		ld1	r10=[r10]
		dep.z	t1=E,32,32	}
{ .mmi;		ld4	K=[Ktbl],4
		ld1	r11=[r11]
		zxt4	E=E		};;
{ .mii;		or	t1=t1,E
		dep	X[15]=X[15],r9,8,8
		dep	r11=r10,r11,8,8	};;
{ .mmi;		and	T1=F,E
		and	T2=A,B
		dep	X[15]=X[15],r11,16,16	}
{ .mmi;		andcm	r8=G,E
		and	r9=A,C
		mux2	t0=A,0x44	};;	// copy lower half to upper
{ .mmi;	(p16)	ld1	X[15-1]=[input],4	// prefetch
		xor	T1=T1,r8		// T1=((e & f) ^ (~e & g))
		shrp	r11=t1,t1,6 }	// ROTR(e,14)
{ .mib;		and	r10=B,C
		xor	T2=T2,r9	};;
{ .mib;		add	T1=T1,H			// T1=Ch(e,f,g)+h
		shrp	r8=t1,t1,11 }	// ROTR(e,18)
{ .mib;		xor	T2=T2,r10		// T2=((a & b) ^ (a & c) ^ (b & c))
		mov	H=G		};;
{ .mib;		xor	r11=r8,r11
		shrp	r9=t1,t1,25 }	// ROTR(e,41)
{ .mib;		mov	G=F
		mov	F=E		};;
{ .mib;		xor	r9=r9,r11		// r9=Sigma1(e)
		shrp	r10=t0,t0,2 }	// ROTR(a,28)
{ .mib;		add	T1=T1,K			// T1=Ch(e,f,g)+h+K512[i]
		mov	E=D		};;
{ .mib;		add	T1=T1,r9		// T1+=Sigma1(e)
		shrp	r11=t0,t0,13 }	// ROTR(a,34)
{ .mib;		mov	D=C
		mov	C=B		};;
{ .mib;		add	T1=T1,X[15]		// T1+=X[i]
		shrp	r8=t0,t0,22 }	// ROTR(a,39)
{ .mib;		xor	r10=r10,r11
		mux2	X[15]=X[15],0x44 };;	// eliminated in 64-bit
{ .mmi;		xor	r10=r8,r10		// r10=Sigma0(a)
		mov	B=A
		add	A=T1,T2		};;
{ .mib;		add	E=E,T1
		add	A=A,r10			// T2=Maj(a,b,c)+Sigma0(a)
	br.ctop.sptk	.L_first16	};;
.L_first16_end:

{ .mii;	mov	ar.lc=64-17
	mov	ar.ec=1			};;

.align	32
.L_rest:
.rotr	X[16]
{ .mib;		ld4	K=[Ktbl],4
		shrp	r8=X[15-1],X[15-1],7 }	// ROTR(s0,1)
{ .mib; 	padd4	X[15]=X[15],X[15-9]	// X[i&0xF]+=X[(i+9)&0xF]
		pshr4.u	s0=X[15-1],sgm0	};;	// s0=X[(i+1)&0xF]>>7
{ .mib;		and	T1=F,E
		shrp	r9=X[15-1],X[15-1],18 }	// ROTR(s0,8)
{ .mib;		andcm	r10=G,E
		pshr4.u	s1=X[15-14],sgm1 };;	// s1=X[(i+14)&0xF]>>6
{ .mmi;		xor	T1=T1,r10		// T1=((e & f) ^ (~e & g))
		xor	r9=r8,r9
		shrp	r10=X[15-14],X[15-14],17 };;// ROTR(s1,19)
{ .mib;		and	T2=A,B		
		shrp	r11=X[15-14],X[15-14],19 }// ROTR(s1,61)
{ .mib;		and	r8=A,C		};;
// I adhere to mmi; in order to hold Itanium 1 back and avoid 6 cycle
// pipeline flush in last bundle. Note that even on Itanium2 the
// latter stalls for one clock cycle...
{ .mmi;		xor	s0=s0,r9		// s0=sigma0(X[(i+1)&0xF])
		dep.z	t1=E,32,32	}
{ .mmi;		xor	r10=r11,r10
		zxt4	E=E		};;
{ .mmi;		or	t1=t1,E
		xor	s1=s1,r10		// s1=sigma1(X[(i+14)&0xF])
		mux2	t0=A,0x44	};;	// copy lower half to upper
{ .mmi;		xor	T2=T2,r8
		shrp	r9=t1,t1,6 }	// ROTR(e,14)
{ .mmi;		and	r10=B,C
		add	T1=T1,H			// T1=Ch(e,f,g)+h
		padd4	X[15]=X[15],s0	};;	// X[i&0xF]+=sigma0(X[(i+1)&0xF])
{ .mmi;		xor	T2=T2,r10		// T2=((a & b) ^ (a & c) ^ (b & c))
		mov	H=G
		shrp	r8=t1,t1,11 };;	// ROTR(e,18)
{ .mmi;		xor	r11=r8,r9
		padd4	X[15]=X[15],s1		// X[i&0xF]+=sigma1(X[(i+14)&0xF])
		shrp	r9=t1,t1,25 }	// ROTR(e,41)
{ .mmi;		mov	G=F
		mov	F=E		};;
{ .mib;		xor	r9=r9,r11		// r9=Sigma1(e)
		shrp	r10=t0,t0,2 }	// ROTR(a,28)
{ .mib;		add	T1=T1,K			// T1=Ch(e,f,g)+h+K512[i]
		mov	E=D		};;
{ .mib;		add	T1=T1,r9		// T1+=Sigma1(e)
		shrp	r11=t0,t0,13 }	// ROTR(a,34)
{ .mib;		mov	D=C
		mov	C=B		};;
{ .mmi;		add	T1=T1,X[15]		// T1+=X[i]
		xor	r10=r10,r11
		shrp	r8=t0,t0,22 };;	// ROTR(a,39)
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
{ .mmb;	add	Ktbl=-4*64,Ktbl
(p16)	add	num=-1,num
(p16)	br.dptk.many	.L_outer	};;

{ .mib;	add	r8=0*4,ctx
	add	r9=1*4,ctx		}
{ .mib;	add	r10=2*4,ctx
	add	r11=3*4,ctx		};;
{ .mmi;	st4	[r8]=A_,4*4
	st4	[r9]=B_,4*4
	mov	ar.lc=lcsave		}
{ .mmi;	st4	[r10]=C_,4*4
	st4	[r11]=D_,4*4
	mov	pr=prsave,0x1ffff	};;
{ .mmb;	st4	[r8]=E_
	st4	[r9]=F_			}
{ .mmb;	st4	[r10]=G_
	st4	[r11]=H_
	br.ret.sptk.many	b0	};;
.endp	sha256_block_data_order#
.align	64
.type	K256#,@object
K256:	data4	0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5
	data4	0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5
	data4	0xd807aa98,0x12835b01,0x243185be,0x550c7dc3
	data4	0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174
	data4	0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc
	data4	0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da
	data4	0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7
	data4	0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967
	data4	0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13
	data4	0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85
	data4	0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3
	data4	0xd192e819,0xd6990624,0xf40e3585,0x106aa070
	data4	0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5
	data4	0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3
	data4	0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208
	data4	0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
.size	K256#,4*64
stringz	"SHA256 block transform for IA64, CRYPTOGAMS by <appro@openssl.org>"

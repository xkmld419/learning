# 1 "asm/md5-ia64.S"

# 240
	.text
# 262
	.type	md5_block_asm_data_order, @function
	.global	md5_block_asm_data_order
	.align	32
	.proc	md5_block_asm_data_order
md5_block_asm_data_order:
.md5_block:
	.prologue
{	.mmi
	.save	ar.pfs, r20 
	alloc	r20  = ar.pfs, 3 , 0 , 29 , 0 
	add 	r23  = 8, in0 
	mov	r24  = ip
}
{	.mmi
	add 	in1  = 0, in1 
	add 	in0  = 0, in0 
	.save	ar.lc, r21 
	mov	r21  = ar.lc
}
;;
{	.mmi
	add	r24  = .md5_tbl_data_order#-.md5_block#, r24 
	and	r10  = 0x3, in1 
}

{	.mmi
	ld4	r27  = [in0 ], 4
	ld4	r29  = [r23 ], 4
	.save pr, r22 
	mov	r22  = pr
	.body
}
;;
{	.mmi
	ld4	r28  = [in0 ]
	ld4	r30  = [r23 ]
	dep	out28  = 0, in1 , 0, 2
} ;;

	rum	psr.be;;

{	.mmb
	ld4	r25  = [r24 ], 4
	cmp.ne	p63 , p0 = 0, r10 
(p63 )	br.cond.spnt.many .md5_unaligned
} ;;




.md5_block_loop0:
{	.mmi
	ld4	out1  = [out28 ], 4
	mov	r14  = r24 
	mov	r15  = r25 
} ;;
{	.mmi
	ld4	out6  = [out28 ], 4
	mov	out24  = r27 
	mov	out25  = r28 
} ;;
{	.mmi
	ld4	out11  = [out28 ], 4
	mov	out26  = r29 
	mov	out27  = r30 
} ;;
{	.mmb
	ld4	out16  = [out28 ], 4
	add	in2  = -1, in2 
	br.call.sptk.many b6  = md5_digest_block0
} ;;




{	.mmi
	add	r27  = r27 , out24 
	add	r28  = r28 , out25 
	cmp.ne	p63 , p0 = 0, in2 
}
{	.mib
	add	r29  = r29 , out26 
	add	r30  = r30 , out27 
(p63 ) br.cond.dptk.many .md5_block_loop0
} ;;

.md5_exit:

	sum	psr.be;;

{	.mmi
	st4	[in0 ] = r28 , -4
	st4	[r23 ] = r30 , -4
	mov	pr = r22 , 0x1ffff ;;
}
{	.mmi
	st4	[in0 ] = r27 
	st4	[r23 ] = r29 
	mov	ar.lc = r21 
} ;;
{	.mib
	mov	ar.pfs = r20 
	br.ret.sptk.few	rp
} ;;
# 415
	.align	32
.md5_unaligned:





{	.mib
	ld4	r19  = [out28 ], 4
	cmp.eq	p63 , p0 = 1, r10 
(p63 )	br.cond.dpnt.many .md5_process1
} ;;
{	.mib
	cmp.eq	p63 , p0 = 2, r10 
	nop	0x0
(p63 )	br.cond.dpnt.many .md5_process2
} ;;
	.md5_process3: { .mib ;
nop 0x0 ;
extr.u r19 = r19, 8 * 3, 32 - 8 * 3 ;
} ;;
.md5_block_loop3: { .mmi ;
ld4 out21 = [out28], 4 ;
mov r14 = r24 ;
mov r15 = r25 ;
} ;;
{ .mmi ;
ld4 out6 = [out28], 4 ;
mov out24 = r27 ;
mov out25 = r28 ;
} ;;
{ .mii ;
ld4 out11 = [out28], 4 ;
dep.z out23 = out21, 32 - 8 * 3, 8 * 3 ;
mov out26 = r29 ;
} { .mmi ;
mov out27 = r30 ;;
or out1 = out23, r19 ;
extr.u r19 = out21, 8 * 3, 32 - 8 * 3 ;
} { .mib ;
ld4 out16 = [out28], 4 ;
add in2 = -1, in2 ;
br.call.sptk.many b6 = md5_digest_block3;
} ;;
{ .mmi ;
add r27 = r27, out24 ;
add r28 = r28, out25 ;
cmp.ne p63, p0 = 0, in2 ;
} { .mib ;
add r29 = r29, out26 ;
add r30 = r30, out27 ;
(p63) br.cond.dptk.many .md5_block_loop3 ;
} ;;
{ .mib ;
nop 0x0 ;
nop 0x0 ;
br.cond.sptk.many .md5_exit ;
} ;;
	.md5_process1: { .mib ;
nop 0x0 ;
extr.u r19 = r19, 8 * 1, 32 - 8 * 1 ;
} ;;
.md5_block_loop1: { .mmi ;
ld4 out21 = [out28], 4 ;
mov r14 = r24 ;
mov r15 = r25 ;
} ;;
{ .mmi ;
ld4 out6 = [out28], 4 ;
mov out24 = r27 ;
mov out25 = r28 ;
} ;;
{ .mii ;
ld4 out11 = [out28], 4 ;
dep.z out23 = out21, 32 - 8 * 1, 8 * 1 ;
mov out26 = r29 ;
} { .mmi ;
mov out27 = r30 ;;
or out1 = out23, r19 ;
extr.u r19 = out21, 8 * 1, 32 - 8 * 1 ;
} { .mib ;
ld4 out16 = [out28], 4 ;
add in2 = -1, in2 ;
br.call.sptk.many b6 = md5_digest_block1;
} ;;
{ .mmi ;
add r27 = r27, out24 ;
add r28 = r28, out25 ;
cmp.ne p63, p0 = 0, in2 ;
} { .mib ;
add r29 = r29, out26 ;
add r30 = r30, out27 ;
(p63) br.cond.dptk.many .md5_block_loop1 ;
} ;;
{ .mib ;
nop 0x0 ;
nop 0x0 ;
br.cond.sptk.many .md5_exit ;
} ;;
	.md5_process2: { .mib ;
nop 0x0 ;
extr.u r19 = r19, 8 * 2, 32 - 8 * 2 ;
} ;;
.md5_block_loop2: { .mmi ;
ld4 out21 = [out28], 4 ;
mov r14 = r24 ;
mov r15 = r25 ;
} ;;
{ .mmi ;
ld4 out6 = [out28], 4 ;
mov out24 = r27 ;
mov out25 = r28 ;
} ;;
{ .mii ;
ld4 out11 = [out28], 4 ;
dep.z out23 = out21, 32 - 8 * 2, 8 * 2 ;
mov out26 = r29 ;
} { .mmi ;
mov out27 = r30 ;;
or out1 = out23, r19 ;
extr.u r19 = out21, 8 * 2, 32 - 8 * 2 ;
} { .mib ;
ld4 out16 = [out28], 4 ;
add in2 = -1, in2 ;
br.call.sptk.many b6 = md5_digest_block2;
} ;;
{ .mmi ;
add r27 = r27, out24 ;
add r28 = r28, out25 ;
cmp.ne p63, p0 = 0, in2 ;
} { .mib ;
add r29 = r29, out26 ;
add r30 = r30, out27 ;
(p63) br.cond.dptk.many .md5_block_loop2 ;
} ;;
{ .mib ;
nop 0x0 ;
nop 0x0 ;
br.cond.sptk.many .md5_exit ;
} ;;

	.endp md5_block_asm_data_order
# 620
	.type md5_digest_block0, @function
	.align 32

	.proc md5_digest_block0
	.prologue
md5_digest_block0:
	.altrp b6 
	.body
{	.mmi
	alloc r8  = ar.pfs, 29 , 0 , 0 , 24 
	mov r9  = 2
	mov ar.lc = 3
} ;;
{	.mii
	cmp.eq p6 , p0 = r0, r0
	mov ar.ec = 0
	nop 0x0
} ;;

.md5_FF_round0:
	{ .mii ;
(p6) ld4 in0 = [in28], 4 ;
add in20 = in1, r15 ;
and in21 = in26, in25 ;
} { .mmi ;
andcm in22 = in27, in25 ;;
add in20 = in20, in24 ;
or in21 = in21, in22 ;
} ;;
{ .mii ;
ld4 r15 = [r14], 4 ;
add in20 = in20, in21 ;;
dep.z in21 = in20, 32, 32 ;
} ;;
{ .mii ;
nop 0x0 ;
shrp in20 = in20, in21, 64 - 7 ;;
add in24 = in20, in25 ;
} ;;
	{ .mii ;
(p6) ld4 in5 = [in28], 4 ;
add in20 = in6, r15 ;
and in21 = in25, in24 ;
} { .mmi ;
andcm in22 = in26, in24 ;;
add in20 = in20, in27 ;
or in21 = in21, in22 ;
} ;;
{ .mii ;
ld4 r15 = [r14], 4 ;
add in20 = in20, in21 ;;
dep.z in21 = in20, 32, 32 ;
} ;;
{ .mii ;
nop 0x0 ;
shrp in20 = in20, in21, 64 - 12 ;;
add in27 = in20, in24 ;
} ;;
	{ .mii ;
(p6) ld4 in10 = [in28], 4 ;
add in20 = in11, r15 ;
and in21 = in24, in27 ;
} { .mmi ;
andcm in22 = in25, in27 ;;
add in20 = in20, in26 ;
or in21 = in21, in22 ;
} ;;
{ .mii ;
ld4 r15 = [r14], 4 ;
add in20 = in20, in21 ;;
dep.z in21 = in20, 32, 32 ;
} ;;
{ .mii ;
nop 0x0 ;
shrp in20 = in20, in21, 64 - 17 ;;
add in26 = in20, in27 ;
} ;;
	{ .mii ;
(p6) ld4 in15 = [in28], 4 ;
add in20 = in16, r15 ;
and in21 = in27, in26 ;
} { .mmi ;
andcm in22 = in24, in26 ;;
add in20 = in20, in25 ;
or in21 = in21, in22 ;
} ;;
{ .mii ;
ld4 r15 = [r14], 4 ;
add in20 = in20, in21 ;;
dep.z in21 = in20, 32, 32 ;
} ;;
{ .mii ;
nop 0x0 ;
shrp in20 = in20, in21, 64 - 22 ;;
add in25 = in20, in26 ;
} { .mib ;
cmp.ne p6, p0 = 0, r9 ;
add r9 = -1, r9 ;
br.ctop.dptk.many .md5_FF_round0 ;
} ;;



	.endp md5_digest_block0

	.type md5_digest_GHI, @function
	.align 32

	.proc md5_digest_GHI
	.prologue
	.regstk 29 , 0 , 0 , 24 
md5_digest_GHI:
	.altrp b6 
	.body
# 665
{	.mmi
	mov in20  = in4 
	mov in21  = in16 
	mov ar.lc = 3
}
{	.mmi
	mov in22  = in14 
	mov in23  = in7 
	mov r11  = in3 
} ;;

{	.mmi
	mov in4  = in9 
	mov in16  = in1 
	mov ar.ec = 1
}
{	.mmi
	mov in14  = in17 
	mov in7  = in11 
	mov in3  = in8 
} ;;

{	.mmi
	mov in9  = in13 
	mov in1  = in6 
	mov r10  = in19 
}
{	.mmi
	mov in17  = in2 
	mov in11  = in18 
	mov in8  = in12 
} ;;

{	.mmi
	mov in13  = in21 
	mov in6  = in22 
	mov in19  = in20 
}
{	.mmi
	mov in2  = in23 
	mov in18  = r11 
	mov in12  = r10 
} ;;

.md5_GG_round:
	{ .mmi ;
add in20 = in4, r15 ;
and in21 = in25, in27 ;
andcm in22 = in26, in27 ;
} ;;
{ .mii ;
add in20 = in20, in24 ;
or in21 = in21, in22 ;;
add in20 = in20, in21 ;
} ;;
{ .mii ;
ld4 r15 = [r14], 4 ;
dep.z in21 = in20, 32, 32 ;;
shrp in20 = in20, in21, 64 - 5 ;
} ;;
{ .mmi ;
add in24 = in20, in25 ;
mov in0 = in4 ;
nop 0x0 ;
} ;;
{ .mmi ;
add in20 = in9, r15 ;
and in21 = in24, in26 ;
andcm in22 = in25, in26 ;
} ;;
{ .mii ;
add in20 = in20, in27 ;
or in21 = in21, in22 ;;
add in20 = in20, in21 ;
} ;;
{ .mii ;
ld4 r15 = [r14], 4 ;
dep.z in21 = in20, 32, 32 ;;
shrp in20 = in20, in21, 64 - 9 ;
} ;;
{ .mmi ;
add in27 = in20, in24 ;
mov in5 = in9 ;
nop 0x0 ;
} ;;
{ .mmi ;
add in20 = in14, r15 ;
and in21 = in27, in25 ;
andcm in22 = in24, in25 ;
} ;;
{ .mii ;
add in20 = in20, in26 ;
or in21 = in21, in22 ;;
add in20 = in20, in21 ;
} ;;
{ .mii ;
ld4 r15 = [r14], 4 ;
dep.z in21 = in20, 32, 32 ;;
shrp in20 = in20, in21, 64 - 14 ;
} ;;
{ .mmi ;
add in26 = in20, in27 ;
mov in10 = in14 ;
nop 0x0 ;
} ;;
{ .mmi ;
add in20 = in19, r15 ;
and in21 = in26, in24 ;
andcm in22 = in27, in24 ;
} ;;
{ .mii ;
add in20 = in20, in25 ;
or in21 = in21, in22 ;;
add in20 = in20, in21 ;
} ;;
{ .mii ;
ld4 r15 = [r14], 4 ;
dep.z in21 = in20, 32, 32 ;;
shrp in20 = in20, in21, 64 - 20 ;
} ;;
{ .mib ;
add in25 = in20, in26 ;
mov in15 = in19 ;
br.ctop.sptk.many .md5_GG_round ;
} ;;
# 718
{	.mmi
	mov in20  = in4 
	mov in21  = in9 
	mov ar.lc = 3
}
{	.mmi
	mov in22  = in19 
	mov in23  = in8 
	mov r11  = in13 
} ;;

{	.mmi
	mov in4  = in3 
	mov in9  = in17 
	mov ar.ec = 1
}
{	.mmi
	mov in19  = in7 
	mov r10  = in2 
	mov r9  = in6 
} ;;

{	.mmi
	mov in3  = in20 
	mov in17  = in21 
	mov in8  = in18 
}
{	.mmi
	mov in13  = in11 
	mov in2  = in1 
	mov in6  = in16 
} ;;

{	.mmi
	mov in18  = in23 
	mov in11  = r11 
	nop 0x0
}
{	.mmi
	mov in7  = in22 
	mov in1  = r10 
	mov in16  = r9 
} ;;

.md5_HH_round:
	{ .mmi ;
add in20 = in4, r15 ;
xor in21 = in25, in26 ;
nop 0x0 ;
} ;;
{ .mii ;
add in20 = in20, in24 ;
xor in21 = in21, in27 ;;
add in20 = in20, in21 ;
} ;;
{ .mii ;
ld4 r15 = [r14], 4 ;
dep.z in21 = in20, 32, 32 ;;
shrp in20 = in20, in21, 64 - 4 ;
} ;;
{ .mmi ;
add in24 = in20, in25 ;
mov in0 = in4 ;
nop 0x0 ;
} ;;
{ .mmi ;
add in20 = in9, r15 ;
xor in21 = in24, in25 ;
nop 0x0 ;
} ;;
{ .mii ;
add in20 = in20, in27 ;
xor in21 = in21, in26 ;;
add in20 = in20, in21 ;
} ;;
{ .mii ;
ld4 r15 = [r14], 4 ;
dep.z in21 = in20, 32, 32 ;;
shrp in20 = in20, in21, 64 - 11 ;
} ;;
{ .mmi ;
add in27 = in20, in24 ;
mov in5 = in9 ;
nop 0x0 ;
} ;;
{ .mmi ;
add in20 = in14, r15 ;
xor in21 = in27, in24 ;
nop 0x0 ;
} ;;
{ .mii ;
add in20 = in20, in26 ;
xor in21 = in21, in25 ;;
add in20 = in20, in21 ;
} ;;
{ .mii ;
ld4 r15 = [r14], 4 ;
dep.z in21 = in20, 32, 32 ;;
shrp in20 = in20, in21, 64 - 16 ;
} ;;
{ .mmi ;
add in26 = in20, in27 ;
mov in10 = in14 ;
nop 0x0 ;
} ;;
{ .mmi ;
add in20 = in19, r15 ;
xor in21 = in26, in27 ;
nop 0x0 ;
} ;;
{ .mii ;
add in20 = in20, in25 ;
xor in21 = in21, in24 ;;
add in20 = in20, in21 ;
} ;;
{ .mii ;
ld4 r15 = [r14], 4 ;
dep.z in21 = in20, 32, 32 ;;
shrp in20 = in20, in21, 64 - 23 ;
} ;;
{ .mib ;
add in25 = in20, in26 ;
mov in15 = in19 ;
br.ctop.sptk.many .md5_HH_round ;
} ;;
# 771
{	.mmi
	mov in20  = in4 
	mov in21  = in16 
	mov ar.lc = 3
}
{	.mmi
	mov in22  = in12 
	mov in23  = in9 
	mov r11  = in3 
} ;;

{	.mmi
	mov in4  = in7 
	mov in16  = in1 
	mov ar.ec = 1
}
{	.mmi
	mov in12  = in17 
	mov in9  = in13 
	mov in3  = in6 
} ;;

{	.mmi
	mov in7  = in11 
	mov in1  = in8 
	mov r10  = in19 
}
{	.mmi
	mov in17  = in2 
	mov in13  = in18 
	mov in6  = in14 
} ;;

{	.mmi
	mov in11  = in21 
	mov in8  = in22 
	mov in19  = in20 
}
{	.mmi
	mov in2  = in23 
	mov in18  = r11 
	mov in14  = r10 
} ;;

.md5_II_round:
	{ .mmi ;
add in20 = in4, r15 ;
andcm in21 = in27, in25 ;
andcm in22 = -1, in26 ;
} ;;
{ .mii ;
add in20 = in20, in24 ;
xor in21 = in21, in22 ;;
add in20 = in20, in21 ;
} ;;
{ .mii ;
ld4 r15 = [r14], 4 ;
dep.z in21 = in20, 32, 32 ;;
shrp in20 = in20, in21, 64 - 6 ;
} ;;
{ .mmi ;
add in24 = in20, in25 ;
mov in0 = in4 ;
nop 0x0 ;
} ;;
{ .mmi ;
add in20 = in9, r15 ;
andcm in21 = in26, in24 ;
andcm in22 = -1, in25 ;
} ;;
{ .mii ;
add in20 = in20, in27 ;
xor in21 = in21, in22 ;;
add in20 = in20, in21 ;
} ;;
{ .mii ;
ld4 r15 = [r14], 4 ;
dep.z in21 = in20, 32, 32 ;;
shrp in20 = in20, in21, 64 - 10 ;
} ;;
{ .mmi ;
add in27 = in20, in24 ;
mov in5 = in9 ;
nop 0x0 ;
} ;;
{ .mmi ;
add in20 = in14, r15 ;
andcm in21 = in25, in27 ;
andcm in22 = -1, in24 ;
} ;;
{ .mii ;
add in20 = in20, in26 ;
xor in21 = in21, in22 ;;
add in20 = in20, in21 ;
} ;;
{ .mii ;
ld4 r15 = [r14], 4 ;
dep.z in21 = in20, 32, 32 ;;
shrp in20 = in20, in21, 64 - 15 ;
} ;;
{ .mmi ;
add in26 = in20, in27 ;
mov in10 = in14 ;
nop 0x0 ;
} ;;
{ .mmi ;
add in20 = in19, r15 ;
andcm in21 = in24, in26 ;
andcm in22 = -1, in27 ;
} ;;
{ .mii ;
add in20 = in20, in25 ;
xor in21 = in21, in22 ;;
add in20 = in20, in21 ;
} ;;
{ .mii ;
ld4 r15 = [r14], 4 ;
dep.z in21 = in20, 32, 32 ;;
shrp in20 = in20, in21, 64 - 21 ;
} ;;
{ .mib ;
add in25 = in20, in26 ;
mov in15 = in19 ;
br.ctop.sptk.many .md5_II_round ;
} ;;

{	.mib
	nop 0x0
	nop 0x0
	br.ret.sptk.many b6 
} ;;

	.endp md5_digest_GHI
# 919
.type md5_digest_block1, @function ;
.align 32 ;
.proc md5_digest_block1 ;
.prologue ;
.altrp b6 ;
.body ;
md5_digest_block1: { .mmi ;
alloc r8 = ar.pfs, 29, 0, 0, 24 ;
mov r9 = 2 ;
mov ar.lc = 3 ;
} ;;
{ .mii ;
cmp.eq p6, p0 = r0, r0 ;
mov ar.ec = 0 ;
nop 0x0 ;
} ;;
.pred.rel "mutex", p7, p8 ;
.md5_FF_round1: { .mii ;
(p6) ld4 in0 = [in28], 4 ;
add in20 = in1, r15 ;
and in21 = in26, in25 ;
} { .mmi ;
andcm in22 = in27, in25 ;;
add in20 = in20, in24 ;
or in21 = in21, in22 ;
} ;;
{ .mii ;
ld4 r15 = [r14], 4 ;
dep.z in23 = in6, 32 - 8 * 1, 8 * 1 ;
add in20 = in20, in21 ;
} ;;
{ .mii ;
or in23 = in23, r19 ;
dep.z in21 = in20, 32, 32 ;;
shrp in20 = in20, in21, 64 - 7 ;
} ;;
{ .mii ;
add in24 = in20, in25 ;
extr.u r19 = in6, 8 * 1, 32 - 8 * 1 ;
mov in6 = in23 ;
} ;;
{ .mii ;
(p6) ld4 in5 = [in28], 4 ;
add in20 = in6, r15 ;
and in21 = in25, in24 ;
} { .mmi ;
andcm in22 = in26, in24 ;;
add in20 = in20, in27 ;
or in21 = in21, in22 ;
} ;;
{ .mii ;
ld4 r15 = [r14], 4 ;
dep.z in23 = in11, 32 - 8 * 1, 8 * 1 ;
add in20 = in20, in21 ;
} ;;
{ .mii ;
or in23 = in23, r19 ;
dep.z in21 = in20, 32, 32 ;;
shrp in20 = in20, in21, 64 - 12 ;
} ;;
{ .mii ;
add in27 = in20, in24 ;
extr.u r19 = in11, 8 * 1, 32 - 8 * 1 ;
mov in11 = in23 ;
} ;;
{ .mii ;
(p6) ld4 in10 = [in28], 4 ;
add in20 = in11, r15 ;
and in21 = in24, in27 ;
} { .mmi ;
andcm in22 = in25, in27 ;;
add in20 = in20, in26 ;
or in21 = in21, in22 ;
} ;;
{ .mii ;
ld4 r15 = [r14], 4 ;
dep.z in23 = in16, 32 - 8 * 1, 8 * 1 ;
add in20 = in20, in21 ;
} ;;
{ .mii ;
or in23 = in23, r19 ;
dep.z in21 = in20, 32, 32 ;;
shrp in20 = in20, in21, 64 - 17 ;
} ;;
{ .mii ;
add in26 = in20, in27 ;
extr.u r19 = in16, 8 * 1, 32 - 8 * 1 ;
mov in16 = in23 ;
} ;;
{ .mii ;
(p6) ld4 in15 = [in28], 4 ;
add in20 = in16, r15 ;
and in21 = in27, in26 ;
} { .mmi ;
andcm in22 = in24, in26 ;;
add in20 = in20, in25 ;
or in21 = in21, in22 ;
} ;;
{ .mii ;
ld4 r15 = [r14], 4 ;
(p6) dep.z in23 = in0, 32 - 8 * 1, 8 * 1 ;
add in20 = in20, in21 ;
} ;;
{ .mii ;
(p6) or in23 = in23, r19 ;
dep.z in21 = in20, 32, 32 ;;
shrp in20 = in20, in21, 64 - 22 ;
} ;;
{ .mii ;
add in25 = in20, in26 ;
(p6) extr.u r19 = in0, 8 * 1, 32 - 8 * 1 ;
(p6) mov in0 = in23 ;
} { .mib ;
cmp.ne p6, p0 = 0, r9 ;
add r9 = -1, r9 ;
br.ctop.sptk.many .md5_FF_round1 ;
} ;;
{ .mib ;
nop 0x0 ;
nop 0x0 ;
br.cond.sptk.many md5_digest_GHI ;
} ;;
.endp md5_digest_block1 
.type md5_digest_block2, @function ;
.align 32 ;
.proc md5_digest_block2 ;
.prologue ;
.altrp b6 ;
.body ;
md5_digest_block2: { .mmi ;
alloc r8 = ar.pfs, 29, 0, 0, 24 ;
mov r9 = 2 ;
mov ar.lc = 3 ;
} ;;
{ .mii ;
cmp.eq p6, p0 = r0, r0 ;
mov ar.ec = 0 ;
nop 0x0 ;
} ;;
.pred.rel "mutex", p7, p8 ;
.md5_FF_round2: { .mii ;
(p6) ld4 in0 = [in28], 4 ;
add in20 = in1, r15 ;
and in21 = in26, in25 ;
} { .mmi ;
andcm in22 = in27, in25 ;;
add in20 = in20, in24 ;
or in21 = in21, in22 ;
} ;;
{ .mii ;
ld4 r15 = [r14], 4 ;
dep.z in23 = in6, 32 - 8 * 2, 8 * 2 ;
add in20 = in20, in21 ;
} ;;
{ .mii ;
or in23 = in23, r19 ;
dep.z in21 = in20, 32, 32 ;;
shrp in20 = in20, in21, 64 - 7 ;
} ;;
{ .mii ;
add in24 = in20, in25 ;
extr.u r19 = in6, 8 * 2, 32 - 8 * 2 ;
mov in6 = in23 ;
} ;;
{ .mii ;
(p6) ld4 in5 = [in28], 4 ;
add in20 = in6, r15 ;
and in21 = in25, in24 ;
} { .mmi ;
andcm in22 = in26, in24 ;;
add in20 = in20, in27 ;
or in21 = in21, in22 ;
} ;;
{ .mii ;
ld4 r15 = [r14], 4 ;
dep.z in23 = in11, 32 - 8 * 2, 8 * 2 ;
add in20 = in20, in21 ;
} ;;
{ .mii ;
or in23 = in23, r19 ;
dep.z in21 = in20, 32, 32 ;;
shrp in20 = in20, in21, 64 - 12 ;
} ;;
{ .mii ;
add in27 = in20, in24 ;
extr.u r19 = in11, 8 * 2, 32 - 8 * 2 ;
mov in11 = in23 ;
} ;;
{ .mii ;
(p6) ld4 in10 = [in28], 4 ;
add in20 = in11, r15 ;
and in21 = in24, in27 ;
} { .mmi ;
andcm in22 = in25, in27 ;;
add in20 = in20, in26 ;
or in21 = in21, in22 ;
} ;;
{ .mii ;
ld4 r15 = [r14], 4 ;
dep.z in23 = in16, 32 - 8 * 2, 8 * 2 ;
add in20 = in20, in21 ;
} ;;
{ .mii ;
or in23 = in23, r19 ;
dep.z in21 = in20, 32, 32 ;;
shrp in20 = in20, in21, 64 - 17 ;
} ;;
{ .mii ;
add in26 = in20, in27 ;
extr.u r19 = in16, 8 * 2, 32 - 8 * 2 ;
mov in16 = in23 ;
} ;;
{ .mii ;
(p6) ld4 in15 = [in28], 4 ;
add in20 = in16, r15 ;
and in21 = in27, in26 ;
} { .mmi ;
andcm in22 = in24, in26 ;;
add in20 = in20, in25 ;
or in21 = in21, in22 ;
} ;;
{ .mii ;
ld4 r15 = [r14], 4 ;
(p6) dep.z in23 = in0, 32 - 8 * 2, 8 * 2 ;
add in20 = in20, in21 ;
} ;;
{ .mii ;
(p6) or in23 = in23, r19 ;
dep.z in21 = in20, 32, 32 ;;
shrp in20 = in20, in21, 64 - 22 ;
} ;;
{ .mii ;
add in25 = in20, in26 ;
(p6) extr.u r19 = in0, 8 * 2, 32 - 8 * 2 ;
(p6) mov in0 = in23 ;
} { .mib ;
cmp.ne p6, p0 = 0, r9 ;
add r9 = -1, r9 ;
br.ctop.sptk.many .md5_FF_round2 ;
} ;;
{ .mib ;
nop 0x0 ;
nop 0x0 ;
br.cond.sptk.many md5_digest_GHI ;
} ;;
.endp md5_digest_block2 
.type md5_digest_block3, @function ;
.align 32 ;
.proc md5_digest_block3 ;
.prologue ;
.altrp b6 ;
.body ;
md5_digest_block3: { .mmi ;
alloc r8 = ar.pfs, 29, 0, 0, 24 ;
mov r9 = 2 ;
mov ar.lc = 3 ;
} ;;
{ .mii ;
cmp.eq p6, p0 = r0, r0 ;
mov ar.ec = 0 ;
nop 0x0 ;
} ;;
.pred.rel "mutex", p7, p8 ;
.md5_FF_round3: { .mii ;
(p6) ld4 in0 = [in28], 4 ;
add in20 = in1, r15 ;
and in21 = in26, in25 ;
} { .mmi ;
andcm in22 = in27, in25 ;;
add in20 = in20, in24 ;
or in21 = in21, in22 ;
} ;;
{ .mii ;
ld4 r15 = [r14], 4 ;
dep.z in23 = in6, 32 - 8 * 3, 8 * 3 ;
add in20 = in20, in21 ;
} ;;
{ .mii ;
or in23 = in23, r19 ;
dep.z in21 = in20, 32, 32 ;;
shrp in20 = in20, in21, 64 - 7 ;
} ;;
{ .mii ;
add in24 = in20, in25 ;
extr.u r19 = in6, 8 * 3, 32 - 8 * 3 ;
mov in6 = in23 ;
} ;;
{ .mii ;
(p6) ld4 in5 = [in28], 4 ;
add in20 = in6, r15 ;
and in21 = in25, in24 ;
} { .mmi ;
andcm in22 = in26, in24 ;;
add in20 = in20, in27 ;
or in21 = in21, in22 ;
} ;;
{ .mii ;
ld4 r15 = [r14], 4 ;
dep.z in23 = in11, 32 - 8 * 3, 8 * 3 ;
add in20 = in20, in21 ;
} ;;
{ .mii ;
or in23 = in23, r19 ;
dep.z in21 = in20, 32, 32 ;;
shrp in20 = in20, in21, 64 - 12 ;
} ;;
{ .mii ;
add in27 = in20, in24 ;
extr.u r19 = in11, 8 * 3, 32 - 8 * 3 ;
mov in11 = in23 ;
} ;;
{ .mii ;
(p6) ld4 in10 = [in28], 4 ;
add in20 = in11, r15 ;
and in21 = in24, in27 ;
} { .mmi ;
andcm in22 = in25, in27 ;;
add in20 = in20, in26 ;
or in21 = in21, in22 ;
} ;;
{ .mii ;
ld4 r15 = [r14], 4 ;
dep.z in23 = in16, 32 - 8 * 3, 8 * 3 ;
add in20 = in20, in21 ;
} ;;
{ .mii ;
or in23 = in23, r19 ;
dep.z in21 = in20, 32, 32 ;;
shrp in20 = in20, in21, 64 - 17 ;
} ;;
{ .mii ;
add in26 = in20, in27 ;
extr.u r19 = in16, 8 * 3, 32 - 8 * 3 ;
mov in16 = in23 ;
} ;;
{ .mii ;
(p6) ld4 in15 = [in28], 4 ;
add in20 = in16, r15 ;
and in21 = in27, in26 ;
} { .mmi ;
andcm in22 = in24, in26 ;;
add in20 = in20, in25 ;
or in21 = in21, in22 ;
} ;;
{ .mii ;
ld4 r15 = [r14], 4 ;
(p6) dep.z in23 = in0, 32 - 8 * 3, 8 * 3 ;
add in20 = in20, in21 ;
} ;;
{ .mii ;
(p6) or in23 = in23, r19 ;
dep.z in21 = in20, 32, 32 ;;
shrp in20 = in20, in21, 64 - 22 ;
} ;;
{ .mii ;
add in25 = in20, in26 ;
(p6) extr.u r19 = in0, 8 * 3, 32 - 8 * 3 ;
(p6) mov in0 = in23 ;
} { .mib ;
cmp.ne p6, p0 = 0, r9 ;
add r9 = -1, r9 ;
br.ctop.sptk.many .md5_FF_round3 ;
} ;;
{ .mib ;
nop 0x0 ;
nop 0x0 ;
br.cond.sptk.many md5_digest_GHI ;
} ;;
.endp md5_digest_block3 

	.align 64
	.type md5_constants, @object
md5_constants:
.md5_tbl_data_order:

	data1 0x78, 0xa4, 0x6a, 0xd7
	data1 0x56, 0xb7, 0xc7, 0xe8
	data1 0xdb, 0x70, 0x20, 0x24
	data1 0xee, 0xce, 0xbd, 0xc1
	data1 0xaf, 0x0f, 0x7c, 0xf5
	data1 0x2a, 0xc6, 0x87, 0x47
	data1 0x13, 0x46, 0x30, 0xa8
	data1 0x01, 0x95, 0x46, 0xfd
	data1 0xd8, 0x98, 0x80, 0x69
	data1 0xaf, 0xf7, 0x44, 0x8b
	data1 0xb1, 0x5b, 0xff, 0xff
	data1 0xbe, 0xd7, 0x5c, 0x89
	data1 0x22, 0x11, 0x90, 0x6b
	data1 0x93, 0x71, 0x98, 0xfd
	data1 0x8e, 0x43, 0x79, 0xa6
	data1 0x21, 0x08, 0xb4, 0x49
	data1 0x62, 0x25, 0x1e, 0xf6
	data1 0x40, 0xb3, 0x40, 0xc0
	data1 0x51, 0x5a, 0x5e, 0x26
	data1 0xaa, 0xc7, 0xb6, 0xe9
	data1 0x5d, 0x10, 0x2f, 0xd6
	data1 0x53, 0x14, 0x44, 0x02
	data1 0x81, 0xe6, 0xa1, 0xd8
	data1 0xc8, 0xfb, 0xd3, 0xe7
	data1 0xe6, 0xcd, 0xe1, 0x21
	data1 0xd6, 0x07, 0x37, 0xc3
	data1 0x87, 0x0d, 0xd5, 0xf4
	data1 0xed, 0x14, 0x5a, 0x45
	data1 0x05, 0xe9, 0xe3, 0xa9
	data1 0xf8, 0xa3, 0xef, 0xfc
	data1 0xd9, 0x02, 0x6f, 0x67
	data1 0x8a, 0x4c, 0x2a, 0x8d
	data1 0x42, 0x39, 0xfa, 0xff
	data1 0x81, 0xf6, 0x71, 0x87
	data1 0x22, 0x61, 0x9d, 0x6d
	data1 0x0c, 0x38, 0xe5, 0xfd
	data1 0x44, 0xea, 0xbe, 0xa4
	data1 0xa9, 0xcf, 0xde, 0x4b
	data1 0x60, 0x4b, 0xbb, 0xf6
	data1 0x70, 0xbc, 0xbf, 0xbe
	data1 0xc6, 0x7e, 0x9b, 0x28
	data1 0xfa, 0x27, 0xa1, 0xea
	data1 0x85, 0x30, 0xef, 0xd4
	data1 0x05, 0x1d, 0x88, 0x04
	data1 0x39, 0xd0, 0xd4, 0xd9
	data1 0xe5, 0x99, 0xdb, 0xe6
	data1 0xf8, 0x7c, 0xa2, 0x1f
	data1 0x65, 0x56, 0xac, 0xc4
	data1 0x44, 0x22, 0x29, 0xf4
	data1 0x97, 0xff, 0x2a, 0x43
	data1 0xa7, 0x23, 0x94, 0xab
	data1 0x39, 0xa0, 0x93, 0xfc
	data1 0xc3, 0x59, 0x5b, 0x65
	data1 0x92, 0xcc, 0x0c, 0x8f
	data1 0x7d, 0xf4, 0xef, 0xff
	data1 0xd1, 0x5d, 0x84, 0x85
	data1 0x4f, 0x7e, 0xa8, 0x6f
	data1 0xe0, 0xe6, 0x2c, 0xfe
	data1 0x14, 0x43, 0x01, 0xa3
	data1 0xa1, 0x11, 0x08, 0x4e
	data1 0x82, 0x7e, 0x53, 0xf7
	data1 0x35, 0xf2, 0x3a, 0xbd
	data1 0xbb, 0xd2, 0xd7, 0x2a
	data1 0x91, 0xd3, 0x86, 0xeb
.size	md5_constants#,64*4

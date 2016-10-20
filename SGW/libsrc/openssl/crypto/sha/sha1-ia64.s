.ident  "sha1-ia64.s, version 1.2"
.ident  "IA-64 ISA artwork by Andy Polyakov <appro@fy.chalmers.se>"
.explicit

.text

tmp0=r8;
tmp1=r9;
tmp2=r10;
tmp3=r11;
ctx=r32;	// in0
inp=r33;	// in1

// void sha1_block_data_order(SHA_CTX *c,const void *p,size_t num);
.global	sha1_block_data_order#
.proc	sha1_block_data_order#
.align	32
sha1_block_data_order:
	.prologue
{ .mmi;	alloc	tmp1=ar.pfs,3,15,0,0
	add	tmp0=4,ctx
	.save	ar.lc,r3
	mov	r3=ar.lc		}
{ .mmi;	add	ctx=0,ctx
	add	inp=0,inp
	mov	r2=pr			};;
tmp4=in2;
tmp5=loc13;
tmp6=loc14;
	.body
{ .mlx;	ld4	loc6=[ctx],8
	movl	r14=0x5a827999	}
{ .mlx;	ld4	loc7=[tmp0],8
	movl	r15=0x6ed9eba1	};;
{ .mlx;	ld4	loc8=[ctx],8
	movl	loc11=0x8f1bbcdc	}
{ .mlx;	ld4	loc9=[tmp0]
	movl	loc12=0xca62c1d6	};;
{ .mmi;	ld4	loc10=[ctx],-16
	add	in2=-1,in2		    // adjust num for ar.lc
	mov	ar.ec=1			};;
{ .mmi;	nop.m	0
	add	tmp3=1,inp
	mov	ar.lc=in2		};; // brp.loop.imp: too far

.Ldtop:
{ .mmi;	mov	loc0=loc6
	mov	loc1=loc7
	mux2	tmp6=loc7,0x44		}
{ .mmi;	mov	loc2=loc8
	mov	loc3=loc9
	mov	loc4=loc10			};;

{ .mmi;	ld1	r16=[inp],2	    // MSB
	ld1	tmp2=[tmp3],2		};;
{ .mmi;	ld1	tmp0=[inp],2
	ld1	tmp4=[tmp3],2		    // LSB
	dep	r16=r16,tmp2,8,8	};;
{ .mmi;	ld1	r17=[inp],2	    // +1
	dep	tmp1=tmp0,tmp4,8,8	};;
{ .mmi;	ld1	tmp2=[tmp3],2		    // +1
	and	tmp4=loc2,loc1
	dep	r16=r16,tmp1,16,16	} //;;
{ .mmi;	andcm	tmp1=loc3,loc1
	add	tmp0=loc4,r14
	dep.z	tmp5=loc0,5,27		};; // a<<5
{ .mmi;	or	tmp4=tmp4,tmp1		    // F_00_19(b,c,d)=(b&c)|(~b&d)
	add	loc5=tmp0,r16	    // f=xi+e+K_00_19
	extr.u	tmp1=loc0,27,5		};; // a>>27
{ .mmi;	ld1	tmp0=[inp],2		    // +1
	add	loc5=loc5,tmp4		    // f+=F_00_19(b,c,d)
	shrp	loc1=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	ld1	tmp4=[tmp3],2		    // +1
	or	tmp5=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc0,0x44		};; // see b in next iteration
{ .mii;	add	loc5=loc5,tmp5		    // f+=ROTATE(a,5)
	dep	r17=r17,tmp2,8,8	// +1
	mux2	r16=r16,0x44	} //;;

{ .mmi;	ld1	r18=[inp],2	    // +1
	dep	tmp1=tmp0,tmp4,8,8	};;
{ .mmi;	ld1	tmp2=[tmp3],2		    // +1
	and	tmp4=loc1,loc0
	dep	r17=r17,tmp1,16,16	} //;;
{ .mmi;	andcm	tmp1=loc2,loc0
	add	tmp0=loc3,r14
	dep.z	tmp5=loc5,5,27		};; // a<<5
{ .mmi;	or	tmp4=tmp4,tmp1		    // F_00_19(b,c,d)=(b&c)|(~b&d)
	add	loc4=tmp0,r17	    // f=xi+e+K_00_19
	extr.u	tmp1=loc5,27,5		};; // a>>27
{ .mmi;	ld1	tmp0=[inp],2		    // +1
	add	loc4=loc4,tmp4		    // f+=F_00_19(b,c,d)
	shrp	loc0=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	ld1	tmp4=[tmp3],2		    // +1
	or	tmp5=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc5,0x44		};; // see b in next iteration
{ .mii;	add	loc4=loc4,tmp5		    // f+=ROTATE(a,5)
	dep	r18=r18,tmp2,8,8	// +1
	mux2	r17=r17,0x44	} //;;

{ .mmi;	ld1	r19=[inp],2	    // +1
	dep	tmp1=tmp0,tmp4,8,8	};;
{ .mmi;	ld1	tmp2=[tmp3],2		    // +1
	and	tmp4=loc0,loc5
	dep	r18=r18,tmp1,16,16	} //;;
{ .mmi;	andcm	tmp1=loc1,loc5
	add	tmp0=loc2,r14
	dep.z	tmp5=loc4,5,27		};; // a<<5
{ .mmi;	or	tmp4=tmp4,tmp1		    // F_00_19(b,c,d)=(b&c)|(~b&d)
	add	loc3=tmp0,r18	    // f=xi+e+K_00_19
	extr.u	tmp1=loc4,27,5		};; // a>>27
{ .mmi;	ld1	tmp0=[inp],2		    // +1
	add	loc3=loc3,tmp4		    // f+=F_00_19(b,c,d)
	shrp	loc5=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	ld1	tmp4=[tmp3],2		    // +1
	or	tmp5=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc4,0x44		};; // see b in next iteration
{ .mii;	add	loc3=loc3,tmp5		    // f+=ROTATE(a,5)
	dep	r19=r19,tmp2,8,8	// +1
	mux2	r18=r18,0x44	} //;;

{ .mmi;	ld1	r20=[inp],2	    // +1
	dep	tmp1=tmp0,tmp4,8,8	};;
{ .mmi;	ld1	tmp2=[tmp3],2		    // +1
	and	tmp4=loc5,loc4
	dep	r19=r19,tmp1,16,16	} //;;
{ .mmi;	andcm	tmp1=loc0,loc4
	add	tmp0=loc1,r14
	dep.z	tmp5=loc3,5,27		};; // a<<5
{ .mmi;	or	tmp4=tmp4,tmp1		    // F_00_19(b,c,d)=(b&c)|(~b&d)
	add	loc2=tmp0,r19	    // f=xi+e+K_00_19
	extr.u	tmp1=loc3,27,5		};; // a>>27
{ .mmi;	ld1	tmp0=[inp],2		    // +1
	add	loc2=loc2,tmp4		    // f+=F_00_19(b,c,d)
	shrp	loc4=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	ld1	tmp4=[tmp3],2		    // +1
	or	tmp5=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc3,0x44		};; // see b in next iteration
{ .mii;	add	loc2=loc2,tmp5		    // f+=ROTATE(a,5)
	dep	r20=r20,tmp2,8,8	// +1
	mux2	r19=r19,0x44	} //;;

{ .mmi;	ld1	r21=[inp],2	    // +1
	dep	tmp1=tmp0,tmp4,8,8	};;
{ .mmi;	ld1	tmp2=[tmp3],2		    // +1
	and	tmp4=loc4,loc3
	dep	r20=r20,tmp1,16,16	} //;;
{ .mmi;	andcm	tmp1=loc5,loc3
	add	tmp0=loc0,r14
	dep.z	tmp5=loc2,5,27		};; // a<<5
{ .mmi;	or	tmp4=tmp4,tmp1		    // F_00_19(b,c,d)=(b&c)|(~b&d)
	add	loc1=tmp0,r20	    // f=xi+e+K_00_19
	extr.u	tmp1=loc2,27,5		};; // a>>27
{ .mmi;	ld1	tmp0=[inp],2		    // +1
	add	loc1=loc1,tmp4		    // f+=F_00_19(b,c,d)
	shrp	loc3=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	ld1	tmp4=[tmp3],2		    // +1
	or	tmp5=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc2,0x44		};; // see b in next iteration
{ .mii;	add	loc1=loc1,tmp5		    // f+=ROTATE(a,5)
	dep	r21=r21,tmp2,8,8	// +1
	mux2	r20=r20,0x44	} //;;

{ .mmi;	ld1	r22=[inp],2	    // +1
	dep	tmp1=tmp0,tmp4,8,8	};;
{ .mmi;	ld1	tmp2=[tmp3],2		    // +1
	and	tmp4=loc3,loc2
	dep	r21=r21,tmp1,16,16	} //;;
{ .mmi;	andcm	tmp1=loc4,loc2
	add	tmp0=loc5,r14
	dep.z	tmp5=loc1,5,27		};; // a<<5
{ .mmi;	or	tmp4=tmp4,tmp1		    // F_00_19(b,c,d)=(b&c)|(~b&d)
	add	loc0=tmp0,r21	    // f=xi+e+K_00_19
	extr.u	tmp1=loc1,27,5		};; // a>>27
{ .mmi;	ld1	tmp0=[inp],2		    // +1
	add	loc0=loc0,tmp4		    // f+=F_00_19(b,c,d)
	shrp	loc2=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	ld1	tmp4=[tmp3],2		    // +1
	or	tmp5=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc1,0x44		};; // see b in next iteration
{ .mii;	add	loc0=loc0,tmp5		    // f+=ROTATE(a,5)
	dep	r22=r22,tmp2,8,8	// +1
	mux2	r21=r21,0x44	} //;;

{ .mmi;	ld1	r23=[inp],2	    // +1
	dep	tmp1=tmp0,tmp4,8,8	};;
{ .mmi;	ld1	tmp2=[tmp3],2		    // +1
	and	tmp4=loc2,loc1
	dep	r22=r22,tmp1,16,16	} //;;
{ .mmi;	andcm	tmp1=loc3,loc1
	add	tmp0=loc4,r14
	dep.z	tmp5=loc0,5,27		};; // a<<5
{ .mmi;	or	tmp4=tmp4,tmp1		    // F_00_19(b,c,d)=(b&c)|(~b&d)
	add	loc5=tmp0,r22	    // f=xi+e+K_00_19
	extr.u	tmp1=loc0,27,5		};; // a>>27
{ .mmi;	ld1	tmp0=[inp],2		    // +1
	add	loc5=loc5,tmp4		    // f+=F_00_19(b,c,d)
	shrp	loc1=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	ld1	tmp4=[tmp3],2		    // +1
	or	tmp5=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc0,0x44		};; // see b in next iteration
{ .mii;	add	loc5=loc5,tmp5		    // f+=ROTATE(a,5)
	dep	r23=r23,tmp2,8,8	// +1
	mux2	r22=r22,0x44	} //;;

{ .mmi;	ld1	r24=[inp],2	    // +1
	dep	tmp1=tmp0,tmp4,8,8	};;
{ .mmi;	ld1	tmp2=[tmp3],2		    // +1
	and	tmp4=loc1,loc0
	dep	r23=r23,tmp1,16,16	} //;;
{ .mmi;	andcm	tmp1=loc2,loc0
	add	tmp0=loc3,r14
	dep.z	tmp5=loc5,5,27		};; // a<<5
{ .mmi;	or	tmp4=tmp4,tmp1		    // F_00_19(b,c,d)=(b&c)|(~b&d)
	add	loc4=tmp0,r23	    // f=xi+e+K_00_19
	extr.u	tmp1=loc5,27,5		};; // a>>27
{ .mmi;	ld1	tmp0=[inp],2		    // +1
	add	loc4=loc4,tmp4		    // f+=F_00_19(b,c,d)
	shrp	loc0=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	ld1	tmp4=[tmp3],2		    // +1
	or	tmp5=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc5,0x44		};; // see b in next iteration
{ .mii;	add	loc4=loc4,tmp5		    // f+=ROTATE(a,5)
	dep	r24=r24,tmp2,8,8	// +1
	mux2	r23=r23,0x44	} //;;

{ .mmi;	ld1	r25=[inp],2	    // +1
	dep	tmp1=tmp0,tmp4,8,8	};;
{ .mmi;	ld1	tmp2=[tmp3],2		    // +1
	and	tmp4=loc0,loc5
	dep	r24=r24,tmp1,16,16	} //;;
{ .mmi;	andcm	tmp1=loc1,loc5
	add	tmp0=loc2,r14
	dep.z	tmp5=loc4,5,27		};; // a<<5
{ .mmi;	or	tmp4=tmp4,tmp1		    // F_00_19(b,c,d)=(b&c)|(~b&d)
	add	loc3=tmp0,r24	    // f=xi+e+K_00_19
	extr.u	tmp1=loc4,27,5		};; // a>>27
{ .mmi;	ld1	tmp0=[inp],2		    // +1
	add	loc3=loc3,tmp4		    // f+=F_00_19(b,c,d)
	shrp	loc5=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	ld1	tmp4=[tmp3],2		    // +1
	or	tmp5=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc4,0x44		};; // see b in next iteration
{ .mii;	add	loc3=loc3,tmp5		    // f+=ROTATE(a,5)
	dep	r25=r25,tmp2,8,8	// +1
	mux2	r24=r24,0x44	} //;;

{ .mmi;	ld1	r26=[inp],2	    // +1
	dep	tmp1=tmp0,tmp4,8,8	};;
{ .mmi;	ld1	tmp2=[tmp3],2		    // +1
	and	tmp4=loc5,loc4
	dep	r25=r25,tmp1,16,16	} //;;
{ .mmi;	andcm	tmp1=loc0,loc4
	add	tmp0=loc1,r14
	dep.z	tmp5=loc3,5,27		};; // a<<5
{ .mmi;	or	tmp4=tmp4,tmp1		    // F_00_19(b,c,d)=(b&c)|(~b&d)
	add	loc2=tmp0,r25	    // f=xi+e+K_00_19
	extr.u	tmp1=loc3,27,5		};; // a>>27
{ .mmi;	ld1	tmp0=[inp],2		    // +1
	add	loc2=loc2,tmp4		    // f+=F_00_19(b,c,d)
	shrp	loc4=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	ld1	tmp4=[tmp3],2		    // +1
	or	tmp5=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc3,0x44		};; // see b in next iteration
{ .mii;	add	loc2=loc2,tmp5		    // f+=ROTATE(a,5)
	dep	r26=r26,tmp2,8,8	// +1
	mux2	r25=r25,0x44	} //;;

{ .mmi;	ld1	r27=[inp],2	    // +1
	dep	tmp1=tmp0,tmp4,8,8	};;
{ .mmi;	ld1	tmp2=[tmp3],2		    // +1
	and	tmp4=loc4,loc3
	dep	r26=r26,tmp1,16,16	} //;;
{ .mmi;	andcm	tmp1=loc5,loc3
	add	tmp0=loc0,r14
	dep.z	tmp5=loc2,5,27		};; // a<<5
{ .mmi;	or	tmp4=tmp4,tmp1		    // F_00_19(b,c,d)=(b&c)|(~b&d)
	add	loc1=tmp0,r26	    // f=xi+e+K_00_19
	extr.u	tmp1=loc2,27,5		};; // a>>27
{ .mmi;	ld1	tmp0=[inp],2		    // +1
	add	loc1=loc1,tmp4		    // f+=F_00_19(b,c,d)
	shrp	loc3=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	ld1	tmp4=[tmp3],2		    // +1
	or	tmp5=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc2,0x44		};; // see b in next iteration
{ .mii;	add	loc1=loc1,tmp5		    // f+=ROTATE(a,5)
	dep	r27=r27,tmp2,8,8	// +1
	mux2	r26=r26,0x44	} //;;

{ .mmi;	ld1	r28=[inp],2	    // +1
	dep	tmp1=tmp0,tmp4,8,8	};;
{ .mmi;	ld1	tmp2=[tmp3],2		    // +1
	and	tmp4=loc3,loc2
	dep	r27=r27,tmp1,16,16	} //;;
{ .mmi;	andcm	tmp1=loc4,loc2
	add	tmp0=loc5,r14
	dep.z	tmp5=loc1,5,27		};; // a<<5
{ .mmi;	or	tmp4=tmp4,tmp1		    // F_00_19(b,c,d)=(b&c)|(~b&d)
	add	loc0=tmp0,r27	    // f=xi+e+K_00_19
	extr.u	tmp1=loc1,27,5		};; // a>>27
{ .mmi;	ld1	tmp0=[inp],2		    // +1
	add	loc0=loc0,tmp4		    // f+=F_00_19(b,c,d)
	shrp	loc2=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	ld1	tmp4=[tmp3],2		    // +1
	or	tmp5=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc1,0x44		};; // see b in next iteration
{ .mii;	add	loc0=loc0,tmp5		    // f+=ROTATE(a,5)
	dep	r28=r28,tmp2,8,8	// +1
	mux2	r27=r27,0x44	} //;;

{ .mmi;	ld1	r29=[inp],2	    // +1
	dep	tmp1=tmp0,tmp4,8,8	};;
{ .mmi;	ld1	tmp2=[tmp3],2		    // +1
	and	tmp4=loc2,loc1
	dep	r28=r28,tmp1,16,16	} //;;
{ .mmi;	andcm	tmp1=loc3,loc1
	add	tmp0=loc4,r14
	dep.z	tmp5=loc0,5,27		};; // a<<5
{ .mmi;	or	tmp4=tmp4,tmp1		    // F_00_19(b,c,d)=(b&c)|(~b&d)
	add	loc5=tmp0,r28	    // f=xi+e+K_00_19
	extr.u	tmp1=loc0,27,5		};; // a>>27
{ .mmi;	ld1	tmp0=[inp],2		    // +1
	add	loc5=loc5,tmp4		    // f+=F_00_19(b,c,d)
	shrp	loc1=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	ld1	tmp4=[tmp3],2		    // +1
	or	tmp5=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc0,0x44		};; // see b in next iteration
{ .mii;	add	loc5=loc5,tmp5		    // f+=ROTATE(a,5)
	dep	r29=r29,tmp2,8,8	// +1
	mux2	r28=r28,0x44	} //;;

{ .mmi;	ld1	r30=[inp],2	    // +1
	dep	tmp1=tmp0,tmp4,8,8	};;
{ .mmi;	ld1	tmp2=[tmp3],2		    // +1
	and	tmp4=loc1,loc0
	dep	r29=r29,tmp1,16,16	} //;;
{ .mmi;	andcm	tmp1=loc2,loc0
	add	tmp0=loc3,r14
	dep.z	tmp5=loc5,5,27		};; // a<<5
{ .mmi;	or	tmp4=tmp4,tmp1		    // F_00_19(b,c,d)=(b&c)|(~b&d)
	add	loc4=tmp0,r29	    // f=xi+e+K_00_19
	extr.u	tmp1=loc5,27,5		};; // a>>27
{ .mmi;	ld1	tmp0=[inp],2		    // +1
	add	loc4=loc4,tmp4		    // f+=F_00_19(b,c,d)
	shrp	loc0=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	ld1	tmp4=[tmp3],2		    // +1
	or	tmp5=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc5,0x44		};; // see b in next iteration
{ .mii;	add	loc4=loc4,tmp5		    // f+=ROTATE(a,5)
	dep	r30=r30,tmp2,8,8	// +1
	mux2	r29=r29,0x44	} //;;

{ .mmi;	ld1	r31=[inp],2	    // +1
	dep	tmp1=tmp0,tmp4,8,8	};;
{ .mmi;	ld1	tmp2=[tmp3],2		    // +1
	and	tmp4=loc0,loc5
	dep	r30=r30,tmp1,16,16	} //;;
{ .mmi;	andcm	tmp1=loc1,loc5
	add	tmp0=loc2,r14
	dep.z	tmp5=loc4,5,27		};; // a<<5
{ .mmi;	or	tmp4=tmp4,tmp1		    // F_00_19(b,c,d)=(b&c)|(~b&d)
	add	loc3=tmp0,r30	    // f=xi+e+K_00_19
	extr.u	tmp1=loc4,27,5		};; // a>>27
{ .mmi;	ld1	tmp0=[inp],2		    // +1
	add	loc3=loc3,tmp4		    // f+=F_00_19(b,c,d)
	shrp	loc5=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	ld1	tmp4=[tmp3],2		    // +1
	or	tmp5=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc4,0x44		};; // see b in next iteration
{ .mii;	add	loc3=loc3,tmp5		    // f+=ROTATE(a,5)
	dep	r31=r31,tmp2,8,8	// +1
	mux2	r30=r30,0x44	} //;;

{ .mii;	and	tmp3=loc5,loc4
	dep	tmp1=tmp0,tmp4,8,8;;
	dep	r31=r31,tmp1,16,16	} //;;
{ .mmi;	andcm	tmp1=loc0,loc4
	add	tmp0=loc1,r14
	dep.z	tmp5=loc3,5,27		};; // a<<5
{ .mmi;	or	tmp4=tmp3,tmp1		    // F_00_19(b,c,d)=(b&c)|(~b&d)
	add	loc2=tmp0,r31	    // f=xi+e+K_00_19
	extr.u	tmp1=loc3,27,5		}   // a>>27
{ .mmi;	xor	tmp2=r16,r18	// +1
	xor	tmp3=r24,r29 // +1
	nop.i	0			};;
{ .mmi;	add	loc2=loc2,tmp4		    // f+=F_00_19(b,c,d)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc4=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi; or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc3,0x44		};; // see b in next iteration
{ .mii;	add	loc2=loc2,tmp1		    // f+=ROTATE(a,5)
	shrp	loc1=tmp2,tmp2,31		    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	mux2	r31=r31,0x44  };;

{ .mmi;	mov	r16=loc1		    // Xupdate
	and	tmp0=loc4,loc3
	dep.z	tmp5=loc2,5,27		}   // a<<5
{ .mmi;	andcm	tmp1=loc5,loc3
	add	tmp4=loc0,r14	};;
{ .mmi;	or	tmp0=tmp0,tmp1		    // F_00_19(b,c,d)=(b&c)|(~b&d)
	add	loc1=loc1,tmp4		    // f+=e+K_00_19
	extr.u	tmp1=loc2,27,5		}   // a>>27
{ .mmi;	xor	tmp2=r17,r19	// +1
	xor	tmp3=r25,r30	// +1
	nop.i	0			};;
{ .mmi;	add	loc1=loc1,tmp0		    // f+=F_00_19(b,c,d)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc3=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc2,0x44		};; // see b in next iteration
{ .mii;	add	loc1=loc1,tmp1		    // f+=ROTATE(a,5)
	shrp	loc0=tmp2,tmp2,31		    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	nop.i	0			};;

{ .mmi;	mov	r17=loc0		    // Xupdate
	and	tmp0=loc3,loc2
	dep.z	tmp5=loc1,5,27		}   // a<<5
{ .mmi;	andcm	tmp1=loc4,loc2
	add	tmp4=loc5,r14	};;
{ .mmi;	or	tmp0=tmp0,tmp1		    // F_00_19(b,c,d)=(b&c)|(~b&d)
	add	loc0=loc0,tmp4		    // f+=e+K_00_19
	extr.u	tmp1=loc1,27,5		}   // a>>27
{ .mmi;	xor	tmp2=r18,r20	// +1
	xor	tmp3=r26,r31	// +1
	nop.i	0			};;
{ .mmi;	add	loc0=loc0,tmp0		    // f+=F_00_19(b,c,d)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc2=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc1,0x44		};; // see b in next iteration
{ .mii;	add	loc0=loc0,tmp1		    // f+=ROTATE(a,5)
	shrp	loc5=tmp2,tmp2,31		    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	nop.i	0			};;

{ .mmi;	mov	r18=loc5		    // Xupdate
	and	tmp0=loc2,loc1
	dep.z	tmp5=loc0,5,27		}   // a<<5
{ .mmi;	andcm	tmp1=loc3,loc1
	add	tmp4=loc4,r14	};;
{ .mmi;	or	tmp0=tmp0,tmp1		    // F_00_19(b,c,d)=(b&c)|(~b&d)
	add	loc5=loc5,tmp4		    // f+=e+K_00_19
	extr.u	tmp1=loc0,27,5		}   // a>>27
{ .mmi;	xor	tmp2=r19,r21	// +1
	xor	tmp3=r27,r16	// +1
	nop.i	0			};;
{ .mmi;	add	loc5=loc5,tmp0		    // f+=F_00_19(b,c,d)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc1=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc0,0x44		};; // see b in next iteration
{ .mii;	add	loc5=loc5,tmp1		    // f+=ROTATE(a,5)
	shrp	loc4=tmp2,tmp2,31		    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	nop.i	0			};;

{ .mmi;	mov	r19=loc4		    // Xupdate
	and	tmp0=loc1,loc0
	dep.z	tmp5=loc5,5,27		}   // a<<5
{ .mmi;	andcm	tmp1=loc2,loc0
	add	tmp4=loc3,r14	};;
{ .mmi;	or	tmp0=tmp0,tmp1		    // F_00_19(b,c,d)=(b&c)|(~b&d)
	add	loc4=loc4,tmp4		    // f+=e+K_00_19
	extr.u	tmp1=loc5,27,5		}   // a>>27
{ .mmi;	xor	tmp2=r20,r22	// +1
	xor	tmp3=r28,r17	// +1
	nop.i	0			};;
{ .mmi;	add	loc4=loc4,tmp0		    // f+=F_00_19(b,c,d)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc0=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc5,0x44		};; // see b in next iteration
{ .mii;	add	loc4=loc4,tmp1		    // f+=ROTATE(a,5)
	shrp	loc3=tmp2,tmp2,31		    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	nop.i	0			};;

{ .mib;	mov	r20=loc3		    // Xupdate
	dep.z	tmp5=loc4,5,27		}   // a<<5
{ .mib;	xor	tmp0=loc0,loc5
	add	tmp4=loc2,r15		};;
{ .mmi;	xor	tmp0=tmp0,loc1		    // F_20_39(b,c,d)=b^c^d
	add	loc3=loc3,tmp4		    // f+=e+K_20_39
	extr.u	tmp1=loc4,27,5		}   // a>>27
{ .mmi;	xor	tmp2=r21,r23	// +1
	xor	tmp3=r29,r18	// +1
	nop.i	0			};;
{ .mmi;	add	loc3=loc3,tmp0		    // f+=F_20_39(b,c,d)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc5=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc4,0x44		};; // see b in next iteration
{ .mii;	add	loc3=loc3,tmp1		    // f+=ROTATE(a,5)
	shrp	loc2=tmp2,tmp2,31		    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	nop.i	0			};;

{ .mib;	mov	r21=loc2		    // Xupdate
	dep.z	tmp5=loc3,5,27		}   // a<<5
{ .mib;	xor	tmp0=loc5,loc4
	add	tmp4=loc1,r15		};;
{ .mmi;	xor	tmp0=tmp0,loc0		    // F_20_39(b,c,d)=b^c^d
	add	loc2=loc2,tmp4		    // f+=e+K_20_39
	extr.u	tmp1=loc3,27,5		}   // a>>27
{ .mmi;	xor	tmp2=r22,r24	// +1
	xor	tmp3=r30,r19	// +1
	nop.i	0			};;
{ .mmi;	add	loc2=loc2,tmp0		    // f+=F_20_39(b,c,d)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc4=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc3,0x44		};; // see b in next iteration
{ .mii;	add	loc2=loc2,tmp1		    // f+=ROTATE(a,5)
	shrp	loc1=tmp2,tmp2,31		    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	nop.i	0			};;

{ .mib;	mov	r22=loc1		    // Xupdate
	dep.z	tmp5=loc2,5,27		}   // a<<5
{ .mib;	xor	tmp0=loc4,loc3
	add	tmp4=loc0,r15		};;
{ .mmi;	xor	tmp0=tmp0,loc5		    // F_20_39(b,c,d)=b^c^d
	add	loc1=loc1,tmp4		    // f+=e+K_20_39
	extr.u	tmp1=loc2,27,5		}   // a>>27
{ .mmi;	xor	tmp2=r23,r25	// +1
	xor	tmp3=r31,r20	// +1
	nop.i	0			};;
{ .mmi;	add	loc1=loc1,tmp0		    // f+=F_20_39(b,c,d)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc3=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc2,0x44		};; // see b in next iteration
{ .mii;	add	loc1=loc1,tmp1		    // f+=ROTATE(a,5)
	shrp	loc0=tmp2,tmp2,31		    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	nop.i	0			};;

{ .mib;	mov	r23=loc0		    // Xupdate
	dep.z	tmp5=loc1,5,27		}   // a<<5
{ .mib;	xor	tmp0=loc3,loc2
	add	tmp4=loc5,r15		};;
{ .mmi;	xor	tmp0=tmp0,loc4		    // F_20_39(b,c,d)=b^c^d
	add	loc0=loc0,tmp4		    // f+=e+K_20_39
	extr.u	tmp1=loc1,27,5		}   // a>>27
{ .mmi;	xor	tmp2=r24,r26	// +1
	xor	tmp3=r16,r21	// +1
	nop.i	0			};;
{ .mmi;	add	loc0=loc0,tmp0		    // f+=F_20_39(b,c,d)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc2=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc1,0x44		};; // see b in next iteration
{ .mii;	add	loc0=loc0,tmp1		    // f+=ROTATE(a,5)
	shrp	loc5=tmp2,tmp2,31		    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	nop.i	0			};;

{ .mib;	mov	r24=loc5		    // Xupdate
	dep.z	tmp5=loc0,5,27		}   // a<<5
{ .mib;	xor	tmp0=loc2,loc1
	add	tmp4=loc4,r15		};;
{ .mmi;	xor	tmp0=tmp0,loc3		    // F_20_39(b,c,d)=b^c^d
	add	loc5=loc5,tmp4		    // f+=e+K_20_39
	extr.u	tmp1=loc0,27,5		}   // a>>27
{ .mmi;	xor	tmp2=r25,r27	// +1
	xor	tmp3=r17,r22	// +1
	nop.i	0			};;
{ .mmi;	add	loc5=loc5,tmp0		    // f+=F_20_39(b,c,d)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc1=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc0,0x44		};; // see b in next iteration
{ .mii;	add	loc5=loc5,tmp1		    // f+=ROTATE(a,5)
	shrp	loc4=tmp2,tmp2,31		    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	nop.i	0			};;

{ .mib;	mov	r25=loc4		    // Xupdate
	dep.z	tmp5=loc5,5,27		}   // a<<5
{ .mib;	xor	tmp0=loc1,loc0
	add	tmp4=loc3,r15		};;
{ .mmi;	xor	tmp0=tmp0,loc2		    // F_20_39(b,c,d)=b^c^d
	add	loc4=loc4,tmp4		    // f+=e+K_20_39
	extr.u	tmp1=loc5,27,5		}   // a>>27
{ .mmi;	xor	tmp2=r26,r28	// +1
	xor	tmp3=r18,r23	// +1
	nop.i	0			};;
{ .mmi;	add	loc4=loc4,tmp0		    // f+=F_20_39(b,c,d)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc0=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc5,0x44		};; // see b in next iteration
{ .mii;	add	loc4=loc4,tmp1		    // f+=ROTATE(a,5)
	shrp	loc3=tmp2,tmp2,31		    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	nop.i	0			};;

{ .mib;	mov	r26=loc3		    // Xupdate
	dep.z	tmp5=loc4,5,27		}   // a<<5
{ .mib;	xor	tmp0=loc0,loc5
	add	tmp4=loc2,r15		};;
{ .mmi;	xor	tmp0=tmp0,loc1		    // F_20_39(b,c,d)=b^c^d
	add	loc3=loc3,tmp4		    // f+=e+K_20_39
	extr.u	tmp1=loc4,27,5		}   // a>>27
{ .mmi;	xor	tmp2=r27,r29	// +1
	xor	tmp3=r19,r24	// +1
	nop.i	0			};;
{ .mmi;	add	loc3=loc3,tmp0		    // f+=F_20_39(b,c,d)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc5=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc4,0x44		};; // see b in next iteration
{ .mii;	add	loc3=loc3,tmp1		    // f+=ROTATE(a,5)
	shrp	loc2=tmp2,tmp2,31		    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	nop.i	0			};;

{ .mib;	mov	r27=loc2		    // Xupdate
	dep.z	tmp5=loc3,5,27		}   // a<<5
{ .mib;	xor	tmp0=loc5,loc4
	add	tmp4=loc1,r15		};;
{ .mmi;	xor	tmp0=tmp0,loc0		    // F_20_39(b,c,d)=b^c^d
	add	loc2=loc2,tmp4		    // f+=e+K_20_39
	extr.u	tmp1=loc3,27,5		}   // a>>27
{ .mmi;	xor	tmp2=r28,r30	// +1
	xor	tmp3=r20,r25	// +1
	nop.i	0			};;
{ .mmi;	add	loc2=loc2,tmp0		    // f+=F_20_39(b,c,d)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc4=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc3,0x44		};; // see b in next iteration
{ .mii;	add	loc2=loc2,tmp1		    // f+=ROTATE(a,5)
	shrp	loc1=tmp2,tmp2,31		    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	nop.i	0			};;

{ .mib;	mov	r28=loc1		    // Xupdate
	dep.z	tmp5=loc2,5,27		}   // a<<5
{ .mib;	xor	tmp0=loc4,loc3
	add	tmp4=loc0,r15		};;
{ .mmi;	xor	tmp0=tmp0,loc5		    // F_20_39(b,c,d)=b^c^d
	add	loc1=loc1,tmp4		    // f+=e+K_20_39
	extr.u	tmp1=loc2,27,5		}   // a>>27
{ .mmi;	xor	tmp2=r29,r31	// +1
	xor	tmp3=r21,r26	// +1
	nop.i	0			};;
{ .mmi;	add	loc1=loc1,tmp0		    // f+=F_20_39(b,c,d)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc3=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc2,0x44		};; // see b in next iteration
{ .mii;	add	loc1=loc1,tmp1		    // f+=ROTATE(a,5)
	shrp	loc0=tmp2,tmp2,31		    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	nop.i	0			};;

{ .mib;	mov	r29=loc0		    // Xupdate
	dep.z	tmp5=loc1,5,27		}   // a<<5
{ .mib;	xor	tmp0=loc3,loc2
	add	tmp4=loc5,r15		};;
{ .mmi;	xor	tmp0=tmp0,loc4		    // F_20_39(b,c,d)=b^c^d
	add	loc0=loc0,tmp4		    // f+=e+K_20_39
	extr.u	tmp1=loc1,27,5		}   // a>>27
{ .mmi;	xor	tmp2=r30,r16	// +1
	xor	tmp3=r22,r27	// +1
	nop.i	0			};;
{ .mmi;	add	loc0=loc0,tmp0		    // f+=F_20_39(b,c,d)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc2=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc1,0x44		};; // see b in next iteration
{ .mii;	add	loc0=loc0,tmp1		    // f+=ROTATE(a,5)
	shrp	loc5=tmp2,tmp2,31		    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	nop.i	0			};;

{ .mib;	mov	r30=loc5		    // Xupdate
	dep.z	tmp5=loc0,5,27		}   // a<<5
{ .mib;	xor	tmp0=loc2,loc1
	add	tmp4=loc4,r15		};;
{ .mmi;	xor	tmp0=tmp0,loc3		    // F_20_39(b,c,d)=b^c^d
	add	loc5=loc5,tmp4		    // f+=e+K_20_39
	extr.u	tmp1=loc0,27,5		}   // a>>27
{ .mmi;	xor	tmp2=r31,r17	// +1
	xor	tmp3=r23,r28	// +1
	nop.i	0			};;
{ .mmi;	add	loc5=loc5,tmp0		    // f+=F_20_39(b,c,d)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc1=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc0,0x44		};; // see b in next iteration
{ .mii;	add	loc5=loc5,tmp1		    // f+=ROTATE(a,5)
	shrp	loc4=tmp2,tmp2,31		    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	nop.i	0			};;

{ .mib;	mov	r31=loc4		    // Xupdate
	dep.z	tmp5=loc5,5,27		}   // a<<5
{ .mib;	xor	tmp0=loc1,loc0
	add	tmp4=loc3,r15		};;
{ .mmi;	xor	tmp0=tmp0,loc2		    // F_20_39(b,c,d)=b^c^d
	add	loc4=loc4,tmp4		    // f+=e+K_20_39
	extr.u	tmp1=loc5,27,5		}   // a>>27
{ .mmi;	xor	tmp2=r16,r18	// +1
	xor	tmp3=r24,r29	// +1
	nop.i	0			};;
{ .mmi;	add	loc4=loc4,tmp0		    // f+=F_20_39(b,c,d)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc0=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc5,0x44		};; // see b in next iteration
{ .mii;	add	loc4=loc4,tmp1		    // f+=ROTATE(a,5)
	shrp	loc3=tmp2,tmp2,31		    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	nop.i	0			};;

{ .mib;	mov	r16=loc3		    // Xupdate
	dep.z	tmp5=loc4,5,27		}   // a<<5
{ .mib;	xor	tmp0=loc0,loc5
	add	tmp4=loc2,r15		};;
{ .mmi;	xor	tmp0=tmp0,loc1		    // F_20_39(b,c,d)=b^c^d
	add	loc3=loc3,tmp4		    // f+=e+K_20_39
	extr.u	tmp1=loc4,27,5		}   // a>>27
{ .mmi;	xor	tmp2=r17,r19	// +1
	xor	tmp3=r25,r30	// +1
	nop.i	0			};;
{ .mmi;	add	loc3=loc3,tmp0		    // f+=F_20_39(b,c,d)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc5=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc4,0x44		};; // see b in next iteration
{ .mii;	add	loc3=loc3,tmp1		    // f+=ROTATE(a,5)
	shrp	loc2=tmp2,tmp2,31		    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	nop.i	0			};;

{ .mib;	mov	r17=loc2		    // Xupdate
	dep.z	tmp5=loc3,5,27		}   // a<<5
{ .mib;	xor	tmp0=loc5,loc4
	add	tmp4=loc1,r15		};;
{ .mmi;	xor	tmp0=tmp0,loc0		    // F_20_39(b,c,d)=b^c^d
	add	loc2=loc2,tmp4		    // f+=e+K_20_39
	extr.u	tmp1=loc3,27,5		}   // a>>27
{ .mmi;	xor	tmp2=r18,r20	// +1
	xor	tmp3=r26,r31	// +1
	nop.i	0			};;
{ .mmi;	add	loc2=loc2,tmp0		    // f+=F_20_39(b,c,d)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc4=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc3,0x44		};; // see b in next iteration
{ .mii;	add	loc2=loc2,tmp1		    // f+=ROTATE(a,5)
	shrp	loc1=tmp2,tmp2,31		    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	nop.i	0			};;

{ .mib;	mov	r18=loc1		    // Xupdate
	dep.z	tmp5=loc2,5,27		}   // a<<5
{ .mib;	xor	tmp0=loc4,loc3
	add	tmp4=loc0,r15		};;
{ .mmi;	xor	tmp0=tmp0,loc5		    // F_20_39(b,c,d)=b^c^d
	add	loc1=loc1,tmp4		    // f+=e+K_20_39
	extr.u	tmp1=loc2,27,5		}   // a>>27
{ .mmi;	xor	tmp2=r19,r21	// +1
	xor	tmp3=r27,r16	// +1
	nop.i	0			};;
{ .mmi;	add	loc1=loc1,tmp0		    // f+=F_20_39(b,c,d)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc3=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc2,0x44		};; // see b in next iteration
{ .mii;	add	loc1=loc1,tmp1		    // f+=ROTATE(a,5)
	shrp	loc0=tmp2,tmp2,31		    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	nop.i	0			};;

{ .mib;	mov	r19=loc0		    // Xupdate
	dep.z	tmp5=loc1,5,27		}   // a<<5
{ .mib;	xor	tmp0=loc3,loc2
	add	tmp4=loc5,r15		};;
{ .mmi;	xor	tmp0=tmp0,loc4		    // F_20_39(b,c,d)=b^c^d
	add	loc0=loc0,tmp4		    // f+=e+K_20_39
	extr.u	tmp1=loc1,27,5		}   // a>>27
{ .mmi;	xor	tmp2=r20,r22	// +1
	xor	tmp3=r28,r17	// +1
	nop.i	0			};;
{ .mmi;	add	loc0=loc0,tmp0		    // f+=F_20_39(b,c,d)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc2=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc1,0x44		};; // see b in next iteration
{ .mii;	add	loc0=loc0,tmp1		    // f+=ROTATE(a,5)
	shrp	loc5=tmp2,tmp2,31		    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	nop.i	0			};;

{ .mib;	mov	r20=loc5		    // Xupdate
	dep.z	tmp5=loc0,5,27		}   // a<<5
{ .mib;	xor	tmp0=loc2,loc1
	add	tmp4=loc4,r15		};;
{ .mmi;	xor	tmp0=tmp0,loc3		    // F_20_39(b,c,d)=b^c^d
	add	loc5=loc5,tmp4		    // f+=e+K_20_39
	extr.u	tmp1=loc0,27,5		}   // a>>27
{ .mmi;	xor	tmp2=r21,r23	// +1
	xor	tmp3=r29,r18	// +1
	nop.i	0			};;
{ .mmi;	add	loc5=loc5,tmp0		    // f+=F_20_39(b,c,d)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc1=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc0,0x44		};; // see b in next iteration
{ .mii;	add	loc5=loc5,tmp1		    // f+=ROTATE(a,5)
	shrp	loc4=tmp2,tmp2,31		    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	nop.i	0			};;

{ .mib;	mov	r21=loc4		    // Xupdate
	dep.z	tmp5=loc5,5,27		}   // a<<5
{ .mib;	xor	tmp0=loc1,loc0
	add	tmp4=loc3,r15		};;
{ .mmi;	xor	tmp0=tmp0,loc2		    // F_20_39(b,c,d)=b^c^d
	add	loc4=loc4,tmp4		    // f+=e+K_20_39
	extr.u	tmp1=loc5,27,5		}   // a>>27
{ .mmi;	xor	tmp2=r22,r24	// +1
	xor	tmp3=r30,r19	// +1
	nop.i	0			};;
{ .mmi;	add	loc4=loc4,tmp0		    // f+=F_20_39(b,c,d)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc0=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc5,0x44		};; // see b in next iteration
{ .mii;	add	loc4=loc4,tmp1		    // f+=ROTATE(a,5)
	shrp	loc3=tmp2,tmp2,31		    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	nop.i	0			};;

{ .mib;	mov	r22=loc3		    // Xupdate
	dep.z	tmp5=loc4,5,27		}   // a<<5
{ .mib;	xor	tmp0=loc0,loc5
	add	tmp4=loc2,r15		};;
{ .mmi;	xor	tmp0=tmp0,loc1		    // F_20_39(b,c,d)=b^c^d
	add	loc3=loc3,tmp4		    // f+=e+K_20_39
	extr.u	tmp1=loc4,27,5		}   // a>>27
{ .mmi;	xor	tmp2=r23,r25	// +1
	xor	tmp3=r31,r20	// +1
	nop.i	0			};;
{ .mmi;	add	loc3=loc3,tmp0		    // f+=F_20_39(b,c,d)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc5=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc4,0x44		};; // see b in next iteration
{ .mii;	add	loc3=loc3,tmp1		    // f+=ROTATE(a,5)
	shrp	loc2=tmp2,tmp2,31		    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	nop.i	0			};;

{ .mib;	mov	r23=loc2		    // Xupdate
	dep.z	tmp5=loc3,5,27		}   // a<<5
{ .mib;	xor	tmp0=loc5,loc4
	add	tmp4=loc1,r15		};;
{ .mmi;	xor	tmp0=tmp0,loc0		    // F_20_39(b,c,d)=b^c^d
	add	loc2=loc2,tmp4		    // f+=e+K_20_39
	extr.u	tmp1=loc3,27,5		}   // a>>27
{ .mmi;	xor	tmp2=r24,r26	// +1
	xor	tmp3=r16,r21	// +1
	nop.i	0			};;
{ .mmi;	add	loc2=loc2,tmp0		    // f+=F_20_39(b,c,d)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc4=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc3,0x44		};; // see b in next iteration
{ .mii;	add	loc2=loc2,tmp1		    // f+=ROTATE(a,5)
	shrp	loc1=tmp2,tmp2,31		    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	nop.i	0			};;

{ .mmi;	mov	r24=loc1		    // Xupdate
	and	tmp0=loc4,loc3
	dep.z	tmp5=loc2,5,27		}   // a<<5
{ .mmi;	and	tmp1=loc5,loc3
	add	tmp4=loc0,loc11	};;
{ .mmi;	or	tmp0=tmp0,tmp1		    // (b&c)|(b&d)
	add	loc1=loc1,tmp4		    // f+=e+K_40_59
	extr.u	tmp1=loc2,27,5		}   // a>>27
{ .mmi;	and	tmp4=loc4,loc5
	xor	tmp2=r25,r27	// +1
	xor	tmp3=r17,r22	// +1
	};;
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc3=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp0=tmp0,tmp4		    // F_40_59(b,c,d)=(b&c)|(b&d)|(c&d)
	mux2	tmp6=loc2,0x44		};; // see b in next iteration
{ .mii;	add	loc1=loc1,tmp0		    // f+=F_40_59(b,c,d)
	shrp	loc0=tmp2,tmp2,31;;	    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	add	loc1=loc1,tmp1		};; // f+=ROTATE(a,5)

{ .mmi;	mov	r25=loc0		    // Xupdate
	and	tmp0=loc3,loc2
	dep.z	tmp5=loc1,5,27		}   // a<<5
{ .mmi;	and	tmp1=loc4,loc2
	add	tmp4=loc5,loc11	};;
{ .mmi;	or	tmp0=tmp0,tmp1		    // (b&c)|(b&d)
	add	loc0=loc0,tmp4		    // f+=e+K_40_59
	extr.u	tmp1=loc1,27,5		}   // a>>27
{ .mmi;	and	tmp4=loc3,loc4
	xor	tmp2=r26,r28	// +1
	xor	tmp3=r18,r23	// +1
	};;
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc2=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp0=tmp0,tmp4		    // F_40_59(b,c,d)=(b&c)|(b&d)|(c&d)
	mux2	tmp6=loc1,0x44		};; // see b in next iteration
{ .mii;	add	loc0=loc0,tmp0		    // f+=F_40_59(b,c,d)
	shrp	loc5=tmp2,tmp2,31;;	    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	add	loc0=loc0,tmp1		};; // f+=ROTATE(a,5)

{ .mmi;	mov	r26=loc5		    // Xupdate
	and	tmp0=loc2,loc1
	dep.z	tmp5=loc0,5,27		}   // a<<5
{ .mmi;	and	tmp1=loc3,loc1
	add	tmp4=loc4,loc11	};;
{ .mmi;	or	tmp0=tmp0,tmp1		    // (b&c)|(b&d)
	add	loc5=loc5,tmp4		    // f+=e+K_40_59
	extr.u	tmp1=loc0,27,5		}   // a>>27
{ .mmi;	and	tmp4=loc2,loc3
	xor	tmp2=r27,r29	// +1
	xor	tmp3=r19,r24	// +1
	};;
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc1=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp0=tmp0,tmp4		    // F_40_59(b,c,d)=(b&c)|(b&d)|(c&d)
	mux2	tmp6=loc0,0x44		};; // see b in next iteration
{ .mii;	add	loc5=loc5,tmp0		    // f+=F_40_59(b,c,d)
	shrp	loc4=tmp2,tmp2,31;;	    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	add	loc5=loc5,tmp1		};; // f+=ROTATE(a,5)

{ .mmi;	mov	r27=loc4		    // Xupdate
	and	tmp0=loc1,loc0
	dep.z	tmp5=loc5,5,27		}   // a<<5
{ .mmi;	and	tmp1=loc2,loc0
	add	tmp4=loc3,loc11	};;
{ .mmi;	or	tmp0=tmp0,tmp1		    // (b&c)|(b&d)
	add	loc4=loc4,tmp4		    // f+=e+K_40_59
	extr.u	tmp1=loc5,27,5		}   // a>>27
{ .mmi;	and	tmp4=loc1,loc2
	xor	tmp2=r28,r30	// +1
	xor	tmp3=r20,r25	// +1
	};;
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc0=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp0=tmp0,tmp4		    // F_40_59(b,c,d)=(b&c)|(b&d)|(c&d)
	mux2	tmp6=loc5,0x44		};; // see b in next iteration
{ .mii;	add	loc4=loc4,tmp0		    // f+=F_40_59(b,c,d)
	shrp	loc3=tmp2,tmp2,31;;	    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	add	loc4=loc4,tmp1		};; // f+=ROTATE(a,5)

{ .mmi;	mov	r28=loc3		    // Xupdate
	and	tmp0=loc0,loc5
	dep.z	tmp5=loc4,5,27		}   // a<<5
{ .mmi;	and	tmp1=loc1,loc5
	add	tmp4=loc2,loc11	};;
{ .mmi;	or	tmp0=tmp0,tmp1		    // (b&c)|(b&d)
	add	loc3=loc3,tmp4		    // f+=e+K_40_59
	extr.u	tmp1=loc4,27,5		}   // a>>27
{ .mmi;	and	tmp4=loc0,loc1
	xor	tmp2=r29,r31	// +1
	xor	tmp3=r21,r26	// +1
	};;
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc5=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp0=tmp0,tmp4		    // F_40_59(b,c,d)=(b&c)|(b&d)|(c&d)
	mux2	tmp6=loc4,0x44		};; // see b in next iteration
{ .mii;	add	loc3=loc3,tmp0		    // f+=F_40_59(b,c,d)
	shrp	loc2=tmp2,tmp2,31;;	    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	add	loc3=loc3,tmp1		};; // f+=ROTATE(a,5)

{ .mmi;	mov	r29=loc2		    // Xupdate
	and	tmp0=loc5,loc4
	dep.z	tmp5=loc3,5,27		}   // a<<5
{ .mmi;	and	tmp1=loc0,loc4
	add	tmp4=loc1,loc11	};;
{ .mmi;	or	tmp0=tmp0,tmp1		    // (b&c)|(b&d)
	add	loc2=loc2,tmp4		    // f+=e+K_40_59
	extr.u	tmp1=loc3,27,5		}   // a>>27
{ .mmi;	and	tmp4=loc5,loc0
	xor	tmp2=r30,r16	// +1
	xor	tmp3=r22,r27	// +1
	};;
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc4=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp0=tmp0,tmp4		    // F_40_59(b,c,d)=(b&c)|(b&d)|(c&d)
	mux2	tmp6=loc3,0x44		};; // see b in next iteration
{ .mii;	add	loc2=loc2,tmp0		    // f+=F_40_59(b,c,d)
	shrp	loc1=tmp2,tmp2,31;;	    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	add	loc2=loc2,tmp1		};; // f+=ROTATE(a,5)

{ .mmi;	mov	r30=loc1		    // Xupdate
	and	tmp0=loc4,loc3
	dep.z	tmp5=loc2,5,27		}   // a<<5
{ .mmi;	and	tmp1=loc5,loc3
	add	tmp4=loc0,loc11	};;
{ .mmi;	or	tmp0=tmp0,tmp1		    // (b&c)|(b&d)
	add	loc1=loc1,tmp4		    // f+=e+K_40_59
	extr.u	tmp1=loc2,27,5		}   // a>>27
{ .mmi;	and	tmp4=loc4,loc5
	xor	tmp2=r31,r17	// +1
	xor	tmp3=r23,r28	// +1
	};;
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc3=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp0=tmp0,tmp4		    // F_40_59(b,c,d)=(b&c)|(b&d)|(c&d)
	mux2	tmp6=loc2,0x44		};; // see b in next iteration
{ .mii;	add	loc1=loc1,tmp0		    // f+=F_40_59(b,c,d)
	shrp	loc0=tmp2,tmp2,31;;	    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	add	loc1=loc1,tmp1		};; // f+=ROTATE(a,5)

{ .mmi;	mov	r31=loc0		    // Xupdate
	and	tmp0=loc3,loc2
	dep.z	tmp5=loc1,5,27		}   // a<<5
{ .mmi;	and	tmp1=loc4,loc2
	add	tmp4=loc5,loc11	};;
{ .mmi;	or	tmp0=tmp0,tmp1		    // (b&c)|(b&d)
	add	loc0=loc0,tmp4		    // f+=e+K_40_59
	extr.u	tmp1=loc1,27,5		}   // a>>27
{ .mmi;	and	tmp4=loc3,loc4
	xor	tmp2=r16,r18	// +1
	xor	tmp3=r24,r29	// +1
	};;
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc2=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp0=tmp0,tmp4		    // F_40_59(b,c,d)=(b&c)|(b&d)|(c&d)
	mux2	tmp6=loc1,0x44		};; // see b in next iteration
{ .mii;	add	loc0=loc0,tmp0		    // f+=F_40_59(b,c,d)
	shrp	loc5=tmp2,tmp2,31;;	    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	add	loc0=loc0,tmp1		};; // f+=ROTATE(a,5)

{ .mmi;	mov	r16=loc5		    // Xupdate
	and	tmp0=loc2,loc1
	dep.z	tmp5=loc0,5,27		}   // a<<5
{ .mmi;	and	tmp1=loc3,loc1
	add	tmp4=loc4,loc11	};;
{ .mmi;	or	tmp0=tmp0,tmp1		    // (b&c)|(b&d)
	add	loc5=loc5,tmp4		    // f+=e+K_40_59
	extr.u	tmp1=loc0,27,5		}   // a>>27
{ .mmi;	and	tmp4=loc2,loc3
	xor	tmp2=r17,r19	// +1
	xor	tmp3=r25,r30	// +1
	};;
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc1=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp0=tmp0,tmp4		    // F_40_59(b,c,d)=(b&c)|(b&d)|(c&d)
	mux2	tmp6=loc0,0x44		};; // see b in next iteration
{ .mii;	add	loc5=loc5,tmp0		    // f+=F_40_59(b,c,d)
	shrp	loc4=tmp2,tmp2,31;;	    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	add	loc5=loc5,tmp1		};; // f+=ROTATE(a,5)

{ .mmi;	mov	r17=loc4		    // Xupdate
	and	tmp0=loc1,loc0
	dep.z	tmp5=loc5,5,27		}   // a<<5
{ .mmi;	and	tmp1=loc2,loc0
	add	tmp4=loc3,loc11	};;
{ .mmi;	or	tmp0=tmp0,tmp1		    // (b&c)|(b&d)
	add	loc4=loc4,tmp4		    // f+=e+K_40_59
	extr.u	tmp1=loc5,27,5		}   // a>>27
{ .mmi;	and	tmp4=loc1,loc2
	xor	tmp2=r18,r20	// +1
	xor	tmp3=r26,r31	// +1
	};;
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc0=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp0=tmp0,tmp4		    // F_40_59(b,c,d)=(b&c)|(b&d)|(c&d)
	mux2	tmp6=loc5,0x44		};; // see b in next iteration
{ .mii;	add	loc4=loc4,tmp0		    // f+=F_40_59(b,c,d)
	shrp	loc3=tmp2,tmp2,31;;	    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	add	loc4=loc4,tmp1		};; // f+=ROTATE(a,5)

{ .mmi;	mov	r18=loc3		    // Xupdate
	and	tmp0=loc0,loc5
	dep.z	tmp5=loc4,5,27		}   // a<<5
{ .mmi;	and	tmp1=loc1,loc5
	add	tmp4=loc2,loc11	};;
{ .mmi;	or	tmp0=tmp0,tmp1		    // (b&c)|(b&d)
	add	loc3=loc3,tmp4		    // f+=e+K_40_59
	extr.u	tmp1=loc4,27,5		}   // a>>27
{ .mmi;	and	tmp4=loc0,loc1
	xor	tmp2=r19,r21	// +1
	xor	tmp3=r27,r16	// +1
	};;
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc5=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp0=tmp0,tmp4		    // F_40_59(b,c,d)=(b&c)|(b&d)|(c&d)
	mux2	tmp6=loc4,0x44		};; // see b in next iteration
{ .mii;	add	loc3=loc3,tmp0		    // f+=F_40_59(b,c,d)
	shrp	loc2=tmp2,tmp2,31;;	    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	add	loc3=loc3,tmp1		};; // f+=ROTATE(a,5)

{ .mmi;	mov	r19=loc2		    // Xupdate
	and	tmp0=loc5,loc4
	dep.z	tmp5=loc3,5,27		}   // a<<5
{ .mmi;	and	tmp1=loc0,loc4
	add	tmp4=loc1,loc11	};;
{ .mmi;	or	tmp0=tmp0,tmp1		    // (b&c)|(b&d)
	add	loc2=loc2,tmp4		    // f+=e+K_40_59
	extr.u	tmp1=loc3,27,5		}   // a>>27
{ .mmi;	and	tmp4=loc5,loc0
	xor	tmp2=r20,r22	// +1
	xor	tmp3=r28,r17	// +1
	};;
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc4=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp0=tmp0,tmp4		    // F_40_59(b,c,d)=(b&c)|(b&d)|(c&d)
	mux2	tmp6=loc3,0x44		};; // see b in next iteration
{ .mii;	add	loc2=loc2,tmp0		    // f+=F_40_59(b,c,d)
	shrp	loc1=tmp2,tmp2,31;;	    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	add	loc2=loc2,tmp1		};; // f+=ROTATE(a,5)

{ .mmi;	mov	r20=loc1		    // Xupdate
	and	tmp0=loc4,loc3
	dep.z	tmp5=loc2,5,27		}   // a<<5
{ .mmi;	and	tmp1=loc5,loc3
	add	tmp4=loc0,loc11	};;
{ .mmi;	or	tmp0=tmp0,tmp1		    // (b&c)|(b&d)
	add	loc1=loc1,tmp4		    // f+=e+K_40_59
	extr.u	tmp1=loc2,27,5		}   // a>>27
{ .mmi;	and	tmp4=loc4,loc5
	xor	tmp2=r21,r23	// +1
	xor	tmp3=r29,r18	// +1
	};;
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc3=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp0=tmp0,tmp4		    // F_40_59(b,c,d)=(b&c)|(b&d)|(c&d)
	mux2	tmp6=loc2,0x44		};; // see b in next iteration
{ .mii;	add	loc1=loc1,tmp0		    // f+=F_40_59(b,c,d)
	shrp	loc0=tmp2,tmp2,31;;	    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	add	loc1=loc1,tmp1		};; // f+=ROTATE(a,5)

{ .mmi;	mov	r21=loc0		    // Xupdate
	and	tmp0=loc3,loc2
	dep.z	tmp5=loc1,5,27		}   // a<<5
{ .mmi;	and	tmp1=loc4,loc2
	add	tmp4=loc5,loc11	};;
{ .mmi;	or	tmp0=tmp0,tmp1		    // (b&c)|(b&d)
	add	loc0=loc0,tmp4		    // f+=e+K_40_59
	extr.u	tmp1=loc1,27,5		}   // a>>27
{ .mmi;	and	tmp4=loc3,loc4
	xor	tmp2=r22,r24	// +1
	xor	tmp3=r30,r19	// +1
	};;
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc2=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp0=tmp0,tmp4		    // F_40_59(b,c,d)=(b&c)|(b&d)|(c&d)
	mux2	tmp6=loc1,0x44		};; // see b in next iteration
{ .mii;	add	loc0=loc0,tmp0		    // f+=F_40_59(b,c,d)
	shrp	loc5=tmp2,tmp2,31;;	    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	add	loc0=loc0,tmp1		};; // f+=ROTATE(a,5)

{ .mmi;	mov	r22=loc5		    // Xupdate
	and	tmp0=loc2,loc1
	dep.z	tmp5=loc0,5,27		}   // a<<5
{ .mmi;	and	tmp1=loc3,loc1
	add	tmp4=loc4,loc11	};;
{ .mmi;	or	tmp0=tmp0,tmp1		    // (b&c)|(b&d)
	add	loc5=loc5,tmp4		    // f+=e+K_40_59
	extr.u	tmp1=loc0,27,5		}   // a>>27
{ .mmi;	and	tmp4=loc2,loc3
	xor	tmp2=r23,r25	// +1
	xor	tmp3=r31,r20	// +1
	};;
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc1=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp0=tmp0,tmp4		    // F_40_59(b,c,d)=(b&c)|(b&d)|(c&d)
	mux2	tmp6=loc0,0x44		};; // see b in next iteration
{ .mii;	add	loc5=loc5,tmp0		    // f+=F_40_59(b,c,d)
	shrp	loc4=tmp2,tmp2,31;;	    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	add	loc5=loc5,tmp1		};; // f+=ROTATE(a,5)

{ .mmi;	mov	r23=loc4		    // Xupdate
	and	tmp0=loc1,loc0
	dep.z	tmp5=loc5,5,27		}   // a<<5
{ .mmi;	and	tmp1=loc2,loc0
	add	tmp4=loc3,loc11	};;
{ .mmi;	or	tmp0=tmp0,tmp1		    // (b&c)|(b&d)
	add	loc4=loc4,tmp4		    // f+=e+K_40_59
	extr.u	tmp1=loc5,27,5		}   // a>>27
{ .mmi;	and	tmp4=loc1,loc2
	xor	tmp2=r24,r26	// +1
	xor	tmp3=r16,r21	// +1
	};;
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc0=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp0=tmp0,tmp4		    // F_40_59(b,c,d)=(b&c)|(b&d)|(c&d)
	mux2	tmp6=loc5,0x44		};; // see b in next iteration
{ .mii;	add	loc4=loc4,tmp0		    // f+=F_40_59(b,c,d)
	shrp	loc3=tmp2,tmp2,31;;	    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	add	loc4=loc4,tmp1		};; // f+=ROTATE(a,5)

{ .mmi;	mov	r24=loc3		    // Xupdate
	and	tmp0=loc0,loc5
	dep.z	tmp5=loc4,5,27		}   // a<<5
{ .mmi;	and	tmp1=loc1,loc5
	add	tmp4=loc2,loc11	};;
{ .mmi;	or	tmp0=tmp0,tmp1		    // (b&c)|(b&d)
	add	loc3=loc3,tmp4		    // f+=e+K_40_59
	extr.u	tmp1=loc4,27,5		}   // a>>27
{ .mmi;	and	tmp4=loc0,loc1
	xor	tmp2=r25,r27	// +1
	xor	tmp3=r17,r22	// +1
	};;
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc5=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp0=tmp0,tmp4		    // F_40_59(b,c,d)=(b&c)|(b&d)|(c&d)
	mux2	tmp6=loc4,0x44		};; // see b in next iteration
{ .mii;	add	loc3=loc3,tmp0		    // f+=F_40_59(b,c,d)
	shrp	loc2=tmp2,tmp2,31;;	    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	add	loc3=loc3,tmp1		};; // f+=ROTATE(a,5)

{ .mmi;	mov	r25=loc2		    // Xupdate
	and	tmp0=loc5,loc4
	dep.z	tmp5=loc3,5,27		}   // a<<5
{ .mmi;	and	tmp1=loc0,loc4
	add	tmp4=loc1,loc11	};;
{ .mmi;	or	tmp0=tmp0,tmp1		    // (b&c)|(b&d)
	add	loc2=loc2,tmp4		    // f+=e+K_40_59
	extr.u	tmp1=loc3,27,5		}   // a>>27
{ .mmi;	and	tmp4=loc5,loc0
	xor	tmp2=r26,r28	// +1
	xor	tmp3=r18,r23	// +1
	};;
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc4=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp0=tmp0,tmp4		    // F_40_59(b,c,d)=(b&c)|(b&d)|(c&d)
	mux2	tmp6=loc3,0x44		};; // see b in next iteration
{ .mii;	add	loc2=loc2,tmp0		    // f+=F_40_59(b,c,d)
	shrp	loc1=tmp2,tmp2,31;;	    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	add	loc2=loc2,tmp1		};; // f+=ROTATE(a,5)

{ .mmi;	mov	r26=loc1		    // Xupdate
	and	tmp0=loc4,loc3
	dep.z	tmp5=loc2,5,27		}   // a<<5
{ .mmi;	and	tmp1=loc5,loc3
	add	tmp4=loc0,loc11	};;
{ .mmi;	or	tmp0=tmp0,tmp1		    // (b&c)|(b&d)
	add	loc1=loc1,tmp4		    // f+=e+K_40_59
	extr.u	tmp1=loc2,27,5		}   // a>>27
{ .mmi;	and	tmp4=loc4,loc5
	xor	tmp2=r27,r29	// +1
	xor	tmp3=r19,r24	// +1
	};;
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc3=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp0=tmp0,tmp4		    // F_40_59(b,c,d)=(b&c)|(b&d)|(c&d)
	mux2	tmp6=loc2,0x44		};; // see b in next iteration
{ .mii;	add	loc1=loc1,tmp0		    // f+=F_40_59(b,c,d)
	shrp	loc0=tmp2,tmp2,31;;	    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	add	loc1=loc1,tmp1		};; // f+=ROTATE(a,5)

{ .mmi;	mov	r27=loc0		    // Xupdate
	and	tmp0=loc3,loc2
	dep.z	tmp5=loc1,5,27		}   // a<<5
{ .mmi;	and	tmp1=loc4,loc2
	add	tmp4=loc5,loc11	};;
{ .mmi;	or	tmp0=tmp0,tmp1		    // (b&c)|(b&d)
	add	loc0=loc0,tmp4		    // f+=e+K_40_59
	extr.u	tmp1=loc1,27,5		}   // a>>27
{ .mmi;	and	tmp4=loc3,loc4
	xor	tmp2=r28,r30	// +1
	xor	tmp3=r20,r25	// +1
	};;
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc2=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp0=tmp0,tmp4		    // F_40_59(b,c,d)=(b&c)|(b&d)|(c&d)
	mux2	tmp6=loc1,0x44		};; // see b in next iteration
{ .mii;	add	loc0=loc0,tmp0		    // f+=F_40_59(b,c,d)
	shrp	loc5=tmp2,tmp2,31;;	    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	add	loc0=loc0,tmp1		};; // f+=ROTATE(a,5)

{ .mib;	mov	r28=loc5		    // Xupdate
	dep.z	tmp5=loc0,5,27		}   // a<<5
{ .mib;	xor	tmp0=loc2,loc1
	add	tmp4=loc4,loc12		};;
{ .mmi;	xor	tmp0=tmp0,loc3		    // F_20_39(b,c,d)=b^c^d
	add	loc5=loc5,tmp4		    // f+=e+K_20_39
	extr.u	tmp1=loc0,27,5		}   // a>>27
{ .mmi;	xor	tmp2=r29,r31	// +1
	xor	tmp3=r21,r26	// +1
	nop.i	0			};;
{ .mmi;	add	loc5=loc5,tmp0		    // f+=F_20_39(b,c,d)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc1=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc0,0x44		};; // see b in next iteration
{ .mii;	add	loc5=loc5,tmp1		    // f+=ROTATE(a,5)
	shrp	loc4=tmp2,tmp2,31		    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	nop.i	0			};;

{ .mib;	mov	r29=loc4		    // Xupdate
	dep.z	tmp5=loc5,5,27		}   // a<<5
{ .mib;	xor	tmp0=loc1,loc0
	add	tmp4=loc3,loc12		};;
{ .mmi;	xor	tmp0=tmp0,loc2		    // F_20_39(b,c,d)=b^c^d
	add	loc4=loc4,tmp4		    // f+=e+K_20_39
	extr.u	tmp1=loc5,27,5		}   // a>>27
{ .mmi;	xor	tmp2=r30,r16	// +1
	xor	tmp3=r22,r27	// +1
	nop.i	0			};;
{ .mmi;	add	loc4=loc4,tmp0		    // f+=F_20_39(b,c,d)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc0=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc5,0x44		};; // see b in next iteration
{ .mii;	add	loc4=loc4,tmp1		    // f+=ROTATE(a,5)
	shrp	loc3=tmp2,tmp2,31		    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	nop.i	0			};;

{ .mib;	mov	r30=loc3		    // Xupdate
	dep.z	tmp5=loc4,5,27		}   // a<<5
{ .mib;	xor	tmp0=loc0,loc5
	add	tmp4=loc2,loc12		};;
{ .mmi;	xor	tmp0=tmp0,loc1		    // F_20_39(b,c,d)=b^c^d
	add	loc3=loc3,tmp4		    // f+=e+K_20_39
	extr.u	tmp1=loc4,27,5		}   // a>>27
{ .mmi;	xor	tmp2=r31,r17	// +1
	xor	tmp3=r23,r28	// +1
	nop.i	0			};;
{ .mmi;	add	loc3=loc3,tmp0		    // f+=F_20_39(b,c,d)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc5=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc4,0x44		};; // see b in next iteration
{ .mii;	add	loc3=loc3,tmp1		    // f+=ROTATE(a,5)
	shrp	loc2=tmp2,tmp2,31		    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	nop.i	0			};;

{ .mib;	mov	r31=loc2		    // Xupdate
	dep.z	tmp5=loc3,5,27		}   // a<<5
{ .mib;	xor	tmp0=loc5,loc4
	add	tmp4=loc1,loc12		};;
{ .mmi;	xor	tmp0=tmp0,loc0		    // F_20_39(b,c,d)=b^c^d
	add	loc2=loc2,tmp4		    // f+=e+K_20_39
	extr.u	tmp1=loc3,27,5		}   // a>>27
{ .mmi;	xor	tmp2=r16,r18	// +1
	xor	tmp3=r24,r29	// +1
	nop.i	0			};;
{ .mmi;	add	loc2=loc2,tmp0		    // f+=F_20_39(b,c,d)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc4=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc3,0x44		};; // see b in next iteration
{ .mii;	add	loc2=loc2,tmp1		    // f+=ROTATE(a,5)
	shrp	loc1=tmp2,tmp2,31		    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	nop.i	0			};;

{ .mib;	mov	r16=loc1		    // Xupdate
	dep.z	tmp5=loc2,5,27		}   // a<<5
{ .mib;	xor	tmp0=loc4,loc3
	add	tmp4=loc0,loc12		};;
{ .mmi;	xor	tmp0=tmp0,loc5		    // F_20_39(b,c,d)=b^c^d
	add	loc1=loc1,tmp4		    // f+=e+K_20_39
	extr.u	tmp1=loc2,27,5		}   // a>>27
{ .mmi;	xor	tmp2=r17,r19	// +1
	xor	tmp3=r25,r30	// +1
	nop.i	0			};;
{ .mmi;	add	loc1=loc1,tmp0		    // f+=F_20_39(b,c,d)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc3=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc2,0x44		};; // see b in next iteration
{ .mii;	add	loc1=loc1,tmp1		    // f+=ROTATE(a,5)
	shrp	loc0=tmp2,tmp2,31		    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	nop.i	0			};;

{ .mib;	mov	r17=loc0		    // Xupdate
	dep.z	tmp5=loc1,5,27		}   // a<<5
{ .mib;	xor	tmp0=loc3,loc2
	add	tmp4=loc5,loc12		};;
{ .mmi;	xor	tmp0=tmp0,loc4		    // F_20_39(b,c,d)=b^c^d
	add	loc0=loc0,tmp4		    // f+=e+K_20_39
	extr.u	tmp1=loc1,27,5		}   // a>>27
{ .mmi;	xor	tmp2=r18,r20	// +1
	xor	tmp3=r26,r31	// +1
	nop.i	0			};;
{ .mmi;	add	loc0=loc0,tmp0		    // f+=F_20_39(b,c,d)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc2=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc1,0x44		};; // see b in next iteration
{ .mii;	add	loc0=loc0,tmp1		    // f+=ROTATE(a,5)
	shrp	loc5=tmp2,tmp2,31		    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	nop.i	0			};;

{ .mib;	mov	r18=loc5		    // Xupdate
	dep.z	tmp5=loc0,5,27		}   // a<<5
{ .mib;	xor	tmp0=loc2,loc1
	add	tmp4=loc4,loc12		};;
{ .mmi;	xor	tmp0=tmp0,loc3		    // F_20_39(b,c,d)=b^c^d
	add	loc5=loc5,tmp4		    // f+=e+K_20_39
	extr.u	tmp1=loc0,27,5		}   // a>>27
{ .mmi;	xor	tmp2=r19,r21	// +1
	xor	tmp3=r27,r16	// +1
	nop.i	0			};;
{ .mmi;	add	loc5=loc5,tmp0		    // f+=F_20_39(b,c,d)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc1=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc0,0x44		};; // see b in next iteration
{ .mii;	add	loc5=loc5,tmp1		    // f+=ROTATE(a,5)
	shrp	loc4=tmp2,tmp2,31		    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	nop.i	0			};;

{ .mib;	mov	r19=loc4		    // Xupdate
	dep.z	tmp5=loc5,5,27		}   // a<<5
{ .mib;	xor	tmp0=loc1,loc0
	add	tmp4=loc3,loc12		};;
{ .mmi;	xor	tmp0=tmp0,loc2		    // F_20_39(b,c,d)=b^c^d
	add	loc4=loc4,tmp4		    // f+=e+K_20_39
	extr.u	tmp1=loc5,27,5		}   // a>>27
{ .mmi;	xor	tmp2=r20,r22	// +1
	xor	tmp3=r28,r17	// +1
	nop.i	0			};;
{ .mmi;	add	loc4=loc4,tmp0		    // f+=F_20_39(b,c,d)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc0=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc5,0x44		};; // see b in next iteration
{ .mii;	add	loc4=loc4,tmp1		    // f+=ROTATE(a,5)
	shrp	loc3=tmp2,tmp2,31		    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	nop.i	0			};;

{ .mib;	mov	r20=loc3		    // Xupdate
	dep.z	tmp5=loc4,5,27		}   // a<<5
{ .mib;	xor	tmp0=loc0,loc5
	add	tmp4=loc2,loc12		};;
{ .mmi;	xor	tmp0=tmp0,loc1		    // F_20_39(b,c,d)=b^c^d
	add	loc3=loc3,tmp4		    // f+=e+K_20_39
	extr.u	tmp1=loc4,27,5		}   // a>>27
{ .mmi;	xor	tmp2=r21,r23	// +1
	xor	tmp3=r29,r18	// +1
	nop.i	0			};;
{ .mmi;	add	loc3=loc3,tmp0		    // f+=F_20_39(b,c,d)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc5=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc4,0x44		};; // see b in next iteration
{ .mii;	add	loc3=loc3,tmp1		    // f+=ROTATE(a,5)
	shrp	loc2=tmp2,tmp2,31		    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	nop.i	0			};;

{ .mib;	mov	r21=loc2		    // Xupdate
	dep.z	tmp5=loc3,5,27		}   // a<<5
{ .mib;	xor	tmp0=loc5,loc4
	add	tmp4=loc1,loc12		};;
{ .mmi;	xor	tmp0=tmp0,loc0		    // F_20_39(b,c,d)=b^c^d
	add	loc2=loc2,tmp4		    // f+=e+K_20_39
	extr.u	tmp1=loc3,27,5		}   // a>>27
{ .mmi;	xor	tmp2=r22,r24	// +1
	xor	tmp3=r30,r19	// +1
	nop.i	0			};;
{ .mmi;	add	loc2=loc2,tmp0		    // f+=F_20_39(b,c,d)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc4=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc3,0x44		};; // see b in next iteration
{ .mii;	add	loc2=loc2,tmp1		    // f+=ROTATE(a,5)
	shrp	loc1=tmp2,tmp2,31		    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	nop.i	0			};;

{ .mib;	mov	r22=loc1		    // Xupdate
	dep.z	tmp5=loc2,5,27		}   // a<<5
{ .mib;	xor	tmp0=loc4,loc3
	add	tmp4=loc0,loc12		};;
{ .mmi;	xor	tmp0=tmp0,loc5		    // F_20_39(b,c,d)=b^c^d
	add	loc1=loc1,tmp4		    // f+=e+K_20_39
	extr.u	tmp1=loc2,27,5		}   // a>>27
{ .mmi;	xor	tmp2=r23,r25	// +1
	xor	tmp3=r31,r20	// +1
	nop.i	0			};;
{ .mmi;	add	loc1=loc1,tmp0		    // f+=F_20_39(b,c,d)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc3=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc2,0x44		};; // see b in next iteration
{ .mii;	add	loc1=loc1,tmp1		    // f+=ROTATE(a,5)
	shrp	loc0=tmp2,tmp2,31		    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	nop.i	0			};;

{ .mib;	mov	r23=loc0		    // Xupdate
	dep.z	tmp5=loc1,5,27		}   // a<<5
{ .mib;	xor	tmp0=loc3,loc2
	add	tmp4=loc5,loc12		};;
{ .mmi;	xor	tmp0=tmp0,loc4		    // F_20_39(b,c,d)=b^c^d
	add	loc0=loc0,tmp4		    // f+=e+K_20_39
	extr.u	tmp1=loc1,27,5		}   // a>>27
{ .mmi;	xor	tmp2=r24,r26	// +1
	xor	tmp3=r16,r21	// +1
	nop.i	0			};;
{ .mmi;	add	loc0=loc0,tmp0		    // f+=F_20_39(b,c,d)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc2=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc1,0x44		};; // see b in next iteration
{ .mii;	add	loc0=loc0,tmp1		    // f+=ROTATE(a,5)
	shrp	loc5=tmp2,tmp2,31		    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	nop.i	0			};;

{ .mib;	mov	r24=loc5		    // Xupdate
	dep.z	tmp5=loc0,5,27		}   // a<<5
{ .mib;	xor	tmp0=loc2,loc1
	add	tmp4=loc4,loc12		};;
{ .mmi;	xor	tmp0=tmp0,loc3		    // F_20_39(b,c,d)=b^c^d
	add	loc5=loc5,tmp4		    // f+=e+K_20_39
	extr.u	tmp1=loc0,27,5		}   // a>>27
{ .mmi;	xor	tmp2=r25,r27	// +1
	xor	tmp3=r17,r22	// +1
	nop.i	0			};;
{ .mmi;	add	loc5=loc5,tmp0		    // f+=F_20_39(b,c,d)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc1=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc0,0x44		};; // see b in next iteration
{ .mii;	add	loc5=loc5,tmp1		    // f+=ROTATE(a,5)
	shrp	loc4=tmp2,tmp2,31		    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	nop.i	0			};;

{ .mib;	mov	r25=loc4		    // Xupdate
	dep.z	tmp5=loc5,5,27		}   // a<<5
{ .mib;	xor	tmp0=loc1,loc0
	add	tmp4=loc3,loc12		};;
{ .mmi;	xor	tmp0=tmp0,loc2		    // F_20_39(b,c,d)=b^c^d
	add	loc4=loc4,tmp4		    // f+=e+K_20_39
	extr.u	tmp1=loc5,27,5		}   // a>>27
{ .mmi;	xor	tmp2=r26,r28	// +1
	xor	tmp3=r18,r23	// +1
	nop.i	0			};;
{ .mmi;	add	loc4=loc4,tmp0		    // f+=F_20_39(b,c,d)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc0=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc5,0x44		};; // see b in next iteration
{ .mii;	add	loc4=loc4,tmp1		    // f+=ROTATE(a,5)
	shrp	loc3=tmp2,tmp2,31		    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	nop.i	0			};;

{ .mib;	mov	r26=loc3		    // Xupdate
	dep.z	tmp5=loc4,5,27		}   // a<<5
{ .mib;	xor	tmp0=loc0,loc5
	add	tmp4=loc2,loc12		};;
{ .mmi;	xor	tmp0=tmp0,loc1		    // F_20_39(b,c,d)=b^c^d
	add	loc3=loc3,tmp4		    // f+=e+K_20_39
	extr.u	tmp1=loc4,27,5		}   // a>>27
{ .mmi;	xor	tmp2=r27,r29	// +1
	xor	tmp3=r19,r24	// +1
	nop.i	0			};;
{ .mmi;	add	loc3=loc3,tmp0		    // f+=F_20_39(b,c,d)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc5=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc4,0x44		};; // see b in next iteration
{ .mii;	add	loc3=loc3,tmp1		    // f+=ROTATE(a,5)
	shrp	loc2=tmp2,tmp2,31		    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	nop.i	0			};;

{ .mib;	mov	r27=loc2		    // Xupdate
	dep.z	tmp5=loc3,5,27		}   // a<<5
{ .mib;	xor	tmp0=loc5,loc4
	add	tmp4=loc1,loc12		};;
{ .mmi;	xor	tmp0=tmp0,loc0		    // F_20_39(b,c,d)=b^c^d
	add	loc2=loc2,tmp4		    // f+=e+K_20_39
	extr.u	tmp1=loc3,27,5		}   // a>>27
{ .mmi;	xor	tmp2=r28,r30	// +1
	xor	tmp3=r20,r25	// +1
	nop.i	0			};;
{ .mmi;	add	loc2=loc2,tmp0		    // f+=F_20_39(b,c,d)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc4=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc3,0x44		};; // see b in next iteration
{ .mii;	add	loc2=loc2,tmp1		    // f+=ROTATE(a,5)
	shrp	loc1=tmp2,tmp2,31		    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	nop.i	0			};;

{ .mib;	mov	r28=loc1		    // Xupdate
	dep.z	tmp5=loc2,5,27		}   // a<<5
{ .mib;	xor	tmp0=loc4,loc3
	add	tmp4=loc0,loc12		};;
{ .mmi;	xor	tmp0=tmp0,loc5		    // F_20_39(b,c,d)=b^c^d
	add	loc1=loc1,tmp4		    // f+=e+K_20_39
	extr.u	tmp1=loc2,27,5		}   // a>>27
{ .mmi;	xor	tmp2=r29,r31	// +1
	xor	tmp3=r21,r26	// +1
	nop.i	0			};;
{ .mmi;	add	loc1=loc1,tmp0		    // f+=F_20_39(b,c,d)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc3=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc2,0x44		};; // see b in next iteration
{ .mii;	add	loc1=loc1,tmp1		    // f+=ROTATE(a,5)
	shrp	loc0=tmp2,tmp2,31		    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	nop.i	0			};;

{ .mib;	mov	r29=loc0		    // Xupdate
	dep.z	tmp5=loc1,5,27		}   // a<<5
{ .mib;	xor	tmp0=loc3,loc2
	add	tmp4=loc5,loc12		};;
{ .mmi;	xor	tmp0=tmp0,loc4		    // F_20_39(b,c,d)=b^c^d
	add	loc0=loc0,tmp4		    // f+=e+K_20_39
	extr.u	tmp1=loc1,27,5		}   // a>>27
{ .mmi;	xor	tmp2=r30,r16	// +1
	xor	tmp3=r22,r27	// +1
	nop.i	0			};;
{ .mmi;	add	loc0=loc0,tmp0		    // f+=F_20_39(b,c,d)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc2=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc1,0x44		};; // see b in next iteration
{ .mii;	add	loc0=loc0,tmp1		    // f+=ROTATE(a,5)
	shrp	loc5=tmp2,tmp2,31		    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	nop.i	0			};;

{ .mib;	mov	r30=loc5		    // Xupdate
	dep.z	tmp5=loc0,5,27		}   // a<<5
{ .mib;	xor	tmp0=loc2,loc1
	add	tmp4=loc4,loc12		};;
{ .mmi;	xor	tmp0=tmp0,loc3		    // F_20_39(b,c,d)=b^c^d
	add	loc5=loc5,tmp4		    // f+=e+K_20_39
	extr.u	tmp1=loc0,27,5		}   // a>>27
{ .mmi;	xor	tmp2=r31,r17	// +1
	xor	tmp3=r23,r28	// +1
	nop.i	0			};;
{ .mmi;	add	loc5=loc5,tmp0		    // f+=F_20_39(b,c,d)
	xor	tmp2=tmp2,tmp3		    // +1
	shrp	loc1=tmp6,tmp6,2		}   // b=ROTATE(b,30)
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	mux2	tmp6=loc0,0x44		};; // see b in next iteration
{ .mii;	add	loc5=loc5,tmp1		    // f+=ROTATE(a,5)
	shrp	loc4=tmp2,tmp2,31		    // f+1=ROTATE(x[0]^x[2]^x[8]^x[13],1)
	nop.i	0			};;

{ .mib;	mov	r31=loc4		    // Xupdate
	dep.z	tmp5=loc5,5,27		}   // a<<5
{ .mib;	xor	tmp0=loc1,loc0
	add	tmp4=loc3,loc12		};;
{ .mib;	xor	tmp0=tmp0,loc2		    // F_20_39(b,c,d)=b^c^d
	extr.u	tmp1=loc5,27,5		}   // a>>27
{ .mib;	add	loc4=loc4,tmp4		    // f+=e+K_20_39
	add	loc7=loc7,loc5		};; // wrap up
{ .mmi;	add	loc4=loc4,tmp0		    // f+=F_20_39(b,c,d)
	shrp	loc0=tmp6,tmp6,2		}   // b=ROTATE(b,30) ;;?
{ .mmi;	or	tmp1=tmp1,tmp5		    // ROTATE(a,5)
	add	loc9=loc9,loc1		};; // wrap up
{ .mib;	add	tmp3=1,inp		    // used in unaligned codepath
	add	loc4=loc4,tmp1		}   // f+=ROTATE(a,5)
{ .mib;	add	loc8=loc8,loc0		    // wrap up
	add	loc10=loc10,loc2		};; // wrap up

{ .mmb;	add	loc6=loc6,loc4
	nop.m	0
	br.ctop.dptk.many	.Ldtop	};;
.Ldend:
{ .mmi;	add	tmp0=4,ctx
	mov	ar.lc=r3		};;
{ .mmi;	st4	[ctx]=loc6,8
	st4	[tmp0]=loc7,8		};;
{ .mmi;	st4	[ctx]=loc8,8
	st4	[tmp0]=loc9		};;
{ .mib;	st4	[ctx]=loc10,-16
	mov	pr=r2,0x1ffff
	br.ret.sptk.many	b0	};;
.endp	sha1_block_data_order#
stringz	"SHA1 block transform for IA64, CRYPTOGAMS by <appro@openssl.org>"

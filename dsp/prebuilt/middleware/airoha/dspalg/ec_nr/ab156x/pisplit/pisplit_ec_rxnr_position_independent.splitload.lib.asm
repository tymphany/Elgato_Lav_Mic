
Build/lib/pisplit_ec_rxnr_position_independent.splitload.lib:     file format elf32-xtensa-le


Disassembly of section .text:

00000000 <_init-0x2d4>:
       0:	00000320 00006bd0 000075fc 00000000      ....k...u......
      10:	000075cc 00000000 000075e4 00006c04     .u.......u...l..
      20:	00008688 00008680 00000338 00000514     ........8.......
      30:	00007890 00007900 00008684 00007654     .x...y......Tv..
      40:	000013b0 0000766d 00001440 00007681     ....mv..@....v..
      50:	000032cc 00007695 00003164 000076ad     .2...v..d1...v..
      60:	000033bc 000076c8 00003938 000076dc     .3...v..89...v..
      70:	00003304 000076f3 000036b0 00007707     .3...v...6...w..
      80:	00003360 0000771e 000013bc 0000773d     `3...w......=w..
      90:	000016a0 00007757 000032e8 00007771     ....Ww...2..qw..
      a0:	00003418 00007792 000013a8 000077a5     .4...w.......w..
      b0:	00003440 000077c3 00003490 000077e5     @4...w...4...w..
      c0:	000034a0 00007610 00006bf0 00300040     .4...v...k..@.0.
      d0:	00200018 00c00100 00600040 00155555     .. .....@.`.UU..
      e0:	00000aac 00000a90 002aaa00 00007800     ..........*..x..
      f0:	00004738 00004630 00000b34 00000b9c     8G..0F..4.......
     100:	00000c34 00000cd8 01507660 00000380     4.......`vP.....
     110:	000013f0 00000698 00003a0c 00005254     .........:..TR..
     120:	00003a2c 0000dfff 00000990 7fff0000     ,:..............
     130:	00007a10 00007a30 00007a70 00007ab0     .z..0z..pz...z..
     140:	0000056c 000005b0 00001dc4 00001804     l...............
     150:	00001b60 00007f00 7f800080 78000800     `..............x
     160:	00017fff 7c000400 000019a4 00001838     .......|....8...
     170:	00001a30 00000d84 00001060 00001c00     0.......`.......
     180:	00005590 00005814 00005ae0 0000595c     .U...X...Z..\Y..
     190:	00005ec0 00006028 00006300 00006448     .^..(`...c..Hd..
     1a0:	00001ad4 000018f0 000019ec 0006ffff     ................
     1b0:	00005d70 00003a48 00001318 0000135c     p]..H:......\...
     1c0:	00006a8c 00006954 4ec4ec4f 00001eb8     .j..Ti..O..N....
     1d0:	00003978 000036f4 00006778 00006764     x9...6..xg..dg..
     1e0:	00006740 00007f40 ffe9ffe9 00007bd0     @g..@........{..
     1f0:	0000ee09 000067d4 000003ec 000034dc     .....g.......4..
     200:	00003744 00003cb0 00003e04 00003f20     D7...<...>.. ?..
     210:	00004098 000041bc 00004278 00004504     .@...A..xB...E..
     220:	00007df0 00003788 000039c8 00003c40     .}...7...9..@<..
     230:	00035e51 00008160 000050a4 00003c68     Q^..`....P..h<..
     240:	00007b90 00007bc0 00007870 00003bd8     .{...{..px...;..
     250:	000e38e4 00007b70 00124925 00007bb0     .8..p{..%I...{..
     260:	80000001 00004a98 00008560 000047f4     .....J..`....G..
     270:	00004758 00004ac8 00004d1c 00004d44     XG...J...M..DM..
     280:	00004d70 00004dc0 00004d98 00004e24     pM...M...M..$N..
     290:	00004dfc 00005070 00005004 0d330000     .M..pP...P....3.
     2a0:	0e390000 000ccccc 00393e4b 00175e97     ..9.....K>9..^..
     2b0:	00005178 0000552c c0010000 3fff0000     xQ..,U.........?
     2c0:	00008660 00000468 000069bc 000075d4     `...h....i...u..
     2d0:	000002ec                                ....

000002d4 <_init>:
     2d4:	008136        	entry	a1, 64
     2d7:	ff4a81        	l32r	a8, 0 (320 <frame_dummy>)
     2da:	f03d           	nop.n
     2dc:	0008e0        	callx8	a8
     2df:	ff4981        	l32r	a8, 4 (6bd0 <__do_global_ctors_aux>)
     2e2:	f03d           	nop.n
     2e4:	0008e0        	callx8	a8
     2e7:	f01d           	retw.n

000002e9 <_init+0x15>:
     2e9:	000000                                        ...

000002ec <__do_global_dtors_aux>:
     2ec:	004136        	entry	a1, 32
     2ef:	ff4621        	l32r	a2, 8 (75fc <__do_global_dtors_aux.completed>)
     2f2:	000232        	l8ui	a3, a2, 0
     2f5:	23ec           	bnez.n	a3, 31b <__do_global_dtors_aux+0x2f>

000002f7 <__do_global_dtors_aux+0xb>:
     2f7:	1248           	l32i.n	a4, a2, 4
     2f9:	0438           	l32i.n	a3, a4, 0
     2fb:	444b           	addi.n	a4, a4, 4
     2fd:	738c           	beqz.n	a3, 308 <__do_global_dtors_aux+0x1c>
     2ff:	1249           	s32i.n	a4, a2, 4
     301:	0003e0        	callx8	a3
     304:	fffbc6        	j	2f7 <__do_global_dtors_aux+0xb>

00000307 <__do_global_dtors_aux+0x1b>:
	...

00000308 <__do_global_dtors_aux+0x1c>:
     308:	ff4131        	l32r	a3, c (0 <_text_start>)
     30b:	ff41a1        	l32r	a10, 10 (75cc <__FRAME_END__>)
     30e:	438c           	beqz.n	a3, 316 <__do_global_dtors_aux+0x2a>
     310:	ff3f81        	l32r	a8, c (0 <_text_start>)
     313:	0008e0        	callx8	a8
     316:	130c           	movi.n	a3, 1
     318:	004232        	s8i	a3, a2, 0
     31b:	f01d           	retw.n

0000031d <__do_global_dtors_aux+0x31>:
     31d:	000000                                        ...

00000320 <frame_dummy>:
     320:	004136        	entry	a1, 32
     323:	ff3c21        	l32r	a2, 14 (0 <_text_start>)
     326:	ff3aa1        	l32r	a10, 10 (75cc <__FRAME_END__>)
     329:	ff3bb1        	l32r	a11, 18 (75e4 <frame_dummy.object>)
     32c:	428c           	beqz.n	a2, 334 <frame_dummy+0x14>
     32e:	ff3981        	l32r	a8, 14 (0 <_text_start>)
     331:	0008e0        	callx8	a8

00000334 <frame_dummy+0x14>:
     334:	f01d           	retw.n

00000336 <frame_dummy+0x16>:
	...

00000338 <common_ECNR_PARA_init_v2>:
     338:	004136        	entry	a1, 32
     33b:	ff3841        	l32r	a4, 1c (6c04 <xt_memcpy>)
     33e:	b32b           	addi.n	a11, a3, 2
     340:	a28b           	addi.n	a10, a2, 8
     342:	79a0c2        	movi	a12, 121
     345:	0004e0        	callx8	a4
     348:	fca052        	movi	a5, 252
     34b:	f4a062        	movi	a6, 244
     34e:	a25a           	add.n	a10, a2, a5
     350:	b36a           	add.n	a11, a3, a6
     352:	5c0c           	movi.n	a12, 5
     354:	0004e0        	callx8	a4
     357:	08a152        	movi	a5, 0x108
     35a:	fea062        	movi	a6, 254
     35d:	a25a           	add.n	a10, a2, a5
     35f:	b36a           	add.n	a11, a3, a6
     361:	d7a0c2        	movi	a12, 215
     364:	0004e0        	callx8	a4
     367:	b8a252        	movi	a5, 0x2b8
     36a:	aca262        	movi	a6, 0x2ac
     36d:	a25a           	add.n	a10, a2, a5
     36f:	b36a           	add.n	a11, a3, a6
     371:	6c3c           	movi.n	a12, 54
     373:	0004e0        	callx8	a4
     376:	ff2a31        	l32r	a3, 20 (8688 <p_ecnr_mem>)
     379:	0338           	l32i.n	a3, a3, 0
     37b:	0239           	s32i.n	a3, a2, 0
     37d:	f01d           	retw.n

0000037f <common_ECNR_PARA_init_v2+0x47>:
	...

00000380 <TX_ECNR_PARA_init>:
     380:	004136        	entry	a1, 32
     383:	ff2741        	l32r	a4, 20 (8688 <p_ecnr_mem>)
     386:	ff2751        	l32r	a5, 24 (8680 <aec_nr_para>)
     389:	ff2761        	l32r	a6, 28 (338 <common_ECNR_PARA_init_v2>)
     38c:	04a8           	l32i.n	a10, a4, 0
     38e:	03bd           	mov.n	a11, a3
     390:	05a9           	s32i.n	a10, a5, 0
     392:	0006e0        	callx8	a6
     395:	0568           	l32i.n	a6, a5, 0
     397:	0478           	l32i.n	a7, a4, 0
     399:	04a182        	movi	a8, 0x104
     39c:	02a192        	movi	a9, 0x102
     39f:	91a4d2        	movi	a13, 0x491
     3a2:	939820        	movnez	a9, a8, a2
     3a5:	0496a2        	l16si	a10, a6, 8
     3a8:	12d7b2        	addmi	a11, a7, 0x1200
     3ab:	2196c2        	l16si	a12, a6, 66
     3ae:	112de0        	slli	a2, a13, 2
     3b1:	2a5ba2        	s16i	a10, a11, 84
     3b4:	2b5bc2        	s16i	a12, a11, 86
     3b7:	baa1e2        	movi	a14, 0x1ba
     3ba:	969a           	add.n	a9, a6, a9
     3bc:	a72a           	add.n	a10, a7, a2
     3be:	ff1721        	l32r	a2, 1c (6c04 <xt_memcpy>)
     3c1:	29fd           	ae_l16si.n	a15, a9, 0
     3c3:	b6ea           	add.n	a11, a6, a14
     3c5:	4c0c           	movi.n	a12, 4
     3c7:	0256f2        	s16i	a15, a6, 4
     3ca:	0002e0        	callx8	a2
     3cd:	0448           	l32i.n	a4, a4, 0
     3cf:	0558           	l32i.n	a5, a5, 0
     3d1:	32a362        	movi	a6, 0x332
     3d4:	24a3e2        	movi	a14, 0x324
     3d7:	b36a           	add.n	a11, a3, a6
     3d9:	a5ea           	add.n	a10, a5, a14
     3db:	1c1c           	movi.n	a12, 17
     3dd:	db95f2        	l16si	a15, a5, 0x1b6
     3e0:	46d442        	addmi	a4, a4, 0x4600
     3e3:	5354f2        	s16i	a15, a4, 166
     3e6:	0002e0        	callx8	a2
     3e9:	f01d           	retw.n

000003eb <TX_ECNR_PARA_init+0x6b>:
	...

000003ec <RX_ECNR_PARA_init>:
     3ec:	004136        	entry	a1, 32
     3ef:	ff0c51        	l32r	a5, 20 (8688 <p_ecnr_mem>)
     3f2:	ff0c61        	l32r	a6, 24 (8680 <aec_nr_para>)
     3f5:	ff0c71        	l32r	a7, 28 (338 <common_ECNR_PARA_init_v2>)
     3f8:	05a8           	l32i.n	a10, a5, 0
     3fa:	03bd           	mov.n	a11, a3
     3fc:	06a9           	s32i.n	a10, a6, 0
     3fe:	0007e0        	callx8	a7
     401:	0638           	l32i.n	a3, a6, 0
     403:	46a372        	movi	a7, 0x346
     406:	b42b           	addi.n	a11, a4, 2
     408:	a37a           	add.n	a10, a3, a7
     40a:	12ac           	beqz.n	a2, 42f <RX_ECNR_PARA_init+0x43>
     40c:	ff0421        	l32r	a2, 1c (6c04 <xt_memcpy>)
     40f:	80a0c2        	movi	a12, 128
     412:	0002e0        	callx8	a2
     415:	0628           	l32i.n	a2, a6, 0
     417:	46a442        	movi	a4, 0x446
     41a:	130c           	movi.n	a3, 1
     41c:	80a0f2        	movi	a15, 128
     41f:	113340        	slli	a3, a3, 12
     422:	804240        	add	a4, a2, a4
     425:	038f76        	loop	a15, 42c <RX_ECNR_PARA_init+0x40>
     428:	643d           	ae_s16i.n	a3, a4, 0
     42a:	442b           	addi.n	a4, a4, 2

0000042c <RX_ECNR_PARA_init+0x40>:
     42c:	000286        	j	43a <RX_ECNR_PARA_init+0x4e>

0000042f <RX_ECNR_PARA_init+0x43>:
     42f:	fefb21        	l32r	a2, 1c (6c04 <xt_memcpy>)
     432:	00a1c2        	movi	a12, 0x100
     435:	0002e0        	callx8	a2
     438:	0628           	l32i.n	a2, a6, 0
     43a:	da9222        	l16si	a2, a2, 0x1b4
     43d:	0538           	l32i.n	a3, a5, 0
     43f:	220b           	addi.n	a2, a2, -1
     441:	46d332        	addmi	a3, a3, 0x4600
     444:	525322        	s16i	a2, a3, 164
     447:	f01d           	retw.n

00000449 <RX_ECNR_PARA_init+0x5d>:
     449:	000000                                        ...

0000044c <EQ_update>:
     44c:	004136        	entry	a1, 32
     44f:	fef441        	l32r	a4, 20 (8688 <p_ecnr_mem>)
     452:	452c           	movi.n	a5, 36
     454:	46d552        	addmi	a5, a5, 0x4600
     457:	0429           	s32i.n	a2, a4, 0
     459:	a25a           	add.n	a10, a2, a5
     45b:	fef021        	l32r	a2, 1c (6c04 <xt_memcpy>)
     45e:	2c0c           	movi.n	a12, 2
     460:	03bd           	mov.n	a11, a3
     462:	0002e0        	callx8	a2
     465:	f01d           	retw.n

00000467 <EQ_update+0x1b>:
	...

00000468 <comfort_noise_gen_wb2>:
     468:	008136        	entry	a1, 64
     46b:	fc24b3a2fe 	{ l32i	a15, a2, 232; ae_zeroq56	aeq0 }
     470:	2f0262        	l8ui	a6, a2, 47
     473:	6f9252        	l16si	a5, a2, 222
     476:	fccf42        	addi	a4, a15, -4
     479:	624414        	ae_sq32f.iu	aeq0, a4, 4
     47c:	624414        	ae_sq32f.iu	aeq0, a4, 4
     47f:	139292        	l16si	a9, a2, 38
     482:	624414        	ae_sq32f.iu	aeq0, a4, 4
     485:	402232        	l32i	a3, a2, 0x100
     488:	624414        	ae_sq32f.iu	aeq0, a4, 4
     48b:	4139           	s32i.n	a3, a1, 16
     48d:	7226b6        	bltui	a6, 2, 503 <comfort_noise_gen_wb2+0x9b>
     490:	41f8           	l32i.n	a15, a1, 16
     492:	8c1272        	l16ui	a7, a2, 0x118
     495:	350b           	addi.n	a3, a5, -1
     497:	6129           	s32i.n	a2, a1, 24
     499:	7139           	s32i.n	a3, a1, 28
     49b:	5169           	s32i.n	a6, a1, 20
     49d:	560b           	addi.n	a5, a6, -1
     49f:	fccf22        	addi	a2, a15, -4
     4a2:	f67c           	movi.n	a6, -1

000004a4 <comfort_noise_gen_wb2+0x3c>:
     4a4:	41f8           	l32i.n	a15, a1, 16
     4a6:	028d           	mov.n	a8, a2
     4a8:	71a8           	l32i.n	a10, a1, 28
     4aa:	a38f60        	movltz	a8, a15, a6
     4ad:	08b8           	l32i.n	a11, a8, 0
     4af:	fedf81        	l32r	a8, 2c (514 <HIFI_sqrt>)
     4b2:	093d           	mov.n	a3, a9
     4b4:	0008e0        	callx8	a8
     4b7:	75a0d2        	movi	a13, 117
     4ba:	039d           	mov.n	a9, a3
     4bc:	bbdd32        	addmi	a3, a13, 0xffffbb00
     4bf:	f1a3e2        	movi	a14, 0x3f1
     4c2:	828730        	mull	a8, a7, a3
     4c5:	b03ee0        	addx8	a3, a14, a14
     4c8:	827730        	mull	a7, a7, a3
     4cb:	300874        	ae_cvtp24a16x2.ll	aep0, a8, a7
     4ce:	360b34        	ae_cvtq48a32s	aeq0, a11
     4d1:	d4e250fa9f 	{ add	a15, a10, a9; ae_mulfq32sp16s.l	aeq1, aeq0, aep0 }
     4d6:	d4620ff55e 	{ addi	a5, a5, -1; ae_mulfq32sp16s.h	aeq0, aeq0, aep0 }
     4db:	fc24fe9f4f 	{ ae_slaasq56s	aeq1, aeq1, a15; nop }
     4e0:	fd4c7e8f0f 	{ ae_slaasq56s	aeq0, aeq0, a15; ae_roundsq32sym	aeq1, aeq1 }
     4e5:	fc4479d41f 	{ ae_sq32f.iu	aeq1, a4, 4; ae_roundsq32sym	aeq0, aeq0 }
     4ea:	224b           	addi.n	a2, a2, 4
     4ec:	661b           	addi.n	a6, a6, 1
     4ee:	624414        	ae_sq32f.iu	aeq0, a4, 4
     4f1:	faf556        	bnez	a5, 4a4 <comfort_noise_gen_wb2+0x3c>
     4f4:	6138           	l32i.n	a3, a1, 24
     4f6:	5168           	l32i.n	a6, a1, 20
     4f8:	fc24807e2f 	{ movi	a2, 126; ae_zeroq56	aeq0 }
     4fd:	8c5372        	s16i	a7, a3, 0x118
     500:	0e3267        	bltu	a2, a6, 512 <comfort_noise_gen_wb2+0xaa>
     503:	7fa022        	movi	a2, 127
     506:	c02260        	sub	a2, a2, a6
     509:	058276        	loop	a2, 512 <comfort_noise_gen_wb2+0xaa>
     50c:	624414        	ae_sq32f.iu	aeq0, a4, 4
     50f:	624414        	ae_sq32f.iu	aeq0, a4, 4

00000512 <comfort_noise_gen_wb2+0xaa>:
     512:	f01d           	retw.n

00000514 <HIFI_sqrt>:
     514:	004136        	entry	a1, 32
     517:	4b13a6        	blti	a3, 1, 566 <HIFI_sqrt+0x52>
     51a:	40e340        	nsa	a4, a3
     51d:	c02240        	sub	a2, a2, a4
     520:	401400        	ssl	a4
     523:	044020        	extui	a4, a2, 0, 1
     526:	a1c300        	sll	a12, a3
     529:	400400        	ssr	a4
     52c:	b130c0        	sra	a3, a12
     52f:	656930        	extui	a6, a3, 25, 7
     532:	febfd1        	l32r	a13, 30 (7890 <TABLE_SQRT>)
     535:	050c           	movi.n	a5, 0
     537:	f0c662        	addi	a6, a6, -16
     53a:	535650        	max	a5, a6, a5
     53d:	9055d0        	addx2	a5, a5, a13
     540:	456d           	ae_l16ui.n	a6, a5, 0
     542:	355d           	ae_l16si.n	a5, a5, 2
     544:	413930        	srli	a3, a3, 9
     547:	11f600        	slli	a15, a6, 16
     54a:	c0e560        	sub	a14, a5, a6
     54d:	300334        	ae_cvtp24a16x2.ll	aep0, a3, a3
     550:	113e00        	slli	a3, a14, 16
     553:	360f34        	ae_cvtq48a32s	aeq0, a15
     556:	364334        	ae_cvtq48a32s	aeq1, a3
     559:	d4e110242f 	{ add	a2, a4, a2; ae_mulaq32sp16u.l	aeq0, aeq1, aep0 }
     55e:	212120        	srai	a2, a2, 1
     561:	368304        	ae_trunca32q48	a3, aeq0
     564:	f01d           	retw.n

00000566 <HIFI_sqrt+0x52>:
     566:	020c           	movi.n	a2, 0
     568:	030c           	movi.n	a3, 0
     56a:	f01d           	retw.n

0000056c <Log2_norm>:
     56c:	004136        	entry	a1, 32
     56f:	024d           	mov.n	a4, a2
     571:	ff7c           	movi.n	a15, -1
     573:	012f10        	slli	a2, a15, 31
     576:	3214a6        	blti	a4, 1, 5ac <Log2_norm+0x40>
     579:	156c           	movi.n	a5, -31
     57b:	2d2357        	blt	a3, a5, 5ac <Log2_norm+0x40>
     57e:	022c           	movi.n	a2, 32
     580:	06a237        	bge	a2, a3, 58a <Log2_norm+0x1e>
     583:	f27c           	movi.n	a2, -1
     585:	412120        	srli	a2, a2, 1
     588:	f01d           	retw.n

0000058a <Log2_norm+0x1e>:
     58a:	feaa51        	l32r	a5, 34 (7900 <TABLE_LOG2_1530>)
     58d:	552840        	extui	a2, a4, 24, 6
     590:	1122e0        	slli	a2, a2, 2
     593:	603524        	ae_lq32f.xu	aeq0, a5, a2
     596:	604514        	ae_lq32f.i	aeq1, a5, 4
     599:	f92ae2044f 	{ ae_movpa24x2	aep0, a4, a4; ae_subq56	aeq1, aeq1, aeq0 }
     59e:	030174        	ae_mulaq32sp16u.l	aeq0, aeq1, aep0
     5a1:	013360        	slli	a3, a3, 26
     5a4:	368f04        	ae_trunca32q48	a15, aeq0
     5a7:	2125f0        	srai	a2, a15, 5
     5aa:	223a           	add.n	a2, a2, a3
     5ac:	f01d           	retw.n

000005ae <Log2_norm+0x42>:
	...

000005b0 <siir_safr>:
     5b0:	004136        	entry	a1, 32
     5b3:	05a237        	bge	a2, a3, 5bc <siir_safr+0xc>
     5b6:	440404        	ae_lp16x2f.i	aep0, a4, 0
     5b9:	000086        	j	5bf <siir_safr+0xf>

000005bc <siir_safr+0xc>:
     5bc:	440414        	ae_lp16x2f.i	aep0, a4, 4
     5bf:	360234        	ae_cvtq48a32s	aeq0, a2
     5c2:	364334        	ae_cvtq48a32s	aeq1, a3
     5c5:	c10014        	ae_mulzaafq32sp16s.lh	aeq0, aeq0, aep0, aeq1, aep0
     5c8:	060934        	ae_roundsq32asym	aeq0, aeq0
     5cb:	368204        	ae_trunca32q48	a2, aeq0
     5ce:	f01d           	retw.n

000005d0 <_start>:
     5d0:	004136        	entry	a1, 32
     5d3:	fe9931        	l32r	a3, 38 (8684 <printf_ptr>)
     5d6:	0228           	l32i.n	a2, a2, 0
     5d8:	fe99a1        	l32r	a10, 3c (7654 <export_parameter_array+0x44>)
     5db:	fe99b1        	l32r	a11, 40 (13b0 <get_aec_nr_memsize>)
     5de:	0329           	s32i.n	a2, a3, 0
     5e0:	0002e0        	callx8	a2
     5e3:	0328           	l32i.n	a2, a3, 0
     5e5:	fe97a1        	l32r	a10, 44 (766d <export_parameter_array+0x5d>)
     5e8:	fe98b1        	l32r	a11, 48 (1440 <Voice_EC_Init>)
     5eb:	0002e0        	callx8	a2
     5ee:	0328           	l32i.n	a2, a3, 0
     5f0:	fe97a1        	l32r	a10, 4c (7681 <export_parameter_array+0x71>)
     5f3:	fe97b1        	l32r	a11, 50 (32cc <Voice_EC_Prcs>)
     5f6:	0002e0        	callx8	a2
     5f9:	0328           	l32i.n	a2, a3, 0
     5fb:	fe96a1        	l32r	a10, 54 (7695 <export_parameter_array+0x85>)
     5fe:	fe96b1        	l32r	a11, 58 (3164 <Voice_PostEC_Prcs>)
     601:	0002e0        	callx8	a2
     604:	0328           	l32i.n	a2, a3, 0
     606:	fe95a1        	l32r	a10, 5c (76ad <export_parameter_array+0x9d>)
     609:	fe95b1        	l32r	a11, 60 (33bc <EC_REF_GAIN_READBACK>)
     60c:	0002e0        	callx8	a2
     60f:	0328           	l32i.n	a2, a3, 0
     611:	fe94a1        	l32r	a10, 64 (76c8 <export_parameter_array+0xb8>)
     614:	fe95b1        	l32r	a11, 68 (3938 <NB_RX_NR_init>)
     617:	0002e0        	callx8	a2
     61a:	0328           	l32i.n	a2, a3, 0
     61c:	fe94a1        	l32r	a10, 6c (76dc <export_parameter_array+0xcc>)
     61f:	fe94b1        	l32r	a11, 70 (3304 <Voice_NB_RX_Prcs>)
     622:	0002e0        	callx8	a2
     625:	0328           	l32i.n	a2, a3, 0
     627:	fe93a1        	l32r	a10, 74 (76f3 <export_parameter_array+0xe3>)
     62a:	fe93b1        	l32r	a11, 78 (36b0 <WB_RX_NR_init>)
     62d:	0002e0        	callx8	a2
     630:	0328           	l32i.n	a2, a3, 0
     632:	fe92a1        	l32r	a10, 7c (7707 <export_parameter_array+0xf7>)
     635:	fe92b1        	l32r	a11, 80 (3360 <Voice_WB_RX_Prcs>)
     638:	0002e0        	callx8	a2
     63b:	0328           	l32i.n	a2, a3, 0
     63d:	fe91a1        	l32r	a10, 84 (771e <export_parameter_array+0x10e>)
     640:	fe92b1        	l32r	a11, 88 (13bc <get_aec_nr_inear_memsize>)
     643:	0002e0        	callx8	a2
     646:	0328           	l32i.n	a2, a3, 0
     648:	fe91a1        	l32r	a10, 8c (773d <export_parameter_array+0x12d>)
     64b:	fe91b1        	l32r	a11, 90 (16a0 <Voice_EC_Inear_Init>)
     64e:	0002e0        	callx8	a2
     651:	0328           	l32i.n	a2, a3, 0
     653:	fe90a1        	l32r	a10, 94 (7757 <export_parameter_array+0x147>)
     656:	fe90b1        	l32r	a11, 98 (32e8 <Voice_EC_Inear_Prcs>)
     659:	0002e0        	callx8	a2
     65c:	0328           	l32i.n	a2, a3, 0
     65e:	fe8fa1        	l32r	a10, 9c (7771 <export_parameter_array+0x161>)
     661:	fe8fb1        	l32r	a11, a0 (3418 <EC_Inear_REF_GAIN_READBACK>)
     664:	0002e0        	callx8	a2
     667:	0328           	l32i.n	a2, a3, 0
     669:	fe8ea1        	l32r	a10, a4 (7792 <export_parameter_array+0x182>)
     66c:	fe8fb1        	l32r	a11, a8 (13a8 <get_ECNR_SVN>)
     66f:	0002e0        	callx8	a2
     672:	0328           	l32i.n	a2, a3, 0
     674:	fe8ea1        	l32r	a10, ac (77a5 <export_parameter_array+0x195>)
     677:	fe8eb1        	l32r	a11, b0 (3440 <EC_PreLim_Coef_READBACK>)
     67a:	0002e0        	callx8	a2
     67d:	0328           	l32i.n	a2, a3, 0
     67f:	fe8da1        	l32r	a10, b4 (77c3 <export_parameter_array+0x1b3>)
     682:	fe8db1        	l32r	a11, b8 (3490 <DaulMIC_power_Coef_READBACK>)
     685:	0002e0        	callx8	a2
     688:	0328           	l32i.n	a2, a3, 0
     68a:	fe8ca1        	l32r	a10, bc (77e5 <export_parameter_array+0x1d5>)
     68d:	fe8cb1        	l32r	a11, c0 (34a0 <PF_GAIN_FB_READBACK>)
     690:	0002e0        	callx8	a2
     693:	fe8c21        	l32r	a2, c4 (7610 <export_parameter_array>)
     696:	f01d           	retw.n

00000698 <SBAEC_INIT>:
     698:	00a136        	entry	a1, 80
     69b:	fe6151        	l32r	a5, 20 (8688 <p_ecnr_mem>)
     69e:	fe8a21        	l32r	a2, c8 (6bf0 <xt_memset>)
     6a1:	29a532        	movi	a3, 0x529
     6a4:	0578           	l32i.n	a7, a5, 0
     6a6:	31a542        	movi	a4, 0x531
     6a9:	0b0c           	movi.n	a11, 0
     6ab:	29d7a2        	addmi	a10, a7, 0x2900
     6ae:	30a1c2        	movi	a12, 0x130
     6b1:	1133d0        	slli	a3, a3, 3
     6b4:	1144d0        	slli	a4, a4, 3
     6b7:	a1a9           	s32i.n	a10, a1, 40
     6b9:	0002e0        	callx8	a2
     6bc:	0558           	l32i.n	a5, a5, 0
     6be:	562c           	movi.n	a6, 37
     6c0:	116690        	slli	a6, a6, 7
     6c3:	a56a           	add.n	a10, a5, a6
     6c5:	0b0c           	movi.n	a11, 0
     6c7:	c0a4c2        	movi	a12, 0x4c0
     6ca:	0002e0        	callx8	a2
     6cd:	fe5591        	l32r	a9, 24 (8680 <aec_nr_para>)
     6d0:	c50c           	movi.n	a5, 12
     6d2:	061c           	movi.n	a6, 16
     6d4:	0928           	l32i.n	a2, a9, 0
     6d6:	041282        	l16ui	a8, a2, 8
     6d9:	105850        	and	a5, a8, a5
     6dc:	04a526        	beqi	a5, 12, 6e4 <SBAEC_INIT+0x4c>
     6df:	105860        	and	a5, a8, a6
     6e2:	459c           	beqz.n	a5, 6fa <SBAEC_INIT+0x62>

000006e4 <SBAEC_INIT+0x4c>:
     6e4:	757c           	movi.n	a5, -9
     6e6:	131262        	l16ui	a6, a2, 38
     6e9:	f45050        	extui	a5, a5, 0, 16
     6ec:	108850        	and	a8, a8, a5
     6ef:	851c           	movi.n	a5, 24
     6f1:	045282        	s16i	a8, a2, 8
     6f4:	023657        	bltu	a6, a5, 6fa <SBAEC_INIT+0x62>
     6f7:	135252        	s16i	a5, a2, 38
     6fa:	373a           	add.n	a3, a7, a3
     6fc:	39a5e2        	movi	a14, 0x539
     6ff:	9139           	s32i.n	a3, a1, 36
     701:	113ed0        	slli	a3, a14, 3
     704:	373a           	add.n	a3, a7, a3
     706:	41a5f2        	movi	a15, 0x541
     709:	6139           	s32i.n	a3, a1, 24
     70b:	113fd0        	slli	a3, a15, 3
     70e:	d74a           	add.n	a13, a7, a4
     710:	373a           	add.n	a3, a7, a3
     712:	096d           	mov.n	a6, a9
     714:	51d9           	s32i.n	a13, a1, 20
     716:	7139           	s32i.n	a3, a1, 28
     718:	4fe837        	bbsi	a8, 3, 76b <SBAEC_INIT+0xd3>
     71b:	e31232        	l16ui	a3, a2, 0x1c6
     71e:	131242        	l16ui	a4, a2, 38
     721:	4b0c           	movi.n	a11, 4
     723:	053316        	beqz	a3, 77a <SBAEC_INIT+0xe2>
     726:	348030        	extui	a8, a3, 0, 4
     729:	41ac30        	srli	a10, a3, 12
     72c:	345430        	extui	a5, a3, 4, 4
     72f:	1188e0        	slli	a8, a8, 2
     732:	34d830        	extui	a13, a3, 8, 4
     735:	a0aab0        	addx4	a10, a10, a11
     738:	11b5d0        	slli	a11, a5, 3
     73b:	113de0        	slli	a3, a13, 2
     73e:	2bd7f2        	addmi	a15, a7, 0x2b00
     741:	884b           	addi.n	a8, a8, 4
     743:	bb8b           	addi.n	a11, a11, 8
     745:	2d5fa2        	s16i	a10, a15, 90
     748:	2bd7e2        	addmi	a14, a7, 0x2b00
     74b:	334b           	addi.n	a3, a3, 4
     74d:	434480        	min	a4, a4, a8
     750:	41f1b0        	srli	a15, a11, 1
     753:	2c5e32        	s16i	a3, a14, 88
     756:	4333a0        	min	a3, a3, a10
     759:	4344f0        	min	a4, a4, a15
     75c:	433430        	min	a3, a4, a3
     75f:	2a5e82        	s16i	a8, a14, 84
     762:	2b5eb2        	s16i	a11, a14, 86
     765:	135232        	s16i	a3, a2, 38
     768:	000806        	j	78c <SBAEC_INIT+0xf4>

0000076b <SBAEC_INIT+0xd3>:
     76b:	fe5a31        	l32r	a3, d4 (c00100 <_end+0xbf7a74>)
     76e:	2bd742        	addmi	a4, a7, 0x2b00
     771:	156432        	s32i	a3, a4, 84
     774:	fe5931        	l32r	a3, d8 (600040 <_end+0x5f79b4>)
     777:	0002c6        	j	786 <SBAEC_INIT+0xee>

0000077a <SBAEC_INIT+0xe2>:
     77a:	fe5431        	l32r	a3, cc (300040 <_end+0x2f79b4>)
     77d:	2bd742        	addmi	a4, a7, 0x2b00
     780:	156432        	s32i	a3, a4, 84
     783:	fe5331        	l32r	a3, d0 (200018 <_end+0x1f798c>)
     786:	2bd742        	addmi	a4, a7, 0x2b00
     789:	166432        	s32i	a3, a4, 88
     78c:	230c           	movi.n	a3, 2
     78e:	29d332        	addmi	a3, a3, 0x2900
     791:	b2eb           	addi.n	a11, a2, 14
     793:	fe2221        	l32r	a2, 1c (6c04 <xt_memcpy>)
     796:	a73a           	add.n	a10, a7, a3
     798:	ec0c           	movi.n	a12, 14
     79a:	0002e0        	callx8	a2
     79d:	0628           	l32i.n	a2, a6, 0
     79f:	fe2051        	l32r	a5, 20 (8688 <p_ecnr_mem>)
     7a2:	29d742        	addmi	a4, a7, 0x2900
     7a5:	061292        	l16ui	a9, a2, 12
     7a8:	d99282        	l16si	a8, a2, 0x1b2
     7ab:	05b8           	l32i.n	a11, a5, 0
     7ad:	02e8           	l32i.n	a14, a2, 0
     7af:	d812d2        	l16ui	a13, a2, 0x1b0
     7b2:	115482        	s16i	a8, a4, 34
     7b5:	fe4981        	l32r	a8, dc (155555 <_end+0x14cec9>)
     7b8:	5f2c           	movi.n	a15, 37
     7ba:	9158           	l32i.n	a5, a1, 36
     7bc:	4489           	s32i.n	a8, a4, 16
     7be:	1189c0        	slli	a8, a9, 4
     7c1:	c692c2        	l16si	a12, a2, 0x18c
     7c4:	112f90        	slli	a2, a15, 7
     7c7:	9489           	s32i.n	a8, a4, 36
     7c9:	b480d0        	extui	a8, a13, 0, 12
     7cc:	0f54c2        	s16i	a12, a4, 30
     7cf:	fb2a           	add.n	a15, a11, a2
     7d1:	81e9           	s32i.n	a14, a1, 32
     7d3:	04de92        	addmi	a9, a14, 0x400
     7d6:	fe4231        	l32r	a3, e0 (aac <Fill_CH_MEM_WB>)
     7d9:	51b8           	l32i.n	a11, a1, 20
     7db:	61c8           	l32i.n	a12, a1, 24
     7dd:	71d8           	l32i.n	a13, a1, 28
     7df:	a1e8           	l32i.n	a14, a1, 40
     7e1:	105482        	s16i	a8, a4, 32
     7e4:	05ad           	mov.n	a10, a5
     7e6:	0199           	s32i.n	a9, a1, 0
     7e8:	0003e0        	callx8	a3
     7eb:	fe0d21        	l32r	a2, 20 (8688 <p_ecnr_mem>)
     7ee:	0648           	l32i.n	a4, a6, 0
     7f0:	2bd792        	addmi	a9, a7, 0x2b00
     7f3:	0228           	l32i.n	a2, a2, 0
     7f5:	1694c2        	l16si	a12, a4, 44
     7f8:	2a99b2        	l16si	a11, a9, 84
     7fb:	12d282        	addmi	a8, a2, 0x1200
     7fe:	12d222        	addmi	a2, a2, 0x1200
     801:	fe3841        	l32r	a4, e4 (a90 <Fill_sbco>)
     804:	2ad732        	addmi	a3, a7, 0x2a00
     807:	182882        	l32i	a8, a8, 96
     80a:	1c2222        	l32i	a2, a2, 112
     80d:	1d0c           	movi.n	a13, 1
     80f:	05ad           	mov.n	a10, a5
     811:	f389           	s32i.n	a8, a3, 60
     813:	b199           	s32i.n	a9, a1, 44
     815:	106322        	s32i	a2, a3, 64
     818:	0004e0        	callx8	a4
     81b:	2bd732        	addmi	a3, a7, 0x2b00
     81e:	9139           	s32i.n	a3, a1, 36
     820:	2b9332        	l16si	a3, a3, 86
     823:	0628           	l32i.n	a2, a6, 0
     825:	51a8           	l32i.n	a10, a1, 20
     827:	f31b           	addi.n	a15, a3, 1
     829:	1792c2        	l16si	a12, a2, 46
     82c:	b3f330        	movgez	a15, a3, a3
     82f:	21b1f0        	srai	a11, a15, 1
     832:	fdfb21        	l32r	a2, 20 (8688 <p_ecnr_mem>)
     835:	1d0c           	movi.n	a13, 1
     837:	0004e0        	callx8	a4
     83a:	0638           	l32i.n	a3, a6, 0
     83c:	2bd752        	addmi	a5, a7, 0x2b00
     83f:	2c95b2        	l16si	a11, a5, 88
     842:	1893c2        	l16si	a12, a3, 48
     845:	61a8           	l32i.n	a10, a1, 24
     847:	1d0c           	movi.n	a13, 1
     849:	5159           	s32i.n	a5, a1, 20
     84b:	0004e0        	callx8	a4
     84e:	0658           	l32i.n	a5, a6, 0
     850:	2bd732        	addmi	a3, a7, 0x2b00
     853:	2d93b2        	l16si	a11, a3, 90
     856:	1995c2        	l16si	a12, a5, 50
     859:	71a8           	l32i.n	a10, a1, 28
     85b:	1d0c           	movi.n	a13, 1
     85d:	0004e0        	callx8	a4
     860:	0668           	l32i.n	a6, a6, 0
     862:	ffa0a2        	movi	a10, 255
     865:	0258           	l32i.n	a5, a2, 0
     867:	29d792        	addmi	a9, a7, 0x2900
     86a:	7fdaa2        	addmi	a10, a10, 0x7f00
     86d:	1459a2        	s16i	a10, a9, 40
     870:	2453a2        	s16i	a10, a3, 72
     873:	0416a2        	l16ui	a10, a6, 8
     876:	a1e8           	l32i.n	a14, a1, 40
     878:	059642        	l16si	a4, a6, 10
     87b:	155942        	s16i	a4, a9, 42
     87e:	12d592        	addmi	a9, a5, 0x1200
     881:	2396f2        	l16si	a15, a6, 70
     884:	159622        	l16si	a2, a6, 42
     887:	215922        	s16i	a2, a9, 66
     88a:	029622        	l16si	a2, a6, 4
     88d:	2653f2        	s16i	a15, a3, 76
     890:	04f0a0        	extui	a15, a10, 0, 1
     893:	162942        	l32i	a4, a9, 88
     896:	14a1a0        	extui	a10, a10, 1, 2
     899:	dc9662        	l16si	a6, a6, 0x1b8
     89c:	2659a2        	s16i	a10, a9, 76
     89f:	6e2d           	ae_s16i.n	a2, a14, 0
     8a1:	146342        	s32i	a4, a3, 80
     8a4:	2553f2        	s16i	a15, a3, 74
     8a7:	275362        	s16i	a6, a3, 78
     8aa:	022a26        	beqi	a10, 2, 8b0 <SBAEC_INIT+0x218>
     8ad:	003706        	j	98d <SBAEC_INIT+0x2f5>
     8b0:	03bd           	mov.n	a11, a3
     8b2:	6139           	s32i.n	a3, a1, 24
     8b4:	b138           	l32i.n	a3, a1, 44
     8b6:	9148           	l32i.n	a4, a1, 36
     8b8:	5128           	l32i.n	a2, a1, 20
     8ba:	2d9bf2        	l16si	a15, a11, 90
     8bd:	2a9362        	l16si	a6, a3, 84
     8c0:	2c9292        	l16si	a9, a2, 88
     8c3:	2b9482        	l16si	a8, a4, 86
     8c6:	81d8           	l32i.n	a13, a1, 32
     8c8:	18a5a2        	movi	a10, 0x518
     8cb:	99fa           	add.n	a9, a9, a15
     8cd:	2daa           	add.n	a2, a13, a10
     8cf:	686a           	add.n	a6, a8, a6
     8d1:	49a5b2        	movi	a11, 0x549
     8d4:	51a5c2        	movi	a12, 0x551
     8d7:	59a5d2        	movi	a13, 0x559
     8da:	669a           	add.n	a6, a6, a9
     8dc:	118bd0        	slli	a8, a11, 3
     8df:	119dd0        	slli	a9, a13, 3
     8e2:	61a5d2        	movi	a13, 0x561
     8e5:	11acd0        	slli	a10, a12, 3
     8e8:	5f2c           	movi.n	a15, 37
     8ea:	a0b650        	addx4	a11, a6, a5
     8ed:	11ddd0        	slli	a13, a13, 3
     8f0:	678a           	add.n	a6, a7, a8
     8f2:	118f90        	slli	a8, a15, 7
     8f5:	57aa           	add.n	a5, a7, a10
     8f7:	0129           	s32i.n	a2, a1, 0
     8f9:	fdf921        	l32r	a2, e0 (aac <Fill_CH_MEM_WB>)
     8fc:	c79a           	add.n	a12, a7, a9
     8fe:	d7da           	add.n	a13, a7, a13
     900:	fb8a           	add.n	a15, a11, a8
     902:	06ad           	mov.n	a10, a6
     904:	05bd           	mov.n	a11, a5
     906:	81c9           	s32i.n	a12, a1, 32
     908:	71d9           	s32i.n	a13, a1, 28
     90a:	0002e0        	callx8	a2
     90d:	fdc421        	l32r	a2, 20 (8688 <p_ecnr_mem>)
     910:	2a93b2        	l16si	a11, a3, 84
     913:	fdc431        	l32r	a3, 24 (8680 <aec_nr_para>)
     916:	29d782        	addmi	a8, a7, 0x2900
     919:	0228           	l32i.n	a2, a2, 0
     91b:	03d8           	l32i.n	a13, a3, 0
     91d:	202882        	l32i	a8, a8, 128
     920:	29d7e2        	addmi	a14, a7, 0x2900
     923:	606e82        	s32i	a8, a14, 0x180
     926:	302e82        	l32i	a8, a14, 192
     929:	2ad7f2        	addmi	a15, a7, 0x2a00
     92c:	0f98           	l32i.n	a9, a15, 0
     92e:	306f82        	s32i	a8, a15, 192
     931:	12d2e2        	addmi	a14, a2, 0x1200
     934:	12d222        	addmi	a2, a2, 0x1200
     937:	2bd782        	addmi	a8, a7, 0x2b00
     93a:	0899           	s32i.n	a9, a8, 0
     93c:	1a9dc2        	l16si	a12, a13, 52
     93f:	fde971        	l32r	a7, e4 (a90 <Fill_sbco>)
     942:	1a2ef2        	l32i	a15, a14, 104
     945:	1c2222        	l32i	a2, a2, 112
     948:	0d0c           	movi.n	a13, 0
     94a:	06ad           	mov.n	a10, a6
     94c:	f8f9           	s32i.n	a15, a8, 60
     94e:	106822        	s32i	a2, a8, 64
     951:	0007e0        	callx8	a7
     954:	2b9422        	l16si	a2, a4, 86
     957:	03f8           	l32i.n	a15, a3, 0
     959:	0d0c           	movi.n	a13, 0
     95b:	621b           	addi.n	a6, a2, 1
     95d:	b36220        	movgez	a6, a2, a2
     960:	1b9fc2        	l16si	a12, a15, 54
     963:	21b160        	srai	a11, a6, 1
     966:	05ad           	mov.n	a10, a5
     968:	0007e0        	callx8	a7
     96b:	0328           	l32i.n	a2, a3, 0
     96d:	5148           	l32i.n	a4, a1, 20
     96f:	81a8           	l32i.n	a10, a1, 32
     971:	1c92c2        	l16si	a12, a2, 56
     974:	2c94b2        	l16si	a11, a4, 88
     977:	0d0c           	movi.n	a13, 0
     979:	0007e0        	callx8	a7
     97c:	0328           	l32i.n	a2, a3, 0
     97e:	61f8           	l32i.n	a15, a1, 24
     980:	71a8           	l32i.n	a10, a1, 28
     982:	1d92c2        	l16si	a12, a2, 58
     985:	2d9fb2        	l16si	a11, a15, 90
     988:	0d0c           	movi.n	a13, 0
     98a:	0007e0        	callx8	a7
     98d:	f01d           	retw.n

0000098f <SBAEC_INIT+0x2f7>:
	...

00000990 <SBAEC_INIT_inear>:
     990:	008136        	entry	a1, 64
     993:	fda441        	l32r	a4, 24 (8680 <aec_nr_para>)
     996:	2bd232        	addmi	a3, a2, 0x2b00
     999:	2a9352        	l16si	a5, a3, 84
     99c:	2b9362        	l16si	a6, a3, 86
     99f:	04f8           	l32i.n	a15, a4, 0
     9a1:	2c93d2        	l16si	a13, a3, 88
     9a4:	2d93e2        	l16si	a14, a3, 90
     9a7:	2bd272        	addmi	a7, a2, 0x2b00
     9aa:	5139           	s32i.n	a3, a1, 20
     9ac:	46d232        	addmi	a3, a2, 0x4600
     9af:	665a           	add.n	a6, a6, a5
     9b1:	3a2352        	l32i	a5, a3, 232
     9b4:	ddea           	add.n	a13, a13, a14
     9b6:	0ff8           	l32i.n	a15, a15, 0
     9b8:	5b2c           	movi.n	a11, 37
     9ba:	f0a082        	movi	a8, 240
     9bd:	36da           	add.n	a3, a6, a13
     9bf:	b0a062        	movi	a6, 176
     9c2:	70a1a2        	movi	a10, 0x170
     9c5:	30a1c2        	movi	a12, 0x130
     9c8:	e0a592        	movi	a9, 0x5e0
     9cb:	d5aa           	add.n	a13, a5, a10
     9cd:	458a           	add.n	a4, a5, a8
     9cf:	11bb90        	slli	a11, a11, 7
     9d2:	656a           	add.n	a6, a5, a6
     9d4:	b0a320        	addx8	a10, a3, a2
     9d7:	fdc281        	l32r	a8, e0 (aac <Fill_CH_MEM_WB>)
     9da:	9f9a           	add.n	a9, a15, a9
     9dc:	faba           	add.n	a15, a10, a11
     9de:	c5ca           	add.n	a12, a5, a12
     9e0:	29d2e2        	addmi	a14, a2, 0x2900
     9e3:	06ad           	mov.n	a10, a6
     9e5:	04bd           	mov.n	a11, a4
     9e7:	61c9           	s32i.n	a12, a1, 24
     9e9:	71d9           	s32i.n	a13, a1, 28
     9eb:	0199           	s32i.n	a9, a1, 0
     9ed:	0008e0        	callx8	a8
     9f0:	12d292        	addmi	a9, a2, 0x1200
     9f3:	1c2992        	l32i	a9, a9, 112
     9f6:	fd8b31        	l32r	a3, 24 (8680 <aec_nr_para>)
     9f9:	a02582        	l32i	a8, a5, 0x280
     9fc:	29d2c2        	addmi	a12, a2, 0x2900
     9ff:	6a6592        	s32i	a9, a5, 0x1a8
     a02:	696582        	s32i	a8, a5, 0x1a4
     a05:	202c82        	l32i	a8, a12, 128
     a08:	03f8           	l32i.n	a15, a3, 0
     a0a:	3a6582        	s32i	a8, a5, 232
     a0d:	302c92        	l32i	a9, a12, 192
     a10:	5138           	l32i.n	a3, a1, 20
     a12:	2ad2e2        	addmi	a14, a2, 0x2a00
     a15:	4a6592        	s32i	a9, a5, 0x128
     a18:	0e28           	l32i.n	a2, a14, 0
     a1a:	02df82        	addmi	a8, a15, 0x200
     a1d:	5a6522        	s32i	a2, a5, 0x168
     a20:	2a93b2        	l16si	a11, a3, 84
     a23:	5798c2        	l16si	a12, a8, 174
     a26:	fdaf21        	l32r	a2, e4 (a90 <Fill_sbco>)
     a29:	4d0c           	movi.n	a13, 4
     a2b:	06ad           	mov.n	a10, a6
     a2d:	0002e0        	callx8	a2
     a30:	2b9362        	l16si	a6, a3, 86
     a33:	fd7c31        	l32r	a3, 24 (8680 <aec_nr_para>)
     a36:	4d0c           	movi.n	a13, 4
     a38:	961b           	addi.n	a9, a6, 1
     a3a:	0388           	l32i.n	a8, a3, 0
     a3c:	b39660        	movgez	a9, a6, a6
     a3f:	21b190        	srai	a11, a9, 1
     a42:	02d882        	addmi	a8, a8, 0x200
     a45:	5898c2        	l16si	a12, a8, 176
     a48:	04ad           	mov.n	a10, a4
     a4a:	0002e0        	callx8	a2
     a4d:	0348           	l32i.n	a4, a3, 0
     a4f:	2c97b2        	l16si	a11, a7, 88
     a52:	61a8           	l32i.n	a10, a1, 24
     a54:	02d442        	addmi	a4, a4, 0x200
     a57:	5994c2        	l16si	a12, a4, 178
     a5a:	4d0c           	movi.n	a13, 4
     a5c:	036d           	mov.n	a6, a3
     a5e:	0002e0        	callx8	a2
     a61:	0348           	l32i.n	a4, a3, 0
     a63:	2d97b2        	l16si	a11, a7, 90
     a66:	71a8           	l32i.n	a10, a1, 28
     a68:	02d432        	addmi	a3, a4, 0x200
     a6b:	5a93c2        	l16si	a12, a3, 180
     a6e:	4d0c           	movi.n	a13, 4
     a70:	0002e0        	callx8	a2
     a73:	0638           	l32i.n	a3, a6, 0
     a75:	6ea222        	movi	a2, 0x26e
     a78:	88a242        	movi	a4, 0x288
     a7b:	a52a           	add.n	a10, a5, a2
     a7d:	fd6721        	l32r	a2, 1c (6c04 <xt_memcpy>)
     a80:	b34a           	add.n	a11, a3, a4
     a82:	3c0c           	movi.n	a12, 3
     a84:	0002e0        	callx8	a2
     a87:	5128           	l32i.n	a2, a1, 20
     a89:	176252        	s32i	a5, a2, 92
     a8c:	f01d           	retw.n

00000a8e <SBAEC_INIT_inear+0xfe>:
	...

00000a90 <Fill_sbco>:
     a90:	004136        	entry	a1, 32
     a93:	ffa062        	movi	a6, 255
     a96:	11f400        	slli	a15, a4, 16
     a99:	7fd662        	addmi	a6, a6, 0x7f00
     a9c:	115232        	s16i	a3, a2, 34
     a9f:	105262        	s16i	a6, a2, 32
     aa2:	125252        	s16i	a5, a2, 36
     aa5:	52f9           	s32i.n	a15, a2, 20
     aa7:	42f9           	s32i.n	a15, a2, 16
     aa9:	f01d           	retw.n

00000aab <Fill_sbco+0x1b>:
	...

00000aac <Fill_CH_MEM_WB>:
     aac:	004136        	entry	a1, 32
     aaf:	8188           	l32i.n	a8, a1, 32
     ab1:	8ca092        	movi	a9, 140
     ab4:	a0a0a2        	movi	a10, 160
     ab7:	50c8f2        	addi	a15, a8, 80
     aba:	c8a0b2        	movi	a11, 200
     abd:	f0a0c2        	movi	a12, 240
     ac0:	d389           	s32i.n	a8, a3, 52
     ac2:	28c8d2        	addi	a13, a8, 40
     ac5:	d2f9           	s32i.n	a15, a2, 52
     ac7:	04a1f2        	movi	a15, 0x104
     aca:	f3d9           	s32i.n	a13, a3, 60
     acc:	64c8e2        	addi	a14, a8, 100
     acf:	b8ba           	add.n	a11, a8, a11
     ad1:	989a           	add.n	a9, a8, a9
     ad3:	a8aa           	add.n	a10, a8, a10
     ad5:	c8ca           	add.n	a12, a8, a12
     ad7:	d8fa           	add.n	a13, a8, a15
     ad9:	78c882        	addi	a8, a8, 120
     adc:	f289           	s32i.n	a8, a2, 60
     ade:	f499           	s32i.n	a9, a4, 60
     ae0:	58c692        	addi	a9, a6, 88
     ae3:	d4e9           	s32i.n	a14, a4, 52
     ae5:	fe99e2        	l16si	a14, a9, 0x1fc
     ae8:	f5a9           	s32i.n	a10, a5, 60
     aea:	5cc6a2        	addi	a10, a6, 92
     aed:	ff9982        	l16si	a8, a9, 0x1fe
     af0:	fe9af2        	l16si	a15, a10, 0x1fc
     af3:	e3b9           	s32i.n	a11, a3, 56
     af5:	ff9a62        	l16si	a6, a10, 0x1fe
     af8:	90be70        	addx2	a11, a14, a7
     afb:	a279           	s32i.n	a7, a2, 40
     afd:	9078b0        	addx2	a7, a8, a11
     b00:	a3b9           	s32i.n	a11, a3, 40
     b02:	a479           	s32i.n	a7, a4, 40
     b04:	907f70        	addx2	a7, a15, a7
     b07:	906670        	addx2	a6, a6, a7
     b0a:	b269           	s32i.n	a6, a2, 44
     b0c:	906e60        	addx2	a6, a14, a6
     b0f:	a579           	s32i.n	a7, a5, 40
     b11:	b369           	s32i.n	a6, a3, 44
     b13:	906860        	addx2	a6, a8, a6
     b16:	e2c9           	s32i.n	a12, a2, 56
     b18:	e4d9           	s32i.n	a13, a4, 56
     b1a:	a3c8           	l32i.n	a12, a3, 40
     b1c:	a2d8           	l32i.n	a13, a2, 40
     b1e:	a4e8           	l32i.n	a14, a4, 40
     b20:	90ff60        	addx2	a15, a15, a6
     b23:	c3c9           	s32i.n	a12, a3, 48
     b25:	c2d9           	s32i.n	a13, a2, 48
     b27:	c4e9           	s32i.n	a14, a4, 48
     b29:	b469           	s32i.n	a6, a4, 44
     b2b:	c579           	s32i.n	a7, a5, 48
     b2d:	b5f9           	s32i.n	a15, a5, 44
     b2f:	f01d           	retw.n

00000b31 <Fill_CH_MEM_WB+0x85>:
     b31:	000000                                        ...

00000b34 <common_x_pow>:
     b34:	004136        	entry	a1, 32
     b37:	360334        	ae_cvtq48a32s	aeq0, a3
     b3a:	fe00bec47f 	{ ae_cvtq48a32s	aeq1, a4; ae_absq56	aeq2, aeq0 }
     b3f:	ff0480005f 	{ movi	a5, 0; ae_absq56	aeq3, aeq1 }
     b44:	d6d80001ff 	{ movi	a15, 1; ae_ltq56s	b0, aeq2, aeq3 }
     b49:	023d           	mov.n	a3, a2
     b4b:	120c           	movi.n	a2, 1
     b4d:	d35f00        	movt	a5, a15, b0
     b50:	65ac           	beqz.n	a5, b7a <common_x_pow+0x46>
     b52:	f9bac0004f 	{ movi	a4, 0; ae_subq56	aeq1, aeq3, aeq2 }
     b57:	0122b0        	slli	a2, a2, 21
     b5a:	36c234        	ae_cvtq48a32s	aeq3, a2
     b5d:	d6cc0001ff 	{ movi	a15, 1; ae_ltq56s	b0, aeq3, aeq1 }
     b62:	120c           	movi.n	a2, 1
     b64:	fd6151        	l32r	a5, e8 (2aaa00 <_end+0x2a2374>)
     b67:	fd04564f0f 	{ movt	a4, a15, b0; ae_movq56	aeq1, aeq0 }
     b6c:	c48c           	beqz.n	a4, b7c <common_x_pow+0x48>
     b6e:	398554        	ae_movpa24x2	aep0, a5, a5
     b71:	030a14        	ae_mulfq32sp16s.l	aeq1, aeq0, aep0
     b74:	065934        	ae_roundsq32asym	aeq1, aeq1
     b77:	000046        	j	b7c <common_x_pow+0x48>

00000b7a <common_x_pow+0x46>:
     b7a:	020c           	movi.n	a2, 0
     b7c:	fd5c41        	l32r	a4, ec (7800 <immediate_num>)
     b7f:	608324        	ae_lq32f.i	aeq2, a3, 8
     b82:	60c334        	ae_lq32f.i	aeq3, a3, 12
     b85:	440404        	ae_lp16x2f.i	aep0, a4, 0
     b88:	c10084        	ae_mulzaafq32sp16s.lh	aeq0, aeq2, aep0, aeq0, aep0
     b8b:	c108d4        	ae_mulzaafq32sp16s.lh	aeq1, aeq3, aep0, aeq1, aep0
     b8e:	060934        	ae_roundsq32asym	aeq0, aeq0
     b91:	fd2c79832f 	{ ae_sq32f.i	aeq0, a3, 8; ae_roundsq32asym	aeq1, aeq1 }
     b96:	621334        	ae_sq32f.i	aeq1, a3, 12
     b99:	f01d           	retw.n

00000b9b <common_x_pow+0x67>:
	...

00000b9c <ref_gain_adapt>:
     b9c:	004136        	entry	a1, 32
     b9f:	5378           	l32i.n	a7, a3, 20
     ba1:	fd5261        	l32r	a6, ec (7800 <immediate_num>)
     ba4:	360734        	ae_cvtq48a32s	aeq0, a7
     ba7:	03b5d6        	bgez	a5, be6 <ref_gain_adapt+0x4a>
     baa:	059252        	l16si	a5, a2, 10
     bad:	440614        	ae_lp16x2f.i	aep0, a6, 4
     bb0:	364434        	ae_cvtq48a32s	aeq1, a4
     bb3:	11d500        	slli	a13, a5, 16
     bb6:	d5637ecdbf 	{ ae_cvtq48a32s	aeq2, a13; ae_mulfq32sp16s.h	aeq3, aeq1, aep0 }
     bbb:	0e0c           	movi.n	a14, 0
     bbd:	d6d80001ff 	{ movi	a15, 1; ae_ltq56s	b0, aeq2, aeq3 }
     bc2:	040c           	movi.n	a4, 0
     bc4:	150c           	movi.n	a5, 1
     bc6:	d3ef00        	movt	a14, a15, b0
     bc9:	0edc           	bnez.n	a14, bdd <ref_gain_adapt+0x41>
     bcb:	440624        	ae_lp16x2f.i	aep0, a6, 8
     bce:	d56340007f 	{ movi	a7, 0; ae_mulfq32sp16s.h	aeq3, aeq1, aep0 }
     bd3:	d6d800018f 	{ movi	a8, 1; ae_ltq56s	b0, aeq2, aeq3 }
     bd8:	d34500        	movt	a4, a5, b0
     bdb:	24ac           	beqz.n	a4, c01 <ref_gain_adapt+0x65>

00000bdd <ref_gain_adapt+0x41>:
     bdd:	a10004        	ae_mulzaafq32sp16s.ll	aeq0, aeq0, aep0, aeq0, aep0
     be0:	060934        	ae_roundsq32asym	aeq0, aeq0
     be3:	368704        	ae_trunca32q48	a7, aeq0
     be6:	120c           	movi.n	a2, 1
     be8:	0122b0        	slli	a2, a2, 21
     beb:	532720        	max	a2, a7, a2
     bee:	5329           	s32i.n	a2, a3, 20
     bf0:	604344        	ae_lq32f.i	aeq1, a3, 16
     bf3:	440654        	ae_lp16x2f.i	aep0, a6, 20
     bf6:	c10044        	ae_mulzaafq32sp16s.lh	aeq0, aeq1, aep0, aeq0, aep0
     bf9:	060934        	ae_roundsq32asym	aeq0, aeq0
     bfc:	620344        	ae_sq32f.i	aeq0, a3, 16
     bff:	f01d           	retw.n

00000c01 <ref_gain_adapt+0x65>:
     c01:	440634        	ae_lp16x2f.i	aep0, a6, 12
     c04:	d76240004f 	{ movi	a4, 0; ae_mulfq32sp16s.h	aeq3, aeq2, aep0 }
     c09:	02d222        	addmi	a2, a2, 0x200
     c0c:	d65c00015f 	{ movi	a5, 1; ae_ltq56s	b0, aeq1, aeq3 }
     c11:	038a94        	ae_mulfq32sp16s.l	aeq3, aeq2, aep0
     c14:	4e0222        	l8ui	a2, a2, 78
     c17:	d6cc16780f 	{ movt	a7, a8, b0; ae_ltq56s	b0, aeq3, aeq1 }
     c1c:	fc0716        	beqz	a7, be0 <ref_gain_adapt+0x44>
     c1f:	370c           	movi.n	a7, 3
     c21:	d34500        	movt	a4, a5, b0
     c24:	14cc           	bnez.n	a4, c29 <ref_gain_adapt+0x8d>
     c26:	b60727        	bnone	a7, a2, be0 <ref_gain_adapt+0x44>
     c29:	440644        	ae_lp16x2f.i	aep0, a6, 16
     c2c:	030214        	ae_mulfq32sp16s.l	aeq0, aeq0, aep0
     c2f:	ffeb46        	j	be0 <ref_gain_adapt+0x44>

00000c32 <ref_gain_adapt+0x96>:
	...

00000c34 <aec_post_calc>:
     c34:	006136        	entry	a1, 48
     c37:	054466        	bnei	a4, 4, c40 <aec_post_calc+0xc>
     c3a:	20c242        	addi	a4, a2, 32
     c3d:	000086        	j	c43 <aec_post_calc+0xf>

00000c40 <aec_post_calc+0xc>:
     c40:	1ec242        	addi	a4, a2, 30
     c43:	150c           	movi.n	a5, 1
     c45:	0155d0        	slli	a5, a5, 19
     c48:	400404        	ae_lp16f.i	aep0, a4, 0
     c4b:	360534        	ae_cvtq48a32s	aeq0, a5
     c4e:	1d0c           	movi.n	a13, 1
     c50:	115d00        	slli	a5, a13, 16
     c53:	d4e2bec5ff 	{ ae_cvtq48a32s	aeq3, a5; ae_mulfq32sp16u.l	aeq0, aeq0, aep0 }
     c58:	23c8           	l32i.n	a12, a3, 8
     c5a:	d6e3b9813f 	{ ae_sq32f.i	aeq0, a1, 12; ae_mulfq32sp16u.l	aeq0, aeq3, aep0 }
     c5f:	3178           	l32i.n	a7, a1, 12
     c61:	33e8           	l32i.n	a14, a3, 12
     c63:	620134        	ae_sq32f.i	aeq0, a1, 12
     c66:	3168           	l32i.n	a6, a1, 12
     c68:	53bc70        	max	a11, a12, a7
     c6b:	fd2141        	l32r	a4, f0 (4738 <airoha_divide_limit_1>)
     c6e:	53fe60        	max	a15, a14, a6
     c71:	43abf0        	min	a10, a11, a15
     c74:	0004e0        	callx8	a4
     c77:	600364        	ae_lq32f.i	aeq0, a3, 24
     c7a:	4c0244        	ae_lp24.i	aep0, a2, 16
     c7d:	379aa4        	ae_truncp24a32x2	aep1, a10, a10
     c80:	e207a1824e 	{ l16si	a4, a2, 48; ae_mulafp24s.ll	aeq0, aep1, aep0 }
     c85:	f966a0225e 	{ l16si	a5, a2, 4; ae_movp48	aep2, aep1 }
     c8a:	fc2460b27e 	{ l16si	a7, a2, 22; ae_roundsq32asym	aeq0, aeq0 }
     c8f:	620364        	ae_sq32f.i	aeq0, a3, 24
     c92:	18a547        	bge	a5, a4, cae <aec_post_calc+0x7a>
     c95:	0a9242        	l16si	a4, a2, 20
     c98:	179252        	l16si	a5, a2, 46
     c9b:	060c           	movi.n	a6, 0
     c9d:	742b           	addi.n	a7, a4, 2
     c9f:	725650        	salt	a5, a6, a5
     ca2:	837450        	moveqz	a7, a4, a5
     ca5:	031674        	wur.ae_sar	a7
     ca8:	0b10a4        	ae_sllssp24s	aep2, aep1
     cab:	000506        	j	cc3 <aec_post_calc+0x8f>

00000cae <aec_post_calc+0x7a>:
     cae:	074d           	mov.n	a4, a7
     cb0:	0f27a6        	blti	a7, 2, cc3 <aec_post_calc+0x8f>
     cb3:	944d           	ae_zext16	a4, a4
     cb5:	f9668ff44e 	{ addi	a4, a4, -1; ae_movp48	aep2, aep1 }
     cba:	058476        	loop	a4, cc3 <aec_post_calc+0x8f>
     cbd:	012174        	ae_mulfp24s.ll	aeq0, aep2, aep1
     cc0:	1b8d24        	ae_roundsp24q48asym	aep2, aeq0

00000cc3 <aec_post_calc+0x8f>:
     cc3:	401274        	ae_lp16f.i	aep1, a2, 14
     cc6:	d82a7d037f 	{ ae_lq32f.i	aeq0, a3, 28; ae_maxp24s	aep1, aep2, aep1 }
     ccb:	011034        	ae_mulafp24s.ll	aeq0, aep1, aep0
     cce:	060934        	ae_roundsq32asym	aeq0, aeq0
     cd1:	620374        	ae_sq32f.i	aeq0, a3, 28
     cd4:	f01d           	retw.n

00000cd6 <aec_post_calc+0xa2>:
	...

00000cd8 <aec_post_calc_FB>:
     cd8:	006136        	entry	a1, 48
     cdb:	140c           	movi.n	a4, 1
     cdd:	0144d0        	slli	a4, a4, 19
     ce0:	1ec272        	addi	a7, a2, 30
     ce3:	160c           	movi.n	a6, 1
     ce5:	360434        	ae_cvtq48a32s	aeq0, a4
     ce8:	400704        	ae_lp16f.i	aep0, a7, 0
     ceb:	116600        	slli	a6, a6, 16
     cee:	d4e2bec6ff 	{ ae_cvtq48a32s	aeq3, a6; ae_mulfq32sp16u.l	aeq0, aeq0, aep0 }
     cf3:	23d8           	l32i.n	a13, a3, 8
     cf5:	d6e3b9813f 	{ ae_sq32f.i	aeq0, a1, 12; ae_mulfq32sp16u.l	aeq0, aeq3, aep0 }
     cfa:	31f8           	l32i.n	a15, a1, 12
     cfc:	620134        	ae_sq32f.i	aeq0, a1, 12
     cff:	33e8           	l32i.n	a14, a3, 12
     d01:	3158           	l32i.n	a5, a1, 12
     d03:	53bdf0        	max	a11, a13, a15
     d06:	972242        	l32i	a4, a2, 0x25c
     d09:	535e50        	max	a5, a14, a5
     d0c:	43ab50        	min	a10, a11, a5
     d0f:	fcf851        	l32r	a5, f0 (4738 <airoha_divide_limit_1>)
     d12:	0005e0        	callx8	a5
     d15:	76c452        	addi	a5, a4, 118
     d18:	600364        	ae_lq32f.i	aeq0, a3, 24
     d1b:	4c0244        	ae_lp24.i	aep0, a2, 16
     d1e:	379aa4        	ae_truncp24a32x2	aep1, a10, a10
     d21:	e207a0226e 	{ l16si	a6, a2, 4; ae_mulafp24s.ll	aeq0, aep1, aep0 }
     d26:	f966aff55e 	{ l16si	a5, a5, 0x1fe; ae_movp48	aep2, aep1 }
     d2b:	fc2460b27e 	{ l16si	a7, a2, 22; ae_roundsq32asym	aeq0, aeq0 }
     d30:	620364        	ae_sq32f.i	aeq0, a3, 24
     d33:	1fa657        	bge	a6, a5, d56 <aec_post_calc_FB+0x7e>
     d36:	78c452        	addi	a5, a4, 120
     d39:	04d462        	addmi	a6, a4, 0x400
     d3c:	0a9222        	l16si	a2, a2, 20
     d3f:	ff9552        	l16si	a5, a5, 0x1fe
     d42:	269662        	l16si	a6, a6, 76
     d45:	722b           	addi.n	a7, a2, 2
     d47:	725650        	salt	a5, a6, a5
     d4a:	837250        	moveqz	a7, a2, a5
     d4d:	031674        	wur.ae_sar	a7
     d50:	0b10a4        	ae_sllssp24s	aep2, aep1
     d53:	000506        	j	d6b <aec_post_calc_FB+0x93>

00000d56 <aec_post_calc_FB+0x7e>:
     d56:	072d           	mov.n	a2, a7
     d58:	0f27a6        	blti	a7, 2, d6b <aec_post_calc_FB+0x93>
     d5b:	922d           	ae_zext16	a2, a2
     d5d:	f9668ff22e 	{ addi	a2, a2, -1; ae_movp48	aep2, aep1 }
     d62:	058276        	loop	a2, d6b <aec_post_calc_FB+0x93>
     d65:	012174        	ae_mulfp24s.ll	aeq0, aep2, aep1
     d68:	1b8d24        	ae_roundsp24q48asym	aep2, aeq0

00000d6b <aec_post_calc_FB+0x93>:
     d6b:	6ea222        	movi	a2, 0x26e
     d6e:	411424        	ae_lp16f.x	aep1, a4, a2
     d71:	d82a7d037f 	{ ae_lq32f.i	aeq0, a3, 28; ae_maxp24s	aep1, aep2, aep1 }
     d76:	011034        	ae_mulafp24s.ll	aeq0, aep1, aep0
     d79:	060934        	ae_roundsq32asym	aeq0, aeq0
     d7c:	620374        	ae_sq32f.i	aeq0, a3, 28
     d7f:	f01d           	retw.n

00000d81 <aec_post_calc_FB+0xa9>:
     d81:	000000                                        ...

00000d84 <subband_aec_block>:
     d84:	006136        	entry	a1, 48
     d87:	a3e8           	l32i.n	a14, a3, 40
     d89:	fd24a1137e 	{ l16si	a7, a3, 34; ae_zeroq56	aeq1 }
     d8e:	f41b           	addi.n	a15, a4, 1
     d90:	3169           	s32i.n	a6, a1, 12
     d92:	a388           	l32i.n	a8, a3, 40
     d94:	9067e0        	addx2	a6, a7, a14
     d97:	b3f440        	movgez	a15, a4, a4
     d9a:	21a1f0        	srai	a10, a15, 1
     d9d:	f3f680        	wur.ae_cbegin0	a8
     da0:	f3f760        	wur.ae_cend0	a6
     da3:	2e24a6        	blti	a4, 2, dd5 <subband_aec_block+0x51>
     da6:	942292        	l32i	a9, a2, 0x250
     da9:	e378           	l32i.n	a7, a3, 56
     dab:	c368           	l32i.n	a6, a3, 48
     dad:	600344        	ae_lq32f.i	aeq0, a3, 16
     db0:	480c           	movi.n	a8, 4
     db2:	440704        	ae_lp16x2f.i	aep0, a7, 0
     db5:	fcc992        	addi	a9, a9, -4
     db8:	198a76        	loop	a10, dd5 <subband_aec_block+0x51>
     dbb:	09001aa68f 	{ ae_lp16x2f.c	aep2, a6, a8; ae_mulzaafq32sp16s.hh	aeq2, aeq0, aep0, aeq0, aep0 }
     dc0:	11006d871f 	{ ae_lp16x2f.iu	aep0, a7, 4; ae_mulzaafq32sp16s.ll	aeq3, aeq0, aep0, aeq0, aep0 }
     dc5:	1baf74        	ae_roundsp16q48asym	aep7, aeq2
     dc8:	1bbf14        	ae_roundsp16q48asym	aep1, aeq3
     dcb:	f04a7e715f 	{ ae_selp24.hl	aep1, aep7, aep1; ae_mulssfp24s.hh.ll	aeq1, aep2, aep2 }
     dd0:	e22473991f 	{ ae_sp16x2f.iu	aep1, a9, 4; ae_mulaafp24s.hh.ll	aeq1, aep1, aep1 }

00000dd5 <subband_aec_block+0x51>:
     dd5:	351234        	ae_sraiq56	aeq0, aeq1, 8
     dd8:	618304        	ae_lq56.i	aeq2, a3, 0
     ddb:	f87230926e 	{ l32i	a6, a2, 36; ae_addq56	aeq0, aeq2, aeq0 }
     de0:	fc00fec6ff 	{ ae_cvtq48a32s	aeq3, a6; ae_satq48s	aeq0, aeq0 }
     de5:	f87b1421af 	{ s32i.n	a10, a1, 8; ae_maxq56s	aeq0, aeq0, aeq3 }
     dea:	1129           	s32i.n	a2, a1, 4
     dec:	630304        	ae_sq56s.i	aeq0, a3, 0
     def:	00e5d6        	bgez	a5, e01 <subband_aec_block+0x7d>
     df2:	109362        	l16si	a6, a3, 32
     df5:	368b04        	ae_trunca32q48	a11, aeq0
     df8:	11a670        	slli	a10, a6, 9
     dfb:	fcbe61        	l32r	a6, f4 (4630 <airoha_divide>)
     dfe:	0006e0        	callx8	a6

00000e01 <subband_aec_block+0x7d>:
     e01:	c378           	l32i.n	a7, a3, 48
     e03:	7614a6        	blti	a4, 1, e7d <subband_aec_block+0xf9>
     e06:	1128           	l32i.n	a2, a1, 4
     e08:	f3c8           	l32i.n	a12, a3, 60
     e0a:	d3d8           	l32i.n	a13, a3, 52
     e0c:	9422f2        	l32i	a15, a2, 0x250
     e0f:	b368           	l32i.n	a6, a3, 44
     e11:	119382        	l16si	a8, a3, 34
     e14:	360b34        	ae_cvtq48a32s	aeq0, a11
     e17:	290c           	movi.n	a9, 2
     e19:	fec6b2        	addi	a11, a6, -2
     e1c:	feccc2        	addi	a12, a12, -2
     e1f:	fecdd2        	addi	a13, a13, -2
     e22:	04ed           	mov.n	a14, a4
     e24:	fecff2        	addi	a15, a15, -2
     e27:	408f14        	ae_lp16f.iu	aep0, a15, 2
     e2a:	fc24e2879f 	{ ae_sp16f.l.c	aep0, a7, a9; nop }
     e2f:	408d14        	ae_lp16f.iu	aep0, a13, 2
     e32:	fd0c136bbf 	{ or	a6, a11, a11; ae_cvtq48p24s.l	aeq1, aep0 }
     e37:	0f18a6        	blti	a8, 1, e4a <subband_aec_block+0xc6>
     e3a:	408614        	ae_lp16f.iu	aep0, a6, 2
     e3d:	098876        	loop	a8, e4a <subband_aec_block+0xc6>
     e40:	fc24d9179f 	{ ae_lp16f.c	aep1, a7, a9; nop }
     e45:	ee21ec861f 	{ ae_lp16f.iu	aep0, a6, 2; ae_mulsfp24s.ll	aeq1, aep0, aep1 }

00000e4a <subband_aec_block+0xc6>:
     e4a:	f86b936bbf 	{ or	a6, a11, a11; ae_roundsp16q48asym	aep0, aeq1 }
     e4f:	d4e2728c1f 	{ ae_sp16f.l.iu	aep0, a12, 2; ae_mulfq32sp16s.l	aeq1, aeq0, aep0 }
     e54:	0205d6        	bgez	a5, e78 <subband_aec_block+0xf4>
     e57:	fc24fe9a4f 	{ ae_slaasq56s	aeq1, aeq1, a10; nop }
     e5c:	1b9d04        	ae_roundsp24q48asym	aep0, aeq1
     e5f:	1518a6        	blti	a8, 1, e78 <subband_aec_block+0xf4>
     e62:	409614        	ae_lp16f.iu	aep1, a6, 2
     e65:	0f8876        	loop	a8, e78 <subband_aec_block+0xf4>
     e68:	fd2c19779f 	{ ae_lp16f.c	aep7, a7, a9; ae_cvtq48p24s.l	aeq1, aep1 }
     e6d:	e2e3ec961f 	{ ae_lp16f.iu	aep1, a6, 2; ae_mulafp24s.ll	aeq1, aep0, aep7 }
     e72:	1b9f24        	ae_roundsp16q48asym	aep2, aeq1
     e75:	4226f4        	ae_sp16f.l.i	aep2, a6, -2
     e78:	ee0b           	addi.n	a14, a14, -1
     e7a:	fa9e56        	bnez	a14, e27 <subband_aec_block+0xa3>

00000e7d <subband_aec_block+0xf9>:
     e7d:	fe2494c37f 	{ s32i.n	a7, a3, 48; ae_zeroq56	aeq2 }
     e82:	fc24bf217f 	{ ae_movq56	aeq1, aeq2; ae_zeroq56	aeq0 }
     e87:	1c24a6        	blti	a4, 2, ea7 <subband_aec_block+0x123>
     e8a:	d368           	l32i.n	a6, a3, 52
     e8c:	f378           	l32i.n	a7, a3, 60
     e8e:	2128           	l32i.n	a2, a1, 8
     e90:	fcc662        	addi	a6, a6, -4
     e93:	fcc772        	addi	a7, a7, -4
     e96:	0d8276        	loop	a2, ea7 <subband_aec_block+0x123>
     e99:	448614        	ae_lp16x2f.iu	aep0, a6, 4
     e9c:	e2006d971f 	{ ae_lp16x2f.iu	aep1, a7, 4; ae_mulaafp24s.hh.ll	aeq1, aep0, aep0 }
     ea1:	050104        	ae_mulaafp24s.hh.ll	aeq0, aep0, aep1
     ea4:	059104        	ae_mulaafp24s.hh.ll	aeq2, aep1, aep1

00000ea7 <subband_aec_block+0x123>:
     ea7:	31f8           	l32i.n	a15, a1, 12
     ea9:	1198           	l32i.n	a9, a1, 4
     eab:	020c           	movi.n	a2, 0
     ead:	060c           	movi.n	a6, 0
     eaf:	2129           	s32i.n	a2, a1, 8
     eb1:	0f4fa6        	blti	a15, 4, ec4 <subband_aec_block+0x140>
     eb4:	35e134        	ae_sraiq56	aeq3, aeq2, 4
     eb7:	fab6c0006f 	{ movi	a6, 0; ae_subq56	aeq2, aeq2, aeq3 }
     ebc:	d65400012f 	{ movi	a2, 1; ae_ltq56s	b0, aeq1, aeq2 }
     ec1:	d36200        	movt	a6, a2, b0

00000ec4 <subband_aec_block+0x140>:
     ec4:	355134        	ae_sraiq56	aeq1, aeq1, 4
     ec7:	fd2c7b010f 	{ ae_sraiq56	aeq0, aeq0, 4; ae_roundsq32asym	aeq1, aeq1 }
     ecc:	fc247f17ef 	{ ae_trunca32q48	a7, aeq1; ae_roundsq32asym	aeq0, aeq0 }
     ed1:	240382        	l8ui	a8, a3, 36
     ed4:	368204        	ae_trunca32q48	a2, aeq0
     ed7:	0f0c           	movi.n	a15, 0
     ed9:	076827        	bbci	a8, 2, ee4 <subband_aec_block+0x160>
     edc:	972982        	l32i	a8, a9, 0x25c
     edf:	628f80        	saltu	a8, a15, a8
     ee2:	2189           	s32i.n	a8, a1, 8
     ee4:	fc8581        	l32r	a8, f8 (b34 <common_x_pow>)
     ee7:	03ad           	mov.n	a10, a3
     ee9:	07bd           	mov.n	a11, a7
     eeb:	02cd           	mov.n	a12, a2
     eed:	0008e0        	callx8	a8
     ef0:	2188           	l32i.n	a8, a1, 8
     ef2:	6a6a           	add.n	a6, a10, a6
     ef4:	48ac           	beqz.n	a8, f1c <subband_aec_block+0x198>
     ef6:	221666        	bnei	a6, 1, f1c <subband_aec_block+0x198>
     ef9:	3168           	l32i.n	a6, a1, 12
     efb:	1188           	l32i.n	a8, a1, 4
     efd:	0b46e6        	bgei	a6, 4, f0c <subband_aec_block+0x188>
     f00:	972862        	l32i	a6, a8, 0x25c
     f03:	76c662        	addi	a6, a6, 118
     f06:	ff9662        	l16si	a6, a6, 0x1fe
     f09:	1d16e6        	bgei	a6, 1, f2a <subband_aec_block+0x1a6>
     f0c:	212220        	srai	a2, a2, 2
     f0f:	602120        	abs	a2, a2
     f12:	902220        	addx2	a2, a2, a2
     f15:	05a277        	bge	a2, a7, f1e <subband_aec_block+0x19a>
     f18:	000386        	j	f2a <subband_aec_block+0x1a6>

00000f1b <subband_aec_block+0x197>:
	...

00000f1c <subband_aec_block+0x198>:
     f1c:	a68c           	beqz.n	a6, f2a <subband_aec_block+0x1a6>
     f1e:	f3a8           	l32i.n	a10, a3, 60
     f20:	d3b8           	l32i.n	a11, a3, 52
     f22:	fc3e21        	l32r	a2, 1c (6c04 <xt_memcpy>)
     f25:	04cd           	mov.n	a12, a4
     f27:	0002e0        	callx8	a2
     f2a:	1128           	l32i.n	a2, a1, 4
     f2c:	fc24a0d22e 	{ l16si	a2, a2, 26; ae_zeroq56	aeq0 }
     f31:	2122a6        	blti	a2, 2, f56 <subband_aec_block+0x1d2>
     f34:	b348           	l32i.n	a4, a3, 44
     f36:	119362        	l16si	a6, a3, 34
     f39:	e21b           	addi.n	a14, a2, 1
     f3b:	b3e220        	movgez	a14, a2, a2
     f3e:	904640        	addx2	a4, a6, a4
     f41:	11f2f0        	slli	a15, a2, 1
     f44:	c024f0        	sub	a2, a4, a15
     f47:	2161e0        	srai	a6, a14, 1
     f4a:	fcc222        	addi	a2, a2, -4
     f4d:	058676        	loop	a6, f56 <subband_aec_block+0x1d2>
     f50:	448214        	ae_lp16x2f.iu	aep0, a2, 4
     f53:	050004        	ae_mulaafp24s.hh.ll	aeq0, aep0, aep0

00000f56 <subband_aec_block+0x1d2>:
     f56:	3328           	l32i.n	a2, a3, 12
     f58:	060c           	movi.n	a6, 0
     f5a:	3500b4        	ae_sraiq56	aeq0, aeq0, 2
     f5d:	53a260        	max	a10, a2, a6
     f60:	fc247023be 	{ l32i	a11, a3, 8; ae_roundsq32asym	aeq0, aeq0 }
     f65:	fc6221        	l32r	a2, f0 (4738 <airoha_divide_limit_1>)
     f68:	368404        	ae_trunca32q48	a4, aeq0
     f6b:	0002e0        	callx8	a2
     f6e:	1198           	l32i.n	a9, a1, 4
     f70:	ffa022        	movi	a2, 255
     f73:	2403f2        	l8ui	a15, a3, 36
     f76:	049962        	l16si	a6, a9, 8
     f79:	7fd222        	addmi	a2, a2, 0x7f00
     f7c:	3170a0        	srai	a7, a10, 16
     f7f:	c02270        	sub	a2, a2, a7
     f82:	532260        	max	a2, a2, a6
     f85:	05ef17        	bbsi	a15, 1, f8e <subband_aec_block+0x20a>
     f88:	105322        	s16i	a2, a3, 32
     f8b:	000786        	j	fad <subband_aec_block+0x229>

00000f8e <subband_aec_block+0x20a>:
     f8e:	039962        	l16si	a6, a9, 6
     f91:	09ad           	mov.n	a10, a9
     f93:	03bd           	mov.n	a11, a3
     f95:	822260        	mull	a2, a2, a6
     f98:	04cd           	mov.n	a12, a4
     f9a:	40d222        	addmi	a2, a2, 0x4000
     f9d:	412f20        	srli	a2, a2, 15
     fa0:	105322        	s16i	a2, a3, 32
     fa3:	fc5621        	l32r	a2, fc (b9c <ref_gain_adapt>)
     fa6:	05dd           	mov.n	a13, a5
     fa8:	0002e0        	callx8	a2
     fab:	1198           	l32i.n	a9, a1, 4
     fad:	059922        	l16si	a2, a9, 10
     fb0:	069972        	l16si	a7, a9, 12
     fb3:	fc4ef1        	l32r	a15, ec (7800 <immediate_num>)
     fb6:	112200        	slli	a2, a2, 16
     fb9:	56a247        	bge	a2, a4, 1013 <subband_aec_block+0x28f>
     fbc:	129322        	l16si	a2, a3, 36
     fbf:	260c           	movi.n	a6, 2
     fc1:	11e700        	slli	a14, a7, 16
     fc4:	202260        	or	a2, a2, a6
     fc7:	125322        	s16i	a2, a3, 36
     fca:	18cf22        	addi	a2, a15, 24
     fcd:	07ae47        	bge	a14, a4, fd8 <subband_aec_block+0x254>
     fd0:	400204        	ae_lp16f.i	aep0, a2, 0
     fd3:	000106        	j	fdb <subband_aec_block+0x257>

00000fd6 <subband_aec_block+0x252>:
	...

00000fd8 <subband_aec_block+0x254>:
     fd8:	400214        	ae_lp16f.i	aep0, a2, 2
     fdb:	0d9922        	l16si	a2, a9, 26
     fde:	3122a6        	blti	a2, 2, 1013 <subband_aec_block+0x28f>
     fe1:	b348           	l32i.n	a4, a3, 44
     fe3:	119362        	l16si	a6, a3, 34
     fe6:	e21b           	addi.n	a14, a2, 1
     fe8:	b3e220        	movgez	a14, a2, a2
     feb:	904640        	addx2	a4, a6, a4
     fee:	11f2f0        	slli	a15, a2, 1
     ff1:	c024f0        	sub	a2, a4, a15
     ff4:	2161e0        	srai	a6, a14, 1
     ff7:	441204        	ae_lp16x2f.i	aep1, a2, 0
     ffa:	fec242        	addi	a4, a2, -2
     ffd:	128676        	loop	a6, 1013 <subband_aec_block+0x28f>
    1000:	011054        	ae_mulfp24s.hl	aeq0, aep1, aep0
    1003:	ea05ed921f 	{ ae_lp16x2f.iu	aep1, a2, 4; ae_mulfp24s.ll	aeq1, aep1, aep0 }
    1008:	1b8f24        	ae_roundsp16q48asym	aep2, aeq0
    100b:	fb6bf2a41f 	{ ae_sp16f.l.iu	aep2, a4, 2; ae_roundsp16q48asym	aep7, aeq1 }
    1010:	42f414        	ae_sp16f.l.iu	aep7, a4, 2

00001013 <subband_aec_block+0x28f>:
    1013:	121322        	l16ui	a2, a3, 36
    1016:	31c8           	l32i.n	a12, a1, 12
    1018:	2148           	l32i.n	a4, a1, 8
    101a:	33e207        	bbsi	a2, 0, 1051 <subband_aec_block+0x2cd>
    101d:	3178           	l32i.n	a7, a1, 12
    101f:	84bc           	beqz.n	a4, 105b <subband_aec_block+0x2d7>
    1021:	972942        	l32i	a4, a9, 0x25c
    1024:	04d462        	addmi	a6, a4, 0x400
    1027:	241642        	l16ui	a4, a6, 72
    102a:	00d5d6        	bgez	a5, 103b <subband_aec_block+0x2b7>
    102d:	0a1766        	bnei	a7, 1, 103b <subband_aec_block+0x2b7>
    1030:	ffa552        	movi	a5, 0x5ff
    1033:	0c3547        	bltu	a5, a4, 1043 <subband_aec_block+0x2bf>
    1036:	441b           	addi.n	a4, a4, 1
    1038:	245642        	s16i	a4, a6, 72

0000103b <subband_aec_block+0x2b7>:
    103b:	944d           	ae_zext16	a4, a4
    103d:	00a652        	movi	a5, 0x600
    1040:	073457        	bltu	a4, a5, 104b <subband_aec_block+0x2c7>
    1043:	240c           	movi.n	a4, 2
    1045:	202240        	or	a2, a2, a4
    1048:	125322        	s16i	a2, a3, 36
    104b:	fc2e21        	l32r	a2, 104 (cd8 <aec_post_calc_FB>)
    104e:	000086        	j	1054 <subband_aec_block+0x2d0>

00001051 <subband_aec_block+0x2cd>:
    1051:	fc2b21        	l32r	a2, 100 (c34 <aec_post_calc>)
    1054:	09ad           	mov.n	a10, a9
    1056:	03bd           	mov.n	a11, a3
    1058:	0002e0        	callx8	a2
    105b:	f01d           	retw.n

0000105d <subband_aec_block+0x2d9>:
    105d:	000000                                        ...

00001060 <subband_aec_mid_channel>:
    1060:	006136        	entry	a1, 48
    1063:	a3e8           	l32i.n	a14, a3, 40
    1065:	fd24a1137e 	{ l16si	a7, a3, 34; ae_zeroq56	aeq1 }
    106a:	3159           	s32i.n	a5, a1, 12
    106c:	a398           	l32i.n	a9, a3, 40
    106e:	e358           	l32i.n	a5, a3, 56
    1070:	9422f2        	l32i	a15, a2, 0x250
    1073:	f3f690        	wur.ae_cbegin0	a9
    1076:	c398           	l32i.n	a9, a3, 48
    1078:	a077e0        	addx4	a7, a7, a14
    107b:	0aa082        	movi	a8, 10
    107e:	f3f770        	wur.ae_cend0	a7
    1081:	04a062        	movi	a6, 4
    1084:	600344        	ae_lq32f.i	aeq0, a3, 16
    1087:	440504        	ae_lp16x2f.i	aep0, a5, 0
    108a:	fccf72        	addi	a7, a15, -4
    108d:	198876        	loop	a8, 10aa <subband_aec_mid_channel+0x4a>
    1090:	09001aa96f 	{ ae_lp16x2f.c	aep2, a9, a6; ae_mulzaafq32sp16s.hh	aeq2, aeq0, aep0, aeq0, aep0 }
    1095:	11006d851f 	{ ae_lp16x2f.iu	aep0, a5, 4; ae_mulzaafq32sp16s.ll	aeq3, aeq0, aep0, aeq0, aep0 }
    109a:	1baf74        	ae_roundsp16q48asym	aep7, aeq2
    109d:	1bbf14        	ae_roundsp16q48asym	aep1, aeq3
    10a0:	f04a7e715f 	{ ae_selp24.hl	aep1, aep7, aep1; ae_mulssfp24s.hh.ll	aeq1, aep2, aep2 }
    10a5:	e22473971f 	{ ae_sp16x2f.iu	aep1, a7, 4; ae_mulaafp24s.hh.ll	aeq1, aep1, aep1 }

000010aa <subband_aec_mid_channel+0x4a>:
    10aa:	9258           	l32i.n	a5, a2, 36
    10ac:	3511b4        	ae_sraiq56	aeq0, aeq1, 6
    10af:	618304        	ae_lq56.i	aeq2, a3, 0
    10b2:	f87221036e 	{ l16si	a6, a3, 32; ae_addq56	aeq0, aeq2, aeq0 }
    10b7:	1155e0        	slli	a5, a5, 2
    10ba:	fc00fec5ff 	{ ae_cvtq48a32s	aeq3, a5; ae_satq48s	aeq0, aeq0 }
    10bf:	f87b13533f 	{ or	a5, a3, a3; ae_maxq56s	aeq0, aeq0, aeq3 }
    10c4:	630304        	ae_sq56s.i	aeq0, a3, 0
    10c7:	00d4d6        	bgez	a4, 10d8 <subband_aec_mid_channel+0x78>
    10ca:	368b04        	ae_trunca32q48	a11, aeq0
    10cd:	11a650        	slli	a10, a6, 11
    10d0:	fc0931        	l32r	a3, f4 (4630 <airoha_divide>)
    10d3:	0003e0        	callx8	a3
    10d6:	053d           	mov.n	a3, a5

000010d8 <subband_aec_mid_channel+0x78>:
    10d8:	d3c8           	l32i.n	a12, a3, 52
    10da:	9422e2        	l32i	a14, a2, 0x250
    10dd:	f388           	l32i.n	a8, a3, 60
    10df:	c368           	l32i.n	a6, a3, 48
    10e1:	119372        	l16si	a7, a3, 34
    10e4:	b338           	l32i.n	a3, a3, 44
    10e6:	2129           	s32i.n	a2, a1, 8
    10e8:	360b34        	ae_cvtq48a32s	aeq0, a11
    10eb:	a90c           	movi.n	a9, 10
    10ed:	04a0b2        	movi	a11, 4
    10f0:	fcccc2        	addi	a12, a12, -4
    10f3:	fec8d2        	addi	a13, a8, -2
    10f6:	fccee2        	addi	a14, a14, -4
    10f9:	fec322        	addi	a2, a3, -2
    10fc:	448e14        	ae_lp16x2f.iu	aep0, a14, 4
    10ff:	fc24e406bf 	{ ae_sp16x2f.c	aep0, a6, a11; nop }
    1104:	448c14        	ae_lp16x2f.iu	aep0, a12, 4
    1107:	fe0413833f 	{ or	a8, a3, a3; ae_cvtq48p24s.h	aeq2, aep0 }
    110c:	0a4084        	ae_cvtq48p24s.l	aeq1, aep0
    110f:	1217a6        	blti	a7, 1, 1125 <subband_aec_mid_channel+0xc5>
    1112:	440304        	ae_lp16x2f.i	aep0, a3, 0
    1115:	0c8776        	loop	a7, 1125 <subband_aec_mid_channel+0xc5>
    1118:	fc24da96bf 	{ ae_lp16x2f.c	aep1, a6, a11; nop }
    111d:	0581c4        	ae_mulssfp24s.hh.ll	aeq2, aep0, aep1
    1120:	ee206d881f 	{ ae_lp16x2f.iu	aep0, a8, 4; ae_mulsafp24s.hl.lh	aeq1, aep0, aep1 }

00001125 <subband_aec_mid_channel+0xc5>:
    1125:	1baf04        	ae_roundsp16q48asym	aep0, aeq2
    1128:	f86bf28d1f 	{ ae_sp16f.l.iu	aep0, a13, 2; ae_roundsp16q48asym	aep1, aeq1 }
    112d:	d4e2729d1f 	{ ae_sp16f.l.iu	aep1, a13, 2; ae_mulfq32sp16s.l	aeq1, aeq0, aep0 }
    1132:	0364d6        	bgez	a4, 116c <subband_aec_mid_channel+0x10c>
    1135:	d5e63e9a4f 	{ ae_slaasq56s	aeq1, aeq1, a10; ae_mulfq32sp16s.l	aeq2, aeq0, aep1 }
    113a:	038d           	mov.n	a8, a3
    113c:	f86fbeaa8f 	{ ae_slaasq56s	aeq2, aeq2, a10; ae_roundsp24q48asym	aep0, aeq1 }
    1141:	f877d3f22f 	{ or	a15, a2, a2; ae_roundsp24q48asym	aep1, aeq2 }
    1146:	2217a6        	blti	a7, 1, 116c <subband_aec_mid_channel+0x10c>
    1149:	dc20ad130f 	{ ae_lp16x2f.i	aep1, a3, 0; ae_selp24.hl	aep0, aep0, aep1 }
    114e:	1a8776        	loop	a7, 116c <subband_aec_mid_channel+0x10c>
    1151:	fd241aa6bf 	{ ae_lp16x2f.c	aep2, a6, a11; ae_cvtq48p24s.h	aeq1, aep1 }
    1156:	fe2c2d981f 	{ ae_lp16x2f.iu	aep1, a8, 4; ae_cvtq48p24s.l	aeq2, aep1 }
    115b:	050a04        	ae_mulaafp24s.hh.ll	aeq1, aep0, aep2
    115e:	058254        	ae_mulasfp24s.hl.lh	aeq2, aep0, aep2
    1161:	1b9f64        	ae_roundsp16q48asym	aep6, aeq1
    1164:	fb73f2ef1f 	{ ae_sp16f.l.iu	aep6, a15, 2; ae_roundsp16q48asym	aep7, aeq2 }
    1169:	42ff14        	ae_sp16f.l.iu	aep7, a15, 2
    116c:	990b           	addi.n	a9, a9, -1
    116e:	f8a956        	bnez	a9, 10fc <subband_aec_mid_channel+0x9c>
    1171:	fd24b0d57e 	{ l32i	a7, a5, 52; ae_zeroq56	aeq1 }
    1176:	f528           	l32i.n	a2, a5, 60
    1178:	c569           	s32i.n	a6, a5, 48
    117a:	a30c           	movi.n	a3, 10
    117c:	208550        	or	a8, a5, a5
    117f:	fe0c6d070f 	{ ae_lp16x2f.i	aep0, a7, 0; ae_movq56	aeq2, aeq1 }
    1184:	fc0c6d120f 	{ ae_lp16x2f.i	aep1, a2, 0; ae_movq56	aeq0, aeq1 }
    1189:	0c8376        	loop	a3, 1199 <subband_aec_mid_channel+0x139>
    118c:	058104        	ae_mulaafp24s.hh.ll	aeq2, aep0, aep1
    118f:	ea202d921f 	{ ae_lp16x2f.iu	aep1, a2, 4; ae_mulasfp24s.hl.lh	aeq0, aep0, aep1 }
    1194:	e2006d871f 	{ ae_lp16x2f.iu	aep0, a7, 4; ae_mulaafp24s.hh.ll	aeq1, aep0, aep0 }

00001199 <subband_aec_mid_channel+0x139>:
    1199:	35a134        	ae_sraiq56	aeq2, aeq2, 4
    119c:	f877bb010f 	{ ae_sraiq56	aeq0, aeq0, 4; ae_roundsp24q48asym	aep0, aeq2 }
    11a1:	f867fb114f 	{ ae_sraiq56	aeq1, aeq1, 4; ae_roundsp24q48asym	aep1, aeq0 }
    11a6:	fd2c7e011f 	{ ae_selp24.hl	aep0, aep0, aep1; ae_roundsq32asym	aeq1, aeq1 }
    11ab:	f1028000af 	{ movi	a10, 0; ae_mulzaafp24s.hh.ll	aeq2, aep0, aep0 }
    11b0:	fb9f21        	l32r	a2, 2c (514 <HIFI_sqrt>)
    11b3:	ff347f16ef 	{ ae_trunca32q48	a6, aeq1; ae_roundsq32asym	aeq3, aeq2 }
    11b8:	36bb04        	ae_trunca32q48	a11, aeq3
    11bb:	070c           	movi.n	a7, 0
    11bd:	0002e0        	callx8	a2
    11c0:	360b34        	ae_cvtq48a32s	aeq0, a11
    11c3:	fc24fe8a0f 	{ ae_slaasq56s	aeq0, aeq0, a10; nop }
    11c8:	fc2453955f 	{ or	a9, a5, a5; ae_roundsq32asym	aeq0, aeq0 }
    11cd:	240522        	l8ui	a2, a5, 36
    11d0:	368c04        	ae_trunca32q48	a12, aeq0
    11d3:	2188           	l32i.n	a8, a1, 8
    11d5:	0ee227        	bbsi	a2, 2, 11e7 <subband_aec_mid_channel+0x187>
    11d8:	fbc821        	l32r	a2, f8 (b34 <common_x_pow>)
    11db:	09ad           	mov.n	a10, a9
    11dd:	06bd           	mov.n	a11, a6
    11df:	0002e0        	callx8	a2
    11e2:	059d           	mov.n	a9, a5
    11e4:	000806        	j	1208 <subband_aec_mid_channel+0x1a8>

000011e7 <subband_aec_mid_channel+0x187>:
    11e7:	fbc431        	l32r	a3, f8 (b34 <common_x_pow>)
    11ea:	09ad           	mov.n	a10, a9
    11ec:	972822        	l32i	a2, a8, 0x25c
    11ef:	06bd           	mov.n	a11, a6
    11f1:	0003e0        	callx8	a3
    11f4:	059d           	mov.n	a9, a5
    11f6:	e28c           	beqz.n	a2, 1208 <subband_aec_mid_channel+0x1a8>
    11f8:	2128           	l32i.n	a2, a1, 8
    11fa:	170c           	movi.n	a7, 1
    11fc:	972222        	l32i	a2, a2, 0x25c
    11ff:	76c222        	addi	a2, a2, 118
    1202:	ff9222        	l16si	a2, a2, 0x1fe
    1205:	0f12e6        	bgei	a2, 1, 1218 <subband_aec_mid_channel+0x1b8>
    1208:	ca8c           	beqz.n	a10, 1218 <subband_aec_mid_channel+0x1b8>
    120a:	f9a8           	l32i.n	a10, a9, 60
    120c:	d9b8           	l32i.n	a11, a9, 52
    120e:	fb8321        	l32r	a2, 1c (6c04 <xt_memcpy>)
    1211:	4c1c           	movi.n	a12, 20
    1213:	0002e0        	callx8	a2
    1216:	059d           	mov.n	a9, a5
    1218:	2128           	l32i.n	a2, a1, 8
    121a:	fc24a0d22e 	{ l16si	a2, a2, 26; ae_zeroq56	aeq0 }
    121f:	1712a6        	blti	a2, 1, 123a <subband_aec_mid_channel+0x1da>
    1222:	119932        	l16si	a3, a9, 34
    1225:	0b2962        	l32i	a6, a9, 44
    1228:	c03320        	sub	a3, a3, a2
    122b:	a03360        	addx4	a3, a3, a6
    122e:	fcc332        	addi	a3, a3, -4
    1231:	058276        	loop	a2, 123a <subband_aec_mid_channel+0x1da>
    1234:	448314        	ae_lp16x2f.iu	aep0, a3, 4
    1237:	050004        	ae_mulaafp24s.hh.ll	aeq0, aep0, aep0

0000123a <subband_aec_mid_channel+0x1da>:
    123a:	3500b4        	ae_sraiq56	aeq0, aeq0, 2
    123d:	fc247039ae 	{ l32i	a10, a9, 12; ae_roundsq32asym	aeq0, aeq0 }
    1242:	29b8           	l32i.n	a11, a9, 8
    1244:	fbab21        	l32r	a2, f0 (4738 <airoha_divide_limit_1>)
    1247:	368604        	ae_trunca32q48	a6, aeq0
    124a:	0002e0        	callx8	a2
    124d:	2198           	l32i.n	a9, a1, 8
    124f:	ffa022        	movi	a2, 255
    1252:	2405f2        	l8ui	a15, a5, 36
    1255:	049932        	l16si	a3, a9, 8
    1258:	7fd222        	addmi	a2, a2, 0x7f00
    125b:	3180a0        	srai	a8, a10, 16
    125e:	c02280        	sub	a2, a2, a8
    1261:	05bd           	mov.n	a11, a5
    1263:	532230        	max	a2, a2, a3
    1266:	05ef17        	bbsi	a15, 1, 126f <subband_aec_mid_channel+0x20f>
    1269:	105b22        	s16i	a2, a11, 32
    126c:	000786        	j	128e <subband_aec_mid_channel+0x22e>

0000126f <subband_aec_mid_channel+0x20f>:
    126f:	039932        	l16si	a3, a9, 6
    1272:	09ad           	mov.n	a10, a9
    1274:	06cd           	mov.n	a12, a6
    1276:	822230        	mull	a2, a2, a3
    1279:	04dd           	mov.n	a13, a4
    127b:	40d222        	addmi	a2, a2, 0x4000
    127e:	412f20        	srli	a2, a2, 15
    1281:	105b22        	s16i	a2, a11, 32
    1284:	fb9e21        	l32r	a2, fc (b9c <ref_gain_adapt>)
    1287:	0002e0        	callx8	a2
    128a:	2198           	l32i.n	a9, a1, 8
    128c:	05bd           	mov.n	a11, a5
    128e:	059922        	l16si	a2, a9, 10
    1291:	069942        	l16si	a4, a9, 12
    1294:	fb96f1        	l32r	a15, ec (7800 <immediate_num>)
    1297:	112200        	slli	a2, a2, 16
    129a:	49a267        	bge	a2, a6, 12e7 <subband_aec_mid_channel+0x287>
    129d:	129b22        	l16si	a2, a11, 36
    12a0:	230c           	movi.n	a3, 2
    12a2:	11e400        	slli	a14, a4, 16
    12a5:	202230        	or	a2, a2, a3
    12a8:	125b22        	s16i	a2, a11, 36
    12ab:	18cf22        	addi	a2, a15, 24
    12ae:	05ae67        	bge	a14, a6, 12b7 <subband_aec_mid_channel+0x257>
    12b1:	400204        	ae_lp16f.i	aep0, a2, 0
    12b4:	000086        	j	12ba <subband_aec_mid_channel+0x25a>

000012b7 <subband_aec_mid_channel+0x257>:
    12b7:	400214        	ae_lp16f.i	aep0, a2, 2
    12ba:	0d9942        	l16si	a4, a9, 26
    12bd:	2614a6        	blti	a4, 1, 12e7 <subband_aec_mid_channel+0x287>
    12c0:	119b22        	l16si	a2, a11, 34
    12c3:	bb38           	l32i.n	a3, a11, 44
    12c5:	c02240        	sub	a2, a2, a4
    12c8:	a02230        	addx4	a2, a2, a3
    12cb:	441204        	ae_lp16x2f.i	aep1, a2, 0
    12ce:	fec232        	addi	a3, a2, -2
    12d1:	128476        	loop	a4, 12e7 <subband_aec_mid_channel+0x287>
    12d4:	011054        	ae_mulfp24s.hl	aeq0, aep1, aep0
    12d7:	ea05ed921f 	{ ae_lp16x2f.iu	aep1, a2, 4; ae_mulfp24s.ll	aeq1, aep1, aep0 }
    12dc:	1b8f24        	ae_roundsp16q48asym	aep2, aeq0
    12df:	fb6bf2a31f 	{ ae_sp16f.l.iu	aep2, a3, 2; ae_roundsp16q48asym	aep7, aeq1 }
    12e4:	42f314        	ae_sp16f.l.iu	aep7, a3, 2

000012e7 <subband_aec_mid_channel+0x287>:
    12e7:	121b22        	l16ui	a2, a11, 36
    12ea:	31c8           	l32i.n	a12, a1, 12
    12ec:	00a642        	movi	a4, 0x600
    12ef:	1be207        	bbsi	a2, 0, 130e <subband_aec_mid_channel+0x2ae>
    12f2:	07ac           	beqz.n	a7, 1316 <subband_aec_mid_channel+0x2b6>
    12f4:	972932        	l32i	a3, a9, 0x25c
    12f7:	250c           	movi.n	a5, 2
    12f9:	202250        	or	a2, a2, a5
    12fc:	04d332        	addmi	a3, a3, 0x400
    12ff:	241332        	l16ui	a3, a3, 72
    1302:	023347        	bltu	a3, a4, 1308 <subband_aec_mid_channel+0x2a8>
    1305:	125b22        	s16i	a2, a11, 36

00001308 <subband_aec_mid_channel+0x2a8>:
    1308:	fb7f21        	l32r	a2, 104 (cd8 <aec_post_calc_FB>)
    130b:	000086        	j	1311 <subband_aec_mid_channel+0x2b1>

0000130e <subband_aec_mid_channel+0x2ae>:
    130e:	fb7c21        	l32r	a2, 100 (c34 <aec_post_calc>)
    1311:	09ad           	mov.n	a10, a9
    1313:	0002e0        	callx8	a2
    1316:	f01d           	retw.n

00001318 <dt_detect>:
    1318:	004136        	entry	a1, 32
    131b:	0c9252        	l16si	a5, a2, 24
    131e:	189242        	l16si	a4, a2, 48
    1321:	199262        	l16si	a6, a2, 50
    1324:	115500        	slli	a5, a5, 16
    1327:	19a537        	bge	a5, a3, 1344 <dt_detect+0x2c>
    132a:	154c           	movi.n	a5, 65
    132c:	142657        	blt	a6, a5, 1344 <dt_detect+0x2c>
    132f:	179252        	l16si	a5, a2, 46
    1332:	0e9232        	l16si	a3, a2, 28
    1335:	442b           	addi.n	a4, a4, 2
    1337:	554b           	addi.n	a5, a5, 4
    1339:	435530        	min	a5, a5, a3
    133c:	175252        	s16i	a5, a2, 46
    133f:	855d           	ae_sext16	a5, a5
    1341:	000246        	j	134e <dt_detect+0x36>

00001344 <dt_detect+0x2c>:
    1344:	0e9232        	l16si	a3, a2, 28
    1347:	050c           	movi.n	a5, 0
    1349:	440b           	addi.n	a4, a4, -1
    134b:	175252        	s16i	a5, a2, 46
    134e:	534450        	max	a4, a4, a5
    1351:	433430        	min	a3, a4, a3
    1354:	185232        	s16i	a3, a2, 48
    1357:	f01d           	retw.n

00001359 <dt_detect+0x41>:
    1359:	000000                                        ...

0000135c <dt_detect_FB>:
    135c:	004136        	entry	a1, 32
    135f:	78c252        	addi	a5, a2, 120
    1362:	fe9562        	l16si	a6, a5, 0x1fc
    1365:	174c           	movi.n	a7, 65
    1367:	1e2477        	blt	a4, a7, 1389 <dt_detect_FB+0x2d>
    136a:	72c242        	addi	a4, a2, 114
    136d:	ff9442        	l16si	a4, a4, 0x1fe
    1370:	ff9572        	l16si	a7, a5, 0x1fe
    1373:	74c282        	addi	a8, a2, 116
    1376:	114400        	slli	a4, a4, 16
    1379:	0ca437        	bge	a4, a3, 1389 <dt_detect_FB+0x2d>
    137c:	ff9822        	l16si	a2, a8, 0x1fe
    137f:	474b           	addi.n	a4, a7, 4
    1381:	362b           	addi.n	a3, a6, 2
    1383:	434420        	min	a4, a4, a2
    1386:	000246        	j	1393 <dt_detect_FB+0x37>

00001389 <dt_detect_FB+0x2d>:
    1389:	74c222        	addi	a2, a2, 116
    138c:	ff9222        	l16si	a2, a2, 0x1fe
    138f:	360b           	addi.n	a3, a6, -1
    1391:	040c           	movi.n	a4, 0
    1393:	84fd           	ae_sext16	a15, a4
    1395:	5333f0        	max	a3, a3, a15
    1398:	432320        	min	a2, a3, a2
    139b:	ff5542        	s16i	a4, a5, 0x1fe
    139e:	fe5522        	s16i	a2, a5, 0x1fc
    13a1:	f01d           	retw.n

000013a3 <dt_detect_FB+0x47>:
    13a3:	00000000                                 .....

000013a8 <get_ECNR_SVN>:
    13a8:	004136        	entry	a1, 32
    13ab:	fb5721        	l32r	a2, 108 (1507660 <_end+0x14fefd4>)
    13ae:	f01d           	retw.n

000013b0 <get_aec_nr_memsize>:
    13b0:	004136        	entry	a1, 32
    13b3:	6fa422        	movi	a2, 0x46f
    13b6:	1122c0        	slli	a2, a2, 4
    13b9:	f01d           	retw.n

000013bb <get_aec_nr_memsize+0xb>:
	...

000013bc <get_aec_nr_inear_memsize>:
    13bc:	004136        	entry	a1, 32
    13bf:	2da122        	movi	a2, 0x12d
    13c2:	1122a0        	slli	a2, a2, 6
    13c5:	f01d           	retw.n

000013c7 <get_aec_nr_inear_memsize+0xb>:
	...

000013c8 <sb_ana_init>:
    13c8:	004136        	entry	a1, 32
    13cb:	a24b           	addi.n	a10, a2, 4
    13cd:	02a9           	s32i.n	a10, a2, 0
    13cf:	fb3e21        	l32r	a2, c8 (6bf0 <xt_memset>)
    13d2:	0b0c           	movi.n	a11, 0
    13d4:	ec1c           	movi.n	a12, 30
    13d6:	0002e0        	callx8	a2
    13d9:	f01d           	retw.n

000013db <sb_ana_init+0x13>:
	...

000013dc <sb_syn_init>:
    13dc:	004136        	entry	a1, 32
    13df:	a24b           	addi.n	a10, a2, 4
    13e1:	02a9           	s32i.n	a10, a2, 0
    13e3:	fb3921        	l32r	a2, c8 (6bf0 <xt_memset>)
    13e6:	0b0c           	movi.n	a11, 0
    13e8:	cc3c           	movi.n	a12, 60
    13ea:	0002e0        	callx8	a2
    13ed:	f01d           	retw.n

000013ef <sb_syn_init+0x13>:
	...

000013f0 <HP_coef_setup>:
    13f0:	004136        	entry	a1, 32
    13f3:	fb0a41        	l32r	a4, 1c (6c04 <xt_memcpy>)
    13f6:	4c0c           	movi.n	a12, 4
    13f8:	02ad           	mov.n	a10, a2
    13fa:	03bd           	mov.n	a11, a3
    13fc:	0004e0        	callx8	a4
    13ff:	a2cb           	addi.n	a10, a2, 12
    1401:	b38b           	addi.n	a11, a3, 8
    1403:	ac0c           	movi.n	a12, 10
    1405:	0004e0        	callx8	a4
    1408:	24c2a2        	addi	a10, a2, 36
    140b:	1cc3b2        	addi	a11, a3, 28
    140e:	8c0c           	movi.n	a12, 8
    1410:	0004e0        	callx8	a4
    1413:	1238           	l32i.n	a3, a2, 4
    1415:	72f8           	l32i.n	a15, a2, 28
    1417:	2239           	s32i.n	a3, a2, 8
    1419:	82f9           	s32i.n	a15, a2, 32
    141b:	f01d           	retw.n

0000141d <HP_coef_setup+0x2d>:
    141d:	000000                                        ...

00001420 <tx_ref_vad_init>:
    1420:	004136        	entry	a1, 32
    1423:	fb2941        	l32r	a4, c8 (6bf0 <xt_memset>)
    1426:	0b0c           	movi.n	a11, 0
    1428:	ec0c           	movi.n	a12, 14
    142a:	02ad           	mov.n	a10, a2
    142c:	0004e0        	callx8	a4
    142f:	03d3b2        	addmi	a11, a3, 0x300
    1432:	fafa31        	l32r	a3, 1c (6c04 <xt_memcpy>)
    1435:	7c0c           	movi.n	a12, 7
    1437:	02ad           	mov.n	a10, a2
    1439:	0003e0        	callx8	a3
    143c:	f01d           	retw.n

0000143e <tx_ref_vad_init+0x1e>:
	...

00001440 <Voice_EC_Init>:
    1440:	004136        	entry	a1, 32
    1443:	faf761        	l32r	a6, 20 (8688 <p_ecnr_mem>)
    1446:	0c0c           	movi.n	a12, 0
    1448:	02ad           	mov.n	a10, a2
    144a:	0639           	s32i.n	a3, a6, 0
    144c:	fb3031        	l32r	a3, 10c (380 <TX_ECNR_PARA_init>)
    144f:	04bd           	mov.n	a11, a4
    1451:	0003e0        	callx8	a3
    1454:	faf421        	l32r	a2, 24 (8680 <aec_nr_para>)
    1457:	0638           	l32i.n	a3, a6, 0
    1459:	08a1f2        	movi	a15, 0x108
    145c:	0228           	l32i.n	a2, a2, 0
    145e:	10d3a2        	addmi	a10, a3, 0x1000
    1461:	13a332        	movi	a3, 0x313
    1464:	239242        	l16si	a4, a2, 70
    1467:	b2fa           	add.n	a11, a2, a15
    1469:	fb2921        	l32r	a2, 110 (13f0 <HP_coef_setup>)
    146c:	654d           	ae_s16i.n	a4, a5, 0
    146e:	0002e0        	callx8	a2
    1471:	0628           	l32i.n	a2, a6, 0
    1473:	1133c0        	slli	a3, a3, 4
    1476:	03a1c2        	movi	a12, 0x103
    1479:	a23a           	add.n	a10, a2, a3
    147b:	fb1331        	l32r	a3, c8 (6bf0 <xt_memset>)
    147e:	11ccd0        	slli	a12, a12, 3
    1481:	0b0c           	movi.n	a11, 0
    1483:	0003e0        	callx8	a3
    1486:	0628           	l32i.n	a2, a6, 0
    1488:	0da442        	movi	a4, 0x40d
    148b:	1144e0        	slli	a4, a4, 2
    148e:	a24a           	add.n	a10, a2, a4
    1490:	0b0c           	movi.n	a11, 0
    1492:	ec0c           	movi.n	a12, 14
    1494:	0003e0        	callx8	a3
    1497:	0628           	l32i.n	a2, a6, 0
    1499:	15a442        	movi	a4, 0x415
    149c:	1144e0        	slli	a4, a4, 2
    149f:	a24a           	add.n	a10, a2, a4
    14a1:	0b0c           	movi.n	a11, 0
    14a3:	f0a0c2        	movi	a12, 240
    14a6:	f0a042        	movi	a4, 240
    14a9:	0003e0        	callx8	a3
    14ac:	0628           	l32i.n	a2, a6, 0
    14ae:	5ba152        	movi	a5, 0x15b
    14b1:	1155b0        	slli	a5, a5, 5
    14b4:	2bd272        	addmi	a7, a2, 0x2b00
    14b7:	225a           	add.n	a2, a2, a5
    14b9:	a24b           	addi.n	a10, a2, 4
    14bb:	0b0c           	movi.n	a11, 0
    14bd:	ec1c           	movi.n	a12, 30
    14bf:	1867a2        	s32i	a10, a7, 96
    14c2:	0003e0        	callx8	a3
    14c5:	0628           	l32i.n	a2, a6, 0
    14c7:	5da152        	movi	a5, 0x15d
    14ca:	1155b0        	slli	a5, a5, 5
    14cd:	2bd272        	addmi	a7, a2, 0x2b00
    14d0:	225a           	add.n	a2, a2, a5
    14d2:	a24b           	addi.n	a10, a2, 4
    14d4:	0b0c           	movi.n	a11, 0
    14d6:	ec1c           	movi.n	a12, 30
    14d8:	2867a2        	s32i	a10, a7, 160
    14db:	0003e0        	callx8	a3
    14de:	0628           	l32i.n	a2, a6, 0
    14e0:	61a152        	movi	a5, 0x161
    14e3:	1155b0        	slli	a5, a5, 5
    14e6:	2cd272        	addmi	a7, a2, 0x2c00
    14e9:	225a           	add.n	a2, a2, a5
    14eb:	a24b           	addi.n	a10, a2, 4
    14ed:	0b0c           	movi.n	a11, 0
    14ef:	cc3c           	movi.n	a12, 60
    14f1:	87a9           	s32i.n	a10, a7, 32
    14f3:	0003e0        	callx8	a3
    14f6:	facb31        	l32r	a3, 24 (8680 <aec_nr_para>)
    14f9:	0658           	l32i.n	a5, a6, 0
    14fb:	c8a072        	movi	a7, 200
    14fe:	0328           	l32i.n	a2, a3, 0
    1500:	10d5d2        	addmi	a13, a5, 0x1000
    1503:	18a182        	movi	a8, 0x118
    1506:	3392c2        	l16si	a12, a2, 102
    1509:	285dc2        	s16i	a12, a13, 80
    150c:	3492c2        	l16si	a12, a2, 104
    150f:	0228           	l32i.n	a2, a2, 0
    1511:	40a192        	movi	a9, 0x140
    1514:	68a1a2        	movi	a10, 0x168
    1517:	90a1b2        	movi	a11, 0x190
    151a:	b8a1e2        	movi	a14, 0x1b8
    151d:	e0a1f2        	movi	a15, 0x1e0
    1520:	295dc2        	s16i	a12, a13, 82
    1523:	12d552        	addmi	a5, a5, 0x1200
    1526:	727a           	add.n	a7, a2, a7
    1528:	424a           	add.n	a4, a2, a4
    152a:	828a           	add.n	a8, a2, a8
    152c:	929a           	add.n	a9, a2, a9
    152e:	a2aa           	add.n	a10, a2, a10
    1530:	b2ba           	add.n	a11, a2, a11
    1532:	c2ea           	add.n	a12, a2, a14
    1534:	d2fa           	add.n	a13, a2, a15
    1536:	78c222        	addi	a2, a2, 120
    1539:	166522        	s32i	a2, a5, 88
    153c:	faf621        	l32r	a2, 114 (698 <SBAEC_INIT>)
    153f:	176572        	s32i	a7, a5, 92
    1542:	186542        	s32i	a4, a5, 96
    1545:	196582        	s32i	a8, a5, 100
    1548:	1a6592        	s32i	a9, a5, 104
    154b:	1b65a2        	s32i	a10, a5, 108
    154e:	1c65b2        	s32i	a11, a5, 112
    1551:	1e65c2        	s32i	a12, a5, 120
    1554:	1d65d2        	s32i	a13, a5, 116
    1557:	0002e0        	callx8	a2
    155a:	faef21        	l32r	a2, 118 (3a0c <NOISE_GEN_init>)
    155d:	0002e0        	callx8	a2
    1560:	0328           	l32i.n	a2, a3, 0
    1562:	0658           	l32i.n	a5, a6, 0
    1564:	c4a072        	movi	a7, 196
    1567:	7228           	l32i.n	a2, a2, 28
    1569:	46d772        	addmi	a7, a7, 0x4600
    156c:	a57a           	add.n	a10, a5, a7
    156e:	fad671        	l32r	a7, c8 (6bf0 <xt_memset>)
    1571:	0b0c           	movi.n	a11, 0
    1573:	0c1c           	movi.n	a12, 16
    1575:	46d542        	addmi	a4, a5, 0x4600
    1578:	f53020        	extui	a3, a2, 16, 16
    157b:	756820        	extui	a6, a2, 24, 8
    157e:	0007e0        	callx8	a7
    1581:	fad6b1        	l32r	a11, dc (155555 <_end+0x14cec9>)
    1584:	faa881        	l32r	a8, 24 (8680 <aec_nr_para>)
    1587:	46d5e2        	addmi	a14, a5, 0x4600
    158a:	46d5d2        	addmi	a13, a5, 0x4600
    158d:	faa451        	l32r	a5, 20 (8688 <p_ecnr_mem>)
    1590:	991c           	movi.n	a9, 25
    1592:	c0b2b0        	sub	a11, a2, a11
    1595:	0888           	l32i.n	a8, a8, 0
    1597:	9393b0        	movnez	a9, a3, a11
    159a:	743020        	extui	a3, a2, 0, 8
    159d:	74f820        	extui	a15, a2, 8, 8
    15a0:	0c1c           	movi.n	a12, 16
    15a2:	0528           	l32i.n	a2, a5, 0
    15a4:	d70c           	movi.n	a7, 13
    15a6:	833cb0        	moveqz	a3, a12, a11
    15a9:	2c0c           	movi.n	a12, 2
    15ab:	9376b0        	movnez	a7, a6, a11
    15ae:	93cfb0        	movnez	a12, a15, a11
    15b1:	2098b2        	l16si	a11, a8, 64
    15b4:	655eb2        	s16i	a11, a14, 202
    15b7:	1e0c           	movi.n	a14, 1
    15b9:	12d2f2        	addmi	a15, a2, 0x1200
    15bc:	275fe2        	s16i	a14, a15, 78
    15bf:	1f98b2        	l16si	a11, a8, 62
    15c2:	3018e2        	l16ui	a14, a8, 96
    15c5:	1e18f2        	l16ui	a15, a8, 60
    15c8:	321882        	l16ui	a8, a8, 100
    15cb:	fa7c           	movi.n	a10, -1
    15cd:	c133e0        	mul16u	a3, a3, a14
    15d0:	01aa10        	slli	a10, a10, 31
    15d3:	8da452        	movi	a5, 0x48d
    15d6:	c18c80        	mul16u	a8, a12, a8
    15d9:	1165e0        	slli	a6, a5, 2
    15dc:	e14492        	s8i	a9, a4, 225
    15df:	efaa           	add.n	a14, a15, a10
    15e1:	119f00        	slli	a9, a15, 16
    15e4:	6454b2        	s16i	a11, a4, 200
    15e7:	e04472        	s8i	a7, a4, 224
    15ea:	726a           	add.n	a7, a2, a6
    15ec:	c06e90        	sub	a6, a14, a9
    15ef:	413530        	srli	a3, a3, 5
    15f2:	41f580        	srli	a15, a8, 5
    15f5:	7b0c           	movi.n	a11, 7
    15f7:	00a452        	movi	a5, 0x400
    15fa:	316462        	s32i	a6, a4, 196
    15fd:	6e5432        	s16i	a3, a4, 220
    1600:	6f5df2        	s16i	a15, a13, 222
    1603:	038b76        	loop	a11, 160a <Voice_EC_Init+0x1ca>
    1606:	675d           	ae_s16i.n	a5, a7, 0
    1608:	772b           	addi.n	a7, a7, 2

0000160a <Voice_EC_Init+0x1ca>:
    160a:	faaf71        	l32r	a7, c8 (6bf0 <xt_memset>)
    160d:	24d2a2        	addmi	a10, a2, 0x2400
    1610:	0b0c           	movi.n	a11, 0
    1612:	0c5c           	movi.n	a12, 80
    1614:	0007e0        	callx8	a7
    1617:	fa8251        	l32r	a5, 20 (8688 <p_ecnr_mem>)
    161a:	25a132        	movi	a3, 0x125
    161d:	1133b0        	slli	a3, a3, 5
    1620:	0528           	l32i.n	a2, a5, 0
    1622:	0b0c           	movi.n	a11, 0
    1624:	4c2c           	movi.n	a12, 36
    1626:	a23a           	add.n	a10, a2, a3
    1628:	0007e0        	callx8	a7
    162b:	0528           	l32i.n	a2, a5, 0
    162d:	fa7d61        	l32r	a6, 24 (8680 <aec_nr_para>)
    1630:	5fa142        	movi	a4, 0x15f
    1633:	12d232        	addmi	a3, a2, 0x1200
    1636:	261332        	l16ui	a3, a3, 76
    1639:	06a8           	l32i.n	a10, a6, 0
    163b:	3e2366        	bnei	a3, 2, 167d <Voice_EC_Init+0x23d>
    163e:	fab721        	l32r	a2, 11c (5254 <TWO_MIC_WB_init>)
    1641:	0002e0        	callx8	a2
    1644:	0528           	l32i.n	a2, a5, 0
    1646:	1134b0        	slli	a3, a4, 5
    1649:	0b0c           	movi.n	a11, 0
    164b:	2bd242        	addmi	a4, a2, 0x2b00
    164e:	223a           	add.n	a2, a2, a3
    1650:	a24b           	addi.n	a10, a2, 4
    1652:	ec1c           	movi.n	a12, 30
    1654:	3864a2        	s32i	a10, a4, 224
    1657:	0007e0        	callx8	a7
    165a:	0528           	l32i.n	a2, a5, 0
    165c:	9ca032        	movi	a3, 156
    165f:	2cd332        	addmi	a3, a3, 0x2c00
    1662:	2cd242        	addmi	a4, a2, 0x2c00
    1665:	223a           	add.n	a2, a2, a3
    1667:	a24b           	addi.n	a10, a2, 4
    1669:	0b0c           	movi.n	a11, 0
    166b:	cc3c           	movi.n	a12, 60
    166d:	2764a2        	s32i	a10, a4, 156
    1670:	0007e0        	callx8	a7
    1673:	06a8           	l32i.n	a10, a6, 0
    1675:	faaa21        	l32r	a2, 120 (3a2c <NC_NOISE_EST_init>)
    1678:	0002e0        	callx8	a2
    167b:	0528           	l32i.n	a2, a5, 0

0000167d <Voice_EC_Init+0x23d>:
    167d:	a8a032        	movi	a3, 168
    1680:	46d332        	addmi	a3, a3, 0x4600
    1683:	223a           	add.n	a2, a2, a3
    1685:	0648           	l32i.n	a4, a6, 0
    1687:	0b0c           	movi.n	a11, 0
    1689:	ec0c           	movi.n	a12, 14
    168b:	02ad           	mov.n	a10, a2
    168d:	0007e0        	callx8	a7
    1690:	fa6331        	l32r	a3, 1c (6c04 <xt_memcpy>)
    1693:	03d4b2        	addmi	a11, a4, 0x300
    1696:	7c0c           	movi.n	a12, 7
    1698:	02ad           	mov.n	a10, a2
    169a:	0003e0        	callx8	a3
    169d:	f01d           	retw.n

0000169f <Voice_EC_Init+0x25f>:
	...

000016a0 <Voice_EC_Inear_Init>:
    16a0:	004136        	entry	a1, 32
    16a3:	fa6961        	l32r	a6, 48 (1440 <Voice_EC_Init>)
    16a6:	02ad           	mov.n	a10, a2
    16a8:	03bd           	mov.n	a11, a3
    16aa:	04cd           	mov.n	a12, a4
    16ac:	05dd           	mov.n	a13, a5
    16ae:	0006e0        	callx8	a6
    16b1:	6fa442        	movi	a4, 0x46f
    16b4:	1144c0        	slli	a4, a4, 4
    16b7:	fa8451        	l32r	a5, c8 (6bf0 <xt_memset>)
    16ba:	46d322        	addmi	a2, a3, 0x4600
    16bd:	a34a           	add.n	a10, a3, a4
    16bf:	0b0c           	movi.n	a11, 0
    16c1:	28a2c2        	movi	a12, 0x228
    16c4:	3a62a2        	s32i	a10, a2, 232
    16c7:	0005e0        	callx8	a5
    16ca:	fa5661        	l32r	a6, 24 (8680 <aec_nr_para>)
    16cd:	a4a082        	movi	a8, 164
    16d0:	48d882        	addmi	a8, a8, 0x4800
    16d3:	0698           	l32i.n	a9, a6, 0
    16d5:	a38a           	add.n	a10, a3, a8
    16d7:	a862a2        	s32i	a10, a2, 0x2a0
    16da:	02d9e2        	addmi	a14, a9, 0x200
    16dd:	49d342        	addmi	a4, a3, 0x4900
    16e0:	4b9e72        	l16si	a7, a14, 150
    16e3:	3c5472        	s16i	a7, a4, 120
    16e6:	079e72        	l16si	a7, a14, 14
    16e9:	0b0c           	movi.n	a11, 0
    16eb:	ec1c           	movi.n	a12, 30
    16ed:	3d5472        	s16i	a7, a4, 122
    16f0:	0005e0        	callx8	a5
    16f3:	e4a042        	movi	a4, 228
    16f6:	48d442        	addmi	a4, a4, 0x4800
    16f9:	a34a           	add.n	a10, a3, a4
    16fb:	0b0c           	movi.n	a11, 0
    16fd:	cc3c           	movi.n	a12, 60
    16ff:	b862a2        	s32i	a10, a2, 0x2e0
    1702:	0005e0        	callx8	a5
    1705:	12d342        	addmi	a4, a3, 0x1200
    1708:	1d2442        	l32i	a4, a4, 116
    170b:	0688           	l32i.n	a8, a6, 0
    170d:	28c4d2        	addi	a13, a4, 40
    1710:	50c4f2        	addi	a15, a4, 80
    1713:	78c442        	addi	a4, a4, 120
    1716:	02d8b2        	addmi	a11, a8, 0x200
    1719:	db62d2        	s32i	a13, a2, 0x36c
    171c:	dc62f2        	s32i	a15, a2, 0x370
    171f:	481bd2        	l16ui	a13, a11, 144
    1722:	dd6242        	s32i	a4, a2, 0x374
    1725:	479b72        	l16si	a7, a11, 142
    1728:	00a1c2        	movi	a12, 0x100
    172b:	49d392        	addmi	a9, a3, 0x4900
    172e:	5377c0        	max	a7, a7, a12
    1731:	4bd3a2        	addmi	a10, a3, 0x4b00
    1734:	41ffd0        	srli	a15, a13, 15
    1737:	3e4af2        	s8i	a15, a10, 62
    173a:	345972        	s16i	a7, a9, 104
    173d:	fa79a1        	l32r	a10, 124 (dfff <_end+0x5973>)
    1740:	d81872        	l16ui	a7, a8, 0x1b0
    1743:	b8a2b2        	movi	a11, 0x2b8
    1746:	1e0c           	movi.n	a14, 1
    1748:	e4f0d0        	extui	a15, a13, 0, 15
    174b:	014e90        	slli	a4, a14, 23
    174e:	b8ba           	add.n	a11, a8, a11
    1750:	538fc0        	max	a8, a15, a12
    1753:	62aa70        	saltu	a10, a10, a7
    1756:	2e7c           	movi.n	a14, -14
    1758:	2d0c           	movi.n	a13, 2
    175a:	93dea0        	movnez	a13, a14, a10
    175d:	417c70        	srli	a7, a7, 12
    1760:	3ba2e2        	movi	a14, 0x23b
    1763:	c24480        	quou	a4, a4, a8
    1766:	7d7a           	add.n	a7, a13, a7
    1768:	11aeb0        	slli	a10, a14, 5
    176b:	355942        	s16i	a4, a9, 106
    176e:	fa6841        	l32r	a4, 110 (13f0 <HP_coef_setup>)
    1771:	a3aa           	add.n	a10, a3, a10
    1773:	4bd3f2        	addmi	a15, a3, 0x4b00
    1776:	1177e0        	slli	a7, a7, 2
    1779:	1e5f72        	s16i	a7, a15, 60
    177c:	0004e0        	callx8	a4
    177f:	fa6a41        	l32r	a4, 128 (990 <SBAEC_INIT_inear>)
    1782:	03ad           	mov.n	a10, a3
    1784:	0004e0        	callx8	a4
    1787:	3a2272        	l32i	a7, a2, 232
    178a:	90a2f2        	movi	a15, 0x290
    178d:	0648           	l32i.n	a4, a6, 0
    178f:	38a4e2        	movi	a14, 0x438
    1792:	a7fa           	add.n	a10, a7, a15
    1794:	0b0c           	movi.n	a11, 0
    1796:	d4a0c2        	movi	a12, 212
    1799:	04d762        	addmi	a6, a7, 0x400
    179c:	27ea           	add.n	a2, a7, a14
    179e:	0005e0        	callx8	a5
    17a1:	02ad           	mov.n	a10, a2
    17a3:	0b0c           	movi.n	a11, 0
    17a5:	8c0c           	movi.n	a12, 8
    17a7:	820c           	movi.n	a2, 8
    17a9:	0005e0        	callx8	a5
    17ac:	29d332        	addmi	a3, a3, 0x2900
    17af:	04d782        	addmi	a8, a7, 0x400
    17b2:	212332        	l32i	a3, a3, 132
    17b5:	03d4e2        	addmi	a14, a4, 0x300
    17b8:	c839           	s32i.n	a3, a8, 48
    17ba:	1a1e32        	l16ui	a3, a14, 52
    17bd:	392752        	l32i	a5, a7, 228
    17c0:	98a3a2        	movi	a10, 0x398
    17c3:	b859           	s32i.n	a5, a8, 44
    17c5:	3b2792        	l32i	a9, a7, 236
    17c8:	57aa           	add.n	a5, a7, a10
    17ca:	fa58d1        	l32r	a13, 12c (7fff0000 <_end+0x7ffe7974>)
    17cd:	d899           	s32i.n	a9, a8, 52
    17cf:	a659           	s32i.n	a5, a6, 40
    17d1:	76d9           	s32i.n	a13, a6, 28
    17d3:	66d9           	s32i.n	a13, a6, 24
    17d5:	239c           	beqz.n	a3, 17eb <Voice_EC_Inear_Init+0x14b>
    17d7:	03d422        	addmi	a2, a4, 0x300
    17da:	857c           	movi.n	a5, -8
    17dc:	1b1222        	l16ui	a2, a2, 54
    17df:	f45050        	extui	a5, a5, 0, 16
    17e2:	248030        	extui	a8, a3, 0, 3
    17e5:	103350        	and	a3, a3, a5
    17e8:	0001c6        	j	17f3 <Voice_EC_Inear_Init+0x153>

000017eb <Voice_EC_Inear_Init+0x14b>:
    17eb:	33a332        	movi	a3, 0x333
    17ee:	1133d0        	slli	a3, a3, 3
    17f1:	080c           	movi.n	a8, 0
    17f3:	04d7f2        	addmi	a15, a7, 0x400
    17f6:	1c5632        	s16i	a3, a6, 56
    17f9:	1d5f82        	s16i	a8, a15, 58
    17fc:	105f22        	s16i	a2, a15, 32
    17ff:	f01d           	retw.n

00001801 <Voice_EC_Inear_Init+0x161>:
    1801:	000000                                        ...

00001804 <mic240_delay>:
    1804:	004136        	entry	a1, 32
    1807:	fa0561        	l32r	a6, 1c (6c04 <xt_memcpy>)
    180a:	02ad           	mov.n	a10, a2
    180c:	04bd           	mov.n	a11, a4
    180e:	05cd           	mov.n	a12, a5
    1810:	0006e0        	callx8	a6
    1813:	f0a072        	movi	a7, 240
    1816:	90a520        	addx2	a10, a5, a2
    1819:	c0c750        	sub	a12, a7, a5
    181c:	03bd           	mov.n	a11, a3
    181e:	1125f0        	slli	a2, a5, 1
    1821:	0006e0        	callx8	a6
    1824:	c02320        	sub	a2, a3, a2
    1827:	e0a1f2        	movi	a15, 0x1e0
    182a:	b2fa           	add.n	a11, a2, a15
    182c:	04ad           	mov.n	a10, a4
    182e:	05cd           	mov.n	a12, a5
    1830:	0006e0        	callx8	a6
    1833:	f01d           	retw.n

00001835 <mic240_delay+0x31>:
    1835:	000000                                        ...

00001838 <tx_downsample_by2>:
    1838:	004136        	entry	a1, 32
    183b:	fa3d71        	l32r	a7, 130 (7a10 <wb_r01>)
    183e:	14a082        	movi	a8, 20
    1841:	fec222        	addi	a2, a2, -2
    1844:	fec332        	addi	a3, a3, -2
    1847:	600604        	ae_lq32f.i	aeq0, a6, 0
    184a:	440404        	ae_lp16x2f.i	aep0, a4, 0
    184d:	948876        	loop	a8, 18e5 <tx_downsample_by2+0xad>
    1850:	e2002f150f 	{ ae_lp24x2f.i	aep1, a5, 0; ae_mulaafp24s.hh.ll	aeq0, aep0, aep0 }
    1855:	fd242c340f 	{ ae_lp16f.i	aep3, a4, 0; ae_cvtq48p24s.h	aeq1, aep1 }
    185a:	de2cb1273f 	{ ae_lp24.i	aep2, a7, 12; ae_selp24.hl	aep4, aep3, aep1 }
    185f:	e88874350f 	{ ae_sp24f.l.i	aep3, a5, 0; ae_mulasfp24s.hh.ll	aeq1, aep2, aep4 }
    1864:	ff2c2f251f 	{ ae_lp24x2f.i	aep2, a5, 8; ae_cvtq48p24s.l	aeq3, aep1 }
    1869:	fa6ff1674f 	{ ae_lp24.i	aep6, a7, 16; ae_roundsp24q48asym	aep5, aeq1 }
    186e:	df55f4551f 	{ ae_sp24f.l.i	aep5, a5, 4; ae_selp24.lh	aep7, aep5, aep2 }
    1873:	e9f871375f 	{ ae_lp24.i	aep3, a7, 20; ae_mulasfp24s.hh.ll	aeq3, aep6, aep7 }
    1878:	fe442c441f 	{ ae_lp16f.i	aep4, a4, 2; ae_cvtq48p24s.h	aeq2, aep2 }
    187d:	f87fed841f 	{ ae_lp16x2f.iu	aep0, a4, 4; ae_roundsp24q48asym	aep1, aeq3 }
    1882:	dd4634152f 	{ ae_sp24f.l.i	aep1, a5, 8; ae_selp24.ll	aep2, aep1, aep2 }
    1887:	05b244        	ae_mulasfp24s.hh.ll	aeq2, aep3, aep2
    188a:	1bad14        	ae_roundsp24q48asym	aep1, aeq2
    188d:	521534        	ae_sp24f.l.i	aep1, a5, 12
    1890:	5c6524        	ae_lp24x2f.i	aep6, a5, 16
    1893:	ffc431770f 	{ ae_lp24.i	aep7, a7, 0; ae_cvtq48p24s.h	aeq3, aep6 }
    1898:	ded0f4454f 	{ ae_sp24f.l.i	aep4, a5, 16; ae_selp24.hl	aep5, aep4, aep6 }
    189d:	e9bc6f253f 	{ ae_lp24x2f.i	aep2, a5, 24; ae_mulasfp24s.hh.ll	aeq3, aep7, aep5 }
    18a2:	fecc31671f 	{ ae_lp24.i	aep6, a7, 4; ae_cvtq48p24s.l	aeq2, aep6 }
    18a7:	fa7fbf18df 	{ ae_movap24s.l	a8, aep1; ae_roundsp24q48asym	aep4, aeq3 }
    18ac:	df51f4455f 	{ ae_sp24f.l.i	aep4, a5, 20; ae_selp24.lh	aep7, aep4, aep2 }
    18b1:	e9f831472f 	{ ae_lp24.i	aep4, a7, 8; ae_mulasfp24s.hh.ll	aeq2, aep6, aep7 }
    18b6:	0aca84        	ae_cvtq48p24s.h	aeq3, aep2
    18b9:	1bad34        	ae_roundsp24q48asym	aep3, aeq2
    18bc:	dd4e34356f 	{ ae_sp24f.l.i	aep3, a5, 24; ae_selp24.ll	aep2, aep3, aep2 }
    18c1:	05ca44        	ae_mulasfp24s.hh.ll	aeq3, aep4, aep2
    18c4:	1bbd54        	ae_roundsp24q48asym	aep5, aeq3
    18c7:	34d904        	ae_movap24s.l	a9, aep5
    18ca:	a98a           	add.n	a10, a9, a8
    18cc:	c08980        	sub	a8, a9, a8
    18cf:	41f1a0        	srli	a15, a10, 1
    18d2:	418180        	srli	a8, a8, 1
    18d5:	39ef84        	ae_movpa24x2	aep6, a15, a8
    18d8:	f9fab4557f 	{ ae_sp24f.l.i	aep5, a5, 28; ae_roundsp16asym	aep2, aep6 }
    18dd:	df4972a31f 	{ ae_sp16f.l.iu	aep2, a3, 2; ae_selp24.hh	aep7, aep2, aep2 }
    18e2:	42f214        	ae_sp16f.l.iu	aep7, a2, 2

000018e5 <tx_downsample_by2+0xad>:
    18e5:	060934        	ae_roundsq32asym	aeq0, aeq0
    18e8:	620604        	ae_sq32f.i	aeq0, a6, 0
    18eb:	f01d           	retw.n

000018ed <tx_downsample_by2+0xb5>:
    18ed:	000000                                        ...

000018f0 <tx_upsample_by2>:
    18f0:	006136        	entry	a1, 48
    18f3:	fa0f91        	l32r	a9, 130 (7a10 <wb_r01>)
    18f6:	4a1c           	movi.n	a10, 20
    18f8:	71eb           	addi.n	a7, a1, 14
    18fa:	81cb           	addi.n	a8, a1, 12
    18fc:	fcc442        	addi	a4, a4, -4
    18ff:	600604        	ae_lq32f.i	aeq0, a6, 0
    1902:	401204        	ae_lp16f.i	aep1, a2, 0
    1905:	402304        	ae_lp16f.i	aep2, a3, 0
    1908:	8f8a76        	loop	a10, 199b <tx_upsample_by2+0xab>
    190b:	dd46ef050f 	{ ae_lp24x2f.i	aep0, a5, 0; ae_subsp24s	aep3, aep1, aep2 }
    1910:	fd0432381f 	{ ae_sp16f.l.i	aep3, a8, 2; ae_cvtq48p24s.h	aeq1, aep0 }
    1915:	ff0c2c370f 	{ ae_lp16f.i	aep3, a7, 0; ae_cvtq48p24s.l	aeq3, aep0 }
    191a:	de0cf1490f 	{ ae_lp24.i	aep4, a9, 0; ae_selp24.hl	aep5, aep3, aep0 }
    191f:	e8b074350f 	{ ae_sp24f.l.i	aep3, a5, 0; ae_mulasfp24s.hh.ll	aeq1, aep4, aep5 }
    1924:	d8452f351f 	{ ae_lp24x2f.i	aep3, a5, 8; ae_addsp24s	aep0, aep1, aep2 }
    1929:	fa6fb1691f 	{ ae_lp24.i	aep6, a9, 4; ae_roundsp24q48asym	aep4, aeq1 }
    192e:	df71f4451f 	{ ae_sp24f.l.i	aep4, a5, 4; ae_selp24.lh	aep7, aep4, aep3 }
    1933:	e9f871592f 	{ ae_lp24.i	aep5, a9, 8; ae_mulasfp24s.hh.ll	aeq3, aep6, aep7 }
    1938:	fe6432080f 	{ ae_sp16f.l.i	aep0, a8, 0; ae_cvtq48p24s.h	aeq2, aep3 }
    193d:	fa7fac680f 	{ ae_lp16f.i	aep6, a8, 0; ae_roundsp24q48asym	aep4, aeq3 }
    1942:	dd7274452f 	{ ae_sp24f.l.i	aep4, a5, 8; ae_selp24.ll	aep3, aep4, aep3 }
    1947:	05d344        	ae_mulasfp24s.hh.ll	aeq2, aep5, aep3
    194a:	1bad24        	ae_roundsp24q48asym	aep2, aeq2
    194d:	e203b4253f 	{ ae_sp24f.l.i	aep2, a5, 12; ae_mulafp24s.ll	aeq0, aep0, aep0 }
    1952:	5c1524        	ae_lp24x2f.i	aep1, a5, 16
    1955:	ff2431493f 	{ ae_lp24.i	aep4, a9, 12; ae_cvtq48p24s.h	aeq3, aep1 }
    195a:	df38f4654f 	{ ae_sp24f.l.i	aep6, a5, 16; ae_selp24.hl	aep7, aep6, aep1 }
    195f:	e9f06f353f 	{ ae_lp24x2f.i	aep3, a5, 24; ae_mulasfp24s.hh.ll	aeq3, aep4, aep7 }
    1964:	fe2c31694f 	{ ae_lp24.i	aep6, a9, 16; ae_cvtq48p24s.l	aeq2, aep1 }
    1969:	fa7fbf2adf 	{ ae_movap24s.l	a10, aep2; ae_roundsp24q48asym	aep4, aeq3 }
    196e:	df71f4455f 	{ ae_sp24f.l.i	aep4, a5, 20; ae_selp24.lh	aep7, aep4, aep3 }
    1973:	e9f831795f 	{ ae_lp24.i	aep7, a9, 20; ae_mulasfp24s.hh.ll	aeq2, aep6, aep7 }
    1978:	ff642c921f 	{ ae_lp16f.iu	aep1, a2, 2; ae_cvtq48p24s.h	aeq3, aep3 }
    197d:	fb77aca31f 	{ ae_lp16f.iu	aep2, a3, 2; ae_roundsp24q48asym	aep6, aeq2 }
    1982:	dd7a74656f 	{ ae_sp24f.l.i	aep6, a5, 24; ae_selp24.ll	aep3, aep6, aep3 }
    1987:	05fb44        	ae_mulasfp24s.hh.ll	aeq3, aep7, aep3
    198a:	1bbd64        	ae_roundsp24q48asym	aep6, aeq3
    198d:	34eb04        	ae_movap24s.l	a11, aep6
    1990:	39fab4        	ae_movpa24x2	aep7, a10, a11
    1993:	f9fef4657f 	{ ae_sp24f.l.i	aep6, a5, 28; ae_roundsp16asym	aep3, aep7 }
    1998:	46b414        	ae_sp16x2f.iu	aep3, a4, 4

0000199b <tx_upsample_by2+0xab>:
    199b:	060934        	ae_roundsq32asym	aeq0, aeq0
    199e:	620604        	ae_sq32f.i	aeq0, a6, 0
    19a1:	f01d           	retw.n

000019a3 <tx_upsample_by2+0xb3>:
	...

000019a4 <emph_filter_N>:
    19a4:	004136        	entry	a1, 32
    19a7:	440404        	ae_lp16x2f.i	aep0, a4, 0
    19aa:	3725a6        	blti	a5, 2, 19e5 <emph_filter_N+0x41>
    19ad:	651b           	addi.n	a6, a5, 1
    19af:	b36550        	movgez	a6, a5, a5
    19b2:	215160        	srai	a5, a6, 1
    19b5:	441304        	ae_lp16x2f.i	aep1, a3, 0
    19b8:	fec232        	addi	a3, a2, -2
    19bb:	fcc222        	addi	a2, a2, -4
    19be:	238576        	loop	a5, 19e5 <emph_filter_N+0x41>
    19c1:	44a214        	ae_lp16x2f.iu	aep2, a2, 4
    19c4:	fc443e20cf 	{ ae_selp24.hh	aep3, aep2, aep0; ae_cvtq48p24s.h	aeq0, aep2 }
    19c9:	0a4284        	ae_cvtq48p24s.l	aeq1, aep2
    19cc:	011004        	ae_mulafp24s.hh	aeq0, aep1, aep0
    19cf:	011a04        	ae_mulafp24s.hh	aeq1, aep1, aep2
    19d2:	051004        	ae_mulaafp24s.hh.ll	aeq0, aep1, aep0
    19d5:	051b04        	ae_mulaafp24s.hh.ll	aeq1, aep1, aep3
    19d8:	1b8f04        	ae_roundsp16q48asym	aep0, aeq0
    19db:	f96bf2831f 	{ ae_sp16f.l.iu	aep0, a3, 2; ae_roundsp16q48asym	aep3, aeq1 }
    19e0:	dc49b2b31f 	{ ae_sp16f.l.iu	aep3, a3, 2; ae_selp24.lh	aep0, aep2, aep2 }

000019e5 <emph_filter_N+0x41>:
    19e5:	460404        	ae_sp16x2f.i	aep0, a4, 0
    19e8:	f01d           	retw.n

000019ea <emph_filter_N+0x46>:
	...

000019ec <deemph_filter_N>:
    19ec:	004136        	entry	a1, 32
    19ef:	600404        	ae_lq32f.i	aeq0, a4, 0
    19f2:	604414        	ae_lq32f.i	aeq1, a4, 4
    19f5:	2e15a6        	blti	a5, 1, 1a27 <deemph_filter_N+0x3b>
    19f8:	f9bdf1        	l32r	a15, ec (7800 <immediate_num>)
    19fb:	ff0c6d030f 	{ ae_lp16x2f.i	aep0, a3, 0; ae_movq56	aeq3, aeq1 }
    1a00:	99002c120f 	{ ae_lp16f.i	aep1, a2, 0; ae_mulzssfq32sp16s.hh	aeq2, aeq0, aep0, aeq0, aep0 }
    1a05:	30cf32        	addi	a3, a15, 48
    1a08:	442304        	ae_lp16x2f.i	aep2, a3, 0
    1a0b:	f03d           	nop.n
    1a0d:	168576        	loop	a5, 1a27 <deemph_filter_N+0x3b>
    1a10:	fb81bf017f 	{ ae_movq56	aeq1, aeq0; ae_mulsfq32sp16s.l	aeq2, aeq3, aep0 }
    1a15:	f1462c921f 	{ ae_lp16f.iu	aep1, a2, 2; ae_mulssfp24s.hh.ll	aeq2, aep1, aep2 }
    1a1a:	fc347f11ff 	{ ae_movq56	aeq3, aeq1; ae_roundsq32asym	aeq0, aeq2 }
    1a1f:	1b8f34        	ae_roundsp16q48asym	aep3, aeq0
    1a22:	99003232ff 	{ ae_sp16f.l.i	aep3, a2, -2; ae_mulzssfq32sp16s.hh	aeq2, aeq0, aep0, aeq0, aep0 }

00001a27 <deemph_filter_N+0x3b>:
    1a27:	620404        	ae_sq32f.i	aeq0, a4, 0
    1a2a:	621414        	ae_sq32f.i	aeq1, a4, 4
    1a2d:	f01d           	retw.n

00001a2f <deemph_filter_N+0x43>:
	...

00001a30 <analy_3band>:
    1a30:	004136        	entry	a1, 32
    1a33:	f9c0b1        	l32r	a11, 134 (7a30 <sb_g10>)
    1a36:	f9c0c1        	l32r	a12, 138 (7a70 <sb_h2ir>)
    1a39:	a34b           	addi.n	a10, a3, 4
    1a3b:	f3f6a0        	wur.ae_cbegin0	a10
    1a3e:	03a8           	l32i.n	a10, a3, 0
    1a40:	40c3f2        	addi	a15, a3, 64
    1a43:	a70c           	movi.n	a7, 10
    1a45:	480c           	movi.n	a8, 4
    1a47:	790c           	movi.n	a9, 7
    1a49:	f3f7f0        	wur.ae_cend0	a15
    1a4c:	fe248fc44e 	{ addi	a4, a4, -4; ae_zeroq56	aeq2 }
    1a51:	fec552        	addi	a5, a5, -2
    1a54:	fec662        	addi	a6, a6, -2
    1a57:	fccbb2        	addi	a11, a11, -4
    1a5a:	fcccc2        	addi	a12, a12, -4
    1a5d:	fcc222        	addi	a2, a2, -4

00001a60 <analy_3band+0x30>:
    1a60:	448214        	ae_lp16x2f.iu	aep0, a2, 4
    1a63:	fc14640a8f 	{ ae_sp16x2f.c	aep0, a10, a8; ae_movq56	aeq0, aeq2 }
    1a68:	0bdd           	mov.n	a13, a11
    1a6a:	20ecc0        	or	a14, a12, a12
    1a6d:	fc24da8a8f 	{ ae_lp16x2f.c	aep0, a10, a8; nop }
    1a72:	449d14        	ae_lp16x2f.iu	aep1, a13, 4
    1a75:	ea05edfe1f 	{ ae_lp16x2f.iu	aep7, a14, 4; ae_mulfp24s.ll	aeq1, aep1, aep0 }
    1a7a:	01f874        	ae_mulfp24s.ll	aeq3, aep7, aep0
    1a7d:	278976        	loop	a9, 1aa8 <analy_3band+0x78>
    1a80:	fc24da8a8f 	{ ae_lp16x2f.c	aep0, a10, a8; nop }
    1a85:	449d14        	ae_lp16x2f.iu	aep1, a13, 4
    1a88:	e2062dae1f 	{ ae_lp16x2f.iu	aep2, a14, 4; ae_mulafp24s.hh	aeq0, aep1, aep0 }
    1a8d:	e207daea8f 	{ ae_lp16x2f.c	aep6, a10, a8; ae_mulafp24s.ll	aeq1, aep1, aep0 }
    1a92:	e30a2dbd1f 	{ ae_lp16x2f.iu	aep3, a13, 4; ae_mulafp24s.hh	aeq2, aep2, aep0 }
    1a97:	e30bedfe1f 	{ ae_lp16x2f.iu	aep7, a14, 4; ae_mulafp24s.ll	aeq3, aep2, aep0 }
    1a9c:	013604        	ae_mulafp24s.hh	aeq0, aep3, aep6
    1a9f:	013e34        	ae_mulafp24s.ll	aeq1, aep3, aep6
    1aa2:	01f604        	ae_mulafp24s.hh	aeq2, aep7, aep6
    1aa5:	01fe34        	ae_mulafp24s.ll	aeq3, aep7, aep6

00001aa8 <analy_3band+0x78>:
    1aa8:	f87bcff77e 	{ addi	a7, a7, -1; ae_roundsp16q48asym	aep1, aeq3 }
    1aad:	fb6a32961f 	{ ae_sp16f.l.iu	aep1, a6, 2; ae_addq56	aeq3, aeq1, aeq0 }
    1ab2:	f92afb30df 	{ ae_sraiq56	aeq3, aeq3, 1; ae_subq56	aeq1, aeq1, aeq0 }
    1ab7:	f873bb101f 	{ ae_sraiq56	aeq0, aeq1, 1; ae_roundsp16q48asym	aep0, aeq2 }
    1abc:	fb7fb2861f 	{ ae_sp16f.l.iu	aep0, a6, 2; ae_roundsp24q48asym	aep6, aeq3 }
    1ac1:	fb63f4e41f 	{ ae_sp24f.l.iu	aep6, a4, 4; ae_roundsp16q48asym	aep7, aeq0 }
    1ac6:	fe24b2f51f 	{ ae_sp16f.l.iu	aep7, a5, 2; ae_zeroq56	aeq2 }
    1acb:	f91756        	bnez	a7, 1a60 <analy_3band+0x30>
    1ace:	03a9           	s32i.n	a10, a3, 0
    1ad0:	f01d           	retw.n

00001ad2 <analy_3band+0xa2>:
	...

00001ad4 <synth_3band>:
    1ad4:	004136        	entry	a1, 32
    1ad7:	c54b           	addi.n	a12, a5, 4
    1ad9:	f3f6c0        	wur.ae_cbegin0	a12
    1adc:	f99671        	l32r	a7, 134 (7a30 <sb_g10>)
    1adf:	f99781        	l32r	a8, 13c (7ab0 <sb_h2ir_v2>)
    1ae2:	0025c2        	l32i	a12, a5, 0
    1ae5:	7cc5f2        	addi	a15, a5, 124
    1ae8:	0aa092        	movi	a9, 10
    1aeb:	fd248004af 	{ movi	a10, 4; ae_zeroq56	aeq1 }
    1af0:	0fa0b2        	movi	a11, 15
    1af3:	f3f7f0        	wur.ae_cend0	a15
    1af6:	fc0c4fe22e 	{ addi	a2, a2, -2; ae_movq56	aeq0, aeq1 }
    1afb:	fec332        	addi	a3, a3, -2
    1afe:	fcc442        	addi	a4, a4, -4
    1b01:	fec662        	addi	a6, a6, -2

00001b04 <synth_3band+0x30>:
    1b04:	408214        	ae_lp16f.iu	aep0, a2, 2
    1b07:	409314        	ae_lp16f.iu	aep1, a3, 2
    1b0a:	d9212df41f 	{ ae_lp16x2f.iu	aep7, a4, 4; ae_addsp24s	aep2, aep0, aep1 }
    1b0f:	078184        	ae_subsp24s	aep0, aep0, aep1
    1b12:	1aa004        	ae_selp24.ll	aep0, aep2, aep0
    1b15:	fc24e40caf 	{ ae_sp16x2f.c	aep0, a12, a10; nop }
    1b1a:	fc24e47caf 	{ ae_sp16x2f.c	aep7, a12, a10; nop }
    1b1f:	441704        	ae_lp16x2f.i	aep1, a7, 0
    1b22:	440804        	ae_lp16x2f.i	aep0, a8, 0
    1b25:	168b76        	loop	a11, 1b3f <synth_3band+0x6b>
    1b28:	fc24daacaf 	{ ae_lp16x2f.c	aep2, a12, a10; nop }
    1b2d:	e247dabcaf 	{ ae_lp16x2f.c	aep3, a12, a10; ae_mulafp24s.ll	aeq1, aep1, aep2 }
    1b32:	e2462d971f 	{ ae_lp16x2f.iu	aep1, a7, 4; ae_mulafp24s.hh	aeq0, aep1, aep2 }
    1b37:	010b94        	ae_mulsfp24s.hl	aeq1, aep0, aep3
    1b3a:	e262ad881f 	{ ae_lp16x2f.iu	aep0, a8, 4; ae_mulafp24s.lh	aeq0, aep0, aep3 }

00001b3f <synth_3band+0x6b>:
    1b3f:	f86b8ff99e 	{ addi	a9, a9, -1; ae_roundsp16q48asym	aep0, aeq1 }
    1b44:	f863f2861f 	{ ae_sp16f.l.iu	aep0, a6, 2; ae_roundsp16q48asym	aep1, aeq0 }
    1b49:	fd248c477e 	{ addi	a7, a7, -60; ae_zeroq56	aeq1 }
    1b4e:	fc0c4c488e 	{ addi	a8, a8, -60; ae_movq56	aeq0, aeq1 }
    1b53:	429614        	ae_sp16f.l.iu	aep1, a6, 2
    1b56:	faa956        	bnez	a9, 1b04 <synth_3band+0x30>
    1b59:	05c9           	s32i.n	a12, a5, 0
    1b5b:	f01d           	retw.n

00001b5d <synth_3band+0x89>:
    1b5d:	000000                                        ...

00001b60 <biquad_2cascade_24bitTx>:
    1b60:	00a136        	entry	a1, 80
    1b63:	542304        	ae_lp24x2.i	aep2, a3, 0
    1b66:	541314        	ae_lp24x2.i	aep1, a3, 8
    1b69:	540324        	ae_lp24x2.i	aep0, a3, 16
    1b6c:	6616a6        	blti	a6, 1, 1bd6 <biquad_2cascade_24bitTx+0x76>
    1b6f:	18c472        	addi	a7, a4, 24
    1b72:	543414        	ae_lp24x2.i	aep3, a4, 8
    1b75:	545404        	ae_lp24x2.i	aep5, a4, 0
    1b78:	546444        	ae_lp24x2.i	aep6, a4, 32
    1b7b:	547434        	ae_lp24x2.i	aep7, a4, 24
    1b7e:	563154        	ae_sp24x2s.i	aep3, a1, 40
    1b81:	565144        	ae_sp24x2s.i	aep5, a1, 32
    1b84:	545424        	ae_lp24x2.i	aep5, a4, 16
    1b87:	543454        	ae_lp24x2.i	aep3, a4, 40
    1b8a:	566134        	ae_sp24x2s.i	aep6, a1, 24
    1b8d:	567124        	ae_sp24x2s.i	aep7, a1, 16
    1b90:	fec542        	addi	a4, a5, -2
    1b93:	fcc222        	addi	a2, a2, -4
    1b96:	4c4764        	ae_lp24.i	aep4, a7, 24
    1b99:	398676        	loop	a6, 1bd6 <biquad_2cascade_24bitTx+0x76>
    1b9c:	547154        	ae_lp24x2.i	aep7, a1, 40
    1b9f:	f05eaee21f 	{ ae_lp24f.iu	aep6, a2, 4; ae_mulzaafp24s.hh.ll	aeq0, aep7, aep2 }
    1ba4:	dd59b0714f 	{ ae_lp24x2.i	aep7, a1, 32; ae_selp24.lh	aep2, aep6, aep2 }
    1ba9:	e25c30713f 	{ ae_lp24x2.i	aep7, a1, 24; ae_mulaafp24s.hh.ll	aeq0, aep7, aep2 }
    1bae:	015184        	ae_mulsfp24s.hh	aeq0, aep5, aep1
    1bb1:	0551c4        	ae_mulssfp24s.hh.ll	aeq0, aep5, aep1
    1bb4:	f13ef0712f 	{ ae_lp24x2.i	aep7, a1, 16; ae_mulzaafp24s.hh.ll	aeq3, aep7, aep1 }
    1bb9:	1b8d64        	ae_roundsp24q48asym	aep6, aeq0
    1bbc:	1ae194        	ae_selp24.lh	aep1, aep6, aep1
    1bbf:	05f904        	ae_mulaafp24s.hh.ll	aeq3, aep7, aep1
    1bc2:	01b884        	ae_mulsfp24s.hh	aeq3, aep3, aep0
    1bc5:	05b8c4        	ae_mulssfp24s.hh.ll	aeq3, aep3, aep0
    1bc8:	1bbd64        	ae_roundsp24q48asym	aep6, aeq3
    1bcb:	f0d2be602f 	{ ae_selp24.lh	aep0, aep6, aep0; ae_mulzaafp24s.hh.ll	aeq0, aep4, aep6 }
    1bd0:	1b8f74        	ae_roundsp16q48asym	aep7, aeq0
    1bd3:	42f414        	ae_sp16f.l.iu	aep7, a4, 2

00001bd6 <biquad_2cascade_24bitTx+0x76>:
    1bd6:	562304        	ae_sp24x2s.i	aep2, a3, 0
    1bd9:	561314        	ae_sp24x2s.i	aep1, a3, 8
    1bdc:	560324        	ae_sp24x2s.i	aep0, a3, 16
    1bdf:	f01d           	retw.n

00001be1 <biquad_2cascade_24bitTx+0x81>:
    1be1:	000000                                        ...

00001be4 <conv_24_to_16>:
    1be4:	004136        	entry	a1, 32
    1be7:	540c           	movi.n	a4, 5
    1be9:	fcc332        	addi	a3, a3, -4
    1bec:	f8c222        	addi	a2, a2, -8
    1bef:	088476        	loop	a4, 1bfb <conv_24_to_16+0x17>
    1bf2:	5c8214        	ae_lp24x2f.iu	aep0, a2, 8
    1bf5:	090184        	ae_roundsp16asym	aep0, aep0
    1bf8:	468314        	ae_sp16x2f.iu	aep0, a3, 4

00001bfb <conv_24_to_16+0x17>:
    1bfb:	f01d           	retw.n

00001bfd <conv_24_to_16+0x19>:
    1bfd:	000000                                        ...

00001c00 <gain_after_ec>:
    1c00:	00a136        	entry	a1, 80
    1c03:	f907c1        	l32r	a12, 20 (8688 <p_ecnr_mem>)
    1c06:	05ed           	mov.n	a14, a5
    1c08:	16a6f2        	movi	a15, 0x616
    1c0b:	0c58           	l32i.n	a5, a12, 0
    1c0d:	04dd           	mov.n	a13, a4
    1c0f:	904ff0        	addx2	a4, a15, a15
    1c12:	9169           	s32i.n	a6, a1, 36
    1c14:	443d04        	ae_lp16x2f.i	aep3, a13, 0
    1c17:	444e04        	ae_lp16x2f.i	aep4, a14, 0
    1c1a:	412544        	ae_lp16f.x	aep2, a5, a4
    1c1d:	07bc           	beqz.n	a7, 1c51 <gain_after_ec+0x51>
    1c1f:	1a0c           	movi.n	a10, 1
    1c21:	f93441        	l32r	a4, f4 (4630 <airoha_divide>)
    1c24:	11aa10        	slli	a10, a10, 15
    1c27:	07bd           	mov.n	a11, a7
    1c29:	0e5d           	mov.n	a5, a14
    1c2b:	0d6d           	mov.n	a6, a13
    1c2d:	562114        	ae_sp24x2s.i	aep2, a1, 8
    1c30:	563134        	ae_sp24x2s.i	aep3, a1, 24
    1c33:	564124        	ae_sp24x2s.i	aep4, a1, 16
    1c36:	0004e0        	callx8	a4
    1c39:	360b34        	ae_cvtq48a32s	aeq0, a11
    1c3c:	540114        	ae_lp24x2.i	aep0, a1, 8
    1c3f:	d4e230412f 	{ ae_lp24x2.i	aep4, a1, 16; ae_mulfq32sp16s.l	aeq0, aeq0, aep0 }
    1c44:	543134        	ae_lp24x2.i	aep3, a1, 24
    1c47:	340a94        	ae_sllaq56	aeq0, aeq0, a10
    1c4a:	f96793d66f 	{ or	a13, a6, a6; ae_roundsp24q48asym	aep2, aeq0 }
    1c4f:	05ed           	mov.n	a14, a5

00001c51 <gain_after_ec+0x51>:
    1c51:	fecd92        	addi	a9, a13, -2
    1c54:	fece82        	addi	a8, a14, -2
    1c57:	fec352        	addi	a5, a3, -2
    1c5a:	fec242        	addi	a4, a2, -2
    1c5d:	a60c           	movi.n	a6, 10
    1c5f:	8139           	s32i.n	a3, a1, 32
    1c61:	fec3b2        	addi	a11, a3, -2
    1c64:	fec2c2        	addi	a12, a2, -2
    1c67:	b1d9           	s32i.n	a13, a1, 44
    1c69:	a1e9           	s32i.n	a14, a1, 40
    1c6b:	08fd           	mov.n	a15, a8
    1c6d:	097d           	mov.n	a7, a9
    1c6f:	20a550        	or	a10, a5, a5
    1c72:	203440        	or	a3, a4, a4
    1c75:	598676        	loop	a6, 1cd2 <gain_after_ec+0xd2>
    1c78:	eb69ac8c1f 	{ ae_lp16f.iu	aep0, a12, 2; ae_mulfp24s.ll	aeq2, aep2, aep3 }
    1c7d:	e868ac9b1f 	{ ae_lp16f.iu	aep1, a11, 2; ae_mulfp24s.hh	aeq0, aep2, aep3 }
    1c82:	ea89faa18f 	{ ae_slliq56	aeq2, aeq2, 4; ae_mulfp24s.ll	aeq1, aep2, aep4 }
    1c87:	e988fa810f 	{ ae_slliq56	aeq0, aeq0, 4; ae_mulfp24s.hh	aeq3, aep2, aep4 }
    1c8c:	eb09ba213f 	{ ae_sq56s.i	aeq2, a1, 24; ae_mulfp24s.ll	aeq2, aep2, aep0 }
    1c91:	ea29ba012f 	{ ae_sq56s.i	aeq0, a1, 16; ae_mulfp24s.ll	aeq0, aep2, aep1 }
    1c96:	355114        	ae_slliq56	aeq1, aeq1, 4
    1c99:	631114        	ae_sq56s.i	aeq1, a1, 8
    1c9c:	356114        	ae_slliq56	aeq1, aeq2, 4
    1c9f:	618124        	ae_lq56.i	aeq2, a1, 16
    1ca2:	fbf3bdc13f 	{ ae_lq56.i	aeq2, a1, 24; ae_roundsp16q48sym	aep6, aeq2 }
    1ca7:	fbf3edbd1f 	{ ae_lp16x2f.iu	aep3, a13, 4; ae_roundsp16q48sym	aep7, aeq2 }
    1cac:	fbebb2e71f 	{ ae_sp16f.l.iu	aep6, a7, 2; ae_roundsp16q48sym	aep6, aeq1 }
    1cb1:	35f114        	ae_slliq56	aeq3, aeq3, 4
    1cb4:	f8fbf2f71f 	{ ae_sp16f.l.iu	aep7, a7, 2; ae_roundsp16q48sym	aep1, aeq3 }
    1cb9:	350114        	ae_slliq56	aeq0, aeq0, 4
    1cbc:	fbe3fde11f 	{ ae_lq56.i	aeq3, a1, 8; ae_roundsp16q48sym	aep7, aeq0 }
    1cc1:	fafbedce1f 	{ ae_lp16x2f.iu	aep4, a14, 4; ae_roundsp16q48sym	aep5, aeq3 }
    1cc6:	429f14        	ae_sp16f.l.iu	aep1, a15, 2
    1cc9:	42df14        	ae_sp16f.l.iu	aep5, a15, 2
    1ccc:	42e414        	ae_sp16f.l.iu	aep6, a4, 2
    1ccf:	42fa14        	ae_sp16f.l.iu	aep7, a10, 2

00001cd2 <gain_after_ec+0xd2>:
    1cd2:	9148           	l32i.n	a4, a1, 36
    1cd4:	a178           	l32i.n	a7, a1, 40
    1cd6:	b1a8           	l32i.n	a10, a1, 44
    1cd8:	021426        	beqi	a4, 1, 1cde <gain_after_ec+0xde>
    1cdb:	002146        	j	1d64 <gain_after_ec+0x164>
    1cde:	f8d041        	l32r	a4, 20 (8688 <p_ecnr_mem>)
    1ce1:	03fd           	mov.n	a15, a3
    1ce3:	8138           	l32i.n	a3, a1, 32
    1ce5:	0468           	l32i.n	a6, a4, 0
    1ce7:	ab4c           	movi.n	a11, 74
    1ce9:	46a0c2        	movi	a12, 70
    1cec:	91a4d2        	movi	a13, 0x491
    1cef:	49a2e2        	movi	a14, 0x249
    1cf2:	0aa042        	movi	a4, 10
    1cf5:	440a04        	ae_lp16x2f.i	aep0, a10, 0
    1cf8:	441704        	ae_lp16x2f.i	aep1, a7, 0
    1cfb:	12dbb2        	addmi	a11, a11, 0x1200
    1cfe:	12dcc2        	addmi	a12, a12, 0x1200
    1d01:	11dde0        	slli	a13, a13, 2
    1d04:	11eed0        	slli	a14, a14, 3
    1d07:	fec332        	addi	a3, a3, -2
    1d0a:	fec222        	addi	a2, a2, -2
    1d0d:	538476        	loop	a4, 1d64 <gain_after_ec+0x164>
    1d10:	4146b4        	ae_lp16f.x	aep4, a6, a11
    1d13:	e81099d6cf 	{ ae_lp16f.x	aep5, a6, a12; ae_mulfp24s.hh	aeq0, aep4, aep0 }
    1d18:	ea11eca21f 	{ ae_lp16f.iu	aep2, a2, 2; ae_mulfp24s.ll	aeq1, aep4, aep0 }
    1d1d:	e934ba802f 	{ ae_slliq56	aeq0, aeq0, 2; ae_mulfp24s.hh	aeq2, aep5, aep1 }
    1d22:	eb35fa906f 	{ ae_slliq56	aeq1, aeq1, 2; ae_mulfp24s.ll	aeq3, aep5, aep1 }
    1d27:	fae3ecb31f 	{ ae_lp16f.iu	aep3, a3, 2; ae_roundsp16q48sym	aep5, aeq0 }
    1d2c:	fbebad8a1f 	{ ae_lp16x2f.iu	aep0, a10, 4; ae_roundsp16q48sym	aep6, aeq1 }
    1d31:	42d914        	ae_sp16f.l.iu	aep5, a9, 2
    1d34:	42e914        	ae_sp16f.l.iu	aep6, a9, 2
    1d37:	35a094        	ae_slliq56	aeq2, aeq2, 2
    1d3a:	fbf3d9c6df 	{ ae_lp16f.x	aep4, a6, a13; ae_roundsp16q48sym	aep7, aeq2 }
    1d3f:	eb5199e6ef 	{ ae_lp16f.x	aep6, a6, a14; ae_mulfp24s.ll	aeq2, aep4, aep2 }
    1d44:	eb79fab02f 	{ ae_slliq56	aeq0, aeq3, 2; ae_mulfp24s.ll	aeq3, aep6, aep3 }
    1d49:	fae3faa06f 	{ ae_slliq56	aeq1, aeq2, 2; ae_roundsp16q48sym	aep5, aeq0 }
    1d4e:	fbebbab0ef 	{ ae_slliq56	aeq3, aeq3, 2; ae_roundsp16q48sym	aep6, aeq1 }
    1d53:	449714        	ae_lp16x2f.iu	aep1, a7, 4
    1d56:	fbfbf2f81f 	{ ae_sp16f.l.iu	aep7, a8, 2; ae_roundsp16q48sym	aep7, aeq3 }
    1d5b:	42d814        	ae_sp16f.l.iu	aep5, a8, 2
    1d5e:	42ef14        	ae_sp16f.l.iu	aep6, a15, 2
    1d61:	42f514        	ae_sp16f.l.iu	aep7, a5, 2

00001d64 <gain_after_ec+0x164>:
    1d64:	f01d           	retw.n

00001d66 <gain_after_ec+0x166>:
	...

00001d68 <adjust_phase>:
    1d68:	004136        	entry	a1, 32
    1d6b:	300334        	ae_cvtp24a16x2.ll	aep0, a3, a3
    1d6e:	360234        	ae_cvtq48a32s	aeq0, a2
    1d71:	a10004        	ae_mulzaafq32sp16s.ll	aeq0, aeq0, aep0, aeq0, aep0
    1d74:	1b8f04        	ae_roundsp16q48asym	aep0, aeq0
    1d77:	0002d4        	ae_cvta32p24.l	a2, aep0
    1d7a:	f01d           	retw.n

00001d7c <adjust_bib_state>:
    1d7c:	004136        	entry	a1, 32
    1d7f:	840c           	movi.n	a4, 8
    1d81:	fcc232        	addi	a3, a2, -4
    1d84:	600204        	ae_lq32f.i	aeq0, a2, 0
    1d87:	0a8476        	loop	a4, 1d95 <adjust_bib_state+0x19>
    1d8a:	350074        	ae_sraiq56	aeq0, aeq0, 1
    1d8d:	f867bd121f 	{ ae_lq32f.iu	aeq0, a2, 4; ae_roundsp24q48asym	aep0, aeq0 }
    1d92:	528314        	ae_sp24f.l.iu	aep0, a3, 4

00001d95 <adjust_bib_state+0x19>:
    1d95:	f01d           	retw.n

00001d97 <adjust_bib_state+0x1b>:
	...

00001d98 <gain_ref>:
    1d98:	004136        	entry	a1, 32
    1d9b:	2314a6        	blti	a4, 1, 1dc2 <gain_ref+0x2a>
    1d9e:	fcc222        	addi	a2, a2, -4
    1da1:	300334        	ae_cvtp24a16x2.ll	aep0, a3, a3
    1da4:	449214        	ae_lp16x2f.iu	aep1, a2, 4
    1da7:	178476        	loop	a4, 1dc2 <gain_ref+0x2a>
    1daa:	011054        	ae_mulfp24s.hl	aeq0, aep1, aep0
    1dad:	ea05ed921f 	{ ae_lp16x2f.iu	aep1, a2, 4; ae_mulfp24s.ll	aeq1, aep1, aep0 }
    1db2:	350094        	ae_slliq56	aeq0, aeq0, 2
    1db5:	f963ba906f 	{ ae_slliq56	aeq1, aeq1, 2; ae_roundsp16q48asym	aep2, aeq0 }
    1dba:	f96bf222ef 	{ ae_sp16f.l.i	aep2, a2, -4; ae_roundsp16q48asym	aep3, aeq1 }
    1dbf:	4232f4        	ae_sp16f.l.i	aep3, a2, -2

00001dc2 <gain_ref+0x2a>:
    1dc2:	f01d           	retw.n

00001dc4 <TxRef_VAD>:
    1dc4:	006136        	entry	a1, 48
    1dc7:	f0a052        	movi	a5, 240
    1dca:	00a042        	movi	a4, 0
    1dcd:	088576        	loop	a5, 1dd9 <TxRef_VAD+0x15>
    1dd0:	235d           	ae_l16si.n	a5, a3, 0
    1dd2:	332b           	addi.n	a3, a3, 2
    1dd4:	605150        	abs	a5, a5
    1dd7:	454a           	add.n	a4, a5, a4

00001dd9 <TxRef_VAD+0x15>:
    1dd9:	f57c           	movi.n	a5, -1
    1ddb:	015510        	slli	a5, a5, 31
    1dde:	04bc           	beqz.n	a4, 1e12 <TxRef_VAD+0x4e>
    1de0:	40e430        	nsa	a3, a4
    1de3:	761c           	movi.n	a6, 23
    1de5:	401300        	ssl	a3
    1de8:	c0b630        	sub	a11, a6, a3
    1deb:	f8d531        	l32r	a3, 140 (56c <Log2_norm>)
    1dee:	a1a400        	sll	a10, a4
    1df1:	0003e0        	callx8	a3
    1df4:	d1a432        	movi	a3, 0x4d1
    1df7:	1133c0        	slli	a3, a3, 4
    1dfa:	b26a30        	mulsh	a6, a10, a3
    1dfd:	823a30        	mull	a3, a10, a3
    1e00:	d1a442        	movi	a4, 0x4d1
    1e03:	1144f0        	slli	a4, a4, 1
    1e06:	f35a           	add.n	a15, a3, a5
    1e08:	464a           	add.n	a4, a6, a4
    1e0a:	623f30        	saltu	a3, a15, a3
    1e0d:	334a           	add.n	a3, a3, a4
    1e0f:	000046        	j	1e14 <TxRef_VAD+0x50>

00001e12 <TxRef_VAD+0x4e>:
    1e12:	030c           	movi.n	a3, 0
    1e14:	039262        	l16si	a6, a2, 6
    1e17:	049272        	l16si	a7, a2, 8
    1e1a:	029242        	l16si	a4, a2, 4
    1e1d:	11e600        	slli	a14, a6, 16
    1e20:	11f700        	slli	a15, a7, 16
    1e23:	306650        	xor	a6, a6, a5
    1e26:	307750        	xor	a7, a7, a5
    1e29:	533340        	max	a3, a3, a4
    1e2c:	c046e0        	sub	a4, a6, a14
    1e2f:	c067f0        	sub	a6, a7, a15
    1e32:	2149           	s32i.n	a4, a1, 8
    1e34:	3169           	s32i.n	a6, a1, 12
    1e36:	418b           	addi.n	a4, a1, 8
    1e38:	42a8           	l32i.n	a10, a2, 16
    1e3a:	f8c261        	l32r	a6, 144 (5b0 <siir_safr>)
    1e3d:	03bd           	mov.n	a11, a3
    1e3f:	04cd           	mov.n	a12, a4
    1e41:	0006e0        	callx8	a6
    1e44:	059272        	l16si	a7, a2, 10
    1e47:	069282        	l16si	a8, a2, 12
    1e4a:	42a9           	s32i.n	a10, a2, 16
    1e4c:	119700        	slli	a9, a7, 16
    1e4f:	307750        	xor	a7, a7, a5
    1e52:	30f850        	xor	a15, a8, a5
    1e55:	118800        	slli	a8, a8, 16
    1e58:	52a8           	l32i.n	a10, a2, 20
    1e5a:	c07790        	sub	a7, a7, a9
    1e5d:	c05f80        	sub	a5, a15, a8
    1e60:	03bd           	mov.n	a11, a3
    1e62:	04cd           	mov.n	a12, a4
    1e64:	2179           	s32i.n	a7, a1, 8
    1e66:	3159           	s32i.n	a5, a1, 12
    1e68:	0006e0        	callx8	a6
    1e6b:	4238           	l32i.n	a3, a2, 16
    1e6d:	224d           	ae_l16si.n	a4, a2, 0
    1e6f:	52a9           	s32i.n	a10, a2, 20
    1e71:	0c1272        	l16ui	a7, a2, 24
    1e74:	334a           	add.n	a3, a3, a4
    1e76:	04a3a7        	bge	a3, a10, 1e7e <TxRef_VAD+0xba>
    1e79:	130c           	movi.n	a3, 1
    1e7b:	000386        	j	1e8d <TxRef_VAD+0xc9>

00001e7e <TxRef_VAD+0xba>:
    1e7e:	0d9242        	l16si	a4, a2, 26
    1e81:	073d           	mov.n	a3, a7
    1e83:	0b1766        	bnei	a7, 1, 1e92 <TxRef_VAD+0xce>
    1e86:	325d           	ae_l16si.n	a5, a2, 2
    1e88:	06a547        	bge	a5, a4, 1e92 <TxRef_VAD+0xce>
    1e8b:	030c           	movi.n	a3, 0
    1e8d:	6239           	s32i.n	a3, a2, 24
    1e8f:	000106        	j	1e97 <TxRef_VAD+0xd3>

00001e92 <TxRef_VAD+0xce>:
    1e92:	441b           	addi.n	a4, a4, 1
    1e94:	0d5242        	s16i	a4, a2, 26
    1e97:	832d           	ae_sext16	a2, a3
    1e99:	f01d           	retw.n

00001e9b <TxRef_VAD+0xd7>:
	...

00001e9c <PEC_Switch>:
    1e9c:	004136        	entry	a1, 32
    1e9f:	b48c           	beqz.n	a4, 1eae <PEC_Switch+0x12>
    1ea1:	025d           	mov.n	a5, a2
    1ea3:	092426        	beqi	a4, 2, 1eb0 <PEC_Switch+0x14>
    1ea6:	032d           	mov.n	a2, a3
    1ea8:	021426        	beqi	a4, 1, 1eae <PEC_Switch+0x12>
    1eab:	432530        	min	a2, a5, a3

00001eae <PEC_Switch+0x12>:
    1eae:	f01d           	retw.n

00001eb0 <PEC_Switch+0x14>:
    1eb0:	532530        	max	a2, a5, a3
    1eb3:	f01d           	retw.n

00001eb5 <PEC_Switch+0x19>:
    1eb5:	000000                                        ...

00001eb8 <Voice_EC_Prcs_common>:
    1eb8:	03c136        	entry	a1, 0x1e0
    1ebb:	06ad           	mov.n	a10, a6
    1ebd:	056d           	mov.n	a6, a5
    1ebf:	f85851        	l32r	a5, 20 (8688 <p_ecnr_mem>)
    1ec2:	1d6192        	s32i	a9, a1, 116
    1ec5:	180c           	movi.n	a8, 1
    1ec7:	05b8           	l32i.n	a11, a5, 0
    1ec9:	0d0c           	movi.n	a13, 0
    1ecb:	1c6142        	s32i	a4, a1, 112
    1ece:	12db92        	addmi	a9, a11, 0x1200
    1ed1:	2619c2        	l16ui	a12, a9, 76
    1ed4:	2bdbf2        	addmi	a15, a11, 0x2b00
    1ed7:	286172        	s32i	a7, a1, 160
    1eda:	fecce2        	addi	a14, a12, -2
    1edd:	216132        	s32i	a3, a1, 132
    1ee0:	624d30        	saltu	a4, a13, a3
    1ee3:	627e80        	saltu	a7, a14, a8
    1ee6:	251f32        	l16ui	a3, a15, 74
    1ee9:	104470        	and	a4, a4, a7
    1eec:	f84c71        	l32r	a7, 1c (6c04 <xt_memcpy>)
    1eef:	06c316        	beqz	a3, 1f5f <Voice_EC_Prcs_common+0xa7>
    1ef2:	46dbd2        	addmi	a13, a11, 0x4600
    1ef5:	541d32        	l16ui	a3, a13, 168
    1ef8:	336122        	s32i	a2, a1, 204
    1efb:	3a2d22        	l32i	a2, a13, 232
    1efe:	20dbc2        	addmi	a12, a11, 0x2000
    1f01:	1aa6f2        	movi	a15, 0x61a
    1f04:	416142        	s32i	a4, a1, 0x104
    1f07:	904ff0        	addx2	a4, a15, a15
    1f0a:	3661a2        	s32i	a10, a1, 216
    1f0d:	2261c2        	s32i	a12, a1, 136
    1f10:	1a61b2        	s32i	a11, a1, 104
    1f13:	060316        	beqz	a3, 1f77 <Voice_EC_Prcs_common+0xbf>
    1f16:	a8a032        	movi	a3, 168
    1f19:	46d332        	addmi	a3, a3, 0x4600
    1f1c:	ab3a           	add.n	a10, a11, a3
    1f1e:	f88a31        	l32r	a3, 148 (1dc4 <TxRef_VAD>)
    1f21:	06bd           	mov.n	a11, a6
    1f23:	0003e0        	callx8	a3
    1f26:	05c8           	l32i.n	a12, a5, 0
    1f28:	1a21b2        	l32i	a11, a1, 104
    1f2b:	12dc52        	addmi	a5, a12, 0x1200
    1f2e:	271552        	l16ui	a5, a5, 78
    1f31:	4c4a           	add.n	a4, a12, a4
    1f33:	857d           	ae_sext16	a7, a5
    1f35:	07da16        	beqz	a10, 1fb6 <Voice_EC_Prcs_common+0xfe>

00001f38 <Voice_EC_Prcs_common+0x80>:
    1f38:	858d           	ae_sext16	a8, a5
    1f3a:	196c           	movi.n	a9, -31
    1f3c:	082897        	blt	a8, a9, 1f48 <Voice_EC_Prcs_common+0x90>
    1f3f:	570b           	addi.n	a5, a7, -1
    1f41:	ff7c           	movi.n	a15, -1
    1f43:	4355f0        	min	a5, a5, a15
    1f46:	645d           	ae_s16i.n	a5, a4, 0
    1f48:	29db42        	addmi	a4, a11, 0x2900
    1f4b:	199472        	l16si	a7, a4, 50
    1f4e:	2221a2        	l32i	a10, a1, 136
    1f51:	ffa182        	movi	a8, 0x1ff
    1f54:	6f2877        	blt	a8, a7, 1fc7 <Voice_EC_Prcs_common+0x10f>
    1f57:	771b           	addi.n	a7, a7, 1
    1f59:	195472        	s16i	a7, a4, 50
    1f5c:	0019c6        	j	1fc7 <Voice_EC_Prcs_common+0x10f>

00001f5f <Voice_EC_Prcs_common+0xa7>:
    1f5f:	f0a0c2        	movi	a12, 240
    1f62:	02bd           	mov.n	a11, a2
    1f64:	0007e0        	callx8	a7
    1f67:	a48c           	beqz.n	a4, 1f75 <Voice_EC_Prcs_common+0xbd>
    1f69:	2821a2        	l32i	a10, a1, 160
    1f6c:	2121b2        	l32i	a11, a1, 132
    1f6f:	f0a0c2        	movi	a12, 240
    1f72:	0007e0        	callx8	a7
    1f75:	f01d           	retw.n

00001f77 <Voice_EC_Prcs_common+0xbf>:
    1f77:	8da432        	movi	a3, 0x48d
    1f7a:	1133e0        	slli	a3, a3, 2
    1f7d:	650c           	movi.n	a5, 6
    1f7f:	3b3a           	add.n	a3, a11, a3
    1f81:	058576        	loop	a5, 1f8a <Voice_EC_Prcs_common+0xd2>
    1f84:	335d           	ae_l16si.n	a5, a3, 2
    1f86:	635d           	ae_s16i.n	a5, a3, 0
    1f88:	332b           	addi.n	a3, a3, 2

00001f8a <Voice_EC_Prcs_common+0xd2>:
    1f8a:	12db52        	addmi	a5, a11, 0x1200
    1f8d:	289552        	l16si	a5, a5, 80
    1f90:	46db72        	addmi	a7, a11, 0x4600
    1f93:	12db82        	addmi	a8, a11, 0x1200
    1f96:	115590        	slli	a5, a5, 7
    1f99:	635d           	ae_s16i.n	a5, a3, 0
    1f9b:	521732        	l16ui	a3, a7, 164
    1f9e:	279872        	l16si	a7, a8, 78
    1fa1:	ffa392        	movi	a9, 0x3ff
    1fa4:	9033b0        	addx2	a3, a3, a11
    1fa7:	12d332        	addmi	a3, a3, 0x1200
    1faa:	1a93f2        	l16si	a15, a3, 52
    1fad:	4b4a           	add.n	a4, a11, a4
    1faf:	0bcd           	mov.n	a12, a11
    1fb1:	075d           	mov.n	a5, a7
    1fb3:	8129f7        	blt	a9, a15, 1f38 <Voice_EC_Prcs_common+0x80>

00001fb6 <Voice_EC_Prcs_common+0xfe>:
    1fb6:	2221a2        	l32i	a10, a1, 136
    1fb9:	858d           	ae_sext16	a8, a5
    1fbb:	08c8e6        	bgei	a8, 32, 1fc7 <Voice_EC_Prcs_common+0x10f>
    1fbe:	571b           	addi.n	a5, a7, 1
    1fc0:	1f0c           	movi.n	a15, 1
    1fc2:	5355f0        	max	a5, a5, a15
    1fc5:	645d           	ae_s16i.n	a5, a4, 0
    1fc7:	854d           	ae_sext16	a4, a5
    1fc9:	29db82        	addmi	a8, a11, 0x2900
    1fcc:	030c           	movi.n	a3, 0
    1fce:	29db92        	addmi	a9, a11, 0x2900
    1fd1:	2adb72        	addmi	a7, a11, 0x2a00
    1fd4:	15a4e2        	movi	a14, 0x415
    1fd7:	316142        	s32i	a4, a1, 196
    1fda:	286932        	s32i	a3, a9, 160
    1fdd:	296932        	s32i	a3, a9, 164
    1fe0:	114ee0        	slli	a4, a14, 2
    1fe3:	46dcf2        	addmi	a15, a12, 0x4600
    1fe6:	386932        	s32i	a3, a9, 224
    1fe9:	396832        	s32i	a3, a8, 228
    1fec:	186832        	s32i	a3, a8, 96
    1fef:	8739           	s32i.n	a3, a7, 32
    1ff1:	196832        	s32i	a3, a8, 100
    1ff4:	9739           	s32i.n	a3, a7, 36
    1ff6:	cc4a           	add.n	a12, a12, a4
    1ff8:	531fd2        	l16ui	a13, a15, 166
    1ffb:	f85441        	l32r	a4, 14c (1804 <mic240_delay>)
    1ffe:	06bd           	mov.n	a11, a6
    2000:	446182        	s32i	a8, a1, 0x110
    2003:	0004e0        	callx8	a4
    2006:	f83041        	l32r	a4, c8 (6bf0 <xt_memset>)
    2009:	40c1a2        	addi	a10, a1, 64
    200c:	0b0c           	movi.n	a11, 0
    200e:	ec0c           	movi.n	a12, 14
    2010:	0004e0        	callx8	a4
    2013:	1a21a2        	l32i	a10, a1, 104
    2016:	1c2162        	l32i	a6, a1, 112
    2019:	84a782        	movi	a8, 0x784
    201c:	26da42        	addmi	a4, a10, 0x2600
    201f:	a08880        	addx4	a8, a8, a8
    2022:	385432        	s16i	a3, a4, 112
    2025:	1b6122        	s32i	a2, a1, 108
    2028:	15e616        	beqz	a6, 218a <Voice_EC_Prcs_common+0x2d2>
    202b:	7ac252        	addi	a5, a2, 122
    202e:	ff95f2        	l16si	a15, a5, 0x1fe
    2031:	326232        	s32i	a3, a2, 200
    2034:	426232        	s32i	a3, a2, 0x108
    2037:	526232        	s32i	a3, a2, 0x148
    203a:	626232        	s32i	a3, a2, 0x188
    203d:	336232        	s32i	a3, a2, 204
    2040:	436232        	s32i	a3, a2, 0x10c
    2043:	536232        	s32i	a3, a2, 0x14c
    2046:	636232        	s32i	a3, a2, 0x18c
    2049:	78a042        	movi	a4, 120
    204c:	398ff4        	ae_movpa24x2	aep0, a15, a15
    204f:	203660        	or	a3, a6, a6
    2052:	441604        	ae_lp16x2f.i	aep1, a6, 0
    2055:	188476        	loop	a4, 2071 <Voice_EC_Prcs_common+0x1b9>
    2058:	011054        	ae_mulfp24s.hl	aeq0, aep1, aep0
    205b:	011874        	ae_mulfp24s.ll	aeq1, aep1, aep0
    205e:	350214        	ae_slliq56	aeq0, aeq0, 8
    2061:	fb63fa924f 	{ ae_slliq56	aeq1, aeq1, 8; ae_roundsp16q48asym	aep7, aeq0 }
    2066:	f96bb2730f 	{ ae_sp16f.l.i	aep7, a3, 0; ae_roundsp16q48asym	aep2, aeq1 }
    206b:	422314        	ae_sp16f.l.i	aep2, a3, 2
    206e:	449314        	ae_lp16x2f.iu	aep1, a3, 4

00002071 <Voice_EC_Prcs_common+0x1b9>:
    2071:	f7eb21        	l32r	a2, 20 (8688 <p_ecnr_mem>)
    2074:	1c21f2        	l32i	a15, a1, 112
    2077:	78a072        	movi	a7, 120
    207a:	02a8           	l32i.n	a10, a2, 0
    207c:	fccf32        	addi	a3, a15, -4
    207f:	204330        	or	a4, a3, a3
    2082:	f8ca62        	addi	a6, a10, -8
    2085:	058776        	loop	a7, 208e <Voice_EC_Prcs_common+0x1d6>
    2088:	448414        	ae_lp16x2f.iu	aep0, a4, 4
    208b:	5e8614        	ae_sp24x2f.iu	aep0, a6, 8
    208e:	1b2122        	l32i	a2, a1, 108
    2091:	1c21d2        	l32i	a13, a1, 112
    2094:	f82f41        	l32r	a4, 150 (1b60 <biquad_2cascade_24bitTx>)
    2097:	70c2c2        	addi	a12, a2, 112
    209a:	f0a0e2        	movi	a14, 240
    209d:	02bd           	mov.n	a11, a2
    209f:	0004e0        	callx8	a4
    20a2:	02d242        	addmi	a4, a2, 0x200
    20a5:	451472        	l16ui	a7, a4, 138
    20a8:	1a21a2        	l32i	a10, a1, 104
    20ab:	84a782        	movi	a8, 0x784
    20ae:	a08880        	addx4	a8, a8, a8
    20b1:	02e7c7        	bbsi	a7, 12, 20b7 <Voice_EC_Prcs_common+0x1ff>
    20b4:	003486        	j	218a <Voice_EC_Prcs_common+0x2d2>
    20b7:	f7da21        	l32r	a2, 20 (8688 <p_ecnr_mem>)
    20ba:	c0a3f2        	movi	a15, 0x3c0
    20bd:	78a092        	movi	a9, 120
    20c0:	002242        	l32i	a4, a2, 0
    20c3:	8064f0        	add	a6, a4, a15
    20c6:	f8c482        	addi	a8, a4, -8
    20c9:	058976        	loop	a9, 20d2 <Voice_EC_Prcs_common+0x21a>
    20cc:	448314        	ae_lp16x2f.iu	aep0, a3, 4
    20cf:	5e8814        	ae_sp24x2f.iu	aep0, a8, 8
    20d2:	1b2122        	l32i	a2, a1, 108
    20d5:	f81fc1        	l32r	a12, 154 (7f00 <nc_vad_lpf_coef>)
    20d8:	f81e31        	l32r	a3, 150 (1b60 <biquad_2cascade_24bitTx>)
    20db:	18c2b2        	addi	a11, a2, 24
    20de:	f0a0e2        	movi	a14, 240
    20e1:	04ad           	mov.n	a10, a4
    20e3:	06dd           	mov.n	a13, a6
    20e5:	0003e0        	callx8	a3
    20e8:	fc2483c03f 	{ movi	a3, 0x3c0; ae_zeroq56	aeq0 }
    20ed:	ff1552        	l16ui	a5, a5, 0x1fe
    20f0:	04d282        	addmi	a8, a2, 0x400
    20f3:	fa7c           	movi.n	a10, -1
    20f5:	410434        	ae_lp16f.x	aep0, a4, a3
    20f8:	1c1832        	l16ui	a3, a8, 56
    20fb:	1d9882        	l16si	a8, a8, 58
    20fe:	01aa10        	slli	a10, a10, 31
    2101:	ff0c           	movi.n	a15, 15
    2103:	c25a50        	quou	a5, a10, a5
    2106:	f0a092        	movi	a9, 240
    2109:	364534        	ae_cvtq48a32s	aeq1, a5
    210c:	c04f80        	sub	a4, a15, a8
    210f:	0f8976        	loop	a9, 2122 <Voice_EC_Prcs_common+0x26a>
    2112:	d5e32c861f 	{ ae_lp16f.iu	aep0, a6, 2; ae_mulfq32sp16s.l	aeq2, aeq1, aep0 }
    2117:	fc24fea48f 	{ ae_slaasq56s	aeq2, aeq2, a4; nop }
    211c:	069a64        	ae_absq56	aeq2, aeq2
    211f:	010ac4        	ae_addq56	aeq0, aeq0, aeq2
    2122:	060934        	ae_roundsq32asym	aeq0, aeq0
    2125:	368404        	ae_trunca32q48	a4, aeq0
    2128:	113300        	slli	a3, a3, 16
    212b:	f80b61        	l32r	a6, 158 (7f800080 <_end+0x7f7f79f4>)
    212e:	04d252        	addmi	a5, a2, 0x400
    2131:	1e6132        	s32i	a3, a1, 120
    2134:	8169           	s32i.n	a6, a1, 32
    2136:	20c162        	addi	a6, a1, 32
    2139:	f5a8           	l32i.n	a10, a5, 60
    213b:	f80231        	l32r	a3, 144 (5b0 <siir_safr>)
    213e:	f80781        	l32r	a8, 15c (78000800 <_end+0x77ff8174>)
    2141:	04bd           	mov.n	a11, a4
    2143:	06cd           	mov.n	a12, a6
    2145:	9189           	s32i.n	a8, a1, 36
    2147:	0003e0        	callx8	a3
    214a:	f5a9           	s32i.n	a10, a5, 60
    214c:	1025a2        	l32i	a10, a5, 64
    214f:	f80481        	l32r	a8, 160 (17fff <_end+0xf973>)
    2152:	f80491        	l32r	a9, 164 (7c000400 <_end+0x7bff7d74>)
    2155:	04bd           	mov.n	a11, a4
    2157:	06cd           	mov.n	a12, a6
    2159:	8189           	s32i.n	a8, a1, 32
    215b:	9199           	s32i.n	a9, a1, 36
    215d:	0003e0        	callx8	a3
    2160:	f538           	l32i.n	a3, a5, 60
    2162:	1e2142        	l32i	a4, a1, 120
    2165:	1065a2        	s32i	a10, a5, 64
    2168:	c03a30        	sub	a3, a10, a3
    216b:	022437        	blt	a4, a3, 2171 <Voice_EC_Prcs_common+0x2b9>
    216e:	03c346        	j	307f <Voice_EC_Prcs_common+0x11c7>
    2171:	1a21a2        	l32i	a10, a1, 104
    2174:	84a782        	movi	a8, 0x784
    2177:	140c           	movi.n	a4, 1
    2179:	04d232        	addmi	a3, a2, 0x400
    217c:	0f0c           	movi.n	a15, 0
    217e:	a08880        	addx4	a8, a8, a8
    2181:	464542        	s8i	a4, a5, 70
    2184:	2253f2        	s16i	a15, a3, 68
    2187:	ffffc6        	j	218a <Voice_EC_Prcs_common+0x2d2>

0000218a <Voice_EC_Prcs_common+0x2d2>:
    218a:	1d2132        	l32i	a3, a1, 116
    218d:	782132        	l32i	a3, a1, 0x1e0
    2190:	9da4f2        	movi	a15, 0x49d
    2193:	436132        	s32i	a3, a1, 0x10c
    2196:	113fd0        	slli	a3, a15, 3
    2199:	3a3a           	add.n	a3, a10, a3
    219b:	29a552        	movi	a5, 0x529
    219e:	1f6132        	s32i	a3, a1, 124
    21a1:	1135d0        	slli	a3, a5, 3
    21a4:	3a3a           	add.n	a3, a10, a3
    21a6:	31a562        	movi	a6, 0x531
    21a9:	6e6132        	s32i	a3, a1, 0x1b8
    21ac:	1136d0        	slli	a3, a6, 3
    21af:	3a3a           	add.n	a3, a10, a3
    21b1:	39a592        	movi	a9, 0x539
    21b4:	6d6132        	s32i	a3, a1, 0x1b4
    21b7:	1139d0        	slli	a3, a9, 3
    21ba:	3a3a           	add.n	a3, a10, a3
    21bc:	41a5b2        	movi	a11, 0x541
    21bf:	6c6132        	s32i	a3, a1, 0x1b0
    21c2:	113bd0        	slli	a3, a11, 3
    21c5:	3a3a           	add.n	a3, a10, a3
    21c7:	1d0c           	movi.n	a13, 1
    21c9:	3121e2        	l32i	a14, a1, 196
    21cc:	6b6132        	s32i	a3, a1, 0x1ac
    21cf:	113d40        	slli	a3, a13, 12
    21d2:	ca8a           	add.n	a12, a10, a8
    21d4:	190c           	movi.n	a9, 1
    21d6:	103730        	and	a3, a7, a3
    21d9:	3b0c           	movi.n	a11, 3
    21db:	a4a052        	movi	a5, 164
    21de:	b0a162        	movi	a6, 0x1b0
    21e1:	30a182        	movi	a8, 0x130
    21e4:	0f0c           	movi.n	a15, 0
    21e6:	4761c2        	s32i	a12, a1, 0x11c
    21e9:	696132        	s32i	a3, a1, 0x1a4
    21ec:	723fe0        	salt	a3, a15, a14
    21ef:	624490        	saltu	a4, a4, a9
    21f2:	f25a           	add.n	a15, a2, a5
    21f4:	526a           	add.n	a5, a2, a6
    21f6:	40db72        	addmi	a7, a11, 0x4000
    21f9:	628a           	add.n	a6, a2, a8
    21fb:	b0a0e2        	movi	a14, 176
    21fe:	70a192        	movi	a9, 0x170
    2201:	fca0c2        	movi	a12, 252
    2204:	f0a0d2        	movi	a13, 240
    2207:	103340        	and	a3, a3, a4
    220a:	7fdc42        	addmi	a4, a12, 0x7f00
    220d:	300774        	ae_cvtp24a16x2.ll	aep0, a7, a7
    2210:	82ea           	add.n	a8, a2, a14
    2212:	666162        	s32i	a6, a1, 0x198
    2215:	72da           	add.n	a7, a2, a13
    2217:	b29a           	add.n	a11, a2, a9
    2219:	94a2e2        	movi	a14, 0x294
    221c:	59a562        	movi	a6, 0x559
    221f:	307444        	ae_cvtp24a16x2.ll	aep7, a4, a4
    2222:	6861f2        	s32i	a15, a1, 0x1a0
    2225:	656172        	s32i	a7, a1, 0x194
    2228:	5661b2        	s32i	a11, a1, 0x158
    222b:	98a3c2        	movi	a12, 0x398
    222e:	f2ea           	add.n	a15, a2, a14
    2230:	1146d0        	slli	a4, a6, 3
    2233:	a8a072        	movi	a7, 168
    2236:	51a5b2        	movi	a11, 0x551
    2239:	d2ca           	add.n	a13, a2, a12
    223b:	9a4a           	add.n	a9, a10, a4
    223d:	556132        	s32i	a3, a1, 0x154
    2240:	646182        	s32i	a8, a1, 0x190
    2243:	5361f2        	s32i	a15, a1, 0x14c
    2246:	827a           	add.n	a8, a2, a7
    2248:	f0a132        	movi	a3, 0x1f0
    224b:	114bd0        	slli	a4, a11, 3
    224e:	61a5f2        	movi	a15, 0x561
    2251:	676152        	s32i	a5, a1, 0x19c
    2254:	523a           	add.n	a5, a2, a3
    2256:	5461d2        	s32i	a13, a1, 0x150
    2259:	da4a           	add.n	a13, a10, a4
    225b:	626182        	s32i	a8, a1, 0x188
    225e:	49a5c2        	movi	a12, 0x549
    2261:	114fd0        	slli	a4, a15, 3
    2264:	7ca082        	movi	a8, 124
    2267:	636152        	s32i	a5, a1, 0x18c
    226a:	5a4a           	add.n	a5, a10, a4
    226c:	25d842        	addmi	a4, a8, 0x2500
    226f:	116cd0        	slli	a6, a12, 3
    2272:	64a032        	movi	a3, 100
    2275:	ea6a           	add.n	a14, a10, a6
    2277:	5c6192        	s32i	a9, a1, 0x170
    227a:	25d362        	addmi	a6, a3, 0x2500
    227d:	ba4a           	add.n	a11, a10, a4
    227f:	d1a492        	movi	a9, 0x4d1
    2282:	7a6a           	add.n	a7, a10, a6
    2284:	1169d0        	slli	a6, a9, 3
    2287:	5b61d2        	s32i	a13, a1, 0x16c
    228a:	5a61e2        	s32i	a14, a1, 0x168
    228d:	4e61b2        	s32i	a11, a1, 0x138
    2290:	4d1c           	movi.n	a13, 20
    2292:	2adab2        	addmi	a11, a10, 0x2a00
    2295:	3da1e2        	movi	a14, 0x13d
    2298:	ca6a           	add.n	a12, a10, a6
    229a:	27dd42        	addmi	a4, a13, 0x2700
    229d:	116eb0        	slli	a6, a14, 5
    22a0:	2761b2        	s32i	a11, a1, 156
    22a3:	25dab2        	addmi	a11, a10, 0x2500
    22a6:	fa4a           	add.n	a15, a10, a4
    22a8:	4a6a           	add.n	a4, a10, a6
    22aa:	48c162        	addi	a6, a1, 72
    22ad:	29da82        	addmi	a8, a10, 0x2900
    22b0:	44c1d2        	addi	a13, a1, 68
    22b3:	4b61b2        	s32i	a11, a1, 0x12c
    22b6:	24dab2        	addmi	a11, a10, 0x2400
    22b9:	2561c2        	s32i	a12, a1, 148
    22bc:	3c61f2        	s32i	a15, a1, 240
    22bf:	6a6162        	s32i	a6, a1, 0x1a8
    22c2:	426182        	s32i	a8, a1, 0x108
    22c5:	4cc1c2        	addi	a12, a1, 76
    22c8:	5861d2        	s32i	a13, a1, 0x160
    22cb:	2adae2        	addmi	a14, a10, 0x2a00
    22ce:	2adaf2        	addmi	a15, a10, 0x2a00
    22d1:	28c162        	addi	a6, a1, 40
    22d4:	25da82        	addmi	a8, a10, 0x2500
    22d7:	25dad2        	addmi	a13, a10, 0x2500
    22da:	4961b2        	s32i	a11, a1, 0x124
    22dd:	04d2b2        	addmi	a11, a2, 0x400
    22e0:	596152        	s32i	a5, a1, 0x164
    22e3:	4f6172        	s32i	a7, a1, 0x13c
    22e6:	3b6142        	s32i	a4, a1, 236
    22e9:	29da52        	addmi	a5, a10, 0x2900
    22ec:	29da72        	addmi	a7, a10, 0x2900
    22ef:	3061c2        	s32i	a12, a1, 192
    22f2:	3f61e2        	s32i	a14, a1, 252
    22f5:	4061f2        	s32i	a15, a1, 0x100
    22f8:	2ada42        	addmi	a4, a10, 0x2a00
    22fb:	4d6162        	s32i	a6, a1, 0x134
    22fe:	376182        	s32i	a8, a1, 220
    2301:	24dac2        	addmi	a12, a10, 0x2400
    2304:	2461d2        	s32i	a13, a1, 144
    2307:	25dae2        	addmi	a14, a10, 0x2500
    230a:	25daf2        	addmi	a15, a10, 0x2500
    230d:	27da62        	addmi	a6, a10, 0x2700
    2310:	28da82        	addmi	a8, a10, 0x2800
    2313:	30c2d2        	addi	a13, a2, 48
    2316:	2b61b2        	s32i	a11, a1, 172
    2319:	ffa0b2        	movi	a11, 255
    231c:	236152        	s32i	a5, a1, 140
    231f:	346172        	s32i	a7, a1, 208
    2322:	576142        	s32i	a4, a1, 0x15c
    2325:	2bda52        	addmi	a5, a10, 0x2b00
    2328:	30c172        	addi	a7, a1, 48
    232b:	4a61c2        	s32i	a12, a1, 0x128
    232e:	4661e2        	s32i	a14, a1, 0x118
    2331:	2961f2        	s32i	a15, a1, 164
    2334:	26da42        	addmi	a4, a10, 0x2600
    2337:	206162        	s32i	a6, a1, 128
    233a:	396182        	s32i	a8, a1, 228
    233d:	50c1c2        	addi	a12, a1, 80
    2340:	3d61d2        	s32i	a13, a1, 244
    2343:	54c1e2        	addi	a14, a1, 84
    2346:	04d2f2        	addmi	a15, a2, 0x400
    2349:	04d2d2        	addmi	a13, a2, 0x400
    234c:	58c182        	addi	a8, a1, 88
    234f:	7fdb32        	addmi	a3, a11, 0x7f00
    2352:	18a462        	movi	a6, 0x418
    2355:	2e6152        	s32i	a5, a1, 184
    2358:	4c6172        	s32i	a7, a1, 0x130
    235b:	266142        	s32i	a4, a1, 152
    235e:	27da52        	addmi	a5, a10, 0x2700
    2361:	24da72        	addmi	a7, a10, 0x2400
    2364:	3861c2        	s32i	a12, a1, 224
    2367:	6161e2        	s32i	a14, a1, 0x184
    236a:	6061f2        	s32i	a15, a1, 0x180
    236d:	98a2f2        	movi	a15, 0x298
    2370:	04d242        	addmi	a4, a2, 0x400
    2373:	29dac2        	addmi	a12, a10, 0x2900
    2376:	4861d2        	s32i	a13, a1, 0x120
    2379:	d26a           	add.n	a13, a2, a6
    237b:	04d2e2        	addmi	a14, a2, 0x400
    237e:	5e6182        	s32i	a8, a1, 0x178
    2381:	356132        	s32i	a3, a1, 212
    2384:	362182        	l32i	a8, a1, 216
    2387:	332162        	l32i	a6, a1, 204
    238a:	222132        	l32i	a3, a1, 136
    238d:	29da92        	addmi	a9, a10, 0x2900
    2390:	2a6152        	s32i	a5, a1, 168
    2393:	3a6172        	s32i	a7, a1, 232
    2396:	2d6142        	s32i	a4, a1, 180
    2399:	04d252        	addmi	a5, a2, 0x400
    239c:	5261c2        	s32i	a12, a1, 0x148
    239f:	4561e2        	s32i	a14, a1, 0x114
    23a2:	02d242        	addmi	a4, a2, 0x200
    23a5:	50c272        	addi	a7, a2, 80
    23a8:	02d2a2        	addmi	a10, a2, 0x200
    23ab:	2821c2        	l32i	a12, a1, 160
    23ae:	1c21e2        	l32i	a14, a1, 112
    23b1:	f2fa           	add.n	a15, a2, a15
    23b3:	5f6142        	s32i	a4, a1, 0x17c
    23b6:	3e6172        	s32i	a7, a1, 248
    23b9:	560124        	ae_sp24x2s.i	aep0, a1, 16
    23bc:	567134        	ae_sp24x2s.i	aep7, a1, 24
    23bf:	2c6152        	s32i	a5, a1, 176
    23c2:	5d61a2        	s32i	a10, a1, 0x174
    23c5:	670c           	movi.n	a7, 6
    23c7:	240c           	movi.n	a4, 2
    23c9:	2f61c2        	s32i	a12, a1, 188
    23cc:	5161d2        	s32i	a13, a1, 0x144
    23cf:	3261e2        	s32i	a14, a1, 200
    23d2:	5061f2        	s32i	a15, a1, 0x140
    23d5:	1e6192        	s32i	a9, a1, 120
    23d8:	003d06        	j	24d0 <Voice_EC_Prcs_common+0x618>

000023db <Voice_EC_Prcs_common+0x523>:
	...

000023dd <Voice_EC_Prcs_common+0x525>:
    23dd:	051787        	beq	a7, a8, 23e6 <Voice_EC_Prcs_common+0x52e>
    23e0:	540124        	ae_lp24x2.i	aep0, a1, 16
    23e3:	a10004        	ae_mulzaafq32sp16s.ll	aeq0, aeq0, aep0, aeq0, aep0
    23e6:	060934        	ae_roundsq32asym	aeq0, aeq0
    23e9:	368704        	ae_trunca32q48	a7, aeq0
    23ec:	150c           	movi.n	a5, 1
    23ee:	2b21d2        	l32i	a13, a1, 172
    23f1:	0155b0        	slli	a5, a5, 21
    23f4:	f73ee1        	l32r	a14, ec (7800 <immediate_num>)
    23f7:	537750        	max	a7, a7, a5
    23fa:	7d79           	s32i.n	a7, a13, 28
    23fc:	604d64        	ae_lq32f.i	aeq1, a13, 24
    23ff:	440e54        	ae_lp16x2f.i	aep0, a14, 20
    2402:	0c010418ff 	{ movi	a15, 0x418; ae_mulzaafq32sp16s.lh	aeq0, aeq1, aep0, aeq0, aep0 }
    2407:	060934        	ae_roundsq32asym	aeq0, aeq0
    240a:	6282f4        	ae_sq32f.x	aeq0, a2, a15
    240d:	5f2152        	l32i	a5, a1, 0x17c
    2410:	3b22a2        	l32i	a10, a2, 236
    2413:	5b22b2        	l32i	a11, a2, 0x16c
    2416:	6b22c2        	l32i	a12, a2, 0x1ac
    2419:	4b22d2        	l32i	a13, a2, 0x12c
    241c:	3c95f2        	l16si	a15, a5, 120
    241f:	f75761        	l32r	a6, 17c (1c00 <gain_after_ec>)
    2422:	0e0c           	movi.n	a14, 0
    2424:	0006e0        	callx8	a6
    2427:	3b22a2        	l32i	a10, a2, 236
    242a:	5b22b2        	l32i	a11, a2, 0x16c
    242d:	4b22c2        	l32i	a12, a2, 0x12c
    2430:	a122e2        	l32i	a14, a2, 0x284
    2433:	6321d2        	l32i	a13, a1, 0x18c
    2436:	f75a61        	l32r	a6, 1a0 (1ad4 <synth_3band>)
    2439:	0006e0        	callx8	a6
    243c:	a122a2        	l32i	a10, a2, 0x284
    243f:	6b22b2        	l32i	a11, a2, 0x1ac
    2442:	3e21d2        	l32i	a13, a1, 248
    2445:	5e21e2        	l32i	a14, a1, 0x178
    2448:	f75721        	l32r	a2, 1a4 (18f0 <tx_upsample_by2>)
    244b:	03cd           	mov.n	a12, a3
    244d:	0002e0        	callx8	a2
    2450:	5d2122        	l32i	a2, a1, 0x174
    2453:	8a0582        	l8ui	a8, a5, 138
    2456:	05a162        	movi	a6, 0x105
    2459:	449272        	l16si	a7, a2, 136
    245c:	048380        	extui	a8, a8, 3, 1
    245f:	4f1c           	movi.n	a15, 20
    2461:	609070        	neg	a9, a7
    2464:	839780        	moveqz	a9, a7, a8
    2467:	1189c0        	slli	a8, a9, 4
    246a:	441304        	ae_lp16x2f.i	aep1, a3, 0
    246d:	037d           	mov.n	a7, a3
    246f:	398884        	ae_movpa24x2	aep0, a8, a8
    2472:	1166c0        	slli	a6, a6, 4
    2475:	188f76        	loop	a15, 2491 <Voice_EC_Prcs_common+0x5d9>
    2478:	011054        	ae_mulfp24s.hl	aeq0, aep1, aep0
    247b:	011874        	ae_mulfp24s.ll	aeq1, aep1, aep0
    247e:	350214        	ae_slliq56	aeq0, aeq0, 8
    2481:	fb63fa924f 	{ ae_slliq56	aeq1, aeq1, 8; ae_roundsp16q48asym	aep7, aeq0 }
    2486:	f96bb2770f 	{ ae_sp16f.l.i	aep7, a7, 0; ae_roundsp16q48asym	aep2, aeq1 }
    248b:	422714        	ae_sp16f.l.i	aep2, a7, 2
    248e:	449714        	ae_lp16x2f.iu	aep1, a7, 4

00002491 <Voice_EC_Prcs_common+0x5d9>:
    2491:	f6e321        	l32r	a2, 20 (8688 <p_ecnr_mem>)
    2494:	6221c2        	l32i	a12, a1, 0x188
    2497:	8d2c           	movi.n	a13, 40
    2499:	0278           	l32i.n	a7, a2, 0
    249b:	f74321        	l32r	a2, 1a8 (19ec <deemph_filter_N>)
    249e:	03ad           	mov.n	a10, a3
    24a0:	b76a           	add.n	a11, a7, a6
    24a2:	0002e0        	callx8	a2
    24a5:	322122        	l32i	a2, a1, 200
    24a8:	50c332        	addi	a3, a3, 80
    24ab:	436132        	s32i	a3, a1, 0x10c
    24ae:	50c222        	addi	a2, a2, 80
    24b1:	326122        	s32i	a2, a1, 200

000024b4 <Voice_EC_Prcs_common+0x5fc>:
    24b4:	6f2172        	l32i	a7, a1, 0x1bc
    24b7:	332162        	l32i	a6, a1, 204
    24ba:	222132        	l32i	a3, a1, 136
    24bd:	1d2182        	l32i	a8, a1, 116
    24c0:	770b           	addi.n	a7, a7, -1
    24c2:	50c662        	addi	a6, a6, 80
    24c5:	50c332        	addi	a3, a3, 80
    24c8:	50c882        	addi	a8, a8, 80
    24cb:	17cc           	bnez.n	a7, 24d0 <Voice_EC_Prcs_common+0x618>
    24cd:	026446        	j	2e62 <Voice_EC_Prcs_common+0xfaa>
    24d0:	f6d421        	l32r	a2, 20 (8688 <p_ecnr_mem>)
    24d3:	0da4f2        	movi	a15, 0x40d
    24d6:	05a152        	movi	a5, 0x105
    24d9:	02e8           	l32i.n	a14, a2, 0
    24db:	112fe0        	slli	a2, a15, 2
    24de:	1155c0        	slli	a5, a5, 4
    24e1:	ce2a           	add.n	a12, a14, a2
    24e3:	f72121        	l32r	a2, 168 (19a4 <emph_filter_N>)
    24e6:	be5a           	add.n	a11, a14, a5
    24e8:	8d2c           	movi.n	a13, 40
    24ea:	06ad           	mov.n	a10, a6
    24ec:	1d6182        	s32i	a8, a1, 116
    24ef:	6f6172        	s32i	a7, a1, 0x1bc
    24f2:	0002e0        	callx8	a2
    24f5:	f6ca71        	l32r	a7, 20 (8688 <p_ecnr_mem>)
    24f8:	07a2f2        	movi	a15, 0x207
    24fb:	8d2c           	movi.n	a13, 40
    24fd:	0778           	l32i.n	a7, a7, 0
    24ff:	03ad           	mov.n	a10, a3
    2501:	b75a           	add.n	a11, a7, a5
    2503:	115fd0        	slli	a5, a15, 3
    2506:	c75a           	add.n	a12, a7, a5
    2508:	0002e0        	callx8	a2
    250b:	f6c551        	l32r	a5, 20 (8688 <p_ecnr_mem>)
    250e:	f71721        	l32r	a2, 16c (1838 <tx_downsample_by2>)
    2511:	40c1e2        	addi	a14, a1, 64
    2514:	0578           	l32i.n	a7, a5, 0
    2516:	06cd           	mov.n	a12, a6
    2518:	12d782        	addmi	a8, a7, 0x1200
    251b:	1728a2        	l32i	a10, a8, 92
    251e:	1828b2        	l32i	a11, a8, 96
    2521:	24d7d2        	addmi	a13, a7, 0x2400
    2524:	0002e0        	callx8	a2
    2527:	0578           	l32i.n	a7, a5, 0
    2529:	21a1f2        	movi	a15, 0x121
    252c:	029d           	mov.n	a9, a2
    252e:	12d782        	addmi	a8, a7, 0x1200
    2531:	052d           	mov.n	a2, a5
    2533:	115fb0        	slli	a5, a15, 5
    2536:	1b28a2        	l32i	a10, a8, 108
    2539:	1c28b2        	l32i	a11, a8, 112
    253c:	6a21e2        	l32i	a14, a1, 0x1a8
    253f:	d75a           	add.n	a13, a7, a5
    2541:	03cd           	mov.n	a12, a3
    2543:	226132        	s32i	a3, a1, 136
    2546:	0009e0        	callx8	a9
    2549:	0278           	l32i.n	a7, a2, 0
    254b:	342152        	l32i	a5, a1, 208
    254e:	1e2132        	l32i	a3, a1, 120
    2551:	5ba1f2        	movi	a15, 0x15b
    2554:	12d792        	addmi	a9, a7, 0x1200
    2557:	112fb0        	slli	a2, a15, 5
    255a:	b72a           	add.n	a11, a7, a2
    255c:	3f25d2        	l32i	a13, a5, 252
    255f:	2f23e2        	l32i	a14, a3, 188
    2562:	1729a2        	l32i	a10, a9, 92
    2565:	1629c2        	l32i	a12, a9, 88
    2568:	f70221        	l32r	a2, 170 (1a30 <analy_3band>)
    256b:	0002e0        	callx8	a2
    256e:	f6ac21        	l32r	a2, 20 (8688 <p_ecnr_mem>)
    2571:	25a132        	movi	a3, 0x125
    2574:	1133b0        	slli	a3, a3, 5
    2577:	0278           	l32i.n	a7, a2, 0
    2579:	422122        	l32i	a2, a1, 0x108
    257c:	ae0c           	movi.n	a14, 10
    257e:	12d782        	addmi	a8, a7, 0x1200
    2581:	b73a           	add.n	a11, a7, a3
    2583:	1f22d2        	l32i	a13, a2, 124
    2586:	1628a2        	l32i	a10, a8, 88
    2589:	f6f131        	l32r	a3, 150 (1b60 <biquad_2cascade_24bitTx>)
    258c:	10d7c2        	addmi	a12, a7, 0x1000
    258f:	0003e0        	callx8	a3
    2592:	f6a331        	l32r	a3, 20 (8688 <p_ecnr_mem>)
    2595:	1e21e2        	l32i	a14, a1, 120
    2598:	5da1f2        	movi	a15, 0x15d
    259b:	0378           	l32i.n	a7, a3, 0
    259d:	4025d2        	l32i	a13, a5, 0x100
    25a0:	115fb0        	slli	a5, a15, 5
    25a3:	12d792        	addmi	a9, a7, 0x1200
    25a6:	b75a           	add.n	a11, a7, a5
    25a8:	302ee2        	l32i	a14, a14, 192
    25ab:	1b29a2        	l32i	a10, a9, 108
    25ae:	1629c2        	l32i	a12, a9, 88
    25b1:	f6ef51        	l32r	a5, 170 (1a30 <analy_3band>)
    25b4:	0005e0        	callx8	a5
    25b7:	0378           	l32i.n	a7, a3, 0
    25b9:	202282        	l32i	a8, a2, 128
    25bc:	550c           	movi.n	a5, 5
    25be:	12d772        	addmi	a7, a7, 0x1200
    25c1:	162792        	l32i	a9, a7, 88
    25c4:	fcc872        	addi	a7, a8, -4
    25c7:	f8c982        	addi	a8, a9, -8
    25ca:	088576        	loop	a5, 25d6 <Voice_EC_Prcs_common+0x71e>
    25cd:	5c8814        	ae_lp24x2f.iu	aep0, a8, 8
    25d0:	090184        	ae_roundsp16asym	aep0, aep0
    25d3:	468714        	ae_sp16x2f.iu	aep0, a7, 4

000025d6 <Voice_EC_Prcs_common+0x71e>:
    25d6:	336162        	s32i	a6, a1, 204
    25d9:	232172        	l32i	a7, a1, 140
    25dc:	312162        	l32i	a6, a1, 196
    25df:	6e21b2        	l32i	a11, a1, 0x1b8
    25e2:	f6e451        	l32r	a5, 174 (d84 <subband_aec_block>)
    25e5:	ac0c           	movi.n	a12, 10
    25e7:	1e0c           	movi.n	a14, 1
    25e9:	07ad           	mov.n	a10, a7
    25eb:	06dd           	mov.n	a13, a6
    25ed:	0005e0        	callx8	a5
    25f0:	6c21b2        	l32i	a11, a1, 0x1b0
    25f3:	ac0c           	movi.n	a12, 10
    25f5:	3e0c           	movi.n	a14, 3
    25f7:	07ad           	mov.n	a10, a7
    25f9:	06dd           	mov.n	a13, a6
    25fb:	0005e0        	callx8	a5
    25fe:	6b21b2        	l32i	a11, a1, 0x1ac
    2601:	4c1c           	movi.n	a12, 20
    2603:	4e0c           	movi.n	a14, 4
    2605:	07ad           	mov.n	a10, a7
    2607:	06dd           	mov.n	a13, a6
    2609:	0005e0        	callx8	a5
    260c:	6d21b2        	l32i	a11, a1, 0x1b4
    260f:	f6da51        	l32r	a5, 178 (1060 <subband_aec_mid_channel>)
    2612:	2d0c           	movi.n	a13, 2
    2614:	07ad           	mov.n	a10, a7
    2616:	06cd           	mov.n	a12, a6
    2618:	0005e0        	callx8	a5
    261b:	272152        	l32i	a5, a1, 156
    261e:	342132        	l32i	a3, a1, 208
    2621:	2122a2        	l32i	a10, a2, 132
    2624:	1125c2        	l32i	a12, a5, 68
    2627:	1e2152        	l32i	a5, a1, 120
    262a:	4123b2        	l32i	a11, a3, 0x104
    262d:	f6d371        	l32r	a7, 17c (1c00 <gain_after_ec>)
    2630:	3125d2        	l32i	a13, a5, 196
    2633:	0e0c           	movi.n	a14, 0
    2635:	0f0c           	movi.n	a15, 0
    2637:	0007e0        	callx8	a7
    263a:	4121f2        	l32i	a15, a1, 0x104
    263d:	027d           	mov.n	a7, a2
    263f:	61a122        	movi	a2, 0x161
    2642:	1a8f16        	beqz	a15, 27ee <Voice_EC_Prcs_common+0x936>
    2645:	f67621        	l32r	a2, 20 (8688 <p_ecnr_mem>)
    2648:	212152        	l32i	a5, a1, 132
    264b:	0fa4f2        	movi	a15, 0x40f
    264e:	0278           	l32i.n	a7, a2, 0
    2650:	05a132        	movi	a3, 0x105
    2653:	112fe0        	slli	a2, a15, 2
    2656:	1133c0        	slli	a3, a3, 4
    2659:	c72a           	add.n	a12, a7, a2
    265b:	f6c321        	l32r	a2, 168 (19a4 <emph_filter_N>)
    265e:	b73a           	add.n	a11, a7, a3
    2660:	8d2c           	movi.n	a13, 40
    2662:	05ad           	mov.n	a10, a5
    2664:	0002e0        	callx8	a2
    2667:	f66e21        	l32r	a2, 20 (8688 <p_ecnr_mem>)
    266a:	91a032        	movi	a3, 145
    266d:	1133a0        	slli	a3, a3, 6
    2670:	0278           	l32i.n	a7, a2, 0
    2672:	5821e2        	l32i	a14, a1, 0x160
    2675:	f6bd21        	l32r	a2, 16c (1838 <tx_downsample_by2>)
    2678:	12d782        	addmi	a8, a7, 0x1200
    267b:	1928a2        	l32i	a10, a8, 100
    267e:	1a28b2        	l32i	a11, a8, 104
    2681:	d73a           	add.n	a13, a7, a3
    2683:	05cd           	mov.n	a12, a5
    2685:	216152        	s32i	a5, a1, 132
    2688:	0002e0        	callx8	a2
    268b:	f66521        	l32r	a2, 20 (8688 <p_ecnr_mem>)
    268e:	402132        	l32i	a3, a1, 0x100
    2691:	3f2152        	l32i	a5, a1, 252
    2694:	0278           	l32i.n	a7, a2, 0
    2696:	5fa1f2        	movi	a15, 0x15f
    2699:	2f23e2        	l32i	a14, a3, 188
    269c:	12d792        	addmi	a9, a7, 0x1200
    269f:	113fb0        	slli	a3, a15, 5
    26a2:	b73a           	add.n	a11, a7, a3
    26a4:	3f25d2        	l32i	a13, a5, 252
    26a7:	1929a2        	l32i	a10, a9, 100
    26aa:	1629c2        	l32i	a12, a9, 88
    26ad:	f6b031        	l32r	a3, 170 (1a30 <analy_3band>)
    26b0:	0003e0        	callx8	a3
    26b3:	0278           	l32i.n	a7, a2, 0
    26b5:	2721f2        	l32i	a15, a1, 156
    26b8:	97a432        	movi	a3, 0x497
    26bb:	1133d0        	slli	a3, a3, 3
    26be:	1f2fd2        	l32i	a13, a15, 124
    26c1:	b73a           	add.n	a11, a7, a3
    26c3:	10d7c2        	addmi	a12, a7, 0x1000
    26c6:	962ca2        	l32i	a10, a12, 0x258
    26c9:	f6a171        	l32r	a7, 150 (1b60 <biquad_2cascade_24bitTx>)
    26cc:	ae0c           	movi.n	a14, 10
    26ce:	0007e0        	callx8	a7
    26d1:	232172        	l32i	a7, a1, 140
    26d4:	5a21b2        	l32i	a11, a1, 0x168
    26d7:	f6a731        	l32r	a3, 174 (d84 <subband_aec_block>)
    26da:	ac0c           	movi.n	a12, 10
    26dc:	1e0c           	movi.n	a14, 1
    26de:	07ad           	mov.n	a10, a7
    26e0:	06dd           	mov.n	a13, a6
    26e2:	0003e0        	callx8	a3
    26e5:	5c21b2        	l32i	a11, a1, 0x170
    26e8:	ac0c           	movi.n	a12, 10
    26ea:	3e0c           	movi.n	a14, 3
    26ec:	07ad           	mov.n	a10, a7
    26ee:	06dd           	mov.n	a13, a6
    26f0:	0003e0        	callx8	a3
    26f3:	5921b2        	l32i	a11, a1, 0x164
    26f6:	4c1c           	movi.n	a12, 20
    26f8:	4e0c           	movi.n	a14, 4
    26fa:	07ad           	mov.n	a10, a7
    26fc:	06dd           	mov.n	a13, a6
    26fe:	0003e0        	callx8	a3
    2701:	5b21b2        	l32i	a11, a1, 0x16c
    2704:	f69d31        	l32r	a3, 178 (1060 <subband_aec_mid_channel>)
    2707:	2d0c           	movi.n	a13, 2
    2709:	07ad           	mov.n	a10, a7
    270b:	402122        	l32i	a2, a1, 0x100
    270e:	06cd           	mov.n	a12, a6
    2710:	0003e0        	callx8	a3
    2713:	572162        	l32i	a6, a1, 0x15c
    2716:	2e2132        	l32i	a3, a1, 184
    2719:	4125b2        	l32i	a11, a5, 0x104
    271c:	2126a2        	l32i	a10, a6, 132
    271f:	1123c2        	l32i	a12, a3, 68
    2722:	3122d2        	l32i	a13, a2, 196
    2725:	f69571        	l32r	a7, 17c (1c00 <gain_after_ec>)
    2728:	1e0c           	movi.n	a14, 1
    272a:	0f0c           	movi.n	a15, 0
    272c:	0007e0        	callx8	a7
    272f:	f63c51        	l32r	a5, 20 (8688 <p_ecnr_mem>)
    2732:	20c1b2        	addi	a11, a1, 32
    2735:	c1cb           	addi.n	a12, a1, 12
    2737:	0578           	l32i.n	a7, a5, 0
    2739:	3f21e2        	l32i	a14, a1, 252
    273c:	9ca0f2        	movi	a15, 156
    273f:	12d782        	addmi	a8, a7, 0x1200
    2742:	550882        	l8ui	a8, a8, 85
    2745:	12d752        	addmi	a5, a7, 0x1200
    2748:	2126a2        	l32i	a10, a6, 132
    274b:	026807        	bbci	a8, 0, 2751 <Voice_EC_Prcs_common+0x899>
    274e:	003886        	j	2834 <Voice_EC_Prcs_common+0x97c>
    2751:	3122c2        	l32i	a12, a2, 196
    2754:	2cdf22        	addmi	a2, a15, 0x2c00
    2757:	412eb2        	l32i	a11, a14, 0x104
    275a:	d72a           	add.n	a13, a7, a2
    275c:	1d25e2        	l32i	a14, a5, 116
    275f:	f69021        	l32r	a2, 1a0 (1ad4 <synth_3band>)
    2762:	0002e0        	callx8	a2
    2765:	f62e21        	l32r	a2, 20 (8688 <p_ecnr_mem>)
    2768:	2f2152        	l32i	a5, a1, 188
    276b:	9f4c           	movi.n	a15, 73
    276d:	0278           	l32i.n	a7, a2, 0
    276f:	1123b2        	l32i	a11, a3, 68
    2772:	113f90        	slli	a3, a15, 7
    2775:	12d782        	addmi	a8, a7, 0x1200
    2778:	1d28a2        	l32i	a10, a8, 116
    277b:	3821e2        	l32i	a14, a1, 224
    277e:	f68961        	l32r	a6, 1a4 (18f0 <tx_upsample_by2>)
    2781:	d73a           	add.n	a13, a7, a3
    2783:	05cd           	mov.n	a12, a5
    2785:	0006e0        	callx8	a6
    2788:	0278           	l32i.n	a7, a2, 0
    278a:	05a132        	movi	a3, 0x105
    278d:	1133c0        	slli	a3, a3, 4
    2790:	09a2f2        	movi	a15, 0x209
    2793:	b73a           	add.n	a11, a7, a3
    2795:	113fd0        	slli	a3, a15, 3
    2798:	c73a           	add.n	a12, a7, a3
    279a:	f68331        	l32r	a3, 1a8 (19ec <deemph_filter_N>)
    279d:	8d2c           	movi.n	a13, 40
    279f:	05ad           	mov.n	a10, a5
    27a1:	0003e0        	callx8	a3
    27a4:	422132        	l32i	a3, a1, 0x108
    27a7:	0278           	l32i.n	a7, a2, 0
    27a9:	3421d2        	l32i	a13, a1, 208
    27ac:	1e21e2        	l32i	a14, a1, 120
    27af:	61a1f2        	movi	a15, 0x161
    27b2:	2123a2        	l32i	a10, a3, 132
    27b5:	12d782        	addmi	a8, a7, 0x1200
    27b8:	113fb0        	slli	a3, a15, 5
    27bb:	412db2        	l32i	a11, a13, 0x104
    27be:	312ec2        	l32i	a12, a14, 196
    27c1:	d73a           	add.n	a13, a7, a3
    27c3:	1e28e2        	l32i	a14, a8, 120
    27c6:	f67631        	l32r	a3, 1a0 (1ad4 <synth_3band>)
    27c9:	0003e0        	callx8	a3
    27cc:	0278           	l32i.n	a7, a2, 0
    27ce:	2721e2        	l32i	a14, a1, 156
    27d1:	23a1f2        	movi	a15, 0x123
    27d4:	12d782        	addmi	a8, a7, 0x1200
    27d7:	112eb2        	l32i	a11, a14, 68
    27da:	112fb0        	slli	a2, a15, 5
    27dd:	1e28a2        	l32i	a10, a8, 120
    27e0:	1d21c2        	l32i	a12, a1, 116
    27e3:	3021e2        	l32i	a14, a1, 192
    27e6:	d72a           	add.n	a13, a7, a2
    27e8:	0006e0        	callx8	a6
    27eb:	00f1c6        	j	2bb6 <Voice_EC_Prcs_common+0xcfe>

000027ee <Voice_EC_Prcs_common+0x936>:
    27ee:	f60c61        	l32r	a6, 20 (8688 <p_ecnr_mem>)
    27f1:	1122b0        	slli	a2, a2, 5
    27f4:	2127a2        	l32i	a10, a7, 132
    27f7:	06f8           	l32i.n	a15, a6, 0
    27f9:	4123b2        	l32i	a11, a3, 0x104
    27fc:	3125c2        	l32i	a12, a5, 196
    27ff:	12df82        	addmi	a8, a15, 0x1200
    2802:	df2a           	add.n	a13, a15, a2
    2804:	1e28e2        	l32i	a14, a8, 120
    2807:	f66621        	l32r	a2, 1a0 (1ad4 <synth_3band>)
    280a:	0002e0        	callx8	a2
    280d:	0678           	l32i.n	a7, a6, 0
    280f:	272122        	l32i	a2, a1, 156
    2812:	1d2152        	l32i	a5, a1, 116
    2815:	23a1f2        	movi	a15, 0x123
    2818:	1122b2        	l32i	a11, a2, 68
    281b:	12d782        	addmi	a8, a7, 0x1200
    281e:	112fb0        	slli	a2, a15, 5
    2821:	d72a           	add.n	a13, a7, a2
    2823:	1e28a2        	l32i	a10, a8, 120
    2826:	3021e2        	l32i	a14, a1, 192
    2829:	f65e21        	l32r	a2, 1a4 (18f0 <tx_upsample_by2>)
    282c:	05cd           	mov.n	a12, a5
    282e:	0002e0        	callx8	a2
    2831:	00e586        	j	2bcb <Voice_EC_Prcs_common+0xd13>

00002834 <Voice_EC_Prcs_common+0x97c>:
    2834:	1f2122        	l32i	a2, a1, 124
    2837:	f65231        	l32r	a3, 180 (5590 <prelim_filter>)
    283a:	02ad           	mov.n	a10, a2
    283c:	0003e0        	callx8	a3
    283f:	4f21b2        	l32i	a11, a1, 0x13c
    2842:	4d21d2        	l32i	a13, a1, 0x134
    2845:	f64f31        	l32r	a3, 184 (5814 <prelim_filter3>)
    2848:	3c0c           	movi.n	a12, 3
    284a:	02ad           	mov.n	a10, a2
    284c:	0003e0        	callx8	a3
    284f:	4e21b2        	l32i	a11, a1, 0x138
    2852:	4c21d2        	l32i	a13, a1, 0x130
    2855:	4c0c           	movi.n	a12, 4
    2857:	02ad           	mov.n	a10, a2
    2859:	0003e0        	callx8	a3
    285c:	372132        	l32i	a3, a1, 220
    285f:	8188           	l32i.n	a8, a1, 32
    2861:	a198           	l32i.n	a9, a1, 40
    2863:	1e1372        	l16ui	a7, a3, 60
    2866:	c168           	l32i.n	a6, a1, 48
    2868:	300774        	ae_cvtp24a16x2.ll	aep0, a7, a7
    286b:	4b2172        	l32i	a7, a1, 0x12c
    286e:	4a21b2        	l32i	a11, a1, 0x128
    2871:	360834        	ae_cvtq48a32s	aeq0, a8
    2874:	10003ec97f 	{ ae_cvtq48a32s	aeq1, a9; ae_mulzaafq32sp16s.ll	aeq0, aeq0, aep0, aeq0, aep0 }
    2879:	1001fec6bf 	{ ae_cvtq48a32s	aeq2, a6; ae_mulzaafq32sp16s.ll	aeq1, aeq1, aep0, aeq1, aep0 }
    287e:	13026047ae 	{ l16si	a10, a7, 8; ae_mulzaafq32sp16s.ll	aeq3, aeq2, aep0, aeq2, aep0 }
    2883:	f863e7ebce 	{ l16si	a12, a11, 252; ae_roundsp16q48asym	aep1, aeq0 }
    2888:	fb6bbf1d5f 	{ ae_cvta32p24.l	a13, aep1; ae_roundsp16q48asym	aep6, aeq1 }
    288d:	fb7bff6e5f 	{ ae_cvta32p24.l	a14, aep6; ae_roundsp16q48asym	aep7, aeq3 }
    2892:	007fd4        	ae_cvta32p24.l	a15, aep7
    2895:	81d9           	s32i.n	a13, a1, 32
    2897:	a1e9           	s32i.n	a14, a1, 40
    2899:	c1f9           	s32i.n	a15, a1, 48
    289b:	2baac7        	bge	a10, a12, 28ca <Voice_EC_Prcs_common+0xa12>
    289e:	20c172        	addi	a7, a1, 32
    28a1:	4721b2        	l32i	a11, a1, 0x11c
    28a4:	f63931        	l32r	a3, 188 (5ae0 <cali_power>)
    28a7:	02ad           	mov.n	a10, a2
    28a9:	07cd           	mov.n	a12, a7
    28ab:	0003e0        	callx8	a3
    28ae:	f63731        	l32r	a3, 18c (595c <cali_module_short>)
    28b1:	02ad           	mov.n	a10, a2
    28b3:	07bd           	mov.n	a11, a7
    28b5:	0003e0        	callx8	a3
    28b8:	292162        	l32i	a6, a1, 164
    28bb:	242132        	l32i	a3, a1, 144
    28be:	5d1692        	l16ui	a9, a6, 186
    28c1:	5e1682        	l16ui	a8, a6, 188
    28c4:	5b1372        	l16ui	a7, a3, 182
    28c7:	000746        	j	28e8 <Voice_EC_Prcs_common+0xa30>

000028ca <Voice_EC_Prcs_common+0xa12>:
    28ca:	242132        	l32i	a3, a1, 144
    28cd:	4621f2        	l32i	a15, a1, 0x118
    28d0:	292162        	l32i	a6, a1, 164
    28d3:	d77c           	movi.n	a7, -3
    28d5:	080c           	movi.n	a8, 0
    28d7:	f47070        	extui	a7, a7, 0, 16
    28da:	090c           	movi.n	a9, 0
    28dc:	5b5372        	s16i	a7, a3, 182
    28df:	5f5f82        	s16i	a8, a15, 190
    28e2:	5d5682        	s16i	a8, a6, 186
    28e5:	5e5682        	s16i	a8, a6, 188
    28e8:	262152        	l32i	a5, a1, 152
    28eb:	032c           	movi.n	a3, 32
    28ed:	899d           	ae_sext16	a9, a9
    28ef:	4215a2        	l16ui	a10, a5, 132
    28f2:	888d           	ae_sext16	a8, a8
    28f4:	87dd           	ae_sext16	a13, a7
    28f6:	63fa30        	minu	a15, a10, a3
    28f9:	c073f0        	sub	a7, a3, a15
    28fc:	829f90        	mull	a9, a15, a9
    28ff:	827780        	mull	a7, a7, a8
    2902:	2521b2        	l32i	a11, a1, 148
    2905:	779a           	add.n	a7, a7, a9
    2907:	10c772        	addi	a7, a7, 16
    290a:	2225c2        	l32i	a12, a5, 136
    290d:	f62031        	l32r	a3, 190 (5ec0 <beam1_proc>)
    2910:	417570        	srli	a7, a7, 5
    2913:	0e0c           	movi.n	a14, 0
    2915:	1f0c           	movi.n	a15, 1
    2917:	02ad           	mov.n	a10, a2
    2919:	5e5672        	s16i	a7, a6, 188
    291c:	0003e0        	callx8	a3
    291f:	2a2172        	l32i	a7, a1, 168
    2922:	5d96d2        	l16si	a13, a6, 186
    2925:	3c21b2        	l32i	a11, a1, 240
    2928:	77c8           	l32i.n	a12, a7, 28
    292a:	0e0c           	movi.n	a14, 0
    292c:	0f0c           	movi.n	a15, 0
    292e:	02ad           	mov.n	a10, a2
    2930:	0003e0        	callx8	a3
    2933:	202132        	l32i	a3, a1, 128
    2936:	3b2152        	l32i	a5, a1, 236
    2939:	5e96d2        	l16si	a13, a6, 188
    293c:	2a23c2        	l32i	a12, a3, 168
    293f:	f61481        	l32r	a8, 190 (5ec0 <beam1_proc>)
    2942:	0e0c           	movi.n	a14, 0
    2944:	0f0c           	movi.n	a15, 0
    2946:	02ad           	mov.n	a10, a2
    2948:	05bd           	mov.n	a11, a5
    294a:	0008e0        	callx8	a8
    294d:	02ad           	mov.n	a10, a2
    294f:	2a23c2        	l32i	a12, a3, 168
    2952:	5e96d2        	l16si	a13, a6, 188
    2955:	f60e21        	l32r	a2, 190 (5ec0 <beam1_proc>)
    2958:	ae0c           	movi.n	a14, 10
    295a:	0f0c           	movi.n	a15, 0
    295c:	05bd           	mov.n	a11, a5
    295e:	0002e0        	callx8	a2
    2961:	6778           	l32i.n	a7, a7, 24
    2963:	3a2122        	l32i	a2, a1, 232
    2966:	05a0f2        	movi	a15, 5
    2969:	fcc772        	addi	a7, a7, -4
    296c:	781282        	l16ui	a8, a2, 240
    296f:	300884        	ae_cvtp24a16x2.ll	aep0, a8, a8
    2972:	449714        	ae_lp16x2f.iu	aep1, a7, 4
    2975:	178f76        	loop	a15, 2990 <Voice_EC_Prcs_common+0xad8>
    2978:	011054        	ae_mulfp24s.hl	aeq0, aep1, aep0
    297b:	ea05ed971f 	{ ae_lp16x2f.iu	aep1, a7, 4; ae_mulfp24s.ll	aeq1, aep1, aep0 }
    2980:	350094        	ae_slliq56	aeq0, aeq0, 2
    2983:	f963ba906f 	{ ae_slliq56	aeq1, aeq1, 2; ae_roundsp16q48asym	aep2, aeq0 }
    2988:	f96bf227ef 	{ ae_sp16f.l.i	aep2, a7, -4; ae_roundsp16q48asym	aep3, aeq1 }
    298d:	4237f4        	ae_sp16f.l.i	aep3, a7, -2

00002990 <Voice_EC_Prcs_common+0xad8>:
    2990:	202122        	l32i	a2, a1, 128
    2993:	3a21e2        	l32i	a14, a1, 232
    2996:	af0c           	movi.n	a15, 10
    2998:	292272        	l32i	a7, a2, 164
    299b:	791e82        	l16ui	a8, a14, 242
    299e:	300884        	ae_cvtp24a16x2.ll	aep0, a8, a8
    29a1:	fcc772        	addi	a7, a7, -4
    29a4:	449714        	ae_lp16x2f.iu	aep1, a7, 4
    29a7:	178f76        	loop	a15, 29c2 <Voice_EC_Prcs_common+0xb0a>
    29aa:	011054        	ae_mulfp24s.hl	aeq0, aep1, aep0
    29ad:	ea05ed971f 	{ ae_lp16x2f.iu	aep1, a7, 4; ae_mulfp24s.ll	aeq1, aep1, aep0 }
    29b2:	350094        	ae_slliq56	aeq0, aeq0, 2
    29b5:	f963ba906f 	{ ae_slliq56	aeq1, aeq1, 2; ae_roundsp16q48asym	aep2, aeq0 }
    29ba:	f96bf227ef 	{ ae_sp16f.l.i	aep2, a7, -4; ae_roundsp16q48asym	aep3, aeq1 }
    29bf:	4237f4        	ae_sp16f.l.i	aep3, a7, -2
    29c2:	f59721        	l32r	a2, 20 (8688 <p_ecnr_mem>)
    29c5:	372152        	l32i	a5, a1, 220
    29c8:	262132        	l32i	a3, a1, 152
    29cb:	0278           	l32i.n	a7, a2, 0
    29cd:	ac0c           	movi.n	a12, 10
    29cf:	2421f2        	l32i	a15, a1, 144
    29d2:	2fd782        	addmi	a8, a7, 0x2f00
    29d5:	2b1882        	l16ui	a8, a8, 86
    29d8:	12d792        	addmi	a9, a7, 0x1200
    29db:	1f21a2        	l32i	a10, a1, 124
    29de:	07d816        	beqz	a8, 2a5f <Voice_EC_Prcs_common+0xba7>
    29e1:	262162        	l32i	a6, a1, 152
    29e4:	f58e81        	l32r	a8, 1c (6c04 <xt_memcpy>)
    29e7:	1825a2        	l32i	a10, a5, 96
    29ea:	2226b2        	l32i	a11, a6, 136
    29ed:	0008e0        	callx8	a8
    29f0:	0288           	l32i.n	a8, a2, 0
    29f2:	2421f2        	l32i	a15, a1, 144
    29f5:	2226c2        	l32i	a12, a6, 136
    29f8:	12d882        	addmi	a8, a8, 0x1200
    29fb:	5b9fd2        	l16si	a13, a15, 182
    29fe:	0691f2        	l16si	a15, a1, 12
    2a01:	1f21a2        	l32i	a10, a1, 124
    2a04:	2521b2        	l32i	a11, a1, 148
    2a07:	f5e331        	l32r	a3, 194 (6028 <beam2_proc>)
    2a0a:	8e1c           	movi.n	a14, 24
    2a0c:	279892        	l16si	a9, a8, 78
    2a0f:	020c           	movi.n	a2, 0
    2a11:	1129           	s32i.n	a2, a1, 4
    2a13:	0199           	s32i.n	a9, a1, 0
    2a15:	0003e0        	callx8	a3
    2a18:	2fd782        	addmi	a8, a7, 0x2f00
    2a1b:	2a18a2        	l16ui	a10, a8, 84
    2a1e:	222692        	l32i	a9, a6, 136
    2a21:	fc7c           	movi.n	a12, -1
    2a23:	8aed           	ae_sext16	a14, a10
    2a25:	01cc10        	slli	a12, a12, 31
    2a28:	f5c1f1        	l32r	a15, 12c (7fff0000 <_end+0x7ffe7974>)
    2a2b:	182572        	l32i	a7, a5, 96
    2a2e:	fec982        	addi	a8, a9, -2
    2a31:	11be00        	slli	a11, a14, 16
    2a34:	309ec0        	xor	a9, a14, a12
    2a37:	c099b0        	sub	a9, a9, a11
    2a3a:	292162        	l32i	a6, a1, 164
    2a3d:	839fa0        	moveqz	a9, a15, a10
    2a40:	a30c           	movi.n	a3, 10
    2a42:	409704        	ae_lp16f.iu	aep1, a7, 0
    2a45:	308994        	ae_cvtp24a16x2.lh	aep0, a9, a9
    2a48:	108376        	loop	a3, 2a5c <Voice_EC_Prcs_common+0xba4>
    2a4b:	e820acf81f 	{ ae_lp16f.iu	aep7, a8, 2; ae_mulfp24s.hh	aeq0, aep0, aep1 }
    2a50:	010734        	ae_mulafp24s.ll	aeq0, aep0, aep7
    2a53:	1b8f14        	ae_roundsp16q48asym	aep1, aeq0
    2a56:	421804        	ae_sp16f.l.i	aep1, a8, 0
    2a59:	409714        	ae_lp16f.iu	aep1, a7, 2
    2a5c:	000706        	j	2a7c <Voice_EC_Prcs_common+0xbc4>

00002a5f <Voice_EC_Prcs_common+0xba7>:
    2a5f:	2223c2        	l32i	a12, a3, 136
    2a62:	5b9fd2        	l16si	a13, a15, 182
    2a65:	0691f2        	l16si	a15, a1, 12
    2a68:	2521b2        	l32i	a11, a1, 148
    2a6b:	f5ca31        	l32r	a3, 194 (6028 <beam2_proc>)
    2a6e:	8e1c           	movi.n	a14, 24
    2a70:	279972        	l16si	a7, a9, 78
    2a73:	020c           	movi.n	a2, 0
    2a75:	1129           	s32i.n	a2, a1, 4
    2a77:	0179           	s32i.n	a7, a1, 0
    2a79:	0003e0        	callx8	a3
    2a7c:	2a2132        	l32i	a3, a1, 168
    2a7f:	1f2172        	l32i	a7, a1, 124
    2a82:	ffa0f2        	movi	a15, 255
    2a85:	73c8           	l32i.n	a12, a3, 28
    2a87:	5d96d2        	l16si	a13, a6, 186
    2a8a:	3c21b2        	l32i	a11, a1, 240
    2a8d:	f5c131        	l32r	a3, 194 (6028 <beam2_proc>)
    2a90:	8e1c           	movi.n	a14, 24
    2a92:	07ad           	mov.n	a10, a7
    2a94:	7fdff2        	addmi	a15, a15, 0x7f00
    2a97:	091c           	movi.n	a9, 16
    2a99:	1129           	s32i.n	a2, a1, 4
    2a9b:	065d           	mov.n	a5, a6
    2a9d:	0199           	s32i.n	a9, a1, 0
    2a9f:	0003e0        	callx8	a3
    2aa2:	202162        	l32i	a6, a1, 128
    2aa5:	3b2132        	l32i	a3, a1, 236
    2aa8:	ffa0f2        	movi	a15, 255
    2aab:	081c           	movi.n	a8, 16
    2aad:	2a26c2        	l32i	a12, a6, 168
    2ab0:	5e95d2        	l16si	a13, a5, 188
    2ab3:	1129           	s32i.n	a2, a1, 4
    2ab5:	0189           	s32i.n	a8, a1, 0
    2ab7:	072d           	mov.n	a2, a7
    2ab9:	07ad           	mov.n	a10, a7
    2abb:	7fdf72        	addmi	a7, a15, 0x7f00
    2abe:	f5b581        	l32r	a8, 194 (6028 <beam2_proc>)
    2ac1:	9e1c           	movi.n	a14, 25
    2ac3:	03bd           	mov.n	a11, a3
    2ac5:	07fd           	mov.n	a15, a7
    2ac7:	0008e0        	callx8	a8
    2aca:	03bd           	mov.n	a11, a3
    2acc:	2a26c2        	l32i	a12, a6, 168
    2acf:	5e95d2        	l16si	a13, a5, 188
    2ad2:	f5b031        	l32r	a3, 194 (6028 <beam2_proc>)
    2ad5:	a80c           	movi.n	a8, 10
    2ad7:	9e1c           	movi.n	a14, 25
    2ad9:	02ad           	mov.n	a10, a2
    2adb:	07fd           	mov.n	a15, a7
    2add:	091c           	movi.n	a9, 16
    2adf:	1189           	s32i.n	a8, a1, 4
    2ae1:	0199           	s32i.n	a9, a1, 0
    2ae3:	0003e0        	callx8	a3
    2ae6:	f5ac31        	l32r	a3, 198 (6300 <beam1_mid_band>)
    2ae9:	02ad           	mov.n	a10, a2
    2aeb:	0003e0        	callx8	a3
    2aee:	392132        	l32i	a3, a1, 228
    2af1:	4921e2        	l32i	a14, a1, 0x124
    2af4:	af0c           	movi.n	a15, 10
    2af6:	0c2372        	l32i	a7, a3, 48
    2af9:	771e82        	l16ui	a8, a14, 238
    2afc:	300884        	ae_cvtp24a16x2.ll	aep0, a8, a8
    2aff:	fcc772        	addi	a7, a7, -4
    2b02:	449714        	ae_lp16x2f.iu	aep1, a7, 4
    2b05:	178f76        	loop	a15, 2b20 <Voice_EC_Prcs_common+0xc68>
    2b08:	011054        	ae_mulfp24s.hl	aeq0, aep1, aep0
    2b0b:	ea05ed971f 	{ ae_lp16x2f.iu	aep1, a7, 4; ae_mulfp24s.ll	aeq1, aep1, aep0 }
    2b10:	350094        	ae_slliq56	aeq0, aeq0, 2
    2b13:	f963ba906f 	{ ae_slliq56	aeq1, aeq1, 2; ae_roundsp16q48asym	aep2, aeq0 }
    2b18:	f96bf227ef 	{ ae_sp16f.l.i	aep2, a7, -4; ae_roundsp16q48asym	aep3, aeq1 }
    2b1d:	4237f4        	ae_sp16f.l.i	aep3, a7, -2

00002b20 <Voice_EC_Prcs_common+0xc68>:
    2b20:	02ad           	mov.n	a10, a2
    2b22:	f59e21        	l32r	a2, 19c (6448 <beam2_mid_band>)
    2b25:	0002e0        	callx8	a2
    2b28:	f53e21        	l32r	a2, 20 (8688 <p_ecnr_mem>)
    2b2b:	262152        	l32i	a5, a1, 152
    2b2e:	2a2162        	l32i	a6, a1, 168
    2b31:	0278           	l32i.n	a7, a2, 0
    2b33:	392122        	l32i	a2, a1, 228
    2b36:	61a132        	movi	a3, 0x161
    2b39:	12d782        	addmi	a8, a7, 0x1200
    2b3c:	1133b0        	slli	a3, a3, 5
    2b3f:	d73a           	add.n	a13, a7, a3
    2b41:	2225a2        	l32i	a10, a5, 136
    2b44:	56b8           	l32i.n	a11, a6, 20
    2b46:	b2c8           	l32i.n	a12, a2, 44
    2b48:	1e28e2        	l32i	a14, a8, 120
    2b4b:	f59531        	l32r	a3, 1a0 (1ad4 <synth_3band>)
    2b4e:	0003e0        	callx8	a3
    2b51:	f53331        	l32r	a3, 20 (8688 <p_ecnr_mem>)
    2b54:	2021e2        	l32i	a14, a1, 128
    2b57:	23a1f2        	movi	a15, 0x123
    2b5a:	0378           	l32i.n	a7, a3, 0
    2b5c:	113fb0        	slli	a3, a15, 5
    2b5f:	282eb2        	l32i	a11, a14, 160
    2b62:	12d782        	addmi	a8, a7, 0x1200
    2b65:	d73a           	add.n	a13, a7, a3
    2b67:	1e28a2        	l32i	a10, a8, 120
    2b6a:	1d21c2        	l32i	a12, a1, 116
    2b6d:	3021e2        	l32i	a14, a1, 192
    2b70:	f58d31        	l32r	a3, 1a4 (18f0 <tx_upsample_by2>)
    2b73:	0003e0        	callx8	a3
    2b76:	2425a2        	l32i	a10, a5, 144
    2b79:	f52951        	l32r	a5, 20 (8688 <p_ecnr_mem>)
    2b7c:	9ca0f2        	movi	a15, 156
    2b7f:	d2c8           	l32i.n	a12, a2, 52
    2b81:	0578           	l32i.n	a7, a5, 0
    2b83:	2cdf22        	addmi	a2, a15, 0x2c00
    2b86:	76b8           	l32i.n	a11, a6, 28
    2b88:	12d782        	addmi	a8, a7, 0x1200
    2b8b:	d72a           	add.n	a13, a7, a2
    2b8d:	1d28e2        	l32i	a14, a8, 116
    2b90:	f58421        	l32r	a2, 1a0 (1ad4 <synth_3band>)
    2b93:	0002e0        	callx8	a2
    2b96:	0578           	l32i.n	a7, a5, 0
    2b98:	202122        	l32i	a2, a1, 128
    2b9b:	2f2152        	l32i	a5, a1, 188
    2b9e:	12d782        	addmi	a8, a7, 0x1200
    2ba1:	9f4c           	movi.n	a15, 73
    2ba3:	2a22b2        	l32i	a11, a2, 168
    2ba6:	112f90        	slli	a2, a15, 7
    2ba9:	1d28a2        	l32i	a10, a8, 116
    2bac:	3821e2        	l32i	a14, a1, 224
    2baf:	d72a           	add.n	a13, a7, a2
    2bb1:	05cd           	mov.n	a12, a5
    2bb3:	0003e0        	callx8	a3

00002bb6 <Voice_EC_Prcs_common+0xcfe>:
    2bb6:	212122        	l32i	a2, a1, 132
    2bb9:	50c552        	addi	a5, a5, 80
    2bbc:	2f6152        	s32i	a5, a1, 188
    2bbf:	f51861        	l32r	a6, 20 (8688 <p_ecnr_mem>)
    2bc2:	1d2152        	l32i	a5, a1, 116
    2bc5:	50c222        	addi	a2, a2, 80
    2bc8:	216122        	s32i	a2, a1, 132
    2bcb:	0678           	l32i.n	a7, a6, 0
    2bcd:	124c           	movi.n	a2, 65
    2bcf:	05a132        	movi	a3, 0x105
    2bd2:	1122a0        	slli	a2, a2, 6
    2bd5:	1133c0        	slli	a3, a3, 4
    2bd8:	c72a           	add.n	a12, a7, a2
    2bda:	f57321        	l32r	a2, 1a8 (19ec <deemph_filter_N>)
    2bdd:	b73a           	add.n	a11, a7, a3
    2bdf:	8d2c           	movi.n	a13, 40
    2be1:	05ad           	mov.n	a10, a5
    2be3:	0002e0        	callx8	a2
    2be6:	1c2122        	l32i	a2, a1, 112
    2be9:	0678           	l32i.n	a7, a6, 0
    2beb:	6821c2        	l32i	a12, a1, 0x1a0
    2bee:	8c2216        	beqz	a2, 24b4 <Voice_EC_Prcs_common+0x5fc>
    2bf1:	f55d21        	l32r	a2, 168 (19a4 <emph_filter_N>)
    2bf4:	b73a           	add.n	a11, a7, a3
    2bf6:	322132        	l32i	a3, a1, 200
    2bf9:	8d2c           	movi.n	a13, 40
    2bfb:	03ad           	mov.n	a10, a3
    2bfd:	0002e0        	callx8	a2
    2c00:	1b2122        	l32i	a2, a1, 108
    2c03:	03cd           	mov.n	a12, a3
    2c05:	3d21d2        	l32i	a13, a1, 244
    2c08:	9f22a2        	l32i	a10, a2, 0x27c
    2c0b:	a022b2        	l32i	a11, a2, 0x280
    2c0e:	6121e2        	l32i	a14, a1, 0x184
    2c11:	f55631        	l32r	a3, 16c (1838 <tx_downsample_by2>)
    2c14:	0003e0        	callx8	a3
    2c17:	0678           	l32i.n	a7, a6, 0
    2c19:	9f22a2        	l32i	a10, a2, 0x27c
    2c1c:	5922d2        	l32i	a13, a2, 0x164
    2c1f:	12d772        	addmi	a7, a7, 0x1200
    2c22:	4922e2        	l32i	a14, a2, 0x124
    2c25:	1627c2        	l32i	a12, a7, 88
    2c28:	6721b2        	l32i	a11, a1, 0x19c
    2c2b:	f55131        	l32r	a3, 170 (1a30 <analy_3band>)
    2c2e:	0003e0        	callx8	a3
    2c31:	0678           	l32i.n	a7, a6, 0
    2c33:	392282        	l32i	a8, a2, 228
    2c36:	05a032        	movi	a3, 5
    2c39:	12d772        	addmi	a7, a7, 0x1200
    2c3c:	162792        	l32i	a9, a7, 88
    2c3f:	fcc872        	addi	a7, a8, -4
    2c42:	f8c982        	addi	a8, a9, -8
    2c45:	088376        	loop	a3, 2c51 <Voice_EC_Prcs_common+0xd99>
    2c48:	5c8814        	ae_lp24x2f.iu	aep0, a8, 8
    2c4b:	090184        	ae_roundsp16asym	aep0, aep0
    2c4e:	468714        	ae_sp16x2f.iu	aep0, a7, 4

00002c51 <Voice_EC_Prcs_common+0xd99>:
    2c51:	232162        	l32i	a6, a1, 140
    2c54:	312122        	l32i	a2, a1, 196
    2c57:	6421b2        	l32i	a11, a1, 0x190
    2c5a:	f54651        	l32r	a5, 174 (d84 <subband_aec_block>)
    2c5d:	ac0c           	movi.n	a12, 10
    2c5f:	1e0c           	movi.n	a14, 1
    2c61:	06ad           	mov.n	a10, a6
    2c63:	02dd           	mov.n	a13, a2
    2c65:	0005e0        	callx8	a5
    2c68:	6621b2        	l32i	a11, a1, 0x198
    2c6b:	ac0c           	movi.n	a12, 10
    2c6d:	3e0c           	movi.n	a14, 3
    2c6f:	06ad           	mov.n	a10, a6
    2c71:	02dd           	mov.n	a13, a2
    2c73:	0005e0        	callx8	a5
    2c76:	602132        	l32i	a3, a1, 0x180
    2c79:	5621b2        	l32i	a11, a1, 0x158
    2c7c:	4c1c           	movi.n	a12, 20
    2c7e:	4e0372        	l8ui	a7, a3, 78
    2c81:	4e0c           	movi.n	a14, 4
    2c83:	06ad           	mov.n	a10, a6
    2c85:	678c           	beqz.n	a7, 2c8f <Voice_EC_Prcs_common+0xdd7>
    2c87:	02dd           	mov.n	a13, a2
    2c89:	0005e0        	callx8	a5
    2c8c:	000486        	j	2ca2 <Voice_EC_Prcs_common+0xdea>

00002c8f <Voice_EC_Prcs_common+0xdd7>:
    2c8f:	1b2132        	l32i	a3, a1, 108
    2c92:	f50d61        	l32r	a6, c8 (6bf0 <xt_memset>)
    2c95:	0b0c           	movi.n	a11, 0
    2c97:	6b23a2        	l32i	a10, a3, 0x1ac
    2c9a:	4c1c           	movi.n	a12, 20
    2c9c:	0006e0        	callx8	a6
    2c9f:	232162        	l32i	a6, a1, 140
    2ca2:	f53531        	l32r	a3, 178 (1060 <subband_aec_mid_channel>)
    2ca5:	6521b2        	l32i	a11, a1, 0x194
    2ca8:	2d0c           	movi.n	a13, 2
    2caa:	06ad           	mov.n	a10, a6
    2cac:	02cd           	mov.n	a12, a2
    2cae:	0003e0        	callx8	a3
    2cb1:	692152        	l32i	a5, a1, 0x1a4
    2cb4:	432132        	l32i	a3, a1, 0x10c
    2cb7:	1b2122        	l32i	a2, a1, 108
    2cba:	15cc           	bnez.n	a5, 2cbf <Voice_EC_Prcs_common+0xe07>
    2cbc:	fdd346        	j	240d <Voice_EC_Prcs_common+0x555>
    2cbf:	2d2152        	l32i	a5, a1, 180
    2cc2:	f4d661        	l32r	a6, 1c (6c04 <xt_memcpy>)
    2cc5:	ac0c           	movi.n	a12, 10
    2cc7:	b5a8           	l32i.n	a10, a5, 44
    2cc9:	d5b8           	l32i.n	a11, a5, 52
    2ccb:	0006e0        	callx8	a6
    2cce:	fd24b2e1ce 	{ l32i	a12, a1, 184; ae_zeroq56	aeq1 }
    2cd3:	2c2162        	l32i	a6, a1, 176
    2cd6:	0c2582        	l32i	a8, a5, 48
    2cd9:	142c92        	l32i	a9, a12, 80
    2cdc:	2b21e2        	l32i	a14, a1, 172
    2cdf:	0a2672        	l32i	a7, a6, 40
    2ce2:	5421b2        	l32i	a11, a1, 0x150
    2ce5:	5121d2        	l32i	a13, a1, 0x144
    2ce8:	05a0f2        	movi	a15, 5
    2ceb:	f3f6b0        	wur.ae_cbegin0	a11
    2cee:	f3f7d0        	wur.ae_cend0	a13
    2cf1:	600e64        	ae_lq32f.i	aeq0, a14, 24
    2cf4:	440804        	ae_lp16x2f.i	aep0, a8, 0
    2cf7:	fcc992        	addi	a9, a9, -4
    2cfa:	04a052        	movi	a5, 4
    2cfd:	198f76        	loop	a15, 2d1a <Voice_EC_Prcs_common+0xe62>
    2d00:	09001aa75f 	{ ae_lp16x2f.c	aep2, a7, a5; ae_mulzaafq32sp16s.hh	aeq2, aeq0, aep0, aeq0, aep0 }
    2d05:	11006d881f 	{ ae_lp16x2f.iu	aep0, a8, 4; ae_mulzaafq32sp16s.ll	aeq3, aeq0, aep0, aeq0, aep0 }
    2d0a:	1baf74        	ae_roundsp16q48asym	aep7, aeq2
    2d0d:	1bbf14        	ae_roundsp16q48asym	aep1, aeq3
    2d10:	f04a7e715f 	{ ae_selp24.hl	aep1, aep7, aep1; ae_mulssfp24s.hh.ll	aeq1, aep2, aep2 }
    2d15:	e22473991f 	{ ae_sp16x2f.iu	aep1, a9, 4; ae_mulaafp24s.hh.ll	aeq1, aep1, aep1 }

00002d1a <Voice_EC_Prcs_common+0xe62>:
    2d1a:	522162        	l32i	a6, a1, 0x148
    2d1d:	90a252        	movi	a5, 0x290
    2d20:	351234        	ae_sraiq56	aeq0, aeq1, 8
    2d23:	61a254        	ae_lq56.x	aeq2, a2, a5
    2d26:	f87230967e 	{ l32i	a7, a6, 36; ae_addq56	aeq0, aeq2, aeq0 }
    2d2b:	fc00fec7ff 	{ ae_cvtq48a32s	aeq3, a7; ae_satq48s	aeq0, aeq0 }
    2d30:	f87b35316e 	{ l32i	a6, a1, 0x14c; ae_maxq56s	aeq0, aeq0, aeq3 }
    2d35:	638254        	ae_sq56s.x	aeq0, a2, a5
    2d38:	552152        	l32i	a5, a1, 0x154
    2d3b:	0e4c           	movi.n	a14, 64
    2d3d:	482182        	l32i	a8, a1, 0x120
    2d40:	b5ac           	beqz.n	a5, 2d6f <Voice_EC_Prcs_common+0xeb7>
    2d42:	ffa092        	movi	a9, 255
    2d45:	119872        	l16si	a7, a8, 34
    2d48:	da4c           	movi.n	a10, 77
    2d4a:	11aa00        	slli	a10, a10, 16
    2d4d:	0b2797        	blt	a7, a9, 2d5c <Voice_EC_Prcs_common+0xea4>
    2d50:	452172        	l32i	a7, a1, 0x114
    2d53:	109772        	l16si	a7, a7, 32
    2d56:	11a700        	slli	a10, a7, 16
    2d59:	000106        	j	2d61 <Voice_EC_Prcs_common+0xea9>

00002d5c <Voice_EC_Prcs_common+0xea4>:
    2d5c:	771b           	addi.n	a7, a7, 1
    2d5e:	115872        	s16i	a7, a8, 34
    2d61:	368b04        	ae_trunca32q48	a11, aeq0
    2d64:	f4e471        	l32r	a7, f4 (4630 <airoha_divide>)
    2d67:	0007e0        	callx8	a7
    2d6a:	2e21c2        	l32i	a12, a1, 184
    2d6d:	0e4c           	movi.n	a14, 64
    2d6f:	2d2182        	l32i	a8, a1, 180
    2d72:	2c2172        	l32i	a7, a1, 176
    2d75:	142c92        	l32i	a9, a12, 80
    2d78:	d8d8           	l32i.n	a13, a8, 52
    2d7a:	b8f8           	l32i.n	a15, a8, 44
    2d7c:	a778           	l32i.n	a7, a7, 40
    2d7e:	360b34        	ae_cvtq48a32s	aeq0, a11
    2d81:	a80c           	movi.n	a8, 10
    2d83:	fec992        	addi	a9, a9, -2
    2d86:	fecfb2        	addi	a11, a15, -2
    2d89:	fecdc2        	addi	a12, a13, -2
    2d8c:	408914        	ae_lp16f.iu	aep0, a9, 2
    2d8f:	fc24e2874f 	{ ae_sp16f.l.c	aep0, a7, a4; nop }
    2d94:	408b14        	ae_lp16f.iu	aep0, a11, 2
    2d97:	fd0c13d66f 	{ or	a13, a6, a6; ae_cvtq48p24s.l	aeq1, aep0 }
    2d9c:	508d14        	ae_lp24f.iu	aep0, a13, 4
    2d9f:	098e76        	loop	a14, 2dac <Voice_EC_Prcs_common+0xef4>
    2da2:	fc24d9174f 	{ ae_lp16f.c	aep1, a7, a4; nop }
    2da7:	ee21ee8d1f 	{ ae_lp24f.iu	aep0, a13, 4; ae_mulsfp24s.ll	aeq1, aep0, aep1 }

00002dac <Voice_EC_Prcs_common+0xef4>:
    2dac:	f86b93d66f 	{ or	a13, a6, a6; ae_roundsp16q48asym	aep0, aeq1 }
    2db1:	d4e2728c1f 	{ ae_sp16f.l.iu	aep0, a12, 2; ae_mulfq32sp16s.l	aeq1, aeq0, aep0 }
    2db6:	e59c           	beqz.n	a5, 2dd8 <Voice_EC_Prcs_common+0xf20>
    2db8:	fc24fe9a4f 	{ ae_slaasq56s	aeq1, aeq1, a10; nop }
    2dbd:	f86fae9d1f 	{ ae_lp24f.iu	aep1, a13, 4; ae_roundsp24q48asym	aep0, aeq1 }
    2dc2:	0020f0        	nop
    2dc5:	0f8e76        	loop	a14, 2dd8 <Voice_EC_Prcs_common+0xf20>
    2dc8:	fd2c19774f 	{ ae_lp16f.c	aep7, a7, a4; ae_cvtq48p24s.l	aeq1, aep1 }
    2dcd:	e2e3ee9d1f 	{ ae_lp24f.iu	aep1, a13, 4; ae_mulafp24s.ll	aeq1, aep0, aep7 }
    2dd2:	1b9d24        	ae_roundsp24q48asym	aep2, aeq1
    2dd5:	522df4        	ae_sp24f.l.i	aep2, a13, -4

00002dd8 <Voice_EC_Prcs_common+0xf20>:
    2dd8:	880b           	addi.n	a8, a8, -1
    2dda:	fae856        	bnez	a8, 2d8c <Voice_EC_Prcs_common+0xed4>
    2ddd:	fc24b2d16e 	{ l32i	a6, a1, 180; ae_zeroq56	aeq0 }
    2de2:	fd0472c1ee 	{ l32i	a14, a1, 176; ae_movq56	aeq1, aeq0 }
    2de7:	5f0c           	movi.n	a15, 5
    2de9:	d6a8           	l32i.n	a10, a6, 52
    2deb:	b6b8           	l32i.n	a11, a6, 44
    2ded:	ae79           	s32i.n	a7, a14, 40
    2def:	fcca82        	addi	a8, a10, -4
    2df2:	fccb72        	addi	a7, a11, -4
    2df5:	0a8f76        	loop	a15, 2e03 <Voice_EC_Prcs_common+0xf4b>
    2df8:	448714        	ae_lp16x2f.iu	aep0, a7, 4
    2dfb:	e2002d981f 	{ ae_lp16x2f.iu	aep1, a8, 4; ae_mulaafp24s.hh.ll	aeq0, aep0, aep0 }
    2e00:	051904        	ae_mulaafp24s.hh.ll	aeq1, aep1, aep1

00002e03 <Voice_EC_Prcs_common+0xf4b>:
    2e03:	359134        	ae_sraiq56	aeq2, aeq1, 4
    2e06:	f9aac0007f 	{ movi	a7, 0; ae_subq56	aeq1, aeq1, aeq2 }
    2e0b:	d64800016f 	{ movi	a6, 1; ae_ltq56s	b0, aeq0, aeq1 }
    2e10:	ac0c           	movi.n	a12, 10
    2e12:	d37600        	movt	a7, a6, b0
    2e15:	478c           	beqz.n	a7, 2e1d <Voice_EC_Prcs_common+0xf65>
    2e17:	f48161        	l32r	a6, 1c (6c04 <xt_memcpy>)
    2e1a:	0006e0        	callx8	a6

00002e1d <Voice_EC_Prcs_common+0xf65>:
    2e1d:	fc24b5017e 	{ l32i	a7, a1, 0x140; ae_zeroq56	aeq0 }
    2e22:	98a262        	movi	a6, 0x298
    2e25:	0f2c           	movi.n	a15, 32
    2e27:	5d0264        	ae_lp24x2f.x	aep0, a2, a6
    2e2a:	0020f0        	nop
    2e2d:	048f76        	loop	a15, 2e35 <Voice_EC_Prcs_common+0xf7d>
    2e30:	e2002f871f 	{ ae_lp24x2f.iu	aep0, a7, 8; ae_mulaafp24s.hh.ll	aeq0, aep0, aep0 }

00002e35 <Voice_EC_Prcs_common+0xf7d>:
    2e35:	2b2162        	l32i	a6, a1, 172
    2e38:	3500b4        	ae_sraiq56	aeq0, aeq0, 2
    2e3b:	064934        	ae_roundsq32asym	aeq1, aeq0
    2e3e:	7678           	l32i.n	a7, a6, 28
    2e40:	360734        	ae_cvtq48a32s	aeq0, a7
    2e43:	15cc           	bnez.n	a5, 2e48 <Voice_EC_Prcs_common+0xf90>
    2e45:	fd68c6        	j	23ec <Voice_EC_Prcs_common+0x534>
    2e48:	369704        	ae_trunca32q48	a7, aeq1
    2e4b:	f4d851        	l32r	a5, 1ac (6ffff <_end+0x67973>)
    2e4e:	760c           	movi.n	a6, 7
    2e50:	118600        	slli	a8, a6, 16
    2e53:	02a577        	bge	a5, a7, 2e59 <Voice_EC_Prcs_common+0xfa1>
    2e56:	fd60c6        	j	23dd <Voice_EC_Prcs_common+0x525>
    2e59:	540134        	ae_lp24x2.i	aep0, a1, 24
    2e5c:	030214        	ae_mulfq32sp16s.l	aeq0, aeq0, aep0
    2e5f:	fd60c6        	j	23e6 <Voice_EC_Prcs_common+0x52e>

00002e62 <Voice_EC_Prcs_common+0xfaa>:
    2e62:	102142        	l32i	a4, a1, 64
    2e65:	130c           	movi.n	a3, 1
    2e67:	113320        	slli	a3, a3, 14
    2e6a:	222347        	blt	a3, a4, 2e90 <Voice_EC_Prcs_common+0xfd8>
    2e6d:	f46c21        	l32r	a2, 20 (8688 <p_ecnr_mem>)
    2e70:	24a542        	movi	a4, 0x524
    2e73:	f04440        	subx8	a4, a4, a4
    2e76:	0258           	l32i.n	a5, a2, 0
    2e78:	860c           	movi.n	a6, 8
    2e7a:	454a           	add.n	a4, a5, a4
    2e7c:	24d552        	addmi	a5, a5, 0x2400
    2e7f:	600504        	ae_lq32f.i	aeq0, a5, 0
    2e82:	0a8676        	loop	a6, 2e90 <Voice_EC_Prcs_common+0xfd8>
    2e85:	350074        	ae_sraiq56	aeq0, aeq0, 1
    2e88:	f867bd151f 	{ ae_lq32f.iu	aeq0, a5, 4; ae_roundsp24q48asym	aep0, aeq0 }
    2e8d:	528414        	ae_sp24f.l.iu	aep0, a4, 4

00002e90 <Voice_EC_Prcs_common+0xfd8>:
    2e90:	132142        	l32i	a4, a1, 76
    2e93:	21a182        	movi	a8, 0x121
    2e96:	23a172        	movi	a7, 0x123
    2e99:	1188b0        	slli	a8, a8, 5
    2e9c:	1177b0        	slli	a7, a7, 5
    2e9f:	202347        	blt	a3, a4, 2ec3 <Voice_EC_Prcs_common+0x100b>
    2ea2:	f45f21        	l32r	a2, 20 (8688 <p_ecnr_mem>)
    2ea5:	c45c           	movi.n	a4, 92
    2ea7:	24d442        	addmi	a4, a4, 0x2400
    2eaa:	0258           	l32i.n	a5, a2, 0
    2eac:	08a062        	movi	a6, 8
    2eaf:	804540        	add	a4, a5, a4
    2eb2:	603754        	ae_lq32f.xu	aeq0, a7, a5
    2eb5:	0a8676        	loop	a6, 2ec3 <Voice_EC_Prcs_common+0x100b>
    2eb8:	350074        	ae_sraiq56	aeq0, aeq0, 1
    2ebb:	f867bd171f 	{ ae_lq32f.iu	aeq0, a7, 4; ae_roundsp24q48asym	aep0, aeq0 }
    2ec0:	528414        	ae_sp24f.l.iu	aep0, a4, 4
    2ec3:	122142        	l32i	a4, a1, 72
    2ec6:	f45621        	l32r	a2, 20 (8688 <p_ecnr_mem>)
    2ec9:	1a2172        	l32i	a7, a1, 104
    2ecc:	232347        	blt	a3, a4, 2ef3 <Voice_EC_Prcs_common+0x103b>
    2ecf:	f45421        	l32r	a2, 20 (8688 <p_ecnr_mem>)
    2ed2:	c41c           	movi.n	a4, 28
    2ed4:	24d442        	addmi	a4, a4, 0x2400
    2ed7:	0258           	l32i.n	a5, a2, 0
    2ed9:	f45121        	l32r	a2, 20 (8688 <p_ecnr_mem>)
    2edc:	08a062        	movi	a6, 8
    2edf:	804540        	add	a4, a5, a4
    2ee2:	603854        	ae_lq32f.xu	aeq0, a8, a5
    2ee5:	0a8676        	loop	a6, 2ef3 <Voice_EC_Prcs_common+0x103b>
    2ee8:	350074        	ae_sraiq56	aeq0, aeq0, 1
    2eeb:	f867bd181f 	{ ae_lq32f.iu	aeq0, a8, 4; ae_roundsp24q48asym	aep0, aeq0 }
    2ef0:	528414        	ae_sp24f.l.iu	aep0, a4, 4
    2ef3:	412142        	l32i	a4, a1, 0x104
    2ef6:	112152        	l32i	a5, a1, 68
    2ef9:	91a062        	movi	a6, 145
    2efc:	067416        	beqz	a4, 2f67 <Voice_EC_Prcs_common+0x10af>
    2eff:	1176a0        	slli	a7, a6, 6
    2f02:	1e2357        	blt	a3, a5, 2f24 <Voice_EC_Prcs_common+0x106c>
    2f05:	f44621        	l32r	a2, 20 (8688 <p_ecnr_mem>)
    2f08:	c43c           	movi.n	a4, 60
    2f0a:	24d442        	addmi	a4, a4, 0x2400
    2f0d:	0258           	l32i.n	a5, a2, 0
    2f0f:	860c           	movi.n	a6, 8
    2f11:	454a           	add.n	a4, a5, a4
    2f13:	603754        	ae_lq32f.xu	aeq0, a7, a5
    2f16:	0a8676        	loop	a6, 2f24 <Voice_EC_Prcs_common+0x106c>
    2f19:	350074        	ae_sraiq56	aeq0, aeq0, 1
    2f1c:	f867bd171f 	{ ae_lq32f.iu	aeq0, a7, 4; ae_roundsp24q48asym	aep0, aeq0 }
    2f21:	528414        	ae_sp24f.l.iu	aep0, a4, 4
    2f24:	142142        	l32i	a4, a1, 80
    2f27:	974c           	movi.n	a7, 73
    2f29:	117790        	slli	a7, a7, 7
    2f2c:	1f2347        	blt	a3, a4, 2f4f <Voice_EC_Prcs_common+0x1097>
    2f2f:	f43c21        	l32r	a2, 20 (8688 <p_ecnr_mem>)
    2f32:	4ca742        	movi	a4, 0x74c
    2f35:	a04440        	addx4	a4, a4, a4
    2f38:	0258           	l32i.n	a5, a2, 0
    2f3a:	860c           	movi.n	a6, 8
    2f3c:	454a           	add.n	a4, a5, a4
    2f3e:	603754        	ae_lq32f.xu	aeq0, a7, a5
    2f41:	0a8676        	loop	a6, 2f4f <Voice_EC_Prcs_common+0x1097>
    2f44:	350074        	ae_sraiq56	aeq0, aeq0, 1
    2f47:	f867bd171f 	{ ae_lq32f.iu	aeq0, a7, 4; ae_roundsp24q48asym	aep0, aeq0 }
    2f4c:	528414        	ae_sp24f.l.iu	aep0, a4, 4
    2f4f:	f49841        	l32r	a4, 1b0 (5d70 <mag_calibration>)
    2f52:	1f21a2        	l32i	a10, a1, 124
    2f55:	0004e0        	callx8	a4
    2f58:	1a2172        	l32i	a7, a1, 104
    2f5b:	f43121        	l32r	a2, 20 (8688 <p_ecnr_mem>)
    2f5e:	050c           	movi.n	a5, 0
    2f60:	25d742        	addmi	a4, a7, 0x2500
    2f63:	d459           	s32i.n	a5, a4, 52
    2f65:	e459           	s32i.n	a5, a4, 56

00002f67 <Voice_EC_Prcs_common+0x10af>:
    2f67:	02a8           	l32i.n	a10, a2, 0
    2f69:	05a162        	movi	a6, 0x105
    2f6c:	1166c0        	slli	a6, a6, 4
    2f6f:	12da42        	addmi	a4, a10, 0x1200
    2f72:	550442        	l8ui	a4, a4, 85
    2f75:	282152        	l32i	a5, a1, 160
    2f78:	216407        	bbci	a4, 0, 2f9d <Voice_EC_Prcs_common+0x10e5>
    2f7b:	f48e41        	l32r	a4, 1b4 (3a48 <NC_NOISE_EST_Prcs>)
    2f7e:	05bd           	mov.n	a11, a5
    2f80:	0004e0        	callx8	a4
    2f83:	0248           	l32i.n	a4, a2, 0
    2f85:	09a2f2        	movi	a15, 0x209
    2f88:	f0a0d2        	movi	a13, 240
    2f8b:	b46a           	add.n	a11, a4, a6
    2f8d:	116fd0        	slli	a6, a15, 3
    2f90:	c46a           	add.n	a12, a4, a6
    2f92:	f48541        	l32r	a4, 1a8 (19ec <deemph_filter_N>)
    2f95:	05ad           	mov.n	a10, a5
    2f97:	0004e0        	callx8	a4
    2f9a:	1a2172        	l32i	a7, a1, 104
    2f9d:	92a452        	movi	a5, 0x492
    2fa0:	51a142        	movi	a4, 0x151
    2fa3:	b05550        	addx8	a5, a5, a5
    2fa6:	1144b0        	slli	a4, a4, 5
    2fa9:	602744        	ae_lq32f.x	aeq0, a7, a4
    2fac:	418574        	ae_lp16f.xu	aep0, a5, a7
    2faf:	d4e234416e 	{ l32i	a6, a1, 0x110; ae_mulfq32sp16s.l	aeq0, aeq0, aep0 }
    2fb4:	20c1f2        	addi	a15, a1, 32
    2fb7:	620f04        	ae_sq32f.i	aeq0, a15, 0
    2fba:	182662        	l32i	a6, a6, 96
    2fbd:	8148           	l32i.n	a4, a1, 32
    2fbf:	2321a2        	l32i	a10, a1, 140
    2fc2:	53b640        	max	a11, a6, a4
    2fc5:	f47c41        	l32r	a4, 1b8 (1318 <dt_detect>)
    2fc8:	0004e0        	callx8	a4
    2fcb:	1a2192        	l32i	a9, a1, 104
    2fce:	0248           	l32i.n	a4, a2, 0
    2fd0:	422c           	movi.n	a2, 36
    2fd2:	29d962        	addmi	a6, a9, 0x2900
    2fd5:	12d472        	addmi	a7, a4, 0x1200
    2fd8:	366d           	ae_l16si.n	a6, a6, 2
    2fda:	279772        	l16si	a7, a7, 78
    2fdd:	2ad222        	addmi	a2, a2, 0x2a00
    2fe0:	64a0e2        	movi	a14, 100
    2fe3:	08a677        	bge	a6, a7, 2fef <Voice_EC_Prcs_common+0x1137>
    2fe6:	f45121        	l32r	a2, 12c (7fff0000 <_end+0x7ffe7974>)
    2fe9:	360234        	ae_cvtq48a32s	aeq0, a2
    2fec:	000586        	j	3006 <Voice_EC_Prcs_common+0x114e>

00002fef <Voice_EC_Prcs_common+0x1137>:
    2fef:	400504        	ae_lp16f.i	aep0, a5, 0
    2ff2:	602924        	ae_lq32f.x	aeq0, a9, a2
    2ff5:	d4e20000ff 	{ movi	a15, 0; ae_mulfq32sp16s.l	aeq0, aeq0, aep0 }
    2ffa:	29de22        	addmi	a2, a14, 0x2900
    2ffd:	606924        	ae_lq32f.x	aeq1, a9, a2
    3000:	0110c4        	ae_maxq56s	aeq0, aeq0, aeq1
    3003:	3561f2        	s32i	a15, a1, 212
    3006:	f863b1c12e 	{ l32i	a2, a1, 112; ae_roundsp16q48asym	aep0, aeq0 }
    300b:	c52c           	movi.n	a5, 44
    300d:	1b21a2        	l32i	a10, a1, 108
    3010:	29d552        	addmi	a5, a5, 0x2900
    3013:	430954        	ae_sp16f.l.x	aep0, a9, a5
    3016:	0d9216        	beqz	a2, 30f3 <Voice_EC_Prcs_common+0x123b>
    3019:	152152        	l32i	a5, a1, 84
    301c:	3d2122        	l32i	a2, a1, 244
    301f:	30ca72        	addi	a7, a10, 48
    3022:	152357        	blt	a3, a5, 303b <Voice_EC_Prcs_common+0x1183>
    3025:	860c           	movi.n	a6, 8
    3027:	600704        	ae_lq32f.i	aeq0, a7, 0
    302a:	2cca52        	addi	a5, a10, 44
    302d:	0a8676        	loop	a6, 303b <Voice_EC_Prcs_common+0x1183>
    3030:	350074        	ae_sraiq56	aeq0, aeq0, 1
    3033:	f867bd121f 	{ ae_lq32f.iu	aeq0, a2, 4; ae_roundsp24q48asym	aep0, aeq0 }
    3038:	528514        	ae_sp24f.l.iu	aep0, a5, 4

0000303b <Voice_EC_Prcs_common+0x1183>:
    303b:	162152        	l32i	a5, a1, 88
    303e:	3e2122        	l32i	a2, a1, 248
    3041:	08a062        	movi	a6, 8
    3044:	132357        	blt	a3, a5, 305b <Voice_EC_Prcs_common+0x11a3>
    3047:	4cca32        	addi	a3, a10, 76
    304a:	600204        	ae_lq32f.i	aeq0, a2, 0
    304d:	0a8676        	loop	a6, 305b <Voice_EC_Prcs_common+0x11a3>
    3050:	350074        	ae_sraiq56	aeq0, aeq0, 1
    3053:	f867bd121f 	{ ae_lq32f.iu	aeq0, a2, 4; ae_roundsp24q48asym	aep0, aeq0 }
    3058:	528314        	ae_sp24f.l.iu	aep0, a3, 4
    305b:	12d432        	addmi	a3, a4, 0x1200
    305e:	322a52        	l32i	a5, a10, 200
    3061:	2b1342        	l16ui	a4, a3, 86
    3064:	330c           	movi.n	a3, 3
    3066:	05bd           	mov.n	a11, a5
    3068:	3f0347        	bnone	a3, a4, 30ab <Voice_EC_Prcs_common+0x11f3>
    306b:	522ab2        	l32i	a11, a10, 0x148
    306e:	144040        	extui	a4, a4, 0, 2
    3071:	332426        	beqi	a4, 2, 30a8 <Voice_EC_Prcs_common+0x11f0>
    3074:	331426        	beqi	a4, 1, 30ab <Voice_EC_Prcs_common+0x11f3>
    3077:	43b5b0        	min	a11, a5, a11
    307a:	000b46        	j	30ab <Voice_EC_Prcs_common+0x11f3>

0000307d <Voice_EC_Prcs_common+0x11c5>:
	...

0000307f <Voice_EC_Prcs_common+0x11c7>:
    307f:	04d232        	addmi	a3, a2, 0x400
    3082:	460542        	l8ui	a4, a5, 70
    3085:	221362        	l16ui	a6, a3, 68
    3088:	1a21a2        	l32i	a10, a1, 104
    308b:	84a782        	movi	a8, 0x784
    308e:	a08880        	addx4	a8, a8, a8
    3091:	021426        	beqi	a4, 1, 3097 <Voice_EC_Prcs_common+0x11df>
    3094:	002a86        	j	3142 <Voice_EC_Prcs_common+0x128a>
    3097:	0266f6        	bgeui	a6, 6, 309d <Voice_EC_Prcs_common+0x11e5>
    309a:	002906        	j	3142 <Voice_EC_Prcs_common+0x128a>
    309d:	040c           	movi.n	a4, 0
    309f:	464542        	s8i	a4, a5, 70
    30a2:	225342        	s16i	a4, a3, 68
    30a5:	fc3846        	j	218a <Voice_EC_Prcs_common+0x2d2>

000030a8 <Voice_EC_Prcs_common+0x11f0>:
    30a8:	53b5b0        	max	a11, a5, a11
    30ab:	29d942        	addmi	a4, a9, 0x2900
    30ae:	1994c2        	l16si	a12, a4, 50
    30b1:	f44241        	l32r	a4, 1bc (135c <dt_detect_FB>)
    30b4:	0004e0        	callx8	a4
    30b7:	f3da21        	l32r	a2, 20 (8688 <p_ecnr_mem>)
    30ba:	0248           	l32i.n	a4, a2, 0
    30bc:	1b2122        	l32i	a2, a1, 108
    30bf:	12d452        	addmi	a5, a4, 0x1200
    30c2:	332262        	l32i	a6, a2, 204
    30c5:	2b1572        	l16ui	a7, a5, 86
    30c8:	532282        	l32i	a8, a2, 0x14c
    30cb:	065d           	mov.n	a5, a6
    30cd:	130377        	bnone	a3, a7, 30e4 <Voice_EC_Prcs_common+0x122c>
    30d0:	143070        	extui	a3, a7, 0, 2
    30d3:	085d           	mov.n	a5, a8
    30d5:	082326        	beqi	a3, 2, 30e1 <Voice_EC_Prcs_common+0x1229>
    30d8:	081326        	beqi	a3, 1, 30e4 <Voice_EC_Prcs_common+0x122c>
    30db:	435650        	min	a5, a6, a5
    30de:	000086        	j	30e4 <Voice_EC_Prcs_common+0x122c>

000030e1 <Voice_EC_Prcs_common+0x1229>:
    30e1:	535650        	max	a5, a6, a5
    30e4:	3521f2        	l32i	a15, a1, 212
    30e7:	f53050        	extui	a3, a5, 16, 16
    30ea:	04d222        	addmi	a2, a2, 0x400
    30ed:	933ff0        	movnez	a3, a15, a15
    30f0:	255232        	s16i	a3, a2, 74
    30f3:	46d422        	addmi	a2, a4, 0x4600
    30f6:	fc24b3525e 	{ l32i	a5, a2, 212; ae_zeroq56	aeq0 }
    30fb:	366252        	s32i	a5, a2, 216
    30fe:	362152        	l32i	a5, a1, 216
    3101:	d4a0f2        	movi	a15, 212
    3104:	78a032        	movi	a3, 120
    3107:	46df22        	addmi	a2, a15, 0x4600
    310a:	440504        	ae_lp16x2f.i	aep0, a5, 0
    310d:	048376        	loop	a3, 3115 <Voice_EC_Prcs_common+0x125d>
    3110:	e2002d851f 	{ ae_lp16x2f.iu	aep0, a5, 4; ae_mulaafp24s.hh.ll	aeq0, aep0, aep0 }

00003115 <Voice_EC_Prcs_common+0x125d>:
    3115:	77a032        	movi	a3, 119
    3118:	52d332        	addmi	a3, a3, 0x5200
    311b:	300334        	ae_cvtp24a16x2.ll	aep0, a3, a3
    311e:	350234        	ae_sraiq56	aeq0, aeq0, 8
    3121:	d4e20000af 	{ movi	a10, 0; ae_mulfq32sp16s.l	aeq0, aeq0, aep0 }
    3126:	f3c131        	l32r	a3, 2c (514 <HIFI_sqrt>)
    3129:	060934        	ae_roundsq32asym	aeq0, aeq0
    312c:	368b04        	ae_trunca32q48	a11, aeq0
    312f:	0003e0        	callx8	a3
    3132:	360b34        	ae_cvtq48a32s	aeq0, a11
    3135:	fc24fe8a0f 	{ ae_slaasq56s	aeq0, aeq0, a10; nop }
    313a:	060934        	ae_roundsq32asym	aeq0, aeq0
    313d:	628424        	ae_sq32f.x	aeq0, a4, a2
    3140:	f01d           	retw.n

00003142 <Voice_EC_Prcs_common+0x128a>:
    3142:	561b           	addi.n	a5, a6, 1
    3144:	225352        	s16i	a5, a3, 68
    3147:	fc0fc6        	j	218a <Voice_EC_Prcs_common+0x2d2>

0000314a <Voice_EC_Prcs_common+0x1292>:
	...

0000314c <conv_16_to_24_x2>:
    314c:	004136        	entry	a1, 32
    314f:	0e14a6        	blti	a4, 1, 3161 <conv_16_to_24_x2+0x15>
    3152:	fcc222        	addi	a2, a2, -4
    3155:	f8c332        	addi	a3, a3, -8
    3158:	058476        	loop	a4, 3161 <conv_16_to_24_x2+0x15>
    315b:	448214        	ae_lp16x2f.iu	aep0, a2, 4
    315e:	5e8314        	ae_sp24x2f.iu	aep0, a3, 8

00003161 <conv_16_to_24_x2+0x15>:
    3161:	f01d           	retw.n

00003163 <conv_16_to_24_x2+0x17>:
	...

00003164 <Voice_PostEC_Prcs>:
    3164:	004136        	entry	a1, 32
    3167:	f3ae61        	l32r	a6, 20 (8688 <p_ecnr_mem>)
    316a:	a3a572        	movi	a7, 0x5a3
    316d:	1177d0        	slli	a7, a7, 3
    3170:	0658           	l32i.n	a5, a6, 0
    3172:	02bd           	mov.n	a11, a2
    3174:	a57a           	add.n	a10, a5, a7
    3176:	f41271        	l32r	a7, 1c0 (6a8c <NOISE_GEN_Prcs>)
    3179:	05cd           	mov.n	a12, a5
    317b:	0007e0        	callx8	a7
    317e:	2bd572        	addmi	a7, a5, 0x2b00
    3181:	251772        	l16ui	a7, a7, 74
    3184:	25a582        	movi	a8, 0x525
    3187:	1198d0        	slli	a9, a8, 3
    318a:	579c           	beqz.n	a7, 31a3 <Voice_PostEC_Prcs+0x3f>
    318c:	29d582        	addmi	a8, a5, 0x2900
    318f:	a59a           	add.n	a10, a5, a9
    3191:	1698e2        	l16si	a14, a8, 44
    3194:	f40c71        	l32r	a7, 1c4 (6954 <aec_post_prcs>)
    3197:	02bd           	mov.n	a11, a2
    3199:	03cd           	mov.n	a12, a3
    319b:	05dd           	mov.n	a13, a5
    319d:	0007e0        	callx8	a7
    31a0:	000306        	j	31b0 <Voice_PostEC_Prcs+0x4c>

000031a3 <Voice_PostEC_Prcs+0x3f>:
    31a3:	f39e51        	l32r	a5, 1c (6c04 <xt_memcpy>)
    31a6:	f0a0c2        	movi	a12, 240
    31a9:	03ad           	mov.n	a10, a3
    31ab:	02bd           	mov.n	a11, a2
    31ad:	0005e0        	callx8	a5
    31b0:	0628           	l32i.n	a2, a6, 0
    31b2:	f40551        	l32r	a5, 1c8 (4ec4ec4f <_end+0x4ec465c3>)
    31b5:	c4a0f2        	movi	a15, 196
    31b8:	2dd232        	addmi	a3, a2, 0x2d00
    31bb:	201332        	l16ui	a3, a3, 64
    31be:	46d272        	addmi	a7, a2, 0x4600
    31c1:	903330        	addx2	a3, a3, a3
    31c4:	a03330        	addx4	a3, a3, a3
    31c7:	336b           	addi.n	a3, a3, 6
    31c9:	a23350        	muluh	a3, a3, a5
    31cc:	e10752        	l8ui	a5, a7, 225
    31cf:	413230        	srli	a3, a3, 2
    31d2:	837d           	ae_sext16	a7, a3
    31d4:	46df32        	addmi	a3, a15, 0x4600
    31d7:	33a757        	bge	a7, a5, 320e <Voice_PostEC_Prcs+0xaa>
    31da:	46d282        	addmi	a8, a2, 0x4600
    31dd:	e00882        	l8ui	a8, a8, 224
    31e0:	46d292        	addmi	a9, a2, 0x4600
    31e3:	46d2a2        	addmi	a10, a2, 0x4600
    31e6:	2da877        	bge	a8, a7, 3217 <Voice_PostEC_Prcs+0xb3>
    31e9:	6f9a92        	l16si	a9, a10, 222
    31ec:	6e9aa2        	l16si	a10, a10, 220
    31ef:	c07780        	sub	a7, a7, a8
    31f2:	c05580        	sub	a5, a5, a8
    31f5:	c0e9a0        	sub	a14, a9, a10
    31f8:	8277e0        	mull	a7, a7, a14
    31fb:	80a0f2        	movi	a15, 128
    31fe:	117780        	slli	a7, a7, 8
    3201:	d25750        	quos	a5, a7, a5
    3204:	55fa           	add.n	a5, a5, a15
    3206:	215850        	srai	a5, a5, 8
    3209:	55aa           	add.n	a5, a5, a10
    320b:	0002c6        	j	321a <Voice_PostEC_Prcs+0xb6>

0000320e <Voice_PostEC_Prcs+0xaa>:
    320e:	46d252        	addmi	a5, a2, 0x4600
    3211:	6f9552        	l16si	a5, a5, 222
    3214:	000086        	j	321a <Voice_PostEC_Prcs+0xb6>

00003217 <Voice_PostEC_Prcs+0xb3>:
    3217:	6e9952        	l16si	a5, a9, 220
    321a:	170c           	movi.n	a7, 1
    321c:	117710        	slli	a7, a7, 15
    321f:	c07750        	sub	a7, a7, a5
    3222:	d8a0c2        	movi	a12, 216
    3225:	6da4d2        	movi	a13, 0x46d
    3228:	300574        	ae_cvtp24a16x2.ll	aep0, a5, a7
    322b:	117dc0        	slli	a7, a13, 4
    322e:	46dc52        	addmi	a5, a12, 0x4600
    3231:	602254        	ae_lq32f.x	aeq0, a2, a5
    3234:	607724        	ae_lq32f.xu	aeq1, a7, a2
    3237:	0c0100ccef 	{ movi	a14, 204; ae_mulzaafq32sp16s.lh	aeq0, aeq1, aep0, aeq0, aep0 }
    323c:	457234        	ae_lp16x2f.x	aep7, a2, a3
    323f:	46de32        	addmi	a3, a14, 0x4600
    3242:	fc447c432f 	{ ae_lq32f.xu	aeq2, a3, a2; ae_roundsq32sym	aeq0, aeq0 }
    3247:	46d252        	addmi	a5, a2, 0x4600
    324a:	0cfe6645fe 	{ l16si	a15, a5, 200; ae_mulzaafq32sp16s.lh	aeq1, aeq0, aep7, aeq2, aep7 }
    324f:	f3a921        	l32r	a2, f4 (4630 <airoha_divide>)
    3252:	ff2c79870f 	{ ae_sq32f.i	aeq0, a7, 0; ae_roundsq32asym	aeq3, aeq1 }
    3257:	36ba04        	ae_trunca32q48	a10, aeq3
    325a:	11bf00        	slli	a11, a15, 16
    325d:	03a9           	s32i.n	a10, a3, 0
    325f:	0002e0        	callx8	a2
    3262:	360b34        	ae_cvtq48a32s	aeq0, a11
    3265:	fecae2        	addi	a14, a10, -2
    3268:	fc24fe8e0f 	{ ae_slaasq56s	aeq0, aeq0, a14; nop }
    326d:	f863b0063e 	{ l32i	a3, a6, 0; ae_roundsp16q48asym	aep0, aeq0 }
    3272:	360f24        	ae_trunca16p24s.l	a15, aep0
    3275:	020c           	movi.n	a2, 0
    3277:	1d0c           	movi.n	a13, 1
    3279:	659552        	l16si	a5, a5, 202
    327c:	742d           	ae_s16i.n	a2, a4, 2
    327e:	112d30        	slli	a2, a13, 13
    3281:	29d332        	addmi	a3, a3, 0x2900
    3284:	532f20        	max	a2, a15, a2
    3287:	233d           	ae_l16si.n	a3, a3, 0
    3289:	432250        	min	a2, a2, a5
    328c:	643d           	ae_s16i.n	a3, a4, 0
    328e:	025422        	s16i	a2, a4, 4
    3291:	f01d           	retw.n

00003293 <Voice_PostEC_Prcs+0x12f>:
	...

00003294 <PostEC_Info>:
    3294:	004136        	entry	a1, 32
    3297:	f36231        	l32r	a3, 20 (8688 <p_ecnr_mem>)
    329a:	ffa062        	movi	a6, 255
    329d:	040c           	movi.n	a4, 0
    329f:	0338           	l32i.n	a3, a3, 0
    32a1:	116690        	slli	a6, a6, 7
    32a4:	150c           	movi.n	a5, 1
    32a6:	29d3d2        	addmi	a13, a3, 0x2900
    32a9:	161de2        	l16ui	a14, a13, 44
    32ac:	724d           	ae_s16i.n	a4, a2, 2
    32ae:	2d4d           	ae_l16si.n	a4, a13, 0
    32b0:	10fe60        	and	a15, a14, a6
    32b3:	115530        	slli	a5, a5, 13
    32b6:	624d           	ae_s16i.n	a4, a2, 0
    32b8:	2bd332        	addmi	a3, a3, 0x2b00
    32bb:	7fcf42        	addi	a4, a15, 127
    32be:	025252        	s16i	a5, a2, 4
    32c1:	7427e0        	extui	a2, a14, 7, 8
    32c4:	245342        	s16i	a4, a3, 72
    32c7:	f01d           	retw.n

000032c9 <PostEC_Info+0x35>:
    32c9:	000000                                        ...

000032cc <Voice_EC_Prcs>:
    32cc:	006136        	entry	a1, 48
    32cf:	f3bf81        	l32r	a8, 1cc (1eb8 <Voice_EC_Prcs_common>)
    32d2:	070c           	movi.n	a7, 0
    32d4:	0c0c           	movi.n	a12, 0
    32d6:	02ad           	mov.n	a10, a2
    32d8:	03bd           	mov.n	a11, a3
    32da:	04dd           	mov.n	a13, a4
    32dc:	05ed           	mov.n	a14, a5
    32de:	06fd           	mov.n	a15, a6
    32e0:	0179           	s32i.n	a7, a1, 0
    32e2:	0008e0        	callx8	a8
    32e5:	f01d           	retw.n

000032e7 <Voice_EC_Prcs+0x1b>:
	...

000032e8 <Voice_EC_Inear_Prcs>:
    32e8:	006136        	entry	a1, 48
    32eb:	f3b881        	l32r	a8, 1cc (1eb8 <Voice_EC_Prcs_common>)
    32ee:	c198           	l32i.n	a9, a1, 48
    32f0:	02ad           	mov.n	a10, a2
    32f2:	03bd           	mov.n	a11, a3
    32f4:	04cd           	mov.n	a12, a4
    32f6:	05dd           	mov.n	a13, a5
    32f8:	06ed           	mov.n	a14, a6
    32fa:	07fd           	mov.n	a15, a7
    32fc:	0199           	s32i.n	a9, a1, 0
    32fe:	0008e0        	callx8	a8
    3301:	f01d           	retw.n

00003303 <Voice_EC_Inear_Prcs+0x1b>:
	...

00003304 <Voice_NB_RX_Prcs>:
    3304:	004136        	entry	a1, 32
    3307:	f34631        	l32r	a3, 20 (8688 <p_ecnr_mem>)
    330a:	03a8           	l32i.n	a10, a3, 0
    330c:	2bda42        	addmi	a4, a10, 0x2b00
    330f:	251442        	l16ui	a4, a4, 74
    3312:	848c           	beqz.n	a4, 331e <Voice_NB_RX_Prcs+0x1a>
    3314:	f3af41        	l32r	a4, 1d0 (3978 <NB_RX_NR_frame_process>)
    3317:	02ad           	mov.n	a10, a2
    3319:	0004e0        	callx8	a4
    331c:	03a8           	l32i.n	a10, a3, 0

0000331e <Voice_NB_RX_Prcs+0x1a>:
    331e:	45da32        	addmi	a3, a10, 0x4500
    3321:	12da42        	addmi	a4, a10, 0x1200
    3324:	349332        	l16si	a3, a3, 104
    3327:	285432        	s16i	a3, a4, 80
    332a:	550432        	l8ui	a3, a4, 85
    332d:	3ca052        	movi	a5, 60
    3330:	f8ca42        	addi	a4, a10, -8
    3333:	266357        	bbci	a3, 5, 335d <Voice_NB_RX_Prcs+0x59>
    3336:	fcc232        	addi	a3, a2, -4
    3339:	058576        	loop	a5, 3342 <Voice_NB_RX_Prcs+0x3e>
    333c:	448314        	ae_lp16x2f.iu	aep0, a3, 4
    333f:	5e8414        	ae_sp24x2f.iu	aep0, a4, 8

00003342 <Voice_NB_RX_Prcs+0x3e>:
    3342:	4da232        	movi	a3, 0x24d
    3345:	33a242        	movi	a4, 0x233
    3348:	1133c0        	slli	a3, a3, 4
    334b:	1144b0        	slli	a4, a4, 5
    334e:	ba3a           	add.n	a11, a10, a3
    3350:	f38031        	l32r	a3, 150 (1b60 <biquad_2cascade_24bitTx>)
    3353:	ca4a           	add.n	a12, a10, a4
    3355:	78a0e2        	movi	a14, 120
    3358:	02dd           	mov.n	a13, a2
    335a:	0003e0        	callx8	a3

0000335d <Voice_NB_RX_Prcs+0x59>:
    335d:	f01d           	retw.n

0000335f <Voice_NB_RX_Prcs+0x5b>:
	...

00003360 <Voice_WB_RX_Prcs>:
    3360:	004136        	entry	a1, 32
    3363:	f32f31        	l32r	a3, 20 (8688 <p_ecnr_mem>)
    3366:	03a8           	l32i.n	a10, a3, 0
    3368:	2bda42        	addmi	a4, a10, 0x2b00
    336b:	251442        	l16ui	a4, a4, 74
    336e:	848c           	beqz.n	a4, 337a <Voice_WB_RX_Prcs+0x1a>
    3370:	f39941        	l32r	a4, 1d4 (36f4 <WB_RX_NR_frame_process>)
    3373:	02ad           	mov.n	a10, a2
    3375:	0004e0        	callx8	a4
    3378:	03a8           	l32i.n	a10, a3, 0

0000337a <Voice_WB_RX_Prcs+0x1a>:
    337a:	45da32        	addmi	a3, a10, 0x4500
    337d:	12da42        	addmi	a4, a10, 0x1200
    3380:	349332        	l16si	a3, a3, 104
    3383:	285432        	s16i	a3, a4, 80
    3386:	550432        	l8ui	a3, a4, 85
    3389:	78a052        	movi	a5, 120
    338c:	f8ca42        	addi	a4, a10, -8
    338f:	266357        	bbci	a3, 5, 33b9 <Voice_WB_RX_Prcs+0x59>
    3392:	fcc232        	addi	a3, a2, -4
    3395:	058576        	loop	a5, 339e <Voice_WB_RX_Prcs+0x3e>
    3398:	448314        	ae_lp16x2f.iu	aep0, a3, 4
    339b:	5e8414        	ae_sp24x2f.iu	aep0, a4, 8

0000339e <Voice_WB_RX_Prcs+0x3e>:
    339e:	4da232        	movi	a3, 0x24d
    33a1:	33a242        	movi	a4, 0x233
    33a4:	1133c0        	slli	a3, a3, 4
    33a7:	1144b0        	slli	a4, a4, 5
    33aa:	ba3a           	add.n	a11, a10, a3
    33ac:	f36931        	l32r	a3, 150 (1b60 <biquad_2cascade_24bitTx>)
    33af:	ca4a           	add.n	a12, a10, a4
    33b1:	f0a0e2        	movi	a14, 240
    33b4:	02dd           	mov.n	a13, a2
    33b6:	0003e0        	callx8	a3

000033b9 <Voice_WB_RX_Prcs+0x59>:
    33b9:	f01d           	retw.n

000033bb <Voice_WB_RX_Prcs+0x5b>:
	...

000033bc <EC_REF_GAIN_READBACK>:
    33bc:	004136        	entry	a1, 32
    33bf:	f31831        	l32r	a3, 20 (8688 <p_ecnr_mem>)
    33c2:	040c           	movi.n	a4, 0
    33c4:	0338           	l32i.n	a3, a3, 0
    33c6:	29d362        	addmi	a6, a3, 0x2900
    33c9:	29d372        	addmi	a7, a3, 0x2900
    33cc:	2ad382        	addmi	a8, a3, 0x2a00
    33cf:	2d1652        	l16ui	a5, a6, 90
    33d2:	4d1662        	l16ui	a6, a6, 154
    33d5:	6d1772        	l16ui	a7, a7, 218
    33d8:	0d1882        	l16ui	a8, a8, 26
    33db:	625d           	ae_s16i.n	a5, a2, 0
    33dd:	726d           	ae_s16i.n	a6, a2, 2
    33df:	025272        	s16i	a7, a2, 4
    33e2:	12d3f2        	addmi	a15, a3, 0x1200
    33e5:	035282        	s16i	a8, a2, 6
    33e8:	261f82        	l16ui	a8, a15, 76
    33eb:	050c           	movi.n	a5, 0
    33ed:	060c           	movi.n	a6, 0
    33ef:	070c           	movi.n	a7, 0
    33f1:	142866        	bnei	a8, 2, 3409 <EC_REF_GAIN_READBACK+0x4d>
    33f4:	2ad352        	addmi	a5, a3, 0x2a00
    33f7:	2ad362        	addmi	a6, a3, 0x2a00
    33fa:	2bd3f2        	addmi	a15, a3, 0x2b00
    33fd:	2d1542        	l16ui	a4, a5, 90
    3400:	4d1552        	l16ui	a5, a5, 154
    3403:	6d1662        	l16ui	a6, a6, 218
    3406:	0d1f72        	l16ui	a7, a15, 26

00003409 <EC_REF_GAIN_READBACK+0x4d>:
    3409:	045242        	s16i	a4, a2, 8
    340c:	055252        	s16i	a5, a2, 10
    340f:	065262        	s16i	a6, a2, 12
    3412:	075272        	s16i	a7, a2, 14
    3415:	f01d           	retw.n

00003417 <EC_REF_GAIN_READBACK+0x5b>:
	...

00003418 <EC_Inear_REF_GAIN_READBACK>:
    3418:	004136        	entry	a1, 32
    341b:	f30131        	l32r	a3, 20 (8688 <p_ecnr_mem>)
    341e:	0338           	l32i.n	a3, a3, 0
    3420:	46d332        	addmi	a3, a3, 0x4600
    3423:	3a2332        	l32i	a3, a3, 232
    3426:	611342        	l16ui	a4, a3, 194
    3429:	8113e2        	l16ui	a14, a3, 0x102
    342c:	a113f2        	l16ui	a15, a3, 0x142
    342f:	c11332        	l16ui	a3, a3, 0x182
    3432:	624d           	ae_s16i.n	a4, a2, 0
    3434:	72ed           	ae_s16i.n	a14, a2, 2
    3436:	0252f2        	s16i	a15, a2, 4
    3439:	035232        	s16i	a3, a2, 6
    343c:	f01d           	retw.n

0000343e <EC_Inear_REF_GAIN_READBACK+0x26>:
	...

00003440 <EC_PreLim_Coef_READBACK>:
    3440:	004136        	entry	a1, 32
    3443:	f2f731        	l32r	a3, 20 (8688 <p_ecnr_mem>)
    3446:	c50c           	movi.n	a5, 12
    3448:	25d552        	addmi	a5, a5, 0x2500
    344b:	0338           	l32i.n	a3, a3, 0
    344d:	360c           	movi.n	a6, 3
    344f:	024d           	mov.n	a4, a2
    3451:	535a           	add.n	a5, a3, a5
    3453:	078676        	loop	a6, 345e <EC_PreLim_Coef_READBACK+0x1e>
    3456:	556d           	ae_l16ui.n	a6, a5, 2
    3458:	646d           	ae_s16i.n	a6, a4, 0
    345a:	554b           	addi.n	a5, a5, 4
    345c:	442b           	addi.n	a4, a4, 2

0000345e <EC_PreLim_Coef_READBACK+0x1e>:
    345e:	ada452        	movi	a5, 0x4ad
    3461:	1155d0        	slli	a5, a5, 3
    3464:	360c           	movi.n	a6, 3
    3466:	426b           	addi.n	a4, a2, 6
    3468:	535a           	add.n	a5, a3, a5
    346a:	078676        	loop	a6, 3475 <EC_PreLim_Coef_READBACK+0x35>
    346d:	556d           	ae_l16ui.n	a6, a5, 2
    346f:	646d           	ae_s16i.n	a6, a4, 0
    3471:	554b           	addi.n	a5, a5, 4
    3473:	442b           	addi.n	a4, a4, 2
    3475:	b44c           	movi.n	a4, 75
    3477:	114490        	slli	a4, a4, 7
    347a:	350c           	movi.n	a5, 3
    347c:	334a           	add.n	a3, a3, a4
    347e:	0cc222        	addi	a2, a2, 12
    3481:	078576        	loop	a5, 348c <EC_PreLim_Coef_READBACK+0x4c>
    3484:	534d           	ae_l16ui.n	a4, a3, 2
    3486:	624d           	ae_s16i.n	a4, a2, 0
    3488:	334b           	addi.n	a3, a3, 4
    348a:	222b           	addi.n	a2, a2, 2
    348c:	f01d           	retw.n

0000348e <EC_PreLim_Coef_READBACK+0x4e>:
	...

00003490 <DaulMIC_power_Coef_READBACK>:
    3490:	004136        	entry	a1, 32
    3493:	f2e321        	l32r	a2, 20 (8688 <p_ecnr_mem>)
    3496:	0228           	l32i.n	a2, a2, 0
    3498:	26d222        	addmi	a2, a2, 0x2600
    349b:	3a9222        	l16si	a2, a2, 116
    349e:	f01d           	retw.n

000034a0 <PF_GAIN_FB_READBACK>:
    34a0:	004136        	entry	a1, 32
    34a3:	f2df21        	l32r	a2, 20 (8688 <p_ecnr_mem>)
    34a6:	ffa032        	movi	a3, 255
    34a9:	0228           	l32i.n	a2, a2, 0
    34ab:	46d222        	addmi	a2, a2, 0x4600
    34ae:	3a2222        	l32i	a2, a2, 232
    34b1:	728c           	beqz.n	a2, 34bc <PF_GAIN_FB_READBACK+0x1c>
    34b3:	04d222        	addmi	a2, a2, 0x400
    34b6:	251222        	l16ui	a2, a2, 74
    34b9:	000086        	j	34bf <PF_GAIN_FB_READBACK+0x1f>

000034bc <PF_GAIN_FB_READBACK+0x1c>:
    34bc:	7fd322        	addmi	a2, a3, 0x7f00
    34bf:	822d           	ae_sext16	a2, a2
    34c1:	f01d           	retw.n

000034c3 <PF_GAIN_FB_READBACK+0x23>:
    34c3:	00000000                                 .....

000034c8 <fill_EQ_gain>:
    34c8:	004136        	entry	a1, 32
    34cb:	0a14a6        	blti	a4, 1, 34d9 <fill_EQ_gain+0x11>
    34ce:	078476        	loop	a4, 34d9 <fill_EQ_gain+0x11>
    34d1:	234d           	ae_l16si.n	a4, a3, 0
    34d3:	624d           	ae_s16i.n	a4, a2, 0
    34d5:	332b           	addi.n	a3, a3, 2
    34d7:	222b           	addi.n	a2, a2, 2

000034d9 <fill_EQ_gain+0x11>:
    34d9:	f01d           	retw.n

000034db <fill_EQ_gain+0x13>:
	...

000034dc <WB_NR_init>:
    34dc:	004136        	entry	a1, 32
    34df:	f2fa61        	l32r	a6, c8 (6bf0 <xt_memset>)
    34e2:	0b0c           	movi.n	a11, 0
    34e4:	8ea0c2        	movi	a12, 142
    34e7:	02ad           	mov.n	a10, a2
    34e9:	070c           	movi.n	a7, 0
    34eb:	0006e0        	callx8	a6
    34ee:	f2cc51        	l32r	a5, 20 (8688 <p_ecnr_mem>)
    34f1:	342326        	beqi	a3, 2, 3529 <WB_NR_init+0x4d>
    34f4:	0d4356        	bnez	a3, 35cc <WB_NR_init+0xf0>
    34f7:	f33861        	l32r	a6, 1d8 (6778 <common_noise_gen_init>)
    34fa:	02ad           	mov.n	a10, a2
    34fc:	04bd           	mov.n	a11, a4
    34fe:	0006e0        	callx8	a6
    3501:	0578           	l32i.n	a7, a5, 0
    3503:	f9a162        	movi	a6, 0x1f9
    3506:	e9a182        	movi	a8, 0x1e9
    3509:	893c           	movi.n	a9, 56
    350b:	1188b0        	slli	a8, a8, 5
    350e:	1166b0        	slli	a6, a6, 5
    3511:	44d992        	addmi	a9, a9, 0x4400
    3514:	a76a           	add.n	a10, a7, a6
    3516:	878a           	add.n	a8, a7, a8
    3518:	679a           	add.n	a6, a7, a9
    351a:	3fd772        	addmi	a7, a7, 0x3f00
    351d:	3e6272        	s32i	a7, a2, 248
    3520:	3f62a2        	s32i	a10, a2, 252
    3523:	406282        	s32i	a8, a2, 0x100
    3526:	005b06        	j	3696 <WB_NR_init+0x1ba>

00003529 <WB_NR_init+0x4d>:
    3529:	501492        	l16ui	a9, a4, 160
    352c:	2d9482        	l16si	a8, a4, 90
    352f:	bc0c           	movi.n	a12, 11
    3531:	105282        	s16i	a8, a2, 32
    3534:	d8a0f2        	movi	a15, 216
    3537:	2c94b2        	l16si	a11, a4, 88
    353a:	f32881        	l32r	a8, 1dc (6764 <complement_para>)
    353d:	2494e2        	l16si	a14, a4, 72
    3540:	93c990        	movnez	a12, a9, a9
    3543:	a2fa           	add.n	a10, a2, a15
    3545:	0352e2        	s16i	a14, a2, 6
    3548:	0252c2        	s16i	a12, a2, 4
    354b:	0008e0        	callx8	a8
    354e:	f32481        	l32r	a8, 1e0 (6740 <fill_vad_thrd_Nbands>)
    3551:	a28b           	addi.n	a10, a2, 8
    3553:	70c4b2        	addi	a11, a4, 112
    3556:	4c0c           	movi.n	a12, 4
    3558:	0008e0        	callx8	a8
    355b:	0598           	l32i.n	a9, a5, 0
    355d:	fda0a2        	movi	a10, 253
    3560:	f32181        	l32r	a8, 1e4 (7f40 <win_512_v3_nc>)
    3563:	11aaa0        	slli	a10, a10, 6
    3566:	4f5c           	movi.n	a15, 84
    3568:	3d6282        	s32i	a8, a2, 244
    356b:	b9aa           	add.n	a11, a9, a10
    356d:	2fdf82        	addmi	a8, a15, 0x2f00
    3570:	a98a           	add.n	a10, a9, a8
    3572:	3e62b2        	s32i	a11, a2, 248
    3575:	0b0c           	movi.n	a11, 0
    3577:	eea0c2        	movi	a12, 238
    357a:	4262a2        	s32i	a10, a2, 0x108
    357d:	0006e0        	callx8	a6
    3580:	dca662        	movi	a6, 0x6dc
    3583:	f06660        	subx8	a6, a6, a6
    3586:	175262        	s16i	a6, a2, 46
    3589:	422262        	l32i	a6, a2, 0x108
    358c:	0588           	l32i.n	a8, a5, 0
    358e:	27a292        	movi	a9, 0x227
    3591:	1199b0        	slli	a9, a9, 5
    3594:	40a3f2        	movi	a15, 0x340
    3597:	667d           	ae_s16i.n	a7, a6, 0
    3599:	f2a071        	l32r	a7, 1c (6c04 <xt_memcpy>)
    359c:	889a           	add.n	a8, a8, a9
    359e:	b4fa           	add.n	a11, a4, a15
    35a0:	a62b           	addi.n	a10, a6, 2
    35a2:	3c0c           	movi.n	a12, 3
    35a4:	c289           	s32i.n	a8, a2, 48
    35a6:	0007e0        	callx8	a7
    35a9:	986c           	movi.n	a8, -23
    35ab:	f30f71        	l32r	a7, 1e8 (ffe9ffe9 <_end+0xffe9795d>)
    35ae:	f48080        	extui	a8, a8, 0, 16
    35b1:	44a1d2        	movi	a13, 0x144
    35b4:	6e2c           	movi.n	a14, 38
    35b6:	1f0c           	movi.n	a15, 1
    35b8:	3679           	s32i.n	a7, a6, 12
    35ba:	085682        	s16i	a8, a6, 16
    35bd:	66da           	add.n	a6, a6, a13
    35bf:	017f20        	slli	a7, a15, 30
    35c2:	038e76        	loop	a14, 35c9 <WB_NR_init+0xed>
    35c5:	0679           	s32i.n	a7, a6, 0
    35c7:	664b           	addi.n	a6, a6, 4

000035c9 <WB_NR_init+0xed>:
    35c9:	0032c6        	j	3698 <WB_NR_init+0x1bc>

000035cc <WB_NR_init+0xf0>:
    35cc:	0578           	l32i.n	a7, a5, 0
    35ce:	0ba282        	movi	a8, 0x20b
    35d1:	1188b0        	slli	a8, a8, 5
    35d4:	4d9492        	l16si	a9, a4, 154
    35d7:	4294c2        	l16si	a12, a4, 132
    35da:	4394d2        	l16si	a13, a4, 134
    35dd:	4494e2        	l16si	a14, a4, 136
    35e0:	778a           	add.n	a7, a7, a8
    35e2:	105292        	s16i	a9, a2, 32
    35e5:	0252c2        	s16i	a12, a2, 4
    35e8:	1152d2        	s16i	a13, a2, 34
    35eb:	0352e2        	s16i	a14, a2, 6
    35ee:	396272        	s32i	a7, a2, 228
    35f1:	252472        	l32i	a7, a4, 148
    35f4:	7279           	s32i.n	a7, a2, 28
    35f6:	d8a0f2        	movi	a15, 216
    35f9:	4c94b2        	l16si	a11, a4, 152
    35fc:	f2f871        	l32r	a7, 1dc (6764 <complement_para>)
    35ff:	a2fa           	add.n	a10, a2, a15
    3601:	0007e0        	callx8	a7
    3604:	8aa072        	movi	a7, 138
    3607:	b47a           	add.n	a11, a4, a7
    3609:	f2f571        	l32r	a7, 1e0 (6740 <fill_vad_thrd_Nbands>)
    360c:	a28b           	addi.n	a10, a2, 8
    360e:	5c0c           	movi.n	a12, 5
    3610:	0007e0        	callx8	a7
    3613:	392272        	l32i	a7, a2, 228
    3616:	46a382        	movi	a8, 0x346
    3619:	00a192        	movi	a9, 0x100
    361c:	848a           	add.n	a8, a4, a8
    361e:	078976        	loop	a9, 3629 <WB_NR_init+0x14d>
    3621:	289d           	ae_l16si.n	a9, a8, 0
    3623:	679d           	ae_s16i.n	a9, a7, 0
    3625:	882b           	addi.n	a8, a8, 2
    3627:	772b           	addi.n	a7, a7, 2

00003629 <WB_NR_init+0x14d>:
    3629:	0588           	l32i.n	a8, a5, 0
    362b:	c5a192        	movi	a9, 0x1c5
    362e:	1199b0        	slli	a9, a9, 5
    3631:	2c02a2        	l8ui	a10, a2, 44
    3634:	989a           	add.n	a9, a8, a9
    3636:	9da6d2        	movi	a13, 0x69d
    3639:	f2ec71        	l32r	a7, 1ec (7bd0 <win_rx_512_v3>)
    363c:	3e6292        	s32i	a9, a2, 248
    363f:	119dd0        	slli	a9, a13, 3
    3642:	1b0c           	movi.n	a11, 1
    3644:	eba0c2        	movi	a12, 235
    3647:	3d6272        	s32i	a7, a2, 244
    364a:	207ab0        	or	a7, a10, a11
    364d:	11aca0        	slli	a10, a12, 6
    3650:	989a           	add.n	a9, a8, a9
    3652:	d8a7e2        	movi	a14, 0x7d8
    3655:	a8aa           	add.n	a10, a8, a10
    3657:	406292        	s32i	a9, a2, 0x100
    365a:	b09ee0        	addx8	a9, a14, a14
    365d:	3f62a2        	s32i	a10, a2, 252
    3660:	a89a           	add.n	a10, a8, a9
    3662:	4e94f2        	l16si	a15, a4, 156
    3665:	46d882        	addmi	a8, a8, 0x4600
    3668:	4a58f2        	s16i	a15, a8, 148
    366b:	2c4272        	s8i	a7, a2, 44
    366e:	4f9472        	l16si	a7, a4, 158
    3671:	0b0c           	movi.n	a11, 0
    3673:	6c0c           	movi.n	a12, 6
    3675:	4b5872        	s16i	a7, a8, 150
    3678:	0006e0        	callx8	a6
    367b:	0568           	l32i.n	a6, a5, 0
    367d:	13a382        	movi	a8, 0x313
    3680:	1188c0        	slli	a8, a8, 4
    3683:	f2db71        	l32r	a7, 1f0 (ee09 <_end+0x677d>)
    3686:	1ba2f2        	movi	a15, 0x21b
    3689:	175272        	s16i	a7, a2, 46
    368c:	868a           	add.n	a8, a6, a8
    368e:	117fb0        	slli	a7, a15, 5
    3691:	667a           	add.n	a6, a6, a7
    3693:	416282        	s32i	a8, a2, 0x104

00003696 <WB_NR_init+0x1ba>:
    3696:	c269           	s32i.n	a6, a2, 48
    3698:	f2d761        	l32r	a6, 1f4 (67d4 <common_NR_init>)
    369b:	02ad           	mov.n	a10, a2
    369d:	04bd           	mov.n	a11, a4
    369f:	03cd           	mov.n	a12, a3
    36a1:	0006e0        	callx8	a6
    36a4:	0538           	l32i.n	a3, a5, 0
    36a6:	1cd332        	addmi	a3, a3, 0x1c00
    36a9:	3a6232        	s32i	a3, a2, 232
    36ac:	f01d           	retw.n

000036ae <WB_NR_init+0x1d2>:
	...

000036b0 <WB_RX_NR_init>:
    36b0:	004136        	entry	a1, 32
    36b3:	f25b51        	l32r	a5, 20 (8688 <p_ecnr_mem>)
    36b6:	0a0c           	movi.n	a10, 0
    36b8:	03bd           	mov.n	a11, a3
    36ba:	0529           	s32i.n	a2, a5, 0
    36bc:	f2cf21        	l32r	a2, 1f8 (3ec <RX_ECNR_PARA_init>)
    36bf:	04cd           	mov.n	a12, a4
    36c1:	0002e0        	callx8	a2
    36c4:	f25821        	l32r	a2, 24 (8680 <aec_nr_para>)
    36c7:	0538           	l32i.n	a3, a5, 0
    36c9:	33a242        	movi	a4, 0x233
    36cc:	0268           	l32i.n	a6, a2, 0
    36ce:	1144b0        	slli	a4, a4, 5
    36d1:	a34a           	add.n	a10, a3, a4
    36d3:	34a1f2        	movi	a15, 0x134
    36d6:	f28e31        	l32r	a3, 110 (13f0 <HP_coef_setup>)
    36d9:	b6fa           	add.n	a11, a6, a15
    36db:	0003e0        	callx8	a3
    36de:	0538           	l32i.n	a3, a5, 0
    36e0:	15a142        	movi	a4, 0x115
    36e3:	1144a0        	slli	a4, a4, 6
    36e6:	02c8           	l32i.n	a12, a2, 0
    36e8:	f2c521        	l32r	a2, 1fc (34dc <WB_NR_init>)
    36eb:	a34a           	add.n	a10, a3, a4
    36ed:	1b0c           	movi.n	a11, 1
    36ef:	0002e0        	callx8	a2
    36f2:	f01d           	retw.n

000036f4 <WB_RX_NR_frame_process>:
    36f4:	004136        	entry	a1, 32
    36f7:	f24a31        	l32r	a3, 20 (8688 <p_ecnr_mem>)
    36fa:	94a052        	movi	a5, 148
    36fd:	d8a762        	movi	a6, 0x7d8
    3700:	0348           	l32i.n	a4, a3, 0
    3702:	46d552        	addmi	a5, a5, 0x4600
    3705:	b06660        	addx8	a6, a6, a6
    3708:	b45a           	add.n	a11, a4, a5
    370a:	c46a           	add.n	a12, a4, a6
    370c:	f29741        	l32r	a4, 168 (19a4 <emph_filter_N>)
    370f:	f0a0d2        	movi	a13, 240
    3712:	02ad           	mov.n	a10, a2
    3714:	0004e0        	callx8	a4
    3717:	0348           	l32i.n	a4, a3, 0
    3719:	15a162        	movi	a6, 0x115
    371c:	1166a0        	slli	a6, a6, 6
    371f:	a46a           	add.n	a10, a4, a6
    3721:	f2b741        	l32r	a4, 200 (3744 <WB_NR_frame_process>)
    3724:	02bd           	mov.n	a11, a2
    3726:	02cd           	mov.n	a12, a2
    3728:	0004e0        	callx8	a4
    372b:	0338           	l32i.n	a3, a3, 0
    372d:	9ca042        	movi	a4, 156
    3730:	46d442        	addmi	a4, a4, 0x4600
    3733:	b35a           	add.n	a11, a3, a5
    3735:	c34a           	add.n	a12, a3, a4
    3737:	f29c31        	l32r	a3, 1a8 (19ec <deemph_filter_N>)
    373a:	f0a0d2        	movi	a13, 240
    373d:	02ad           	mov.n	a10, a2
    373f:	0003e0        	callx8	a3
    3742:	f01d           	retw.n

00003744 <WB_NR_frame_process>:
    3744:	004136        	entry	a1, 32
    3747:	f2af51        	l32r	a5, 204 (3cb0 <fft_wo>)
    374a:	02ad           	mov.n	a10, a2
    374c:	03bd           	mov.n	a11, a3
    374e:	0005e0        	callx8	a5
    3751:	f2ad31        	l32r	a3, 208 (3e04 <NR_vad1>)
    3754:	02ad           	mov.n	a10, a2
    3756:	0003e0        	callx8	a3
    3759:	f2ac31        	l32r	a3, 20c (3f20 <NR_vad2>)
    375c:	02ad           	mov.n	a10, a2
    375e:	0003e0        	callx8	a3
    3761:	f2ab31        	l32r	a3, 210 (4098 <NR_NoiseEstimate>)
    3764:	02ad           	mov.n	a10, a2
    3766:	0003e0        	callx8	a3
    3769:	f2aa31        	l32r	a3, 214 (41bc <NR_NoiseWithGain>)
    376c:	02ad           	mov.n	a10, a2
    376e:	0003e0        	callx8	a3
    3771:	f2a931        	l32r	a3, 218 (4278 <NR_WienerFilter>)
    3774:	02ad           	mov.n	a10, a2
    3776:	0003e0        	callx8	a3
    3779:	f2a831        	l32r	a3, 21c (4504 <ifft_wo_single>)
    377c:	02ad           	mov.n	a10, a2
    377e:	04bd           	mov.n	a11, a4
    3780:	0003e0        	callx8	a3
    3783:	f01d           	retw.n

00003785 <WB_NR_frame_process+0x41>:
    3785:	000000                                        ...

00003788 <NB_NR_init>:
    3788:	004136        	entry	a1, 32
    378b:	f24f61        	l32r	a6, c8 (6bf0 <xt_memset>)
    378e:	0b0c           	movi.n	a11, 0
    3790:	8ea0c2        	movi	a12, 142
    3793:	02ad           	mov.n	a10, a2
    3795:	0006e0        	callx8	a6
    3798:	f22231        	l32r	a3, 20 (8688 <p_ecnr_mem>)
    379b:	f22241        	l32r	a4, 24 (8680 <aec_nr_para>)
    379e:	0ba272        	movi	a7, 0x20b
    37a1:	0358           	l32i.n	a5, a3, 0
    37a3:	0488           	l32i.n	a8, a4, 0
    37a5:	1177b0        	slli	a7, a7, 5
    37a8:	557a           	add.n	a5, a5, a7
    37aa:	3f98c2        	l16si	a12, a8, 126
    37ad:	3598d2        	l16si	a13, a8, 106
    37b0:	3698e2        	l16si	a14, a8, 108
    37b3:	3798f2        	l16si	a15, a8, 110
    37b6:	396252        	s32i	a5, a2, 228
    37b9:	1052c2        	s16i	a12, a2, 32
    37bc:	0252d2        	s16i	a13, a2, 4
    37bf:	1152e2        	s16i	a14, a2, 34
    37c2:	0352f2        	s16i	a15, a2, 6
    37c5:	1e2852        	l32i	a5, a8, 120
    37c8:	7259           	s32i.n	a5, a2, 28
    37ca:	d8a0a2        	movi	a10, 216
    37cd:	3e98b2        	l16si	a11, a8, 124
    37d0:	f28351        	l32r	a5, 1dc (6764 <complement_para>)
    37d3:	a2aa           	add.n	a10, a2, a10
    37d5:	0005e0        	callx8	a5
    37d8:	0478           	l32i.n	a7, a4, 0
    37da:	a28b           	addi.n	a10, a2, 8
    37dc:	4c0c           	movi.n	a12, 4
    37de:	70c7b2        	addi	a11, a7, 112
    37e1:	f27f71        	l32r	a7, 1e0 (6740 <fill_vad_thrd_Nbands>)
    37e4:	0007e0        	callx8	a7
    37e7:	0478           	l32i.n	a7, a4, 0
    37e9:	392282        	l32i	a8, a2, 228
    37ec:	46a392        	movi	a9, 0x346
    37ef:	80a0a2        	movi	a10, 128
    37f2:	809790        	add	a9, a7, a9
    37f5:	078a76        	loop	a10, 3800 <NB_NR_init+0x78>
    37f8:	29ad           	ae_l16si.n	a10, a9, 0
    37fa:	68ad           	ae_s16i.n	a10, a8, 0
    37fc:	992b           	addi.n	a9, a9, 2
    37fe:	882b           	addi.n	a8, a8, 2

00003800 <NB_NR_init+0x78>:
    3800:	0398           	l32i.n	a9, a3, 0
    3802:	c5a1a2        	movi	a10, 0x1c5
    3805:	11aab0        	slli	a10, a10, 5
    3808:	a9aa           	add.n	a10, a9, a10
    380a:	2c02b2        	l8ui	a11, a2, 44
    380d:	9da6c2        	movi	a12, 0x69d
    3810:	3e62a2        	s32i	a10, a2, 248
    3813:	11acd0        	slli	a10, a12, 3
    3816:	f28281        	l32r	a8, 220 (7df0 <win_rx_256_v3>)
    3819:	a9aa           	add.n	a10, a9, a10
    381b:	1e0c           	movi.n	a14, 1
    381d:	eba0f2        	movi	a15, 235
    3820:	d8a7d2        	movi	a13, 0x7d8
    3823:	3d6282        	s32i	a8, a2, 244
    3826:	208be0        	or	a8, a11, a14
    3829:	11bfa0        	slli	a11, a15, 6
    382c:	4062a2        	s32i	a10, a2, 0x100
    382f:	b0add0        	addx8	a10, a13, a13
    3832:	b9ba           	add.n	a11, a9, a11
    3834:	a9aa           	add.n	a10, a9, a10
    3836:	4097e2        	l16si	a14, a7, 128
    3839:	46d992        	addmi	a9, a9, 0x4600
    383c:	4a59e2        	s16i	a14, a9, 148
    383f:	4197f2        	l16si	a15, a7, 130
    3842:	3f62b2        	s32i	a11, a2, 252
    3845:	0b0c           	movi.n	a11, 0
    3847:	6c0c           	movi.n	a12, 6
    3849:	2c4282        	s8i	a8, a2, 44
    384c:	4b59f2        	s16i	a15, a9, 150
    384f:	0006e0        	callx8	a6
    3852:	0388           	l32i.n	a8, a3, 0
    3854:	13a392        	movi	a9, 0x313
    3857:	1ba2a2        	movi	a10, 0x21b
    385a:	1199c0        	slli	a9, a9, 4
    385d:	11aab0        	slli	a10, a10, 5
    3860:	a0a0b2        	movi	a11, 160
    3863:	777c           	movi.n	a7, -9
    3865:	c89a           	add.n	a12, a8, a9
    3867:	88aa           	add.n	a8, a8, a10
    3869:	760c           	movi.n	a6, 7
    386b:	7f1c           	movi.n	a15, 23
    386d:	017750        	slli	a7, a7, 27
    3870:	119f50        	slli	a9, a15, 11
    3873:	4162c2        	s32i	a12, a2, 0x104
    3876:	ca0c           	movi.n	a10, 12
    3878:	b2ba           	add.n	a11, a2, a11
    387a:	2e4262        	s8i	a6, a2, 46
    387d:	c289           	s32i.n	a8, a2, 48

0000387f <NB_NR_init+0xf7>:
    387f:	08cd           	mov.n	a12, a8
    3881:	0b79           	s32i.n	a7, a11, 0
    3883:	038a76        	loop	a10, 388a <NB_NR_init+0x102>
    3886:	6c9d           	ae_s16i.n	a9, a12, 0
    3888:	cc2b           	addi.n	a12, a12, 2

0000388a <NB_NR_init+0x102>:
    388a:	660b           	addi.n	a6, a6, -1
    388c:	18c882        	addi	a8, a8, 24
    388f:	bb4b           	addi.n	a11, a11, 4
    3891:	fea656        	bnez	a6, 387f <NB_NR_init+0xf7>
    3894:	0468           	l32i.n	a6, a4, 0
    3896:	c4a072        	movi	a7, 196
    3899:	a27a           	add.n	a10, a2, a7
    389b:	2e96b2        	l16si	a11, a6, 92
    389e:	0005e0        	callx8	a5
    38a1:	0468           	l32i.n	a6, a4, 0
    38a3:	c8a072        	movi	a7, 200
    38a6:	a27a           	add.n	a10, a2, a7
    38a8:	2f96b2        	l16si	a11, a6, 94
    38ab:	0005e0        	callx8	a5
    38ae:	0468           	l32i.n	a6, a4, 0
    38b0:	cca072        	movi	a7, 204
    38b3:	a27a           	add.n	a10, a2, a7
    38b5:	3096b2        	l16si	a11, a6, 96
    38b8:	0005e0        	callx8	a5
    38bb:	0468           	l32i.n	a6, a4, 0
    38bd:	d0a072        	movi	a7, 208
    38c0:	a27a           	add.n	a10, a2, a7
    38c2:	3196b2        	l16si	a11, a6, 98
    38c5:	0005e0        	callx8	a5
    38c8:	0468           	l32i.n	a6, a4, 0
    38ca:	d4a072        	movi	a7, 212
    38cd:	a27a           	add.n	a10, a2, a7
    38cf:	3296b2        	l16si	a11, a6, 100
    38d2:	0005e0        	callx8	a5
    38d5:	0448           	l32i.n	a4, a4, 0
    38d7:	78a052        	movi	a5, 120
    38da:	160c           	movi.n	a6, 1
    38dc:	0448           	l32i.n	a4, a4, 0
    38de:	0249           	s32i.n	a4, a2, 0
    38e0:	ffa782        	movi	a8, 0x7ff
    38e3:	9a6c           	movi.n	a10, -23
    38e5:	0338           	l32i.n	a3, a3, 0
    38e7:	2f4252        	s8i	a5, a2, 47
    38ea:	1158b0        	slli	a5, a8, 5
    38ed:	155262        	s16i	a6, a2, 42
    38f0:	f460a0        	extui	a6, a10, 0, 16
    38f3:	f7a1c2        	movi	a12, 0x1f7
    38f6:	f7a3d2        	movi	a13, 0x3f7
    38f9:	715252        	s16i	a5, a2, 226
    38fc:	115de0        	slli	a5, a13, 2
    38ff:	6f5262        	s16i	a6, a2, 222
    3902:	116cd0        	slli	a6, a12, 3
    3905:	646a           	add.n	a6, a4, a6
    3907:	545a           	add.n	a5, a4, a5
    3909:	402242        	l32i	a4, a2, 0x100
    390c:	00a192        	movi	a9, 0x100
    390f:	f207b1        	l32r	a11, 12c (7fff0000 <_end+0x7ffe7974>)
    3912:	1cd332        	addmi	a3, a3, 0x1c00
    3915:	78a0e2        	movi	a14, 120
    3918:	01a0f2        	movi	a15, 1
    391b:	125292        	s16i	a9, a2, 36
    391e:	4462b2        	s32i	a11, a2, 0x110
    3921:	3b6262        	s32i	a6, a2, 236
    3924:	3c6252        	s32i	a5, a2, 240
    3927:	3a6232        	s32i	a3, a2, 232
    392a:	012f20        	slli	a2, a15, 30
    392d:	038e76        	loop	a14, 3934 <NB_NR_init+0x1ac>
    3930:	0429           	s32i.n	a2, a4, 0
    3932:	444b           	addi.n	a4, a4, 4
    3934:	f01d           	retw.n

00003936 <NB_NR_init+0x1ae>:
	...

00003938 <NB_RX_NR_init>:
    3938:	004136        	entry	a1, 32
    393b:	f1b951        	l32r	a5, 20 (8688 <p_ecnr_mem>)
    393e:	1a0c           	movi.n	a10, 1
    3940:	03bd           	mov.n	a11, a3
    3942:	0529           	s32i.n	a2, a5, 0
    3944:	f22d21        	l32r	a2, 1f8 (3ec <RX_ECNR_PARA_init>)
    3947:	04cd           	mov.n	a12, a4
    3949:	0002e0        	callx8	a2
    394c:	f1b621        	l32r	a2, 24 (8680 <aec_nr_para>)
    394f:	0538           	l32i.n	a3, a5, 0
    3951:	33a242        	movi	a4, 0x233
    3954:	0228           	l32i.n	a2, a2, 0
    3956:	60a1f2        	movi	a15, 0x160
    3959:	1144b0        	slli	a4, a4, 5
    395c:	b2fa           	add.n	a11, a2, a15
    395e:	f1ec21        	l32r	a2, 110 (13f0 <HP_coef_setup>)
    3961:	a34a           	add.n	a10, a3, a4
    3963:	0002e0        	callx8	a2
    3966:	0528           	l32i.n	a2, a5, 0
    3968:	15a132        	movi	a3, 0x115
    396b:	1133a0        	slli	a3, a3, 6
    396e:	a23a           	add.n	a10, a2, a3
    3970:	f22d21        	l32r	a2, 224 (3788 <NB_NR_init>)
    3973:	0002e0        	callx8	a2
    3976:	f01d           	retw.n

00003978 <NB_RX_NR_frame_process>:
    3978:	004136        	entry	a1, 32
    397b:	f1a931        	l32r	a3, 20 (8688 <p_ecnr_mem>)
    397e:	94a052        	movi	a5, 148
    3981:	d8a762        	movi	a6, 0x7d8
    3984:	0348           	l32i.n	a4, a3, 0
    3986:	46d552        	addmi	a5, a5, 0x4600
    3989:	b06660        	addx8	a6, a6, a6
    398c:	b45a           	add.n	a11, a4, a5
    398e:	c46a           	add.n	a12, a4, a6
    3990:	f1f641        	l32r	a4, 168 (19a4 <emph_filter_N>)
    3993:	78a0d2        	movi	a13, 120
    3996:	02ad           	mov.n	a10, a2
    3998:	0004e0        	callx8	a4
    399b:	0348           	l32i.n	a4, a3, 0
    399d:	15a162        	movi	a6, 0x115
    39a0:	1166a0        	slli	a6, a6, 6
    39a3:	a46a           	add.n	a10, a4, a6
    39a5:	f22041        	l32r	a4, 228 (39c8 <NB_NR_frame_process>)
    39a8:	02bd           	mov.n	a11, a2
    39aa:	02cd           	mov.n	a12, a2
    39ac:	0004e0        	callx8	a4
    39af:	0338           	l32i.n	a3, a3, 0
    39b1:	9ca042        	movi	a4, 156
    39b4:	46d442        	addmi	a4, a4, 0x4600
    39b7:	b35a           	add.n	a11, a3, a5
    39b9:	c34a           	add.n	a12, a3, a4
    39bb:	f1fb31        	l32r	a3, 1a8 (19ec <deemph_filter_N>)
    39be:	78a0d2        	movi	a13, 120
    39c1:	02ad           	mov.n	a10, a2
    39c3:	0003e0        	callx8	a3
    39c6:	f01d           	retw.n

000039c8 <NB_NR_frame_process>:
    39c8:	004136        	entry	a1, 32
    39cb:	f20e51        	l32r	a5, 204 (3cb0 <fft_wo>)
    39ce:	02ad           	mov.n	a10, a2
    39d0:	03bd           	mov.n	a11, a3
    39d2:	0005e0        	callx8	a5
    39d5:	f20c31        	l32r	a3, 208 (3e04 <NR_vad1>)
    39d8:	02ad           	mov.n	a10, a2
    39da:	0003e0        	callx8	a3
    39dd:	f20b31        	l32r	a3, 20c (3f20 <NR_vad2>)
    39e0:	02ad           	mov.n	a10, a2
    39e2:	0003e0        	callx8	a3
    39e5:	f20a31        	l32r	a3, 210 (4098 <NR_NoiseEstimate>)
    39e8:	02ad           	mov.n	a10, a2
    39ea:	0003e0        	callx8	a3
    39ed:	f20931        	l32r	a3, 214 (41bc <NR_NoiseWithGain>)
    39f0:	02ad           	mov.n	a10, a2
    39f2:	0003e0        	callx8	a3
    39f5:	f20831        	l32r	a3, 218 (4278 <NR_WienerFilter>)
    39f8:	02ad           	mov.n	a10, a2
    39fa:	0003e0        	callx8	a3
    39fd:	f20731        	l32r	a3, 21c (4504 <ifft_wo_single>)
    3a00:	02ad           	mov.n	a10, a2
    3a02:	04bd           	mov.n	a11, a4
    3a04:	0003e0        	callx8	a3
    3a07:	f01d           	retw.n

00003a09 <NB_NR_frame_process+0x41>:
    3a09:	000000                                        ...

00003a0c <NOISE_GEN_init>:
    3a0c:	004136        	entry	a1, 32
    3a0f:	f18421        	l32r	a2, 20 (8688 <p_ecnr_mem>)
    3a12:	f18431        	l32r	a3, 24 (8680 <aec_nr_para>)
    3a15:	a3a542        	movi	a4, 0x5a3
    3a18:	0228           	l32i.n	a2, a2, 0
    3a1a:	1144d0        	slli	a4, a4, 3
    3a1d:	03c8           	l32i.n	a12, a3, 0
    3a1f:	a24a           	add.n	a10, a2, a4
    3a21:	f1f621        	l32r	a2, 1fc (34dc <WB_NR_init>)
    3a24:	0b0c           	movi.n	a11, 0
    3a26:	0002e0        	callx8	a2
    3a29:	f01d           	retw.n

00003a2b <NOISE_GEN_init+0x1f>:
	...

00003a2c <NC_NOISE_EST_init>:
    3a2c:	004136        	entry	a1, 32
    3a2f:	f17c31        	l32r	a3, 20 (8688 <p_ecnr_mem>)
    3a32:	c7a542        	movi	a4, 0x5c7
    3a35:	1144d0        	slli	a4, a4, 3
    3a38:	0338           	l32i.n	a3, a3, 0
    3a3a:	2b0c           	movi.n	a11, 2
    3a3c:	02cd           	mov.n	a12, a2
    3a3e:	a34a           	add.n	a10, a3, a4
    3a40:	f1ef31        	l32r	a3, 1fc (34dc <WB_NR_init>)
    3a43:	0003e0        	callx8	a3
    3a46:	f01d           	retw.n

00003a48 <NC_NOISE_EST_Prcs>:
    3a48:	006136        	entry	a1, 48
    3a4b:	c7a542        	movi	a4, 0x5c7
    3a4e:	1144d0        	slli	a4, a4, 3
    3a51:	424a           	add.n	a4, a2, a4
    3a53:	f1ec51        	l32r	a5, 204 (3cb0 <fft_wo>)
    3a56:	04ad           	mov.n	a10, a4
    3a58:	03bd           	mov.n	a11, a3
    3a5a:	0005e0        	callx8	a5
    3a5d:	f1ea31        	l32r	a3, 208 (3e04 <NR_vad1>)
    3a60:	04ad           	mov.n	a10, a4
    3a62:	0003e0        	callx8	a3
    3a65:	f1e931        	l32r	a3, 20c (3f20 <NR_vad2>)
    3a68:	04ad           	mov.n	a10, a4
    3a6a:	0003e0        	callx8	a3
    3a6d:	2fd232        	addmi	a3, a2, 0x2f00
    3a70:	a338           	l32i.n	a3, a3, 40
    3a72:	2fd242        	addmi	a4, a2, 0x2f00
    3a75:	102452        	l32i	a5, a4, 64
    3a78:	0368           	l32i.n	a6, a3, 0
    3a7a:	0d0c           	movi.n	a13, 0
    3a7c:	1116e6        	bgei	a6, 1, 3a91 <NC_NOISE_EST_Prcs+0x49>
    3a7f:	1348           	l32i.n	a4, a3, 4
    3a81:	0c14e6        	bgei	a4, 1, 3a91 <NC_NOISE_EST_Prcs+0x49>
    3a84:	2348           	l32i.n	a4, a3, 8
    3a86:	0714e6        	bgei	a4, 1, 3a91 <NC_NOISE_EST_Prcs+0x49>
    3a89:	3338           	l32i.n	a3, a3, 12
    3a8b:	0213e6        	bgei	a3, 1, 3a91 <NC_NOISE_EST_Prcs+0x49>
    3a8e:	0050c6        	j	3bd5 <NC_NOISE_EST_Prcs+0x18d>

00003a91 <NC_NOISE_EST_Prcs+0x49>:
    3a91:	1d0c           	movi.n	a13, 1
    3a93:	090532        	l8ui	a3, a5, 9
    3a96:	66a672        	movi	a7, 0x666
    3a99:	10b316        	beqz	a3, 3ba8 <NC_NOISE_EST_Prcs+0x160>
    3a9c:	070c           	movi.n	a7, 0
    3a9e:	0895c2        	l16si	a12, a5, 16
    3aa1:	061582        	l16ui	a8, a5, 12
    3aa4:	aca062        	movi	a6, 172
    3aa7:	44a1e2        	movi	a14, 0x144
    3aaa:	9cad           	ae_zext16	a10, a12
    3aac:	456a           	add.n	a4, a5, a6
    3aae:	35ea           	add.n	a3, a5, a14
    3ab0:	1d1a87        	beq	a10, a8, 3ad1 <NC_NOISE_EST_Prcs+0x89>
    3ab3:	0d6d           	mov.n	a6, a13
    3ab5:	88dd           	ae_sext16	a13, a8
    3ab7:	f1dd81        	l32r	a8, 22c (3c40 <equal_power>)
    3aba:	6e2c           	movi.n	a14, 38
    3abc:	04ad           	mov.n	a10, a4
    3abe:	03bd           	mov.n	a11, a3
    3ac0:	0008e0        	callx8	a8
    3ac3:	06dd           	mov.n	a13, a6
    3ac5:	44a1e2        	movi	a14, 0x144
    3ac8:	aca062        	movi	a6, 172
    3acb:	0655a2        	s16i	a10, a5, 12
    3ace:	0855a2        	s16i	a10, a5, 16
    3ad1:	fc2480019f 	{ movi	a9, 1; ae_zeroq56	aeq0 }
    3ad6:	119910        	slli	a9, a9, 15
    3ad9:	fd0455c97f 	{ sub	a12, a9, a7; ae_movq56	aeq1, aeq0 }
    3ade:	6b2c           	movi.n	a11, 38
    3ae0:	40a1f2        	movi	a15, 0x140
    3ae3:	60a564        	ae_lq32f.x	aeq2, a5, a6
    3ae6:	2ed282        	addmi	a8, a2, 0x2e00
    3ae9:	3007c4        	ae_cvtp24a16x2.ll	aep0, a7, a12
    3aec:	65fa           	add.n	a6, a5, a15
    3aee:	188b76        	loop	a11, 3b0a <NC_NOISE_EST_Prcs+0xc2>
    3af1:	60d614        	ae_lq32f.iu	aeq3, a6, 4
    3af4:	b78c           	beqz.n	a7, 3b03 <NC_NOISE_EST_Prcs+0xbb>
    3af6:	0f037d541f 	{ ae_lq32f.iu	aeq2, a4, 4; ae_mulzaafq32sp16s.lh	aeq3, aeq3, aep0, aeq2, aep0 }
    3afb:	f97f39b60f 	{ ae_sq32f.i	aeq3, a6, 0; ae_maxq56s	aeq1, aeq1, aeq3 }
    3b00:	000106        	j	3b08 <NC_NOISE_EST_Prcs+0xc0>

00003b03 <NC_NOISE_EST_Prcs+0xbb>:
    3b03:	f97f3d541f 	{ ae_lq32f.iu	aeq2, a4, 4; ae_maxq56s	aeq1, aeq1, aeq3 }
    3b08:	f03d           	nop.n

00003b0a <NC_NOISE_EST_Prcs+0xc2>:
    3b0a:	365404        	ae_nsaq56s	a4, aeq1
    3b0d:	960c           	movi.n	a6, 9
    3b0f:	44a192        	movi	a9, 0x144
    3b12:	1b2467        	blt	a4, a6, 3b31 <NC_NOISE_EST_Prcs+0xe9>
    3b15:	f8c442        	addi	a4, a4, -8
    3b18:	c0aa40        	sub	a10, a10, a4
    3b1b:	672c           	movi.n	a7, 38
    3b1d:	401400        	ssl	a4
    3b20:	459a           	add.n	a4, a5, a9
    3b22:	0655a2        	s16i	a10, a5, 12
    3b25:	088776        	loop	a7, 3b31 <NC_NOISE_EST_Prcs+0xe9>
    3b28:	0468           	l32i.n	a6, a4, 0
    3b2a:	a16600        	sll	a6, a6
    3b2d:	0469           	s32i.n	a6, a4, 0
    3b2f:	444b           	addi.n	a4, a4, 4

00003b31 <NC_NOISE_EST_Prcs+0xe9>:
    3b31:	e868           	l32i.n	a6, a8, 56
    3b33:	672c           	movi.n	a7, 38
    3b35:	054d           	mov.n	a4, a5
    3b37:	66cb           	addi.n	a6, a6, 12
    3b39:	0c8776        	loop	a7, 3b49 <NC_NOISE_EST_Prcs+0x101>
    3b3c:	5478           	l32i.n	a7, a4, 20
    3b3e:	2b6472        	s32i	a7, a4, 172
    3b41:	0678           	l32i.n	a7, a6, 0
    3b43:	5479           	s32i.n	a7, a4, 20
    3b45:	664b           	addi.n	a6, a6, 4
    3b47:	444b           	addi.n	a4, a4, 4

00003b49 <NC_NOISE_EST_Prcs+0x101>:
    3b49:	2fd272        	addmi	a7, a2, 0x2f00
    3b4c:	352d           	ae_l16si.n	a2, a5, 2
    3b4e:	080542        	l8ui	a4, a5, 8
    3b51:	079562        	l16si	a6, a5, 14
    3b54:	0a97f2        	l16si	a15, a7, 20
    3b57:	0845d2        	s8i	a13, a5, 8
    3b5a:	085562        	s16i	a6, a5, 16
    3b5d:	094542        	s8i	a4, a5, 9
    3b60:	0755f2        	s16i	a15, a5, 14
    3b63:	059216        	beqz	a2, 3bc0 <NC_NOISE_EST_Prcs+0x178>
    3b66:	8a4d           	ae_sext16	a4, a10
    3b68:	860c           	movi.n	a6, 8
    3b6a:	c04640        	sub	a4, a6, a4
    3b6d:	ff1c           	movi.n	a15, 31
    3b6f:	4344f0        	min	a4, a4, a15
    3b72:	6065e4        	ae_lq32f.x	aeq1, a5, a14
    3b75:	662c           	movi.n	a6, 38
    3b77:	047496        	bltz	a4, 3bc2 <NC_NOISE_EST_Prcs+0x17a>
    3b7a:	078676        	loop	a6, 3b85 <NC_NOISE_EST_Prcs+0x13d>
    3b7d:	3454b4        	ae_sraaq56	aeq1, aeq1, a4
    3b80:	f8667d331f 	{ ae_lq32f.iu	aeq1, a3, 4; ae_addsq56s	aeq0, aeq0, aeq1 }

00003b85 <NC_NOISE_EST_Prcs+0x13d>:
    3b85:	f1aa31        	l32r	a3, 230 (35e51 <_end+0x2d7c5>)
    3b88:	398334        	ae_movpa24x2	aep0, a3, a3
    3b8b:	f88020053e 	{ l16si	a3, a5, 0; ae_mulfq32sp24s.l	aeq0, aeq0, aep0 }
    3b90:	039562        	l16si	a6, a5, 6
    3b93:	fc4460257e 	{ l16si	a7, a5, 4; ae_roundsq32sym	aeq0, aeq0 }
    3b98:	620134        	ae_sq32f.i	aeq0, a1, 12
    3b9b:	3148           	l32i.n	a4, a1, 12
    3b9d:	214840        	srai	a4, a4, 8
    3ba0:	10a427        	bge	a4, a2, 3bb4 <NC_NOISE_EST_Prcs+0x16c>
    3ba3:	273a           	add.n	a2, a7, a3
    3ba5:	000386        	j	3bb7 <NC_NOISE_EST_Prcs+0x16f>

00003ba8 <NC_NOISE_EST_Prcs+0x160>:
    3ba8:	ef0d56        	bnez	a13, 3a9c <NC_NOISE_EST_Prcs+0x54>
    3bab:	080532        	l8ui	a3, a5, 8
    3bae:	eec316        	beqz	a3, 3a9e <NC_NOISE_EST_Prcs+0x56>
    3bb1:	ffb9c6        	j	3a9c <NC_NOISE_EST_Prcs+0x54>

00003bb4 <NC_NOISE_EST_Prcs+0x16c>:
    3bb4:	c02360        	sub	a2, a3, a6
    3bb7:	a22d           	ae_clamps16	a2, a2
    3bb9:	030c           	movi.n	a3, 0
    3bbb:	532230        	max	a2, a2, a3
    3bbe:	652d           	ae_s16i.n	a2, a5, 0
    3bc0:	f01d           	retw.n

00003bc2 <NC_NOISE_EST_Prcs+0x17a>:
    3bc2:	604040        	neg	a4, a4
    3bc5:	098676        	loop	a6, 3bd2 <NC_NOISE_EST_Prcs+0x18a>
    3bc8:	fc24fe944f 	{ ae_slaasq56s	aeq1, aeq1, a4; nop }
    3bcd:	f8667d331f 	{ ae_lq32f.iu	aeq1, a3, 4; ae_addsq56s	aeq0, aeq0, aeq1 }

00003bd2 <NC_NOISE_EST_Prcs+0x18a>:
    3bd2:	ffebc6        	j	3b85 <NC_NOISE_EST_Prcs+0x13d>

00003bd5 <NC_NOISE_EST_Prcs+0x18d>:
    3bd5:	ffae86        	j	3a93 <NC_NOISE_EST_Prcs+0x4b>

00003bd8 <take_min_four>:
    3bd8:	004136        	entry	a1, 32
    3bdb:	224d           	ae_l16si.n	a4, a2, 0
    3bdd:	5023a6        	blti	a3, 2, 3c31 <take_min_four+0x59>
    3be0:	ffa052        	movi	a5, 255
    3be3:	7fd552        	addmi	a5, a5, 0x7f00
    3be6:	630b           	addi.n	a6, a3, -1
    3be8:	222b           	addi.n	a2, a2, 2
    3bea:	048d           	mov.n	a8, a4
    3bec:	053d           	mov.n	a3, a5
    3bee:	207550        	or	a7, a5, a5
    3bf1:	318676        	loop	a6, 3c26 <take_min_four+0x4e>
    3bf4:	22ad           	ae_l16si.n	a10, a2, 0
    3bf6:	086d           	mov.n	a6, a8
    3bf8:	079d           	mov.n	a9, a7
    3bfa:	0a4d           	mov.n	a4, a10
    3bfc:	1c2a87        	blt	a10, a8, 3c1c <take_min_four+0x44>
    3bff:	084d           	mov.n	a4, a8
    3c01:	0a6d           	mov.n	a6, a10
    3c03:	079d           	mov.n	a9, a7
    3c05:	132a77        	blt	a10, a7, 3c1c <take_min_four+0x44>
    3c08:	084d           	mov.n	a4, a8
    3c0a:	076d           	mov.n	a6, a7
    3c0c:	0a9d           	mov.n	a9, a10
    3c0e:	0a2a37        	blt	a10, a3, 3c1c <take_min_four+0x44>
    3c11:	4355a0        	min	a5, a5, a10
    3c14:	039d           	mov.n	a9, a3
    3c16:	084d           	mov.n	a4, a8
    3c18:	076d           	mov.n	a6, a7
    3c1a:	053d           	mov.n	a3, a5

00003c1c <take_min_four+0x44>:
    3c1c:	035d           	mov.n	a5, a3
    3c1e:	222b           	addi.n	a2, a2, 2
    3c20:	093d           	mov.n	a3, a9
    3c22:	067d           	mov.n	a7, a6
    3c24:	048d           	mov.n	a8, a4

00003c26 <take_min_four+0x4e>:
    3c26:	249a           	add.n	a2, a4, a9
    3c28:	365a           	add.n	a3, a6, a5
    3c2a:	223a           	add.n	a2, a2, a3
    3c2c:	112220        	slli	a2, a2, 14
    3c2f:	f01d           	retw.n

00003c31 <take_min_four+0x59>:
    3c31:	ffa062        	movi	a6, 255
    3c34:	7fd662        	addmi	a6, a6, 0x7f00
    3c37:	069d           	mov.n	a9, a6
    3c39:	065d           	mov.n	a5, a6
    3c3b:	fff9c6        	j	3c26 <take_min_four+0x4e>

00003c3e <take_min_four+0x66>:
	...

00003c40 <equal_power>:
    3c40:	004136        	entry	a1, 32
    3c43:	c07450        	sub	a7, a4, a5
    3c46:	a34570        	movltz	a4, a5, a7
    3c49:	1416a6        	blti	a6, 1, 3c61 <equal_power+0x21>
    3c4c:	a33270        	movltz	a3, a2, a7
    3c4f:	605170        	abs	a5, a7
    3c52:	fcc322        	addi	a2, a3, -4
    3c55:	088676        	loop	a6, 3c61 <equal_power+0x21>
    3c58:	601214        	ae_lq32f.iu	aeq0, a2, 4
    3c5b:	3405a4        	ae_srlaq56	aeq0, aeq0, a5
    3c5e:	620204        	ae_sq32f.i	aeq0, a2, 0

00003c61 <equal_power+0x21>:
    3c61:	042d           	mov.n	a2, a4
    3c63:	f01d           	retw.n

00003c65 <equal_power+0x25>:
    3c65:	000000                                        ...

00003c68 <gen_power>:
    3c68:	004136        	entry	a1, 32
    3c6b:	002252        	l32i	a5, a2, 0
    3c6e:	3a2242        	l32i	a4, a2, 232
    3c71:	1123a6        	blti	a3, 2, 3c86 <gen_power+0x1e>
    3c74:	217130        	srai	a7, a3, 1
    3c77:	f8c432        	addi	a3, a4, -8
    3c7a:	206550        	or	a6, a5, a5
    3c7d:	058776        	loop	a7, 3c86 <gen_power+0x1e>
    3c80:	5c8614        	ae_lp24x2f.iu	aep0, a6, 8
    3c83:	5e8314        	ae_sp24x2f.iu	aep0, a3, 8

00003c86 <gen_power+0x1e>:
    3c86:	139232        	l16si	a3, a2, 38
    3c89:	2f0272        	l8ui	a7, a2, 47
    3c8c:	160c           	movi.n	a6, 1
    3c8e:	1133f0        	slli	a3, a3, 1
    3c91:	c06630        	sub	a6, a6, a3
    3c94:	f73b           	addi.n	a15, a7, 3
    3c96:	fcc532        	addi	a3, a5, -4
    3c99:	5c0404        	ae_lp24x2f.i	aep0, a4, 0
    3c9c:	6e5262        	s16i	a6, a2, 220
    3c9f:	0a8f76        	loop	a15, 3cad <gen_power+0x45>
    3ca2:	f003af841f 	{ ae_lp24x2f.iu	aep0, a4, 8; ae_mulzaap24s.hh.ll	aeq0, aep0, aep0 }
    3ca7:	060834        	ae_roundsq32sym	aeq0, aeq0
    3caa:	624314        	ae_sq32f.iu	aeq0, a3, 4

00003cad <gen_power+0x45>:
    3cad:	f01d           	retw.n

00003caf <gen_power+0x47>:
	...

00003cb0 <fft_wo>:
    3cb0:	006136        	entry	a1, 48
    3cb3:	2e0242        	l8ui	a4, a2, 46
    3cb6:	750c           	movi.n	a5, 7
    3cb8:	160c           	movi.n	a6, 1
    3cba:	625540        	saltu	a5, a5, a4
    3cbd:	fcc442        	addi	a4, a4, -4
    3cc0:	624460        	saltu	a4, a4, a6
    3cc3:	205540        	or	a5, a5, a4
    3cc6:	f0a0c2        	movi	a12, 240
    3cc9:	15cc           	bnez.n	a5, 3cce <fft_wo+0x1e>
    3ccb:	78a0c2        	movi	a12, 120

00003cce <fft_wo+0x1e>:
    3cce:	270c           	movi.n	a7, 2
    3cd0:	05cc           	bnez.n	a5, 3cd4 <fft_wo+0x24>
    3cd2:	470c           	movi.n	a7, 4
    3cd4:	060c           	movi.n	a6, 0
    3cd6:	204330        	or	a4, a3, a3
    3cd9:	098c76        	loop	a12, 3ce6 <fft_wo+0x36>
    3cdc:	248d           	ae_l16si.n	a8, a4, 0
    3cde:	442b           	addi.n	a4, a4, 2
    3ce0:	608180        	abs	a8, a8
    3ce3:	206860        	or	a6, a8, a6

00003ce6 <fft_wo+0x36>:
    3ce6:	00a2f2        	movi	a15, 0x200
    3ce9:	15cc           	bnez.n	a5, 3cee <fft_wo+0x3e>
    3ceb:	00a1f2        	movi	a15, 0x100

00003cee <fft_wo+0x3e>:
    3cee:	f15141        	l32r	a4, 234 (8160 <fftorder_512_12b>)
    3cf1:	35cc           	bnez.n	a5, 3cf8 <fft_wo+0x48>
    3cf3:	548b           	addi.n	a5, a4, 8
    3cf5:	000046        	j	3cfa <fft_wo+0x4a>

00003cf8 <fft_wo+0x48>:
    3cf8:	544b           	addi.n	a5, a4, 4
    3cfa:	40e640        	nsa	a4, a6
    3cfd:	1292a2        	l16si	a10, a2, 36
    3d00:	060c           	movi.n	a6, 0
    3d02:	f0c442        	addi	a4, a4, -16
    3d05:	3e22e2        	l32i	a14, a2, 248
    3d08:	534460        	max	a4, a4, a6
    3d0b:	02b8           	l32i.n	a11, a2, 0
    3d0d:	3d2282        	l32i	a8, a2, 244
    3d10:	1149           	s32i.n	a4, a1, 4
    3d12:	4344a0        	min	a4, a4, a10
    3d15:	031644        	wur.ae_sar	a4
    3d18:	440e04        	ae_lp16x2f.i	aep0, a14, 0
    3d1b:	a0afb0        	addx4	a10, a15, a11
    3d1e:	41d2f0        	srli	a13, a15, 2
    3d21:	1197f0        	slli	a9, a7, 1
    3d24:	2149           	s32i.n	a4, a1, 8
    3d26:	31f9           	s32i.n	a15, a1, 12
    3d28:	f823ad180f 	{ ae_lp16x2f.i	aep1, a8, 0; ae_sllssp24s	aep0, aep0 }
    3d2d:	ba4b           	addi.n	a11, a10, 4
    3d2f:	158d76        	loop	a13, 3d48 <fft_wo+0x98>
    3d32:	e820adfe1f 	{ ae_lp16x2f.iu	aep7, a14, 4; ae_mulfp24s.hh	aeq0, aep0, aep1 }
    3d37:	eb21ed981f 	{ ae_lp16x2f.iu	aep1, a8, 4; ae_mulfp24s.ll	aeq3, aep0, aep1 }
    3d3c:	f83fb84a6f 	{ ae_sq32f.x	aeq0, a10, a6; ae_sllssp24s	aep0, aep7 }
    3d41:	62bb64        	ae_sq32f.x	aeq3, a11, a6
    3d44:	256d           	ae_l16si.n	a6, a5, 0
    3d46:	559a           	add.n	a5, a5, a9

00003d48 <fft_wo+0x98>:
    3d48:	fc24b3e27e 	{ l32i	a7, a2, 248; ae_zeroq56	aeq0 }
    3d4d:	41f1c0        	srli	a15, a12, 1
    3d50:	f842154dff 	{ sub	a4, a13, a15; ae_truncp24q48x2	aep0, aeq0, aeq0 }
    3d55:	fcc7e2        	addi	a14, a7, -4
    3d58:	0214e6        	bgei	a4, 1, 3d5e <fft_wo+0xae>
    3d5b:	002706        	j	3dfb <fft_wo+0x14b>
    3d5e:	90cc70        	addx2	a12, a12, a7
    3d61:	442c04        	ae_lp16x2f.i	aep2, a12, 0
    3d64:	f03d           	nop.n
    3d66:	098476        	loop	a4, 3d73 <fft_wo+0xc3>
    3d69:	fc4873ae1f 	{ ae_sp16x2f.iu	aep2, a14, 4; ae_abssp24s	aep1, aep2 }
    3d6e:	dc202dac1f 	{ ae_lp16x2f.iu	aep2, a12, 4; ae_orp48	aep0, aep0, aep1 }
    3d73:	c04df0        	sub	a4, a13, a15
    3d76:	4498f4        	ae_lp16x2f.iu	aep1, a8, -4
    3d79:	f03d           	nop.n
    3d7b:	f03d           	nop.n
    3d7d:	1d8476        	loop	a4, 3d9e <fft_wo+0xee>
    3d80:	f92bf3ae1f 	{ ae_sp16x2f.iu	aep2, a14, 4; ae_sllssp24s	aep3, aep2 }
    3d85:	013154        	ae_mulfp24s.hl	aeq0, aep3, aep1
    3d88:	e92ded98ff 	{ ae_lp16x2f.iu	aep1, a8, -4; ae_mulfp24s.lh	aeq3, aep3, aep1 }
    3d8d:	fe48384a6f 	{ ae_sq32f.x	aeq0, a10, a6; ae_abssp24s	aep4, aep2 }
    3d92:	dc80387b6f 	{ ae_sq32f.x	aeq3, a11, a6; ae_orp48	aep0, aep0, aep4 }
    3d97:	256d           	ae_l16si.n	a6, a5, 0
    3d99:	44ac14        	ae_lp16x2f.iu	aep2, a12, 4
    3d9c:	559a           	add.n	a5, a5, a9

00003d9e <fft_wo+0xee>:
    3d9e:	0084d4        	ae_cvta32p24.h	a4, aep0
    3da1:	0007d4        	ae_cvta32p24.l	a7, aep0
    3da4:	11d8           	l32i.n	a13, a1, 4
    3da6:	204470        	or	a4, a4, a7
    3da9:	40e440        	nsa	a4, a4
    3dac:	434d40        	min	a4, a13, a4
    3daf:	125242        	s16i	a4, a2, 36
    3db2:	440304        	ae_lp16x2f.i	aep0, a3, 0
    3db5:	188f76        	loop	a15, 3dd1 <fft_wo+0x121>
    3db8:	f923b38e1f 	{ ae_sp16x2f.iu	aep0, a14, 4; ae_sllssp24s	aep2, aep0 }
    3dbd:	ea28ad831f 	{ ae_lp16x2f.iu	aep0, a3, 4; ae_mulfp24s.hl	aeq0, aep2, aep1 }
    3dc2:	e929ed98ff 	{ ae_lp16x2f.iu	aep1, a8, -4; ae_mulfp24s.lh	aeq3, aep2, aep1 }
    3dc7:	628a64        	ae_sq32f.x	aeq0, a10, a6
    3dca:	62bb64        	ae_sq32f.x	aeq3, a11, a6
    3dcd:	256d           	ae_l16si.n	a6, a5, 0
    3dcf:	559a           	add.n	a5, a5, a9

00003dd1 <fft_wo+0x121>:
    3dd1:	3148           	l32i.n	a4, a1, 12
    3dd3:	0238           	l32i.n	a3, a2, 0
    3dd5:	8c0c           	movi.n	a12, 8
    3dd7:	41b140        	srli	a11, a4, 1
    3dda:	a0a430        	addx4	a10, a4, a3
    3ddd:	f11631        	l32r	a3, 238 (50a4 <sw_fft_32b_2811>)
    3de0:	1d0c           	movi.n	a13, 1
    3de2:	0e0c           	movi.n	a14, 0
    3de4:	0003e0        	callx8	a3
    3de7:	2138           	l32i.n	a3, a1, 8
    3de9:	04bd           	mov.n	a11, a4
    3deb:	c033a0        	sub	a3, a3, a10
    3dee:	135232        	s16i	a3, a2, 38
    3df1:	f11231        	l32r	a3, 23c (3c68 <gen_power>)
    3df4:	02ad           	mov.n	a10, a2
    3df6:	0003e0        	callx8	a3
    3df9:	f01d           	retw.n

00003dfb <fft_wo+0x14b>:
    3dfb:	4498f4        	ae_lp16x2f.iu	aep1, a8, -4
    3dfe:	ffe706        	j	3d9e <fft_wo+0xee>

00003e01 <fft_wo+0x151>:
    3e01:	000000                                        ...

00003e04 <NR_vad1>:
    3e04:	006136        	entry	a1, 48
    3e07:	2e02b2        	l8ui	a11, a2, 46
    3e0a:	0238           	l32i.n	a3, a2, 0
    3e0c:	f10d81        	l32r	a8, 240 (7b90 <wb_sb_ptn>)
    3e0f:	066b16        	beqz	a11, 3e79 <NR_vad1+0x75>
    3e12:	f10c61        	l32r	a6, 244 (7bc0 <nb_sb_ptn>)
    3e15:	eca092        	movi	a9, 236
    3e18:	4153b0        	srli	a5, a11, 3
    3e1b:	f29a           	add.n	a15, a2, a9
    3e1d:	fd24bd033f 	{ ae_lq32f.i	aeq0, a3, 12; ae_zeroq56	aeq1 }
    3e22:	31b9           	s32i.n	a11, a1, 12
    3e24:	117be0        	slli	a7, a11, 2
    3e27:	00a042        	movi	a4, 0
    3e2a:	936850        	movnez	a6, a8, a5
    3e2d:	0cc332        	addi	a3, a3, 12
    3e30:	0261f2        	s32i	a15, a1, 8

00003e33 <NR_vad1+0x2f>:
    3e33:	265d           	ae_l16si.n	a5, a6, 0
    3e35:	368d           	ae_l16si.n	a8, a6, 2
    3e37:	c05850        	sub	a5, a8, a5
    3e3a:	0715a6        	blti	a5, 1, 3e45 <NR_vad1+0x41>
    3e3d:	048576        	loop	a5, 3e45 <NR_vad1+0x41>
    3e40:	f96a3d131f 	{ ae_lq32f.iu	aeq0, a3, 4; ae_addq56	aeq1, aeq1, aeq0 }

00003e45 <NR_vad1+0x41>:
    3e45:	365504        	ae_nsaq56s	a5, aeq1
    3e48:	6e9282        	l16si	a8, a2, 220
    3e4b:	f8c552        	addi	a5, a5, -8
    3e4e:	fc24fe950f 	{ ae_slaasq56s	aeq0, aeq1, a5; nop }
    3e53:	c0b850        	sub	a11, a8, a5
    3e56:	368a04        	ae_trunca32q48	a10, aeq0
    3e59:	f0b951        	l32r	a5, 140 (56c <Log2_norm>)
    3e5c:	0005e0        	callx8	a5
    3e5f:	fd24b3b25e 	{ l32i	a5, a2, 236; ae_zeroq56	aeq1 }
    3e64:	600304        	ae_lq32f.i	aeq0, a3, 0
    3e67:	662b           	addi.n	a6, a6, 2
    3e69:	854a           	add.n	a8, a5, a4
    3e6b:	444b           	addi.n	a4, a4, 4
    3e6d:	08a9           	s32i.n	a10, a8, 0
    3e6f:	c09747        	bne	a7, a4, 3e33 <NR_vad1+0x2f>
    3e72:	31b8           	l32i.n	a11, a1, 12
    3e74:	21c8           	l32i.n	a12, a1, 8
    3e76:	0001c6        	j	3e81 <NR_vad1+0x7d>

00003e79 <NR_vad1+0x75>:
    3e79:	3b2252        	l32i	a5, a2, 236
    3e7c:	eca032        	movi	a3, 236
    3e7f:	c23a           	add.n	a12, a2, a3
    3e81:	f0f161        	l32r	a6, 248 (7870 <NR_immediate_num>)
    3e84:	06cb16        	beqz	a11, 3ef4 <NR_vad1+0xf0>
    3e87:	039272        	l16si	a7, a2, 6
    3e8a:	c4a082        	movi	a8, 196
    3e8d:	c8a092        	movi	a9, 200
    3e90:	34c232        	addi	a3, a2, 52
    3e93:	117700        	slli	a7, a7, 16
    3e96:	360734        	ae_cvtq48a32s	aeq0, a7
    3e99:	604504        	ae_lq32f.i	aeq1, a5, 0
    3e9c:	54c242        	addi	a4, a2, 84
    3e9f:	070c           	movi.n	a7, 0
    3ea1:	608304        	ae_lq32f.i	aeq2, a3, 0
    3ea4:	828a           	add.n	a8, a2, a8
    3ea6:	929a           	add.n	a9, a2, a9
    3ea8:	338b76        	loop	a11, 3edf <NR_vad1+0xdb>
    3eab:	d6c813a99f 	{ or	a10, a9, a9; ae_ltq56s	b0, aeq2, aeq1 }
    3eb0:	09ed           	mov.n	a14, a9
    3eb2:	f76b           	addi.n	a15, a7, 6
    3eb4:	d3a800        	movt	a10, a8, b0
    3eb7:	440a04        	ae_lp16x2f.i	aep0, a10, 0
    3eba:	c18064        	ae_mulzaafq32sp16s.lh	aeq2, aeq1, aep0, aeq2, aep0
    3ebd:	622304        	ae_sq32f.i	aeq2, a3, 0
    3ec0:	60d414        	ae_lq32f.iu	aeq3, a4, 4
    3ec3:	09b104        	ae_ltq56s	b0, aeq1, aeq3
    3ec6:	d3e800        	movt	a14, a8, b0
    3ec9:	447e04        	ae_lp16x2f.i	aep7, a14, 0
    3ecc:	c1ff74        	ae_mulzaafq32sp16s.lh	aeq3, aeq1, aep7, aeq3, aep7
    3ecf:	fab6f9b40f 	{ ae_sq32f.i	aeq3, a4, 0; ae_subq56	aeq2, aeq2, aeq3 }
    3ed4:	d6503d351f 	{ ae_lq32f.iu	aeq1, a5, 4; ae_ltq56s	b0, aeq0, aeq2 }
    3ed9:	609314        	ae_lq32f.iu	aeq2, a3, 4
    3edc:	d37f00        	movt	a7, a15, b0

00003edf <NR_vad1+0xdb>:
    3edf:	030c           	movi.n	a3, 0
    3ee1:	0c58           	l32i.n	a5, a12, 0
    3ee3:	724370        	salt	a4, a3, a7
    3ee6:	f68b           	addi.n	a15, a6, 8
    3ee8:	36cb           	addi.n	a3, a6, 12
    3eea:	933f40        	movnez	a3, a15, a4
    3eed:	7bcc           	bnez.n	a11, 3ef8 <NR_vad1+0xf4>
    3eef:	000a86        	j	3f1d <NR_vad1+0x119>

00003ef2 <NR_vad1+0xee>:
	...

00003ef4 <NR_vad1+0xf0>:
    3ef4:	36cb           	addi.n	a3, a6, 12
    3ef6:	3bac           	beqz.n	a11, 3f1d <NR_vad1+0x119>
    3ef8:	1092f2        	l16si	a15, a2, 32
    3efb:	440304        	ae_lp16x2f.i	aep0, a3, 0
    3efe:	7cc222        	addi	a2, a2, 124
    3f01:	113f00        	slli	a3, a15, 16
    3f04:	608504        	ae_lq32f.i	aeq2, a5, 0
    3f07:	600204        	ae_lq32f.i	aeq0, a2, 0
    3f0a:	364334        	ae_cvtq48a32s	aeq1, a3
    3f0d:	0c8b76        	loop	a11, 3f1d <NR_vad1+0x119>
    3f10:	0f003d121f 	{ ae_lq32f.iu	aeq0, a2, 4; ae_mulzaafq32sp16s.lh	aeq2, aeq2, aep0, aeq0, aep0 }
    3f15:	fb773d551f 	{ ae_lq32f.iu	aeq2, a5, 4; ae_maxq56s	aeq3, aeq1, aeq2 }
    3f1a:	6232f4        	ae_sq32f.i	aeq3, a2, -4

00003f1d <NR_vad1+0x119>:
    3f1d:	f01d           	retw.n

00003f1f <NR_vad1+0x11b>:
	...

00003f20 <NR_vad2>:
    3f20:	008136        	entry	a1, 64
    3f23:	151232        	l16ui	a3, a2, 42
    3f26:	021242        	l16ui	a4, a2, 4
    3f29:	2e02a2        	l8ui	a10, a2, 46
    3f2c:	109252        	l16si	a5, a2, 32
    3f2f:	3e9347        	bne	a3, a4, 3f71 <NR_vad2+0x51>
    3f32:	040c           	movi.n	a4, 0
    3f34:	155242        	s16i	a4, a2, 42
    3f37:	71a9           	s32i.n	a10, a1, 28
    3f39:	053d           	mov.n	a3, a5
    3f3b:	074a16        	beqz	a10, 3fb3 <NR_vad2+0x93>
    3f3e:	c288           	l32i.n	a8, a2, 48
    3f40:	f0c361        	l32r	a6, 24c (3bd8 <take_min_four>)
    3f43:	a0a042        	movi	a4, 160
    3f46:	08d332        	addmi	a3, a3, 0x800
    3f49:	115300        	slli	a5, a3, 16
    3f4c:	0a7d           	mov.n	a7, a10
    3f4e:	424a           	add.n	a4, a2, a4
    3f50:	083d           	mov.n	a3, a8
    3f52:	6189           	s32i.n	a8, a1, 24

00003f54 <NR_vad2+0x34>:
    3f54:	cb0c           	movi.n	a11, 12
    3f56:	03ad           	mov.n	a10, a3
    3f58:	0006e0        	callx8	a6
    3f5b:	5385a0        	max	a8, a5, a10
    3f5e:	770b           	addi.n	a7, a7, -1
    3f60:	0489           	s32i.n	a8, a4, 0
    3f62:	18c332        	addi	a3, a3, 24
    3f65:	444b           	addi.n	a4, a4, 4
    3f67:	fe9756        	bnez	a7, 3f54 <NR_vad2+0x34>
    3f6a:	001246        	j	3fb7 <NR_vad2+0x97>

00003f6d <NR_vad2+0x4d>:
    3f6d:	00000000                                ....

00003f71 <NR_vad2+0x51>:
    3f71:	7cc232        	addi	a3, a2, 124
    3f74:	600304        	ae_lq32f.i	aeq0, a3, 0
    3f77:	078a16        	beqz	a10, 3ff3 <NR_vad2+0xd3>
    3f7a:	620154        	ae_sq32f.i	aeq0, a1, 20
    3f7d:	c258           	l32i.n	a5, a2, 48
    3f7f:	2d0262        	l8ui	a6, a2, 45
    3f82:	604154        	ae_lq32f.i	aeq1, a1, 20
    3f85:	841c           	movi.n	a4, 24
    3f87:	905650        	addx2	a5, a6, a5
    3f8a:	e8c552        	addi	a5, a5, -24
    3f8d:	0c8a76        	loop	a10, 3f9d <NR_vad2+0x7d>
    3f90:	f8ebda054f 	{ ae_lp16f.xu	aep0, a5, a4; ae_roundsp16q48sym	aep1, aeq1 }
    3f95:	d822bd331f 	{ ae_lq32f.iu	aeq1, a3, 4; ae_minp24s	aep0, aep0, aep1 }
    3f9a:	420504        	ae_sp16f.l.i	aep0, a5, 0

00003f9d <NR_vad2+0x7d>:
    3f9d:	159232        	l16si	a3, a2, 42
    3fa0:	f0ad81        	l32r	a8, 254 (7b70 <wb_score_bands>)
    3fa3:	f0ab41        	l32r	a4, 250 (e38e4 <_end+0xdb258>)
    3fa6:	331b           	addi.n	a3, a3, 1
    3fa8:	155232        	s16i	a3, a2, 42
    3fab:	4c8ab6        	bltui	a10, 8, 3ffb <NR_vad2+0xdb>
    3fae:	043d           	mov.n	a3, a4
    3fb0:	001346        	j	4001 <NR_vad2+0xe1>

00003fb3 <NR_vad2+0x93>:
    3fb3:	c238           	l32i.n	a3, a2, 48
    3fb5:	6139           	s32i.n	a3, a1, 24
    3fb7:	2d0232        	l8ui	a3, a2, 45
    3fba:	71a8           	l32i.n	a10, a1, 28
    3fbc:	331b           	addi.n	a3, a3, 1
    3fbe:	744030        	extui	a4, a3, 0, 8
    3fc1:	f4c442        	addi	a4, a4, -12
    3fc4:	934340        	movnez	a4, a3, a4
    3fc7:	7cc232        	addi	a3, a2, 124
    3fca:	600304        	ae_lq32f.i	aeq0, a3, 0
    3fcd:	2d4242        	s8i	a4, a2, 45
    3fd0:	fa9c           	beqz.n	a10, 3ff3 <NR_vad2+0xd3>
    3fd2:	620144        	ae_sq32f.i	aeq0, a1, 16
    3fd5:	6168           	l32i.n	a6, a1, 24
    3fd7:	744040        	extui	a4, a4, 0, 8
    3fda:	604144        	ae_lq32f.i	aeq1, a1, 16
    3fdd:	906460        	addx2	a6, a4, a6
    3fe0:	841c           	movi.n	a4, 24
    3fe2:	e8c652        	addi	a5, a6, -24
    3fe5:	078a76        	loop	a10, 3ff0 <NR_vad2+0xd0>
    3fe8:	f8ebbd331f 	{ ae_lq32f.iu	aeq1, a3, 4; ae_roundsp16q48sym	aep0, aeq1 }
    3fed:	438544        	ae_sp16f.l.xu	aep0, a5, a4

00003ff0 <NR_vad2+0xd0>:
    3ff0:	ffea46        	j	3f9d <NR_vad2+0x7d>

00003ff3 <NR_vad2+0xd3>:
    3ff3:	159232        	l16si	a3, a2, 42
    3ff6:	331b           	addi.n	a3, a3, 1
    3ff8:	155232        	s16i	a3, a2, 42
    3ffb:	f09881        	l32r	a8, 25c (7bb0 <nb_score_bands>)
    3ffe:	f09631        	l32r	a3, 258 (124925 <_end+0x11c299>)
    4001:	398334        	ae_movpa24x2	aep0, a3, a3
    4004:	3c2242        	l32i	a4, a2, 240
    4007:	a0a052        	movi	a5, 160
    400a:	fc20fc452f 	{ ae_lq32f.xu	aeq2, a5, a2; ae_zerop48	aep1 }
    400f:	fc24b9813f 	{ ae_sq32f.i	aeq0, a1, 12; ae_zeroq56	aeq0 }
    4014:	604134        	ae_lq32f.i	aeq1, a1, 12
    4017:	326b           	addi.n	a3, a2, 6
    4019:	fbaac7c26e 	{ addi	a6, a2, 124; ae_subq56	aeq3, aeq1, aeq2 }
    401e:	fcc472        	addi	a7, a4, -4
    4021:	1e4a66        	bnei	a10, 4, 4043 <NR_vad2+0x123>
    4024:	420c           	movi.n	a2, 4
    4026:	178276        	loop	a2, 4041 <NR_vad2+0x121>
    4029:	f8aaec831f 	{ ae_lp16f.iu	aep0, a3, 2; ae_subq56	aeq0, aeq1, aeq2 }
    402e:	ff0c3d361f 	{ ae_lq32f.iu	aeq1, a6, 4; ae_cvtq48p24s.l	aeq3, aep0 }
    4033:	f8a6fd551f 	{ ae_lq32f.iu	aeq2, a5, 4; ae_subq56	aeq0, aeq0, aeq3 }
    4038:	350074        	ae_sraiq56	aeq0, aeq0, 1
    403b:	098c74        	ae_truncp24q48x2	aep7, aeq0, aeq0
    403e:	52f714        	ae_sp24f.l.iu	aep7, a7, 4

00004041 <NR_vad2+0x121>:
    4041:	f01d           	retw.n

00004043 <NR_vad2+0x123>:
    4043:	fec882        	addi	a8, a8, -2
    4046:	caac           	beqz.n	a10, 4076 <NR_vad2+0x156>
    4048:	190c           	movi.n	a9, 1
    404a:	288a76        	loop	a10, 4076 <NR_vad2+0x156>
    404d:	40a314        	ae_lp16f.iu	aep2, a3, 2
    4050:	fe4c00004f 	{ movi	a4, 0; ae_cvtq48p24s.l	aeq2, aep2 }
    4055:	d6d82ca81f 	{ ae_lp16f.iu	aep2, a8, 2; ae_ltq56s	b0, aeq2, aeq3 }
    405a:	f9bad6490f 	{ movt	a4, a9, b0; ae_subq56	aeq1, aeq3, aeq2 }
    405f:	148c           	beqz.n	a4, 4064 <NR_vad2+0x144>
    4061:	079214        	ae_addsp24s	aep1, aep1, aep2
    4064:	f6c3bb109f 	{ ae_sraiq56	aeq2, aeq1, 1; ae_mulafq32sp24s.l	aeq0, aeq3, aep0 }
    4069:	f9d23d361f 	{ ae_lq32f.iu	aeq1, a6, 4; ae_truncp24q48x2	aep2, aeq2, aeq2 }
    406e:	609514        	ae_lq32f.iu	aeq2, a5, 4
    4071:	fbaaf4a71f 	{ ae_sp24f.l.iu	aep2, a7, 4; ae_subq56	aeq3, aeq1, aeq2 }

00004076 <NR_vad2+0x156>:
    4076:	408314        	ae_lp16f.iu	aep0, a3, 2
    4079:	fd0c00003f 	{ movi	a3, 0; ae_cvtq48p24s.l	aeq1, aep0 }
    407e:	d64400014f 	{ movi	a4, 1; ae_ltq56s	b0, aeq1, aeq0 }
    4083:	d33400        	movt	a3, a4, b0
    4086:	438c           	beqz.n	a3, 408e <NR_vad2+0x16e>
    4088:	408814        	ae_lp16f.iu	aep0, a8, 2
    408b:	079014        	ae_addsp24s	aep1, aep1, aep0

0000408e <NR_vad2+0x16e>:
    408e:	28c222        	addi	a2, a2, 40
    4091:	421204        	ae_sp16f.l.i	aep1, a2, 0
    4094:	f01d           	retw.n

00004096 <NR_vad2+0x176>:
	...

00004098 <NR_NoiseEstimate>:
    4098:	004136        	entry	a1, 32
    409b:	2f0232        	l8ui	a3, a2, 47
    409e:	0248           	l32i.n	a4, a2, 0
    40a0:	0e9272        	l16si	a7, a2, 28
    40a3:	039c           	beqz.n	a3, 40b7 <NR_NoiseEstimate+0x1f>
    40a5:	08d452        	addmi	a5, a4, 0x800
    40a8:	060c           	movi.n	a6, 0
    40aa:	1177c0        	slli	a7, a7, 4
    40ad:	068376        	loop	a3, 40b7 <NR_NoiseEstimate+0x1f>
    40b0:	0579           	s32i.n	a7, a5, 0
    40b2:	ee6562        	s32i	a6, a5, 0x3b8
    40b5:	554b           	addi.n	a5, a5, 4

000040b7 <NR_NoiseEstimate+0x1f>:
    40b7:	141262        	l16ui	a6, a2, 40
    40ba:	451c           	movi.n	a5, 20
    40bc:	2e0272        	l8ui	a7, a2, 46
    40bf:	613567        	bltu	a5, a6, 4124 <NR_NoiseEstimate+0x8c>
    40c2:	075d           	mov.n	a5, a7
    40c4:	0576f6        	bgeui	a6, 7, 40cd <NR_NoiseEstimate+0x35>
    40c7:	cca062        	movi	a6, 204
    40ca:	0002c6        	j	40d9 <NR_NoiseEstimate+0x41>

000040cd <NR_NoiseEstimate+0x35>:
    40cd:	05b6f6        	bgeui	a6, 16, 40d6 <NR_NoiseEstimate+0x3e>
    40d0:	d0a062        	movi	a6, 208
    40d3:	000086        	j	40d9 <NR_NoiseEstimate+0x41>

000040d6 <NR_NoiseEstimate+0x3e>:
    40d6:	d4a062        	movi	a6, 212
    40d9:	626a           	add.n	a6, a2, a6
    40db:	045516        	beqz	a5, 4124 <NR_NoiseEstimate+0x8c>
    40de:	77a1a2        	movi	a10, 0x177
    40e1:	0668           	l32i.n	a6, a6, 0
    40e3:	3c2272        	l32i	a7, a2, 240
    40e6:	f05691        	l32r	a9, 240 (7b90 <wb_sb_ptn>)
    40e9:	11aad0        	slli	a10, a10, 3
    40ec:	080c           	movi.n	a8, 0
    40ee:	a4aa           	add.n	a10, a4, a10
    40f0:	0007c6        	j	4113 <NR_NoiseEstimate+0x7b>

000040f3 <NR_NoiseEstimate+0x5b>:
    40f3:	881b           	addi.n	a8, a8, 1
    40f5:	90c890        	addx2	a12, a8, a9
    40f8:	2dbd           	ae_l16si.n	a11, a13, 0
    40fa:	2ccd           	ae_l16si.n	a12, a12, 0
    40fc:	10abc7        	bge	a11, a12, 4110 <NR_NoiseEstimate+0x78>
    40ff:	c0ccb0        	sub	a12, a12, a11
    4102:	a0bba0        	addx4	a11, a11, a10
    4105:	078c76        	loop	a12, 4110 <NR_NoiseEstimate+0x78>
    4108:	0bc8           	l32i.n	a12, a11, 0
    410a:	cc6a           	add.n	a12, a12, a6
    410c:	0bc9           	s32i.n	a12, a11, 0
    410e:	bb4b           	addi.n	a11, a11, 4

00004110 <NR_NoiseEstimate+0x78>:
    4110:	101857        	beq	a8, a5, 4124 <NR_NoiseEstimate+0x8c>

00004113 <NR_NoiseEstimate+0x7b>:
    4113:	a0b870        	addx4	a11, a8, a7
    4116:	0bb8           	l32i.n	a11, a11, 0
    4118:	90d890        	addx2	a13, a8, a9
    411b:	d41ba6        	blti	a11, 1, 40f3 <NR_NoiseEstimate+0x5b>
    411e:	881b           	addi.n	a8, a8, 1
    4120:	fffb06        	j	4110 <NR_NoiseEstimate+0x78>

00004123 <NR_NoiseEstimate+0x8b>:
	...

00004124 <NR_NoiseEstimate+0x8c>:
    4124:	6e9282        	l16si	a8, a2, 220
    4127:	6f9292        	l16si	a9, a2, 222
    412a:	402262        	l32i	a6, a2, 0x100
    412d:	987d           	ae_zext16	a7, a8
    412f:	99ad           	ae_zext16	a10, a9
    4131:	54cb           	addi.n	a5, a4, 12
    4133:	2417a7        	beq	a7, a10, 415b <NR_NoiseEstimate+0xc3>
    4136:	c09890        	sub	a9, a8, a9
    4139:	a37a90        	movltz	a7, a10, a9
    413c:	539c           	beqz.n	a3, 4155 <NR_NoiseEstimate+0xbd>
    413e:	06ad           	mov.n	a10, a6
    4140:	a3a590        	movltz	a10, a5, a9
    4143:	608190        	abs	a8, a9
    4146:	fcca92        	addi	a9, a10, -4
    4149:	088376        	loop	a3, 4155 <NR_NoiseEstimate+0xbd>
    414c:	601914        	ae_lq32f.iu	aeq0, a9, 4
    414f:	3408a4        	ae_srlaq56	aeq0, aeq0, a8
    4152:	620904        	ae_sq32f.i	aeq0, a9, 0

00004155 <NR_NoiseEstimate+0xbd>:
    4155:	6f5272        	s16i	a7, a2, 222
    4158:	6e5272        	s16i	a7, a2, 220

0000415b <NR_NoiseEstimate+0xc3>:
    415b:	fc2482ed7f 	{ movi	a7, 0x2ed; ae_zeroq56	aeq0 }
    4160:	b3ac           	beqz.n	a3, 418f <NR_NoiseEstimate+0xf7>
    4162:	1177e0        	slli	a7, a7, 2
    4165:	604434        	ae_lq32f.i	aeq1, a4, 12
    4168:	fcc662        	addi	a6, a6, -4
    416b:	447a           	add.n	a4, a4, a7
    416d:	1e8376        	loop	a3, 418f <NR_NoiseEstimate+0xf7>
    4170:	448414        	ae_lp16x2f.iu	aep0, a4, 4
    4173:	447d           	ae_l16ui.n	a7, a4, 0
    4175:	609614        	ae_lq32f.iu	aeq2, a6, 4
    4178:	c18894        	ae_mulzaafq32sp16s.lh	aeq3, aeq2, aep0, aeq1, aep0
    417b:	978c           	beqz.n	a7, 4188 <NR_NoiseEstimate+0xf0>
    417d:	f87b3d351f 	{ ae_lq32f.iu	aeq1, a5, 4; ae_maxq56s	aeq0, aeq0, aeq3 }
    4182:	623604        	ae_sq32f.i	aeq3, a6, 0
    4185:	000106        	j	418d <NR_NoiseEstimate+0xf5>

00004188 <NR_NoiseEstimate+0xf0>:
    4188:	f8733d351f 	{ ae_lq32f.iu	aeq1, a5, 4; ae_maxq56s	aeq0, aeq0, aeq2 }
    418d:	f03d           	nop.n

0000418f <NR_NoiseEstimate+0xf7>:
    418f:	364404        	ae_nsaq56s	a4, aeq0
    4192:	950c           	movi.n	a5, 9
    4194:	212457        	blt	a4, a5, 41b9 <NR_NoiseEstimate+0x121>
    4197:	6f9262        	l16si	a6, a2, 222
    419a:	f8c442        	addi	a4, a4, -8
    419d:	402252        	l32i	a5, a2, 0x100
    41a0:	c06640        	sub	a6, a6, a4
    41a3:	6f5262        	s16i	a6, a2, 222
    41a6:	f38c           	beqz.n	a3, 41b9 <NR_NoiseEstimate+0x121>
    41a8:	fcc522        	addi	a2, a5, -4
    41ab:	f03d           	nop.n
    41ad:	088376        	loop	a3, 41b9 <NR_NoiseEstimate+0x121>
    41b0:	601214        	ae_lq32f.iu	aeq0, a2, 4
    41b3:	340494        	ae_sllaq56	aeq0, aeq0, a4
    41b6:	620204        	ae_sq32f.i	aeq0, a2, 0
    41b9:	f01d           	retw.n

000041bb <NR_NoiseEstimate+0x123>:
	...

000041bc <NR_NoiseWithGain>:
    41bc:	004136        	entry	a1, 32
    41bf:	141262        	l16ui	a6, a2, 40
    41c2:	0238           	l32i.n	a3, a2, 0
    41c4:	2e0242        	l8ui	a4, a2, 46
    41c7:	5c76b6        	bltui	a6, 7, 4227 <NR_NoiseWithGain+0x6b>
    41ca:	0f9272        	l16si	a7, a2, 30
    41cd:	056416        	beqz	a4, 4227 <NR_NoiseWithGain+0x6b>
    41d0:	581c           	movi.n	a8, 21
    41d2:	626680        	saltu	a6, a6, a8
    41d5:	f71b           	addi.n	a15, a7, 1
    41d7:	4181f0        	srli	a8, a15, 1
    41da:	40d882        	addmi	a8, a8, 0x4000
    41dd:	838760        	moveqz	a8, a7, a6
    41e0:	300884        	ae_cvtp24a16x2.ll	aep0, a8, a8
    41e3:	3c2252        	l32i	a5, a2, 240
    41e6:	f01681        	l32r	a8, 240 (7b90 <wb_sb_ptn>)
    41e9:	08d362        	addmi	a6, a3, 0x800
    41ec:	070c           	movi.n	a7, 0

000041ee <NR_NoiseWithGain+0x32>:
    41ee:	a09750        	addx4	a9, a7, a5
    41f1:	0998           	l32i.n	a9, a9, 0
    41f3:	90b780        	addx2	a11, a7, a8
    41f6:	2819a6        	blti	a9, 1, 4222 <NR_NoiseWithGain+0x66>
    41f9:	771b           	addi.n	a7, a7, 1
    41fb:	90a780        	addx2	a10, a7, a8
    41fe:	2b9d           	ae_l16si.n	a9, a11, 0
    4200:	2aad           	ae_l16si.n	a10, a10, 0
    4202:	a0b960        	addx4	a11, a9, a6
    4205:	13a9a7        	bge	a9, a10, 421c <NR_NoiseWithGain+0x60>
    4208:	c0aa90        	sub	a10, a10, a9
    420b:	fccb92        	addi	a9, a11, -4
    420e:	0a8a76        	loop	a10, 421c <NR_NoiseWithGain+0x60>
    4211:	601914        	ae_lq32f.iu	aeq0, a9, 4
    4214:	030214        	ae_mulfq32sp16s.l	aeq0, aeq0, aep0
    4217:	368a04        	ae_trunca32q48	a10, aeq0
    421a:	09a9           	s32i.n	a10, a9, 0

0000421c <NR_NoiseWithGain+0x60>:
    421c:	ce9747        	bne	a7, a4, 41ee <NR_NoiseWithGain+0x32>
    421f:	000106        	j	4227 <NR_NoiseWithGain+0x6b>

00004222 <NR_NoiseWithGain+0x66>:
    4222:	771b           	addi.n	a7, a7, 1
    4224:	fffd06        	j	421c <NR_NoiseWithGain+0x60>

00004227 <NR_NoiseWithGain+0x6b>:
    4227:	6f9242        	l16si	a4, a2, 222
    422a:	2f0252        	l8ui	a5, a2, 47
    422d:	402272        	l32i	a7, a2, 0x100
    4230:	446b           	addi.n	a4, a4, 6
    4232:	705242        	s16i	a4, a2, 224
    4235:	c59c           	beqz.n	a5, 4255 <NR_NoiseWithGain+0x99>
    4237:	08d342        	addmi	a4, a3, 0x800
    423a:	fca762        	movi	a6, 0x7fc
    423d:	4c0404        	ae_lp24.i	aep0, a4, 0
    4240:	336a           	add.n	a3, a3, a6
    4242:	600704        	ae_lq32f.i	aeq0, a7, 0
    4245:	072d           	mov.n	a2, a7
    4247:	0a8576        	loop	a5, 4255 <NR_NoiseWithGain+0x99>
    424a:	f8e071841f 	{ ae_lp24.iu	aep0, a4, 4; ae_mulrfq32sp24s.l	aeq1, aeq0, aep0 }
    424f:	601214        	ae_lq32f.iu	aeq0, a2, 4
    4252:	625314        	ae_sq32f.iu	aeq1, a3, 4

00004255 <NR_NoiseWithGain+0x99>:
    4255:	f01d           	retw.n

00004257 <NR_NoiseWithGain+0x9b>:
	...

00004258 <common_gain_limiter>:
    4258:	004136        	entry	a1, 32
    425b:	025d           	mov.n	a5, a2
    425d:	032d           	mov.n	a2, a3
    425f:	1015a6        	blti	a5, 1, 4273 <common_gain_limiter+0x1b>
    4262:	378444        	ae_truncp24a32x2	aep0, a4, a4
    4265:	401204        	ae_lp16f.i	aep1, a2, 0
    4268:	078576        	loop	a5, 4273 <common_gain_limiter+0x1b>
    426b:	d9222c922f 	{ ae_lp16f.iu	aep1, a2, 4; ae_maxp24s	aep2, aep0, aep1 }
    4270:	5222f4        	ae_sp24f.l.i	aep2, a2, -4

00004273 <common_gain_limiter+0x1b>:
    4273:	f01d           	retw.n

00004275 <common_gain_limiter+0x1d>:
    4275:	000000                                        ...

00004278 <NR_WienerFilter>:
    4278:	004136        	entry	a1, 32
    427b:	0268           	l32i.n	a6, a2, 0
    427d:	6e92a2        	l16si	a10, a2, 220
    4280:	7192b2        	l16si	a11, a2, 226
    4283:	2f0232        	l8ui	a3, a2, 47
    4286:	96cb           	addi.n	a9, a6, 12
    4288:	040c           	movi.n	a4, 0
    428a:	08d672        	addmi	a7, a6, 0x800
    428d:	0a5d           	mov.n	a5, a10
    428f:	098d           	mov.n	a8, a9
    4291:	07aab7        	bge	a10, a11, 429c <NR_WienerFilter+0x24>
    4294:	412282        	l32i	a8, a2, 0x104
    4297:	140c           	movi.n	a4, 1
    4299:	205bb0        	or	a5, a11, a11

0000429c <NR_WienerFilter+0x24>:
    429c:	7092c2        	l16si	a12, a2, 224
    429f:	72d5c0        	salt	a13, a5, a12
    42a2:	5355c0        	max	a5, a5, a12
    42a5:	9387d0        	movnez	a8, a7, a13
    42a8:	1c15a7        	beq	a5, a10, 42c8 <NR_WienerFilter+0x50>
    42ab:	939c           	beqz.n	a3, 42c8 <NR_WienerFilter+0x50>
    42ad:	c0b5a0        	sub	a11, a5, a10
    42b0:	a398b0        	movltz	a9, a8, a11
    42b3:	60a1b0        	abs	a10, a11
    42b6:	fcc992        	addi	a9, a9, -4
    42b9:	088376        	loop	a3, 42c5 <NR_WienerFilter+0x4d>
    42bc:	601914        	ae_lq32f.iu	aeq0, a9, 4
    42bf:	340aa4        	ae_srlaq56	aeq0, aeq0, a10
    42c2:	620904        	ae_sq32f.i	aeq0, a9, 0

000042c5 <NR_WienerFilter+0x4d>:
    42c5:	7112b2        	l16ui	a11, a2, 226

000042c8 <NR_WienerFilter+0x50>:
    42c8:	8b9d           	ae_sext16	a9, a11
    42ca:	1c1597        	beq	a5, a9, 42ea <NR_WienerFilter+0x72>
    42cd:	939c           	beqz.n	a3, 42ea <NR_WienerFilter+0x72>
    42cf:	4122b2        	l32i	a11, a2, 0x104
    42d2:	c0a590        	sub	a10, a5, a9
    42d5:	6091a0        	abs	a9, a10
    42d8:	a3b8a0        	movltz	a11, a8, a10
    42db:	fccba2        	addi	a10, a11, -4
    42de:	088376        	loop	a3, 42ea <NR_WienerFilter+0x72>
    42e1:	601a14        	ae_lq32f.iu	aeq0, a10, 4
    42e4:	3409a4        	ae_srlaq56	aeq0, aeq0, a9
    42e7:	620a04        	ae_sq32f.i	aeq0, a10, 0

000042ea <NR_WienerFilter+0x72>:
    42ea:	709292        	l16si	a9, a2, 224
    42ed:	191597        	beq	a5, a9, 430a <NR_WienerFilter+0x92>
    42f0:	639c           	beqz.n	a3, 430a <NR_WienerFilter+0x92>
    42f2:	c0a590        	sub	a10, a5, a9
    42f5:	b387a0        	movgez	a8, a7, a10
    42f8:	6091a0        	abs	a9, a10
    42fb:	fcc882        	addi	a8, a8, -4
    42fe:	088376        	loop	a3, 430a <NR_WienerFilter+0x92>
    4301:	601814        	ae_lq32f.iu	aeq0, a8, 4
    4304:	3409a4        	ae_srlaq56	aeq0, aeq0, a9
    4307:	620804        	ae_sq32f.i	aeq0, a8, 0
    430a:	6e5252        	s16i	a5, a2, 220
    430d:	705252        	s16i	a5, a2, 224
    4310:	715252        	s16i	a5, a2, 226
    4313:	412252        	l32i	a5, a2, 0x104
    4316:	668b           	addi.n	a6, a6, 8
    4318:	109316        	beqz	a3, 4425 <NR_WienerFilter+0x1ad>
    431b:	180c           	movi.n	a8, 1
    431d:	ffa092        	movi	a9, 255
    4320:	d8a0b2        	movi	a11, 216
    4323:	4502b4        	ae_lp16x2f.x	aep0, a2, a11
    4326:	600704        	ae_lq32f.i	aeq0, a7, 0
    4329:	604504        	ae_lq32f.i	aeq1, a5, 0
    432c:	118810        	slli	a8, a8, 15
    432f:	7fd992        	addmi	a9, a9, 0x7f00
    4332:	f96a13a66f 	{ or	a10, a6, a6; ae_addq56	aeq1, aeq1, aeq0 }
    4337:	05bd           	mov.n	a11, a5
    4339:	3a8376        	loop	a3, 4377 <NR_WienerFilter+0xff>
    433c:	609a14        	ae_lq32f.iu	aeq2, a10, 4
    433f:	0e00ff0caf 	{ ae_nsaq56s	a12, aeq0; ae_mulzaafq32sp16s.lh	aeq1, aeq2, aep0, aeq1, aep0 }
    4344:	08ed           	mov.n	a14, a8
    4346:	365d04        	ae_nsaq56s	a13, aeq1
    4349:	43ccd0        	min	a12, a12, a13
    434c:	e9ccd2        	addi	a13, a12, -23
    434f:	fc24fe9d4f 	{ ae_slaasq56s	aeq1, aeq1, a13; nop }
    4354:	369d04        	ae_trunca32q48	a13, aeq1
    4357:	38ccc2        	addi	a12, a12, 56
    435a:	340c94        	ae_sllaq56	aeq0, aeq0, a12
    435d:	4d8c           	beqz.n	a13, 4365 <NR_WienerFilter+0xed>
    435f:	368c04        	ae_trunca32q48	a12, aeq0
    4362:	d2ecd0        	quos	a14, a12, a13

00004365 <NR_WienerFilter+0xed>:
    4365:	0bdd           	mov.n	a13, a11
    4367:	c0c9e0        	sub	a12, a9, a14
    436a:	605d14        	ae_lq32f.iu	aeq1, a13, 4
    436d:	601714        	ae_lq32f.iu	aeq0, a7, 4
    4370:	f96a140bcf 	{ s32i.n	a12, a11, 0; ae_addq56	aeq1, aeq1, aeq0 }
    4375:	0dbd           	mov.n	a11, a13

00004377 <NR_WienerFilter+0xff>:
    4377:	79a082        	movi	a8, 121
    437a:	22c272        	addi	a7, a2, 34
    437d:	02b387        	bgeu	a3, a8, 4383 <NR_WienerFilter+0x10b>
    4380:	002b46        	j	4431 <NR_WienerFilter+0x1b9>
    4383:	8a9282        	l16si	a8, a2, 0x114
    4386:	059d           	mov.n	a9, a5
    4388:	10a1a2        	movi	a10, 0x110
    438b:	2418a6        	blti	a8, 1, 43b3 <NR_WienerFilter+0x13b>
    438e:	6022a4        	ae_lq32f.x	aeq0, a2, a10
    4391:	400704        	ae_lp16f.i	aep0, a7, 0
    4394:	d4e22c150f 	{ ae_lp16f.i	aep1, a5, 0; ae_mulfq32sp16s.l	aeq0, aeq0, aep0 }
    4399:	059d           	mov.n	a9, a5
    439b:	368f04        	ae_trunca32q48	a15, aeq0
    439e:	378ff4        	ae_truncp24a32x2	aep0, a15, a15
    43a1:	f03d           	nop.n
    43a3:	f03d           	nop.n
    43a5:	078876        	loop	a8, 43b0 <NR_WienerFilter+0x138>
    43a8:	d9222c992f 	{ ae_lp16f.iu	aep1, a9, 4; ae_maxp24s	aep2, aep0, aep1 }
    43ad:	5229f4        	ae_sp24f.l.i	aep2, a9, -4
    43b0:	8a9282        	l16si	a8, a2, 0x114

000043b3 <NR_WienerFilter+0x13b>:
    43b3:	1192a2        	l16si	a10, a2, 34
    43b6:	8b92b2        	l16si	a11, a2, 0x116
    43b9:	11aa00        	slli	a10, a10, 16
    43bc:	c08b80        	sub	a8, a11, a8
    43bf:	378aa4        	ae_truncp24a32x2	aep0, a10, a10
    43c2:	1018a6        	blti	a8, 1, 43d6 <NR_WienerFilter+0x15e>
    43c5:	401904        	ae_lp16f.i	aep1, a9, 0
    43c8:	078876        	loop	a8, 43d3 <NR_WienerFilter+0x15b>
    43cb:	d9222c992f 	{ ae_lp16f.iu	aep1, a9, 4; ae_maxp24s	aep2, aep0, aep1 }
    43d0:	5229f4        	ae_sp24f.l.i	aep2, a9, -4

000043d3 <NR_WienerFilter+0x15b>:
    43d3:	8b92b2        	l16si	a11, a2, 0x116

000043d6 <NR_WienerFilter+0x15e>:
    43d6:	ef9aa1        	l32r	a10, 240 (7b90 <wb_sb_ptn>)
    43d9:	0ca182        	movi	a8, 0x10c
    43dc:	603824        	ae_lq32f.xu	aeq0, a8, a2
    43df:	d4e2208ace 	{ l16si	a12, a10, 16; ae_mulfq32sp16s.l	aeq0, aeq0, aep0 }
    43e4:	368d04        	ae_trunca32q48	a13, aeq0
    43e7:	c0bcb0        	sub	a11, a12, a11
    43ea:	378dd4        	ae_truncp24a32x2	aep0, a13, a13
    43ed:	121ba6        	blti	a11, 1, 4403 <NR_WienerFilter+0x18b>
    43f0:	401904        	ae_lp16f.i	aep1, a9, 0
    43f3:	f03d           	nop.n
    43f5:	078b76        	loop	a11, 4400 <NR_WienerFilter+0x188>
    43f8:	d9222c992f 	{ ae_lp16f.iu	aep1, a9, 4; ae_maxp24s	aep2, aep0, aep1 }
    43fd:	5229f4        	ae_sp24f.l.i	aep2, a9, -4

00004400 <NR_WienerFilter+0x188>:
    4400:	089ac2        	l16si	a12, a10, 16

00004403 <NR_WienerFilter+0x18b>:
    4403:	501804        	ae_lp24f.i	aep1, a8, 0
    4406:	ea2195a3cf 	{ sub	a10, a3, a12; ae_mulfp24s.ll	aeq0, aep0, aep1 }
    440b:	368804        	ae_trunca32q48	a8, aeq0
    440e:	371aa6        	blti	a10, 1, 4449 <NR_WienerFilter+0x1d1>
    4411:	378884        	ae_truncp24a32x2	aep0, a8, a8
    4414:	401904        	ae_lp16f.i	aep1, a9, 0
    4417:	078a76        	loop	a10, 4422 <NR_WienerFilter+0x1aa>
    441a:	d9222c992f 	{ ae_lp16f.iu	aep1, a9, 4; ae_maxp24s	aep2, aep0, aep1 }
    441f:	5229f4        	ae_sp24f.l.i	aep2, a9, -4

00004422 <NR_WienerFilter+0x1aa>:
    4422:	0008c6        	j	4449 <NR_WienerFilter+0x1d1>

00004425 <NR_WienerFilter+0x1ad>:
    4425:	119282        	l16si	a8, a2, 34
    4428:	22c272        	addi	a7, a2, 34
    442b:	118800        	slli	a8, a8, 16
    442e:	0005c6        	j	4449 <NR_WienerFilter+0x1d1>

00004431 <NR_WienerFilter+0x1b9>:
    4431:	278d           	ae_l16si.n	a8, a7, 0
    4433:	400504        	ae_lp16f.i	aep0, a5, 0
    4436:	059d           	mov.n	a9, a5
    4438:	118800        	slli	a8, a8, 16
    443b:	379884        	ae_truncp24a32x2	aep1, a8, a8
    443e:	078376        	loop	a3, 4449 <NR_WienerFilter+0x1d1>
    4441:	d9062c892f 	{ ae_lp16f.iu	aep0, a9, 4; ae_maxp24s	aep2, aep1, aep0 }
    4446:	5229f4        	ae_sp24f.l.i	aep2, a9, -4

00004449 <NR_WienerFilter+0x1d1>:
    4449:	4422e2        	l32i	a14, a2, 0x110
    444c:	400704        	ae_lp16f.i	aep0, a7, 0
    444f:	360e34        	ae_cvtq48a32s	aeq0, a14
    4452:	d4e23002fe 	{ l32i	a15, a2, 0; ae_mulfq32sp16s.l	aeq0, aeq0, aep0 }
    4457:	f8e780009f 	{ movi	a9, 0; ae_roundsp24q48sym	aep0, aeq0 }
    445c:	fccf72        	addi	a7, a15, -4
    445f:	528714        	ae_sp24f.l.iu	aep0, a7, 4
    4462:	528714        	ae_sp24f.l.iu	aep0, a7, 4
    4465:	528714        	ae_sp24f.l.iu	aep0, a7, 4
    4468:	092316        	beqz	a3, 44fe <NR_WienerFilter+0x286>
    446b:	4122a2        	l32i	a10, a2, 0x104
    446e:	500a04        	ae_lp24f.i	aep0, a10, 0
    4471:	178376        	loop	a3, 448c <NR_WienerFilter+0x214>
    4474:	601614        	ae_lq32f.iu	aeq0, a6, 4
    4477:	080524        	ae_mulfq32sp24s.l	aeq0, aeq0, aep0
    447a:	080514        	ae_mulrfq32sp24s.l	aeq0, aeq0, aep0
    447d:	368b04        	ae_trunca32q48	a11, aeq0
    4480:	620a04        	ae_sq32f.i	aeq0, a10, 0
    4483:	528714        	ae_sp24f.l.iu	aep0, a7, 4
    4486:	508a14        	ae_lp24f.iu	aep0, a10, 4
    4489:	5399b0        	max	a9, a9, a11
    448c:	b49c           	beqz.n	a4, 44ab <NR_WienerFilter+0x233>
    448e:	719262        	l16si	a6, a2, 226
    4491:	40e940        	nsa	a4, a9
    4494:	c06640        	sub	a6, a6, a4
    4497:	715262        	s16i	a6, a2, 226
    449a:	d38c           	beqz.n	a3, 44ab <NR_WienerFilter+0x233>
    449c:	401400        	ssl	a4
    449f:	088376        	loop	a3, 44ab <NR_WienerFilter+0x233>
    44a2:	0538           	l32i.n	a3, a5, 0
    44a4:	a13300        	sll	a3, a3
    44a7:	0539           	s32i.n	a3, a5, 0
    44a9:	554b           	addi.n	a5, a5, 4
    44ab:	2e0232        	l8ui	a3, a2, 46
    44ae:	0fa042        	movi	a4, 15
    44b1:	05a052        	movi	a5, 5
    44b4:	413330        	srli	a3, a3, 3
    44b7:	935430        	movnez	a5, a4, a3
    44ba:	378884        	ae_truncp24a32x2	aep0, a8, a8
    44bd:	028576        	loop	a5, 44c3 <NR_WienerFilter+0x24b>
    44c0:	528714        	ae_sp24f.l.iu	aep0, a7, 4

000044c3 <NR_WienerFilter+0x24b>:
    44c3:	00a142        	movi	a4, 0x100
    44c6:	80a052        	movi	a5, 128
    44c9:	0022f2        	l32i	a15, a2, 0
    44cc:	935430        	movnez	a5, a4, a3
    44cf:	3a2232        	l32i	a3, a2, 232
    44d2:	392222        	l32i	a2, a2, 228
    44d5:	fccf42        	addi	a4, a15, -4
    44d8:	5c1304        	ae_lp24x2f.i	aep1, a3, 0
    44db:	402204        	ae_lp16f.i	aep2, a2, 0
    44de:	508414        	ae_lp24f.iu	aep0, a4, 4
    44e1:	178576        	loop	a5, 44fc <NR_WienerFilter+0x284>
    44e4:	011254        	ae_mulfp24s.hl	aeq0, aep1, aep2
    44e7:	ea45ef931f 	{ ae_lp24x2f.iu	aep1, a3, 8; ae_mulfp24s.ll	aeq1, aep1, aep2 }
    44ec:	f8e02ca21f 	{ ae_lp16f.iu	aep2, a2, 2; ae_mulrfq32sp24s.l	aeq0, aeq0, aep0 }
    44f1:	f8e16e841f 	{ ae_lp24f.iu	aep0, a4, 4; ae_mulrfq32sp24s.l	aeq1, aeq1, aep0 }
    44f6:	6203e4        	ae_sq32f.i	aeq0, a3, -8
    44f9:	6213f4        	ae_sq32f.i	aeq1, a3, -4
    44fc:	f01d           	retw.n

000044fe <NR_WienerFilter+0x286>:
    44fe:	090c           	movi.n	a9, 0
    4500:	ffe206        	j	448c <NR_WienerFilter+0x214>

00004503 <NR_WienerFilter+0x28b>:
	...

00004504 <ifft_wo_single>:
    4504:	004136        	entry	a1, 32
    4507:	ef5091        	l32r	a9, 248 (7870 <NR_immediate_num>)
    450a:	ef4a71        	l32r	a7, 234 (8160 <fftorder_512_12b>)
    450d:	2e02c2        	l8ui	a12, a2, 46
    4510:	fc20c200af 	{ movi	a10, 0x200; ae_zerop48	aep1 }
    4515:	00a142        	movi	a4, 0x100
    4518:	0258           	l32i.n	a5, a2, 0
    451a:	440904        	ae_lp16x2f.i	aep0, a9, 0
    451d:	442914        	ae_lp16x2f.i	aep2, a9, 4
    4520:	f72b           	addi.n	a15, a7, 2
    4522:	974b           	addi.n	a9, a7, 4
    4524:	4173c0        	srli	a7, a12, 3
    4527:	934a70        	movnez	a4, a10, a7
    452a:	3a22a2        	l32i	a10, a2, 232
    452d:	060c           	movi.n	a6, 0
    452f:	2b0c           	movi.n	a11, 2
    4531:	480c           	movi.n	a8, 4
    4533:	938b70        	movnez	a8, a11, a7
    4536:	016562        	s32i	a6, a5, 4
    4539:	006562        	s32i	a6, a5, 0
    453c:	41b140        	srli	a11, a4, 1
    453f:	939f70        	movnez	a9, a15, a7
    4542:	5c3a04        	ae_lp24x2f.i	aep3, a10, 0
    4545:	168b76        	loop	a11, 455f <ifft_wo_single+0x5b>
    4548:	df2dfe391f 	{ ae_selp24.hl	aep4, aep3, aep1; ae_selp24.lh	aep7, aep3, aep1 }
    454d:	f012a0096e 	{ l16si	a6, a9, 0; ae_mulzaafp24s.hh.ll	aeq0, aep4, aep0 }
    4552:	f05eefba1f 	{ ae_lp24x2f.iu	aep3, a10, 8; ae_mulzaafp24s.hh.ll	aeq1, aep7, aep2 }
    4557:	998a           	add.n	a9, a9, a8
    4559:	098d44        	ae_truncp24q48x2	aep4, aeq0, aeq1
    455c:	5f4564        	ae_sp24x2f.x	aep4, a5, a6

0000455f <ifft_wo_single+0x5b>:
    455f:	fd5800f0cf 	{ movi	a12, 240; ae_negsp24s	aep2, aep2 }
    4564:	78a062        	movi	a6, 120
    4567:	bb0b           	addi.n	a11, a11, -1
    4569:	5cbae4        	ae_lp24x2f.iu	aep3, a10, -16
    456c:	936c70        	movnez	a6, a12, a7
    456f:	168b76        	loop	a11, 4589 <ifft_wo_single+0x85>
    4572:	df2dfe391f 	{ ae_selp24.hl	aep4, aep3, aep1; ae_selp24.lh	aep7, aep3, aep1 }
    4577:	f052a009be 	{ l16si	a11, a9, 0; ae_mulzaafp24s.hh.ll	aeq0, aep4, aep2 }
    457c:	f01eefbaff 	{ ae_lp24x2f.iu	aep3, a10, -8; ae_mulzaafp24s.hh.ll	aeq1, aep7, aep0 }
    4581:	998a           	add.n	a9, a9, a8
    4583:	098d44        	ae_truncp24q48x2	aep4, aeq0, aeq1
    4586:	5f45b4        	ae_sp24x2f.x	aep4, a5, a11
    4589:	ef2b81        	l32r	a8, 238 (50a4 <sw_fft_32b_2811>)
    458c:	8c0c           	movi.n	a12, 8
    458e:	0d0c           	movi.n	a13, 0
    4590:	1e0c           	movi.n	a14, 1
    4592:	05ad           	mov.n	a10, a5
    4594:	04bd           	mov.n	a11, a4
    4596:	0008e0        	callx8	a8
    4599:	3f22b2        	l32i	a11, a2, 252
    459c:	139292        	l16si	a9, a2, 38
    459f:	b87c           	movi.n	a8, -5
    45a1:	cd7c           	movi.n	a13, -4
    45a3:	3d22e2        	l32i	a14, a2, 244
    45a6:	93d870        	movnez	a13, a8, a7
    45a9:	10a1c2        	movi	a12, 0x110
    45ac:	88a082        	movi	a8, 136
    45af:	938c70        	movnez	a8, a12, a7
    45b2:	fdaa           	add.n	a15, a13, a10
    45b4:	fecb72        	addi	a7, a11, -2
    45b7:	c02f90        	sub	a2, a15, a9
    45ba:	fec3c2        	addi	a12, a3, -2
    45bd:	600504        	ae_lq32f.i	aeq0, a5, 0
    45c0:	fecea2        	addi	a10, a14, -2
    45c3:	9098e0        	addx2	a9, a8, a14
    45c6:	07dd           	mov.n	a13, a7
    45c8:	1c8676        	loop	a6, 45e8 <ifft_wo_single+0xe4>
    45cb:	fc24fe820f 	{ ae_slaasq56s	aeq0, aeq0, a2; nop }
    45d0:	fc446c8d1f 	{ ae_lp16f.iu	aep0, a13, 2; ae_roundsq32sym	aeq0, aeq0 }
    45d5:	4099f4        	ae_lp16f.iu	aep1, a9, -2
    45d8:	ea20ecfa1f 	{ ae_lp16f.iu	aep7, a10, 2; ae_mulfp24s.hl	aeq1, aep0, aep1 }
    45dd:	f49efd152f 	{ ae_lq32f.iu	aeq0, a5, 8; ae_mulafq32sp16s.l	aeq1, aeq0, aep7 }
    45e2:	1b9e04        	ae_roundsp16q48sym	aep0, aeq1
    45e5:	428c14        	ae_sp16f.l.iu	aep0, a12, 2
    45e8:	11a6f0        	slli	a10, a6, 1
    45eb:	c0a4a0        	sub	a10, a4, a10
    45ee:	291aa6        	blti	a10, 1, 461b <ifft_wo_single+0x117>
    45f1:	9046b0        	addx2	a4, a6, a11
    45f4:	90f8b0        	addx2	a15, a8, a11
    45f7:	400304        	ae_lp16f.i	aep0, a3, 0
    45fa:	fec442        	addi	a4, a4, -2
    45fd:	fecf62        	addi	a6, a15, -2
    4600:	178a76        	loop	a10, 461b <ifft_wo_single+0x117>
    4603:	fc0c2c99ff 	{ ae_lp16f.iu	aep1, a9, -2; ae_cvtq48p24s.l	aeq0, aep0 }
    4608:	40e614        	ae_lp16f.iu	aep6, a6, 2
    460b:	e23b2cf41f 	{ ae_lp16f.iu	aep7, a4, 2; ae_mulafp24s.hl	aeq0, aep6, aep1 }
    4610:	408314        	ae_lp16f.iu	aep0, a3, 2
    4613:	f9e3b2760f 	{ ae_sp16f.l.i	aep7, a6, 0; ae_roundsp16q48sym	aep2, aeq0 }
    4618:	4223f4        	ae_sp16f.l.i	aep2, a3, -2

0000461b <ifft_wo_single+0x117>:
    461b:	600504        	ae_lq32f.i	aeq0, a5, 0
    461e:	0c8876        	loop	a8, 462e <ifft_wo_single+0x12a>
    4621:	fc24fe824f 	{ ae_slaasq56s	aeq1, aeq0, a2; nop }
    4626:	f8ebbd152f 	{ ae_lq32f.iu	aeq0, a5, 8; ae_roundsp16q48sym	aep0, aeq1 }
    462b:	428714        	ae_sp16f.l.iu	aep0, a7, 2

0000462e <ifft_wo_single+0x12a>:
    462e:	f01d           	retw.n

00004630 <airoha_divide>:
    4630:	004136        	entry	a1, 32
    4633:	204320        	or	a4, a3, a2
    4636:	0d8416        	beqz	a4, 4712 <airoha_divide+0xe2>
    4639:	0de216        	beqz	a2, 471b <airoha_divide+0xeb>
    463c:	0e2316        	beqz	a3, 4722 <airoha_divide+0xf2>
    463f:	609120        	abs	a9, a2
    4642:	60b130        	abs	a11, a3
    4645:	055f90        	extui	a5, a9, 31, 1
    4648:	056fb0        	extui	a6, a11, 31, 1
    464b:	400500        	ssr	a5
    464e:	304320        	xor	a4, a3, a2
    4651:	912090        	srl	a2, a9
    4654:	400600        	ssr	a6
    4657:	91c0b0        	srl	a12, a11
    465a:	40e2d0        	nsa	a13, a2
    465d:	40ec30        	nsa	a3, a12
    4660:	e31b           	addi.n	a14, a3, 1
    4662:	401d00        	ssl	a13
    4665:	317fb0        	srai	a7, a11, 31
    4668:	a13200        	sll	a3, a2
    466b:	401e00        	ssl	a14
    466e:	a75a           	add.n	a10, a7, a5
    4670:	a17c00        	sll	a7, a12
    4673:	f55070        	extui	a5, a7, 16, 16
    4676:	c26350        	quou	a6, a3, a5
    4679:	829650        	mull	a9, a6, a5
    467c:	978d           	ae_zext16	a8, a7
    467e:	c09390        	sub	a9, a3, a9
    4681:	828680        	mull	a8, a6, a8
    4684:	c0fed0        	sub	a15, a14, a13
    4687:	119900        	slli	a9, a9, 16
    468a:	2afa           	add.n	a2, a10, a15
    468c:	09b987        	bgeu	a9, a8, 4699 <airoha_divide+0x69>

0000468f <airoha_divide+0x5f>:
    468f:	997a           	add.n	a9, a9, a7
    4691:	660b           	addi.n	a6, a6, -1
    4693:	02b987        	bgeu	a9, a8, 4699 <airoha_divide+0x69>
    4696:	f5b977        	bgeu	a9, a7, 468f <airoha_divide+0x5f>
    4699:	c0e980        	sub	a14, a9, a8
    469c:	c25e50        	quou	a5, a14, a5
    469f:	11a600        	slli	a10, a6, 16
    46a2:	623370        	saltu	a3, a3, a7
    46a5:	2b0c           	movi.n	a11, 2
    46a7:	c09b30        	sub	a9, a11, a3
    46aa:	5a5a           	add.n	a5, a10, a5
    46ac:	f5f060        	extui	a15, a6, 16, 16
    46af:	6265a0        	saltu	a6, a5, a10
    46b2:	d59a           	add.n	a13, a5, a9
    46b4:	fc1c           	movi.n	a12, 31
    46b6:	6f6a           	add.n	a6, a15, a6
    46b8:	625d50        	saltu	a5, a13, a5
    46bb:	565a           	add.n	a5, a6, a5
    46bd:	400900        	ssr	a9
    46c0:	3079c0        	xor	a7, a9, a12
    46c3:	9180d0        	srl	a8, a13
    46c6:	401700        	ssl	a7
    46c9:	1165f0        	slli	a6, a5, 1
    46cc:	a16600        	sll	a6, a6
    46cf:	400900        	ssr	a9
    46d2:	0f0c           	movi.n	a15, 0
    46d4:	206860        	or	a6, a8, a6
    46d7:	915050        	srl	a5, a5
    46da:	1e0c           	movi.n	a14, 1
    46dc:	7286f0        	salt	a8, a6, a15
    46df:	629f50        	saltu	a9, a15, a5
    46e2:	1a0c           	movi.n	a10, 1
    46e4:	3073e0        	xor	a7, a3, a14
    46e7:	839850        	moveqz	a9, a8, a5
    46ea:	015a20        	slli	a5, a10, 30
    46ed:	eedcb1        	l32r	a11, 260 (80000001 <_end+0x7fff7975>)
    46f0:	727a           	add.n	a7, a2, a7
    46f2:	fc7c           	movi.n	a12, -1
    46f4:	835690        	moveqz	a5, a6, a9
    46f7:	4131c0        	srli	a3, a12, 1
    46fa:	60e050        	neg	a14, a5
    46fd:	d79a           	add.n	a13, a7, a9
    46ff:	f23c           	movi.n	a2, 63
    4701:	c0cdf2        	addi	a15, a13, -64
    4704:	a33b40        	movltz	a3, a11, a4
    4707:	b3e540        	movgez	a14, a5, a4
    470a:	932df0        	movnez	a2, a13, a15
    470d:	933ef0        	movnez	a3, a14, a15
    4710:	f01d           	retw.n

00004712 <airoha_divide+0xe2>:
    4712:	f37c           	movi.n	a3, -1
    4714:	020c           	movi.n	a2, 0
    4716:	413130        	srli	a3, a3, 1
    4719:	f01d           	retw.n

0000471b <airoha_divide+0xeb>:
    471b:	030c           	movi.n	a3, 0
    471d:	c1af22        	movi	a2, -63
    4720:	f01d           	retw.n

00004722 <airoha_divide+0xf2>:
    4722:	eecf31        	l32r	a3, 260 (80000001 <_end+0x7fff7975>)
    4725:	040c           	movi.n	a4, 0
    4727:	f57c           	movi.n	a5, -1
    4729:	724420        	salt	a4, a4, a2
    472c:	415150        	srli	a5, a5, 1
    472f:	f23c           	movi.n	a2, 63
    4731:	933540        	movnez	a3, a5, a4
    4734:	f01d           	retw.n

00004736 <airoha_divide+0x106>:
	...

00004738 <airoha_divide_limit_1>:
    4738:	006136        	entry	a1, 48
    473b:	ee6e41        	l32r	a4, f4 (4630 <airoha_divide>)
    473e:	02ad           	mov.n	a10, a2
    4740:	03bd           	mov.n	a11, a3
    4742:	0004e0        	callx8	a4
    4745:	360b34        	ae_cvtq48a32s	aeq0, a11
    4748:	fc24fe8a0f 	{ ae_slaasq56s	aeq0, aeq0, a10; nop }
    474d:	060934        	ae_roundsq32asym	aeq0, aeq0
    4750:	620134        	ae_sq32f.i	aeq0, a1, 12
    4753:	3128           	l32i.n	a2, a1, 12
    4755:	f01d           	retw.n

00004757 <airoha_divide_limit_1+0x1f>:
	...

00004758 <xtensa_fft4_32b>:
    4758:	00c136        	entry	a1, 96
    475b:	600254        	ae_lq32f.i	aeq0, a2, 20
    475e:	604274        	ae_lq32f.i	aeq1, a2, 28
    4761:	fa663d626f 	{ ae_lq32f.i	aeq3, a2, 24; ae_addq56	aeq2, aeq0, aeq1 }
    4766:	f826fa217f 	{ ae_sq56s.i	aeq2, a1, 56; ae_subq56	aeq0, aeq0, aeq1 }
    476b:	630134        	ae_sq56s.i	aeq0, a1, 24
    476e:	600244        	ae_lq32f.i	aeq0, a2, 16
    4771:	fa7a3d220f 	{ ae_lq32f.i	aeq1, a2, 0; ae_addq56	aeq2, aeq3, aeq0 }
    4776:	f83afa211f 	{ ae_sq56s.i	aeq2, a1, 8; ae_subq56	aeq0, aeq3, aeq0 }
    477b:	630164        	ae_sq56s.i	aeq0, a1, 48
    477e:	600224        	ae_lq32f.i	aeq0, a2, 8
    4781:	fb6a3dc11f 	{ ae_lq56.i	aeq2, a1, 8; ae_addq56	aeq3, aeq1, aeq0 }
    4786:	01bac4        	ae_addq56	aeq2, aeq3, aeq2
    4789:	f82afa212f 	{ ae_sq56s.i	aeq2, a1, 16; ae_subq56	aeq0, aeq1, aeq0 }
    478e:	618124        	ae_lq56.i	aeq2, a1, 16
    4791:	622204        	ae_sq32f.i	aeq2, a2, 0
    4794:	618114        	ae_lq56.i	aeq2, a1, 8
    4797:	f9bafd621f 	{ ae_lq32f.i	aeq3, a2, 4; ae_subq56	aeq1, aeq3, aeq2 }
    479c:	631114        	ae_sq56s.i	aeq1, a1, 8
    479f:	614134        	ae_lq56.i	aeq1, a1, 24
    47a2:	fb663a315f 	{ ae_sq56s.i	aeq3, a1, 40; ae_addq56	aeq3, aeq0, aeq1 }
    47a7:	f826fd423f 	{ ae_lq32f.i	aeq2, a2, 12; ae_subq56	aeq0, aeq0, aeq1 }
    47ac:	633124        	ae_sq56s.i	aeq3, a1, 16
    47af:	614114        	ae_lq56.i	aeq1, a1, 8
    47b2:	61c154        	ae_lq56.i	aeq3, a1, 40
    47b5:	630134        	ae_sq56s.i	aeq0, a1, 24
    47b8:	f9fa39924f 	{ ae_sq32f.i	aeq1, a2, 16; ae_addq56	aeq1, aeq3, aeq2 }
    47bd:	632144        	ae_sq56s.i	aeq2, a1, 32
    47c0:	618124        	ae_lq56.i	aeq2, a1, 16
    47c3:	610174        	ae_lq56.i	aeq0, a1, 56
    47c6:	fa6a39a22f 	{ ae_sq32f.i	aeq2, a2, 8; ae_addq56	aeq2, aeq1, aeq0 }
    47cb:	f82afa211f 	{ ae_sq56s.i	aeq2, a1, 8; ae_subq56	aeq0, aeq1, aeq0 }
    47d0:	618134        	ae_lq56.i	aeq2, a1, 24
    47d3:	622264        	ae_sq32f.i	aeq2, a2, 24
    47d6:	618144        	ae_lq56.i	aeq2, a1, 32
    47d9:	f9baf9825f 	{ ae_sq32f.i	aeq0, a2, 20; ae_subq56	aeq1, aeq3, aeq2 }
    47de:	610164        	ae_lq56.i	aeq0, a1, 48
    47e1:	fb6a3dc11f 	{ ae_lq56.i	aeq2, a1, 8; ae_addq56	aeq3, aeq1, aeq0 }
    47e6:	f92af9a21f 	{ ae_sq32f.i	aeq2, a2, 4; ae_subq56	aeq1, aeq1, aeq0 }
    47eb:	623234        	ae_sq32f.i	aeq3, a2, 12
    47ee:	621274        	ae_sq32f.i	aeq1, a2, 28
    47f1:	f01d           	retw.n

000047f3 <xtensa_fft4_32b+0x9b>:
	...

000047f4 <xtensa_fft8_32b>:
    47f4:	022136        	entry	a1, 0x110
    47f7:	600254        	ae_lq32f.i	aeq0, a2, 20
    47fa:	604274        	ae_lq32f.i	aeq1, a2, 28
    47fd:	fb663d426f 	{ ae_lq32f.i	aeq2, a2, 24; ae_addq56	aeq3, aeq0, aeq1 }
    4802:	f826fa315f 	{ ae_sq56s.i	aeq3, a1, 40; ae_subq56	aeq0, aeq0, aeq1 }
    4807:	630124        	ae_sq56s.i	aeq0, a1, 16
    480a:	600244        	ae_lq32f.i	aeq0, a2, 16
    480d:	f9723d620f 	{ ae_lq32f.i	aeq3, a2, 0; ae_addq56	aeq1, aeq2, aeq0 }
    4812:	f832c7816e 	{ addi	a6, a1, 120; ae_subq56	aeq0, aeq2, aeq0 }
    4817:	630134        	ae_sq56s.i	aeq0, a1, 24
    481a:	600224        	ae_lq32f.i	aeq0, a2, 8
    481d:	01b8c4        	ae_addq56	aeq2, aeq3, aeq0
    4820:	fa3afa211f 	{ ae_sq56s.i	aeq2, a1, 8; ae_subq56	aeq2, aeq3, aeq0 }
    4825:	61c114        	ae_lq56.i	aeq3, a1, 8
    4828:	f83efd621f 	{ ae_lq32f.i	aeq3, a2, 4; ae_subq56	aeq0, aeq3, aeq1 }
    482d:	6306c4        	ae_sq56s.i	aeq0, a6, -32
    4830:	610124        	ae_lq56.i	aeq0, a1, 16
    4833:	fb723a314f 	{ ae_sq56s.i	aeq3, a1, 32; ae_addq56	aeq3, aeq2, aeq0 }
    4838:	fa32fd86cf 	{ ae_lq56.i	aeq0, a6, -32; ae_subq56	aeq2, aeq2, aeq0 }
    483d:	6316e4        	ae_sq56s.i	aeq1, a6, -16
    4840:	620244        	ae_sq32f.i	aeq0, a2, 16
    4843:	604234        	ae_lq32f.i	aeq1, a2, 12
    4846:	610144        	ae_lq56.i	aeq0, a1, 32
    4849:	f8663a364f 	{ ae_sq56s.i	aeq3, a6, 32; ae_addq56	aeq0, aeq0, aeq1 }
    484e:	623224        	ae_sq32f.i	aeq3, a2, 8
    4851:	61c154        	ae_lq56.i	aeq3, a1, 40
    4854:	fba6fa06df 	{ ae_sq56s.i	aeq0, a6, -24; ae_subq56	aeq3, aeq0, aeq3 }
    4859:	610144        	ae_lq56.i	aeq0, a1, 32
    485c:	f826fa36ff 	{ ae_sq56s.i	aeq3, a6, -8; ae_subq56	aeq0, aeq0, aeq1 }
    4861:	623254        	ae_sq32f.i	aeq3, a2, 20
    4864:	61c134        	ae_lq56.i	aeq3, a1, 24
    4867:	f9e602023e 	{ addi	a3, a2, 32; ae_addq56	aeq1, aeq0, aeq3 }
    486c:	f8a6fa163f 	{ ae_sq56s.i	aeq1, a6, 24; ae_subq56	aeq0, aeq0, aeq3 }
    4871:	621234        	ae_sq32f.i	aeq1, a2, 12
    4874:	604304        	ae_lq32f.i	aeq1, a3, 0
    4877:	6316b4        	ae_sq56s.i	aeq1, a6, -40
    487a:	632664        	ae_sq56s.i	aeq2, a6, 48
    487d:	622264        	ae_sq32f.i	aeq2, a2, 24
    4880:	608314        	ae_lq32f.i	aeq2, a3, 4
    4883:	630654        	ae_sq56s.i	aeq0, a6, 40
    4886:	620274        	ae_sq32f.i	aeq0, a2, 28
    4889:	600374        	ae_lq32f.i	aeq0, a3, 28
    488c:	01d142        	addmi	a4, a1, 0x100
    488f:	632134        	ae_sq56s.i	aeq2, a1, 24
    4892:	608324        	ae_lq32f.i	aeq2, a3, 8
    4895:	60c334        	ae_lq32f.i	aeq3, a3, 12
    4898:	604354        	ae_lq32f.i	aeq1, a3, 20
    489b:	630164        	ae_sq56s.i	aeq0, a1, 48
    489e:	6106b4        	ae_lq56.i	aeq0, a6, -40
    48a1:	f8e23a312f 	{ ae_sq56s.i	aeq3, a1, 16; ae_addq56	aeq0, aeq0, aeq2 }
    48a6:	60c344        	ae_lq32f.i	aeq3, a3, 16
    48a9:	631144        	ae_sq56s.i	aeq1, a1, 32
    48ac:	604364        	ae_lq32f.i	aeq1, a3, 24
    48af:	b0c442        	addi	a4, a4, -80
    48b2:	01d152        	addmi	a5, a1, 0x100
    48b5:	fb7e3a361f 	{ ae_sq56s.i	aeq3, a6, 8; ae_addq56	aeq3, aeq3, aeq1 }
    48ba:	632604        	ae_sq56s.i	aeq2, a6, 0
    48bd:	631624        	ae_sq56s.i	aeq1, a6, 16
    48c0:	620404        	ae_sq32f.i	aeq0, a4, 0
    48c3:	608404        	ae_lq32f.i	aeq2, a4, 0
    48c6:	b4c552        	addi	a5, a5, -76
    48c9:	6326a4        	ae_sq56s.i	aeq2, a6, -48
    48cc:	623504        	ae_sq32f.i	aeq3, a5, 0
    48cf:	01d1a2        	addmi	a10, a1, 0x100
    48d2:	610134        	ae_lq56.i	aeq0, a1, 24
    48d5:	61c124        	ae_lq56.i	aeq3, a1, 16
    48d8:	f8e63d450f 	{ ae_lq32f.i	aeq2, a5, 0; ae_addq56	aeq0, aeq0, aeq3 }
    48dd:	b8ca42        	addi	a4, a10, -72
    48e0:	632694        	ae_sq56s.i	aeq2, a6, -56
    48e3:	620404        	ae_sq32f.i	aeq0, a4, 0
    48e6:	01d1b2        	addmi	a11, a1, 0x100
    48e9:	610144        	ae_lq56.i	aeq0, a1, 32
    48ec:	61c164        	ae_lq56.i	aeq3, a1, 48
    48ef:	f8e63d440f 	{ ae_lq32f.i	aeq2, a4, 0; ae_addq56	aeq0, aeq0, aeq3 }
    48f4:	bccb52        	addi	a5, a11, -68
    48f7:	632174        	ae_sq56s.i	aeq2, a1, 56
    48fa:	620504        	ae_sq32f.i	aeq0, a5, 0
    48fd:	01d1c2        	addmi	a12, a1, 0x100
    4900:	610114        	ae_lq56.i	aeq0, a1, 8
    4903:	61c6e4        	ae_lq56.i	aeq3, a6, -16
    4906:	f9e63d450f 	{ ae_lq32f.i	aeq2, a5, 0; ae_addq56	aeq1, aeq0, aeq3 }
    490b:	c0cc42        	addi	a4, a12, -64
    490e:	632114        	ae_sq56s.i	aeq2, a1, 8
    4911:	621404        	ae_sq32f.i	aeq1, a4, 0
    4914:	01d1d2        	addmi	a13, a1, 0x100
    4917:	61c154        	ae_lq56.i	aeq3, a1, 40
    491a:	6146d4        	ae_lq56.i	aeq1, a6, -24
    491d:	f9ee0c4d5e 	{ addi	a5, a13, -60; ae_addq56	aeq1, aeq1, aeq3 }
    4922:	60c404        	ae_lq32f.i	aeq3, a4, 0
    4925:	621504        	ae_sq32f.i	aeq1, a5, 0
    4928:	6186a4        	ae_lq56.i	aeq2, a6, -48
    492b:	614694        	ae_lq56.i	aeq1, a6, -56
    492e:	f9763d450f 	{ ae_lq32f.i	aeq2, a5, 0; ae_addq56	aeq1, aeq2, aeq1 }
    4933:	f83ec082bf 	{ movi	a11, 130; ae_subq56	aeq0, aeq3, aeq1 }
    4938:	f97e3a015f 	{ ae_sq56s.i	aeq0, a1, 40; ae_addq56	aeq1, aeq3, aeq1 }
    493d:	610154        	ae_lq56.i	aeq0, a1, 40
    4940:	620304        	ae_sq32f.i	aeq0, a3, 0
    4943:	61c114        	ae_lq56.i	aeq3, a1, 8
    4946:	610174        	ae_lq56.i	aeq0, a1, 56
    4949:	fbe639920f 	{ ae_sq32f.i	aeq1, a2, 0; ae_addq56	aeq3, aeq0, aeq3 }
    494e:	016fc4        	ae_subq56	aeq1, aeq2, aeq3
    4951:	01d1e2        	addmi	a14, a1, 0x100
    4954:	f8f639931f 	{ ae_sq32f.i	aeq1, a3, 4; ae_addq56	aeq0, aeq2, aeq3 }
    4959:	c8ce42        	addi	a4, a14, -56
    495c:	6146c4        	ae_lq56.i	aeq1, a6, -32
    495f:	620214        	ae_sq32f.i	aeq0, a2, 4
    4962:	621404        	ae_sq32f.i	aeq1, a4, 0
    4965:	01d1f2        	addmi	a15, a1, 0x100
    4968:	cccf52        	addi	a5, a15, -52
    496b:	6186f4        	ae_lq56.i	aeq2, a6, -8
    496e:	600404        	ae_lq32f.i	aeq0, a4, 0
    4971:	622504        	ae_sq32f.i	aeq2, a5, 0
    4974:	614114        	ae_lq56.i	aeq1, a1, 8
    4977:	61c174        	ae_lq56.i	aeq3, a1, 56
    497a:	f93efd450f 	{ ae_lq32f.i	aeq2, a5, 0; ae_subq56	aeq1, aeq3, aeq1 }
    497f:	01cdc4        	ae_subq56	aeq3, aeq0, aeq1
    4982:	f86639b34f 	{ ae_sq32f.i	aeq3, a3, 16; ae_addq56	aeq0, aeq0, aeq1 }
    4987:	6146a4        	ae_lq56.i	aeq1, a6, -48
    498a:	61c694        	ae_lq56.i	aeq3, a6, -56
    498d:	f93ef9824f 	{ ae_sq32f.i	aeq0, a2, 16; ae_subq56	aeq1, aeq3, aeq1 }
    4992:	01edc4        	ae_subq56	aeq3, aeq2, aeq1
    4995:	f97639b35f 	{ ae_sq32f.i	aeq3, a3, 20; ae_addq56	aeq1, aeq2, aeq1 }
    499a:	01d172        	addmi	a7, a1, 0x100
    499d:	6186b4        	ae_lq56.i	aeq2, a6, -40
    49a0:	61c604        	ae_lq56.i	aeq3, a6, 0
    49a3:	f8b6cd074e 	{ addi	a4, a7, -48; ae_subq56	aeq0, aeq2, aeq3 }
    49a8:	621254        	ae_sq32f.i	aeq1, a2, 20
    49ab:	620404        	ae_sq32f.i	aeq0, a4, 0
    49ae:	01d182        	addmi	a8, a1, 0x100
    49b1:	610134        	ae_lq56.i	aeq0, a1, 24
    49b4:	61c124        	ae_lq56.i	aeq3, a1, 16
    49b7:	f9a6fd440f 	{ ae_lq32f.i	aeq2, a4, 0; ae_subq56	aeq1, aeq0, aeq3 }
    49bc:	d4c852        	addi	a5, a8, -44
    49bf:	632114        	ae_sq56s.i	aeq2, a1, 8
    49c2:	621504        	ae_sq32f.i	aeq1, a5, 0
    49c5:	01d192        	addmi	a9, a1, 0x100
    49c8:	61c624        	ae_lq56.i	aeq3, a6, 16
    49cb:	614614        	ae_lq56.i	aeq1, a6, 8
    49ce:	f9aefd650f 	{ ae_lq32f.i	aeq3, a5, 0; ae_subq56	aeq1, aeq1, aeq3 }
    49d3:	d8c942        	addi	a4, a9, -40
    49d6:	633124        	ae_sq56s.i	aeq3, a1, 16
    49d9:	621404        	ae_sq32f.i	aeq1, a4, 0
    49dc:	610144        	ae_lq56.i	aeq0, a1, 32
    49df:	618164        	ae_lq56.i	aeq2, a1, 48
    49e2:	faa2fd040f 	{ ae_lq32f.i	aeq0, a4, 0; ae_subq56	aeq2, aeq0, aeq2 }
    49e7:	dcca52        	addi	a5, a10, -36
    49ea:	630134        	ae_sq56s.i	aeq0, a1, 24
    49ed:	622504        	ae_sq32f.i	aeq2, a5, 0
    49f0:	78c172        	addi	a7, a1, 120
    49f3:	600504        	ae_lq32f.i	aeq0, a5, 0
    49f6:	614744        	ae_lq56.i	aeq1, a7, 32
    49f9:	630164        	ae_sq56s.i	aeq0, a1, 48
    49fc:	621c84        	ae_sq32f.i	aeq1, a12, -32
    49ff:	618114        	ae_lq56.i	aeq2, a1, 8
    4a02:	61c124        	ae_lq56.i	aeq3, a1, 16
    4a05:	016bc4        	ae_addq56	aeq1, aeq2, aeq3
    4a08:	fabafde13f 	{ ae_lq56.i	aeq3, a1, 24; ae_subq56	aeq2, aeq3, aeq2 }
    4a0d:	f93afa115f 	{ ae_sq56s.i	aeq1, a1, 40; ae_subq56	aeq1, aeq3, aeq0 }
    4a12:	632114        	ae_sq56s.i	aeq2, a1, 8
    4a15:	610734        	ae_lq56.i	aeq0, a7, 24
    4a18:	5adb42        	addmi	a4, a11, 0x5a00
    4a1b:	608c84        	ae_lq32f.i	aeq2, a12, -32
    4a1e:	631144        	ae_sq56s.i	aeq1, a1, 32
    4a21:	620d94        	ae_sq32f.i	aeq0, a13, -28
    4a24:	300444        	ae_cvtp24a16x2.ll	aep0, a4, a4
    4a27:	610154        	ae_lq56.i	aeq0, a1, 40
    4a2a:	0c00bda16f 	{ ae_lq56.i	aeq1, a1, 48; ae_mulzaafq32sp16s.lh	aeq0, aeq0, aep0, aeq1, aep0 }
    4a2f:	015bc4        	ae_addq56	aeq1, aeq1, aeq3
    4a32:	fb32fa112f 	{ ae_sq56s.i	aeq1, a1, 16; ae_subq56	aeq3, aeq2, aeq0 }
    4a37:	f8723d2d9f 	{ ae_lq32f.i	aeq1, a13, -28; ae_addq56	aeq0, aeq2, aeq0 }
    4a3c:	633134        	ae_sq56s.i	aeq3, a1, 24
    4a3f:	61c134        	ae_lq56.i	aeq3, a1, 24
    4a42:	623324        	ae_sq32f.i	aeq3, a3, 8
    4a45:	61c124        	ae_lq56.i	aeq3, a1, 16
    4a48:	618114        	ae_lq56.i	aeq2, a1, 8
    4a4b:	0f02ba016f 	{ ae_sq56s.i	aeq0, a1, 48; ae_mulzaafq32sp16s.lh	aeq2, aeq2, aep0, aeq3, aep0 }
    4a50:	610164        	ae_lq56.i	aeq0, a1, 48
    4a53:	f8aaf9822f 	{ ae_sq32f.i	aeq0, a2, 8; ae_subq56	aeq0, aeq1, aeq2 }
    4a58:	f9ea3dc76f 	{ ae_lq56.i	aeq2, a7, 48; ae_addq56	aeq1, aeq1, aeq2 }
    4a5d:	621234        	ae_sq32f.i	aeq1, a2, 12
    4a60:	620334        	ae_sq32f.i	aeq0, a3, 12
    4a63:	614114        	ae_lq56.i	aeq1, a1, 8
    4a66:	3c03b9aeaf 	{ ae_sq32f.i	aeq2, a14, -24; ae_mulzasfq32sp16s.lh	aeq0, aeq1, aep0, aeq3, aep0 }
    4a6b:	614154        	ae_lq56.i	aeq1, a1, 40
    4a6e:	61c144        	ae_lq56.i	aeq3, a1, 32
    4a71:	3e01fde75f 	{ ae_lq56.i	aeq3, a7, 40; ae_mulzasfq32sp16s.lh	aeq1, aeq3, aep0, aeq1, aep0 }
    4a76:	608ea4        	ae_lq32f.i	aeq2, a14, -24
    4a79:	fb32f9bfbf 	{ ae_sq32f.i	aeq3, a15, -20; ae_subq56	aeq3, aeq2, aeq0 }
    4a7e:	f8723d4fbf 	{ ae_lq32f.i	aeq2, a15, -20; ae_addq56	aeq0, aeq2, aeq0 }
    4a83:	fb36f9b36f 	{ ae_sq32f.i	aeq3, a3, 24; ae_subq56	aeq3, aeq2, aeq1 }
    4a88:	f97639826f 	{ ae_sq32f.i	aeq0, a2, 24; ae_addq56	aeq1, aeq2, aeq1 }
    4a8d:	623374        	ae_sq32f.i	aeq3, a3, 28
    4a90:	621274        	ae_sq32f.i	aeq1, a2, 28
    4a93:	f01d           	retw.n

00004a95 <xtensa_fft8_32b+0x2a1>:
    4a95:	000000                                        ...

00004a98 <F_get_oft_32b>:
    4a98:	004136        	entry	a1, 32
    4a9b:	0cc2e6        	bgei	a2, 32, 4aab <F_get_oft_32b+0x13>
    4a9e:	134226        	beqi	a2, 4, 4ab5 <F_get_oft_32b+0x1d>
    4aa1:	148226        	beqi	a2, 8, 4ab9 <F_get_oft_32b+0x21>
    4aa4:	09b266        	bnei	a2, 16, 4ab1 <F_get_oft_32b+0x19>
    4aa7:	021c           	movi.n	a2, 16
    4aa9:	f01d           	retw.n

00004aab <F_get_oft_32b+0x13>:
    4aab:	0ec226        	beqi	a2, 32, 4abd <F_get_oft_32b+0x25>
    4aae:	0fd226        	beqi	a2, 64, 4ac1 <F_get_oft_32b+0x29>
    4ab1:	220c           	movi.n	a2, 2
    4ab3:	f01d           	retw.n

00004ab5 <F_get_oft_32b+0x1d>:
    4ab5:	024c           	movi.n	a2, 64
    4ab7:	f01d           	retw.n

00004ab9 <F_get_oft_32b+0x21>:
    4ab9:	022c           	movi.n	a2, 32
    4abb:	f01d           	retw.n

00004abd <F_get_oft_32b+0x25>:
    4abd:	820c           	movi.n	a2, 8
    4abf:	f01d           	retw.n

00004ac1 <F_get_oft_32b+0x29>:
    4ac1:	420c           	movi.n	a2, 4
    4ac3:	f01d           	retw.n

00004ac5 <F_get_oft_32b+0x2d>:
    4ac5:	000000                                        ...

00004ac8 <xtensa_fft_pass_32b>:
    4ac8:	00c136        	entry	a1, 96
    4acb:	905330        	addx2	a5, a3, a3
    4ace:	1143c0        	slli	a4, a3, 4
    4ad1:	b05520        	addx8	a5, a5, a2
    4ad4:	602244        	ae_lq32f.x	aeq0, a2, a4
    4ad7:	604504        	ae_lq32f.i	aeq1, a5, 0
    4ada:	f8663a014f 	{ ae_sq56s.i	aeq0, a1, 32; ae_addq56	aeq0, aeq0, aeq1 }
    4adf:	608204        	ae_lq32f.i	aeq2, a2, 0
    4ae2:	f832fa012f 	{ ae_sq56s.i	aeq0, a1, 16; ae_subq56	aeq0, aeq2, aeq0 }
    4ae7:	fc4450724f 	{ add	a7, a2, a4; ae_roundsq32sym	aeq0, aeq0 }
    4aec:	631114        	ae_sq56s.i	aeq1, a1, 8
    4aef:	630154        	ae_sq56s.i	aeq0, a1, 40
    4af2:	600514        	ae_lq32f.i	aeq0, a5, 4
    4af5:	604714        	ae_lq32f.i	aeq1, a7, 4
    4af8:	fa6a3a216f 	{ ae_sq56s.i	aeq2, a1, 48; ae_addq56	aeq2, aeq1, aeq0 }
    4afd:	f82acfc74e 	{ addi	a4, a7, -4; ae_subq56	aeq0, aeq1, aeq0 }
    4b02:	60c214        	ae_lq32f.i	aeq3, a2, 4
    4b05:	614154        	ae_lq56.i	aeq1, a1, 40
    4b08:	f9baf9d41f 	{ ae_sq32f.iu	aeq1, a4, 4; ae_subq56	aeq1, aeq3, aeq2 }
    4b0d:	fc4c7a013f 	{ ae_sq56s.i	aeq0, a1, 24; ae_roundsq32sym	aeq0, aeq1 }
    4b12:	fafa3a015f 	{ ae_sq56s.i	aeq0, a1, 40; ae_addq56	aeq2, aeq3, aeq2 }
    4b17:	ff547da12f 	{ ae_lq56.i	aeq1, a1, 16; ae_roundsq32sym	aeq3, aeq2 }
    4b1c:	610164        	ae_lq56.i	aeq0, a1, 48
    4b1f:	f9663d815f 	{ ae_lq56.i	aeq0, a1, 40; ae_addq56	aeq1, aeq0, aeq1 }
    4b24:	b06320        	addx8	a6, a3, a2
    4b27:	fd4c4fc22e 	{ addi	a2, a2, -4; ae_roundsq32sym	aeq1, aeq1 }
    4b2c:	624414        	ae_sq32f.iu	aeq0, a4, 4
    4b2f:	625214        	ae_sq32f.iu	aeq1, a2, 4
    4b32:	627214        	ae_sq32f.iu	aeq3, a2, 4
    4b35:	614114        	ae_lq56.i	aeq1, a1, 8
    4b38:	618144        	ae_lq56.i	aeq2, a1, 32
    4b3b:	f8aafd260f 	{ ae_lq32f.i	aeq1, a6, 0; ae_subq56	aeq0, aeq1, aeq2 }
    4b40:	61c134        	ae_lq56.i	aeq3, a1, 24
    4b43:	faaecfc55e 	{ addi	a5, a5, -4; ae_subq56	aeq2, aeq1, aeq3 }
    4b48:	f9ee3d661f 	{ ae_lq32f.i	aeq3, a6, 4; ae_addq56	aeq1, aeq1, aeq3 }
    4b4d:	f93afa111f 	{ ae_sq56s.i	aeq1, a1, 8; ae_subq56	aeq1, aeq3, aeq0 }
    4b52:	fe544fc66e 	{ addi	a6, a6, -4; ae_roundsq32sym	aeq2, aeq2 }
    4b57:	f87a39e51f 	{ ae_sq32f.iu	aeq2, a5, 4; ae_addq56	aeq0, aeq3, aeq0 }
    4b5c:	fd4c7de11f 	{ ae_lq56.i	aeq3, a1, 8; ae_roundsq32sym	aeq1, aeq1 }
    4b61:	fd5c79d51f 	{ ae_sq32f.iu	aeq1, a5, 4; ae_roundsq32sym	aeq1, aeq3 }
    4b66:	edbf71        	l32r	a7, 264 (4a98 <F_get_oft_32b>)
    4b69:	83ad           	ae_sext16	a10, a3
    4b6b:	fc4479d61f 	{ ae_sq32f.iu	aeq1, a6, 4; ae_roundsq32sym	aeq0, aeq0 }
    4b70:	624614        	ae_sq32f.iu	aeq0, a6, 4
    4b73:	0007e0        	callx8	a7
    4b76:	edbc71        	l32r	a7, 268 (8560 <ROOTS_512_Q16>)
    4b79:	00a082        	movi	a8, 0
    4b7c:	119af0        	slli	a9, a10, 1
    4b7f:	907a70        	addx2	a7, a10, a7
    4b82:	fcc772        	addi	a7, a7, -4
    4b85:	458784        	ae_lp16x2f.xu	aep0, a7, a8
    4b88:	218130        	srai	a8, a3, 1
    4b8b:	0223e6        	bgei	a3, 2, 4b91 <xtensa_fft_pass_32b+0xc9>
    4b8e:	002e86        	j	4c4c <xtensa_fft_pass_32b+0x184>
    4b91:	b78876        	loop	a8, 4c4c <xtensa_fft_pass_32b+0x184>
    4b94:	604414        	ae_lq32f.i	aeq1, a4, 4
    4b97:	608424        	ae_lq32f.i	aeq2, a4, 8
    4b9a:	30033d651f 	{ ae_lq32f.i	aeq3, a5, 4; ae_mulzaaq32sp16u.lh	aeq0, aeq1, aep0, aeq2, aep0 }
    4b9f:	6300bd252f 	{ ae_lq32f.i	aeq1, a5, 8; ae_mulzasq32sp16u.lh	aeq2, aeq2, aep0, aeq1, aep0 }
    4ba4:	630114        	ae_sq56s.i	aeq0, a1, 8
    4ba7:	6301ba214f 	{ ae_sq56s.i	aeq2, a1, 32; ae_mulzasq32sp16u.lh	aeq2, aeq3, aep0, aeq1, aep0 }
    4bac:	3103fd811f 	{ ae_lq56.i	aeq0, a1, 8; ae_mulzaaq32sp16u.lh	aeq3, aeq1, aep0, aeq3, aep0 }
    4bb1:	f8e23d221f 	{ ae_lq32f.i	aeq1, a2, 4; ae_addq56	aeq0, aeq0, aeq2 }
    4bb6:	fa2afa213f 	{ ae_sq56s.i	aeq2, a1, 24; ae_subq56	aeq2, aeq1, aeq0 }
    4bbb:	f86a3a212f 	{ ae_sq56s.i	aeq2, a1, 16; ae_addq56	aeq0, aeq1, aeq0 }
    4bc0:	618144        	ae_lq56.i	aeq2, a1, 32
    4bc3:	f8f63a015f 	{ ae_sq56s.i	aeq0, a1, 40; ae_addq56	aeq0, aeq2, aeq3 }
    4bc8:	633164        	ae_sq56s.i	aeq3, a1, 48
    4bcb:	61c124        	ae_lq56.i	aeq3, a1, 16
    4bce:	ff5c7d222f 	{ ae_lq32f.i	aeq1, a2, 8; ae_roundsq32sym	aeq3, aeq3 }
    4bd3:	fb2afa312f 	{ ae_sq56s.i	aeq3, a1, 16; ae_subq56	aeq3, aeq1, aeq0 }
    4bd8:	f86a3a317f 	{ ae_sq56s.i	aeq3, a1, 56; ae_addq56	aeq0, aeq1, aeq0 }
    4bdd:	fc447de12f 	{ ae_lq56.i	aeq3, a1, 16; ae_roundsq32sym	aeq0, aeq0 }
    4be2:	627414        	ae_sq32f.iu	aeq3, a4, 4
    4be5:	61c174        	ae_lq56.i	aeq3, a1, 56
    4be8:	ff5c5b879f 	{ ae_lp16x2f.xu	aep0, a7, a9; ae_roundsq32sym	aeq3, aeq3 }
    4bed:	627414        	ae_sq32f.iu	aeq3, a4, 4
    4bf0:	61c154        	ae_lq56.i	aeq3, a1, 40
    4bf3:	fd5c7de11f 	{ ae_lq56.i	aeq3, a1, 8; ae_roundsq32sym	aeq1, aeq3 }
    4bf8:	625214        	ae_sq32f.iu	aeq1, a2, 4
    4bfb:	624214        	ae_sq32f.iu	aeq0, a2, 4
    4bfe:	614164        	ae_lq56.i	aeq1, a1, 48
    4c01:	f836fd261f 	{ ae_lq32f.i	aeq1, a6, 4; ae_subq56	aeq0, aeq2, aeq1 }
    4c06:	0198c4        	ae_addq56	aeq2, aeq1, aeq0
    4c09:	f82afa212f 	{ ae_sq56s.i	aeq2, a1, 16; ae_subq56	aeq0, aeq1, aeq0 }
    4c0e:	fc447dc13f 	{ ae_lq56.i	aeq2, a1, 24; ae_roundsq32sym	aeq0, aeq0 }
    4c13:	fbb6fdc12f 	{ ae_lq56.i	aeq2, a1, 16; ae_subq56	aeq3, aeq2, aeq3 }
    4c18:	fe547a314f 	{ ae_sq56s.i	aeq3, a1, 32; ae_roundsq32sym	aeq2, aeq2 }
    4c1d:	632114        	ae_sq56s.i	aeq2, a1, 8
    4c20:	608624        	ae_lq32f.i	aeq2, a6, 8
    4c23:	01ebc4        	ae_addq56	aeq3, aeq2, aeq3
    4c26:	633134        	ae_sq56s.i	aeq3, a1, 24
    4c29:	61c114        	ae_lq56.i	aeq3, a1, 8
    4c2c:	627614        	ae_sq32f.iu	aeq3, a6, 4
    4c2f:	61c134        	ae_lq56.i	aeq3, a1, 24
    4c32:	06f834        	ae_roundsq32sym	aeq3, aeq3
    4c35:	633114        	ae_sq56s.i	aeq3, a1, 8
    4c38:	61c114        	ae_lq56.i	aeq3, a1, 8
    4c3b:	627614        	ae_sq32f.iu	aeq3, a6, 4
    4c3e:	61c144        	ae_lq56.i	aeq3, a1, 32
    4c41:	f9b6f9c51f 	{ ae_sq32f.iu	aeq0, a5, 4; ae_subq56	aeq1, aeq2, aeq3 }
    4c46:	06d834        	ae_roundsq32sym	aeq3, aeq1
    4c49:	627514        	ae_sq32f.iu	aeq3, a5, 4

00004c4c <xtensa_fft_pass_32b+0x184>:
    4c4c:	0243e6        	bgei	a3, 4, 4c52 <xtensa_fft_pass_32b+0x18a>
    4c4f:	003146        	j	4d18 <xtensa_fft_pass_32b+0x250>
    4c52:	603090        	neg	a3, a9
    4c55:	1193f0        	slli	a9, a3, 1
    4c58:	880b           	addi.n	a8, a8, -1
    4c5a:	458794        	ae_lp16x2f.xu	aep0, a7, a9
    4c5d:	b78876        	loop	a8, 4d18 <xtensa_fft_pass_32b+0x250>
    4c60:	604424        	ae_lq32f.i	aeq1, a4, 8
    4c63:	608414        	ae_lq32f.i	aeq2, a4, 4
    4c66:	30033d652f 	{ ae_lq32f.i	aeq3, a5, 8; ae_mulzaaq32sp16u.lh	aeq0, aeq1, aep0, aeq2, aep0 }
    4c6b:	9300bd251f 	{ ae_lq32f.i	aeq1, a5, 4; ae_mulzsaq32sp16u.lh	aeq2, aeq2, aep0, aeq1, aep0 }
    4c70:	630114        	ae_sq56s.i	aeq0, a1, 8
    4c73:	9301ba214f 	{ ae_sq56s.i	aeq2, a1, 32; ae_mulzsaq32sp16u.lh	aeq2, aeq3, aep0, aeq1, aep0 }
    4c78:	3103fd811f 	{ ae_lq56.i	aeq0, a1, 8; ae_mulzaaq32sp16u.lh	aeq3, aeq1, aep0, aeq3, aep0 }
    4c7d:	f8e23d221f 	{ ae_lq32f.i	aeq1, a2, 4; ae_addq56	aeq0, aeq0, aeq2 }
    4c82:	fa2afa213f 	{ ae_sq56s.i	aeq2, a1, 24; ae_subq56	aeq2, aeq1, aeq0 }
    4c87:	f86a3a212f 	{ ae_sq56s.i	aeq2, a1, 16; ae_addq56	aeq0, aeq1, aeq0 }
    4c8c:	618144        	ae_lq56.i	aeq2, a1, 32
    4c8f:	f8f63a015f 	{ ae_sq56s.i	aeq0, a1, 40; ae_addq56	aeq0, aeq2, aeq3 }
    4c94:	633164        	ae_sq56s.i	aeq3, a1, 48
    4c97:	61c124        	ae_lq56.i	aeq3, a1, 16
    4c9a:	ff5c7d222f 	{ ae_lq32f.i	aeq1, a2, 8; ae_roundsq32sym	aeq3, aeq3 }
    4c9f:	fb2afa312f 	{ ae_sq56s.i	aeq3, a1, 16; ae_subq56	aeq3, aeq1, aeq0 }
    4ca4:	f86a3a317f 	{ ae_sq56s.i	aeq3, a1, 56; ae_addq56	aeq0, aeq1, aeq0 }
    4ca9:	fc447de12f 	{ ae_lq56.i	aeq3, a1, 16; ae_roundsq32sym	aeq0, aeq0 }
    4cae:	627414        	ae_sq32f.iu	aeq3, a4, 4
    4cb1:	61c174        	ae_lq56.i	aeq3, a1, 56
    4cb4:	ff5c5b873f 	{ ae_lp16x2f.xu	aep0, a7, a3; ae_roundsq32sym	aeq3, aeq3 }
    4cb9:	627414        	ae_sq32f.iu	aeq3, a4, 4
    4cbc:	61c154        	ae_lq56.i	aeq3, a1, 40
    4cbf:	fd5c7de11f 	{ ae_lq56.i	aeq3, a1, 8; ae_roundsq32sym	aeq1, aeq3 }
    4cc4:	625214        	ae_sq32f.iu	aeq1, a2, 4
    4cc7:	624214        	ae_sq32f.iu	aeq0, a2, 4
    4cca:	614164        	ae_lq56.i	aeq1, a1, 48
    4ccd:	f836fd261f 	{ ae_lq32f.i	aeq1, a6, 4; ae_subq56	aeq0, aeq2, aeq1 }
    4cd2:	0198c4        	ae_addq56	aeq2, aeq1, aeq0
    4cd5:	f82afa212f 	{ ae_sq56s.i	aeq2, a1, 16; ae_subq56	aeq0, aeq1, aeq0 }
    4cda:	fc447dc13f 	{ ae_lq56.i	aeq2, a1, 24; ae_roundsq32sym	aeq0, aeq0 }
    4cdf:	fbb6fdc12f 	{ ae_lq56.i	aeq2, a1, 16; ae_subq56	aeq3, aeq2, aeq3 }
    4ce4:	fe547a314f 	{ ae_sq56s.i	aeq3, a1, 32; ae_roundsq32sym	aeq2, aeq2 }
    4ce9:	632114        	ae_sq56s.i	aeq2, a1, 8
    4cec:	608624        	ae_lq32f.i	aeq2, a6, 8
    4cef:	01ebc4        	ae_addq56	aeq3, aeq2, aeq3
    4cf2:	633134        	ae_sq56s.i	aeq3, a1, 24
    4cf5:	61c114        	ae_lq56.i	aeq3, a1, 8
    4cf8:	627614        	ae_sq32f.iu	aeq3, a6, 4
    4cfb:	61c134        	ae_lq56.i	aeq3, a1, 24
    4cfe:	06f834        	ae_roundsq32sym	aeq3, aeq3
    4d01:	633114        	ae_sq56s.i	aeq3, a1, 8
    4d04:	61c114        	ae_lq56.i	aeq3, a1, 8
    4d07:	627614        	ae_sq32f.iu	aeq3, a6, 4
    4d0a:	61c144        	ae_lq56.i	aeq3, a1, 32
    4d0d:	f9b6f9c51f 	{ ae_sq32f.iu	aeq0, a5, 4; ae_subq56	aeq1, aeq2, aeq3 }
    4d12:	06d834        	ae_roundsq32sym	aeq3, aeq1
    4d15:	627514        	ae_sq32f.iu	aeq3, a5, 4
    4d18:	f01d           	retw.n

00004d1a <xtensa_fft_pass_32b+0x252>:
	...

00004d1c <xtensa_fft16_32b>:
    4d1c:	004136        	entry	a1, 32
    4d1f:	ed5331        	l32r	a3, 26c (47f4 <xtensa_fft8_32b>)
    4d22:	02ad           	mov.n	a10, a2
    4d24:	0003e0        	callx8	a3
    4d27:	ed5231        	l32r	a3, 270 (4758 <xtensa_fft4_32b>)
    4d2a:	40c2a2        	addi	a10, a2, 64
    4d2d:	0003e0        	callx8	a3
    4d30:	60c2a2        	addi	a10, a2, 96
    4d33:	0003e0        	callx8	a3
    4d36:	ed4f31        	l32r	a3, 274 (4ac8 <xtensa_fft_pass_32b>)
    4d39:	4b0c           	movi.n	a11, 4
    4d3b:	02ad           	mov.n	a10, a2
    4d3d:	0003e0        	callx8	a3
    4d40:	f01d           	retw.n

00004d42 <xtensa_fft16_32b+0x26>:
	...

00004d44 <xtensa_fft32_32b>:
    4d44:	004136        	entry	a1, 32
    4d47:	ed4c31        	l32r	a3, 278 (4d1c <xtensa_fft16_32b>)
    4d4a:	02ad           	mov.n	a10, a2
    4d4c:	0003e0        	callx8	a3
    4d4f:	80a032        	movi	a3, 128
    4d52:	a23a           	add.n	a10, a2, a3
    4d54:	ed4631        	l32r	a3, 26c (47f4 <xtensa_fft8_32b>)
    4d57:	0003e0        	callx8	a3
    4d5a:	c0a042        	movi	a4, 192
    4d5d:	a24a           	add.n	a10, a2, a4
    4d5f:	0003e0        	callx8	a3
    4d62:	ed4431        	l32r	a3, 274 (4ac8 <xtensa_fft_pass_32b>)
    4d65:	8b0c           	movi.n	a11, 8
    4d67:	02ad           	mov.n	a10, a2
    4d69:	0003e0        	callx8	a3
    4d6c:	f01d           	retw.n

00004d6e <xtensa_fft32_32b+0x2a>:
	...

00004d70 <xtensa_fft64_32b>:
    4d70:	004136        	entry	a1, 32
    4d73:	ed4231        	l32r	a3, 27c (4d44 <xtensa_fft32_32b>)
    4d76:	02ad           	mov.n	a10, a2
    4d78:	0003e0        	callx8	a3
    4d7b:	ed3f31        	l32r	a3, 278 (4d1c <xtensa_fft16_32b>)
    4d7e:	01d2a2        	addmi	a10, a2, 0x100
    4d81:	0003e0        	callx8	a3
    4d84:	80a142        	movi	a4, 0x180
    4d87:	a24a           	add.n	a10, a2, a4
    4d89:	0003e0        	callx8	a3
    4d8c:	ed3a31        	l32r	a3, 274 (4ac8 <xtensa_fft_pass_32b>)
    4d8f:	0b1c           	movi.n	a11, 16
    4d91:	02ad           	mov.n	a10, a2
    4d93:	0003e0        	callx8	a3
    4d96:	f01d           	retw.n

00004d98 <xtensa_fft128_32b>:
    4d98:	004136        	entry	a1, 32
    4d9b:	ed3931        	l32r	a3, 280 (4d70 <xtensa_fft64_32b>)
    4d9e:	02ad           	mov.n	a10, a2
    4da0:	0003e0        	callx8	a3
    4da3:	ed3631        	l32r	a3, 27c (4d44 <xtensa_fft32_32b>)
    4da6:	02d2a2        	addmi	a10, a2, 0x200
    4da9:	0003e0        	callx8	a3
    4dac:	03d2a2        	addmi	a10, a2, 0x300
    4daf:	0003e0        	callx8	a3
    4db2:	ed3031        	l32r	a3, 274 (4ac8 <xtensa_fft_pass_32b>)
    4db5:	0b2c           	movi.n	a11, 32
    4db7:	02ad           	mov.n	a10, a2
    4db9:	0003e0        	callx8	a3
    4dbc:	f01d           	retw.n

00004dbe <xtensa_fft128_32b+0x26>:
	...

00004dc0 <xtensa_fft256_32b>:
    4dc0:	004136        	entry	a1, 32
    4dc3:	ed2f31        	l32r	a3, 280 (4d70 <xtensa_fft64_32b>)
    4dc6:	02ad           	mov.n	a10, a2
    4dc8:	0003e0        	callx8	a3
    4dcb:	ed2c41        	l32r	a4, 27c (4d44 <xtensa_fft32_32b>)
    4dce:	02d2a2        	addmi	a10, a2, 0x200
    4dd1:	0004e0        	callx8	a4
    4dd4:	03d2a2        	addmi	a10, a2, 0x300
    4dd7:	0004e0        	callx8	a4
    4dda:	ed2641        	l32r	a4, 274 (4ac8 <xtensa_fft_pass_32b>)
    4ddd:	0b2c           	movi.n	a11, 32
    4ddf:	02ad           	mov.n	a10, a2
    4de1:	0004e0        	callx8	a4
    4de4:	04d2a2        	addmi	a10, a2, 0x400
    4de7:	0003e0        	callx8	a3
    4dea:	06d2a2        	addmi	a10, a2, 0x600
    4ded:	0003e0        	callx8	a3
    4df0:	0b4c           	movi.n	a11, 64
    4df2:	02ad           	mov.n	a10, a2
    4df4:	0004e0        	callx8	a4
    4df7:	f01d           	retw.n

00004df9 <xtensa_fft256_32b+0x39>:
    4df9:	000000                                        ...

00004dfc <xtensa_fft512_32b>:
    4dfc:	004136        	entry	a1, 32
    4dff:	ed2131        	l32r	a3, 284 (4dc0 <xtensa_fft256_32b>)
    4e02:	02ad           	mov.n	a10, a2
    4e04:	0003e0        	callx8	a3
    4e07:	ed2031        	l32r	a3, 288 (4d98 <xtensa_fft128_32b>)
    4e0a:	08d2a2        	addmi	a10, a2, 0x800
    4e0d:	0003e0        	callx8	a3
    4e10:	0cd2a2        	addmi	a10, a2, 0xc00
    4e13:	0003e0        	callx8	a3
    4e16:	ed1731        	l32r	a3, 274 (4ac8 <xtensa_fft_pass_32b>)
    4e19:	80a0b2        	movi	a11, 128
    4e1c:	02ad           	mov.n	a10, a2
    4e1e:	0003e0        	callx8	a3
    4e21:	f01d           	retw.n

00004e23 <xtensa_fft512_32b+0x27>:
	...

00004e24 <real_split>:
    4e24:	00a136        	entry	a1, 80
    4e27:	216120        	srai	a6, a2, 1
    4e2a:	050c           	movi.n	a5, 0
    4e2c:	1388           	l32i.n	a8, a3, 4
    4e2e:	0378           	l32i.n	a7, a3, 0
    4e30:	c0c260        	sub	a12, a2, a6
    4e33:	1459           	s32i.n	a5, a4, 4
    4e35:	b05c30        	addx8	a5, a12, a3
    4e38:	15e8           	l32i.n	a14, a5, 4
    4e3a:	787a           	add.n	a7, a8, a7
    4e3c:	0479           	s32i.n	a7, a4, 0
    4e3e:	b0d640        	addx8	a13, a6, a4
    4e41:	0558           	l32i.n	a5, a5, 0
    4e43:	11fef0        	slli	a15, a14, 1
    4e46:	0d59           	s32i.n	a5, a13, 0
    4e48:	6050f0        	neg	a5, a15
    4e4b:	215150        	srai	a5, a5, 1
    4e4e:	1d59           	s32i.n	a5, a13, 4
    4e50:	ed0551        	l32r	a5, 264 (4a98 <F_get_oft_32b>)
    4e53:	82ad           	ae_sext16	a10, a2
    4e55:	b169           	s32i.n	a6, a1, 44
    4e57:	0005e0        	callx8	a5
    4e5a:	81a072        	movi	a7, 129
    4e5d:	727270        	salt	a7, a2, a7
    4e60:	401700        	ssl	a7
    4e63:	a17a00        	sll	a7, a10
    4e66:	fec782        	addi	a8, a7, -2
    4e69:	ecff51        	l32r	a5, 268 (8560 <ROOTS_512_Q16>)
    4e6c:	11b8f0        	slli	a11, a8, 1
    4e6f:	41b2b0        	srli	a11, a11, 2
    4e72:	7fa092        	movi	a9, 127
    4e75:	a0bb50        	addx4	a11, a11, a5
    4e78:	729970        	salt	a9, a9, a7
    4e7b:	60a070        	neg	a10, a7
    4e7e:	441b04        	ae_lp16x2f.i	aep1, a11, 0
    4e81:	02e7e6        	bgei	a7, 128, 4e87 <real_split+0x63>
    4e84:	1a9194        	ae_selp24.lh	aep1, aep1, aep1

00004e87 <real_split+0x63>:
    4e87:	160c           	movi.n	a6, 1
    4e89:	116610        	slli	a6, a6, 15
    4e8c:	300664        	ae_cvtp24a16x2.ll	aep0, a6, a6
    4e8f:	937a90        	movnez	a7, a10, a9
    4e92:	0242e6        	bgei	a2, 4, 4e98 <real_split+0x74>
    4e95:	002446        	j	4f2a <real_split+0x106>
    4e98:	b168           	l32i.n	a6, a1, 44
    4e9a:	b0b230        	addx8	a11, a2, a3
    4e9d:	14c3e2        	addi	a14, a3, 20
    4ea0:	f60b           	addi.n	a15, a6, -1
    4ea2:	fccbd2        	addi	a13, a11, -4
    4ea5:	c48b           	addi.n	a12, a4, 8
    4ea7:	60c334        	ae_lq32f.i	aeq3, a3, 12
    4eaa:	604324        	ae_lq32f.i	aeq1, a3, 8
    4ead:	9027807daf 	{ movi	a10, 125; ae_mulzsaq32sp16u.lh	aeq0, aeq1, aep1, aeq3, aep1 }
    4eb2:	f8ceb2        	addi	a11, a14, -8
    4eb5:	718f76        	loop	a15, 4f2a <real_split+0x106>
    4eb8:	608df4        	ae_lq32f.i	aeq2, a13, -4
    4ebb:	887a           	add.n	a8, a8, a7
    4ebd:	c225ba014f 	{ ae_sq56s.i	aeq0, a1, 32; ae_mulzssq32sp16u.lh	aeq0, aeq3, aep1, aeq1, aep1 }
    4ec2:	c30864        	ae_mulzaafq32sp16u.lh	aeq1, aeq1, aep0, aeq2, aep0
    4ec5:	630114        	ae_sq56s.i	aeq0, a1, 8
    4ec8:	600d04        	ae_lq32f.i	aeq0, a13, 0
    4ecb:	32247a113f 	{ ae_sq56s.i	aeq1, a1, 24; ae_mulzaaq32sp16u.lh	aeq1, aeq2, aep1, aeq0, aep1 }
    4ed0:	4b014fcdde 	{ addi	a13, a13, -4; ae_mulzasfq32sp16u.lh	aeq3, aeq3, aep0, aeq0, aep0 }
    4ed5:	90263dc11f 	{ ae_lq56.i	aeq2, a1, 8; ae_mulzsaq32sp16u.lh	aeq0, aeq0, aep1, aeq2, aep1 }
    4eda:	fafa3a112f 	{ ae_sq56s.i	aeq1, a1, 16; ae_addq56	aeq2, aeq3, aeq2 }
    4edf:	f8723de14f 	{ ae_lq56.i	aeq3, a1, 32; ae_addq56	aeq0, aeq2, aeq0 }
    4ee4:	614134        	ae_lq56.i	aeq1, a1, 24
    4ee7:	fbee3da12f 	{ ae_lq56.i	aeq1, a1, 16; ae_addq56	aeq3, aeq1, aeq3 }
    4eec:	f97e3b001f 	{ ae_sraiq56	aeq0, aeq0, 1; ae_addq56	aeq1, aeq3, aeq1 }
    4ef1:	1168f0        	slli	a6, a8, 1
    4ef4:	fc447b105f 	{ ae_sraiq56	aeq1, aeq1, 1; ae_roundsq32sym	aeq0, aeq0 }
    4ef9:	416260        	srli	a6, a6, 2
    4efc:	72fa80        	salt	a15, a10, a8
    4eff:	fd4c798c1f 	{ ae_sq32f.i	aeq0, a12, 4; ae_roundsq32sym	aeq1, aeq1 }
    4f04:	a06650        	addx4	a6, a6, a5
    4f07:	3099f0        	xor	a9, a9, a15
    4f0a:	621c04        	ae_sq32f.i	aeq1, a12, 0
    4f0d:	cc4b           	addi.n	a12, a12, 4
    4f0f:	60e070        	neg	a14, a7
    4f12:	441604        	ae_lp16x2f.i	aep1, a6, 0
    4f15:	19cc           	bnez.n	a9, 4f1a <real_split+0xf6>
    4f17:	1a9194        	ae_selp24.lh	aep1, aep1, aep1
    4f1a:	604b14        	ae_lq32f.i	aeq1, a11, 4
    4f1d:	937ef0        	movnez	a7, a14, a15
    4f20:	60db24        	ae_lq32f.iu	aeq3, a11, 8
    4f23:	90278fcdde 	{ addi	a13, a13, -4; ae_mulzsaq32sp16u.lh	aeq0, aeq1, aep1, aeq3, aep1 }
    4f28:	cc4b           	addi.n	a12, a12, 4

00004f2a <real_split+0x106>:
    4f2a:	b168           	l32i.n	a6, a1, 44
    4f2c:	ef7c           	movi.n	a15, -2
    4f2e:	c08f70        	sub	a8, a15, a7
    4f31:	b61b           	addi.n	a11, a6, 1
    4f33:	1168f0        	slli	a6, a8, 1
    4f36:	7da0a2        	movi	a10, 125
    4f39:	416260        	srli	a6, a6, 2
    4f3c:	72ca80        	salt	a12, a10, a8
    4f3f:	a06650        	addx4	a6, a6, a5
    4f42:	3099c0        	xor	a9, a9, a12
    4f45:	441604        	ae_lp16x2f.i	aep1, a6, 0
    4f48:	498c           	beqz.n	a9, 4f50 <real_split+0x12c>
    4f4a:	0b2b27        	blt	a11, a2, 4f59 <real_split+0x135>
    4f4d:	002b86        	j	4fff <real_split+0x1db>

00004f50 <real_split+0x12c>:
    4f50:	1a9194        	ae_selp24.lh	aep1, aep1, aep1
    4f53:	022b27        	blt	a11, a2, 4f59 <real_split+0x135>
    4f56:	002946        	j	4fff <real_split+0x1db>
    4f59:	b1f8           	l32i.n	a15, a1, 44
    4f5b:	60a070        	neg	a10, a7
    4f5e:	93a7c0        	movnez	a10, a7, a12
    4f61:	f77c           	movi.n	a7, -1
    4f63:	b0ef30        	addx8	a14, a15, a3
    4f66:	307f70        	xor	a7, a15, a7
    4f69:	c062b0        	sub	a6, a2, a11
    4f6c:	b0db30        	addx8	a13, a11, a3
    4f6f:	b0c630        	addx8	a12, a6, a3
    4f72:	14cee2        	addi	a14, a14, 20
    4f75:	772a           	add.n	a7, a7, a2
    4f77:	b0bb40        	addx8	a11, a11, a4
    4f7a:	7da042        	movi	a4, 125
    4f7d:	60cd14        	ae_lq32f.i	aeq3, a13, 4
    4f80:	604d04        	ae_lq32f.i	aeq1, a13, 0
    4f83:	3c4b           	addi.n	a3, a12, 4
    4f85:	c0278f8e2e 	{ addi	a2, a14, -8; ae_mulzssq32sp16u.lh	aeq0, aeq1, aep1, aeq3, aep1 }
    4f8a:	718776        	loop	a7, 4fff <real_split+0x1db>
    4f8d:	6083f4        	ae_lq32f.i	aeq2, a3, -4
    4f90:	88aa           	add.n	a8, a8, a10
    4f92:	9225ba014f 	{ ae_sq56s.i	aeq0, a1, 32; ae_mulzsaq32sp16u.lh	aeq0, aeq3, aep1, aeq1, aep1 }
    4f97:	1803404b6e 	{ addi	a6, a11, 4; ae_mulzaafq32sp16u.lh	aeq1, aeq1, aep0, aeq2, aep0 }
    4f9c:	630114        	ae_sq56s.i	aeq0, a1, 8
    4f9f:	600304        	ae_lq32f.i	aeq0, a3, 0
    4fa2:	62247a113f 	{ ae_sq56s.i	aeq1, a1, 24; ae_mulzasq32sp16u.lh	aeq1, aeq2, aep1, aeq0, aep1 }
    4fa7:	4b014fc33e 	{ addi	a3, a3, -4; ae_mulzasfq32sp16u.lh	aeq3, aeq3, aep0, aeq0, aep0 }
    4fac:	c0263dc11f 	{ ae_lq56.i	aeq2, a1, 8; ae_mulzssq32sp16u.lh	aeq0, aeq0, aep1, aeq2, aep1 }
    4fb1:	fafa3a112f 	{ ae_sq56s.i	aeq1, a1, 16; ae_addq56	aeq2, aeq3, aeq2 }
    4fb6:	f8723de14f 	{ ae_lq56.i	aeq3, a1, 32; ae_addq56	aeq0, aeq2, aeq0 }
    4fbb:	614134        	ae_lq56.i	aeq1, a1, 24
    4fbe:	fbee3da12f 	{ ae_lq56.i	aeq1, a1, 16; ae_addq56	aeq3, aeq1, aeq3 }
    4fc3:	f97e3b001f 	{ ae_sraiq56	aeq0, aeq0, 1; ae_addq56	aeq1, aeq3, aeq1 }
    4fc8:	fc447b105f 	{ ae_sraiq56	aeq1, aeq1, 1; ae_roundsq32sym	aeq0, aeq0 }
    4fcd:	11c8f0        	slli	a12, a8, 1
    4fd0:	fd4c798b1f 	{ ae_sq32f.i	aeq0, a11, 4; ae_roundsq32sym	aeq1, aeq1 }
    4fd5:	41c2c0        	srli	a12, a12, 2
    4fd8:	621b04        	ae_sq32f.i	aeq1, a11, 0
    4fdb:	72b480        	salt	a11, a4, a8
    4fde:	a0cc50        	addx4	a12, a12, a5
    4fe1:	3099b0        	xor	a9, a9, a11
    4fe4:	6070a0        	neg	a7, a10
    4fe7:	441c04        	ae_lp16x2f.i	aep1, a12, 0
    4fea:	19cc           	bnez.n	a9, 4fef <real_split+0x1cb>
    4fec:	1a9194        	ae_selp24.lh	aep1, aep1, aep1
    4fef:	93a7b0        	movnez	a10, a7, a11
    4ff2:	604214        	ae_lq32f.i	aeq1, a2, 4
    4ff5:	60d224        	ae_lq32f.iu	aeq3, a2, 8
    4ff8:	c0278fc33e 	{ addi	a3, a3, -4; ae_mulzssq32sp16u.lh	aeq0, aeq1, aep1, aeq3, aep1 }
    4ffd:	b64b           	addi.n	a11, a6, 4

00004fff <real_split+0x1db>:
    4fff:	f01d           	retw.n

00005001 <real_split+0x1dd>:
    5001:	000000                                        ...

00005004 <xtensa_real_fft_32b>:
    5004:	004136        	entry	a1, 32
    5007:	1142d0        	slli	a4, a2, 3
    500a:	c04340        	sub	a4, a3, a4
    500d:	0be2e6        	bgei	a2, 128, 501c <xtensa_real_fft_32b+0x18>
    5010:	148226        	beqi	a2, 8, 5028 <xtensa_real_fft_32b+0x24>
    5013:	1fd266        	bnei	a2, 64, 5036 <xtensa_real_fft_32b+0x32>
    5016:	ec9a51        	l32r	a5, 280 (4d70 <xtensa_fft64_32b>)
    5019:	000506        	j	5031 <xtensa_real_fft_32b+0x2d>

0000501c <xtensa_real_fft_32b+0x18>:
    501c:	0ee226        	beqi	a2, 128, 502e <xtensa_real_fft_32b+0x2a>
    501f:	13f266        	bnei	a2, 0x100, 5036 <xtensa_real_fft_32b+0x32>
    5022:	ec9851        	l32r	a5, 284 (4dc0 <xtensa_fft256_32b>)
    5025:	000206        	j	5031 <xtensa_real_fft_32b+0x2d>

00005028 <xtensa_real_fft_32b+0x24>:
    5028:	ec9151        	l32r	a5, 26c (47f4 <xtensa_fft8_32b>)
    502b:	000086        	j	5031 <xtensa_real_fft_32b+0x2d>

0000502e <xtensa_real_fft_32b+0x2a>:
    502e:	ec9651        	l32r	a5, 288 (4d98 <xtensa_fft128_32b>)
    5031:	03ad           	mov.n	a10, a3
    5033:	0005e0        	callx8	a5
    5036:	ec9551        	l32r	a5, 28c (4e24 <real_split>)
    5039:	02ad           	mov.n	a10, a2
    503b:	03bd           	mov.n	a11, a3
    503d:	04cd           	mov.n	a12, a4
    503f:	0005e0        	callx8	a5
    5042:	0358           	l32i.n	a5, a3, 0
    5044:	1368           	l32i.n	a6, a3, 4
    5046:	070c           	movi.n	a7, 0
    5048:	1379           	s32i.n	a7, a3, 4
    504a:	c05560        	sub	a5, a5, a6
    504d:	0359           	s32i.n	a5, a3, 0
    504f:	1922a6        	blti	a2, 2, 506c <xtensa_real_fft_32b+0x68>
    5052:	520b           	addi.n	a5, a2, -1
    5054:	448b           	addi.n	a4, a4, 8
    5056:	b02530        	addx8	a2, a5, a3
    5059:	0f8576        	loop	a5, 506c <xtensa_real_fft_32b+0x68>
    505c:	1438           	l32i.n	a3, a4, 4
    505e:	0458           	l32i.n	a5, a4, 0
    5060:	448b           	addi.n	a4, a4, 8
    5062:	603030        	neg	a3, a3
    5065:	0259           	s32i.n	a5, a2, 0
    5067:	1239           	s32i.n	a3, a2, 4
    5069:	f8c222        	addi	a2, a2, -8

0000506c <xtensa_real_fft_32b+0x68>:
    506c:	f01d           	retw.n

0000506e <xtensa_real_fft_32b+0x6a>:
	...

00005070 <xtensa_comp_fft_32b>:
    5070:	004136        	entry	a1, 32
    5073:	0bf2e6        	bgei	a2, 0x100, 5082 <xtensa_comp_fft_32b+0x12>
    5076:	17b226        	beqi	a2, 16, 5091 <xtensa_comp_fft_32b+0x21>
    5079:	22e266        	bnei	a2, 128, 509f <xtensa_comp_fft_32b+0x2f>
    507c:	ec8321        	l32r	a2, 288 (4d98 <xtensa_fft128_32b>)
    507f:	0005c6        	j	509a <xtensa_comp_fft_32b+0x2a>

00005082 <xtensa_comp_fft_32b+0x12>:
    5082:	11f226        	beqi	a2, 0x100, 5097 <xtensa_comp_fft_32b+0x27>
    5085:	00a242        	movi	a4, 0x200
    5088:	139247        	bne	a2, a4, 509f <xtensa_comp_fft_32b+0x2f>
    508b:	ec8121        	l32r	a2, 290 (4dfc <xtensa_fft512_32b>)
    508e:	000206        	j	509a <xtensa_comp_fft_32b+0x2a>

00005091 <xtensa_comp_fft_32b+0x21>:
    5091:	ec7921        	l32r	a2, 278 (4d1c <xtensa_fft16_32b>)
    5094:	000086        	j	509a <xtensa_comp_fft_32b+0x2a>

00005097 <xtensa_comp_fft_32b+0x27>:
    5097:	ec7b21        	l32r	a2, 284 (4dc0 <xtensa_fft256_32b>)
    509a:	03ad           	mov.n	a10, a3
    509c:	0002e0        	callx8	a2
    509f:	f01d           	retw.n

000050a1 <xtensa_comp_fft_32b+0x31>:
    50a1:	000000                                        ...

000050a4 <sw_fft_32b_2811>:
    50a4:	006136        	entry	a1, 48
    50a7:	0d1666        	bnei	a6, 1, 50b8 <sw_fft_32b_2811+0x14>
    50aa:	176c           	movi.n	a7, -31
    50ac:	f47070        	extui	a7, a7, 0, 16
    50af:	12f3e6        	bgei	a3, 0x100, 50c5 <sw_fft_32b_2811+0x21>
    50b2:	2fb366        	bnei	a3, 16, 50e5 <sw_fft_32b_2811+0x41>
    50b5:	000c86        	j	50eb <sw_fft_32b_2811+0x47>

000050b8 <sw_fft_32b_2811+0x14>:
    50b8:	569c           	beqz.n	a6, 50d1 <sw_fft_32b_2811+0x2d>
    50ba:	1173f0        	slli	a7, a3, 1
    50bd:	039d           	mov.n	a9, a3
    50bf:	2d13e6        	bgei	a3, 1, 50f0 <sw_fft_32b_2811+0x4c>
    50c2:	001506        	j	511a <sw_fft_32b_2811+0x76>

000050c5 <sw_fft_32b_2811+0x21>:
    50c5:	22f326        	beqi	a3, 0x100, 50eb <sw_fft_32b_2811+0x47>
    50c8:	00a282        	movi	a8, 0x200
    50cb:	1c1387        	beq	a3, a8, 50eb <sw_fft_32b_2811+0x47>
    50ce:	002846        	j	5173 <sw_fft_32b_2811+0xcf>

000050d1 <sw_fft_32b_2811+0x2d>:
    50d1:	176c           	movi.n	a7, -31
    50d3:	f47070        	extui	a7, a7, 0, 16
    50d6:	08e3e6        	bgei	a3, 128, 50e2 <sw_fft_32b_2811+0x3e>
    50d9:	0e8326        	beqi	a3, 8, 50eb <sw_fft_32b_2811+0x47>
    50dc:	0bd326        	beqi	a3, 64, 50eb <sw_fft_32b_2811+0x47>
    50df:	002406        	j	5173 <sw_fft_32b_2811+0xcf>

000050e2 <sw_fft_32b_2811+0x3e>:
    50e2:	05f326        	beqi	a3, 0x100, 50eb <sw_fft_32b_2811+0x47>
    50e5:	02e326        	beqi	a3, 128, 50eb <sw_fft_32b_2811+0x47>
    50e8:	0021c6        	j	5173 <sw_fft_32b_2811+0xcf>
    50eb:	939d           	ae_zext16	a9, a3
    50ed:	1179f0        	slli	a7, a9, 1
    50f0:	600204        	ae_lq32f.i	aeq0, a2, 0
    50f3:	028d           	mov.n	a8, a2
    50f5:	088776        	loop	a7, 5101 <sw_fft_32b_2811+0x5d>
    50f8:	3404b4        	ae_sraaq56	aeq0, aeq0, a4
    50fb:	620804        	ae_sq32f.i	aeq0, a8, 0
    50fe:	601814        	ae_lq32f.iu	aeq0, a8, 4

00005101 <sw_fft_32b_2811+0x5d>:
    5101:	151666        	bnei	a6, 1, 511a <sw_fft_32b_2811+0x76>
    5104:	ec6461        	l32r	a6, 294 (5070 <xtensa_comp_fft_32b>)
    5107:	03ad           	mov.n	a10, a3
    5109:	02bd           	mov.n	a11, a2
    510b:	3129           	s32i.n	a2, a1, 12
    510d:	0006e0        	callx8	a6
    5110:	030c           	movi.n	a3, 0
    5112:	1d1526        	beqi	a5, 1, 5133 <sw_fft_32b_2811+0x8f>
    5115:	001606        	j	5171 <sw_fft_32b_2811+0xcd>

00005118 <sw_fft_32b_2811+0x74>:
	...

0000511a <sw_fft_32b_2811+0x76>:
    511a:	ec5f61        	l32r	a6, 298 (5004 <xtensa_real_fft_32b>)
    511d:	03ad           	mov.n	a10, a3
    511f:	02bd           	mov.n	a11, a2
    5121:	3129           	s32i.n	a2, a1, 12
    5123:	092d           	mov.n	a2, a9
    5125:	0006e0        	callx8	a6
    5128:	11f2e0        	slli	a15, a2, 2
    512b:	603070        	neg	a3, a7
    512e:	0f7d           	mov.n	a7, a15
    5130:	3d1566        	bnei	a5, 1, 5171 <sw_fft_32b_2811+0xcd>
    5133:	3128           	l32i.n	a2, a1, 12
    5135:	a02320        	addx4	a2, a3, a2
    5138:	fd24bd020f 	{ ae_lq32f.i	aeq0, a2, 0; ae_zeroq56	aeq1 }
    513d:	0f17a6        	blti	a7, 1, 5150 <sw_fft_32b_2811+0xac>
    5140:	fe0453322f 	{ or	a3, a2, a2; ae_movq56	aeq2, aeq0 }
    5145:	078776        	loop	a7, 5150 <sw_fft_32b_2811+0xac>
    5148:	ff80bd531f 	{ ae_lq32f.iu	aeq2, a3, 4; ae_absq56	aeq3, aeq2 }
    514d:	0179e4        	ae_orq56	aeq1, aeq1, aeq3

00005150 <sw_fft_32b_2811+0xac>:
    5150:	621124        	ae_sq32f.i	aeq1, a1, 8
    5153:	2158           	l32i.n	a5, a1, 8
    5155:	030c           	movi.n	a3, 0
    5157:	40e560        	nsa	a6, a5
    515a:	b33650        	movgez	a3, a6, a5
    515d:	0d17a6        	blti	a7, 1, 516e <sw_fft_32b_2811+0xca>
    5160:	0a8776        	loop	a7, 516e <sw_fft_32b_2811+0xca>
    5163:	fc24fe830f 	{ ae_slaasq56s	aeq0, aeq0, a3; nop }
    5168:	620204        	ae_sq32f.i	aeq0, a2, 0
    516b:	601214        	ae_lq32f.iu	aeq0, a2, 4
    516e:	c04430        	sub	a4, a4, a3

00005171 <sw_fft_32b_2811+0xcd>:
    5171:	047d           	mov.n	a7, a4
    5173:	872d           	ae_sext16	a2, a7
    5175:	f01d           	retw.n

00005177 <sw_fft_32b_2811+0xd3>:
	...

00005178 <find_phase>:
    5178:	004136        	entry	a1, 32
    517b:	344030        	extui	a4, a3, 0, 4
    517e:	0e34a6        	blti	a4, 3, 5190 <find_phase+0x18>
    5181:	1954e6        	bgei	a4, 5, 519e <find_phase+0x26>
    5184:	253426        	beqi	a4, 3, 51ad <find_phase+0x35>
    5187:	374466        	bnei	a4, 4, 51c2 <find_phase+0x4a>
    518a:	600214        	ae_lq32f.i	aeq0, a2, 4
    518d:	000586        	j	51a7 <find_phase+0x2f>

00005190 <find_phase+0x18>:
    5190:	f49c           	beqz.n	a4, 51b3 <find_phase+0x3b>
    5192:	3b1426        	beqi	a4, 1, 51d1 <find_phase+0x59>
    5195:	292466        	bnei	a4, 2, 51c2 <find_phase+0x4a>
    5198:	600224        	ae_lq32f.i	aeq0, a2, 8
    519b:	000d46        	j	51d4 <find_phase+0x5c>

0000519e <find_phase+0x26>:
    519e:	175426        	beqi	a4, 5, 51b9 <find_phase+0x41>
    51a1:	1d6466        	bnei	a4, 6, 51c2 <find_phase+0x4a>
    51a4:	600224        	ae_lq32f.i	aeq0, a2, 8
    51a7:	604204        	ae_lq32f.i	aeq1, a2, 0
    51aa:	000746        	j	51cb <find_phase+0x53>

000051ad <find_phase+0x35>:
    51ad:	600214        	ae_lq32f.i	aeq0, a2, 4
    51b0:	000206        	j	51bc <find_phase+0x44>

000051b3 <find_phase+0x3b>:
    51b3:	600204        	ae_lq32f.i	aeq0, a2, 0
    51b6:	000686        	j	51d4 <find_phase+0x5c>

000051b9 <find_phase+0x41>:
    51b9:	600224        	ae_lq32f.i	aeq0, a2, 8
    51bc:	604204        	ae_lq32f.i	aeq1, a2, 0
    51bf:	000506        	j	51d7 <find_phase+0x5f>

000051c2 <find_phase+0x4a>:
    51c2:	600224        	ae_lq32f.i	aeq0, a2, 8
    51c5:	604214        	ae_lq32f.i	aeq1, a2, 4
    51c8:	0b7426        	beqi	a4, 7, 51d7 <find_phase+0x5f>
    51cb:	060824        	ae_negq56	aeq0, aeq0
    51ce:	000146        	j	51d7 <find_phase+0x5f>

000051d1 <find_phase+0x59>:
    51d1:	600214        	ae_lq32f.i	aeq0, a2, 4
    51d4:	064b34        	ae_movq56	aeq1, aeq0
    51d7:	0118c4        	ae_addq56	aeq0, aeq1, aeq0
    51da:	350074        	ae_sraiq56	aeq0, aeq0, 1
    51dd:	026347        	bbci	a3, 4, 51e3 <find_phase+0x6b>
    51e0:	060824        	ae_negq56	aeq0, aeq0
    51e3:	060934        	ae_roundsq32asym	aeq0, aeq0
    51e6:	368204        	ae_trunca32q48	a2, aeq0
    51e9:	f01d           	retw.n

000051eb <find_phase+0x73>:
	...

000051ec <memset_S16>:
    51ec:	004136        	entry	a1, 32
    51ef:	0614a6        	blti	a4, 1, 51f9 <memset_S16+0xd>
    51f2:	038476        	loop	a4, 51f9 <memset_S16+0xd>
    51f5:	623d           	ae_s16i.n	a3, a2, 0
    51f7:	222b           	addi.n	a2, a2, 2

000051f9 <memset_S16+0xd>:
    51f9:	f01d           	retw.n

000051fb <memset_S16+0xf>:
	...

000051fc <beam2_setup>:
    51fc:	004136        	entry	a1, 32
    51ff:	140c           	movi.n	a4, 1
    5201:	014440        	slli	a4, a4, 28
    5204:	c50c           	movi.n	a5, 12
    5206:	1f0c           	movi.n	a15, 1
    5208:	0c5232        	s16i	a3, a2, 24
    520b:	2249           	s32i.n	a4, a2, 8
    520d:	113f40        	slli	a3, a15, 12
    5210:	20c222        	addi	a2, a2, 32
    5213:	038576        	loop	a5, 521a <beam2_setup+0x1e>
    5216:	623d           	ae_s16i.n	a3, a2, 0
    5218:	222b           	addi.n	a2, a2, 2

0000521a <beam2_setup+0x1e>:
    521a:	f01d           	retw.n

0000521c <cal_vad_thrd_x1>:
    521c:	004136        	entry	a1, 32
    521f:	ebb351        	l32r	a5, ec (7800 <immediate_num>)
    5222:	440304        	ae_lp16x2f.i	aep0, a3, 0
    5225:	441204        	ae_lp16x2f.i	aep1, a2, 0
    5228:	dd056d257f 	{ ae_lp16x2f.i	aep2, a5, 28; ae_selp24.hh	aep3, aep1, aep0 }
    522d:	f06abe103f 	{ ae_selp24.ll	aep0, aep1, aep0; ae_mulzaafp24s.hh.ll	aeq0, aep2, aep3 }
    5232:	042804        	ae_mulzaafp24s.hh.ll	aeq1, aep2, aep0
    5235:	1b8f64        	ae_roundsp16q48asym	aep6, aeq0
    5238:	fb6bf2640f 	{ ae_sp16f.l.i	aep6, a4, 0; ae_roundsp16q48asym	aep7, aeq1 }
    523d:	427414        	ae_sp16f.l.i	aep7, a4, 2
    5240:	f01d           	retw.n

00005242 <cal_vad_thrd_x1+0x26>:
	...

00005244 <CH13_cb_setup>:
    5244:	004136        	entry	a1, 32
    5247:	14c232        	addi	a3, a2, 20
    524a:	6cc2f2        	addi	a15, a2, 108
    524d:	3239           	s32i.n	a3, a2, 12
    524f:	d2f9           	s32i.n	a15, a2, 52
    5251:	f01d           	retw.n

00005253 <CH13_cb_setup+0xf>:
	...

00005254 <TWO_MIC_WB_init>:
    5254:	004136        	entry	a1, 32
    5257:	eb7231        	l32r	a3, 20 (8688 <p_ecnr_mem>)
    525a:	9da4f2        	movi	a15, 0x49d
    525d:	eb9a41        	l32r	a4, c8 (6bf0 <xt_memset>)
    5260:	0358           	l32i.n	a5, a3, 0
    5262:	113fd0        	slli	a3, a15, 3
    5265:	0b0c           	movi.n	a11, 0
    5267:	353a           	add.n	a3, a5, a3
    5269:	0ca2c2        	movi	a12, 0x20c
    526c:	03ad           	mov.n	a10, a3
    526e:	0004e0        	callx8	a4
    5271:	84a772        	movi	a7, 0x784
    5274:	f6a062        	movi	a6, 246
    5277:	b4a042        	movi	a4, 180
    527a:	a07770        	addx4	a7, a7, a7
    527d:	24d662        	addmi	a6, a6, 0x2400
    5280:	b24a           	add.n	a11, a2, a4
    5282:	457a           	add.n	a4, a5, a7
    5284:	eb6671        	l32r	a7, 1c (6c04 <xt_memcpy>)
    5287:	a56a           	add.n	a10, a5, a6
    5289:	5c0c           	movi.n	a12, 5
    528b:	0007e0        	callx8	a7
    528e:	c0a092        	movi	a9, 192
    5291:	5f9282        	l16si	a8, a2, 190
    5294:	25d562        	addmi	a6, a5, 0x2500
    5297:	b29a           	add.n	a11, a2, a9
    5299:	4c0c           	movi.n	a12, 4
    529b:	04ad           	mov.n	a10, a4
    529d:	665682        	s16i	a8, a6, 204
    52a0:	0007e0        	callx8	a7
    52a3:	a4a042        	movi	a4, 164
    52a6:	25d442        	addmi	a4, a4, 0x2500
    52a9:	c8a082        	movi	a8, 200
    52ac:	b28a           	add.n	a11, a2, a8
    52ae:	a54a           	add.n	a10, a5, a4
    52b0:	8c0c           	movi.n	a12, 8
    52b2:	0007e0        	callx8	a7
    52b5:	6c92a2        	l16si	a10, a2, 216
    52b8:	28d5b2        	addmi	a11, a5, 0x2800
    52bb:	6f92c2        	l16si	a12, a2, 222
    52be:	a4a1f2        	movi	a15, 0x1a4
    52c1:	7e5ba2        	s16i	a10, a11, 252
    52c4:	5a56c2        	s16i	a12, a6, 180
    52c7:	6d9282        	l16si	a8, a2, 218
    52ca:	7092d2        	l16si	a13, a2, 224
    52cd:	24d5e2        	addmi	a14, a5, 0x2400
    52d0:	b2fa           	add.n	a11, a2, a15
    52d2:	6c0c           	movi.n	a12, 6
    52d4:	03ad           	mov.n	a10, a3
    52d6:	1f5682        	s16i	a8, a6, 62
    52d9:	7a5ed2        	s16i	a13, a14, 244
    52dc:	0007e0        	callx8	a7
    52df:	d7a732        	movi	a3, 0x7d7
    52e2:	0133d0        	slli	a3, a3, 19
    52e5:	3c0c           	movi.n	a12, 3
    52e7:	326632        	s32i	a3, a6, 200
    52ea:	013c50        	slli	a3, a12, 27
    52ed:	340c           	movi.n	a4, 3
    52ef:	596632        	s32i	a3, a6, 0x164
    52f2:	ebeae1        	l32r	a14, 29c (d330000 <_end+0xd327974>)
    52f5:	113430        	slli	a3, a4, 13
    52f8:	1a0c           	movi.n	a10, 1
    52fa:	5866e2        	s32i	a14, a6, 0x160
    52fd:	1e5632        	s16i	a3, a6, 60
    5300:	013a20        	slli	a3, a10, 30
    5303:	26d5c2        	addmi	a12, a5, 0x2600
    5306:	1b0c           	movi.n	a11, 1
    5308:	77a1e2        	movi	a14, 0x177
    530b:	a5a4a2        	movi	a10, 0x4a5
    530e:	014bb0        	slli	a4, a11, 21
    5311:	25d582        	addmi	a8, a5, 0x2500
    5314:	3e6632        	s32i	a3, a6, 248
    5317:	116ad0        	slli	a6, a10, 3
    531a:	156c32        	s32i	a3, a12, 84
    531d:	5c39           	s32i.n	a3, a12, 20
    531f:	ec39           	s32i.n	a3, a12, 56
    5321:	113ec0        	slli	a3, a14, 4
    5324:	74a0b2        	movi	a11, 116
    5327:	2ca4c2        	movi	a12, 0x42c
    532a:	bb5832        	s16i	a3, a8, 0x176
    532d:	b03cc0        	addx8	a3, a12, a12
    5330:	106842        	s32i	a4, a8, 64
    5333:	25db42        	addmi	a4, a11, 0x2500
    5336:	656a           	add.n	a6, a5, a6
    5338:	fca0d2        	movi	a13, 252
    533b:	ebd991        	l32r	a9, 2a0 (e390000 <_end+0xe387974>)
    533e:	136862        	s32i	a6, a8, 76
    5341:	25dd62        	addmi	a6, a13, 0x2500
    5344:	454a           	add.n	a4, a5, a4
    5346:	353a           	add.n	a3, a5, a3
    5348:	ce0c           	movi.n	a14, 12
    534a:	1f0c           	movi.n	a15, 1
    534c:	1f6832        	s32i	a3, a8, 124
    534f:	126892        	s32i	a9, a8, 72
    5352:	196842        	s32i	a4, a8, 100
    5355:	113f20        	slli	a3, a15, 14
    5358:	656a           	add.n	a6, a5, a6
    535a:	038e76        	loop	a14, 5361 <TWO_MIC_WB_init+0x10d>
    535d:	663d           	ae_s16i.n	a3, a6, 0
    535f:	662b           	addi.n	a6, a6, 2

00005361 <TWO_MIC_WB_init+0x10d>:
    5361:	2ad542        	addmi	a4, a5, 0x2a00
    5364:	27d5d2        	addmi	a13, a5, 0x2700
    5367:	1448           	l32i.n	a4, a4, 4
    5369:	7d49           	s32i.n	a4, a13, 28
    536b:	0248           	l32i.n	a4, a2, 0
    536d:	6f9232        	l16si	a3, a2, 222
    5370:	729282        	l16si	a8, a2, 228
    5373:	26d592        	addmi	a9, a5, 0x2600
    5376:	2bd5e2        	addmi	a14, a5, 0x2b00
    5379:	6a5982        	s16i	a8, a9, 212
    537c:	1e98           	l32i.n	a9, a14, 4
    537e:	3cc4f2        	addi	a15, a4, 60
    5381:	6d99           	s32i.n	a9, a13, 24
    5383:	5df9           	s32i.n	a15, a13, 20
    5385:	d31b           	addi.n	a13, a3, 1
    5387:	2ad592        	addmi	a9, a5, 0x2a00
    538a:	b3d330        	movgez	a13, a3, a3
    538d:	112982        	l32i	a8, a9, 68
    5390:	27d5a2        	addmi	a10, a5, 0x2700
    5393:	2a6a82        	s32i	a8, a10, 168
    5396:	112e82        	l32i	a8, a14, 68
    5399:	4131d0        	srli	a3, a13, 1
    539c:	25d562        	addmi	a6, a5, 0x2500
    539f:	296a82        	s32i	a8, a10, 164
    53a2:	50c4c2        	addi	a12, a4, 80
    53a5:	5c5632        	s16i	a3, a6, 184
    53a8:	28d5f2        	addmi	a15, a5, 0x2800
    53ab:	b12a32        	l32i	a3, a10, 0x2c4
    53ae:	286ac2        	s32i	a12, a10, 160
    53b1:	df39           	s32i.n	a3, a15, 52
    53b3:	312932        	l32i	a3, a9, 196
    53b6:	1c0c           	movi.n	a12, 1
    53b8:	26d5a2        	addmi	a10, a5, 0x2600
    53bb:	612f82        	l32i	a8, a15, 0x184
    53be:	dca0d2        	movi	a13, 220
    53c1:	cf39           	s32i.n	a3, a15, 48
    53c3:	bf49           	s32i.n	a4, a15, 44
    53c5:	016c40        	slli	a6, a12, 28
    53c8:	246a82        	s32i	a8, a10, 144
    53cb:	26dd82        	addmi	a8, a13, 0x2600
    53ce:	212932        	l32i	a3, a9, 132
    53d1:	28c442        	addi	a4, a4, 40
    53d4:	1e0c           	movi.n	a14, 1
    53d6:	cf0c           	movi.n	a15, 12
    53d8:	236a32        	s32i	a3, a10, 140
    53db:	113e40        	slli	a3, a14, 12
    53de:	858a           	add.n	a8, a5, a8
    53e0:	226a42        	s32i	a4, a10, 136
    53e3:	316a62        	s32i	a6, a10, 196
    53e6:	038f76        	loop	a15, 53ed <TWO_MIC_WB_init+0x199>
    53e9:	683d           	ae_s16i.n	a3, a8, 0
    53eb:	882b           	addi.n	a8, a8, 2
    53ed:	749242        	l16si	a4, a2, 232
    53f0:	eda4f2        	movi	a15, 0x4ed
    53f3:	27d592        	addmi	a9, a5, 0x2700
    53f6:	305942        	s16i	a4, a9, 96
    53f9:	114fd0        	slli	a4, a15, 3
    53fc:	ca0c           	movi.n	a10, 12
    53fe:	854a           	add.n	a8, a5, a4
    5400:	146962        	s32i	a6, a9, 80
    5403:	038a76        	loop	a10, 540a <TWO_MIC_WB_init+0x1b6>
    5406:	683d           	ae_s16i.n	a3, a8, 0
    5408:	882b           	addi.n	a8, a8, 2
    540a:	759242        	l16si	a4, a2, 234
    540d:	f4a0f2        	movi	a15, 244
    5410:	27d592        	addmi	a9, a5, 0x2700
    5413:	765942        	s16i	a4, a9, 236
    5416:	27df42        	addmi	a4, a15, 0x2700
    5419:	ca0c           	movi.n	a10, 12
    541b:	808540        	add	a8, a5, a4
    541e:	376962        	s32i	a6, a9, 220
    5421:	038a76        	loop	a10, 5428 <TWO_MIC_WB_init+0x1d4>
    5424:	683d           	ae_s16i.n	a3, a8, 0
    5426:	882b           	addi.n	a8, a8, 2
    5428:	739242        	l16si	a4, a2, 230
    542b:	84a4e2        	movi	a14, 0x484
    542e:	28d592        	addmi	a9, a5, 0x2800
    5431:	475942        	s16i	a4, a9, 142
    5434:	b04ee0        	addx8	a4, a14, a14
    5437:	cf0c           	movi.n	a15, 12
    5439:	246962        	s32i	a6, a9, 144
    543c:	654a           	add.n	a6, a5, a4
    543e:	038f76        	loop	a15, 5445 <TWO_MIC_WB_init+0x1f1>
    5441:	663d           	ae_s16i.n	a3, a6, 0
    5443:	662b           	addi.n	a6, a6, 2
    5445:	54a492        	movi	a9, 0x454
    5448:	b09990        	addx8	a9, a9, a9
    544b:	26d542        	addmi	a4, a5, 0x2600
    544e:	d59a           	add.n	a13, a5, a9
    5450:	84a7f2        	movi	a15, 0x784
    5453:	a09ff0        	addx4	a9, a15, a15
    5456:	2f64d2        	s32i	a13, a4, 188
    5459:	4d4c           	movi.n	a13, 68
    545b:	451594        	ae_lp16x2f.x	aep1, a5, a9
    545e:	28dd92        	addmi	a9, a13, 0x2800
    5461:	a4a0d2        	movi	a13, 164
    5464:	eb22f1        	l32r	a15, ec (7800 <immediate_num>)
    5467:	84a582        	movi	a8, 0x584
    546a:	25ddd2        	addmi	a13, a13, 0x2500
    546d:	f08880        	subx8	a8, a8, a8
    5470:	aca5c2        	movi	a12, 0x5ac
    5473:	4525d4        	ae_lp16x2f.x	aep2, a5, a13
    5476:	f0ccc0        	subx8	a12, a12, a12
    5479:	dd4550858f 	{ add	a8, a5, a8; ae_selp24.hh	aep3, aep1, aep2 }
    547e:	dc466d0f7f 	{ ae_lp16x2f.i	aep0, a15, 28; ae_selp24.ll	aep1, aep1, aep2 }
    5483:	f062800cef 	{ movi	a14, 12; ae_mulzaafp24s.hh.ll	aeq0, aep0, aep3 }
    5488:	256482        	s32i	a8, a4, 148
    548b:	28de42        	addmi	a4, a14, 0x2800
    548e:	27d5b2        	addmi	a11, a5, 0x2700
    5491:	f022d0c5cf 	{ add	a12, a5, a12; ae_mulzaafp24s.hh.ll	aeq1, aep0, aep1 }
    5496:	f963c0bcef 	{ movi	a14, 188; ae_roundsp16q48asym	aep3, aeq0 }
    549b:	fa4c           	movi.n	a10, 79
    549d:	2b6bc2        	s32i	a12, a11, 172
    54a0:	28dec2        	addmi	a12, a14, 0x2800
    54a3:	454a           	add.n	a4, a5, a4
    54a5:	fa6b809cef 	{ movi	a14, 156; ae_roundsp16q48asym	aep4, aeq1 }
    54aa:	11aa90        	slli	a10, a10, 7
    54ad:	356b42        	s32i	a4, a11, 212
    54b0:	25deb2        	addmi	a11, a14, 0x2500
    54b3:	b3a4f2        	movi	a15, 0x4b3
    54b6:	43bb54        	ae_sp16f.l.xu	aep3, a11, a5
    54b9:	27d532        	addmi	a3, a5, 0x2700
    54bc:	a5aa           	add.n	a10, a5, a10
    54be:	b5a4e2        	movi	a14, 0x4b5
    54c1:	114fd0        	slli	a4, a15, 3
    54c4:	424b14        	ae_sp16f.l.i	aep4, a11, 2
    54c7:	1263a2        	s32i	a10, a3, 72
    54ca:	11aed0        	slli	a10, a14, 3
    54cd:	e5a462        	movi	a6, 0x4e5
    54d0:	455544        	ae_lp16x2f.x	aep5, a5, a4
    54d3:	4525a4        	ae_lp16x2f.x	aep2, a5, a10
    54d6:	1166d0        	slli	a6, a6, 3
    54d9:	df5510656f 	{ add	a6, a5, a6; ae_selp24.hh	aep6, aep5, aep2 }
    54de:	f1c2be527f 	{ ae_selp24.ll	aep1, aep5, aep2; ae_mulzaafp24s.hh.ll	aeq2, aep0, aep6 }
    54e3:	f122d4836f 	{ s32i.n	a6, a3, 32; ae_mulzaafp24s.hh.ll	aeq3, aep0, aep1 }
    54e8:	fb73c12dff 	{ movi	a15, 0x12d; ae_roundsp16q48asym	aep7, aeq2 }
    54ed:	f87bc4cf6f 	{ movi	a6, 0x4cf; ae_roundsp16q48asym	aep1, aeq3 }
    54f2:	113fb0        	slli	a3, a15, 5
    54f5:	1146d0        	slli	a4, a6, 3
    54f8:	28d582        	addmi	a8, a5, 0x2800
    54fb:	b59a           	add.n	a11, a5, a9
    54fd:	94a1e2        	movi	a14, 0x194
    5500:	e8b9           	s32i.n	a11, a8, 56
    5502:	f5ca           	add.n	a15, a5, a12
    5504:	43f354        	ae_sp16f.l.xu	aep7, a3, a5
    5507:	b2ea           	add.n	a11, a2, a14
    5509:	a54a           	add.n	a10, a5, a4
    550b:	8c0c           	movi.n	a12, 8
    550d:	2168f2        	s32i	a15, a8, 132
    5510:	421314        	ae_sp16f.l.i	aep1, a3, 2
    5513:	0007e0        	callx8	a7
    5516:	eac221        	l32r	a2, 20 (8688 <p_ecnr_mem>)
    5519:	25d532        	addmi	a3, a5, 0x2500
    551c:	0228           	l32i.n	a2, a2, 0
    551e:	12d222        	addmi	a2, a2, 0x1200
    5521:	162222        	l32i	a2, a2, 88
    5524:	186322        	s32i	a2, a3, 96
    5527:	f01d           	retw.n

00005529 <TWO_MIC_WB_init+0x2d5>:
    5529:	000000                                        ...

0000552c <take_max_four>:
    552c:	004136        	entry	a1, 32
    552f:	224d           	ae_l16si.n	a4, a2, 0
    5531:	4e23a6        	blti	a3, 2, 5583 <take_max_four+0x57>
    5534:	f57c           	movi.n	a5, -1
    5536:	115510        	slli	a5, a5, 15
    5539:	630b           	addi.n	a6, a3, -1
    553b:	222b           	addi.n	a2, a2, 2
    553d:	048d           	mov.n	a8, a4
    553f:	053d           	mov.n	a3, a5
    5541:	057d           	mov.n	a7, a5
    5543:	318676        	loop	a6, 5578 <take_max_four+0x4c>
    5546:	22ad           	ae_l16si.n	a10, a2, 0
    5548:	086d           	mov.n	a6, a8
    554a:	079d           	mov.n	a9, a7
    554c:	0a4d           	mov.n	a4, a10
    554e:	1c28a7        	blt	a8, a10, 556e <take_max_four+0x42>
    5551:	084d           	mov.n	a4, a8
    5553:	0a6d           	mov.n	a6, a10
    5555:	079d           	mov.n	a9, a7
    5557:	1327a7        	blt	a7, a10, 556e <take_max_four+0x42>
    555a:	084d           	mov.n	a4, a8
    555c:	076d           	mov.n	a6, a7
    555e:	0a9d           	mov.n	a9, a10
    5560:	0a23a7        	blt	a3, a10, 556e <take_max_four+0x42>
    5563:	5355a0        	max	a5, a5, a10
    5566:	039d           	mov.n	a9, a3
    5568:	084d           	mov.n	a4, a8
    556a:	076d           	mov.n	a6, a7
    556c:	053d           	mov.n	a3, a5

0000556e <take_max_four+0x42>:
    556e:	035d           	mov.n	a5, a3
    5570:	222b           	addi.n	a2, a2, 2
    5572:	093d           	mov.n	a3, a9
    5574:	067d           	mov.n	a7, a6
    5576:	048d           	mov.n	a8, a4

00005578 <take_max_four+0x4c>:
    5578:	249a           	add.n	a2, a4, a9
    557a:	365a           	add.n	a3, a6, a5
    557c:	223a           	add.n	a2, a2, a3
    557e:	112220        	slli	a2, a2, 14
    5581:	f01d           	retw.n

00005583 <take_max_four+0x57>:
    5583:	f67c           	movi.n	a6, -1
    5585:	116610        	slli	a6, a6, 15
    5588:	069d           	mov.n	a9, a6
    558a:	065d           	mov.n	a5, a6
    558c:	fffa06        	j	5578 <take_max_four+0x4c>

0000558f <take_max_four+0x63>:
	...

00005590 <prelim_filter>:
    5590:	012136        	entry	a1, 144
    5593:	40c262        	addi	a6, a2, 64
    5596:	46c2c2        	addi	a12, a2, 70
    5599:	f3f7c0        	wur.ae_cend0	a12
    559c:	f3f660        	wur.ae_cbegin0	a6
    559f:	22c2f2        	addi	a15, a2, 34
    55a2:	e149           	s32i.n	a4, a1, 56
    55a4:	6922d2        	l32i	a13, a2, 0x1a4
    55a7:	6a22e2        	l32i	a14, a2, 0x1a8
    55aa:	682272        	l32i	a7, a2, 0x1a0
    55ad:	192252        	l32i	a5, a2, 100
    55b0:	24c242        	addi	a4, a2, 36
    55b3:	c129           	s32i.n	a2, a1, 48
    55b5:	eac421        	l32r	a2, c8 (6bf0 <xt_memset>)
    55b8:	400f04        	ae_lp16f.i	aep0, a15, 0
    55bb:	60c1a2        	addi	a10, a1, 96
    55be:	0b0c           	movi.n	a11, 0
    55c0:	6c0c           	movi.n	a12, 6
    55c2:	d139           	s32i.n	a3, a1, 52
    55c4:	21d9           	s32i.n	a13, a1, 8
    55c6:	b1e9           	s32i.n	a14, a1, 44
    55c8:	41f9           	s32i.n	a15, a1, 16
    55ca:	560134        	ae_sp24x2s.i	aep0, a1, 24
    55cd:	0002e0        	callx8	a2
    55d0:	21e8           	l32i.n	a14, a1, 8
    55d2:	b1f8           	l32i.n	a15, a1, 44
    55d4:	eb3481        	l32r	a8, 2a4 (ccccc <_end+0xc4640>)
    55d7:	5cc122        	addi	a2, a1, 92
    55da:	40c192        	addi	a9, a1, 64
    55dd:	39a884        	ae_movpa24x2	aep2, a8, a8
    55e0:	f129           	s32i.n	a2, a1, 60
    55e2:	541134        	ae_lp24x2.i	aep1, a1, 24
    55e5:	0aa062        	movi	a6, 10
    55e8:	02a032        	movi	a3, 2
    55eb:	fd248feeae 	{ addi	a10, a14, -2; ae_zeroq56	aeq1 }
    55f0:	fec772        	addi	a7, a7, -2
    55f3:	fecf22        	addi	a2, a15, -2
    55f6:	03a0b2        	movi	a11, 3
    55f9:	fec9c2        	addi	a12, a9, -2
    55fc:	562144        	ae_sp24x2s.i	aep2, a1, 32

000055ff <prelim_filter+0x6f>:
    55ff:	408a14        	ae_lp16f.iu	aep0, a10, 2
    5602:	0b61a2        	s32i	a10, a1, 44
    5605:	608404        	ae_lq32f.i	aeq2, a4, 0
    5608:	fc0c62853f 	{ ae_sp16f.l.c	aep0, a5, a3; ae_movq56	aeq0, aeq1 }
    560d:	0c8b76        	loop	a11, 561d <prelim_filter+0x8d>
    5610:	fc24d9053f 	{ ae_lp16f.c	aep0, a5, a3; nop }
    5615:	f682fd541f 	{ ae_lq32f.iu	aeq2, a4, 4; ae_mulafq32sp16s.l	aeq1, aeq2, aep0 }
    561a:	010034        	ae_mulafp24s.ll	aeq0, aep0, aep0

0000561d <prelim_filter+0x8d>:
    561d:	ff2c3a909f 	{ ae_slliq56	aeq2, aeq1, 1; ae_cvtq48p24s.l	aeq3, aep1 }
    5622:	f9763b002f 	{ ae_sraiq56	aeq0, aeq0, 2; ae_addq56	aeq1, aeq2, aeq1 }
    5627:	fc244001af 	{ movi	a10, 1; ae_roundsq32asym	aeq0, aeq0 }
    562c:	f93eff08ef 	{ ae_trunca32q48	a8, aeq0; ae_subq56	aeq1, aeq3, aeq1 }
    5631:	f86bb2971f 	{ ae_sp16f.l.iu	aep1, a7, 2; ae_roundsp16q48asym	aep0, aeq1 }
    5636:	0009d4        	ae_cvta32p24.l	a9, aep0
    5639:	11aa20        	slli	a10, a10, 14
    563c:	53b8a0        	max	a11, a8, a10
    563f:	eaad81        	l32r	a8, f4 (4630 <airoha_divide>)
    5642:	428c14        	ae_sp16f.l.iu	aep0, a12, 2
    5645:	21a690        	srai	a10, a9, 6
    5648:	21c9           	s32i.n	a12, a1, 8
    564a:	0008e0        	callx8	a8
    564d:	360b34        	ae_cvtq48a32s	aeq0, a11
    5650:	f188           	l32i.n	a8, a1, 60
    5652:	fc24fe8a0f 	{ ae_slaasq56s	aeq0, aeq0, a10; nop }
    5657:	3b0c           	movi.n	a11, 3
    5659:	f8678f044e 	{ addi	a4, a4, -16; ae_roundsp24q48asym	aep0, aeq0 }
    565e:	542144        	ae_lp24x2.i	aep2, a1, 32
    5661:	1a8b76        	loop	a11, 567f <prelim_filter+0xef>
    5664:	601414        	ae_lq32f.iu	aeq0, a4, 4
    5667:	fc24d9153f 	{ ae_lp16f.c	aep1, a5, a3; nop }
    566c:	e223bd381f 	{ ae_lq32f.iu	aeq1, a8, 4; ae_mulafp24s.ll	aeq0, aep0, aep1 }
    5671:	060934        	ae_roundsq32asym	aeq0, aeq0
    5674:	f4caf9840f 	{ ae_sq32f.i	aeq0, a4, 0; ae_mulafq32sp24s.l	aeq1, aeq0, aep2 }
    5679:	06d934        	ae_roundsq32asym	aeq3, aeq1
    567c:	623804        	ae_sq32f.i	aeq3, a8, 0
    567f:	b1a8           	l32i.n	a10, a1, 44
    5681:	21c8           	l32i.n	a12, a1, 8
    5683:	660b           	addi.n	a6, a6, -1
    5685:	fd248f844e 	{ addi	a4, a4, -8; ae_zeroq56	aeq1 }
    568a:	409214        	ae_lp16f.iu	aep1, a2, 2
    568d:	f6e656        	bnez	a6, 55ff <prelim_filter+0x6f>
    5690:	fc24b0c16e 	{ l32i	a6, a1, 48; ae_zeroq56	aeq0 }
    5695:	042122        	l32i	a2, a1, 16
    5698:	eb0441        	l32r	a4, 2a8 (393e4b <_end+0x38b7bf>)
    569b:	fe0444013e 	{ addi	a3, a1, 64; ae_movq56	aeq2, aeq0 }
    56a0:	421204        	ae_sp16f.l.i	aep1, a2, 0
    56a3:	682622        	l32i	a2, a6, 0x1a0
    56a6:	0aa0f2        	movi	a15, 10
    56a9:	398444        	ae_movpa24x2	aep0, a4, a4
    56ac:	196652        	s32i	a5, a6, 100
    56af:	401304        	ae_lp16f.i	aep1, a3, 0
    56b2:	fec242        	addi	a4, a2, -2
    56b5:	158f76        	loop	a15, 56ce <prelim_filter+0x13e>
    56b8:	ea21ece41f 	{ ae_lp16f.iu	aep6, a4, 2; ae_mulfp24s.ll	aeq1, aep0, aep1 }
    56bd:	ebc1ec931f 	{ ae_lp16f.iu	aep1, a3, 2; ae_mulfp24s.ll	aeq3, aep0, aep6 }
    56c2:	1b9d24        	ae_roundsp24q48asym	aep2, aeq1
    56c5:	1bbd74        	ae_roundsp24q48asym	aep7, aeq3
    56c8:	012234        	ae_mulafp24s.ll	aeq0, aep2, aep2
    56cb:	01f734        	ae_mulafp24s.ll	aeq2, aep7, aep7
    56ce:	fd2440014f 	{ movi	a4, 1; ae_roundsq32asym	aeq1, aeq0 }
    56d3:	114400        	slli	a4, a4, 16
    56d6:	fc147ec4ff 	{ ae_cvtq48a32s	aeq3, a4; ae_movq56	aeq0, aeq2 }
    56db:	ea8431        	l32r	a3, ec (7800 <immediate_num>)
    56de:	fe2445c65e 	{ addi	a5, a6, 92; ae_roundsq32asym	aeq2, aeq0 }
    56e3:	f8aafa312f 	{ ae_sq56s.i	aeq3, a1, 16; ae_subq56	aeq0, aeq1, aeq2 }
    56e8:	60c504        	ae_lq32f.i	aeq3, a5, 0
    56eb:	633114        	ae_sq56s.i	aeq3, a1, 8
    56ee:	61c124        	ae_lq56.i	aeq3, a1, 16
    56f1:	d6c42d037f 	{ ae_lp16x2f.i	aep0, a3, 28; ae_ltq56s	b0, aeq3, aeq0 }
    56f6:	6005f4        	ae_lq32f.i	aeq0, a5, -4
    56f9:	0e003de11f 	{ ae_lq56.i	aeq3, a1, 8; ae_mulzaafq32sp16s.lh	aeq0, aeq2, aep0, aeq0, aep0 }
    56fe:	0c03c000ef 	{ movi	a14, 0; ae_mulzaafq32sp16s.lh	aeq1, aeq1, aep0, aeq3, aep0 }
    5703:	fc244001ff 	{ movi	a15, 1; ae_roundsq32asym	aeq0, aeq0 }
    5708:	fd2c56ef0f 	{ movt	a14, a15, b0; ae_roundsq32asym	aeq1, aeq1 }
    570d:	621504        	ae_sq32f.i	aeq1, a5, 0
    5710:	6205f4        	ae_sq32f.i	aeq0, a5, -4
    5713:	5eac           	beqz.n	a14, 573c <prelim_filter+0x1ac>
    5715:	40c132        	addi	a3, a1, 64
    5718:	80a042        	movi	a4, 128
    571b:	5f0c           	movi.n	a15, 5
    571d:	398444        	ae_movpa24x2	aep0, a4, a4
    5720:	fcc222        	addi	a2, a2, -4
    5723:	441304        	ae_lp16x2f.i	aep1, a3, 0
    5726:	128f76        	loop	a15, 573c <prelim_filter+0x1ac>
    5729:	cc04eda21f 	{ ae_lp16x2f.iu	aep2, a2, 4; ae_sraip24	aep1, aep1, 1 }
    572e:	0f20a4        	ae_sraip24	aep2, aep2, 1
    5731:	d9292d931f 	{ ae_lp16x2f.iu	aep1, a3, 4; ae_addsp24s	aep2, aep2, aep1 }
    5736:	07a024        	ae_addsp24s	aep2, aep2, aep0
    5739:	4623f4        	ae_sp16x2f.i	aep2, a3, -4

0000573c <prelim_filter+0x1ac>:
    573c:	ea6821        	l32r	a2, dc (155555 <_end+0x14cec9>)
    573f:	4cc632        	addi	a3, a6, 76
    5742:	0b9652        	l16si	a5, a6, 22
    5745:	398224        	ae_movpa24x2	aep0, a2, a2
    5748:	600304        	ae_lq32f.i	aeq0, a3, 0
    574b:	f6c2b1664e 	{ l32i	a4, a6, 88; ae_mulafq32sp24s.l	aeq0, aeq2, aep0 }
    5750:	11f520        	slli	a15, a5, 14
    5753:	fc247176ae 	{ l32i	a10, a6, 92; ae_roundsq32asym	aeq0, aeq0 }
    5758:	ea6621        	l32r	a2, f0 (4738 <airoha_divide_limit_1>)
    575b:	53b4f0        	max	a11, a4, a15
    575e:	620304        	ae_sq32f.i	aeq0, a3, 0
    5761:	0002e0        	callx8	a2
    5764:	ead241        	l32r	a4, 2ac (175e97 <_end+0x16d80b>)
    5767:	379aa4        	ae_truncp24a32x2	aep1, a10, a10
    576a:	f8e6e2044f 	{ ae_movpa24x2	aep0, a4, a4; ae_roundsp16asym	aep1, aep1 }
    576f:	692642        	l32i	a4, a6, 0x1a4
    5772:	20c652        	addi	a5, a6, 32
    5775:	421504        	ae_sp16f.l.i	aep1, a5, 0
    5778:	af0c           	movi.n	a15, 10
    577a:	600314        	ae_lq32f.i	aeq0, a3, 4
    577d:	401404        	ae_lp16f.i	aep1, a4, 0
    5780:	0a8f76        	loop	a15, 578e <prelim_filter+0x1fe>
    5783:	ea21ec941f 	{ ae_lp16f.iu	aep1, a4, 2; ae_mulfp24s.ll	aeq1, aep0, aep1 }
    5788:	1b9d24        	ae_roundsp24q48asym	aep2, aeq1
    578b:	012234        	ae_mulafp24s.ll	aeq0, aep2, aep2

0000578e <prelim_filter+0x1fe>:
    578e:	fc2462b6be 	{ l16si	a11, a6, 86; ae_roundsq32asym	aeq0, aeq0 }
    5793:	620314        	ae_sq32f.i	aeq0, a3, 4
    5796:	eac631        	l32r	a3, 2b0 (5178 <find_phase>)
    5799:	60c1a2        	addi	a10, a1, 96
    579c:	0003e0        	callx8	a3
    579f:	1626f2        	l32i	a15, a6, 88
    57a2:	ffa042        	movi	a4, 255
    57a5:	00a472        	movi	a7, 0x400
    57a8:	0a5d           	mov.n	a5, a10
    57aa:	068d           	mov.n	a8, a6
    57ac:	7fd442        	addmi	a4, a4, 0x7f00
    57af:	030c           	movi.n	a3, 0
    57b1:	502f77        	blt	a15, a7, 5805 <prelim_filter+0x275>
    57b4:	109832        	l16si	a3, a8, 32
    57b7:	089862        	l16si	a6, a8, 16
    57ba:	0998b2        	l16si	a11, a8, 18
    57bd:	c0a360        	sub	a10, a3, a6
    57c0:	130c           	movi.n	a3, 1
    57c2:	3f1aa6        	blti	a10, 1, 5805 <prelim_filter+0x275>
    57c5:	086d           	mov.n	a6, a8
    57c7:	1baab7        	bge	a10, a11, 57e6 <prelim_filter+0x256>
    57ca:	40c142        	addi	a4, a1, 64
    57cd:	0002e0        	callx8	a2
    57d0:	a20c           	movi.n	a2, 10
    57d2:	378aa4        	ae_truncp24a32x2	aep0, a10, a10
    57d5:	401404        	ae_lp16f.i	aep1, a4, 0
    57d8:	0a8276        	loop	a2, 57e6 <prelim_filter+0x256>
    57db:	ea05ac941f 	{ ae_lp16f.iu	aep1, a4, 2; ae_mulfp24s.ll	aeq0, aep1, aep0 }
    57e0:	1b8f24        	ae_roundsp16q48asym	aep2, aeq0
    57e3:	4224f4        	ae_sp16f.l.i	aep2, a4, -2

000057e6 <prelim_filter+0x256>:
    57e6:	682642        	l32i	a4, a6, 0x1a0
    57e9:	40c122        	addi	a2, a1, 64
    57ec:	05a0f2        	movi	a15, 5
    57ef:	440204        	ae_lp16x2f.i	aep0, a2, 0
    57f2:	fcc442        	addi	a4, a4, -4
    57f5:	0a8f76        	loop	a15, 5803 <prelim_filter+0x273>
    57f8:	449414        	ae_lp16x2f.iu	aep1, a4, 4
    57fb:	dc06ed821f 	{ ae_lp16x2f.iu	aep0, a2, 4; ae_subsp24s	aep1, aep1, aep0 }
    5800:	461404        	ae_sp16x2f.i	aep1, a4, 0

00005803 <prelim_filter+0x273>:
    5803:	040c           	movi.n	a4, 0

00005805 <prelim_filter+0x275>:
    5805:	d128           	l32i.n	a2, a1, 52
    5807:	e1f8           	l32i.n	a15, a1, 56
    5809:	0259           	s32i.n	a5, a2, 0
    580b:	1239           	s32i.n	a3, a2, 4
    580d:	6f4d           	ae_s16i.n	a4, a15, 0
    580f:	f01d           	retw.n

00005811 <prelim_filter+0x281>:
    5811:	000000                                        ...

00005814 <prelim_filter3>:
    5814:	00a136        	entry	a1, 80
    5817:	8159           	s32i.n	a5, a1, 32
    5819:	2b1252        	l16ui	a5, a2, 86
    581c:	1c3466        	bnei	a4, 3, 583c <prelim_filter3+0x28>
    581f:	170c           	movi.n	a7, 1
    5821:	854d           	ae_sext16	a4, a5
    5823:	34a262        	movi	a6, 0x234
    5826:	117740        	slli	a7, a7, 12
    5829:	2f0c           	movi.n	a15, 2
    582b:	30a2e2        	movi	a14, 0x230
    582e:	626a           	add.n	a6, a2, a6
    5830:	a90c           	movi.n	a9, 10
    5832:	3179           	s32i.n	a7, a1, 12
    5834:	22ea           	add.n	a2, a2, a14
    5836:	214540        	srai	a4, a4, 5
    5839:	000646        	j	5856 <prelim_filter3+0x42>

0000583c <prelim_filter3+0x28>:
    583c:	170c           	movi.n	a7, 1
    583e:	854d           	ae_sext16	a4, a5
    5840:	c0a262        	movi	a6, 0x2c0
    5843:	117720        	slli	a7, a7, 14
    5846:	5f0c           	movi.n	a15, 5
    5848:	bca2e2        	movi	a14, 0x2bc
    584b:	626a           	add.n	a6, a2, a6
    584d:	491c           	movi.n	a9, 20
    584f:	3179           	s32i.n	a7, a1, 12
    5851:	22ea           	add.n	a2, a2, a14
    5853:	214a40        	srai	a4, a4, 10
    5856:	51f9           	s32i.n	a15, a1, 20
    5858:	0288           	l32i.n	a8, a2, 0
    585a:	0668           	l32i.n	a6, a6, 0
    585c:	0358           	l32i.n	a5, a3, 0
    585e:	16c3a2        	addi	a10, a3, 22
    5861:	10c3f2        	addi	a15, a3, 16
    5864:	734b           	addi.n	a7, a3, 4
    5866:	4149           	s32i.n	a4, a1, 16
    5868:	7139           	s32i.n	a3, a1, 28
    586a:	240c           	movi.n	a4, 2
    586c:	3b0c           	movi.n	a11, 3
    586e:	f3f6f0        	wur.ae_cbegin0	a15
    5871:	f3f7a0        	wur.ae_cend0	a10
    5874:	61a9           	s32i.n	a10, a1, 24
    5876:	fc24ac0a0f 	{ ae_lp16f.i	aep0, a10, 0; ae_zeroq56	aeq0 }
    587b:	093d           	mov.n	a3, a9
    587d:	fec622        	addi	a2, a6, -2
    5880:	fec882        	addi	a8, a8, -2
    5883:	9179           	s32i.n	a7, a1, 36
    5885:	a199           	s32i.n	a9, a1, 40
    5887:	409814        	ae_lp16f.iu	aep1, a8, 2
    588a:	0b6182        	s32i	a8, a1, 44
    588d:	608704        	ae_lq32f.i	aeq2, a7, 0
    5890:	fd0462954f 	{ ae_sp16f.l.c	aep1, a5, a4; ae_movq56	aeq1, aeq0 }
    5895:	0c8b76        	loop	a11, 58a5 <prelim_filter3+0x91>
    5898:	fc24d9154f 	{ ae_lp16f.c	aep1, a5, a4; nop }
    589d:	f686fd571f 	{ ae_lq32f.iu	aeq2, a7, 4; ae_mulafq32sp16s.l	aeq1, aeq2, aep1 }
    58a2:	011134        	ae_mulafp24s.ll	aeq0, aep1, aep1

000058a5 <prelim_filter3+0x91>:
    58a5:	ff0c3a909f 	{ ae_slliq56	aeq2, aeq1, 1; ae_cvtq48p24s.l	aeq3, aep0 }
    58aa:	f97630518e 	{ l32i	a8, a1, 20; ae_addq56	aeq1, aeq2, aeq1 }
    58af:	f93efb002f 	{ ae_sraiq56	aeq0, aeq0, 2; ae_subq56	aeq1, aeq3, aeq1 }
    58b4:	1b9f74        	ae_roundsp16q48asym	aep7, aeq1
    58b7:	fc247f7f5f 	{ ae_cvta32p24.l	a15, aep7; ae_roundsq32asym	aeq0, aeq0 }
    58bc:	400800        	ssr	a8
    58bf:	368b04        	ae_trunca32q48	a11, aeq0
    58c2:	ea0b81        	l32r	a8, f0 (4738 <airoha_divide_limit_1>)
    58c5:	b1a0f0        	sra	a10, a15
    58c8:	0008e0        	callx8	a8
    58cb:	3b0c           	movi.n	a11, 3
    58cd:	f4c782        	addi	a8, a7, -12
    58d0:	6007d4        	ae_lq32f.i	aeq0, a7, -12
    58d3:	378aa4        	ae_truncp24a32x2	aep0, a10, a10
    58d6:	108b76        	loop	a11, 58ea <prelim_filter3+0xd6>
    58d9:	fc24d9154f 	{ ae_lp16f.c	aep1, a5, a4; nop }
    58de:	010134        	ae_mulafp24s.ll	aeq0, aep0, aep1
    58e1:	060934        	ae_roundsq32asym	aeq0, aeq0
    58e4:	620804        	ae_sq32f.i	aeq0, a8, 0
    58e7:	601814        	ae_lq32f.iu	aeq0, a8, 4
    58ea:	fc248f487e 	{ addi	a7, a8, -12; ae_zeroq56	aeq0 }
    58ef:	a198           	l32i.n	a9, a1, 40
    58f1:	b188           	l32i.n	a8, a1, 44
    58f3:	330b           	addi.n	a3, a3, -1
    58f5:	408214        	ae_lp16f.iu	aep0, a2, 2
    58f8:	f8b356        	bnez	a3, 5887 <prelim_filter3+0x73>
    58fb:	7128           	l32i.n	a2, a1, 28
    58fd:	61e8           	l32i.n	a14, a1, 24
    58ff:	41f190        	srli	a15, a9, 1
    5902:	006252        	s32i	a5, a2, 0
    5905:	fc24b20e0f 	{ ae_sp16f.l.i	aep0, a14, 0; ae_zeroq56	aeq0 }
    590a:	440604        	ae_lp16x2f.i	aep0, a6, 0
    590d:	048f76        	loop	a15, 5915 <prelim_filter3+0x101>
    5910:	e2002d861f 	{ ae_lp16x2f.iu	aep0, a6, 4; ae_mulaafp24s.hh.ll	aeq0, aep0, aep0 }
    5915:	fc2470313e 	{ l32i	a3, a1, 12; ae_roundsq32asym	aeq0, aeq0 }
    591a:	368204        	ae_trunca32q48	a2, aeq0
    591d:	91a8           	l32i.n	a10, a1, 36
    591f:	41b8           	l32i.n	a11, a1, 16
    5921:	722320        	salt	a2, a3, a2
    5924:	ea6331        	l32r	a3, 2b0 (5178 <find_phase>)
    5927:	0003e0        	callx8	a3
    592a:	8138           	l32i.n	a3, a1, 32
    592c:	1329           	s32i.n	a2, a3, 4
    592e:	03a9           	s32i.n	a10, a3, 0
    5930:	f01d           	retw.n

00005932 <prelim_filter3+0x11e>:
	...

00005934 <cm_short_vad>:
    5934:	004136        	entry	a1, 32
    5937:	235d           	ae_l16si.n	a5, a3, 0
    5939:	024d           	mov.n	a4, a2
    593b:	420c           	movi.n	a2, 4
    593d:	192547        	blt	a5, a4, 595a <cm_short_vad+0x26>
    5940:	335d           	ae_l16si.n	a5, a3, 2
    5942:	220c           	movi.n	a2, 2
    5944:	029362        	l16si	a6, a3, 4
    5947:	0f2547        	blt	a5, a4, 595a <cm_short_vad+0x26>
    594a:	020c           	movi.n	a2, 0
    594c:	0a2647        	blt	a6, a4, 595a <cm_short_vad+0x26>
    594f:	039322        	l16si	a2, a3, 6
    5952:	cf7c           	movi.n	a15, -4
    5954:	722240        	salt	a2, a2, a4
    5957:	9022f0        	addx2	a2, a2, a15

0000595a <cm_short_vad+0x26>:
    595a:	f01d           	retw.n

0000595c <cali_module_short>:
    595c:	004136        	entry	a1, 32
    595f:	e9b041        	l32r	a4, 20 (8688 <p_ecnr_mem>)
    5962:	050c           	movi.n	a5, 0
    5964:	0448           	l32i.n	a4, a4, 0
    5966:	12d442        	addmi	a4, a4, 0x1200
    5969:	279442        	l16si	a4, a4, 78
    596c:	4b24a6        	blti	a4, 2, 59bb <cali_module_short+0x5f>
    596f:	1348           	l32i.n	a4, a3, 4
    5971:	0378           	l32i.n	a7, a3, 0
    5973:	2b0c           	movi.n	a11, 2
    5975:	5014a6        	blti	a4, 1, 59c9 <cali_module_short+0x6d>
    5978:	579282        	l16si	a8, a2, 174
    597b:	689262        	l16si	a6, a2, 208
    597e:	317070        	srai	a7, a7, 16
    5981:	47a877        	bge	a8, a7, 59cc <cali_module_short+0x70>
    5984:	569252        	l16si	a5, a2, 172
    5987:	669292        	l16si	a9, a2, 204
    598a:	6e92a2        	l16si	a10, a2, 220
    598d:	727570        	salt	a7, a5, a7
    5990:	9057b0        	addx2	a5, a7, a11
    5993:	773b           	addi.n	a7, a7, 3
    5995:	676a           	add.n	a6, a7, a6
    5997:	436690        	min	a6, a6, a9
    599a:	6b2ae6        	bgei	a10, 2, 5a09 <cali_module_short+0xad>
    599d:	6d9272        	l16si	a7, a2, 218
    59a0:	04d292        	addmi	a9, a2, 0x400
    59a3:	0a99a2        	l16si	a10, a9, 20
    59a6:	871b           	addi.n	a8, a7, 1
    59a8:	887d           	ae_sext16	a7, a8
    59aa:	6d5282        	s16i	a8, a2, 218
    59ad:	38c7a6        	blti	a7, 32, 59e9 <cali_module_short+0x8d>
    59b0:	00a282        	movi	a8, 0x200
    59b3:	4d9787        	bne	a7, a8, 5a04 <cali_module_short+0xa8>
    59b6:	270c           	movi.n	a7, 2
    59b8:	001286        	j	5a06 <cali_module_short+0xaa>

000059bb <cali_module_short+0x5f>:
    59bb:	040c           	movi.n	a4, 0
    59bd:	675242        	s16i	a4, a2, 206
    59c0:	6b5242        	s16i	a4, a2, 214
    59c3:	695242        	s16i	a4, a2, 210
    59c6:	003a46        	j	5ab3 <cali_module_short+0x157>

000059c9 <cali_module_short+0x6d>:
    59c9:	000fc6        	j	5a0c <cali_module_short+0xb0>

000059cc <cali_module_short+0x70>:
    59cc:	589282        	l16si	a8, a2, 176
    59cf:	050c           	movi.n	a5, 0
    59d1:	599292        	l16si	a9, a2, 178
    59d4:	312877        	blt	a8, a7, 5a09 <cali_module_short+0xad>
    59d7:	f9c652        	addi	a5, a6, -7
    59da:	1ca977        	bge	a9, a7, 59fa <cali_module_short+0x9e>
    59dd:	560b           	addi.n	a5, a6, -1
    59df:	0f0c           	movi.n	a15, 0
    59e1:	5365f0        	max	a6, a5, a15
    59e4:	e57c           	movi.n	a5, -2
    59e6:	0007c6        	j	5a09 <cali_module_short+0xad>

000059e9 <cali_module_short+0x8d>:
    59e9:	162282        	l32i	a8, a2, 88
    59ec:	117a60        	slli	a7, a10, 10
    59ef:	0b0c           	movi.n	a11, 0
    59f1:	14a877        	bge	a8, a7, 5a09 <cali_module_short+0xad>
    59f4:	6d52b2        	s16i	a11, a2, 218
    59f7:	000386        	j	5a09 <cali_module_short+0xad>

000059fa <cali_module_short+0x9e>:
    59fa:	0f0c           	movi.n	a15, 0
    59fc:	5365f0        	max	a6, a5, a15
    59ff:	c57c           	movi.n	a5, -4
    5a01:	000106        	j	5a09 <cali_module_short+0xad>

00005a04 <cali_module_short+0xa8>:
    5a04:	170c           	movi.n	a7, 1
    5a06:	6e5272        	s16i	a7, a2, 220
    5a09:	685262        	s16i	a6, a2, 208
    5a0c:	3368           	l32i.n	a6, a3, 12
    5a0e:	675252        	s16i	a5, a2, 206
    5a11:	070c           	movi.n	a7, 0
    5a13:	6716a6        	blti	a6, 1, 5a7e <cali_module_short+0x122>
    5a16:	2368           	l32i.n	a6, a3, 8
    5a18:	5e9282        	l16si	a8, a2, 188
    5a1b:	450c           	movi.n	a5, 4
    5a1d:	317060        	srai	a7, a6, 16
    5a20:	1a2877        	blt	a8, a7, 5a3e <cali_module_short+0xe2>
    5a23:	5f9282        	l16si	a8, a2, 190
    5a26:	250c           	movi.n	a5, 2
    5a28:	609292        	l16si	a9, a2, 192
    5a2b:	0f2877        	blt	a8, a7, 5a3e <cali_module_short+0xe2>
    5a2e:	050c           	movi.n	a5, 0
    5a30:	0a2977        	blt	a9, a7, 5a3e <cali_module_short+0xe2>
    5a33:	619252        	l16si	a5, a2, 194
    5a36:	cf7c           	movi.n	a15, -4
    5a38:	725570        	salt	a5, a5, a7
    5a3b:	9055f0        	addx2	a5, a5, a15
    5a3e:	695252        	s16i	a5, a2, 210
    5a41:	3c14a6        	blti	a4, 1, 5a81 <cali_module_short+0x125>
    5a44:	e9aa41        	l32r	a4, ec (7800 <immediate_num>)
    5a47:	0358           	l32i.n	a5, a3, 0
    5a49:	378564        	ae_truncp24a32x2	aep0, a5, a6
    5a4c:	441474        	ae_lp16x2f.i	aep1, a4, 28
    5a4f:	f006a5a2fe 	{ l16si	a15, a2, 180; ae_mulzaafp24s.hh.ll	aeq0, aep1, aep0 }
    5a54:	450c           	movi.n	a5, 4
    5a56:	f863a5b26e 	{ l16si	a6, a2, 182; ae_roundsp16q48asym	aep0, aeq0 }
    5a5b:	360424        	ae_trunca16p24s.l	a4, aep0
    5a5e:	5c9272        	l16si	a7, a2, 184
    5a61:	1c2f47        	blt	a15, a4, 5a81 <cali_module_short+0x125>
    5a64:	844d           	ae_sext16	a4, a4
    5a66:	250c           	movi.n	a5, 2
    5a68:	152647        	blt	a6, a4, 5a81 <cali_module_short+0x125>
    5a6b:	050c           	movi.n	a5, 0
    5a6d:	102747        	blt	a7, a4, 5a81 <cali_module_short+0x125>
    5a70:	5d9252        	l16si	a5, a2, 186
    5a73:	cf7c           	movi.n	a15, -4
    5a75:	724540        	salt	a4, a5, a4
    5a78:	9054f0        	addx2	a5, a4, a15
    5a7b:	000086        	j	5a81 <cali_module_short+0x125>

00005a7e <cali_module_short+0x122>:
    5a7e:	695272        	s16i	a7, a2, 210
    5a81:	5368           	l32i.n	a6, a3, 20
    5a83:	040c           	movi.n	a4, 0
    5a85:	6b5252        	s16i	a5, a2, 214
    5a88:	2716a6        	blti	a6, 1, 5ab3 <cali_module_short+0x157>
    5a8b:	4338           	l32i.n	a3, a3, 16
    5a8d:	629252        	l16si	a5, a2, 196
    5a90:	440c           	movi.n	a4, 4
    5a92:	313030        	srai	a3, a3, 16
    5a95:	1a2537        	blt	a5, a3, 5ab3 <cali_module_short+0x157>
    5a98:	639252        	l16si	a5, a2, 198
    5a9b:	240c           	movi.n	a4, 2
    5a9d:	649262        	l16si	a6, a2, 200
    5aa0:	0f2537        	blt	a5, a3, 5ab3 <cali_module_short+0x157>
    5aa3:	040c           	movi.n	a4, 0
    5aa5:	0a2637        	blt	a6, a3, 5ab3 <cali_module_short+0x157>
    5aa8:	659242        	l16si	a4, a2, 202
    5aab:	cf7c           	movi.n	a15, -4
    5aad:	723430        	salt	a3, a4, a3
    5ab0:	9043f0        	addx2	a4, a3, a15
    5ab3:	6a5242        	s16i	a4, a2, 212
    5ab6:	f01d           	retw.n

00005ab8 <LH_34_1_setup>:
    5ab8:	004136        	entry	a1, 32
    5abb:	0348           	l32i.n	a4, a3, 0
    5abd:	13d8           	l32i.n	a13, a3, 4
    5abf:	02e8           	l32i.n	a14, a2, 0
    5ac1:	315040        	srai	a5, a4, 16
    5ac4:	f8d552        	addmi	a5, a5, 0xfffff800
    5ac7:	a55d           	ae_clamps16	a5, a5
    5ac9:	82f8           	l32i.n	a15, a2, 32
    5acb:	fecd32        	addi	a3, a13, -2
    5ace:	115500        	slli	a5, a5, 16
    5ad1:	835430        	moveqz	a5, a4, a3
    5ad4:	533e50        	max	a3, a14, a5
    5ad7:	434f50        	min	a4, a15, a5
    5ada:	0239           	s32i.n	a3, a2, 0
    5adc:	8249           	s32i.n	a4, a2, 32
    5ade:	f01d           	retw.n

00005ae0 <cali_power>:
    5ae0:	006136        	entry	a1, 48
    5ae3:	e94f51        	l32r	a5, 20 (8688 <p_ecnr_mem>)
    5ae6:	0a0c           	movi.n	a10, 0
    5ae8:	0558           	l32i.n	a5, a5, 0
    5aea:	12d552        	addmi	a5, a5, 0x1200
    5aed:	fd24a2755e 	{ l16si	a5, a5, 78; ae_zeroq56	aeq1 }
    5af2:	0225e6        	bgei	a5, 2, 5af8 <cali_power+0x18>
    5af5:	009b86        	j	5d67 <cali_power+0x287>
    5af8:	1448           	l32i.n	a4, a4, 4
    5afa:	0214e6        	bgei	a4, 1, 5b00 <cali_power+0x20>
    5afd:	009986        	j	5d67 <cali_power+0x287>
    5b00:	1c1342        	l16ui	a4, a3, 56
    5b03:	721352        	l16ui	a5, a3, 228
    5b06:	300454        	ae_cvtp24a16x2.ll	aep0, a4, a5
    5b09:	e800a6c3ee 	{ l16si	a14, a3, 216; ae_mulfp24s.hh	aeq0, aep0, aep0 }
    5b0e:	6922f2        	l32i	a15, a2, 0x1a4
    5b11:	d4e236a22e 	{ l32i	a2, a2, 0x1a8; ae_mulfq32sp16s.l	aeq0, aeq0, aep0 }
    5b16:	4e1b           	addi.n	a4, a14, 1
    5b18:	644040        	extui	a4, a4, 0, 7
    5b1b:	f86380056f 	{ movi	a6, 5; ae_roundsp16q48asym	aep0, aeq0 }
    5b20:	6c5342        	s16i	a4, a3, 216
    5b23:	fc0c77011f 	{ ae_sp24x2s.i	aep0, a1, 8; ae_movq56	aeq0, aeq1 }
    5b28:	fccf42        	addi	a4, a15, -4
    5b2b:	440204        	ae_lp16x2f.i	aep0, a2, 0
    5b2e:	098676        	loop	a6, 5b3b <cali_power+0x5b>
    5b31:	e2002d941f 	{ ae_lp16x2f.iu	aep1, a4, 4; ae_mulaafp24s.hh.ll	aeq0, aep0, aep0 }
    5b36:	e2246d821f 	{ ae_lp16x2f.iu	aep0, a2, 4; ae_mulaafp24s.hh.ll	aeq1, aep1, aep1 }

00005b3b <cali_power+0x5b>:
    5b3b:	739322        	l16si	a2, a3, 230
    5b3e:	140c           	movi.n	a4, 1
    5b40:	114410        	slli	a4, a4, 15
    5b43:	3500b4        	ae_sraiq56	aeq0, aeq0, 2
    5b46:	fc2440c45f 	{ movi	a5, 196; ae_roundsq32asym	aeq0, aeq0 }
    5b4b:	c04420        	sub	a4, a4, a2
    5b4e:	3550b4        	ae_sraiq56	aeq1, aeq1, 2
    5b51:	fd2c40c86f 	{ movi	a6, 200; ae_roundsq32asym	aeq1, aeq1 }
    5b56:	60b534        	ae_lq32f.xu	aeq2, a5, a3
    5b59:	300244        	ae_cvtp24a16x2.ll	aep0, a2, a4
    5b5c:	0e003c663f 	{ ae_lq32f.xu	aeq3, a6, a3; ae_mulzaafq32sp16s.lh	aeq0, aeq2, aep0, aeq0, aep0 }
    5b61:	0e01c001ef 	{ movi	a14, 1; ae_mulzaafq32sp16s.lh	aeq1, aeq3, aep0, aeq1, aep0 }
    5b66:	060934        	ae_roundsq32asym	aeq0, aeq0
    5b69:	fd2c79850f 	{ ae_sq32f.i	aeq0, a5, 0; ae_roundsq32asym	aeq1, aeq1 }
    5b6e:	621604        	ae_sq32f.i	aeq1, a6, 0
    5b71:	312352        	l32i	a5, a3, 196
    5b74:	3223f2        	l32i	a15, a3, 200
    5b77:	112e40        	slli	a2, a14, 12
    5b7a:	21a350        	srai	a10, a5, 3
    5b7d:	e95c51        	l32r	a5, f0 (4738 <airoha_divide_limit_1>)
    5b80:	53b2f0        	max	a11, a2, a15
    5b83:	0005e0        	callx8	a5
    5b86:	212342        	l32i	a4, a3, 132
    5b89:	292372        	l32i	a7, a3, 164
    5b8c:	6c1392        	l16ui	a9, a3, 216
    5b8f:	5384a0        	max	a8, a4, a10
    5b92:	4347a0        	min	a4, a7, a10
    5b95:	0a2d           	mov.n	a2, a10
    5b97:	216382        	s32i	a8, a3, 132
    5b9a:	296342        	s32i	a4, a3, 164
    5b9d:	052956        	bnez	a9, 5bf3 <cali_power+0x113>
    5ba0:	6d9372        	l16si	a7, a3, 218
    5ba3:	378884        	ae_truncp24a32x2	aep0, a8, a8
    5ba6:	f8e2ab944f 	{ ae_truncp24a32x2	aep1, a4, a4; ae_roundsp16asym	aep0, aep0 }
    5bab:	f8e6c088df 	{ movi	a13, 136; ae_roundsp16asym	aep1, aep1 }
    5bb0:	a8a0e2        	movi	a14, 168
    5bb3:	e9c081        	l32r	a8, 2b4 (552c <take_max_four>)
    5bb6:	a3da           	add.n	a10, a3, a13
    5bb8:	11f7f0        	slli	a15, a7, 1
    5bbb:	cb0c           	movi.n	a11, 12
    5bbd:	43ea           	add.n	a4, a3, a14
    5bbf:	430af4        	ae_sp16f.l.x	aep0, a10, a15
    5bc2:	4314f4        	ae_sp16f.l.x	aep1, a4, a15
    5bc5:	0008e0        	callx8	a8
    5bc8:	e9a181        	l32r	a8, 24c (3bd8 <take_min_four>)
    5bcb:	2863a2        	s32i	a10, a3, 160
    5bce:	cb0c           	movi.n	a11, 12
    5bd0:	04ad           	mov.n	a10, a4
    5bd2:	0008e0        	callx8	a8
    5bd5:	e9b841        	l32r	a4, 2b8 (c0010000 <_end+0xc0007974>)
    5bd8:	ad0c           	movi.n	a13, 10
    5bda:	216342        	s32i	a4, a3, 132
    5bdd:	724d70        	salt	a4, a13, a7
    5be0:	e71b           	addi.n	a14, a7, 1
    5be2:	0f0c           	movi.n	a15, 0
    5be4:	e9b681        	l32r	a8, 2bc (3fff0000 <_end+0x3ffe7974>)
    5be7:	83fe40        	moveqz	a15, a14, a4
    5bea:	3063a2        	s32i	a10, a3, 192
    5bed:	296382        	s32i	a8, a3, 164
    5bf0:	6d53f2        	s16i	a15, a3, 218

00005bf3 <cali_power+0x113>:
    5bf3:	0648           	l32i.n	a4, a6, 0
    5bf5:	1f0c           	movi.n	a15, 1
    5bf7:	10df62        	addmi	a6, a15, 0x1000
    5bfa:	02a467        	bge	a4, a6, 5c00 <cali_power+0x120>
    5bfd:	0027c6        	j	5ca0 <cali_power+0x1c0>
    5c00:	36c234        	ae_cvtq48a32s	aeq3, a2
    5c03:	540114        	ae_lp24x2.i	aep0, a1, 8
    5c06:	d6e300c04f 	{ movi	a4, 192; ae_mulfq32sp16s.l	aeq0, aeq3, aep0 }
    5c0b:	606344        	ae_lq32f.x	aeq1, a3, a4
    5c0e:	d6440000ff 	{ movi	a15, 0; ae_ltq56s	b0, aeq1, aeq0 }
    5c13:	160c           	movi.n	a6, 1
    5c15:	e93541        	l32r	a4, ec (7800 <immediate_num>)
    5c18:	d3f600        	movt	a15, a6, b0
    5c1b:	8fac           	beqz.n	a15, 5c47 <cali_power+0x167>
    5c1d:	fc2440d06f 	{ movi	a6, 208; ae_roundsq32asym	aeq0, aeq0 }
    5c22:	cca0f2        	movi	a15, 204
    5c25:	38c442        	addi	a4, a4, 56
    5c28:	440404        	ae_lp16x2f.i	aep0, a4, 0
    5c2b:	607634        	ae_lq32f.xu	aeq1, a6, a3
    5c2e:	0c03ed142f 	{ ae_lp16x2f.i	aep1, a4, 8; ae_mulzaafq32sp16s.lh	aeq1, aeq1, aep0, aeq3, aep0 }
    5c33:	60bf34        	ae_lq32f.xu	aeq2, a15, a3
    5c36:	c11184        	ae_mulzaafq32sp16s.lh	aeq0, aeq2, aep1, aeq0, aep1
    5c39:	065934        	ae_roundsq32asym	aeq1, aeq1
    5c3c:	fc2479960f 	{ ae_sq32f.i	aeq1, a6, 0; ae_roundsq32asym	aeq0, aeq0 }
    5c41:	620f04        	ae_sq32f.i	aeq0, a15, 0
    5c44:	001606        	j	5ca0 <cali_power+0x1c0>

00005c47 <cali_power+0x167>:
    5c47:	a0a042        	movi	a4, 160
    5c4a:	602344        	ae_lq32f.x	aeq0, a3, a4
    5c4d:	d4e20000ff 	{ movi	a15, 0; ae_mulfq32sp16s.l	aeq0, aeq0, aep0 }
    5c52:	160c           	movi.n	a6, 1
    5c54:	d6c430011f 	{ ae_lp24x2.i	aep0, a1, 8; ae_ltq56s	b0, aeq3, aeq0 }
    5c59:	000bd4        	ae_cvta32p24.l	a11, aep0
    5c5c:	e92641        	l32r	a4, f4 (4630 <airoha_divide>)
    5c5f:	d3f600        	movt	a15, a6, b0
    5c62:	afbc           	beqz.n	a15, 5ca0 <cali_power+0x1c0>
    5c64:	02ad           	mov.n	a10, a2
    5c66:	633114        	ae_sq56s.i	aeq3, a1, 8
    5c69:	0004e0        	callx8	a4
    5c6c:	e92041        	l32r	a4, ec (7800 <immediate_num>)
    5c6f:	360b34        	ae_cvtq48a32s	aeq0, a11
    5c72:	d0a062        	movi	a6, 208
    5c75:	fc24fe8a0f 	{ ae_slaasq56s	aeq0, aeq0, a10; nop }
    5c7a:	fc2440cc7f 	{ movi	a7, 204; ae_roundsq32asym	aeq0, aeq0 }
    5c7f:	38c442        	addi	a4, a4, 56
    5c82:	607634        	ae_lq32f.xu	aeq1, a6, a3
    5c85:	441414        	ae_lp16x2f.i	aep1, a4, 4
    5c88:	0c253c473f 	{ ae_lq32f.xu	aeq2, a7, a3; ae_mulzaafq32sp16s.lh	aeq0, aeq1, aep1, aeq0, aep1 }
    5c8d:	440404        	ae_lp16x2f.i	aep0, a4, 0
    5c90:	fc247de11f 	{ ae_lq56.i	aeq3, a1, 8; ae_roundsq32asym	aeq0, aeq0 }
    5c95:	0f02b9860f 	{ ae_sq32f.i	aeq0, a6, 0; ae_mulzaafq32sp16s.lh	aeq2, aeq2, aep0, aeq3, aep0 }
    5c9a:	06e934        	ae_roundsq32asym	aeq3, aeq2
    5c9d:	623704        	ae_sq32f.i	aeq3, a7, 0
    5ca0:	332342        	l32i	a4, a3, 204
    5ca3:	342362        	l32i	a6, a3, 208
    5ca6:	c02240        	sub	a2, a2, a4
    5ca9:	c0b640        	sub	a11, a6, a4
    5cac:	21a120        	srai	a10, a2, 1
    5caf:	0005e0        	callx8	a5
    5cb2:	761322        	l16ui	a2, a3, 236
    5cb5:	f550a0        	extui	a5, a10, 16, 16
    5cb8:	7413d2        	l16ui	a13, a3, 232
    5cbb:	824d           	ae_sext16	a4, a2
    5cbd:	112400        	slli	a2, a4, 16
    5cc0:	33aa27        	bge	a10, a2, 5cf7 <cali_power+0x217>
    5cc3:	6f9342        	l16si	a4, a3, 222
    5cc6:	852c           	movi.n	a5, 40
    5cc8:	7513c2        	l16ui	a12, a3, 234
    5ccb:	441b           	addi.n	a4, a4, 1
    5ccd:	844d           	ae_sext16	a4, a4
    5ccf:	434450        	min	a4, a4, a5
    5cd2:	3004c4        	ae_cvtp24a16x2.ll	aep0, a4, a12
    5cd5:	ea0080d4df 	{ movi	a13, 212; ae_mulfp24s.hl	aeq0, aep0, aep0 }
    5cda:	6063d4        	ae_lq32f.x	aeq1, a3, a13
    5cdd:	360294        	ae_sllisq56s	aeq0, aeq0, 10
    5ce0:	f82ac000ff 	{ movi	a15, 0; ae_subq56	aeq0, aeq1, aeq0 }
    5ce5:	060934        	ae_roundsq32asym	aeq0, aeq0
    5ce8:	368e04        	ae_trunca32q48	a14, aeq0
    5ceb:	6f5342        	s16i	a4, a3, 222
    5cee:	534ef0        	max	a4, a14, a15
    5cf1:	356342        	s32i	a4, a3, 212
    5cf4:	000a46        	j	5d21 <cali_power+0x241>

00005cf7 <cali_power+0x217>:
    5cf7:	c04540        	sub	a4, a5, a4
    5cfa:	3004d4        	ae_cvtp24a16x2.ll	aep0, a4, a13
    5cfd:	e80080d4ef 	{ movi	a14, 212; ae_mulfp24s.hh	aeq0, aep0, aep0 }
    5d02:	d4e20000ff 	{ movi	a15, 0; ae_mulfq32sp16s.l	aeq0, aeq0, aep0 }
    5d07:	6f53f2        	s16i	a15, a3, 222
    5d0a:	3603d4        	ae_sllisq56s	aeq0, aeq0, 15
    5d0d:	fc247c2e3f 	{ ae_lq32f.xu	aeq1, a14, a3; ae_roundsq32asym	aeq0, aeq0 }
    5d12:	350174        	ae_sraiq56	aeq0, aeq0, 5
    5d15:	0118c4        	ae_addq56	aeq0, aeq1, aeq0
    5d18:	060934        	ae_roundsq32asym	aeq0, aeq0
    5d1b:	620e04        	ae_sq32f.i	aeq0, a14, 0
    5d1e:	352342        	l32i	a4, a3, 212
    5d21:	150c           	movi.n	a5, 1
    5d23:	015520        	slli	a5, a5, 30
    5d26:	214140        	srai	a4, a4, 1
    5d29:	445a           	add.n	a4, a4, a5
    5d2b:	378a44        	ae_truncp24a32x2	aep0, a10, a4
    5d2e:	e801bec27f 	{ ae_cvtq48a32s	aeq1, a2; ae_mulfp24s.lh	aeq0, aep0, aep0 }
    5d33:	0e0c           	movi.n	a14, 0
    5d35:	350054        	ae_slliq56	aeq0, aeq0, 1
    5d38:	d5440001ff 	{ movi	a15, 1; ae_leq56s	b0, aeq1, aeq0 }
    5d3d:	060c           	movi.n	a6, 0
    5d3f:	140c           	movi.n	a4, 1
    5d41:	d3ef00        	movt	a14, a15, b0
    5d44:	6e8c           	beqz.n	a14, 5d4e <cali_power+0x26e>
    5d46:	6e9322        	l16si	a2, a3, 220
    5d49:	224b           	addi.n	a2, a2, 4
    5d4b:	000546        	j	5d64 <cali_power+0x284>

00005d4e <cali_power+0x26e>:
    5d4e:	779322        	l16si	a2, a3, 238
    5d51:	6e9352        	l16si	a5, a3, 220
    5d54:	112200        	slli	a2, a2, 16
    5d57:	364234        	ae_cvtq48a32s	aeq1, a2
    5d5a:	d6480fc52e 	{ addi	a2, a5, -4; ae_ltq56s	b0, aeq0, aeq1 }
    5d5f:	d36400        	movt	a6, a4, b0
    5d62:	168c           	beqz.n	a6, 5d67 <cali_power+0x287>
    5d64:	6e5322        	s16i	a2, a3, 220
    5d67:	3120a0        	srai	a2, a10, 16
    5d6a:	705322        	s16i	a2, a3, 224
    5d6d:	f01d           	retw.n

00005d6f <cali_power+0x28f>:
	...

00005d70 <mag_calibration>:
    5d70:	004136        	entry	a1, 32
    5d73:	e8de31        	l32r	a3, ec (7800 <immediate_num>)
    5d76:	e8a042        	movi	a4, 232
    5d79:	4cc2e2        	addi	a14, a2, 76
    5d7c:	28c362        	addi	a6, a3, 40
    5d7f:	603424        	ae_lq32f.xu	aeq0, a4, a2
    5d82:	440604        	ae_lp16x2f.i	aep0, a6, 0
    5d85:	604e04        	ae_lq32f.i	aeq1, a14, 0
    5d88:	0c0080ecff 	{ movi	a15, 236; ae_mulzaafq32sp16s.lh	aeq0, aeq0, aep0, aeq1, aep0 }
    5d8d:	608e14        	ae_lq32f.i	aeq2, a14, 4
    5d90:	fc247c6f2f 	{ ae_lq32f.xu	aeq3, a15, a2; ae_roundsq32asym	aeq0, aeq0 }
    5d95:	0e0379840f 	{ ae_sq32f.i	aeq0, a4, 0; ae_mulzaafq32sp16s.lh	aeq1, aeq3, aep0, aeq2, aep0 }
    5d9a:	3a22b2        	l32i	a11, a2, 232
    5d9d:	ff2c40015f 	{ movi	a5, 1; ae_roundsq32asym	aeq3, aeq1 }
    5da2:	10d552        	addmi	a5, a5, 0x1000
    5da5:	623f04        	ae_sq32f.i	aeq3, a15, 0
    5da8:	02ab57        	bge	a11, a5, 5dae <mag_calibration+0x3e>
    5dab:	004386        	j	5ebd <mag_calibration+0x14d>
    5dae:	3b2242        	l32i	a4, a2, 236
    5db1:	8ea152        	movi	a5, 0x18e
    5db4:	21a240        	srai	a10, a4, 2
    5db7:	e8ce41        	l32r	a4, f0 (4738 <airoha_divide_limit_1>)
    5dba:	0004e0        	callx8	a4
    5dbd:	6c9242        	l16si	a4, a2, 216
    5dc0:	378aa4        	ae_truncp24a32x2	aep0, a10, a10
    5dc3:	f8e2b3c26e 	{ l32i	a6, a2, 240; ae_roundsp16asym	aep0, aep0 }
    5dc8:	441b           	addi.n	a4, a4, 1
    5dca:	94fd           	ae_zext16	a15, a4
    5dcc:	430254        	ae_sp16f.l.x	aep0, a2, a5
    5dcf:	6c5242        	s16i	a4, a2, 216
    5dd2:	02cf26        	beqi	a15, 32, 5dd8 <mag_calibration+0x68>
    5dd5:	0033c6        	j	5ea8 <mag_calibration+0x138>
    5dd8:	739262        	l16si	a6, a2, 230
    5ddb:	f4a072        	movi	a7, 244
    5dde:	a27a           	add.n	a10, a2, a7
    5de0:	040c           	movi.n	a4, 0
    5de2:	791252        	l16ui	a5, a2, 242
    5de5:	9066a0        	addx2	a6, a6, a10
    5de8:	6c5242        	s16i	a4, a2, 216
    5deb:	665d           	ae_s16i.n	a5, a6, 0
    5ded:	e93151        	l32r	a5, 2b4 (552c <take_max_four>)
    5df0:	cb0c           	movi.n	a11, 12
    5df2:	0005e0        	callx8	a5
    5df5:	739272        	l16si	a7, a2, 230
    5df8:	14a182        	movi	a8, 0x114
    5dfb:	4362a2        	s32i	a10, a2, 0x10c
    5dfe:	a28a           	add.n	a10, a2, a8
    5e00:	891252        	l16ui	a5, a2, 0x112
    5e03:	9077a0        	addx2	a7, a7, a10
    5e06:	675d           	ae_s16i.n	a5, a7, 0
    5e08:	e91151        	l32r	a5, 24c (3bd8 <take_min_four>)
    5e0b:	cb0c           	movi.n	a11, 12
    5e0d:	0ca162        	movi	a6, 0x10c
    5e10:	0005e0        	callx8	a5
    5e13:	e4a052        	movi	a5, 228
    5e16:	418524        	ae_lp16f.xu	aep0, a5, a2
    5e19:	ea01fb926f 	{ ae_lq32f.x	aeq0, a2, a6; ae_mulfp24s.ll	aeq1, aep0, aep0 }
    5e1e:	4b62a2        	s32i	a10, a2, 0x12c
    5e21:	f86b812cef 	{ movi	a14, 0x12c; ae_roundsp16q48asym	aep0, aeq1 }
    5e26:	d5e23bb2ef 	{ ae_lq32f.x	aeq1, a2, a14; ae_mulfq32sp16s.l	aeq2, aeq0, aep0 }
    5e2b:	0f0c           	movi.n	a15, 0
    5e2d:	d65400017f 	{ movi	a7, 1; ae_ltq56s	b0, aeq1, aeq2 }
    5e32:	cda062        	movi	a6, 205
    5e35:	6cd662        	addmi	a6, a6, 0x6c00
    5e38:	d3f700        	movt	a15, a7, b0
    5e3b:	6fac           	beqz.n	a15, 5e65 <mag_calibration+0xf5>
    5e3d:	fe3458866f 	{ ae_cvtp24a16x2.ll	aep0, a6, a6; ae_roundsq32asym	aeq2, aeq2 }
    5e42:	d7e20000ff 	{ movi	a15, 0; ae_mulfq32sp16s.l	aeq2, aeq2, aep0 }
    5e47:	170c           	movi.n	a7, 1
    5e49:	d6c800e06f 	{ movi	a6, 224; ae_ltq56s	b0, aeq2, aeq1 }
    5e4e:	2cc332        	addi	a3, a3, 44
    5e51:	d3f700        	movt	a15, a7, b0
    5e54:	df8c           	beqz.n	a15, 5e65 <mag_calibration+0xf5>
    5e56:	440304        	ae_lp16x2f.i	aep0, a3, 0
    5e59:	607624        	ae_lq32f.xu	aeq1, a6, a2
    5e5c:	c10044        	ae_mulzaafq32sp16s.lh	aeq0, aeq1, aep0, aeq0, aep0
    5e5f:	060934        	ae_roundsq32asym	aeq0, aeq0
    5e62:	620604        	ae_sq32f.i	aeq0, a6, 0

00005e65 <mag_calibration+0xf5>:
    5e65:	3822b2        	l32i	a11, a2, 224
    5e68:	e87131        	l32r	a3, 2c (514 <HIFI_sqrt>)
    5e6b:	0a0c           	movi.n	a10, 0
    5e6d:	0003e0        	callx8	a3
    5e70:	7392d2        	l16si	a13, a2, 230
    5e73:	c79232        	l16si	a3, a2, 0x18e
    5e76:	360b34        	ae_cvtq48a32s	aeq0, a11
    5e79:	fc24fe8a0f 	{ ae_slaasq56s	aeq0, aeq0, a10; nop }
    5e7e:	fc246c050f 	{ ae_lp16f.i	aep0, a5, 0; ae_roundsq32asym	aeq0, aeq0 }
    5e83:	d4e2001d5e 	{ addi	a5, a13, 1; ae_mulfq32sp16s.l	aeq0, aeq0, aep0 }
    5e88:	113300        	slli	a3, a3, 16
    5e8b:	85ed           	ae_sext16	a14, a5
    5e8d:	b60c           	movi.n	a6, 11
    5e8f:	3c6232        	s32i	a3, a2, 240
    5e92:	446232        	s32i	a3, a2, 0x110
    5e95:	7236e0        	salt	a3, a6, a14
    5e98:	935430        	movnez	a5, a4, a3
    5e9b:	f8638542fe 	{ addi	a15, a2, 84; ae_roundsp16q48asym	aep0, aeq0 }
    5ea0:	735252        	s16i	a5, a2, 230
    5ea3:	420f04        	ae_sp16f.l.i	aep0, a15, 0
    5ea6:	f01d           	retw.n

00005ea8 <mag_calibration+0x138>:
    5ea8:	c79232        	l16si	a3, a2, 0x18e
    5eab:	442252        	l32i	a5, a2, 0x110
    5eae:	113300        	slli	a3, a3, 16
    5eb1:	534630        	max	a4, a6, a3
    5eb4:	433530        	min	a3, a5, a3
    5eb7:	3c6242        	s32i	a4, a2, 240
    5eba:	446232        	s32i	a3, a2, 0x110
    5ebd:	f01d           	retw.n

00005ebf <mag_calibration+0x14f>:
	...

00005ec0 <beam1_proc>:
    5ec0:	008136        	entry	a1, 64
    5ec3:	5169           	s32i.n	a6, a1, 20
    5ec5:	fc2491664f 	{ addx2	a6, a6, a4; ae_zeroq56	aeq0 }
    5eca:	3815a6        	blti	a5, 1, 5f06 <beam1_proc+0x46>
    5ecd:	5f0c           	movi.n	a15, 5
    5ecf:	4179           	s32i.n	a7, a1, 16
    5ed1:	440604        	ae_lp16x2f.i	aep0, a6, 0
    5ed4:	064d           	mov.n	a4, a6
    5ed6:	048f76        	loop	a15, 5ede <beam1_proc+0x1e>
    5ed9:	e2002d841f 	{ ae_lp16x2f.iu	aep0, a4, 4; ae_mulaafp24s.hh.ll	aeq0, aep0, aep0 }

00005ede <beam1_proc+0x1e>:
    5ede:	3500b4        	ae_sraiq56	aeq0, aeq0, 2
    5ee1:	fc2440014f 	{ movi	a4, 1; ae_roundsq32asym	aeq0, aeq0 }
    5ee6:	1a0c           	movi.n	a10, 1
    5ee8:	270c           	movi.n	a7, 2
    5eea:	014470        	slli	a4, a4, 25
    5eed:	01aab0        	slli	a10, a10, 21
    5ef0:	727750        	salt	a7, a7, a5
    5ef3:	93a470        	movnez	a10, a4, a7
    5ef6:	368b04        	ae_trunca32q48	a11, aeq0
    5ef9:	e87e41        	l32r	a4, f4 (4630 <airoha_divide>)
    5efc:	0004e0        	callx8	a4
    5eff:	4178           	l32i.n	a7, a1, 16
    5f01:	71a9           	s32i.n	a10, a1, 28
    5f03:	ffffc6        	j	5f06 <beam1_proc+0x46>

00005f06 <beam1_proc+0x46>:
    5f06:	14c342        	addi	a4, a3, 20
    5f09:	24c3a2        	addi	a10, a3, 36
    5f0c:	fec692        	addi	a9, a6, -2
    5f0f:	10c362        	addi	a6, a3, 16
    5f12:	3388           	l32i.n	a8, a3, 12
    5f14:	13c8           	l32i.n	a12, a3, 4
    5f16:	4139           	s32i.n	a3, a1, 16
    5f18:	6e9232        	l16si	a3, a2, 220
    5f1b:	0d0c           	movi.n	a13, 0
    5f1d:	6169           	s32i.n	a6, a1, 24
    5f1f:	400604        	ae_lp16f.i	aep0, a6, 0
    5f22:	5168           	l32i.n	a6, a1, 20
    5f24:	627d70        	saltu	a7, a13, a7
    5f27:	723d30        	salt	a3, a13, a3
    5f2a:	103730        	and	a3, a7, a3
    5f2d:	030470        	rsr.br	a7
    5f30:	90c6c0        	addx2	a12, a6, a12
    5f33:	0a0370        	depbits	a7, a3, 0, 1
    5f36:	130470        	wsr.br	a7
    5f39:	0e0c           	movi.n	a14, 0
    5f3b:	1f0c           	movi.n	a15, 1
    5f3d:	fecc62        	addi	a6, a12, -2
    5f40:	360b34        	ae_cvtq48a32s	aeq0, a11
    5f43:	f3f640        	wur.ae_cbegin0	a4
    5f46:	fd0c16ef0f 	{ movt	a14, a15, b0; ae_cvtq48p24s.l	aeq1, aep0 }
    5f4b:	2b0c           	movi.n	a11, 2
    5f4d:	870c           	movi.n	a7, 8
    5f4f:	f3f7a0        	wur.ae_cend0	a10
    5f52:	0a4d           	mov.n	a4, a10
    5f54:	20f660        	or	a15, a6, a6
    5f57:	409914        	ae_lp16f.iu	aep1, a9, 2
    5f5a:	fc24e298bf 	{ ae_sp16f.l.c	aep1, a8, a11; nop }
    5f5f:	400404        	ae_lp16f.i	aep0, a4, 0
    5f62:	20c440        	or	a12, a4, a4
    5f65:	098776        	loop	a7, 5f72 <beam1_proc+0xb2>
    5f68:	fc24d918bf 	{ ae_lp16f.c	aep1, a8, a11; nop }
    5f6d:	ee21ec8c1f 	{ ae_lp16f.iu	aep0, a12, 2; ae_mulsfp24s.ll	aeq1, aep0, aep1 }

00005f72 <beam1_proc+0xb2>:
    5f72:	f86bec8f1f 	{ ae_lp16f.iu	aep0, a15, 2; ae_roundsp16q48asym	aep1, aeq1 }
    5f77:	0914a4        	ae_movp48	aep2, aep1
    5f7a:	1eac           	beqz.n	a14, 5f9f <beam1_proc+0xdf>
    5f7c:	402204        	ae_lp16f.i	aep2, a2, 0
    5f7f:	eb29bd227f 	{ ae_lq32f.i	aeq1, a2, 28; ae_mulfp24s.ll	aeq2, aep2, aep1 }
    5f84:	407224        	ae_lp16f.i	aep7, a2, 4
    5f87:	35a094        	ae_slliq56	aeq2, aeq2, 2
    5f8a:	f59fbd626f 	{ ae_lq32f.i	aeq3, a2, 24; ae_mulafq32sp16s.l	aeq2, aeq1, aep7 }
    5f8f:	403214        	ae_lp16f.i	aep3, a2, 2
    5f92:	f78fb9b27f 	{ ae_sq32f.i	aeq3, a2, 28; ae_mulafq32sp16s.l	aeq2, aeq3, aep3 }
    5f97:	06a934        	ae_roundsq32asym	aeq2, aeq2
    5f9a:	f973b9a26f 	{ ae_sq32f.i	aeq2, a2, 24; ae_roundsp16q48asym	aep2, aeq2 }

00005f9f <beam1_proc+0xdf>:
    5f9f:	d4e672a61f 	{ ae_sp16f.l.iu	aep2, a6, 2; ae_mulfq32sp16s.l	aeq1, aeq0, aep1 }
    5fa4:	f0cc42        	addi	a4, a12, -16
    5fa7:	2815a6        	blti	a5, 1, 5fd3 <beam1_proc+0x113>
    5faa:	0430d0        	extui	a3, a13, 0, 1
    5fad:	23ec           	bnez.n	a3, 5fd3 <beam1_proc+0x113>
    5faf:	7138           	l32i.n	a3, a1, 28
    5fb1:	fc24fe934f 	{ ae_slaasq56s	aeq1, aeq1, a3; nop }
    5fb6:	f86fec2c8f 	{ ae_lp16f.i	aep2, a12, -16; ae_roundsp24q48asym	aep1, aeq1 }
    5fbb:	f03d           	nop.n
    5fbd:	0f8776        	loop	a7, 5fd0 <beam1_proc+0x110>
    5fc0:	fd4c1978bf 	{ ae_lp16f.c	aep7, a8, a11; ae_cvtq48p24s.l	aeq1, aep2 }
    5fc5:	e2e7eca41f 	{ ae_lp16f.iu	aep2, a4, 2; ae_mulafp24s.ll	aeq1, aep1, aep7 }
    5fca:	1b9f34        	ae_roundsp16q48asym	aep3, aeq1
    5fcd:	4234f4        	ae_sp16f.l.i	aep3, a4, -2

00005fd0 <beam1_proc+0x110>:
    5fd0:	f0c442        	addi	a4, a4, -16

00005fd3 <beam1_proc+0x113>:
    5fd3:	fd0c001dde 	{ addi	a13, a13, 1; ae_cvtq48p24s.l	aeq1, aep0 }
    5fd8:	029d26        	beqi	a13, 10, 5fde <beam1_proc+0x11e>
    5fdb:	ffde06        	j	5f57 <beam1_proc+0x97>
    5fde:	4158           	l32i.n	a5, a1, 16
    5fe0:	6138           	l32i.n	a3, a1, 24
    5fe2:	440c           	movi.n	a4, 4
    5fe4:	3589           	s32i.n	a8, a5, 12
    5fe6:	fc24b2030f 	{ ae_sp16f.l.i	aep0, a3, 0; ae_zeroq56	aeq0 }
    5feb:	440a04        	ae_lp16x2f.i	aep0, a10, 0
    5fee:	0a3d           	mov.n	a3, a10
    5ff0:	048476        	loop	a4, 5ff8 <beam1_proc+0x138>
    5ff3:	e2002d831f 	{ ae_lp16x2f.iu	aep0, a3, 4; ae_mulaafp24s.hh.ll	aeq0, aep0, aep0 }

00005ff8 <beam1_proc+0x138>:
    5ff8:	f863a0622e 	{ l16si	a2, a2, 12; ae_roundsp16q48asym	aep0, aeq0 }
    5ffd:	360324        	ae_trunca16p24s.l	a3, aep0
    6000:	095532        	s16i	a3, a5, 18
    6003:	faa042        	movi	a4, 250
    6006:	19a237        	bge	a2, a3, 6023 <beam1_proc+0x163>
    6009:	7ed422        	addmi	a2, a4, 0x7e00
    600c:	08a032        	movi	a3, 8
    600f:	300224        	ae_cvtp24a16x2.ll	aep0, a2, a2
    6012:	401a04        	ae_lp16f.i	aep1, a10, 0
    6015:	0a8376        	loop	a3, 6023 <beam1_proc+0x163>
    6018:	ea21ac9a1f 	{ ae_lp16f.iu	aep1, a10, 2; ae_mulfp24s.ll	aeq0, aep0, aep1 }
    601d:	1b8f24        	ae_roundsp16q48asym	aep2, aeq0
    6020:	422af4        	ae_sp16f.l.i	aep2, a10, -2

00006023 <beam1_proc+0x163>:
    6023:	f01d           	retw.n

00006025 <beam1_proc+0x165>:
    6025:	000000                                        ...

00006028 <beam2_proc>:
    6028:	00e136        	entry	a1, 112
    602b:	03cd           	mov.n	a12, a3
    602d:	fd24a1d39e 	{ l16si	a9, a3, 58; ae_zeroq56	aeq1 }
    6032:	f3a8           	l32i.n	a10, a3, 60
    6034:	078d           	mov.n	a8, a7
    6036:	117910        	slli	a7, a9, 15
    6039:	727a70        	salt	a7, a10, a7
    603c:	6cc3a2        	addi	a10, a3, 108
    603f:	05fd           	mov.n	a15, a5
    6041:	352b           	addi.n	a3, a5, 2
    6043:	cd7c           	movi.n	a13, -4
    6045:	eb7c           	movi.n	a11, -2
    6047:	93fd30        	movnez	a15, a13, a3
    604a:	f3f6a0        	wur.ae_cbegin0	a10
    604d:	8d7c           	movi.n	a13, -8
    604f:	aa7c           	movi.n	a10, -6
    6051:	fec692        	addi	a9, a6, -2
    6054:	93ad30        	movnez	a10, a13, a3
    6057:	be7c           	movi.n	a14, -5
    6059:	72b5b0        	salt	a11, a5, a11
    605c:	839670        	moveqz	a9, a6, a7
    605f:	83eab0        	moveqz	a14, a10, a11
    6062:	1d2132        	l32i	a3, a1, 116
    6065:	80d872        	addmi	a7, a8, 0xffff8000
    6068:	0ca8           	l32i.n	a10, a12, 0
    606a:	303884        	ae_cvtp24a16x2.ll	aep3, a8, a8
    606d:	d199           	s32i.n	a9, a1, 52
    606f:	190c           	movi.n	a9, 1
    6071:	871b           	addi.n	a8, a7, 1
    6073:	f1e9           	s32i.n	a14, a1, 60
    6075:	0e0c           	movi.n	a14, 0
    6077:	93feb0        	movnez	a15, a14, a11
    607a:	626870        	saltu	a6, a8, a7
    607d:	729590        	salt	a9, a5, a9
    6080:	1cb8           	l32i.n	a11, a12, 4
    6082:	10d960        	and	a13, a9, a6
    6085:	e1f9           	s32i.n	a15, a1, 56
    6087:	3accf2        	addi	a15, a12, 58
    608a:	9063a0        	addx2	a6, a3, a10
    608d:	81f9           	s32i.n	a15, a1, 32
    608f:	404ff4        	ae_lp16f.i	aep4, a15, -2
    6092:	fec6f2        	addi	a15, a6, -2
    6095:	dc68           	l32i.n	a6, a12, 52
    6097:	904340        	addx2	a4, a3, a4
    609a:	7ccc72        	addi	a7, a12, 124
    609d:	9033b0        	addx2	a3, a3, a11
    60a0:	1c21e2        	l32i	a14, a1, 112
    60a3:	40cc82        	addi	a8, a12, 64
    60a6:	71e9           	s32i.n	a14, a1, 28
    60a8:	fec492        	addi	a9, a4, -2
    60ab:	fec3e2        	addi	a14, a3, -2
    60ae:	9159           	s32i.n	a5, a1, 36
    60b0:	f3f770        	wur.ae_cend0	a7
    60b3:	230c           	movi.n	a3, 2
    60b5:	040c           	movi.n	a4, 0
    60b7:	b189           	s32i.n	a8, a1, 44
    60b9:	a179           	s32i.n	a7, a1, 40
    60bb:	1061c2        	s32i	a12, a1, 64
    60be:	563114        	ae_sp24x2s.i	aep3, a1, 8
    60c1:	c1d9           	s32i.n	a13, a1, 48
    60c3:	0004c6        	j	60da <beam2_proc+0xb2>

000060c6 <beam2_proc+0x9e>:
    60c6:	408914        	ae_lp16f.iu	aep0, a9, 2
    60c9:	fa62b2cf1f 	{ ae_sp16f.l.iu	aep4, a15, 2; ae_movp48	aep4, aep0 }
    60ce:	075d           	mov.n	a5, a7
    60d0:	441b           	addi.n	a4, a4, 1
    60d2:	057d           	mov.n	a7, a5
    60d4:	029466        	bnei	a4, 10, 60da <beam2_proc+0xb2>
    60d7:	0042c6        	j	61e6 <beam2_proc+0x1be>
    60da:	6e1252        	l16ui	a5, a2, 220
    60dd:	408e14        	ae_lp16f.iu	aep0, a14, 2
    60e0:	fc24a2863f 	{ ae_sp16f.l.c	aep0, a6, a3; ae_zeroq56	aeq0 }
    60e5:	fdd516        	beqz	a5, 60c6 <beam2_proc+0x9e>
    60e8:	850c           	movi.n	a5, 8
    60ea:	400704        	ae_lp16f.i	aep0, a7, 0
    60ed:	098576        	loop	a5, 60fa <beam2_proc+0xd2>
    60f0:	fc24d9163f 	{ ae_lp16f.c	aep1, a6, a3; nop }
    60f5:	e223ac871f 	{ ae_lp16f.iu	aep0, a7, 2; ae_mulafp24s.ll	aeq0, aep0, aep1 }

000060fa <beam2_proc+0xd2>:
    60fa:	f867bf456f 	{ ae_trunca16p24s.l	a5, aep4; ae_roundsp24q48asym	aep0, aeq0 }
    60ff:	0ac484        	ae_cvtq48p24s.l	aeq3, aep4
    6102:	058bc4        	ae_mulssfp24s.hh.ll	aeq3, aep0, aep3
    6105:	605150        	abs	a5, a5
    6108:	1bbf04        	ae_roundsp16q48asym	aep0, aeq3
    610b:	360824        	ae_trunca16p24s.l	a8, aep0
    610e:	608180        	abs	a8, a8
    6111:	02a587        	bge	a5, a8, 6117 <beam2_proc+0xef>
    6114:	094484        	ae_movp48	aep0, aep4

00006117 <beam2_proc+0xef>:
    6117:	40c914        	ae_lp16f.iu	aep4, a9, 2
    611a:	f0c752        	addi	a5, a7, -16
    611d:	428f14        	ae_sp16f.l.iu	aep0, a15, 2
    6120:	facd16        	beqz	a13, 60d0 <beam2_proc+0xa8>
    6123:	025426        	beqi	a4, 5, 6129 <beam2_proc+0x101>
    6126:	08e456        	bnez	a4, 61b8 <beam2_proc+0x190>
    6129:	fc2499063f 	{ ae_lp16f.c	aep0, a6, a3; ae_zeroq56	aeq0 }
    612e:	fc24d9063f 	{ ae_lp16f.c	aep0, a6, a3; nop }
    6133:	fc24d9063f 	{ ae_lp16f.c	aep0, a6, a3; nop }
    6138:	fc24d9063f 	{ ae_lp16f.c	aep0, a6, a3; nop }
    613d:	480c           	movi.n	a8, 4
    613f:	1161f2        	s32i	a15, a1, 68
    6142:	e203b7412f 	{ ae_sp24x2s.i	aep4, a1, 16; ae_mulafp24s.ll	aeq0, aep0, aep0 }
    6147:	1261e2        	s32i	a14, a1, 72
    614a:	136192        	s32i	a9, a1, 76
    614d:	078876        	loop	a8, 6158 <beam2_proc+0x130>
    6150:	fc24d9063f 	{ ae_lp16f.c	aep0, a6, a3; nop }
    6155:	010034        	ae_mulafp24s.ll	aeq0, aep0, aep0

00006158 <beam2_proc+0x130>:
    6158:	b188           	l32i.n	a8, a1, 44
    615a:	3500b4        	ae_sraiq56	aeq0, aeq0, 2
    615d:	190c           	movi.n	a9, 1
    615f:	604804        	ae_lq32f.i	aeq1, a8, 0
    6162:	f96a30f1ae 	{ l32i	a10, a1, 60; ae_addq56	aeq1, aeq1, aeq0 }
    6167:	fc244000df 	{ movi	a13, 0; ae_roundsq32asym	aeq0, aeq0 }
    616c:	fd2c79880f 	{ ae_sq32f.i	aeq0, a8, 0; ae_roundsq32asym	aeq1, aeq1 }
    6171:	689282        	l16si	a8, a2, 208
    6174:	369b04        	ae_trunca32q48	a11, aeq1
    6177:	119940        	slli	a9, a9, 12
    617a:	728d80        	salt	a8, a13, a8
    617d:	e1d8           	l32i.n	a13, a1, 56
    617f:	116cb2        	s32i	a11, a12, 68
    6182:	729b90        	salt	a9, a11, a9
    6185:	cc7c           	movi.n	a12, -4
    6187:	d1e8           	l32i.n	a14, a1, 52
    6189:	93ac90        	movnez	a10, a12, a9
    618c:	83ad80        	moveqz	a10, a13, a8
    618f:	808ea0        	add	a8, a14, a10
    6192:	401800        	ssl	a8
    6195:	01a0f2        	movi	a15, 1
    6198:	e7d781        	l32r	a8, f4 (4630 <airoha_divide>)
    619b:	a1af00        	sll	a10, a15
    619e:	0008e0        	callx8	a8
    61a1:	fd24b101ce 	{ l32i	a12, a1, 64; ae_zeroq56	aeq1 }
    61a6:	0c21d2        	l32i	a13, a1, 48
    61a9:	132192        	l32i	a9, a1, 76
    61ac:	1221e2        	l32i	a14, a1, 72
    61af:	1121f2        	l32i	a15, a1, 68
    61b2:	543114        	ae_lp24x2.i	aep3, a1, 8
    61b5:	544124        	ae_lp24x2.i	aep4, a1, 16

000061b8 <beam2_proc+0x190>:
    61b8:	400f04        	ae_lp16f.i	aep0, a15, 0
    61bb:	360b34        	ae_cvtq48a32s	aeq0, a11
    61be:	d4e200088f 	{ movi	a8, 8; ae_mulfq32sp16s.l	aeq0, aeq0, aep0 }
    61c3:	fc24fe8a0f 	{ ae_slaasq56s	aeq0, aeq0, a10; nop }
    61c8:	f867ac178f 	{ ae_lp16f.i	aep1, a7, -16; ae_roundsp24q48asym	aep0, aeq0 }
    61cd:	0f8876        	loop	a8, 61e0 <beam2_proc+0x1b8>
    61d0:	fc2c19763f 	{ ae_lp16f.c	aep7, a6, a3; ae_cvtq48p24s.l	aeq0, aep1 }
    61d5:	e2e3ac951f 	{ ae_lp16f.iu	aep1, a5, 2; ae_mulafp24s.ll	aeq0, aep0, aep7 }
    61da:	1b8f24        	ae_roundsp16q48asym	aep2, aeq0
    61dd:	4225f4        	ae_sp16f.l.i	aep2, a5, -2

000061e0 <beam2_proc+0x1b8>:
    61e0:	f0c552        	addi	a5, a5, -16
    61e3:	ffba46        	j	60d0 <beam2_proc+0xa8>

000061e6 <beam2_proc+0x1be>:
    61e6:	dc69           	s32i.n	a6, a12, 52
    61e8:	a168           	l32i.n	a6, a1, 40
    61ea:	8158           	l32i.n	a5, a1, 32
    61ec:	430c           	movi.n	a3, 4
    61ee:	440604        	ae_lp16x2f.i	aep0, a6, 0
    61f1:	064d           	mov.n	a4, a6
    61f3:	4245f4        	ae_sp16f.l.i	aep4, a5, -2
    61f6:	048376        	loop	a3, 61fe <beam2_proc+0x1d6>
    61f9:	e2006d841f 	{ ae_lp16x2f.iu	aep0, a4, 4; ae_mulaafp24s.hh.ll	aeq1, aep0, aep0 }

000061fe <beam2_proc+0x1d6>:
    61fe:	f86bb0914e 	{ l32i	a4, a1, 36; ae_roundsp16q48asym	aep0, aeq1 }
    6203:	d37c           	movi.n	a3, -3
    6205:	420504        	ae_sp16f.l.i	aep0, a5, 0
    6208:	712347        	blt	a3, a4, 627d <beam2_proc+0x255>
    620b:	681222        	l16ui	a2, a2, 208
    620e:	06b256        	bnez	a2, 627d <beam2_proc+0x255>
    6211:	279c22        	l16si	a2, a12, 78
    6214:	1d9c42        	l16si	a4, a12, 58
    6217:	122c52        	l32i	a5, a12, 72
    621a:	221b           	addi.n	a2, a2, 1
    621c:	823d           	ae_sext16	a3, a2
    621e:	275c22        	s16i	a2, a12, 78
    6221:	53c3a6        	blti	a3, 32, 6278 <beam2_proc+0x250>
    6224:	122c32        	l32i	a3, a12, 72
    6227:	289c52        	l16si	a5, a12, 80
    622a:	54cca2        	addi	a10, a12, 84
    622d:	10c332        	addi	a3, a3, 16
    6230:	020c           	movi.n	a2, 0
    6232:	9055a0        	addx2	a5, a5, a10
    6235:	413530        	srli	a3, a3, 5
    6238:	1d9c42        	l16si	a4, a12, 58
    623b:	275c22        	s16i	a2, a12, 78
    623e:	653d           	ae_s16i.n	a3, a5, 0
    6240:	289c32        	l16si	a3, a12, 80
    6243:	be0c           	movi.n	a14, 11
    6245:	126c42        	s32i	a4, a12, 72
    6248:	331b           	addi.n	a3, a3, 1
    624a:	83fd           	ae_sext16	a15, a3
    624c:	724ef0        	salt	a4, a14, a15
    624f:	933240        	movnez	a3, a2, a4
    6252:	e7fe21        	l32r	a2, 24c (3bd8 <take_min_four>)
    6255:	cb0c           	movi.n	a11, 12
    6257:	285c32        	s16i	a3, a12, 80
    625a:	0c4d           	mov.n	a4, a12
    625c:	0002e0        	callx8	a2
    625f:	e7a321        	l32r	a2, ec (7800 <immediate_num>)
    6262:	f438           	l32i.n	a3, a4, 60
    6264:	0abd           	mov.n	a11, a10
    6266:	20c2c2        	addi	a12, a2, 32
    6269:	e7b621        	l32r	a2, 144 (5b0 <siir_safr>)
    626c:	03ad           	mov.n	a10, a3
    626e:	0002e0        	callx8	a2
    6271:	04cd           	mov.n	a12, a4
    6273:	f4a9           	s32i.n	a10, a4, 60
    6275:	000106        	j	627d <beam2_proc+0x255>

00006278 <beam2_proc+0x250>:
    6278:	254a           	add.n	a2, a5, a4
    627a:	126c22        	s32i	a2, a12, 72
    627d:	112c22        	l32i	a2, a12, 68
    6280:	130c           	movi.n	a3, 1
    6282:	10d332        	addmi	a3, a3, 0x1000
    6285:	722237        	blt	a2, a3, 62fb <beam2_proc+0x2d3>
    6288:	7128           	l32i.n	a2, a1, 28
    628a:	d33c           	movi.n	a3, 61
    628c:	d7a042        	movi	a4, 215
    628f:	1522e6        	bgei	a2, 2, 62a8 <beam2_proc+0x280>
    6292:	66a022        	movi	a2, 102
    6295:	b33c           	movi.n	a3, 59
    6297:	46d222        	addmi	a2, a2, 0x4600
    629a:	113380        	slli	a3, a3, 8
    629d:	ff3c           	movi.n	a15, 63
    629f:	300324        	ae_cvtp24a16x2.ll	aep0, a3, a2
    62a2:	112f70        	slli	a2, a15, 9
    62a5:	0002c6        	j	62b4 <beam2_proc+0x28c>

000062a8 <beam2_proc+0x280>:
    62a8:	269c22        	l16si	a2, a12, 76
    62ab:	4ad332        	addmi	a3, a3, 0x4a00
    62ae:	43d442        	addmi	a4, a4, 0x4300
    62b1:	300434        	ae_cvtp24a16x2.ll	aep0, a4, a3
    62b4:	1d9c32        	l16si	a3, a12, 58
    62b7:	fc48           	l32i.n	a4, a12, 60
    62b9:	364434        	ae_cvtq48a32s	aeq1, a4
    62bc:	113310        	slli	a3, a3, 15
    62bf:	d5633ec33f 	{ ae_cvtq48a32s	aeq0, a3; ae_mulfq32sp16s.h	aeq2, aeq1, aep0 }
    62c4:	d4e34000ef 	{ movi	a14, 0; ae_mulfq32sp16s.l	aeq1, aeq1, aep0 }
    62c9:	d6c00001ff 	{ movi	a15, 1; ae_ltq56s	b0, aeq2, aeq0 }
    62ce:	d54840003f 	{ movi	a3, 0; ae_leq56s	b1, aeq0, aeq1 }
    62d3:	140c           	movi.n	a4, 1
    62d5:	d3ef00        	movt	a14, a15, b0
    62d8:	fe9c           	beqz.n	a14, 62fb <beam2_proc+0x2d3>
    62da:	d33410        	movt	a3, a4, b1
    62dd:	438c           	beqz.n	a3, 62e5 <beam2_proc+0x2bd>
    62df:	aea022        	movi	a2, 174
    62e2:	7fd222        	addmi	a2, a2, 0x7f00
    62e5:	8f0c           	movi.n	a15, 8
    62e7:	300224        	ae_cvtp24a16x2.ll	aep0, a2, a2
    62ea:	401604        	ae_lp16f.i	aep1, a6, 0
    62ed:	0a8f76        	loop	a15, 62fb <beam2_proc+0x2d3>
    62f0:	ea21ac961f 	{ ae_lp16f.iu	aep1, a6, 2; ae_mulfp24s.ll	aeq0, aep0, aep1 }
    62f5:	1b8f24        	ae_roundsp16q48asym	aep2, aeq0
    62f8:	4226f4        	ae_sp16f.l.i	aep2, a6, -2

000062fb <beam2_proc+0x2d3>:
    62fb:	f01d           	retw.n

000062fd <beam2_proc+0x2d5>:
    62fd:	000000                                        ...

00006300 <beam1_mid_band>:
    6300:	006136        	entry	a1, 48
    6303:	fc24a6b23e 	{ l16si	a3, a2, 214; ae_zeroq56	aeq0 }
    6308:	3a13a6        	blti	a3, 1, 6346 <beam1_mid_band+0x46>
    630b:	d32242        	l32i	a4, a2, 0x34c
    630e:	a50c           	movi.n	a5, 10
    6310:	440404        	ae_lp16x2f.i	aep0, a4, 0
    6313:	f03d           	nop.n
    6315:	048576        	loop	a5, 631d <beam1_mid_band+0x1d>
    6318:	e2002d841f 	{ ae_lp16x2f.iu	aep0, a4, 4; ae_mulaafp24s.hh.ll	aeq0, aep0, aep0 }

0000631d <beam1_mid_band+0x1d>:
    631d:	3500b4        	ae_sraiq56	aeq0, aeq0, 2
    6320:	fc2440014f 	{ movi	a4, 1; ae_roundsq32asym	aeq0, aeq0 }
    6325:	368704        	ae_trunca32q48	a7, aeq0
    6328:	150c           	movi.n	a5, 1
    632a:	1a0c           	movi.n	a10, 1
    632c:	260c           	movi.n	a6, 2
    632e:	114450        	slli	a4, a4, 11
    6331:	53b740        	max	a11, a7, a4
    6334:	015570        	slli	a5, a5, 25
    6337:	01aab0        	slli	a10, a10, 21
    633a:	726630        	salt	a6, a6, a3
    633d:	e76d41        	l32r	a4, f4 (4630 <airoha_divide>)
    6340:	93a560        	movnez	a10, a5, a6
    6343:	0004e0        	callx8	a4

00006346 <beam1_mid_band+0x46>:
    6346:	d32252        	l32i	a5, a2, 0x34c
    6349:	d222e2        	l32i	a14, a2, 0x348
    634c:	54a3d2        	movi	a13, 0x354
    634f:	5ca342        	movi	a4, 0x35c
    6352:	7ca372        	movi	a7, 0x37c
    6355:	d42262        	l32i	a6, a2, 0x350
    6358:	c24a           	add.n	a12, a2, a4
    635a:	427a           	add.n	a4, a2, a7
    635c:	458d24        	ae_lp16x2f.xu	aep0, a13, a2
    635f:	360b34        	ae_cvtq48a32s	aeq0, a11
    6362:	31d9           	s32i.n	a13, a1, 12
    6364:	f3f6c0        	wur.ae_cbegin0	a12
    6367:	fcc5c2        	addi	a12, a5, -4
    636a:	fe040fcede 	{ addi	a13, a14, -4; ae_cvtq48p24s.h	aeq2, aep0 }
    636f:	080c           	movi.n	a8, 0
    6371:	490c           	movi.n	a9, 4
    6373:	8b0c           	movi.n	a11, 8
    6375:	f3f740        	wur.ae_cend0	a4
    6378:	045d           	mov.n	a5, a4
    637a:	fecee2        	addi	a14, a14, -2
    637d:	fd0c2d9c1f 	{ ae_lp16x2f.iu	aep1, a12, 4; ae_cvtq48p24s.l	aeq1, aep0 }
    6382:	fc24e4169f 	{ ae_sp16x2f.c	aep1, a6, a9; nop }
    6387:	440504        	ae_lp16x2f.i	aep0, a5, 0
    638a:	20f550        	or	a15, a5, a5
    638d:	0c8b76        	loop	a11, 639d <beam1_mid_band+0x9d>
    6390:	fc24da969f 	{ ae_lp16x2f.c	aep1, a6, a9; nop }
    6395:	0581c4        	ae_mulssfp24s.hh.ll	aeq2, aep0, aep1
    6398:	ee206d8f1f 	{ ae_lp16x2f.iu	aep0, a15, 4; ae_mulsafp24s.hl.lh	aeq1, aep0, aep1 }

0000639d <beam1_mid_band+0x9d>:
    639d:	f873ed8d1f 	{ ae_lp16x2f.iu	aep0, a13, 4; ae_roundsp16q48asym	aep1, aeq2 }
    63a2:	f96bb29e1f 	{ ae_sp16f.l.iu	aep1, a14, 2; ae_roundsp16q48asym	aep2, aeq1 }
    63a7:	d5ea0e0f5e 	{ addi	a5, a15, -32; ae_mulfq32sp16s.l	aeq2, aeq0, aep2 }
    63ac:	d4e672ae1f 	{ ae_sp16f.l.iu	aep2, a14, 2; ae_mulfq32sp16s.l	aeq1, aeq0, aep1 }
    63b1:	3a13a6        	blti	a3, 1, 63ef <beam1_mid_band+0xef>
    63b4:	047080        	extui	a7, a8, 0, 1
    63b7:	47fc           	bnez.n	a7, 63ef <beam1_mid_band+0xef>
    63b9:	442f84        	ae_lp16x2f.i	aep2, a15, -32
    63bc:	fc24feaa8f 	{ ae_slaasq56s	aeq2, aeq2, a10; nop }
    63c1:	f977fe9a4f 	{ ae_slaasq56s	aeq1, aeq1, a10; ae_roundsp24q48asym	aep3, aeq2 }
    63c6:	fb6fcfe5fe 	{ addi	a15, a5, -2; ae_roundsp24q48asym	aep7, aeq1 }
    63cb:	1afb14        	ae_selp24.hl	aep1, aep7, aep3
    63ce:	1a8b76        	loop	a11, 63ec <beam1_mid_band+0xec>
    63d1:	fd441ae69f 	{ ae_lp16x2f.c	aep6, a6, a9; ae_cvtq48p24s.h	aeq1, aep2 }
    63d6:	fe4c2da51f 	{ ae_lp16x2f.iu	aep2, a5, 4; ae_cvtq48p24s.l	aeq2, aep2 }
    63db:	051e04        	ae_mulaafp24s.hh.ll	aeq1, aep1, aep6
    63de:	059654        	ae_mulasfp24s.hl.lh	aeq2, aep1, aep6
    63e1:	1b9f34        	ae_roundsp16q48asym	aep3, aeq1
    63e4:	fb73f2bf1f 	{ ae_sp16f.l.iu	aep3, a15, 2; ae_roundsp16q48asym	aep7, aeq2 }
    63e9:	42ff14        	ae_sp16f.l.iu	aep7, a15, 2
    63ec:	e0c552        	addi	a5, a5, -32

000063ef <beam1_mid_band+0xef>:
    63ef:	fe0400188e 	{ addi	a8, a8, 1; ae_cvtq48p24s.h	aeq2, aep0 }
    63f4:	859866        	bnei	a8, 10, 637d <beam1_mid_band+0x7d>
    63f7:	31f8           	l32i.n	a15, a1, 12
    63f9:	830c           	movi.n	a3, 8
    63fb:	d46262        	s32i	a6, a2, 0x350
    63fe:	fc24b30f0f 	{ ae_sp16x2f.i	aep0, a15, 0; ae_zeroq56	aeq0 }
    6403:	440504        	ae_lp16x2f.i	aep0, a5, 0
    6406:	048376        	loop	a3, 640e <beam1_mid_band+0x10e>
    6409:	e2002d851f 	{ ae_lp16x2f.iu	aep0, a5, 4; ae_mulaafp24s.hh.ll	aeq0, aep0, aep0 }

0000640e <beam1_mid_band+0x10e>:
    640e:	3500b4        	ae_sraiq56	aeq0, aeq0, 2
    6411:	f863a0625e 	{ l16si	a5, a2, 12; ae_roundsp16q48asym	aep0, aeq0 }
    6416:	360624        	ae_trunca16p24s.l	a6, aep0
    6419:	03d232        	addmi	a3, a2, 0x300
    641c:	2c5362        	s16i	a6, a3, 88
    641f:	20a567        	bge	a5, a6, 6443 <beam1_mid_band+0x143>
    6422:	faa032        	movi	a3, 250
    6425:	7ed332        	addmi	a3, a3, 0x7e00
    6428:	7ca362        	movi	a6, 0x37c
    642b:	051c           	movi.n	a5, 16
    642d:	411264        	ae_lp16f.x	aep1, a2, a6
    6430:	300334        	ae_cvtp24a16x2.ll	aep0, a3, a3
    6433:	f03d           	nop.n
    6435:	0a8576        	loop	a5, 6443 <beam1_mid_band+0x143>
    6438:	ea21ac941f 	{ ae_lp16f.iu	aep1, a4, 2; ae_mulfp24s.ll	aeq0, aep0, aep1 }
    643d:	1b8f24        	ae_roundsp16q48asym	aep2, aeq0
    6440:	4224f4        	ae_sp16f.l.i	aep2, a4, -2

00006443 <beam1_mid_band+0x143>:
    6443:	f01d           	retw.n

00006445 <beam1_mid_band+0x145>:
    6445:	000000                                        ...

00006448 <beam2_mid_band>:
    6448:	00c136        	entry	a1, 96
    644b:	03d2f2        	addmi	a15, a2, 0x300
    644e:	02cd           	mov.n	a12, a2
    6450:	529f72        	l16si	a7, a15, 164
    6453:	d4a342        	movi	a4, 0x3d4
    6456:	ea2282        	l32i	a8, a2, 0x3a8
    6459:	6b92b2        	l16si	a11, a2, 214
    645c:	a0a3a2        	movi	a10, 0x3a0
    645f:	424a           	add.n	a4, a2, a4
    6461:	f4a392        	movi	a9, 0x3f4
    6464:	e72262        	l32i	a6, a2, 0x39c
    6467:	d22252        	l32i	a5, a2, 0x348
    646a:	d32232        	l32i	a3, a2, 0x34c
    646d:	d122d2        	l32i	a13, a2, 0x344
    6470:	117710        	slli	a7, a7, 15
    6473:	f3f640        	wur.ae_cbegin0	a4
    6476:	429a           	add.n	a4, a2, a9
    6478:	45ba24        	ae_lp16x2f.xu	aep3, a10, a2
    647b:	51f9           	s32i.n	a15, a1, 20
    647d:	621c           	movi.n	a2, 22
    647f:	8e1c           	movi.n	a14, 24
    6481:	91a9           	s32i.n	a10, a1, 36
    6483:	727870        	salt	a7, a8, a7
    6486:	190c           	movi.n	a9, 1
    6488:	f3f740        	wur.ae_cend0	a4
    648b:	071be6        	bgei	a11, 1, 6496 <beam2_mid_band+0x4e>
    648e:	6e1c82        	l16ui	a8, a12, 220
    6491:	190c           	movi.n	a9, 1
    6493:	629890        	saltu	a9, a8, a9

00006496 <beam2_mid_band+0x4e>:
    6496:	fe248fcd8e 	{ addi	a8, a13, -4; ae_zeroq56	aeq2 }
    649b:	93e270        	movnez	a14, a2, a7
    649e:	fcc372        	addi	a7, a3, -4
    64a1:	6189           	s32i.n	a8, a1, 24
    64a3:	3b2b           	addi.n	a3, a11, 2
    64a5:	8a7c           	movi.n	a10, -8
    64a7:	a87c           	movi.n	a8, -6
    64a9:	938a30        	movnez	a8, a10, a3
    64ac:	0b2d           	mov.n	a2, a11
    64ae:	ca7c           	movi.n	a10, -4
    64b0:	932a30        	movnez	a2, a10, a3
    64b3:	ea7c           	movi.n	a10, -2
    64b5:	723ba0        	salt	a3, a11, a10
    64b8:	bd7c           	movi.n	a13, -5
    64ba:	83d830        	moveqz	a13, a8, a3
    64bd:	b1d9           	s32i.n	a13, a1, 44
    64bf:	0d0c           	movi.n	a13, 0
    64c1:	932d30        	movnez	a2, a13, a3
    64c4:	61d8           	l32i.n	a13, a1, 24
    64c6:	fcc5f2        	addi	a15, a5, -4
    64c9:	81b9           	s32i.n	a11, a1, 32
    64cb:	f86e94a12f 	{ s32i.n	a2, a1, 40; ae_movp48	aep0, aep3 }
    64d0:	450c           	movi.n	a5, 4
    64d2:	020c           	movi.n	a2, 0
    64d4:	7149           	s32i.n	a4, a1, 28
    64d6:	d1c9           	s32i.n	a12, a1, 52
    64d8:	c199           	s32i.n	a9, a1, 48
    64da:	003ac6        	j	65c9 <beam2_mid_band+0x181>

000064dd <beam2_mid_band+0x95>:
    64dd:	00000000                                ....

000064e1 <beam2_mid_band+0x99>:
    64e1:	080c           	movi.n	a8, 0
    64e3:	f8e28e034e 	{ addi	a4, a3, -32; ae_roundsp16asym	aep0, aep0 }
    64e8:	468d14        	ae_sp16x2f.iu	aep0, a13, 4
    64eb:	0d2956        	bnez	a9, 65c1 <beam2_mid_band+0x179>
    64ee:	025226        	beqi	a2, 5, 64f4 <beam2_mid_band+0xac>
    64f1:	089256        	bnez	a2, 657e <beam2_mid_band+0x136>
    64f4:	fc249a865f 	{ ae_lp16x2f.c	aep0, a6, a5; ae_zeroq56	aeq0 }
    64f9:	fc24da865f 	{ ae_lp16x2f.c	aep0, a6, a5; nop }
    64fe:	fc24da865f 	{ ae_lp16x2f.c	aep0, a6, a5; nop }
    6503:	fc24da865f 	{ ae_lp16x2f.c	aep0, a6, a5; nop }
    6508:	f1f9           	s32i.n	a15, a1, 60
    650a:	4f0c           	movi.n	a15, 4
    650c:	e20037311f 	{ ae_sp24x2s.i	aep3, a1, 8; ae_mulaafp24s.hh.ll	aeq0, aep0, aep0 }
    6511:	61d9           	s32i.n	a13, a1, 24
    6513:	e179           	s32i.n	a7, a1, 56
    6515:	078f76        	loop	a15, 6520 <beam2_mid_band+0xd8>
    6518:	fc24da865f 	{ ae_lp16x2f.c	aep0, a6, a5; nop }
    651d:	050004        	ae_mulaafp24s.hh.ll	aeq0, aep0, aep0

00006520 <beam2_mid_band+0xd8>:
    6520:	aca372        	movi	a7, 0x3ac
    6523:	606c74        	ae_lq32f.x	aeq1, a12, a7
    6526:	3500b4        	ae_sraiq56	aeq0, aeq0, 2
    6529:	f96a268c9e 	{ l16si	a9, a12, 208; ae_addq56	aeq1, aeq1, aeq0 }
    652e:	fd2c4000af 	{ movi	a10, 0; ae_roundsq32asym	aeq1, aeq1 }
    6533:	fc247f1bef 	{ ae_trunca32q48	a11, aeq1; ae_roundsq32asym	aeq0, aeq0 }
    6538:	729a90        	salt	a9, a10, a9
    653b:	628c74        	ae_sq32f.x	aeq0, a12, a7
    653e:	ec6cb2        	s32i	a11, a12, 0x3b0
    6541:	1c0c           	movi.n	a12, 1
    6543:	b1a8           	l32i.n	a10, a1, 44
    6545:	117c50        	slli	a7, a12, 11
    6548:	72db70        	salt	a13, a11, a7
    654b:	cf7c           	movi.n	a15, -4
    654d:	93afd0        	movnez	a10, a15, a13
    6550:	a1d8           	l32i.n	a13, a1, 40
    6552:	1f0c           	movi.n	a15, 1
    6554:	0e7d           	mov.n	a7, a14
    6556:	83ad90        	moveqz	a10, a13, a9
    6559:	9eaa           	add.n	a9, a14, a10
    655b:	fec9a2        	addi	a10, a9, -2
    655e:	83a980        	moveqz	a10, a9, a8
    6561:	401a00        	ssl	a10
    6564:	e6e481        	l32r	a8, f4 (4630 <airoha_divide>)
    6567:	a1af00        	sll	a10, a15
    656a:	0008e0        	callx8	a8
    656d:	07ed           	mov.n	a14, a7
    656f:	d1c8           	l32i.n	a12, a1, 52
    6571:	c198           	l32i.n	a9, a1, 48
    6573:	f1f8           	l32i.n	a15, a1, 60
    6575:	e178           	l32i.n	a7, a1, 56
    6577:	61d8           	l32i.n	a13, a1, 24
    6579:	fe24b0311f 	{ ae_lp24x2.i	aep3, a1, 8; ae_zeroq56	aeq2 }

0000657e <beam2_mid_band+0x136>:
    657e:	440d04        	ae_lp16x2f.i	aep0, a13, 0
    6581:	360b34        	ae_cvtq48a32s	aeq0, a11
    6584:	d46240088f 	{ movi	a8, 8; ae_mulfq32sp16s.h	aeq1, aeq0, aep0 }
    6589:	d4e22d138f 	{ ae_lp16x2f.i	aep1, a3, -32; ae_mulfq32sp16s.l	aeq0, aeq0, aep0 }
    658e:	fc24fe9a4f 	{ ae_slaasq56s	aeq1, aeq1, a10; nop }
    6593:	fb6ffe8a0f 	{ ae_slaasq56s	aeq0, aeq0, a10; ae_roundsp24q48asym	aep7, aeq1 }
    6598:	f9678fe43e 	{ addi	a3, a4, -2; ae_roundsp24q48asym	aep2, aeq0 }
    659d:	1afa04        	ae_selp24.hl	aep0, aep7, aep2
    65a0:	1a8876        	loop	a8, 65be <beam2_mid_band+0x176>
    65a3:	fc241ae65f 	{ ae_lp16x2f.c	aep6, a6, a5; ae_cvtq48p24s.h	aeq0, aep1 }
    65a8:	fd2c2d941f 	{ ae_lp16x2f.iu	aep1, a4, 4; ae_cvtq48p24s.l	aeq1, aep1 }
    65ad:	050604        	ae_mulaafp24s.hh.ll	aeq0, aep0, aep6
    65b0:	050e54        	ae_mulasfp24s.hl.lh	aeq1, aep0, aep6
    65b3:	1b8f24        	ae_roundsp16q48asym	aep2, aeq0
    65b6:	fb6bf2a31f 	{ ae_sp16f.l.iu	aep2, a3, 2; ae_roundsp16q48asym	aep7, aeq1 }
    65bb:	42f314        	ae_sp16f.l.iu	aep7, a3, 2

000065be <beam2_mid_band+0x176>:
    65be:	e0c442        	addi	a4, a4, -32

000065c1 <beam2_mid_band+0x179>:
    65c1:	f86e80122e 	{ addi	a2, a2, 1; ae_movp48	aep0, aep3 }
    65c6:	519226        	beqi	a2, 10, 661b <beam2_mid_band+0x1d3>
    65c9:	fc24ad9f1f 	{ ae_lp16x2f.iu	aep1, a15, 4; ae_zeroq56	aeq0 }
    65ce:	fd0464165f 	{ ae_sp16x2f.c	aep1, a6, a5; ae_movq56	aeq1, aeq0 }
    65d3:	880c           	movi.n	a8, 8
    65d5:	441404        	ae_lp16x2f.i	aep1, a4, 0
    65d8:	043d           	mov.n	a3, a4
    65da:	0020f0        	nop
    65dd:	0c8876        	loop	a8, 65ed <beam2_mid_band+0x1a5>
    65e0:	fc24daa65f 	{ ae_lp16x2f.c	aep2, a6, a5; nop }
    65e5:	051a54        	ae_mulasfp24s.hl.lh	aeq1, aep1, aep2
    65e8:	e2442d931f 	{ ae_lp16x2f.iu	aep1, a3, 4; ae_mulaafp24s.hh.ll	aeq0, aep1, aep2 }

000065ed <beam2_mid_band+0x1a5>:
    65ed:	f96f80004f 	{ movi	a4, 0; ae_roundsp24q48asym	aep2, aeq1 }
    65f2:	f867c0018f 	{ movi	a8, 1; ae_roundsp24q48asym	aep1, aeq0 }
    65f7:	f002fe12bf 	{ ae_selp24.ll	aep2, aep1, aep2; ae_mulzaafp24s.hh.ll	aeq1, aep0, aep0 }
    65fc:	dc42edb71f 	{ ae_lp16x2f.iu	aep3, a7, 4; ae_subsp24s	aep1, aep0, aep2 }
    6601:	079284        	ae_subsp24s	aep0, aep1, aep2
    6604:	090184        	ae_roundsp16asym	aep0, aep0
    6607:	048804        	ae_mulzaafp24s.hh.ll	aeq3, aep0, aep0
    660a:	09b104        	ae_ltq56s	b0, aeq1, aeq3
    660d:	d34800        	movt	a4, a8, b0
    6610:	ecd416        	beqz	a4, 64e1 <beam2_mid_band+0x99>
    6613:	f86680018f 	{ movi	a8, 1; ae_movp48	aep0, aep1 }
    6618:	ffb1c6        	j	64e3 <beam2_mid_band+0x9b>

0000661b <beam2_mid_band+0x1d3>:
    661b:	9128           	l32i.n	a2, a1, 36
    661d:	7178           	l32i.n	a7, a1, 28
    661f:	e76c62        	s32i	a6, a12, 0x39c
    6622:	f4a362        	movi	a6, 0x3f4
    6625:	830c           	movi.n	a3, 8
    6627:	463204        	ae_sp16x2f.i	aep3, a2, 0
    662a:	450c64        	ae_lp16x2f.x	aep0, a12, a6
    662d:	072d           	mov.n	a2, a7
    662f:	048376        	loop	a3, 6637 <beam2_mid_band+0x1ef>
    6632:	e3002d821f 	{ ae_lp16x2f.iu	aep0, a2, 4; ae_mulaafp24s.hh.ll	aeq2, aep0, aep0 }

00006637 <beam2_mid_band+0x1ef>:
    6637:	81f8           	l32i.n	a15, a1, 32
    6639:	3520b4        	ae_sraiq56	aeq0, aeq2, 2
    663c:	f8638ffd2f 	{ movi	a2, -3; ae_roundsp16q48asym	aep0, aeq0 }
    6641:	a4a332        	movi	a3, 0x3a4
    6644:	430c34        	ae_sp16f.l.x	aep0, a12, a3
    6647:	02a2f7        	bge	a2, a15, 664d <beam2_mid_band+0x205>
    664a:	002106        	j	66d2 <beam2_mid_band+0x28a>
    664d:	681c22        	l16ui	a2, a12, 208
    6650:	5158           	l32i.n	a5, a1, 20
    6652:	ed2c82        	l32i	a8, a12, 0x3b4
    6655:	079256        	bnez	a2, 66d2 <beam2_mid_band+0x28a>
    6658:	03dc22        	addmi	a2, a12, 0x300
    665b:	5c9232        	l16si	a3, a2, 184
    665e:	ed2c92        	l32i	a9, a12, 0x3b4
    6661:	51b8           	l32i.n	a11, a1, 20
    6663:	331b           	addi.n	a3, a3, 1
    6665:	834d           	ae_sext16	a4, a3
    6667:	5c5232        	s16i	a3, a2, 184
    666a:	5cc4a6        	blti	a4, 32, 66ca <beam2_mid_band+0x282>
    666d:	03dc62        	addmi	a6, a12, 0x300
    6670:	5d96d2        	l16si	a13, a6, 186
    6673:	bca372        	movi	a7, 0x3bc
    6676:	10c942        	addi	a4, a9, 16
    6679:	030c           	movi.n	a3, 0
    667b:	ac7a           	add.n	a10, a12, a7
    667d:	5c5232        	s16i	a3, a2, 184
    6680:	414540        	srli	a4, a4, 5
    6683:	902da0        	addx2	a2, a13, a10
    6686:	529b52        	l16si	a5, a11, 164
    6689:	624d           	ae_s16i.n	a4, a2, 0
    668b:	5d9622        	l16si	a2, a6, 186
    668e:	be0c           	movi.n	a14, 11
    6690:	cb0c           	movi.n	a11, 12
    6692:	221b           	addi.n	a2, a2, 1
    6694:	82fd           	ae_sext16	a15, a2
    6696:	724ef0        	salt	a4, a14, a15
    6699:	932340        	movnez	a2, a3, a4
    669c:	5d5622        	s16i	a2, a6, 186
    669f:	e6eb21        	l32r	a2, 24c (3bd8 <take_min_four>)
    66a2:	ed6c52        	s32i	a5, a12, 0x3b4
    66a5:	f4a362        	movi	a6, 0x3f4
    66a8:	0c4d           	mov.n	a4, a12
    66aa:	0002e0        	callx8	a2
    66ad:	e68f21        	l32r	a2, ec (7800 <immediate_num>)
    66b0:	ea2432        	l32i	a3, a4, 0x3a8
    66b3:	0abd           	mov.n	a11, a10
    66b5:	20c2c2        	addi	a12, a2, 32
    66b8:	e6a321        	l32r	a2, 144 (5b0 <siir_safr>)
    66bb:	03ad           	mov.n	a10, a3
    66bd:	0002e0        	callx8	a2
    66c0:	7178           	l32i.n	a7, a1, 28
    66c2:	04cd           	mov.n	a12, a4
    66c4:	ea64a2        	s32i	a10, a4, 0x3a8
    66c7:	0001c6        	j	66d2 <beam2_mid_band+0x28a>

000066ca <beam2_mid_band+0x282>:
    66ca:	529522        	l16si	a2, a5, 164
    66cd:	282a           	add.n	a2, a8, a2
    66cf:	ed6c22        	s32i	a2, a12, 0x3b4
    66d2:	ec2c22        	l32i	a2, a12, 0x3b0
    66d5:	130c           	movi.n	a3, 1
    66d7:	10d332        	addmi	a3, a3, 0x1000
    66da:	5f2237        	blt	a2, a3, 673d <beam2_mid_band+0x2f5>
    66dd:	5148           	l32i.n	a4, a1, 20
    66df:	d23c           	movi.n	a2, 61
    66e1:	130c           	movi.n	a3, 1
    66e3:	529442        	l16si	a4, a4, 164
    66e6:	ea2c52        	l32i	a5, a12, 0x3a8
    66e9:	4ad222        	addmi	a2, a2, 0x4a00
    66ec:	113320        	slli	a3, a3, 14
    66ef:	364534        	ae_cvtq48a32s	aeq1, a5
    66f2:	300324        	ae_cvtp24a16x2.ll	aep0, a3, a2
    66f5:	11d410        	slli	a13, a4, 15
    66f8:	d5633ecd3f 	{ ae_cvtq48a32s	aeq0, a13; ae_mulfq32sp16s.h	aeq2, aeq1, aep0 }
    66fd:	d4e34000ef 	{ movi	a14, 0; ae_mulfq32sp16s.l	aeq1, aeq1, aep0 }
    6702:	d6c00001ff 	{ movi	a15, 1; ae_ltq56s	b0, aeq2, aeq0 }
    6707:	d64440002f 	{ movi	a2, 0; ae_ltq56s	b1, aeq1, aeq0 }
    670c:	130c           	movi.n	a3, 1
    670e:	d3ef00        	movt	a14, a15, b0
    6711:	8eac           	beqz.n	a14, 673d <beam2_mid_band+0x2f5>
    6713:	d32310        	movt	a2, a3, b1
    6716:	728c           	beqz.n	a2, 6721 <beam2_mid_band+0x2d9>
    6718:	03dc22        	addmi	a2, a12, 0x300
    671b:	531222        	l16ui	a2, a2, 166
    671e:	000146        	j	6727 <beam2_mid_band+0x2df>

00006721 <beam2_mid_band+0x2d9>:
    6721:	aea022        	movi	a2, 174
    6724:	7fd222        	addmi	a2, a2, 0x7f00
    6727:	0f1c           	movi.n	a15, 16
    6729:	300224        	ae_cvtp24a16x2.ll	aep0, a2, a2
    672c:	411c64        	ae_lp16f.x	aep1, a12, a6
    672f:	0a8f76        	loop	a15, 673d <beam2_mid_band+0x2f5>
    6732:	ea21ac971f 	{ ae_lp16f.iu	aep1, a7, 2; ae_mulfp24s.ll	aeq0, aep0, aep1 }
    6737:	1b8f24        	ae_roundsp16q48asym	aep2, aeq0
    673a:	4227f4        	ae_sp16f.l.i	aep2, a7, -2

0000673d <beam2_mid_band+0x2f5>:
    673d:	f01d           	retw.n

0000673f <beam2_mid_band+0x2f7>:
	...

00006740 <fill_vad_thrd_Nbands>:
    6740:	004136        	entry	a1, 32
    6743:	1a14a6        	blti	a4, 1, 6761 <fill_vad_thrd_Nbands+0x21>
    6746:	944d           	ae_zext16	a4, a4
    6748:	158476        	loop	a4, 6761 <fill_vad_thrd_Nbands+0x21>
    674b:	234d           	ae_l16si.n	a4, a3, 0
    674d:	332b           	addi.n	a3, a3, 2
    674f:	745840        	extui	a5, a4, 8, 8
    6752:	744040        	extui	a4, a4, 0, 8
    6755:	115590        	slli	a5, a5, 7
    6758:	114490        	slli	a4, a4, 7
    675b:	625d           	ae_s16i.n	a5, a2, 0
    675d:	724d           	ae_s16i.n	a4, a2, 2
    675f:	224b           	addi.n	a2, a2, 4

00006761 <fill_vad_thrd_Nbands+0x21>:
    6761:	f01d           	retw.n

00006763 <fill_vad_thrd_Nbands+0x23>:
	...

00006764 <complement_para>:
    6764:	004136        	entry	a1, 32
    6767:	f47c           	movi.n	a4, -1
    6769:	114410        	slli	a4, a4, 15
    676c:	c0f430        	sub	a15, a4, a3
    676f:	623d           	ae_s16i.n	a3, a2, 0
    6771:	72fd           	ae_s16i.n	a15, a2, 2
    6773:	f01d           	retw.n

00006775 <complement_para+0x11>:
    6775:	000000                                        ...

00006778 <common_noise_gen_init>:
    6778:	004136        	entry	a1, 32
    677b:	2c93d2        	l16si	a13, a3, 88
    677e:	2d9352        	l16si	a5, a3, 90
    6781:	fe7c           	movi.n	a14, -1
    6783:	140c           	movi.n	a4, 1
    6785:	105252        	s16i	a5, a2, 32
    6788:	115e10        	slli	a5, a14, 15
    678b:	8c5242        	s16i	a4, a2, 0x118
    678e:	2293b2        	l16si	a11, a3, 68
    6791:	2493c2        	l16si	a12, a3, 72
    6794:	c045d0        	sub	a4, a5, a13
    6797:	4f0c           	movi.n	a15, 4
    6799:	6d5242        	s16i	a4, a2, 218
    679c:	0252b2        	s16i	a11, a2, 4
    679f:	0352c2        	s16i	a12, a2, 6
    67a2:	6c52d2        	s16i	a13, a2, 216
    67a5:	70c332        	addi	a3, a3, 112
    67a8:	428b           	addi.n	a4, a2, 8
    67aa:	158f76        	loop	a15, 67c3 <common_noise_gen_init+0x4b>
    67ad:	235d           	ae_l16si.n	a5, a3, 0
    67af:	332b           	addi.n	a3, a3, 2
    67b1:	746850        	extui	a6, a5, 8, 8
    67b4:	745050        	extui	a5, a5, 0, 8
    67b7:	116690        	slli	a6, a6, 7
    67ba:	115590        	slli	a5, a5, 7
    67bd:	646d           	ae_s16i.n	a6, a4, 0
    67bf:	745d           	ae_s16i.n	a5, a4, 2
    67c1:	444b           	addi.n	a4, a4, 4

000067c3 <common_noise_gen_init+0x4b>:
    67c3:	740c           	movi.n	a4, 7
    67c5:	e6be31        	l32r	a3, 2c0 (8660 <win_ns>)
    67c8:	78d442        	addmi	a4, a4, 0x7800
    67cb:	3d6232        	s32i	a3, a2, 244
    67ce:	175242        	s16i	a4, a2, 46
    67d1:	f01d           	retw.n

000067d3 <common_noise_gen_init+0x5b>:
	...

000067d4 <common_NR_init>:
    67d4:	004136        	entry	a1, 32
    67d7:	2e0252        	l8ui	a5, a2, 46
    67da:	c268           	l32i.n	a6, a2, 48
    67dc:	a0a0b2        	movi	a11, 160
    67df:	35ac           	beqz.n	a5, 6806 <common_NR_init+0x32>
    67e1:	787c           	movi.n	a8, -9
    67e3:	791c           	movi.n	a9, 23
    67e5:	070c           	movi.n	a7, 0
    67e7:	018850        	slli	a8, a8, 27
    67ea:	119950        	slli	a9, a9, 11
    67ed:	ca0c           	movi.n	a10, 12
    67ef:	b2ba           	add.n	a11, a2, a11

000067f1 <common_NR_init+0x1d>:
    67f1:	06cd           	mov.n	a12, a6
    67f3:	0b89           	s32i.n	a8, a11, 0
    67f5:	038a76        	loop	a10, 67fc <common_NR_init+0x28>
    67f8:	6c9d           	ae_s16i.n	a9, a12, 0
    67fa:	cc2b           	addi.n	a12, a12, 2

000067fc <common_NR_init+0x28>:
    67fc:	771b           	addi.n	a7, a7, 1
    67fe:	18c662        	addi	a6, a6, 24
    6801:	bb4b           	addi.n	a11, a11, 4
    6803:	ea3757        	bltu	a7, a5, 67f1 <common_NR_init+0x1d>

00006806 <common_NR_init+0x32>:
    6806:	2e9362        	l16si	a6, a3, 92
    6809:	f57c           	movi.n	a5, -1
    680b:	115510        	slli	a5, a5, 15
    680e:	625262        	s16i	a6, a2, 196
    6811:	c06560        	sub	a6, a5, a6
    6814:	635262        	s16i	a6, a2, 198
    6817:	2f9362        	l16si	a6, a3, 94
    681a:	645262        	s16i	a6, a2, 200
    681d:	ffa792        	movi	a9, 0x7ff
    6820:	c06560        	sub	a6, a5, a6
    6823:	655262        	s16i	a6, a2, 202
    6826:	309362        	l16si	a6, a3, 96
    6829:	665262        	s16i	a6, a2, 204
    682c:	0378           	l32i.n	a7, a3, 0
    682e:	c06560        	sub	a6, a5, a6
    6831:	675262        	s16i	a6, a2, 206
    6834:	319362        	l16si	a6, a3, 98
    6837:	685262        	s16i	a6, a2, 208
    683a:	180c           	movi.n	a8, 1
    683c:	c06560        	sub	a6, a5, a6
    683f:	695262        	s16i	a6, a2, 210
    6842:	329362        	l16si	a6, a3, 100
    6845:	9b6c           	movi.n	a11, -23
    6847:	1199b0        	slli	a9, a9, 5
    684a:	155282        	s16i	a8, a2, 42
    684d:	f480b0        	extui	a8, a11, 0, 16
    6850:	c05560        	sub	a5, a5, a6
    6853:	f7a1d2        	movi	a13, 0x1f7
    6856:	f7a3e2        	movi	a14, 0x3f7
    6859:	00a1f2        	movi	a15, 0x100
    685c:	715292        	s16i	a9, a2, 226
    685f:	6f5282        	s16i	a8, a2, 222
    6862:	118dd0        	slli	a8, a13, 3
    6865:	119ee0        	slli	a9, a14, 2
    6868:	6a5262        	s16i	a6, a2, 212
    686b:	6b5252        	s16i	a5, a2, 214
    686e:	e62fc1        	l32r	a12, 12c (7fff0000 <_end+0x7ffe7974>)
    6871:	1252f2        	s16i	a15, a2, 36
    6874:	6da0a2        	movi	a10, 109
    6877:	878a           	add.n	a8, a7, a8
    6879:	0279           	s32i.n	a7, a2, 0
    687b:	779a           	add.n	a7, a7, a9
    687d:	7893f2        	l16si	a15, a3, 240
    6880:	8b52a2        	s16i	a10, a2, 0x116
    6883:	4362c2        	s32i	a12, a2, 0x10c
    6886:	4462c2        	s32i	a12, a2, 0x110
    6889:	3b6282        	s32i	a8, a2, 236
    688c:	3c6272        	s32i	a7, a2, 240
    688f:	8a52f2        	s16i	a15, a2, 0x114
    6892:	132426        	beqi	a4, 2, 68a9 <common_NR_init+0xd5>
    6895:	2f0242        	l8ui	a4, a2, 47
    6898:	402222        	l32i	a2, a2, 0x100
    689b:	130c           	movi.n	a3, 1
    689d:	848c           	beqz.n	a4, 68a9 <common_NR_init+0xd5>
    689f:	013320        	slli	a3, a3, 30
    68a2:	038476        	loop	a4, 68a9 <common_NR_init+0xd5>
    68a5:	0239           	s32i.n	a3, a2, 0
    68a7:	224b           	addi.n	a2, a2, 4

000068a9 <common_NR_init+0xd5>:
    68a9:	f01d           	retw.n

000068ab <common_NR_init+0xd7>:
	...

000068ac <get_post_ec_memsize>:
    68ac:	004136        	entry	a1, 32
    68af:	bfa122        	movi	a2, 0x1bf
    68b2:	1122c0        	slli	a2, a2, 4
    68b5:	f01d           	retw.n

000068b7 <get_post_ec_memsize+0xb>:
	...

000068b8 <EXT_POST_EC_Init>:
    68b8:	004136        	entry	a1, 32
    68bb:	bfa1c2        	movi	a12, 0x1bf
    68be:	e60241        	l32r	a4, c8 (6bf0 <xt_memset>)
    68c1:	11ccd0        	slli	a12, a12, 3
    68c4:	0b0c           	movi.n	a11, 0
    68c6:	02ad           	mov.n	a10, a2
    68c8:	0004e0        	callx8	a4
    68cb:	e5d441        	l32r	a4, 1c (6c04 <xt_memcpy>)
    68ce:	a28b           	addi.n	a10, a2, 8
    68d0:	b32b           	addi.n	a11, a3, 2
    68d2:	79a0c2        	movi	a12, 121
    68d5:	0004e0        	callx8	a4
    68d8:	fca052        	movi	a5, 252
    68db:	f4a062        	movi	a6, 244
    68de:	a25a           	add.n	a10, a2, a5
    68e0:	b36a           	add.n	a11, a3, a6
    68e2:	5c0c           	movi.n	a12, 5
    68e4:	0004e0        	callx8	a4
    68e7:	18d232        	addmi	a3, a2, 0x1800
    68ea:	e63b41        	l32r	a4, 1d8 (6778 <common_noise_gen_init>)
    68ed:	03ad           	mov.n	a10, a3
    68ef:	02bd           	mov.n	a11, a2
    68f1:	0229           	s32i.n	a2, a2, 0
    68f3:	0004e0        	callx8	a4
    68f6:	bfa642        	movi	a4, 0x6bf
    68f9:	1144e0        	slli	a4, a4, 2
    68fc:	424a           	add.n	a4, a2, a4
    68fe:	47a6d2        	movi	a13, 0x647
    6901:	3e6342        	s32i	a4, a3, 248
    6904:	114de0        	slli	a4, a13, 2
    6907:	424a           	add.n	a4, a2, a4
    6909:	cfa6e2        	movi	a14, 0x6cf
    690c:	406342        	s32i	a4, a3, 0x100
    690f:	114ee0        	slli	a4, a14, 2
    6912:	c7a662        	movi	a6, 0x6c7
    6915:	424a           	add.n	a4, a2, a4
    6917:	1166e0        	slli	a6, a6, 2
    691a:	c349           	s32i.n	a4, a3, 48
    691c:	e63641        	l32r	a4, 1f4 (67d4 <common_NR_init>)
    691f:	626a           	add.n	a6, a2, a6
    6921:	0c0c           	movi.n	a12, 0
    6923:	03ad           	mov.n	a10, a3
    6925:	02bd           	mov.n	a11, a2
    6927:	3f6362        	s32i	a6, a3, 252
    692a:	0004e0        	callx8	a4
    692d:	10d232        	addmi	a3, a2, 0x1000
    6930:	18d242        	addmi	a4, a2, 0x1800
    6933:	ffa0d2        	movi	a13, 255
    6936:	3a6432        	s32i	a3, a4, 232
    6939:	059252        	l16si	a5, a2, 10
    693c:	1bd2c2        	addmi	a12, a2, 0x1b00
    693f:	7fdd42        	addmi	a4, a13, 0x7f00
    6942:	8192f2        	l16si	a15, a2, 0x102
    6945:	735c52        	s16i	a5, a12, 230
    6948:	755cf2        	s16i	a15, a12, 234
    694b:	725c42        	s16i	a4, a12, 228
    694e:	745c42        	s16i	a4, a12, 232
    6951:	f01d           	retw.n

00006953 <EXT_POST_EC_Init+0x9b>:
	...

00006954 <aec_post_prcs>:
    6954:	004136        	entry	a1, 32
    6957:	ffa0f2        	movi	a15, 255
    695a:	427d           	ae_l16ui.n	a7, a2, 0
    695c:	626d           	ae_s16i.n	a6, a2, 0
    695e:	303664        	ae_cvtp24a16x2.ll	aep3, a6, a6
    6961:	7fdf62        	addmi	a6, a15, 0x7f00
    6964:	401214        	ae_lp16f.i	aep1, a2, 2
    6967:	302664        	ae_cvtp24a16x2.ll	aep2, a6, a6
    696a:	ea458fff8f 	{ movi	a8, -1; ae_mulfp24s.ll	aeq0, aep1, aep2 }
    696f:	302774        	ae_cvtp24a16x2.ll	aep2, a7, a7
    6972:	dd4ec0102f 	{ movi	a2, 16; ae_subsp24s	aep3, aep3, aep2 }
    6977:	418980        	srli	a8, a8, 9
    697a:	f867e2088f 	{ ae_movpa24x2	aep0, a8, a8; ae_roundsp24q48asym	aep1, aeq0 }
    697f:	f60c           	movi.n	a6, 15
    6981:	cd2c4fe55e 	{ addi	a5, a5, -2; ae_sraip24	aep3, aep3, 4 }
    6986:	fec442        	addi	a4, a4, -2
    6989:	404304        	ae_lp16f.i	aep4, a3, 0

0000698c <aec_post_prcs+0x38>:
    698c:	07a324        	ae_addsp24s	aep2, aep2, aep3
    698f:	0782d4        	ae_subsp24s	aep5, aep0, aep2
    6992:	015174        	ae_mulfp24s.ll	aeq0, aep5, aep1
    6995:	060934        	ae_roundsq32asym	aeq0, aeq0
    6998:	188676        	loop	a6, 69b4 <aec_post_prcs+0x60>
    699b:	ea89ecf51f 	{ ae_lp16f.iu	aep7, a5, 2; ae_mulfp24s.ll	aeq1, aep2, aep4 }
    69a0:	d5fe2cc31f 	{ ae_lp16f.iu	aep4, a3, 2; ae_mulfq32sp16s.l	aeq2, aeq0, aep7 }
    69a5:	065934        	ae_roundsq32asym	aeq1, aeq1
    69a8:	35a094        	ae_slliq56	aeq2, aeq2, 2
    69ab:	015ac4        	ae_addq56	aeq1, aeq1, aeq2
    69ae:	1b9f54        	ae_roundsp16q48asym	aep5, aeq1
    69b1:	42d414        	ae_sp16f.l.iu	aep5, a4, 2

000069b4 <aec_post_prcs+0x60>:
    69b4:	220b           	addi.n	a2, a2, -1
    69b6:	fd2256        	bnez	a2, 698c <aec_post_prcs+0x38>
    69b9:	f01d           	retw.n

000069bb <aec_post_prcs+0x67>:
	...

000069bc <ns_ifft_wo>:
    69bc:	004136        	entry	a1, 32
    69bf:	250c           	movi.n	a5, 2
    69c1:	030480        	rsr.br	a8
    69c4:	0248           	l32i.n	a4, a2, 0
    69c6:	0a1580        	depbits	a8, a5, 0, 2
    69c9:	e61a71        	l32r	a7, 234 (8160 <fftorder_512_12b>)
    69cc:	3a2252        	l32i	a5, a2, 232
    69cf:	060c           	movi.n	a6, 0
    69d1:	1469           	s32i.n	a6, a4, 4
    69d3:	0469           	s32i.n	a6, a4, 0
    69d5:	80a0f2        	movi	a15, 128
    69d8:	130480        	wsr.br	a8
    69db:	674b           	addi.n	a6, a7, 4
    69dd:	5c0504        	ae_lp24x2f.i	aep0, a5, 0
    69e0:	0e8f76        	loop	a15, 69f2 <ns_ifft_wo+0x36>
    69e3:	fc1860067e 	{ l16si	a7, a6, 0; ae_negsp24s	aep1, aep0 }
    69e8:	e0042f851f 	{ ae_lp24x2f.iu	aep0, a5, 8; ae_movtp24x2	aep1, aep0, b0:b1 }
    69ed:	664b           	addi.n	a6, a6, 4
    69ef:	5f1474        	ae_sp24x2f.x	aep1, a4, a7

000069f2 <ns_ifft_wo+0x36>:
    69f2:	7fa072        	movi	a7, 127
    69f5:	5c85e4        	ae_lp24x2f.iu	aep0, a5, -16
    69f8:	098776        	loop	a7, 6a05 <ns_ifft_wo+0x49>
    69fb:	267d           	ae_l16si.n	a7, a6, 0
    69fd:	664b           	addi.n	a6, a6, 4
    69ff:	5f0474        	ae_sp24x2f.x	aep0, a4, a7
    6a02:	5c85f4        	ae_lp24x2f.iu	aep0, a5, -8
    6a05:	e60c51        	l32r	a5, 238 (50a4 <sw_fft_32b_2811>)
    6a08:	00a1b2        	movi	a11, 0x100
    6a0b:	8c0c           	movi.n	a12, 8
    6a0d:	0d0c           	movi.n	a13, 0
    6a0f:	1e0c           	movi.n	a14, 1
    6a11:	04ad           	mov.n	a10, a4
    6a13:	0005e0        	callx8	a5
    6a16:	139262        	l16si	a6, a2, 38
    6a19:	3f22e2        	l32i	a14, a2, 252
    6a1c:	3d2272        	l32i	a7, a2, 244
    6a1f:	c0fa60        	sub	a15, a10, a6
    6a22:	fece22        	addi	a2, a14, -2
    6a25:	091c           	movi.n	a9, 16
    6a27:	fec352        	addi	a5, a3, -2
    6a2a:	fec762        	addi	a6, a7, -2
    6a2d:	600404        	ae_lq32f.i	aeq0, a4, 0
    6a30:	20c772        	addi	a7, a7, 32
    6a33:	f8cf32        	addi	a3, a15, -8
    6a36:	028d           	mov.n	a8, a2
    6a38:	1c8976        	loop	a9, 6a58 <ns_ifft_wo+0x9c>
    6a3b:	fc24fe830f 	{ ae_slaasq56s	aeq0, aeq0, a3; nop }
    6a40:	fc446c881f 	{ ae_lp16f.iu	aep0, a8, 2; ae_roundsq32sym	aeq0, aeq0 }
    6a45:	4097f4        	ae_lp16f.iu	aep1, a7, -2
    6a48:	ea20ecf61f 	{ ae_lp16f.iu	aep7, a6, 2; ae_mulfp24s.hl	aeq1, aep0, aep1 }
    6a4d:	f49efd142f 	{ ae_lq32f.iu	aeq0, a4, 8; ae_mulafq32sp16s.l	aeq1, aeq0, aep7 }
    6a52:	1b9e04        	ae_roundsp16q48sym	aep0, aeq1
    6a55:	428514        	ae_sp16f.l.iu	aep0, a5, 2
    6a58:	e0a062        	movi	a6, 224
    6a5b:	f03d           	nop.n
    6a5d:	0f8676        	loop	a6, 6a70 <ns_ifft_wo+0xb4>
    6a60:	fc24fe834f 	{ ae_slaasq56s	aeq1, aeq0, a3; nop }
    6a65:	fd4c7d142f 	{ ae_lq32f.iu	aeq0, a4, 8; ae_roundsq32sym	aeq1, aeq1 }
    6a6a:	1b9e04        	ae_roundsp16q48sym	aep0, aeq1
    6a6d:	428514        	ae_sp16f.l.iu	aep0, a5, 2
    6a70:	051c           	movi.n	a5, 16
    6a72:	0020f0        	nop
    6a75:	0f8576        	loop	a5, 6a88 <ns_ifft_wo+0xcc>
    6a78:	fc24fe834f 	{ ae_slaasq56s	aeq1, aeq0, a3; nop }
    6a7d:	fd4c7d142f 	{ ae_lq32f.iu	aeq0, a4, 8; ae_roundsq32sym	aeq1, aeq1 }
    6a82:	1b9e04        	ae_roundsp16q48sym	aep0, aeq1
    6a85:	428214        	ae_sp16f.l.iu	aep0, a2, 2
    6a88:	f01d           	retw.n

00006a8a <ns_ifft_wo+0xce>:
	...

00006a8c <NOISE_GEN_Prcs>:
    6a8c:	004136        	entry	a1, 32
    6a8f:	fc2080786f 	{ movi	a6, 120; ae_zerop48	aep0 }
    6a94:	441304        	ae_lp16x2f.i	aep1, a3, 0
    6a97:	035d           	mov.n	a5, a3
    6a99:	f03d           	nop.n
    6a9b:	f03d           	nop.n
    6a9d:	048676        	loop	a6, 6aa5 <NOISE_GEN_Prcs+0x19>
    6aa0:	d821ad951f 	{ ae_lp16x2f.iu	aep1, a5, 4; ae_maxabssp24s	aep0, aep0, aep1 }

00006aa5 <NOISE_GEN_Prcs+0x19>:
    6aa5:	dc01e1225e 	{ l16si	a5, a2, 36; ae_selp24.lh	aep1, aep0, aep0 }
    6aaa:	d82233e2be 	{ l32i	a11, a2, 248; ae_maxp24s	aep0, aep0, aep1 }
    6aaf:	0008d4        	ae_cvta32p24.l	a8, aep0
    6ab2:	3d2262        	l32i	a6, a2, 244
    6ab5:	02a8           	l32i.n	a10, a2, 0
    6ab7:	40e880        	nsa	a8, a8
    6aba:	435850        	min	a5, a8, a5
    6abd:	e5ddc1        	l32r	a12, 234 (8160 <fftorder_512_12b>)
    6ac0:	125282        	s16i	a8, a2, 36
    6ac3:	031654        	wur.ae_sar	a5
    6ac6:	04a492        	movi	a9, 0x404
    6ac9:	447b04        	ae_lp16x2f.i	aep7, a11, 0
    6acc:	8d0c           	movi.n	a13, 8
    6ace:	9a9a           	add.n	a9, a10, a9
    6ad0:	070c           	movi.n	a7, 0
    6ad2:	08cc82        	addi	a8, a12, 8
    6ad5:	04daa2        	addmi	a10, a10, 0x400
    6ad8:	f83fad160f 	{ ae_lp16x2f.i	aep1, a6, 0; ae_sllssp24s	aep0, aep7 }
    6add:	158d76        	loop	a13, 6af6 <NOISE_GEN_Prcs+0x6a>
    6ae0:	e820adfb1f 	{ ae_lp16x2f.iu	aep7, a11, 4; ae_mulfp24s.hh	aeq0, aep0, aep1 }
    6ae5:	eb21ed961f 	{ ae_lp16x2f.iu	aep1, a6, 4; ae_mulfp24s.ll	aeq3, aep0, aep1 }
    6aea:	f83fb84a7f 	{ ae_sq32f.x	aeq0, a10, a7; ae_sllssp24s	aep0, aep7 }
    6aef:	62b974        	ae_sq32f.x	aeq3, a9, a7
    6af2:	287d           	ae_l16si.n	a7, a8, 0
    6af4:	888b           	addi.n	a8, a8, 8
    6af6:	70a0b2        	movi	a11, 112
    6af9:	440304        	ae_lp16x2f.i	aep0, a3, 0
    6afc:	f03d           	nop.n
    6afe:	138b76        	loop	a11, 6b15 <NOISE_GEN_Prcs+0x89>
    6b01:	f823ed831f 	{ ae_lp16x2f.iu	aep0, a3, 4; ae_sllssp24s	aep1, aep0 }
    6b06:	0a0984        	ae_cvtq48p24s.h	aeq0, aep1
    6b09:	fd2c384a7f 	{ ae_sq32f.x	aeq0, a10, a7; ae_cvtq48p24s.l	aeq1, aep1 }
    6b0e:	629974        	ae_sq32f.x	aeq1, a9, a7
    6b11:	287d           	ae_l16si.n	a7, a8, 0
    6b13:	888b           	addi.n	a8, a8, 8
    6b15:	3e22b2        	l32i	a11, a2, 248
    6b18:	8c0c           	movi.n	a12, 8
    6b1a:	fccbb2        	addi	a11, a11, -4
    6b1d:	188c76        	loop	a12, 6b39 <NOISE_GEN_Prcs+0xad>
    6b20:	f823eda6ff 	{ ae_lp16x2f.iu	aep2, a6, -4; ae_sllssp24s	aep1, aep0 }
    6b25:	ea44b38b1f 	{ ae_sp16x2f.iu	aep0, a11, 4; ae_mulfp24s.hl	aeq0, aep1, aep2 }
    6b2a:	e945ed831f 	{ ae_lp16x2f.iu	aep0, a3, 4; ae_mulfp24s.lh	aeq3, aep1, aep2 }
    6b2f:	628a74        	ae_sq32f.x	aeq0, a10, a7
    6b32:	62b974        	ae_sq32f.x	aeq3, a9, a7
    6b35:	287d           	ae_l16si.n	a7, a8, 0
    6b37:	888b           	addi.n	a8, a8, 8
    6b39:	0238           	l32i.n	a3, a2, 0
    6b3b:	80a0b2        	movi	a11, 128
    6b3e:	8c0c           	movi.n	a12, 8
    6b40:	04d3a2        	addmi	a10, a3, 0x400
    6b43:	e5bd31        	l32r	a3, 238 (50a4 <sw_fft_32b_2811>)
    6b46:	1d0c           	movi.n	a13, 1
    6b48:	0e0c           	movi.n	a14, 0
    6b4a:	0003e0        	callx8	a3
    6b4d:	c035a0        	sub	a3, a5, a10
    6b50:	135232        	s16i	a3, a2, 38
    6b53:	e5ba31        	l32r	a3, 23c (3c68 <gen_power>)
    6b56:	00a1b2        	movi	a11, 0x100
    6b59:	02ad           	mov.n	a10, a2
    6b5b:	0003e0        	callx8	a3
    6b5e:	e5aa31        	l32r	a3, 208 (3e04 <NR_vad1>)
    6b61:	02ad           	mov.n	a10, a2
    6b63:	0003e0        	callx8	a3
    6b66:	e5a931        	l32r	a3, 20c (3f20 <NR_vad2>)
    6b69:	02ad           	mov.n	a10, a2
    6b6b:	0003e0        	callx8	a3
    6b6e:	e5a831        	l32r	a3, 210 (4098 <NR_NoiseEstimate>)
    6b71:	02ad           	mov.n	a10, a2
    6b73:	0003e0        	callx8	a3
    6b76:	e5d331        	l32r	a3, 2c4 (468 <comfort_noise_gen_wb2>)
    6b79:	02ad           	mov.n	a10, a2
    6b7b:	0003e0        	callx8	a3
    6b7e:	e5d231        	l32r	a3, 2c8 (69bc <ns_ifft_wo>)
    6b81:	02ad           	mov.n	a10, a2
    6b83:	04bd           	mov.n	a11, a4
    6b85:	0003e0        	callx8	a3
    6b88:	f01d           	retw.n

00006b8a <NOISE_GEN_Prcs+0xfe>:
	...

00006b8c <EXT_POST_EC_PRCS>:
    6b8c:	004136        	entry	a1, 32
    6b8f:	e58c71        	l32r	a7, 1c0 (6a8c <NOISE_GEN_Prcs>)
    6b92:	18d2a2        	addmi	a10, a2, 0x1800
    6b95:	03bd           	mov.n	a11, a3
    6b97:	02cd           	mov.n	a12, a2
    6b99:	0007e0        	callx8	a7
    6b9c:	f9a682        	movi	a8, 0x6f9
    6b9f:	115590        	slli	a5, a5, 7
    6ba2:	1188e0        	slli	a8, a8, 2
    6ba5:	7fc5e2        	addi	a14, a5, 127
    6ba8:	e58751        	l32r	a5, 1c4 (6954 <aec_post_prcs>)
    6bab:	1bd272        	addmi	a7, a2, 0x1b00
    6bae:	a28a           	add.n	a10, a2, a8
    6bb0:	03bd           	mov.n	a11, a3
    6bb2:	04cd           	mov.n	a12, a4
    6bb4:	02dd           	mov.n	a13, a2
    6bb6:	7457e2        	s16i	a14, a7, 232
    6bb9:	0005e0        	callx8	a5
    6bbc:	759722        	l16si	a2, a7, 234
    6bbf:	1f0c           	movi.n	a15, 1
    6bc1:	030c           	movi.n	a3, 0
    6bc3:	662d           	ae_s16i.n	a2, a6, 0
    6bc5:	112f30        	slli	a2, a15, 13
    6bc8:	763d           	ae_s16i.n	a3, a6, 2
    6bca:	025622        	s16i	a2, a6, 4
    6bcd:	f01d           	retw.n

00006bcf <EXT_POST_EC_PRCS+0x43>:
	...

00006bd0 <__do_global_ctors_aux>:
    6bd0:	004136        	entry	a1, 32
    6bd3:	e5be31        	l32r	a3, 2cc (75d4 <__CTOR_END__>)
    6bd6:	fcc322        	addi	a2, a3, -4
    6bd9:	0228           	l32i.n	a2, a2, 0
    6bdb:	f8c332        	addi	a3, a3, -8
    6bde:	0a0226        	beqi	a2, -1, 6bec <__do_global_ctors_aux+0x1c>

00006be1 <__do_global_ctors_aux+0x11>:
    6be1:	0002e0        	callx8	a2
    6be4:	0328           	l32i.n	a2, a3, 0
    6be6:	fcc332        	addi	a3, a3, -4
    6be9:	f40266        	bnei	a2, -1, 6be1 <__do_global_ctors_aux+0x11>
    6bec:	f01d           	retw.n

00006bee <__do_global_ctors_aux+0x1e>:
	...

00006bf0 <xt_memset>:
    6bf0:	004136        	entry	a1, 32
    6bf3:	928c           	beqz.n	a2, 6c00 <xt_memset+0x10>
    6bf5:	748c           	beqz.n	a4, 6c00 <xt_memset+0x10>
    6bf7:	025d           	mov.n	a5, a2
    6bf9:	038476        	loop	a4, 6c00 <xt_memset+0x10>
    6bfc:	653d           	ae_s16i.n	a3, a5, 0
    6bfe:	552b           	addi.n	a5, a5, 2

00006c00 <xt_memset+0x10>:
    6c00:	f01d           	retw.n

00006c02 <xt_memset+0x12>:
	...

00006c04 <xt_memcpy>:
    6c04:	004136        	entry	a1, 32
    6c07:	025d           	mov.n	a5, a2
    6c09:	020c           	movi.n	a2, 0
    6c0b:	b5bc           	beqz.n	a5, 6c4a <xt_memcpy+0x46>
    6c0d:	93bc           	beqz.n	a3, 6c4a <xt_memcpy+0x46>
    6c0f:	24b357        	bgeu	a3, a5, 6c37 <xt_memcpy+0x33>
    6c12:	902430        	addx2	a2, a4, a3
    6c15:	1eb527        	bgeu	a5, a2, 6c37 <xt_memcpy+0x33>
    6c18:	052d           	mov.n	a2, a5
    6c1a:	c4ac           	beqz.n	a4, 6c4a <xt_memcpy+0x46>
    6c1c:	240b           	addi.n	a2, a4, -1
    6c1e:	1122f0        	slli	a2, a2, 1
    6c21:	332a           	add.n	a3, a3, a2
    6c23:	652a           	add.n	a6, a5, a2
    6c25:	0b8476        	loop	a4, 6c34 <xt_memcpy+0x30>
    6c28:	232d           	ae_l16si.n	a2, a3, 0
    6c2a:	662d           	ae_s16i.n	a2, a6, 0
    6c2c:	fec332        	addi	a3, a3, -2
    6c2f:	052d           	mov.n	a2, a5
    6c31:	fec662        	addi	a6, a6, -2

00006c34 <xt_memcpy+0x30>:
    6c34:	000486        	j	6c4a <xt_memcpy+0x46>

00006c37 <xt_memcpy+0x33>:
    6c37:	052d           	mov.n	a2, a5
    6c39:	d48c           	beqz.n	a4, 6c4a <xt_memcpy+0x46>
    6c3b:	056d           	mov.n	a6, a5
    6c3d:	098476        	loop	a4, 6c4a <xt_memcpy+0x46>
    6c40:	232d           	ae_l16si.n	a2, a3, 0
    6c42:	662d           	ae_s16i.n	a2, a6, 0
    6c44:	332b           	addi.n	a3, a3, 2
    6c46:	052d           	mov.n	a2, a5
    6c48:	662b           	addi.n	a6, a6, 2

00006c4a <xt_memcpy+0x46>:
    6c4a:	f01d           	retw.n

00006c4c <_fini>:
    6c4c:	008136        	entry	a1, 64
    6c4f:	e5a081        	l32r	a8, 2d0 (2ec <__do_global_dtors_aux>)
    6c52:	f03d           	nop.n
    6c54:	0008e0        	callx8	a8

00006c57 <_fini+0xb>:
    6c57:	f01d           	retw.n

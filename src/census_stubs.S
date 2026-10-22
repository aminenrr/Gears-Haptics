.text
.globl cen_stub_0
cen_stub_0:
  movq %rdx, g_cen_rdx+0(%rip)
  lock incl g_cen_cnt+0(%rip)
  jmp *g_cen_orig+0(%rip)
.globl cen_stub_1
cen_stub_1:
  movq %rdx, g_cen_rdx+8(%rip)
  lock incl g_cen_cnt+4(%rip)
  jmp *g_cen_orig+8(%rip)
.globl cen_stub_2
cen_stub_2:
  movq %rdx, g_cen_rdx+16(%rip)
  lock incl g_cen_cnt+8(%rip)
  jmp *g_cen_orig+16(%rip)
.globl cen_stub_3
cen_stub_3:
  movq %rdx, g_cen_rdx+24(%rip)
  lock incl g_cen_cnt+12(%rip)
  jmp *g_cen_orig+24(%rip)
.globl cen_stub_4
cen_stub_4:
  movq %rdx, g_cen_rdx+32(%rip)
  lock incl g_cen_cnt+16(%rip)
  jmp *g_cen_orig+32(%rip)
.globl cen_stub_5
cen_stub_5:
  movq %rdx, g_cen_rdx+40(%rip)
  lock incl g_cen_cnt+20(%rip)
  jmp *g_cen_orig+40(%rip)
.globl cen_stub_6
cen_stub_6:
  movq %rdx, g_cen_rdx+48(%rip)
  lock incl g_cen_cnt+24(%rip)
  jmp *g_cen_orig+48(%rip)
.globl cen_stub_7
cen_stub_7:
  movq %rdx, g_cen_rdx+56(%rip)
  lock incl g_cen_cnt+28(%rip)
  jmp *g_cen_orig+56(%rip)
.globl cen_stub_8
cen_stub_8:
  movq %rdx, g_cen_rdx+64(%rip)
  lock incl g_cen_cnt+32(%rip)
  jmp *g_cen_orig+64(%rip)
.globl cen_stub_9
cen_stub_9:
  movq %rdx, g_cen_rdx+72(%rip)
  lock incl g_cen_cnt+36(%rip)
  jmp *g_cen_orig+72(%rip)
.globl cen_stub_10
cen_stub_10:
  movq %rdx, g_cen_rdx+80(%rip)
  lock incl g_cen_cnt+40(%rip)
  jmp *g_cen_orig+80(%rip)
.globl cen_stub_11
cen_stub_11:
  movq %rdx, g_cen_rdx+88(%rip)
  lock incl g_cen_cnt+44(%rip)
  jmp *g_cen_orig+88(%rip)
.globl cen_stub_12
cen_stub_12:
  movq %rdx, g_cen_rdx+96(%rip)
  lock incl g_cen_cnt+48(%rip)
  jmp *g_cen_orig+96(%rip)
.globl cen_stub_13
cen_stub_13:
  movq %rdx, g_cen_rdx+104(%rip)
  lock incl g_cen_cnt+52(%rip)
  jmp *g_cen_orig+104(%rip)
.globl cen_stub_14
cen_stub_14:
  movq %rdx, g_cen_rdx+112(%rip)
  lock incl g_cen_cnt+56(%rip)
  jmp *g_cen_orig+112(%rip)
.globl cen_stub_15
cen_stub_15:
  movq %rdx, g_cen_rdx+120(%rip)
  lock incl g_cen_cnt+60(%rip)
  jmp *g_cen_orig+120(%rip)
.globl cen_stub_16
cen_stub_16:
  movq %rdx, g_cen_rdx+128(%rip)
  lock incl g_cen_cnt+64(%rip)
  jmp *g_cen_orig+128(%rip)
.globl cen_stub_17
cen_stub_17:
  movq %rdx, g_cen_rdx+136(%rip)
  lock incl g_cen_cnt+68(%rip)
  jmp *g_cen_orig+136(%rip)
.globl cen_stub_18
cen_stub_18:
  movq %rdx, g_cen_rdx+144(%rip)
  lock incl g_cen_cnt+72(%rip)
  jmp *g_cen_orig+144(%rip)
.globl cen_stub_19
cen_stub_19:
  movq %rdx, g_cen_rdx+152(%rip)
  lock incl g_cen_cnt+76(%rip)
  jmp *g_cen_orig+152(%rip)
.globl cen_stub_20
cen_stub_20:
  movq %rdx, g_cen_rdx+160(%rip)
  lock incl g_cen_cnt+80(%rip)
  jmp *g_cen_orig+160(%rip)
.globl cen_stub_21
cen_stub_21:
  movq %rdx, g_cen_rdx+168(%rip)
  lock incl g_cen_cnt+84(%rip)
  jmp *g_cen_orig+168(%rip)
.globl cen_stub_22
cen_stub_22:
  movq %rdx, g_cen_rdx+176(%rip)
  lock incl g_cen_cnt+88(%rip)
  jmp *g_cen_orig+176(%rip)
.globl cen_stub_23
cen_stub_23:
  movq %rdx, g_cen_rdx+184(%rip)
  lock incl g_cen_cnt+92(%rip)
  jmp *g_cen_orig+184(%rip)
.globl cen_stub_24
cen_stub_24:
  movq %rdx, g_cen_rdx+192(%rip)
  lock incl g_cen_cnt+96(%rip)
  jmp *g_cen_orig+192(%rip)
.globl cen_stub_25
cen_stub_25:
  movq %rdx, g_cen_rdx+200(%rip)
  lock incl g_cen_cnt+100(%rip)
  jmp *g_cen_orig+200(%rip)
.globl cen_stub_26
cen_stub_26:
  movq %rdx, g_cen_rdx+208(%rip)
  lock incl g_cen_cnt+104(%rip)
  jmp *g_cen_orig+208(%rip)
.globl cen_stub_27
cen_stub_27:
  movq %rdx, g_cen_rdx+216(%rip)
  lock incl g_cen_cnt+108(%rip)
  jmp *g_cen_orig+216(%rip)
.globl cen_stub_28
cen_stub_28:
  movq %rdx, g_cen_rdx+224(%rip)
  lock incl g_cen_cnt+112(%rip)
  jmp *g_cen_orig+224(%rip)
.globl cen_stub_29
cen_stub_29:
  movq %rdx, g_cen_rdx+232(%rip)
  lock incl g_cen_cnt+116(%rip)
  jmp *g_cen_orig+232(%rip)
.globl cen_stub_30
cen_stub_30:
  movq %rdx, g_cen_rdx+240(%rip)
  lock incl g_cen_cnt+120(%rip)
  jmp *g_cen_orig+240(%rip)
.globl cen_stub_31
cen_stub_31:
  movq %rdx, g_cen_rdx+248(%rip)
  lock incl g_cen_cnt+124(%rip)
  jmp *g_cen_orig+248(%rip)
.globl cen_stub_32
cen_stub_32:
  movq %rdx, g_cen_rdx+256(%rip)
  lock incl g_cen_cnt+128(%rip)
  jmp *g_cen_orig+256(%rip)
.globl cen_stub_33
cen_stub_33:
  movq %rdx, g_cen_rdx+264(%rip)
  lock incl g_cen_cnt+132(%rip)
  jmp *g_cen_orig+264(%rip)
.globl cen_stub_34
cen_stub_34:
  movq %rdx, g_cen_rdx+272(%rip)
  lock incl g_cen_cnt+136(%rip)
  jmp *g_cen_orig+272(%rip)
.globl cen_stub_35
cen_stub_35:
  movq %rdx, g_cen_rdx+280(%rip)
  lock incl g_cen_cnt+140(%rip)
  jmp *g_cen_orig+280(%rip)
.globl cen_stub_36
cen_stub_36:
  movq %rdx, g_cen_rdx+288(%rip)
  lock incl g_cen_cnt+144(%rip)
  jmp *g_cen_orig+288(%rip)
.globl cen_stub_37
cen_stub_37:
  movq %rdx, g_cen_rdx+296(%rip)
  lock incl g_cen_cnt+148(%rip)
  jmp *g_cen_orig+296(%rip)
.globl cen_stub_38
cen_stub_38:
  movq %rdx, g_cen_rdx+304(%rip)
  lock incl g_cen_cnt+152(%rip)
  jmp *g_cen_orig+304(%rip)
.globl cen_stub_39
cen_stub_39:
  movq %rdx, g_cen_rdx+312(%rip)
  lock incl g_cen_cnt+156(%rip)
  jmp *g_cen_orig+312(%rip)
.globl cen_stub_40
cen_stub_40:
  movq %rdx, g_cen_rdx+320(%rip)
  lock incl g_cen_cnt+160(%rip)
  jmp *g_cen_orig+320(%rip)
.globl cen_stub_41
cen_stub_41:
  movq %rdx, g_cen_rdx+328(%rip)
  lock incl g_cen_cnt+164(%rip)
  jmp *g_cen_orig+328(%rip)
.globl cen_stub_42
cen_stub_42:
  movq %rdx, g_cen_rdx+336(%rip)
  lock incl g_cen_cnt+168(%rip)
  jmp *g_cen_orig+336(%rip)
.globl cen_stub_43
cen_stub_43:
  movq %rdx, g_cen_rdx+344(%rip)
  lock incl g_cen_cnt+172(%rip)
  jmp *g_cen_orig+344(%rip)
.globl cen_stub_44
cen_stub_44:
  movq %rdx, g_cen_rdx+352(%rip)
  lock incl g_cen_cnt+176(%rip)
  jmp *g_cen_orig+352(%rip)
.globl cen_stub_45
cen_stub_45:
  movq %rdx, g_cen_rdx+360(%rip)
  lock incl g_cen_cnt+180(%rip)
  jmp *g_cen_orig+360(%rip)
.globl cen_stub_46
cen_stub_46:
  movq %rdx, g_cen_rdx+368(%rip)
  lock incl g_cen_cnt+184(%rip)
  jmp *g_cen_orig+368(%rip)
.globl cen_stub_47
cen_stub_47:
  movq %rdx, g_cen_rdx+376(%rip)
  lock incl g_cen_cnt+188(%rip)
  jmp *g_cen_orig+376(%rip)
.globl cen_stub_48
cen_stub_48:
  movq %rdx, g_cen_rdx+384(%rip)
  lock incl g_cen_cnt+192(%rip)
  jmp *g_cen_orig+384(%rip)
.globl cen_stub_49
cen_stub_49:
  movq %rdx, g_cen_rdx+392(%rip)
  lock incl g_cen_cnt+196(%rip)
  jmp *g_cen_orig+392(%rip)
.globl cen_stub_50
cen_stub_50:
  movq %rdx, g_cen_rdx+400(%rip)
  lock incl g_cen_cnt+200(%rip)
  jmp *g_cen_orig+400(%rip)
.globl cen_stub_51
cen_stub_51:
  movq %rdx, g_cen_rdx+408(%rip)
  lock incl g_cen_cnt+204(%rip)
  jmp *g_cen_orig+408(%rip)
.globl cen_stub_52
cen_stub_52:
  movq %rdx, g_cen_rdx+416(%rip)
  lock incl g_cen_cnt+208(%rip)
  jmp *g_cen_orig+416(%rip)
.globl cen_stub_53
cen_stub_53:
  movq %rdx, g_cen_rdx+424(%rip)
  lock incl g_cen_cnt+212(%rip)
  jmp *g_cen_orig+424(%rip)
.globl cen_stub_54
cen_stub_54:
  movq %rdx, g_cen_rdx+432(%rip)
  lock incl g_cen_cnt+216(%rip)
  jmp *g_cen_orig+432(%rip)
.globl cen_stub_55
cen_stub_55:
  movq %rdx, g_cen_rdx+440(%rip)
  lock incl g_cen_cnt+220(%rip)
  jmp *g_cen_orig+440(%rip)
.globl cen_stub_56
cen_stub_56:
  movq %rdx, g_cen_rdx+448(%rip)
  lock incl g_cen_cnt+224(%rip)
  jmp *g_cen_orig+448(%rip)
.globl cen_stub_57
cen_stub_57:
  movq %rdx, g_cen_rdx+456(%rip)
  lock incl g_cen_cnt+228(%rip)
  jmp *g_cen_orig+456(%rip)
.globl cen_stub_58
cen_stub_58:
  movq %rdx, g_cen_rdx+464(%rip)
  lock incl g_cen_cnt+232(%rip)
  jmp *g_cen_orig+464(%rip)
.globl cen_stub_59
cen_stub_59:
  movq %rdx, g_cen_rdx+472(%rip)
  lock incl g_cen_cnt+236(%rip)
  jmp *g_cen_orig+472(%rip)
.globl cen_stub_60
cen_stub_60:
  movq %rdx, g_cen_rdx+480(%rip)
  lock incl g_cen_cnt+240(%rip)
  jmp *g_cen_orig+480(%rip)
.globl cen_stub_61
cen_stub_61:
  movq %rdx, g_cen_rdx+488(%rip)
  lock incl g_cen_cnt+244(%rip)
  jmp *g_cen_orig+488(%rip)
.globl cen_stub_62
cen_stub_62:
  movq %rdx, g_cen_rdx+496(%rip)
  lock incl g_cen_cnt+248(%rip)
  jmp *g_cen_orig+496(%rip)
.globl cen_stub_63
cen_stub_63:
  movq %rdx, g_cen_rdx+504(%rip)
  lock incl g_cen_cnt+252(%rip)
  jmp *g_cen_orig+504(%rip)
.globl cen_stub_64
cen_stub_64:
  movq %rdx, g_cen_rdx+512(%rip)
  lock incl g_cen_cnt+256(%rip)
  jmp *g_cen_orig+512(%rip)
.globl cen_stub_65
cen_stub_65:
  movq %rdx, g_cen_rdx+520(%rip)
  lock incl g_cen_cnt+260(%rip)
  jmp *g_cen_orig+520(%rip)
.globl cen_stub_66
cen_stub_66:
  movq %rdx, g_cen_rdx+528(%rip)
  lock incl g_cen_cnt+264(%rip)
  jmp *g_cen_orig+528(%rip)
.globl cen_stub_67
cen_stub_67:
  movq %rdx, g_cen_rdx+536(%rip)
  lock incl g_cen_cnt+268(%rip)
  jmp *g_cen_orig+536(%rip)
.globl cen_stub_68
cen_stub_68:
  movq %rdx, g_cen_rdx+544(%rip)
  lock incl g_cen_cnt+272(%rip)
  jmp *g_cen_orig+544(%rip)
.globl cen_stub_69
cen_stub_69:
  movq %rdx, g_cen_rdx+552(%rip)
  lock incl g_cen_cnt+276(%rip)
  jmp *g_cen_orig+552(%rip)
.globl cen_stub_70
cen_stub_70:
  movq %rdx, g_cen_rdx+560(%rip)
  lock incl g_cen_cnt+280(%rip)
  jmp *g_cen_orig+560(%rip)
.globl cen_stub_71
cen_stub_71:
  movq %rdx, g_cen_rdx+568(%rip)
  lock incl g_cen_cnt+284(%rip)
  jmp *g_cen_orig+568(%rip)
.globl cen_stub_72
cen_stub_72:
  movq %rdx, g_cen_rdx+576(%rip)
  lock incl g_cen_cnt+288(%rip)
  jmp *g_cen_orig+576(%rip)
.globl cen_stub_73
cen_stub_73:
  movq %rdx, g_cen_rdx+584(%rip)
  lock incl g_cen_cnt+292(%rip)
  jmp *g_cen_orig+584(%rip)
.globl cen_stub_74
cen_stub_74:
  movq %rdx, g_cen_rdx+592(%rip)
  lock incl g_cen_cnt+296(%rip)
  jmp *g_cen_orig+592(%rip)
.globl cen_stub_75
cen_stub_75:
  movq %rdx, g_cen_rdx+600(%rip)
  lock incl g_cen_cnt+300(%rip)
  jmp *g_cen_orig+600(%rip)
.globl cen_stub_76
cen_stub_76:
  movq %rdx, g_cen_rdx+608(%rip)
  lock incl g_cen_cnt+304(%rip)
  jmp *g_cen_orig+608(%rip)
.globl cen_stub_77
cen_stub_77:
  movq %rdx, g_cen_rdx+616(%rip)
  lock incl g_cen_cnt+308(%rip)
  jmp *g_cen_orig+616(%rip)
.globl cen_stub_78
cen_stub_78:
  movq %rdx, g_cen_rdx+624(%rip)
  lock incl g_cen_cnt+312(%rip)
  jmp *g_cen_orig+624(%rip)
.globl cen_stub_79
cen_stub_79:
  movq %rdx, g_cen_rdx+632(%rip)
  lock incl g_cen_cnt+316(%rip)
  jmp *g_cen_orig+632(%rip)
.globl cen_stub_80
cen_stub_80:
  movq %rdx, g_cen_rdx+640(%rip)
  lock incl g_cen_cnt+320(%rip)
  jmp *g_cen_orig+640(%rip)
.globl cen_stub_81
cen_stub_81:
  movq %rdx, g_cen_rdx+648(%rip)
  lock incl g_cen_cnt+324(%rip)
  jmp *g_cen_orig+648(%rip)
.globl cen_stub_82
cen_stub_82:
  movq %rdx, g_cen_rdx+656(%rip)
  lock incl g_cen_cnt+328(%rip)
  jmp *g_cen_orig+656(%rip)
.globl cen_stub_83
cen_stub_83:
  movq %rdx, g_cen_rdx+664(%rip)
  lock incl g_cen_cnt+332(%rip)
  jmp *g_cen_orig+664(%rip)
.globl cen_stub_84
cen_stub_84:
  movq %rdx, g_cen_rdx+672(%rip)
  lock incl g_cen_cnt+336(%rip)
  jmp *g_cen_orig+672(%rip)
.globl cen_stub_85
cen_stub_85:
  movq %rdx, g_cen_rdx+680(%rip)
  lock incl g_cen_cnt+340(%rip)
  jmp *g_cen_orig+680(%rip)
.globl cen_stub_86
cen_stub_86:
  movq %rdx, g_cen_rdx+688(%rip)
  lock incl g_cen_cnt+344(%rip)
  jmp *g_cen_orig+688(%rip)
.globl cen_stub_87
cen_stub_87:
  movq %rdx, g_cen_rdx+696(%rip)
  lock incl g_cen_cnt+348(%rip)
  jmp *g_cen_orig+696(%rip)
.globl cen_stub_88
cen_stub_88:
  movq %rdx, g_cen_rdx+704(%rip)
  lock incl g_cen_cnt+352(%rip)
  jmp *g_cen_orig+704(%rip)
.globl cen_stub_89
cen_stub_89:
  movq %rdx, g_cen_rdx+712(%rip)
  lock incl g_cen_cnt+356(%rip)
  jmp *g_cen_orig+712(%rip)
.globl cen_stub_90
cen_stub_90:
  movq %rdx, g_cen_rdx+720(%rip)
  lock incl g_cen_cnt+360(%rip)
  jmp *g_cen_orig+720(%rip)
.globl cen_stub_91
cen_stub_91:
  movq %rdx, g_cen_rdx+728(%rip)
  lock incl g_cen_cnt+364(%rip)
  jmp *g_cen_orig+728(%rip)
.globl cen_stub_92
cen_stub_92:
  movq %rdx, g_cen_rdx+736(%rip)
  lock incl g_cen_cnt+368(%rip)
  jmp *g_cen_orig+736(%rip)
.globl cen_stub_93
cen_stub_93:
  movq %rdx, g_cen_rdx+744(%rip)
  lock incl g_cen_cnt+372(%rip)
  jmp *g_cen_orig+744(%rip)
.globl cen_stub_94
cen_stub_94:
  movq %rdx, g_cen_rdx+752(%rip)
  lock incl g_cen_cnt+376(%rip)
  jmp *g_cen_orig+752(%rip)
.globl cen_stub_95
cen_stub_95:
  movq %rdx, g_cen_rdx+760(%rip)
  lock incl g_cen_cnt+380(%rip)
  jmp *g_cen_orig+760(%rip)
.globl cen_stub_96
cen_stub_96:
  movq %rdx, g_cen_rdx+768(%rip)
  lock incl g_cen_cnt+384(%rip)
  jmp *g_cen_orig+768(%rip)
.globl cen_stub_97
cen_stub_97:
  movq %rdx, g_cen_rdx+776(%rip)
  lock incl g_cen_cnt+388(%rip)
  jmp *g_cen_orig+776(%rip)
.globl cen_stub_98
cen_stub_98:
  movq %rdx, g_cen_rdx+784(%rip)
  lock incl g_cen_cnt+392(%rip)
  jmp *g_cen_orig+784(%rip)
.globl cen_stub_99
cen_stub_99:
  movq %rdx, g_cen_rdx+792(%rip)
  lock incl g_cen_cnt+396(%rip)
  jmp *g_cen_orig+792(%rip)
.globl cen_stub_100
cen_stub_100:
  movq %rdx, g_cen_rdx+800(%rip)
  lock incl g_cen_cnt+400(%rip)
  jmp *g_cen_orig+800(%rip)
.globl cen_stub_101
cen_stub_101:
  movq %rdx, g_cen_rdx+808(%rip)
  lock incl g_cen_cnt+404(%rip)
  jmp *g_cen_orig+808(%rip)
.globl cen_stub_102
cen_stub_102:
  movq %rdx, g_cen_rdx+816(%rip)
  lock incl g_cen_cnt+408(%rip)
  jmp *g_cen_orig+816(%rip)
.globl cen_stub_103
cen_stub_103:
  movq %rdx, g_cen_rdx+824(%rip)
  lock incl g_cen_cnt+412(%rip)
  jmp *g_cen_orig+824(%rip)
.globl cen_stub_104
cen_stub_104:
  movq %rdx, g_cen_rdx+832(%rip)
  lock incl g_cen_cnt+416(%rip)
  jmp *g_cen_orig+832(%rip)
.globl cen_stub_105
cen_stub_105:
  movq %rdx, g_cen_rdx+840(%rip)
  lock incl g_cen_cnt+420(%rip)
  jmp *g_cen_orig+840(%rip)
.globl cen_stub_106
cen_stub_106:
  movq %rdx, g_cen_rdx+848(%rip)
  lock incl g_cen_cnt+424(%rip)
  jmp *g_cen_orig+848(%rip)
.globl cen_stub_107
cen_stub_107:
  movq %rdx, g_cen_rdx+856(%rip)
  lock incl g_cen_cnt+428(%rip)
  jmp *g_cen_orig+856(%rip)
.globl cen_stub_108
cen_stub_108:
  movq %rdx, g_cen_rdx+864(%rip)
  lock incl g_cen_cnt+432(%rip)
  jmp *g_cen_orig+864(%rip)
.globl cen_stub_109
cen_stub_109:
  movq %rdx, g_cen_rdx+872(%rip)
  lock incl g_cen_cnt+436(%rip)
  jmp *g_cen_orig+872(%rip)
.globl cen_stub_110
cen_stub_110:
  movq %rdx, g_cen_rdx+880(%rip)
  lock incl g_cen_cnt+440(%rip)
  jmp *g_cen_orig+880(%rip)
.globl cen_stub_111
cen_stub_111:
  movq %rdx, g_cen_rdx+888(%rip)
  lock incl g_cen_cnt+444(%rip)
  jmp *g_cen_orig+888(%rip)
.globl cen_stub_112
cen_stub_112:
  movq %rdx, g_cen_rdx+896(%rip)
  lock incl g_cen_cnt+448(%rip)
  jmp *g_cen_orig+896(%rip)
.globl cen_stub_113
cen_stub_113:
  movq %rdx, g_cen_rdx+904(%rip)
  lock incl g_cen_cnt+452(%rip)
  jmp *g_cen_orig+904(%rip)
.globl cen_stub_114
cen_stub_114:
  movq %rdx, g_cen_rdx+912(%rip)
  lock incl g_cen_cnt+456(%rip)
  jmp *g_cen_orig+912(%rip)
.globl cen_stub_115
cen_stub_115:
  movq %rdx, g_cen_rdx+920(%rip)
  lock incl g_cen_cnt+460(%rip)
  jmp *g_cen_orig+920(%rip)
.globl cen_stub_116
cen_stub_116:
  movq %rdx, g_cen_rdx+928(%rip)
  lock incl g_cen_cnt+464(%rip)
  jmp *g_cen_orig+928(%rip)
.globl cen_stub_117
cen_stub_117:
  movq %rdx, g_cen_rdx+936(%rip)
  lock incl g_cen_cnt+468(%rip)
  jmp *g_cen_orig+936(%rip)
.globl cen_stub_118
cen_stub_118:
  movq %rdx, g_cen_rdx+944(%rip)
  lock incl g_cen_cnt+472(%rip)
  jmp *g_cen_orig+944(%rip)
.globl cen_stub_119
cen_stub_119:
  movq %rdx, g_cen_rdx+952(%rip)
  lock incl g_cen_cnt+476(%rip)
  jmp *g_cen_orig+952(%rip)
.globl cen_stub_120
cen_stub_120:
  movq %rdx, g_cen_rdx+960(%rip)
  lock incl g_cen_cnt+480(%rip)
  jmp *g_cen_orig+960(%rip)
.globl cen_stub_121
cen_stub_121:
  movq %rdx, g_cen_rdx+968(%rip)
  lock incl g_cen_cnt+484(%rip)
  jmp *g_cen_orig+968(%rip)
.globl cen_stub_122
cen_stub_122:
  movq %rdx, g_cen_rdx+976(%rip)
  lock incl g_cen_cnt+488(%rip)
  jmp *g_cen_orig+976(%rip)
.globl cen_stub_123
cen_stub_123:
  movq %rdx, g_cen_rdx+984(%rip)
  lock incl g_cen_cnt+492(%rip)
  jmp *g_cen_orig+984(%rip)
.globl cen_stub_124
cen_stub_124:
  movq %rdx, g_cen_rdx+992(%rip)
  lock incl g_cen_cnt+496(%rip)
  jmp *g_cen_orig+992(%rip)
.globl cen_stub_125
cen_stub_125:
  movq %rdx, g_cen_rdx+1000(%rip)
  lock incl g_cen_cnt+500(%rip)
  jmp *g_cen_orig+1000(%rip)
.globl cen_stub_126
cen_stub_126:
  movq %rdx, g_cen_rdx+1008(%rip)
  lock incl g_cen_cnt+504(%rip)
  jmp *g_cen_orig+1008(%rip)
.globl cen_stub_127
cen_stub_127:
  movq %rdx, g_cen_rdx+1016(%rip)
  lock incl g_cen_cnt+508(%rip)
  jmp *g_cen_orig+1016(%rip)
.globl cen_stub_128
cen_stub_128:
  movq %rdx, g_cen_rdx+1024(%rip)
  lock incl g_cen_cnt+512(%rip)
  jmp *g_cen_orig+1024(%rip)
.globl cen_stub_129
cen_stub_129:
  movq %rdx, g_cen_rdx+1032(%rip)
  lock incl g_cen_cnt+516(%rip)
  jmp *g_cen_orig+1032(%rip)
.globl cen_stub_130
cen_stub_130:
  movq %rdx, g_cen_rdx+1040(%rip)
  lock incl g_cen_cnt+520(%rip)
  jmp *g_cen_orig+1040(%rip)
.globl cen_stub_131
cen_stub_131:
  movq %rdx, g_cen_rdx+1048(%rip)
  lock incl g_cen_cnt+524(%rip)
  jmp *g_cen_orig+1048(%rip)
.globl cen_stub_132
cen_stub_132:
  movq %rdx, g_cen_rdx+1056(%rip)
  lock incl g_cen_cnt+528(%rip)
  jmp *g_cen_orig+1056(%rip)
.globl cen_stub_133
cen_stub_133:
  movq %rdx, g_cen_rdx+1064(%rip)
  lock incl g_cen_cnt+532(%rip)
  jmp *g_cen_orig+1064(%rip)
.globl cen_stub_134
cen_stub_134:
  movq %rdx, g_cen_rdx+1072(%rip)
  lock incl g_cen_cnt+536(%rip)
  jmp *g_cen_orig+1072(%rip)
.globl cen_stub_135
cen_stub_135:
  movq %rdx, g_cen_rdx+1080(%rip)
  lock incl g_cen_cnt+540(%rip)
  jmp *g_cen_orig+1080(%rip)
.globl cen_stub_136
cen_stub_136:
  movq %rdx, g_cen_rdx+1088(%rip)
  lock incl g_cen_cnt+544(%rip)
  jmp *g_cen_orig+1088(%rip)
.globl cen_stub_137
cen_stub_137:
  movq %rdx, g_cen_rdx+1096(%rip)
  lock incl g_cen_cnt+548(%rip)
  jmp *g_cen_orig+1096(%rip)
.globl cen_stub_138
cen_stub_138:
  movq %rdx, g_cen_rdx+1104(%rip)
  lock incl g_cen_cnt+552(%rip)
  jmp *g_cen_orig+1104(%rip)
.globl cen_stub_139
cen_stub_139:
  movq %rdx, g_cen_rdx+1112(%rip)
  lock incl g_cen_cnt+556(%rip)
  jmp *g_cen_orig+1112(%rip)
.globl cen_stub_140
cen_stub_140:
  movq %rdx, g_cen_rdx+1120(%rip)
  lock incl g_cen_cnt+560(%rip)
  jmp *g_cen_orig+1120(%rip)
.globl cen_stub_141
cen_stub_141:
  movq %rdx, g_cen_rdx+1128(%rip)
  lock incl g_cen_cnt+564(%rip)
  jmp *g_cen_orig+1128(%rip)
.globl cen_stub_142
cen_stub_142:
  movq %rdx, g_cen_rdx+1136(%rip)
  lock incl g_cen_cnt+568(%rip)
  jmp *g_cen_orig+1136(%rip)
.globl cen_stub_143
cen_stub_143:
  movq %rdx, g_cen_rdx+1144(%rip)
  lock incl g_cen_cnt+572(%rip)
  jmp *g_cen_orig+1144(%rip)
.globl cen_stub_144
cen_stub_144:
  movq %rdx, g_cen_rdx+1152(%rip)
  lock incl g_cen_cnt+576(%rip)
  jmp *g_cen_orig+1152(%rip)
.globl cen_stub_145
cen_stub_145:
  movq %rdx, g_cen_rdx+1160(%rip)
  lock incl g_cen_cnt+580(%rip)
  jmp *g_cen_orig+1160(%rip)
.globl cen_stub_146
cen_stub_146:
  movq %rdx, g_cen_rdx+1168(%rip)
  lock incl g_cen_cnt+584(%rip)
  jmp *g_cen_orig+1168(%rip)
.globl cen_stub_147
cen_stub_147:
  movq %rdx, g_cen_rdx+1176(%rip)
  lock incl g_cen_cnt+588(%rip)
  jmp *g_cen_orig+1176(%rip)
.globl cen_stub_148
cen_stub_148:
  movq %rdx, g_cen_rdx+1184(%rip)
  lock incl g_cen_cnt+592(%rip)
  jmp *g_cen_orig+1184(%rip)
.globl cen_stub_149
cen_stub_149:
  movq %rdx, g_cen_rdx+1192(%rip)
  lock incl g_cen_cnt+596(%rip)
  jmp *g_cen_orig+1192(%rip)
.globl cen_stub_150
cen_stub_150:
  movq %rdx, g_cen_rdx+1200(%rip)
  lock incl g_cen_cnt+600(%rip)
  jmp *g_cen_orig+1200(%rip)
.globl cen_stub_151
cen_stub_151:
  movq %rdx, g_cen_rdx+1208(%rip)
  lock incl g_cen_cnt+604(%rip)
  jmp *g_cen_orig+1208(%rip)
.globl cen_stub_152
cen_stub_152:
  movq %rdx, g_cen_rdx+1216(%rip)
  lock incl g_cen_cnt+608(%rip)
  jmp *g_cen_orig+1216(%rip)
.globl cen_stub_153
cen_stub_153:
  movq %rdx, g_cen_rdx+1224(%rip)
  lock incl g_cen_cnt+612(%rip)
  jmp *g_cen_orig+1224(%rip)
.globl cen_stub_154
cen_stub_154:
  movq %rdx, g_cen_rdx+1232(%rip)
  lock incl g_cen_cnt+616(%rip)
  jmp *g_cen_orig+1232(%rip)
.globl cen_stub_155
cen_stub_155:
  movq %rdx, g_cen_rdx+1240(%rip)
  lock incl g_cen_cnt+620(%rip)
  jmp *g_cen_orig+1240(%rip)
.globl cen_stub_156
cen_stub_156:
  movq %rdx, g_cen_rdx+1248(%rip)
  lock incl g_cen_cnt+624(%rip)
  jmp *g_cen_orig+1248(%rip)
.globl cen_stub_157
cen_stub_157:
  movq %rdx, g_cen_rdx+1256(%rip)
  lock incl g_cen_cnt+628(%rip)
  jmp *g_cen_orig+1256(%rip)
.globl cen_stub_158
cen_stub_158:
  movq %rdx, g_cen_rdx+1264(%rip)
  lock incl g_cen_cnt+632(%rip)
  jmp *g_cen_orig+1264(%rip)
.globl cen_stub_159
cen_stub_159:
  movq %rdx, g_cen_rdx+1272(%rip)
  lock incl g_cen_cnt+636(%rip)
  jmp *g_cen_orig+1272(%rip)
.globl cen_stub_160
cen_stub_160:
  movq %rdx, g_cen_rdx+1280(%rip)
  lock incl g_cen_cnt+640(%rip)
  jmp *g_cen_orig+1280(%rip)
.globl cen_stub_161
cen_stub_161:
  movq %rdx, g_cen_rdx+1288(%rip)
  lock incl g_cen_cnt+644(%rip)
  jmp *g_cen_orig+1288(%rip)
.globl cen_stub_162
cen_stub_162:
  movq %rdx, g_cen_rdx+1296(%rip)
  lock incl g_cen_cnt+648(%rip)
  jmp *g_cen_orig+1296(%rip)
.globl cen_stub_163
cen_stub_163:
  movq %rdx, g_cen_rdx+1304(%rip)
  lock incl g_cen_cnt+652(%rip)
  jmp *g_cen_orig+1304(%rip)
.globl cen_stub_164
cen_stub_164:
  movq %rdx, g_cen_rdx+1312(%rip)
  lock incl g_cen_cnt+656(%rip)
  jmp *g_cen_orig+1312(%rip)
.globl cen_stub_165
cen_stub_165:
  movq %rdx, g_cen_rdx+1320(%rip)
  lock incl g_cen_cnt+660(%rip)
  jmp *g_cen_orig+1320(%rip)
.globl cen_stub_166
cen_stub_166:
  movq %rdx, g_cen_rdx+1328(%rip)
  lock incl g_cen_cnt+664(%rip)
  jmp *g_cen_orig+1328(%rip)
.globl cen_stub_167
cen_stub_167:
  movq %rdx, g_cen_rdx+1336(%rip)
  lock incl g_cen_cnt+668(%rip)
  jmp *g_cen_orig+1336(%rip)
.globl cen_stub_168
cen_stub_168:
  movq %rdx, g_cen_rdx+1344(%rip)
  lock incl g_cen_cnt+672(%rip)
  jmp *g_cen_orig+1344(%rip)
.globl cen_stub_169
cen_stub_169:
  movq %rdx, g_cen_rdx+1352(%rip)
  lock incl g_cen_cnt+676(%rip)
  jmp *g_cen_orig+1352(%rip)
.globl cen_stub_170
cen_stub_170:
  movq %rdx, g_cen_rdx+1360(%rip)
  lock incl g_cen_cnt+680(%rip)
  jmp *g_cen_orig+1360(%rip)
.globl cen_stub_171
cen_stub_171:
  movq %rdx, g_cen_rdx+1368(%rip)
  lock incl g_cen_cnt+684(%rip)
  jmp *g_cen_orig+1368(%rip)
.globl cen_stub_172
cen_stub_172:
  movq %rdx, g_cen_rdx+1376(%rip)
  lock incl g_cen_cnt+688(%rip)
  jmp *g_cen_orig+1376(%rip)
.globl cen_stub_173
cen_stub_173:
  movq %rdx, g_cen_rdx+1384(%rip)
  lock incl g_cen_cnt+692(%rip)
  jmp *g_cen_orig+1384(%rip)
.globl cen_stub_174
cen_stub_174:
  movq %rdx, g_cen_rdx+1392(%rip)
  lock incl g_cen_cnt+696(%rip)
  jmp *g_cen_orig+1392(%rip)
.globl cen_stub_175
cen_stub_175:
  movq %rdx, g_cen_rdx+1400(%rip)
  lock incl g_cen_cnt+700(%rip)
  jmp *g_cen_orig+1400(%rip)
.globl cen_stub_176
cen_stub_176:
  movq %rdx, g_cen_rdx+1408(%rip)
  lock incl g_cen_cnt+704(%rip)
  jmp *g_cen_orig+1408(%rip)
.globl cen_stub_177
cen_stub_177:
  movq %rdx, g_cen_rdx+1416(%rip)
  lock incl g_cen_cnt+708(%rip)
  jmp *g_cen_orig+1416(%rip)
.globl cen_stub_178
cen_stub_178:
  movq %rdx, g_cen_rdx+1424(%rip)
  lock incl g_cen_cnt+712(%rip)
  jmp *g_cen_orig+1424(%rip)
.globl cen_stub_179
cen_stub_179:
  movq %rdx, g_cen_rdx+1432(%rip)
  lock incl g_cen_cnt+716(%rip)
  jmp *g_cen_orig+1432(%rip)
.globl cen_stub_180
cen_stub_180:
  movq %rdx, g_cen_rdx+1440(%rip)
  lock incl g_cen_cnt+720(%rip)
  jmp *g_cen_orig+1440(%rip)
.globl cen_stub_181
cen_stub_181:
  movq %rdx, g_cen_rdx+1448(%rip)
  lock incl g_cen_cnt+724(%rip)
  jmp *g_cen_orig+1448(%rip)
.globl cen_stub_182
cen_stub_182:
  movq %rdx, g_cen_rdx+1456(%rip)
  lock incl g_cen_cnt+728(%rip)
  jmp *g_cen_orig+1456(%rip)
.globl cen_stub_183
cen_stub_183:
  movq %rdx, g_cen_rdx+1464(%rip)
  lock incl g_cen_cnt+732(%rip)
  jmp *g_cen_orig+1464(%rip)
.globl cen_stub_184
cen_stub_184:
  movq %rdx, g_cen_rdx+1472(%rip)
  lock incl g_cen_cnt+736(%rip)
  jmp *g_cen_orig+1472(%rip)
.globl cen_stub_185
cen_stub_185:
  movq %rdx, g_cen_rdx+1480(%rip)
  lock incl g_cen_cnt+740(%rip)
  jmp *g_cen_orig+1480(%rip)
.globl cen_stub_186
cen_stub_186:
  movq %rdx, g_cen_rdx+1488(%rip)
  lock incl g_cen_cnt+744(%rip)
  jmp *g_cen_orig+1488(%rip)
.globl cen_stub_187
cen_stub_187:
  movq %rdx, g_cen_rdx+1496(%rip)
  lock incl g_cen_cnt+748(%rip)
  jmp *g_cen_orig+1496(%rip)
.globl cen_stub_188
cen_stub_188:
  movq %rdx, g_cen_rdx+1504(%rip)
  lock incl g_cen_cnt+752(%rip)
  jmp *g_cen_orig+1504(%rip)
.globl cen_stub_189
cen_stub_189:
  movq %rdx, g_cen_rdx+1512(%rip)
  lock incl g_cen_cnt+756(%rip)
  jmp *g_cen_orig+1512(%rip)
.globl cen_stub_190
cen_stub_190:
  movq %rdx, g_cen_rdx+1520(%rip)
  lock incl g_cen_cnt+760(%rip)
  jmp *g_cen_orig+1520(%rip)
.globl cen_stub_191
cen_stub_191:
  movq %rdx, g_cen_rdx+1528(%rip)
  lock incl g_cen_cnt+764(%rip)
  jmp *g_cen_orig+1528(%rip)
.globl cen_stub_192
cen_stub_192:
  movq %rdx, g_cen_rdx+1536(%rip)
  lock incl g_cen_cnt+768(%rip)
  jmp *g_cen_orig+1536(%rip)
.globl cen_stub_193
cen_stub_193:
  movq %rdx, g_cen_rdx+1544(%rip)
  lock incl g_cen_cnt+772(%rip)
  jmp *g_cen_orig+1544(%rip)
.globl cen_stub_194
cen_stub_194:
  movq %rdx, g_cen_rdx+1552(%rip)
  lock incl g_cen_cnt+776(%rip)
  jmp *g_cen_orig+1552(%rip)
.globl cen_stub_195
cen_stub_195:
  movq %rdx, g_cen_rdx+1560(%rip)
  lock incl g_cen_cnt+780(%rip)
  jmp *g_cen_orig+1560(%rip)
.globl cen_stub_196
cen_stub_196:
  movq %rdx, g_cen_rdx+1568(%rip)
  lock incl g_cen_cnt+784(%rip)
  jmp *g_cen_orig+1568(%rip)
.globl cen_stub_197
cen_stub_197:
  movq %rdx, g_cen_rdx+1576(%rip)
  lock incl g_cen_cnt+788(%rip)
  jmp *g_cen_orig+1576(%rip)
.globl cen_stub_198
cen_stub_198:
  movq %rdx, g_cen_rdx+1584(%rip)
  lock incl g_cen_cnt+792(%rip)
  jmp *g_cen_orig+1584(%rip)
.globl cen_stub_199
cen_stub_199:
  movq %rdx, g_cen_rdx+1592(%rip)
  lock incl g_cen_cnt+796(%rip)
  jmp *g_cen_orig+1592(%rip)
.globl cen_stub_200
cen_stub_200:
  movq %rdx, g_cen_rdx+1600(%rip)
  lock incl g_cen_cnt+800(%rip)
  jmp *g_cen_orig+1600(%rip)
.globl cen_stub_201
cen_stub_201:
  movq %rdx, g_cen_rdx+1608(%rip)
  lock incl g_cen_cnt+804(%rip)
  jmp *g_cen_orig+1608(%rip)
.globl cen_stub_202
cen_stub_202:
  movq %rdx, g_cen_rdx+1616(%rip)
  lock incl g_cen_cnt+808(%rip)
  jmp *g_cen_orig+1616(%rip)
.globl cen_stub_203
cen_stub_203:
  movq %rdx, g_cen_rdx+1624(%rip)
  lock incl g_cen_cnt+812(%rip)
  jmp *g_cen_orig+1624(%rip)
.globl cen_stub_204
cen_stub_204:
  movq %rdx, g_cen_rdx+1632(%rip)
  lock incl g_cen_cnt+816(%rip)
  jmp *g_cen_orig+1632(%rip)
.globl cen_stub_205
cen_stub_205:
  movq %rdx, g_cen_rdx+1640(%rip)
  lock incl g_cen_cnt+820(%rip)
  jmp *g_cen_orig+1640(%rip)
.globl cen_stub_206
cen_stub_206:
  movq %rdx, g_cen_rdx+1648(%rip)
  lock incl g_cen_cnt+824(%rip)
  jmp *g_cen_orig+1648(%rip)
.globl cen_stub_207
cen_stub_207:
  movq %rdx, g_cen_rdx+1656(%rip)
  lock incl g_cen_cnt+828(%rip)
  jmp *g_cen_orig+1656(%rip)
.globl cen_stub_208
cen_stub_208:
  movq %rdx, g_cen_rdx+1664(%rip)
  lock incl g_cen_cnt+832(%rip)
  jmp *g_cen_orig+1664(%rip)
.globl cen_stub_209
cen_stub_209:
  movq %rdx, g_cen_rdx+1672(%rip)
  lock incl g_cen_cnt+836(%rip)
  jmp *g_cen_orig+1672(%rip)
.globl cen_stub_210
cen_stub_210:
  movq %rdx, g_cen_rdx+1680(%rip)
  lock incl g_cen_cnt+840(%rip)
  jmp *g_cen_orig+1680(%rip)
.globl cen_stub_211
cen_stub_211:
  movq %rdx, g_cen_rdx+1688(%rip)
  lock incl g_cen_cnt+844(%rip)
  jmp *g_cen_orig+1688(%rip)
.globl cen_stub_212
cen_stub_212:
  movq %rdx, g_cen_rdx+1696(%rip)
  lock incl g_cen_cnt+848(%rip)
  jmp *g_cen_orig+1696(%rip)
.globl cen_stub_213
cen_stub_213:
  movq %rdx, g_cen_rdx+1704(%rip)
  lock incl g_cen_cnt+852(%rip)
  jmp *g_cen_orig+1704(%rip)
.globl cen_stub_214
cen_stub_214:
  movq %rdx, g_cen_rdx+1712(%rip)
  lock incl g_cen_cnt+856(%rip)
  jmp *g_cen_orig+1712(%rip)
.globl cen_stub_215
cen_stub_215:
  movq %rdx, g_cen_rdx+1720(%rip)
  lock incl g_cen_cnt+860(%rip)
  jmp *g_cen_orig+1720(%rip)
.globl cen_stub_216
cen_stub_216:
  movq %rdx, g_cen_rdx+1728(%rip)
  lock incl g_cen_cnt+864(%rip)
  jmp *g_cen_orig+1728(%rip)
.globl cen_stub_217
cen_stub_217:
  movq %rdx, g_cen_rdx+1736(%rip)
  lock incl g_cen_cnt+868(%rip)
  jmp *g_cen_orig+1736(%rip)
.globl cen_stub_218
cen_stub_218:
  movq %rdx, g_cen_rdx+1744(%rip)
  lock incl g_cen_cnt+872(%rip)
  jmp *g_cen_orig+1744(%rip)
.globl cen_stub_219
cen_stub_219:
  movq %rdx, g_cen_rdx+1752(%rip)
  lock incl g_cen_cnt+876(%rip)
  jmp *g_cen_orig+1752(%rip)
.globl cen_stub_220
cen_stub_220:
  movq %rdx, g_cen_rdx+1760(%rip)
  lock incl g_cen_cnt+880(%rip)
  jmp *g_cen_orig+1760(%rip)
.globl cen_stub_221
cen_stub_221:
  movq %rdx, g_cen_rdx+1768(%rip)
  lock incl g_cen_cnt+884(%rip)
  jmp *g_cen_orig+1768(%rip)
.globl cen_stub_222
cen_stub_222:
  movq %rdx, g_cen_rdx+1776(%rip)
  lock incl g_cen_cnt+888(%rip)
  jmp *g_cen_orig+1776(%rip)
.globl cen_stub_223
cen_stub_223:
  movq %rdx, g_cen_rdx+1784(%rip)
  lock incl g_cen_cnt+892(%rip)
  jmp *g_cen_orig+1784(%rip)
.globl cen_stub_224
cen_stub_224:
  movq %rdx, g_cen_rdx+1792(%rip)
  lock incl g_cen_cnt+896(%rip)
  jmp *g_cen_orig+1792(%rip)
.globl cen_stub_225
cen_stub_225:
  movq %rdx, g_cen_rdx+1800(%rip)
  lock incl g_cen_cnt+900(%rip)
  jmp *g_cen_orig+1800(%rip)
.globl cen_stub_226
cen_stub_226:
  movq %rdx, g_cen_rdx+1808(%rip)
  lock incl g_cen_cnt+904(%rip)
  jmp *g_cen_orig+1808(%rip)
.globl cen_stub_227
cen_stub_227:
  movq %rdx, g_cen_rdx+1816(%rip)
  lock incl g_cen_cnt+908(%rip)
  jmp *g_cen_orig+1816(%rip)
.globl cen_stub_228
cen_stub_228:
  movq %rdx, g_cen_rdx+1824(%rip)
  lock incl g_cen_cnt+912(%rip)
  jmp *g_cen_orig+1824(%rip)
.globl cen_stub_229
cen_stub_229:
  movq %rdx, g_cen_rdx+1832(%rip)
  lock incl g_cen_cnt+916(%rip)
  jmp *g_cen_orig+1832(%rip)
.globl cen_stub_230
cen_stub_230:
  movq %rdx, g_cen_rdx+1840(%rip)
  lock incl g_cen_cnt+920(%rip)
  jmp *g_cen_orig+1840(%rip)
.globl cen_stub_231
cen_stub_231:
  movq %rdx, g_cen_rdx+1848(%rip)
  lock incl g_cen_cnt+924(%rip)
  jmp *g_cen_orig+1848(%rip)
.globl cen_stub_232
cen_stub_232:
  movq %rdx, g_cen_rdx+1856(%rip)
  lock incl g_cen_cnt+928(%rip)
  jmp *g_cen_orig+1856(%rip)
.globl cen_stub_233
cen_stub_233:
  movq %rdx, g_cen_rdx+1864(%rip)
  lock incl g_cen_cnt+932(%rip)
  jmp *g_cen_orig+1864(%rip)
.globl cen_stub_234
cen_stub_234:
  movq %rdx, g_cen_rdx+1872(%rip)
  lock incl g_cen_cnt+936(%rip)
  jmp *g_cen_orig+1872(%rip)
.globl cen_stub_235
cen_stub_235:
  movq %rdx, g_cen_rdx+1880(%rip)
  lock incl g_cen_cnt+940(%rip)
  jmp *g_cen_orig+1880(%rip)
.globl cen_stub_236
cen_stub_236:
  movq %rdx, g_cen_rdx+1888(%rip)
  lock incl g_cen_cnt+944(%rip)
  jmp *g_cen_orig+1888(%rip)
.globl cen_stub_237
cen_stub_237:
  movq %rdx, g_cen_rdx+1896(%rip)
  lock incl g_cen_cnt+948(%rip)
  jmp *g_cen_orig+1896(%rip)
.globl cen_stub_238
cen_stub_238:
  movq %rdx, g_cen_rdx+1904(%rip)
  lock incl g_cen_cnt+952(%rip)
  jmp *g_cen_orig+1904(%rip)
.globl cen_stub_239
cen_stub_239:
  movq %rdx, g_cen_rdx+1912(%rip)
  lock incl g_cen_cnt+956(%rip)
  jmp *g_cen_orig+1912(%rip)
.globl cen_stub_240
cen_stub_240:
  movq %rdx, g_cen_rdx+1920(%rip)
  lock incl g_cen_cnt+960(%rip)
  jmp *g_cen_orig+1920(%rip)
.globl cen_stub_241
cen_stub_241:
  movq %rdx, g_cen_rdx+1928(%rip)
  lock incl g_cen_cnt+964(%rip)
  jmp *g_cen_orig+1928(%rip)
.globl cen_stub_242
cen_stub_242:
  movq %rdx, g_cen_rdx+1936(%rip)
  lock incl g_cen_cnt+968(%rip)
  jmp *g_cen_orig+1936(%rip)
.globl cen_stub_243
cen_stub_243:
  movq %rdx, g_cen_rdx+1944(%rip)
  lock incl g_cen_cnt+972(%rip)
  jmp *g_cen_orig+1944(%rip)
.globl cen_stub_244
cen_stub_244:
  movq %rdx, g_cen_rdx+1952(%rip)
  lock incl g_cen_cnt+976(%rip)
  jmp *g_cen_orig+1952(%rip)
.globl cen_stub_245
cen_stub_245:
  movq %rdx, g_cen_rdx+1960(%rip)
  lock incl g_cen_cnt+980(%rip)
  jmp *g_cen_orig+1960(%rip)
.globl cen_stub_246
cen_stub_246:
  movq %rdx, g_cen_rdx+1968(%rip)
  lock incl g_cen_cnt+984(%rip)
  jmp *g_cen_orig+1968(%rip)
.globl cen_stub_247
cen_stub_247:
  movq %rdx, g_cen_rdx+1976(%rip)
  lock incl g_cen_cnt+988(%rip)
  jmp *g_cen_orig+1976(%rip)
.globl cen_stub_248
cen_stub_248:
  movq %rdx, g_cen_rdx+1984(%rip)
  lock incl g_cen_cnt+992(%rip)
  jmp *g_cen_orig+1984(%rip)
.globl cen_stub_249
cen_stub_249:
  movq %rdx, g_cen_rdx+1992(%rip)
  lock incl g_cen_cnt+996(%rip)
  jmp *g_cen_orig+1992(%rip)
.globl cen_stub_250
cen_stub_250:
  movq %rdx, g_cen_rdx+2000(%rip)
  lock incl g_cen_cnt+1000(%rip)
  jmp *g_cen_orig+2000(%rip)
.globl cen_stub_251
cen_stub_251:
  movq %rdx, g_cen_rdx+2008(%rip)
  lock incl g_cen_cnt+1004(%rip)
  jmp *g_cen_orig+2008(%rip)
.globl cen_stub_252
cen_stub_252:
  movq %rdx, g_cen_rdx+2016(%rip)
  lock incl g_cen_cnt+1008(%rip)
  jmp *g_cen_orig+2016(%rip)
.globl cen_stub_253
cen_stub_253:
  movq %rdx, g_cen_rdx+2024(%rip)
  lock incl g_cen_cnt+1012(%rip)
  jmp *g_cen_orig+2024(%rip)
.globl cen_stub_254
cen_stub_254:
  movq %rdx, g_cen_rdx+2032(%rip)
  lock incl g_cen_cnt+1016(%rip)
  jmp *g_cen_orig+2032(%rip)
.globl cen_stub_255
cen_stub_255:
  movq %rdx, g_cen_rdx+2040(%rip)
  lock incl g_cen_cnt+1020(%rip)
  jmp *g_cen_orig+2040(%rip)
.globl ak_stub_0
ak_stub_0:
  push %rcx
  push %rdx
  push %r8
  push %r9
  sub $0x28, %rsp
  mov %rdx, %r8
  mov %rcx, %rdx
  mov $0, %ecx
  call ak_log
  add $0x28, %rsp
  pop %r9
  pop %r8
  pop %rdx
  pop %rcx
  jmp *g_ak_orig+0(%rip)
.globl ak_stub_1
ak_stub_1:
  push %rcx
  push %rdx
  push %r8
  push %r9
  sub $0x28, %rsp
  mov %rdx, %r8
  mov %rcx, %rdx
  mov $1, %ecx
  call ak_log
  add $0x28, %rsp
  pop %r9
  pop %r8
  pop %rdx
  pop %rcx
  jmp *g_ak_orig+8(%rip)
.globl ak_stub_2
ak_stub_2:
  push %rcx
  push %rdx
  push %r8
  push %r9
  sub $0x28, %rsp
  mov %rdx, %r8
  mov %rcx, %rdx
  mov $2, %ecx
  call ak_log
  add $0x28, %rsp
  pop %r9
  pop %r8
  pop %rdx
  pop %rcx
  jmp *g_ak_orig+16(%rip)
.globl ak_stub_3
ak_stub_3:
  push %rcx
  push %rdx
  push %r8
  push %r9
  sub $0x28, %rsp
  mov %rdx, %r8
  mov %rcx, %rdx
  mov $3, %ecx
  call ak_log
  add $0x28, %rsp
  pop %r9
  pop %r8
  pop %rdx
  pop %rcx
  jmp *g_ak_orig+24(%rip)
.globl ak_stub_4
ak_stub_4:
  push %rcx
  push %rdx
  push %r8
  push %r9
  sub $0x28, %rsp
  mov %rdx, %r8
  mov %rcx, %rdx
  mov $4, %ecx
  call ak_log
  add $0x28, %rsp
  pop %r9
  pop %r8
  pop %rdx
  pop %rcx
  jmp *g_ak_orig+32(%rip)
.globl ak_stub_5
ak_stub_5:
  push %rcx
  push %rdx
  push %r8
  push %r9
  sub $0x28, %rsp
  mov %rdx, %r8
  mov %rcx, %rdx
  mov $5, %ecx
  call ak_log
  add $0x28, %rsp
  pop %r9
  pop %r8
  pop %rdx
  pop %rcx
  jmp *g_ak_orig+40(%rip)
.globl ak_stub_6
ak_stub_6:
  push %rcx
  push %rdx
  push %r8
  push %r9
  sub $0x28, %rsp
  mov %rdx, %r8
  mov %rcx, %rdx
  mov $6, %ecx
  call ak_log
  add $0x28, %rsp
  pop %r9
  pop %r8
  pop %rdx
  pop %rcx
  jmp *g_ak_orig+48(%rip)
.globl ak_stub_7
ak_stub_7:
  push %rcx
  push %rdx
  push %r8
  push %r9
  sub $0x28, %rsp
  mov %rdx, %r8
  mov %rcx, %rdx
  mov $7, %ecx
  call ak_log
  add $0x28, %rsp
  pop %r9
  pop %r8
  pop %rdx
  pop %rcx
  jmp *g_ak_orig+56(%rip)
.globl ak_stub_8
ak_stub_8:
  push %rcx
  push %rdx
  push %r8
  push %r9
  sub $0x28, %rsp
  mov %rdx, %r8
  mov %rcx, %rdx
  mov $8, %ecx
  call ak_log
  add $0x28, %rsp
  pop %r9
  pop %r8
  pop %rdx
  pop %rcx
  jmp *g_ak_orig+64(%rip)
.globl ak_stub_9
ak_stub_9:
  push %rcx
  push %rdx
  push %r8
  push %r9
  sub $0x28, %rsp
  mov %rdx, %r8
  mov %rcx, %rdx
  mov $9, %ecx
  call ak_log
  add $0x28, %rsp
  pop %r9
  pop %r8
  pop %rdx
  pop %rcx
  jmp *g_ak_orig+72(%rip)
.globl ak_stub_10
ak_stub_10:
  push %rcx
  push %rdx
  push %r8
  push %r9
  sub $0x28, %rsp
  mov %rdx, %r8
  mov %rcx, %rdx
  mov $10, %ecx
  call ak_log
  add $0x28, %rsp
  pop %r9
  pop %r8
  pop %rdx
  pop %rcx
  jmp *g_ak_orig+80(%rip)
.globl ak_stub_11
ak_stub_11:
  push %rcx
  push %rdx
  push %r8
  push %r9
  sub $0x28, %rsp
  mov %rdx, %r8
  mov %rcx, %rdx
  mov $11, %ecx
  call ak_log
  add $0x28, %rsp
  pop %r9
  pop %r8
  pop %rdx
  pop %rcx
  jmp *g_ak_orig+88(%rip)
.globl ak_stub_12
ak_stub_12:
  push %rcx
  push %rdx
  push %r8
  push %r9
  sub $0x28, %rsp
  mov %rdx, %r8
  mov %rcx, %rdx
  mov $12, %ecx
  call ak_log
  add $0x28, %rsp
  pop %r9
  pop %r8
  pop %rdx
  pop %rcx
  jmp *g_ak_orig+96(%rip)
.globl ak_stub_13
ak_stub_13:
  push %rcx
  push %rdx
  push %r8
  push %r9
  sub $0x28, %rsp
  mov %rdx, %r8
  mov %rcx, %rdx
  mov $13, %ecx
  call ak_log
  add $0x28, %rsp
  pop %r9
  pop %r8
  pop %rdx
  pop %rcx
  jmp *g_ak_orig+104(%rip)
.globl ak_stub_14
ak_stub_14:
  push %rcx
  push %rdx
  push %r8
  push %r9
  sub $0x28, %rsp
  mov %rdx, %r8
  mov %rcx, %rdx
  mov $14, %ecx
  call ak_log
  add $0x28, %rsp
  pop %r9
  pop %r8
  pop %rdx
  pop %rcx
  jmp *g_ak_orig+112(%rip)
.globl ak_stub_15
ak_stub_15:
  push %rcx
  push %rdx
  push %r8
  push %r9
  sub $0x28, %rsp
  mov %rdx, %r8
  mov %rcx, %rdx
  mov $15, %ecx
  call ak_log
  add $0x28, %rsp
  pop %r9
  pop %r8
  pop %rdx
  pop %rcx
  jmp *g_ak_orig+120(%rip)
.data
.globl cen_stub_table
.p2align 3
cen_stub_table:
  .quad cen_stub_0
  .quad cen_stub_1
  .quad cen_stub_2
  .quad cen_stub_3
  .quad cen_stub_4
  .quad cen_stub_5
  .quad cen_stub_6
  .quad cen_stub_7
  .quad cen_stub_8
  .quad cen_stub_9
  .quad cen_stub_10
  .quad cen_stub_11
  .quad cen_stub_12
  .quad cen_stub_13
  .quad cen_stub_14
  .quad cen_stub_15
  .quad cen_stub_16
  .quad cen_stub_17
  .quad cen_stub_18
  .quad cen_stub_19
  .quad cen_stub_20
  .quad cen_stub_21
  .quad cen_stub_22
  .quad cen_stub_23
  .quad cen_stub_24
  .quad cen_stub_25
  .quad cen_stub_26
  .quad cen_stub_27
  .quad cen_stub_28
  .quad cen_stub_29
  .quad cen_stub_30
  .quad cen_stub_31
  .quad cen_stub_32
  .quad cen_stub_33
  .quad cen_stub_34
  .quad cen_stub_35
  .quad cen_stub_36
  .quad cen_stub_37
  .quad cen_stub_38
  .quad cen_stub_39
  .quad cen_stub_40
  .quad cen_stub_41
  .quad cen_stub_42
  .quad cen_stub_43
  .quad cen_stub_44
  .quad cen_stub_45
  .quad cen_stub_46
  .quad cen_stub_47
  .quad cen_stub_48
  .quad cen_stub_49
  .quad cen_stub_50
  .quad cen_stub_51
  .quad cen_stub_52
  .quad cen_stub_53
  .quad cen_stub_54
  .quad cen_stub_55
  .quad cen_stub_56
  .quad cen_stub_57
  .quad cen_stub_58
  .quad cen_stub_59
  .quad cen_stub_60
  .quad cen_stub_61
  .quad cen_stub_62
  .quad cen_stub_63
  .quad cen_stub_64
  .quad cen_stub_65
  .quad cen_stub_66
  .quad cen_stub_67
  .quad cen_stub_68
  .quad cen_stub_69
  .quad cen_stub_70
  .quad cen_stub_71
  .quad cen_stub_72
  .quad cen_stub_73
  .quad cen_stub_74
  .quad cen_stub_75
  .quad cen_stub_76
  .quad cen_stub_77
  .quad cen_stub_78
  .quad cen_stub_79
  .quad cen_stub_80
  .quad cen_stub_81
  .quad cen_stub_82
  .quad cen_stub_83
  .quad cen_stub_84
  .quad cen_stub_85
  .quad cen_stub_86
  .quad cen_stub_87
  .quad cen_stub_88
  .quad cen_stub_89
  .quad cen_stub_90
  .quad cen_stub_91
  .quad cen_stub_92
  .quad cen_stub_93
  .quad cen_stub_94
  .quad cen_stub_95
  .quad cen_stub_96
  .quad cen_stub_97
  .quad cen_stub_98
  .quad cen_stub_99
  .quad cen_stub_100
  .quad cen_stub_101
  .quad cen_stub_102
  .quad cen_stub_103
  .quad cen_stub_104
  .quad cen_stub_105
  .quad cen_stub_106
  .quad cen_stub_107
  .quad cen_stub_108
  .quad cen_stub_109
  .quad cen_stub_110
  .quad cen_stub_111
  .quad cen_stub_112
  .quad cen_stub_113
  .quad cen_stub_114
  .quad cen_stub_115
  .quad cen_stub_116
  .quad cen_stub_117
  .quad cen_stub_118
  .quad cen_stub_119
  .quad cen_stub_120
  .quad cen_stub_121
  .quad cen_stub_122
  .quad cen_stub_123
  .quad cen_stub_124
  .quad cen_stub_125
  .quad cen_stub_126
  .quad cen_stub_127
  .quad cen_stub_128
  .quad cen_stub_129
  .quad cen_stub_130
  .quad cen_stub_131
  .quad cen_stub_132
  .quad cen_stub_133
  .quad cen_stub_134
  .quad cen_stub_135
  .quad cen_stub_136
  .quad cen_stub_137
  .quad cen_stub_138
  .quad cen_stub_139
  .quad cen_stub_140
  .quad cen_stub_141
  .quad cen_stub_142
  .quad cen_stub_143
  .quad cen_stub_144
  .quad cen_stub_145
  .quad cen_stub_146
  .quad cen_stub_147
  .quad cen_stub_148
  .quad cen_stub_149
  .quad cen_stub_150
  .quad cen_stub_151
  .quad cen_stub_152
  .quad cen_stub_153
  .quad cen_stub_154
  .quad cen_stub_155
  .quad cen_stub_156
  .quad cen_stub_157
  .quad cen_stub_158
  .quad cen_stub_159
  .quad cen_stub_160
  .quad cen_stub_161
  .quad cen_stub_162
  .quad cen_stub_163
  .quad cen_stub_164
  .quad cen_stub_165
  .quad cen_stub_166
  .quad cen_stub_167
  .quad cen_stub_168
  .quad cen_stub_169
  .quad cen_stub_170
  .quad cen_stub_171
  .quad cen_stub_172
  .quad cen_stub_173
  .quad cen_stub_174
  .quad cen_stub_175
  .quad cen_stub_176
  .quad cen_stub_177
  .quad cen_stub_178
  .quad cen_stub_179
  .quad cen_stub_180
  .quad cen_stub_181
  .quad cen_stub_182
  .quad cen_stub_183
  .quad cen_stub_184
  .quad cen_stub_185
  .quad cen_stub_186
  .quad cen_stub_187
  .quad cen_stub_188
  .quad cen_stub_189
  .quad cen_stub_190
  .quad cen_stub_191
  .quad cen_stub_192
  .quad cen_stub_193
  .quad cen_stub_194
  .quad cen_stub_195
  .quad cen_stub_196
  .quad cen_stub_197
  .quad cen_stub_198
  .quad cen_stub_199
  .quad cen_stub_200
  .quad cen_stub_201
  .quad cen_stub_202
  .quad cen_stub_203
  .quad cen_stub_204
  .quad cen_stub_205
  .quad cen_stub_206
  .quad cen_stub_207
  .quad cen_stub_208
  .quad cen_stub_209
  .quad cen_stub_210
  .quad cen_stub_211
  .quad cen_stub_212
  .quad cen_stub_213
  .quad cen_stub_214
  .quad cen_stub_215
  .quad cen_stub_216
  .quad cen_stub_217
  .quad cen_stub_218
  .quad cen_stub_219
  .quad cen_stub_220
  .quad cen_stub_221
  .quad cen_stub_222
  .quad cen_stub_223
  .quad cen_stub_224
  .quad cen_stub_225
  .quad cen_stub_226
  .quad cen_stub_227
  .quad cen_stub_228
  .quad cen_stub_229
  .quad cen_stub_230
  .quad cen_stub_231
  .quad cen_stub_232
  .quad cen_stub_233
  .quad cen_stub_234
  .quad cen_stub_235
  .quad cen_stub_236
  .quad cen_stub_237
  .quad cen_stub_238
  .quad cen_stub_239
  .quad cen_stub_240
  .quad cen_stub_241
  .quad cen_stub_242
  .quad cen_stub_243
  .quad cen_stub_244
  .quad cen_stub_245
  .quad cen_stub_246
  .quad cen_stub_247
  .quad cen_stub_248
  .quad cen_stub_249
  .quad cen_stub_250
  .quad cen_stub_251
  .quad cen_stub_252
  .quad cen_stub_253
  .quad cen_stub_254
  .quad cen_stub_255
.globl ak_stub_table
ak_stub_table:
  .quad ak_stub_0
  .quad ak_stub_1
  .quad ak_stub_2
  .quad ak_stub_3
  .quad ak_stub_4
  .quad ak_stub_5
  .quad ak_stub_6
  .quad ak_stub_7
  .quad ak_stub_8
  .quad ak_stub_9
  .quad ak_stub_10
  .quad ak_stub_11
  .quad ak_stub_12
  .quad ak_stub_13
  .quad ak_stub_14
  .quad ak_stub_15

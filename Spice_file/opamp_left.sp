.subckt m18 VB2 VB3 VBN VNC AVSS nodeEnd
MM47<1> AVSS VB2 AVSS AVSS nch_ulvt_mac fin=16 l=0.12u m=4 roll=1 common=1
MM47<4> AVSS VNC AVSS AVSS nch_ulvt_mac fin=16 l=0.12u m=4 roll=1 common=1
MM47<3> AVSS VBN AVSS AVSS nch_ulvt_mac fin=16 l=0.12u m=4 roll=1 common=1
MM47<2> AVSS VB3 AVSS AVSS nch_ulvt_mac fin=16 l=0.12u m=4 roll=1 common=1
.ends

.subckt m19 VB2 AVSS VBN VNC IN iref nodeEnd
MM33 VB2   VB2 AVSS  AVSS nch_ulvt_mac fin=4 l=0.24u m=1 roll=1 common=0
MM35 VBN   VB2 VNC   AVSS nch_ulvt_mac fin=8 l=0.12u m=1 roll=1 common=0
MM39 IN    VB2 net3  AVSS nch_ulvt_mac fin=8 l=0.12u m=1 roll=1 common=0
MM43 iref  VB2 net16 AVSS nch_ulvt_mac fin=8 l=0.12u m=1 roll=1 common=0
MM34 VNC   VBN AVSS  AVSS nch_ulvt_mac fin=4 l=0.12u m=2 roll=2 common=0
MM38 net3  VBN AVSS  AVSS nch_ulvt_mac fin=4 l=0.12u m=2 roll=2 common=0
MM42 net16 VBN AVSS  AVSS nch_ulvt_mac fin=4 l=0.12u m=2 roll=2 common=0
.ends

.subckt m20 VDD_REG VB1 VBP VPC nodeEnd
MM48<1> VDD_REG VB1 VDD_REG VDD_REG pch_ulvt_mac fin=16 l=0.12u m=4 roll=1 common=0
MM48<2> VDD_REG VBP VDD_REG VDD_REG pch_ulvt_mac fin=16 l=0.12u m=4 roll=2 common=0
MM48<3> VDD_REG VPC VDD_REG VDD_REG pch_ulvt_mac fin=16 l=0.12u m=4 roll=2 common=0
.ends

.subckt m21 IP VB1 net1 nodeEnd
MM40 IP VB1 net1 net1 pch_ulvt_mac fin=8 l=0.072u m=1 roll=1 common=0
.ends

.subckt m22 VB3 AVSS nodeEnd
MM44 VB3 VB3 AVSS AVSS nch_ulvt_mac fin=4 l=0.12u m=10 roll=1 common=0
.ends

.subckt m23 VB3 AVSS nodeEnd
MM36 VBP VB1 VPC VPC pch_ulvt_mac fin=8 l=0.072u m=1 roll=1 common=0
.ends

.subckt m24 VB3 VB1 net4 nodeEnd
MM45 VB3 VB1 net4 net4 pch_ulvt_mac fin=8 l=0.072u m=7 roll=1 common=0
.ends

.subckt m25 net4 VBP VDD_REG nodeEnd
MM0  VB1  VB1 VDD_REG VDD_REG pch_ulvt_mac fin=4 l=0.12u  m=1  roll=1 common=0
MM37 VPC  VBP VDD_REG VDD_REG pch_ulvt_mac fin=4 l=0.072u m=2  roll=1 common=0
MM41 net1 VBP VDD_REG VDD_REG pch_ulvt_mac fin=4 l=0.072u m=2  roll=1 common=0
MM46 net4 VBP VDD_REG VDD_REG pch_ulvt_mac fin=4 l=0.072u m=14 roll=1 common=0
.ends

.end
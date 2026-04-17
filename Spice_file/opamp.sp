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

.subckt m1 AVSS VDD_REG vout vo nodeEnd
MMS9 VDD_REG vo vout AVSS nch_ulvt_mac fin=20 l=0.09u m=12 roll=1 common=0
.ends

.subckt m2 AVSS VB3 vout nodeEnd
MM10 vout VB3 AVSS AVSS nch_ulvt_mac fin=4 l=0.24u m=10 roll=1 common=0
.ends

.subckt m3 AVSS Vx Vy VP VNC P1 P2 nodeEnd 
MM14 P2   Vx  VP VP pch_ulvt_mac fin=4 l=0.072u m=2 roll=1 common=0
MM31 AVSS VNC VP VP pch_ulvt_mac fin=4 l=0.072u m=4 roll=1 common=0 
MM13 P1   Vy  VP VP pch_ulvt_mac fin=4 l=0.072u m=2 roll=2 common=0
.ends

.subckt m4 AVSS VB2 VBN VBN_N VBN_P P1 P2 nodeEnd
MM17 VBN_N VB2 P1   AVSS nch_ulvt_mac fin=8 l=0.12u m=1 roll=1 common=0 
MM18 VBN_P VB2 P2   AVSS nch_ulvt_mac fin=8 l=0.12u m=1 roll=1 common=0 
MM15 P1    VBN AVSS AVSS nch_ulvt_mac fin=4 l=0.12u m=4 roll=2 common=0 
MM16 P2    VBN AVSS AVSS nch_ulvt_mac fin=4 l=0.12u m=4 roll=3 common=0
.ends

.subckt m5 IN VP VDD_REG nodeEnd
MM11 IN IN VDD_REG VDD_REG pch_ulvt_mac fin=4 l=0.072u m=2 roll=1 common=0 
MM12 VP IN VDD_REG VDD_REG pch_ulvt_mac fin=4 l=0.072u m=6 roll=2 common=0
.ends

.subckt m6 VBN_P VBN_N IN VDD_REG nodeEnd
MM20 VBN_P IN VDD_REG VDD_REG pch_ulvt_mac fin=4 l=0.072u m=2 roll=1 common=0 
MM19 VBN_N IN VDD_REG VDD_REG pch_ulvt_mac fin=4 l=0.072u m=2 roll=2 common=0
.ends

.subckt m7 AVSS Vx Vy VBN_P VBN_N vo vob nodeEnd 
MM6 vo  VBN_P Vx AVSS nch_ulvt_mac fin=8 l=0.12u m=1 roll=1 common=0 
MM5 vob VBN_N Vy AVSS nch_ulvt_mac fin=8 l=0.12u m=1 roll=2 common=0
.ends

.subckt m8 iref VBP_N vob nodeEnd
MM3LDM iref iref  iref iref pch_ulvt_mac fin=8 l=0.072u m=1 roll=1 common=1 
MM3    vob  VBP_N iref iref pch_ulvt_mac fin=8 l=0.072u m=1 roll=1 common=1 
MM3RDM vob  iref  iref iref pch_ulvt_mac fin=8 l=0.072u m=1 roll=1 common=1
.ends

.subckt m9 Irbl VBP_P vo nodeEnd
MM4LDM vo   Irbl  Irbl Irbl pch_ulvt_mac fin=8 l=0.072u m=1 roll=1 common=1
MM4    vo   VBP_P Irbl Irbl pch_ulvt_mac fin=8 l=0.072u m=1 roll=1 common=1
MM4RDM Irbl Irbl  Irbl Irbl pch_ulvt_mac fin=8 l=0.072u m=1 roll=1 common=1
.ends

.subckt m10 AVSS Vx Vy vob nodeEnd
MNDM_1   AVSS AVSS AVSS AVSS nch_ulvt_mac fin=4 l=0.12u m=1 roll=1 common=1
MM8      Vx   vob  AVSS AVSS nch_ulvt_mac fin=4 l=0.12u m=1 roll=1 common=1
MM7      Vy   vob  AVSS AVSS nch_ulvt_mac fin=4 l=0.12u m=1 roll=1 common=l
MNDM_1_2 AVSS AVSS AVSS AVSS nch_ulvt_mac fin=4 l=0.12u m=1 roll=1 common=1
MNDM_1_3 AVSS AVSS AVSS AVSS nch_ulvt_mac fin=4 l=0.12u m=1 roll=2 common=1
MM8_2    Vx   vob  AVSS AVSS nch_ulvt_mac fin=4 l=0.12u m=1 roll=2 common=1
MM7_2    Vy   vob  AVSS AVSS nch_ulvt_mac fin=4 l=0.12u m=1 roll=2 common=1
MNDM_1_4 AVSS AVSS AVSS AVSS nch_ulvt_mac fin=4 l=0.12u m=1 roll=2 common=1
.ends

.subckt m11 Irbl iref VBP VDD_REG nodeEnd
MM1   iref VBP VDD_REG VDD_REG pch_ulvt_mac fin=4 l=0.072u m=2 roll=1 common=1
MM2   Irbl VBP VDD_REG VDD_REG pch_ulvt_mac fin=4 l=0.072u m=2 roll=1 common=1
MM1_2 iref VBP VDD_REG VDD_REG pch_ulvt_mac fin=4 l=0.072u m=2 roll=2 common=1
MM2_2 Irbl VBP VDD_REG VDD_REG pch_ulvt_mac fin=4 l=0.072u m=2 roll=2 common=1
.ends

.subckt m12 AVSS IP VN nodeEnd
MM22   VN   IP   AVSS AVSS nch_ulvt_mac fin=4 l=0.072u m=4 roll=1 common=0
MM21DM IP   AVSS AVSS AVSS nch_ulvt_mac fin=4 l=0.072u m=1 roll=2 common=1
MM21   IP   IP   AVSS AVSS nch_ulvt_mac fin=4 l=0.072u m=1 roll=2 common=1
MNDM   AVSS AVSS AVSS AVSS nch_ulvt_mac fin=4 l=0.072u m=1 roll=2 common=1
.ends

.subckt m13 AVSS Irbl iref N1 N2 VN VPC VDD_REG nodeEnd
MM23DM   N1      AVSS AVSS AVSS nch_ulvt_mac fin=4 l=0.12u m=1 roll=1 common=1
MM23     N1      iref VN   AVSS nch_ulvt_mac fin=4 l=0.12u m=2 roll=1 common=1
MM23DM_2 N1      AVSS AVSS AVSS nch_ulvt_mac fin=4 l=0.12u m=1 roll=1 common=1
MM24DM   N2      AVSS AVSS AVSS nch_ulvt_mac fin=4 l=0.12u m=1 roll=2 common=1
MM24_2   N2      Irbl VN   AVSS nch_ulvt_mac fin=4 l=0.12u m=2 roll=2 common=1
MM24DM_2 N2      AVSS AVSS AVSS nch_ulvt_mac fin=4 l=0.12u m=1 roll=2 common=1
MM32     VDD_REG VPC  VN   AVSS nch_ulvt_mac fin=4 l=0.12u m=4 roll=3 common=1
.ends

.subckt m14 N1 N2 VBP VDD_REG nodeEnd
MM25 N1 VBP VDD_REG VDD_REG pch_ulvt_mac fin=4 l=0.072u m=4 roll=1 common=0
MM26 N2 VBP VDD_REG VDD_REG pch_ulvt_mac fin=4 l=0.072u m=4 roll=2 common=0
.ends

.subckt m15 N2 VBP_P VB1 nodeEnd
MM28DM   N2    N2  N2 N2 pch_ulvt_mac fin=8 l=0.072u m=1 roll=1 common=1
MM28     VBP_P VB1 N2 N2 pch_ulvt_mac fin=8 l=0.072u m=1 roll=1 common=1
MM28DM_R VBP_P N2  N2 N2 pch_ulvt_mac fin=8 l=0.072u m=1 roll=1 common=1
.ends

.subckt m16 AVSS VBP_P VBP_N IP nodeEnd
MM29DM  AVSS  AVSS AVSS AVSS nch_ulvt_mac fin=4 l=0.072u m=1 roll=1 common=1
MM29    VBP_N IP   AVSS AVSS nch_ulvt_mac fin=4 l=0.072u m=1 roll=1 common=1
MM29DMR VBP_N AVSS AVSS AVSS nch_ulvt_mac fin=4 l=0.072u m=1 roll=1 common=1
MM30DM  AVSS  AVSS AVSS AVSS nch_ulvt_mac fin=4 l=0.072u m=1 roll=2 common=1
MM30    VBP_P IP   AVSS AVSS nch_ulvt_mac fin=4 l=0.072u m=1 roll=2 common=1
MM30DMR VBP_P AVSS AVSS AVSS nch_ulvt_mac fin=4 l=0.072u m=1 roll=2 common=1
.ends

.subckt m17 VBP_N VB1 N1 nodeEnd
MM27DM   N1    N1  N1 N1 pch_ulvt_mac fin=8 l=0.072u m=1 roll=1 common=1
MM27     VBP_N VB1 N1 N1 pch_ulvt_mac fin=8 l=0.072u m=1 roll=1 common=1
MM27DM_R VBP_N N1  N1 N1 pch_ulvt_mac fin=8 l=0.072u m=1 roll=1 common=1
.ends

.end

.end
#include "__cf_nmm2DFullBody.h"
#include <math.h>
#include "nmm2DFullBody_acc.h"
#include "nmm2DFullBody_acc_private.h"
#include <stdio.h>
#include "slexec_vm_simstruct_bridge.h"
#include "slexec_vm_zc_functions.h"
#include "slexec_vm_lookup_functions.h"
#include "slsv_diagnostic_codegen_c_api.h"
#include "simtarget/slSimTgtMdlrefSfcnBridge.h"
#include "simstruc.h"
#include "fixedpoint.h"
#define CodeFormat S-Function
#define AccDefine1 Accelerator_S-Function
#include "simtarget/slAccSfcnBridge.h"
#ifndef __RTW_UTFREE__  
extern void * utMalloc ( size_t ) ; extern void utFree ( void * ) ;
#endif
boolean_T nmm2DFullBody_acc_rt_TDelayUpdateTailOrGrowBuf ( int_T * bufSzPtr ,
int_T * tailPtr , int_T * headPtr , int_T * lastPtr , real_T tMinusDelay ,
real_T * * tBufPtr , real_T * * uBufPtr , real_T * * xBufPtr , boolean_T
isfixedbuf , boolean_T istransportdelay , int_T * maxNewBufSzPtr ) { int_T
testIdx ; int_T tail = * tailPtr ; int_T bufSz = * bufSzPtr ; real_T * tBuf =
* tBufPtr ; real_T * xBuf = ( NULL ) ; int_T numBuffer = 2 ; if (
istransportdelay ) { numBuffer = 3 ; xBuf = * xBufPtr ; } testIdx = ( tail <
( bufSz - 1 ) ) ? ( tail + 1 ) : 0 ; if ( ( tMinusDelay <= tBuf [ testIdx ] )
&& ! isfixedbuf ) { int_T j ; real_T * tempT ; real_T * tempU ; real_T *
tempX = ( NULL ) ; real_T * uBuf = * uBufPtr ; int_T newBufSz = bufSz + 1024
; if ( newBufSz > * maxNewBufSzPtr ) { * maxNewBufSzPtr = newBufSz ; } tempU
= ( real_T * ) utMalloc ( numBuffer * newBufSz * sizeof ( real_T ) ) ; if (
tempU == ( NULL ) ) { return ( false ) ; } tempT = tempU + newBufSz ; if (
istransportdelay ) tempX = tempT + newBufSz ; for ( j = tail ; j < bufSz ; j
++ ) { tempT [ j - tail ] = tBuf [ j ] ; tempU [ j - tail ] = uBuf [ j ] ; if
( istransportdelay ) tempX [ j - tail ] = xBuf [ j ] ; } for ( j = 0 ; j <
tail ; j ++ ) { tempT [ j + bufSz - tail ] = tBuf [ j ] ; tempU [ j + bufSz -
tail ] = uBuf [ j ] ; if ( istransportdelay ) tempX [ j + bufSz - tail ] =
xBuf [ j ] ; } if ( * lastPtr > tail ) { * lastPtr -= tail ; } else { *
lastPtr += ( bufSz - tail ) ; } * tailPtr = 0 ; * headPtr = bufSz ; utFree (
uBuf ) ; * bufSzPtr = newBufSz ; * tBufPtr = tempT ; * uBufPtr = tempU ; if (
istransportdelay ) * xBufPtr = tempX ; } else { * tailPtr = testIdx ; }
return ( true ) ; } real_T nmm2DFullBody_acc_rt_TDelayInterpolate ( real_T
tMinusDelay , real_T tStart , real_T * tBuf , real_T * uBuf , int_T bufSz ,
int_T * lastIdx , int_T oldestIdx , int_T newIdx , real_T initOutput ,
boolean_T discrete , boolean_T minorStepAndTAtLastMajorOutput ) { int_T i ;
real_T yout , t1 , t2 , u1 , u2 ; if ( ( newIdx == 0 ) && ( oldestIdx == 0 )
&& ( tMinusDelay > tStart ) ) return initOutput ; if ( tMinusDelay <= tStart
) return initOutput ; if ( ( tMinusDelay <= tBuf [ oldestIdx ] ) ) { if (
discrete ) { return ( uBuf [ oldestIdx ] ) ; } else { int_T tempIdx =
oldestIdx + 1 ; if ( oldestIdx == bufSz - 1 ) tempIdx = 0 ; t1 = tBuf [
oldestIdx ] ; t2 = tBuf [ tempIdx ] ; u1 = uBuf [ oldestIdx ] ; u2 = uBuf [
tempIdx ] ; if ( t2 == t1 ) { if ( tMinusDelay >= t2 ) { yout = u2 ; } else {
yout = u1 ; } } else { real_T f1 = ( t2 - tMinusDelay ) / ( t2 - t1 ) ;
real_T f2 = 1.0 - f1 ; yout = f1 * u1 + f2 * u2 ; } return yout ; } } if (
minorStepAndTAtLastMajorOutput ) { if ( newIdx != 0 ) { if ( * lastIdx ==
newIdx ) { ( * lastIdx ) -- ; } newIdx -- ; } else { if ( * lastIdx == newIdx
) { * lastIdx = bufSz - 1 ; } newIdx = bufSz - 1 ; } } i = * lastIdx ; if (
tBuf [ i ] < tMinusDelay ) { while ( tBuf [ i ] < tMinusDelay ) { if ( i ==
newIdx ) break ; i = ( i < ( bufSz - 1 ) ) ? ( i + 1 ) : 0 ; } } else { while
( tBuf [ i ] >= tMinusDelay ) { i = ( i > 0 ) ? i - 1 : ( bufSz - 1 ) ; } i =
( i < ( bufSz - 1 ) ) ? ( i + 1 ) : 0 ; } * lastIdx = i ; if ( discrete ) {
double tempEps = ( DBL_EPSILON ) * 128.0 ; double localEps = tempEps *
muDoubleScalarAbs ( tBuf [ i ] ) ; if ( tempEps > localEps ) { localEps =
tempEps ; } localEps = localEps / 2.0 ; if ( tMinusDelay >= ( tBuf [ i ] -
localEps ) ) { yout = uBuf [ i ] ; } else { if ( i == 0 ) { yout = uBuf [
bufSz - 1 ] ; } else { yout = uBuf [ i - 1 ] ; } } } else { if ( i == 0 ) {
t1 = tBuf [ bufSz - 1 ] ; u1 = uBuf [ bufSz - 1 ] ; } else { t1 = tBuf [ i -
1 ] ; u1 = uBuf [ i - 1 ] ; } t2 = tBuf [ i ] ; u2 = uBuf [ i ] ; if ( t2 ==
t1 ) { if ( tMinusDelay >= t2 ) { yout = u2 ; } else { yout = u1 ; } } else {
real_T f1 = ( t2 - tMinusDelay ) / ( t2 - t1 ) ; real_T f2 = 1.0 - f1 ; yout
= f1 * u1 + f2 * u2 ; } } return ( yout ) ; } void rt_ssGetBlockPath (
SimStruct * S , int_T sysIdx , int_T blkIdx , char_T * * path ) {
_ssGetBlockPath ( S , sysIdx , blkIdx , path ) ; } void rt_ssSet_slErrMsg (
SimStruct * S , void * diag ) { if ( ! _ssIsErrorStatusAslErrMsg ( S ) ) {
_ssSet_slErrMsg ( S , diag ) ; } else { _ssDiscardDiagnostic ( S , diag ) ; }
} void rt_ssReportDiagnosticAsWarning ( SimStruct * S , void * diag ) {
_ssReportDiagnosticAsWarning ( S , diag ) ; } void
nmm2DFullBody_GroundFrictionModel_Init ( SimStruct * S ,
B_GroundFrictionModel_nmm2DFullBody_T * localB ,
DW_GroundFrictionModel_nmm2DFullBody_T * localDW ,
P_GroundFrictionModel_nmm2DFullBody_T * localP ) { localDW ->
Memory_PreviousInput = localP -> P_21 ; localDW -> Memory_PreviousInput_p =
localP -> P_22 ; localDW -> Memory_PreviousInput_e = localP -> P_18 ; localB
-> B_0_0_0 = localP -> P_0 ; localB -> B_0_7_0 = localP -> P_17 ; localB ->
B_0_11_0 = localP -> P_1 ; localDW -> Memory_PreviousInput_j = localP -> P_20
; localB -> B_1_8_0 = localP -> P_6 ; localB -> B_1_14_0 = localP -> P_19 ;
localB -> B_2_3_0 = localP -> P_12 ; localB -> B_2_9_0 = localP -> P_13 ; }
void nmm2DFullBody_GroundFrictionModel_Reset ( SimStruct * S ,
DW_GroundFrictionModel_nmm2DFullBody_T * localDW ,
P_GroundFrictionModel_nmm2DFullBody_T * localP ) { localDW ->
Memory_PreviousInput = localP -> P_21 ; localDW -> Memory_PreviousInput_p =
localP -> P_22 ; } void nmm2DFullBody_GroundFrictionModel_Disable ( SimStruct
* S , B_GroundFrictionModel_nmm2DFullBody_T * localB ,
DW_GroundFrictionModel_nmm2DFullBody_T * localDW ,
P_GroundFrictionModel_nmm2DFullBody_T * localP ) { if ( localDW ->
XAxisKineticFrictionModel_MODE ) { localB -> B_0_7_0 = localP -> P_17 ;
localB -> B_0_11_0 = localP -> P_1 ; localDW ->
XAxisKineticFrictionModel_MODE = false ; } if ( localDW ->
XAxisStaticFrictionModel_MODE ) { localB -> B_1_8_0 = localP -> P_6 ; localB
-> B_1_14_0 = localP -> P_19 ; localDW -> XAxisStaticFrictionModel_MODE =
false ; } localB -> B_2_3_0 = localP -> P_12 ; localB -> B_2_9_0 = localP ->
P_13 ; localDW -> GroundFrictionModel_MODE = false ; } void
nmm2DFullBody_GroundFrictionModel ( SimStruct * S , boolean_T rtu_Contact ,
real_T rtu_dym , real_T rtu_vyms , real_T rtu_xm , real_T rtu_vxms ,
B_GroundFrictionModel_nmm2DFullBody_T * localB , const
ConstB_GroundFrictionModel_nmm2DFullBody_T * localC ,
DW_GroundFrictionModel_nmm2DFullBody_T * localDW ,
P_GroundFrictionModel_nmm2DFullBody_T * localP ) { int32_T isHit ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( ( isHit != 0 ) && ( ssIsMajorTimeStep ( S
) != 0 ) ) { if ( rtu_Contact ) { if ( ! localDW -> GroundFrictionModel_MODE
) { if ( ssGetTaskTime ( S , 1 ) != ssGetTStart ( S ) ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; }
nmm2DFullBody_GroundFrictionModel_Reset ( S , localDW , localP ) ; localDW ->
GroundFrictionModel_MODE = true ; } } else { if ( localDW ->
GroundFrictionModel_MODE ) { ssSetBlockStateForSolverChangedAtMajorStep ( S )
; nmm2DFullBody_GroundFrictionModel_Disable ( S , localB , localDW , localP )
; } } } if ( localDW -> GroundFrictionModel_MODE ) { isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { localB -> B_2_0_0 = localDW ->
Memory_PreviousInput ; } localB -> B_2_1_0 = localP -> P_15 * rtu_dym ;
localB -> B_2_2_0 = localP -> P_16 * rtu_vyms ; localB -> B_2_3_0 = ( 1.0 -
localB -> B_2_2_0 ) * - localB -> B_2_1_0 * ( real_T ) ( localB -> B_2_2_0 <
1.0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { localB ->
B_2_4_0 = ! localB -> B_2_0_0 ; if ( ssIsMajorTimeStep ( S ) != 0 ) { if (
localB -> B_2_4_0 ) { if ( ! localDW -> XAxisKineticFrictionModel_MODE ) { if
( ssGetTaskTime ( S , 1 ) != ssGetTStart ( S ) ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } localDW ->
Memory_PreviousInput_e = localP -> P_18 ; localDW ->
XAxisKineticFrictionModel_MODE = true ; } } else { if ( localDW ->
XAxisKineticFrictionModel_MODE ) { ssSetBlockStateForSolverChangedAtMajorStep
( S ) ; localB -> B_0_7_0 = localP -> P_17 ; localB -> B_0_11_0 = localP ->
P_1 ; localDW -> XAxisKineticFrictionModel_MODE = false ; } } } } if (
localDW -> XAxisKineticFrictionModel_MODE ) { localB -> B_0_0_0 = rtu_xm ; if
( ssIsMajorTimeStep ( S ) != 0 ) { localDW -> Abs_MODE_k = ( rtu_vxms >= 0.0
) ; } localB -> B_0_3_0 = ( localDW -> Abs_MODE_k > 0 ? rtu_vxms : - rtu_vxms
) - localB -> B_0_2_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { if ( ssIsMajorTimeStep ( S ) != 0 ) { localDW -> Compare_Mode_h = (
localB -> B_0_3_0 < localC -> B_0_4_0 ) ; } localB -> B_0_5_0 = localDW ->
Compare_Mode_h ; localB -> B_0_7_0 = ( ( int32_T ) localB -> B_0_5_0 > (
int32_T ) localDW -> Memory_PreviousInput_e ) ; if ( rtu_vxms > 0.0 ) {
localDW -> Sign1_MODE = 1 ; } else if ( rtu_vxms < 0.0 ) { localDW ->
Sign1_MODE = - 1 ; } else { localDW -> Sign1_MODE = 0 ; } localB -> B_0_9_0 =
localP -> P_4 * ( real_T ) localDW -> Sign1_MODE ; } localB -> B_0_10_0 =
localP -> P_5 * localB -> B_2_3_0 ; localB -> B_0_11_0 = localB -> B_0_9_0 *
localB -> B_0_10_0 ; if ( ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC (
localDW -> XAxisKineticFrictionModel_SubsysRanBC ) ; } } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( ( isHit != 0 ) && ( ssIsMajorTimeStep ( S
) != 0 ) ) { if ( localB -> B_2_0_0 ) { if ( ! localDW ->
XAxisStaticFrictionModel_MODE ) { if ( ssGetTaskTime ( S , 1 ) != ssGetTStart
( S ) ) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } localDW ->
Memory_PreviousInput_j = localP -> P_20 ; localDW ->
XAxisStaticFrictionModel_MODE = true ; } } else { if ( localDW ->
XAxisStaticFrictionModel_MODE ) { ssSetBlockStateForSolverChangedAtMajorStep
( S ) ; localB -> B_1_8_0 = localP -> P_6 ; localB -> B_1_14_0 = localP ->
P_19 ; localDW -> XAxisStaticFrictionModel_MODE = false ; } } } if ( localDW
-> XAxisStaticFrictionModel_MODE ) { localB -> B_1_0_0 = localP -> P_7 *
localB -> B_2_3_0 ; if ( ssIsMajorTimeStep ( S ) != 0 ) { localDW -> Abs_MODE
= ( localB -> B_1_0_0 >= 0.0 ) ; } localB -> B_1_2_0 = rtu_xm - localB ->
B_0_0_0 ; localB -> B_1_3_0 = localP -> P_8 * localB -> B_1_2_0 ; localB ->
B_1_4_0 = localP -> P_9 * rtu_vxms ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if
( isHit != 0 ) { if ( ssIsMajorTimeStep ( S ) != 0 ) { localDW ->
Compare_Mode = ( localB -> B_2_3_0 > localC -> B_1_6_0 ) ; } localB ->
B_1_7_0 = localDW -> Compare_Mode ; } localB -> B_1_8_0 = (
muDoubleScalarSign ( localB -> B_1_3_0 ) * localB -> B_1_4_0 + 1.0 ) * -
localB -> B_1_3_0 * ( real_T ) localB -> B_1_7_0 ; if ( ssIsMajorTimeStep ( S
) != 0 ) { localDW -> Abs_MODE_i = ( localB -> B_1_8_0 >= 0.0 ) ; } localB ->
B_1_10_0 = ( localDW -> Abs_MODE_i > 0 ? localB -> B_1_8_0 : - localB ->
B_1_8_0 ) - ( localDW -> Abs_MODE > 0 ? localB -> B_1_0_0 : - localB ->
B_1_0_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { if (
ssIsMajorTimeStep ( S ) != 0 ) { localDW -> Compare_Mode_o = ( localB ->
B_1_10_0 > localC -> B_1_11_0 ) ; } localB -> B_1_12_0 = localDW ->
Compare_Mode_o ; localB -> B_1_14_0 = ( ( int32_T ) localB -> B_1_12_0 > (
int32_T ) localDW -> Memory_PreviousInput_j ) ; } if ( ssIsMajorTimeStep ( S
) != 0 ) { srUpdateBC ( localDW -> XAxisStaticFrictionModel_SubsysRanBC ) ; }
} isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { isHit = ( int32_T
) ( ( ( ( ( uint32_T ) localB -> B_0_7_0 << 1 ) + localB -> B_1_14_0 ) << 1 )
+ localDW -> Memory_PreviousInput_p ) ; localB -> B_2_8_0 [ 0U ] = localP ->
P_23 [ ( uint32_T ) isHit ] ; localB -> B_2_8_0 [ 1U ] = localP -> P_23 [
isHit + 8U ] ; } localB -> B_2_9_0 = localB -> B_1_8_0 + localB -> B_0_11_0 ;
if ( ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( localDW ->
GroundFrictionModel_SubsysRanBC ) ; } } } void
nmm2DFullBody_GroundFrictionModelTID4 ( SimStruct * S ,
B_GroundFrictionModel_nmm2DFullBody_T * localB ,
DW_GroundFrictionModel_nmm2DFullBody_T * localDW ,
P_GroundFrictionModel_nmm2DFullBody_T * localP ) { localB -> B_0_2_0 = localP
-> P_2 ; if ( ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( localDW ->
XAxisKineticFrictionModel_SubsysRanBC ) ; srUpdateBC ( localDW ->
GroundFrictionModel_SubsysRanBC ) ; } } void
nmm2DFullBody_GroundFrictionModel_Update ( SimStruct * S ,
B_GroundFrictionModel_nmm2DFullBody_T * localB ,
DW_GroundFrictionModel_nmm2DFullBody_T * localDW ) { int32_T isHit ; if (
localDW -> GroundFrictionModel_MODE ) { isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { localDW -> Memory_PreviousInput = localB -> B_2_8_0 [ 0 ]
; } if ( localDW -> XAxisKineticFrictionModel_MODE ) { isHit = ssIsSampleHit
( S , 1 , 0 ) ; if ( isHit != 0 ) { localDW -> Memory_PreviousInput_e =
localB -> B_0_5_0 ; } } if ( localDW -> XAxisStaticFrictionModel_MODE ) {
isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { localDW ->
Memory_PreviousInput_j = localB -> B_1_12_0 ; } } isHit = ssIsSampleHit ( S ,
1 , 0 ) ; if ( isHit != 0 ) { localDW -> Memory_PreviousInput_p = localB ->
B_2_8_0 [ 0 ] ; } } } void nmm2DFullBody_GroundFrictionModel_ZC ( SimStruct *
S , real_T rtu_vxms , B_GroundFrictionModel_nmm2DFullBody_T * localB , const
ConstB_GroundFrictionModel_nmm2DFullBody_T * localC ,
DW_GroundFrictionModel_nmm2DFullBody_T * localDW ,
ZCV_GroundFrictionModel_nmm2DFullBody_T * localZCSV ) { if ( localDW ->
GroundFrictionModel_MODE ) { if ( localDW -> XAxisKineticFrictionModel_MODE )
{ localZCSV -> Abs_AbsZc_ZC_fg = rtu_vxms ; localZCSV ->
Compare_RelopInput_ZC_b = localB -> B_0_3_0 - localC -> B_0_4_0 ; localZCSV
-> Sign1_Input_ZC = rtu_vxms ; } else { { real_T * zcsv = & ( localZCSV ->
Abs_AbsZc_ZC_fg ) ; int_T i ; for ( i = 0 ; i < 3 ; i ++ ) { zcsv [ i ] = 0.0
; } } } if ( localDW -> XAxisStaticFrictionModel_MODE ) { localZCSV ->
Abs_AbsZc_ZC_m = localB -> B_1_0_0 ; localZCSV -> Compare_RelopInput_ZC_p =
localB -> B_2_3_0 - localC -> B_1_6_0 ; localZCSV -> Abs_AbsZc_ZC_k = localB
-> B_1_8_0 ; localZCSV -> Compare_RelopInput_ZC_n = localB -> B_1_10_0 -
localC -> B_1_11_0 ; } else { { real_T * zcsv = & ( localZCSV ->
Abs_AbsZc_ZC_m ) ; int_T i ; for ( i = 0 ; i < 4 ; i ++ ) { zcsv [ i ] = 0.0
; } } } } else { { real_T * zcsv = & ( localZCSV -> Abs_AbsZc_ZC_m ) ; int_T
i ; for ( i = 0 ; i < 7 ; i ++ ) { zcsv [ i ] = 0.0 ; } } } } void
nmm2DFullBody_GroundFrictionModel_Term ( SimStruct * const S ) { } void
nmm2DFullBody_LeftForcePlate_Init ( SimStruct * S ,
B_LeftForcePlate_nmm2DFullBody_T * localB , P_LeftForcePlate_nmm2DFullBody_T
* localP ) { localB -> B_6_0_0 [ 0 ] = localP -> P_0 ; localB -> B_6_2_0 [ 0
] = localP -> P_0 ; localB -> B_6_1_0 [ 0 ] = localP -> P_0 ; localB ->
B_6_0_0 [ 1 ] = localP -> P_0 ; localB -> B_6_2_0 [ 1 ] = localP -> P_0 ;
localB -> B_6_1_0 [ 1 ] = localP -> P_0 ; } void
nmm2DFullBody_LeftForcePlate_Disable ( SimStruct * S ,
B_LeftForcePlate_nmm2DFullBody_T * localB , DW_LeftForcePlate_nmm2DFullBody_T
* localDW , P_LeftForcePlate_nmm2DFullBody_T * localP ) { localB -> B_6_0_0 [
0 ] = localP -> P_0 ; localB -> B_6_2_0 [ 0 ] = localP -> P_0 ; localB ->
B_6_1_0 [ 0 ] = localP -> P_0 ; localB -> B_6_0_0 [ 1 ] = localP -> P_0 ;
localB -> B_6_2_0 [ 1 ] = localP -> P_0 ; localB -> B_6_1_0 [ 1 ] = localP ->
P_0 ; localDW -> LeftForcePlate_MODE = false ; } void
nmm2DFullBody_LeftForcePlate ( SimStruct * S , boolean_T rtu_Enable , real_T
rtu_GRF_BallNN , real_T rtu_GRF_BallNN_p , real_T rtu_GRF_HeelNN , real_T
rtu_GRF_HeelNN_f , B_LeftForcePlate_nmm2DFullBody_T * localB ,
DW_LeftForcePlate_nmm2DFullBody_T * localDW ,
P_LeftForcePlate_nmm2DFullBody_T * localP ) { int32_T isHit ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( ( isHit != 0 ) && ( ssIsMajorTimeStep ( S
) != 0 ) ) { if ( rtu_Enable ) { if ( ! localDW -> LeftForcePlate_MODE ) { if
( ssGetTaskTime ( S , 1 ) != ssGetTStart ( S ) ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } localDW ->
LeftForcePlate_MODE = true ; } } else { if ( localDW -> LeftForcePlate_MODE )
{ ssSetBlockStateForSolverChangedAtMajorStep ( S ) ;
nmm2DFullBody_LeftForcePlate_Disable ( S , localB , localDW , localP ) ; } }
} if ( localDW -> LeftForcePlate_MODE ) { localB -> B_6_0_0 [ 0 ] =
rtu_GRF_BallNN ; localB -> B_6_0_0 [ 1 ] = rtu_GRF_BallNN_p ; localB ->
B_6_1_0 [ 0 ] = rtu_GRF_HeelNN ; localB -> B_6_1_0 [ 1 ] = rtu_GRF_HeelNN_f ;
localB -> B_6_2_0 [ 0 ] = localB -> B_6_0_0 [ 0 ] + localB -> B_6_1_0 [ 0 ] ;
localB -> B_6_2_0 [ 1 ] = localB -> B_6_0_0 [ 1 ] + localB -> B_6_1_0 [ 1 ] ;
if ( ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( localDW ->
LeftForcePlate_SubsysRanBC ) ; } } } void nmm2DFullBody_LeftForcePlate_Term (
SimStruct * const S ) { } void nmm2DFullBody_StancePhase_Init ( SimStruct * S
, B_StancePhase_nmm2DFullBody_T * localB , P_StancePhase_nmm2DFullBody_T *
localP ) { localB -> B_15_2_0 = localP -> P_0 ; } void
nmm2DFullBody_StancePhase_Disable ( SimStruct * S ,
B_StancePhase_nmm2DFullBody_T * localB , DW_StancePhase_nmm2DFullBody_T *
localDW , P_StancePhase_nmm2DFullBody_T * localP ) { localB -> B_15_2_0 =
localP -> P_0 ; localDW -> StancePhase_MODE = false ; } void
nmm2DFullBody_StancePhase_ZC ( SimStruct * S , real_T rtu_Enable ,
DW_StancePhase_nmm2DFullBody_T * localDW , ZCV_StancePhase_nmm2DFullBody_T *
localZCSV ) { localZCSV -> StancePhase_Enable_ZC_f = rtu_Enable ; if ( !
localDW -> StancePhase_MODE ) { } } void nmm2DFullBody_StancePhase (
SimStruct * S , real_T rtu_Enable , real_T rtu_FmGAS ,
B_StancePhase_nmm2DFullBody_T * localB , DW_StancePhase_nmm2DFullBody_T *
localDW , P_StancePhase_nmm2DFullBody_T * localP ) { int32_T isHit ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( ( isHit != 0 ) && ( ssIsMajorTimeStep ( S
) != 0 ) ) { if ( rtu_Enable > 0.0 ) { if ( ! localDW -> StancePhase_MODE ) {
if ( ssGetTaskTime ( S , 1 ) != ssGetTStart ( S ) ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } localDW ->
StancePhase_MODE = true ; } } else { if ( localDW -> StancePhase_MODE ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ;
nmm2DFullBody_StancePhase_Disable ( S , localB , localDW , localP ) ; } } }
if ( localDW -> StancePhase_MODE ) { localB -> B_15_0_0 = localP -> P_1 *
rtu_FmGAS ; localB -> B_15_2_0 = localB -> B_15_1_0 + localB -> B_15_0_0 ; if
( ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( localDW ->
StancePhase_SubsysRanBC ) ; } } } void nmm2DFullBody_StancePhaseTID4 (
SimStruct * S , B_StancePhase_nmm2DFullBody_T * localB ,
DW_StancePhase_nmm2DFullBody_T * localDW , P_StancePhase_nmm2DFullBody_T *
localP ) { localB -> B_15_1_0 = localP -> P_2 ; if ( ssIsMajorTimeStep ( S )
!= 0 ) { srUpdateBC ( localDW -> StancePhase_SubsysRanBC ) ; } } void
nmm2DFullBody_StancePhase_Term ( SimStruct * const S ) { } void
nmm2DFullBody_SwingPhase_Init ( SimStruct * S , B_SwingPhase_nmm2DFullBody_T
* localB , P_SwingPhase_nmm2DFullBody_T * localP ) { localB -> B_16_2_0 =
localP -> P_0 ; } void nmm2DFullBody_SwingPhase_Disable ( SimStruct * S ,
B_SwingPhase_nmm2DFullBody_T * localB , DW_SwingPhase_nmm2DFullBody_T *
localDW , P_SwingPhase_nmm2DFullBody_T * localP ) { localB -> B_16_2_0 =
localP -> P_0 ; localDW -> SwingPhase_MODE = false ; } void
nmm2DFullBody_SwingPhase ( SimStruct * S , boolean_T rtu_Enable , real_T
rtu_FmGLU , B_SwingPhase_nmm2DFullBody_T * localB ,
DW_SwingPhase_nmm2DFullBody_T * localDW , P_SwingPhase_nmm2DFullBody_T *
localP ) { int32_T isHit ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( ( isHit
!= 0 ) && ( ssIsMajorTimeStep ( S ) != 0 ) ) { if ( rtu_Enable ) { if ( !
localDW -> SwingPhase_MODE ) { if ( ssGetTaskTime ( S , 1 ) != ssGetTStart (
S ) ) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } localDW ->
SwingPhase_MODE = true ; } } else { if ( localDW -> SwingPhase_MODE ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ;
nmm2DFullBody_SwingPhase_Disable ( S , localB , localDW , localP ) ; } } } if
( localDW -> SwingPhase_MODE ) { localB -> B_16_0_0 = localP -> P_1 *
rtu_FmGLU ; localB -> B_16_2_0 = localB -> B_16_1_0 + localB -> B_16_0_0 ; if
( ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( localDW ->
SwingPhase_SubsysRanBC ) ; } } } void nmm2DFullBody_SwingPhaseTID4 (
SimStruct * S , B_SwingPhase_nmm2DFullBody_T * localB ,
DW_SwingPhase_nmm2DFullBody_T * localDW , P_SwingPhase_nmm2DFullBody_T *
localP ) { localB -> B_16_1_0 = localP -> P_2 ; if ( ssIsMajorTimeStep ( S )
!= 0 ) { srUpdateBC ( localDW -> SwingPhase_SubsysRanBC ) ; } } void
nmm2DFullBody_SwingPhase_Term ( SimStruct * const S ) { } void
nmm2DFullBody_StancePhase_l_Init ( SimStruct * S ,
B_StancePhase_nmm2DFullBody_o_T * localB , P_StancePhase_nmm2DFullBody_j_T *
localP ) { localB -> B_17_14_0 = localP -> P_0 ; } void
nmm2DFullBody_StancePhase_j_Disable ( SimStruct * S ,
B_StancePhase_nmm2DFullBody_o_T * localB , DW_StancePhase_nmm2DFullBody_i_T *
localDW , P_StancePhase_nmm2DFullBody_j_T * localP ) { localB -> B_17_14_0 =
localP -> P_0 ; localDW -> StancePhase_MODE = false ; } void
nmm2DFullBody_StancePhase_g_ZC ( SimStruct * S , real_T rtu_Enable ,
DW_StancePhase_nmm2DFullBody_i_T * localDW ,
ZCV_StancePhase_nmm2DFullBody_h_T * localZCSV ) { localZCSV ->
StancePhase_Enable_ZC_b = rtu_Enable ; if ( ! localDW -> StancePhase_MODE ) {
} } void nmm2DFullBody_StancePhase_i ( SimStruct * S , real_T rtu_Enable ,
real_T rtu_Theta , real_T rtu_dThetarads , real_T rtu_CLdxThighN , real_T
rtu_OLonCL , B_StancePhase_nmm2DFullBody_o_T * localB ,
DW_StancePhase_nmm2DFullBody_i_T * localDW , P_StancePhase_nmm2DFullBody_j_T
* localP ) { int32_T isHit ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( (
isHit != 0 ) && ( ssIsMajorTimeStep ( S ) != 0 ) ) { if ( rtu_Enable > 0.0 )
{ if ( ! localDW -> StancePhase_MODE ) { if ( ssGetTaskTime ( S , 1 ) !=
ssGetTStart ( S ) ) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; }
localDW -> StancePhase_MODE = true ; } } else { if ( localDW ->
StancePhase_MODE ) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ;
nmm2DFullBody_StancePhase_j_Disable ( S , localB , localDW , localP ) ; } } }
if ( localDW -> StancePhase_MODE ) { localB -> B_17_2_0 = rtu_Theta - localB
-> B_17_1_0 ; localB -> B_17_3_0 = localP -> P_3 * localB -> B_17_2_0 ;
localB -> B_17_4_0 = localP -> P_4 * rtu_dThetarads ; localB -> B_17_5_0 =
localB -> B_17_3_0 + localB -> B_17_4_0 ; if ( localB -> B_17_5_0 > localP ->
P_5 ) { localB -> B_17_6_0 = localP -> P_5 ; } else if ( localB -> B_17_5_0 <
localP -> P_6 ) { localB -> B_17_6_0 = localP -> P_6 ; } else { localB ->
B_17_6_0 = localB -> B_17_5_0 ; } localB -> B_17_7_0 = localB -> B_17_0_0 +
localB -> B_17_6_0 ; if ( rtu_CLdxThighN > localP -> P_7 ) { localB ->
B_17_8_0 = localP -> P_7 ; } else if ( rtu_CLdxThighN < localP -> P_8 ) {
localB -> B_17_8_0 = localP -> P_8 ; } else { localB -> B_17_8_0 =
rtu_CLdxThighN ; } localB -> B_17_9_0 = localP -> P_9 * localB -> B_17_8_0 ;
localB -> B_17_10_0 = localB -> B_17_7_0 * localB -> B_17_9_0 ; if ( localB
-> B_17_10_0 > localP -> P_10 ) { localB -> B_17_11_0 = localP -> P_10 ; }
else if ( localB -> B_17_10_0 < localP -> P_11 ) { localB -> B_17_11_0 =
localP -> P_11 ; } else { localB -> B_17_11_0 = localB -> B_17_10_0 ; }
localB -> B_17_12_0 = localP -> P_12 * rtu_OLonCL ; localB -> B_17_13_0 =
localB -> B_17_11_0 - localB -> B_17_12_0 ; localB -> B_17_14_0 = localP ->
P_13 * localB -> B_17_13_0 ; if ( ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC
( localDW -> StancePhase_SubsysRanBC ) ; } } } void
nmm2DFullBody_StancePhase_iTID4 ( SimStruct * S ,
B_StancePhase_nmm2DFullBody_o_T * localB , DW_StancePhase_nmm2DFullBody_i_T *
localDW , P_StancePhase_nmm2DFullBody_j_T * localP ) { localB -> B_17_0_0 =
localP -> P_1 ; localB -> B_17_1_0 = localP -> P_2 ; if ( ssIsMajorTimeStep (
S ) != 0 ) { srUpdateBC ( localDW -> StancePhase_SubsysRanBC ) ; } } void
nmm2DFullBody_StancePhase_g_Term ( SimStruct * const S ) { } void
nmm2DFullBody_StancePhase_p_Init ( SimStruct * S ,
B_StancePhase_nmm2DFullBody_j_T * localB , P_StancePhase_nmm2DFullBody_d_T *
localP ) { localB -> B_18_18_0 = localP -> P_0 ; localB -> B_18_15_0 = localP
-> P_1 ; localB -> B_18_2_0 = localP -> P_2 ; } void
nmm2DFullBody_StancePhase_o_Disable ( SimStruct * S ,
B_StancePhase_nmm2DFullBody_j_T * localB , DW_StancePhase_nmm2DFullBody_in_T
* localDW , P_StancePhase_nmm2DFullBody_d_T * localP ) { localB -> B_18_18_0
= localP -> P_0 ; localB -> B_18_15_0 = localP -> P_1 ; localDW ->
StancePhase_MODE = false ; } void nmm2DFullBody_StancePhase_f_ZC ( SimStruct
* S , real_T rtu_Enable , DW_StancePhase_nmm2DFullBody_in_T * localDW ,
ZCV_StancePhase_nmm2DFullBody_hs_T * localZCSV ) { localZCSV ->
StancePhase_Enable_ZC_k = rtu_Enable ; if ( ! localDW -> StancePhase_MODE ) {
} } void nmm2DFullBody_StancePhase_o ( SimStruct * S , real_T rtu_Enable ,
real_T rtu_Theta , real_T rtu_dThetarads , real_T rtu_CLdxThighN , real_T
rtu_OLonCL , B_StancePhase_nmm2DFullBody_j_T * localB ,
DW_StancePhase_nmm2DFullBody_in_T * localDW , P_StancePhase_nmm2DFullBody_d_T
* localP ) { int32_T isHit ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( (
isHit != 0 ) && ( ssIsMajorTimeStep ( S ) != 0 ) ) { if ( rtu_Enable > 0.0 )
{ if ( ! localDW -> StancePhase_MODE ) { if ( ssGetTaskTime ( S , 1 ) !=
ssGetTStart ( S ) ) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; }
localDW -> StancePhase_MODE = true ; } } else { if ( localDW ->
StancePhase_MODE ) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ;
nmm2DFullBody_StancePhase_o_Disable ( S , localB , localDW , localP ) ; } } }
if ( localDW -> StancePhase_MODE ) { localB -> B_18_2_0 = rtu_Theta - localB
-> B_18_1_0 ; localB -> B_18_3_0 = localP -> P_5 * localB -> B_18_2_0 ;
localB -> B_18_4_0 = localP -> P_6 * rtu_dThetarads ; localB -> B_18_5_0 =
localB -> B_18_3_0 + localB -> B_18_4_0 ; if ( localB -> B_18_5_0 > localP ->
P_7 ) { localB -> B_18_6_0 = localP -> P_7 ; } else if ( localB -> B_18_5_0 <
localP -> P_8 ) { localB -> B_18_6_0 = localP -> P_8 ; } else { localB ->
B_18_6_0 = localB -> B_18_5_0 ; } localB -> B_18_7_0 = localB -> B_18_0_0 +
localB -> B_18_6_0 ; if ( rtu_CLdxThighN > localP -> P_9 ) { localB ->
B_18_8_0 = localP -> P_9 ; } else if ( rtu_CLdxThighN < localP -> P_10 ) {
localB -> B_18_8_0 = localP -> P_10 ; } else { localB -> B_18_8_0 =
rtu_CLdxThighN ; } localB -> B_18_9_0 = localP -> P_11 * localB -> B_18_8_0 ;
localB -> B_18_10_0 = localB -> B_18_7_0 * localB -> B_18_9_0 ; if ( localB
-> B_18_5_0 > localP -> P_12 ) { localB -> B_18_11_0 = localP -> P_12 ; }
else if ( localB -> B_18_5_0 < localP -> P_13 ) { localB -> B_18_11_0 =
localP -> P_13 ; } else { localB -> B_18_11_0 = localB -> B_18_5_0 ; } localB
-> B_18_12_0 = localB -> B_18_0_0 - localB -> B_18_11_0 ; localB -> B_18_13_0
= localB -> B_18_9_0 * localB -> B_18_12_0 ; if ( localB -> B_18_10_0 >
localP -> P_14 ) { localB -> B_18_14_0 = localP -> P_14 ; } else if ( localB
-> B_18_10_0 < localP -> P_15 ) { localB -> B_18_14_0 = localP -> P_15 ; }
else { localB -> B_18_14_0 = localB -> B_18_10_0 ; } localB -> B_18_15_0 =
localP -> P_16 * localB -> B_18_14_0 ; localB -> B_18_16_0 = localP -> P_17 *
rtu_OLonCL ; if ( localB -> B_18_13_0 > localP -> P_18 ) { localB ->
B_18_17_0 = localP -> P_18 ; } else if ( localB -> B_18_13_0 < localP -> P_19
) { localB -> B_18_17_0 = localP -> P_19 ; } else { localB -> B_18_17_0 =
localB -> B_18_13_0 ; } localB -> B_18_18_0 = localB -> B_18_17_0 + localB ->
B_18_16_0 ; if ( ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( localDW ->
StancePhase_SubsysRanBC ) ; } } } void nmm2DFullBody_StancePhase_oTID4 (
SimStruct * S , B_StancePhase_nmm2DFullBody_j_T * localB ,
DW_StancePhase_nmm2DFullBody_in_T * localDW , P_StancePhase_nmm2DFullBody_d_T
* localP ) { localB -> B_18_0_0 = localP -> P_3 ; localB -> B_18_1_0 = localP
-> P_4 ; if ( ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( localDW ->
StancePhase_SubsysRanBC ) ; } } void nmm2DFullBody_StancePhase_p_Term (
SimStruct * const S ) { } void nmm2DFullBody_SwingPhase_f_Init ( SimStruct *
S , B_SwingPhase_nmm2DFullBody_n_T * localB , P_SwingPhase_nmm2DFullBody_i_T
* localP ) { localB -> B_19_6_0 = localP -> P_0 ; localB -> B_19_9_0 = localP
-> P_1 ; } void nmm2DFullBody_SwingPhase_l_Disable ( SimStruct * S ,
B_SwingPhase_nmm2DFullBody_n_T * localB , DW_SwingPhase_nmm2DFullBody_n_T *
localDW , P_SwingPhase_nmm2DFullBody_i_T * localP ) { localB -> B_19_6_0 =
localP -> P_0 ; localB -> B_19_9_0 = localP -> P_1 ; localDW ->
SwingPhase_MODE = false ; } void nmm2DFullBody_SwingPhase_b ( SimStruct * S ,
boolean_T rtu_Enable , real_T rtu_Delta_Theta , real_T rtu_LceHFL , real_T
rtu_LceHAM , real_T rtu_FmHAM , B_SwingPhase_nmm2DFullBody_n_T * localB ,
DW_SwingPhase_nmm2DFullBody_n_T * localDW , P_SwingPhase_nmm2DFullBody_i_T *
localP ) { int32_T isHit ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( ( isHit
!= 0 ) && ( ssIsMajorTimeStep ( S ) != 0 ) ) { if ( rtu_Enable ) { if ( !
localDW -> SwingPhase_MODE ) { if ( ssGetTaskTime ( S , 1 ) != ssGetTStart (
S ) ) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } localDW ->
SwingPhase_MODE = true ; } } else { if ( localDW -> SwingPhase_MODE ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ;
nmm2DFullBody_SwingPhase_l_Disable ( S , localB , localDW , localP ) ; } } }
if ( localDW -> SwingPhase_MODE ) { localB -> B_19_0_0 = localP -> P_2 *
rtu_LceHFL ; localB -> B_19_1_0 = localP -> P_3 * rtu_Delta_Theta ; localB ->
B_19_2_0 = localP -> P_4 * rtu_LceHAM ; localB -> B_19_4_0 = localB ->
B_19_3_0 + localB -> B_19_1_0 ; localB -> B_19_5_0 = localB -> B_19_4_0 +
localB -> B_19_0_0 ; localB -> B_19_6_0 = localB -> B_19_5_0 - localB ->
B_19_2_0 ; localB -> B_19_7_0 = localP -> P_6 * rtu_FmHAM ; localB ->
B_19_9_0 = localB -> B_19_8_0 + localB -> B_19_7_0 ; if ( ssIsMajorTimeStep (
S ) != 0 ) { srUpdateBC ( localDW -> SwingPhase_SubsysRanBC ) ; } } } void
nmm2DFullBody_SwingPhase_bTID4 ( SimStruct * S ,
B_SwingPhase_nmm2DFullBody_n_T * localB , DW_SwingPhase_nmm2DFullBody_n_T *
localDW , P_SwingPhase_nmm2DFullBody_i_T * localP ) { localB -> B_19_3_0 =
localP -> P_5 ; localB -> B_19_8_0 = localP -> P_7 ; if ( ssIsMajorTimeStep (
S ) != 0 ) { srUpdateBC ( localDW -> SwingPhase_SubsysRanBC ) ; } } void
nmm2DFullBody_SwingPhase_f_Term ( SimStruct * const S ) { } void
nmm2DFullBody_StancePhase_b_Init ( SimStruct * S ,
B_StancePhase_nmm2DFullBody_e_T * localB , P_StancePhase_nmm2DFullBody_di_T *
localP ) { localB -> B_20_7_0 = localP -> P_0 ; localB -> B_20_6_0 = localP
-> P_1 ; } void nmm2DFullBody_StancePhase_d_Disable ( SimStruct * S ,
B_StancePhase_nmm2DFullBody_e_T * localB , DW_StancePhase_nmm2DFullBody_l_T *
localDW , P_StancePhase_nmm2DFullBody_di_T * localP ) { localB -> B_20_7_0 =
localP -> P_0 ; localB -> B_20_6_0 = localP -> P_1 ; localDW ->
StancePhase_MODE = false ; } void nmm2DFullBody_StancePhase_b_ZC ( SimStruct
* S , real_T rtu_Enable , DW_StancePhase_nmm2DFullBody_l_T * localDW ,
ZCV_StancePhase_nmm2DFullBody_n_T * localZCSV ) { localZCSV ->
StancePhase_Enable_ZC_i = rtu_Enable ; if ( ! localDW -> StancePhase_MODE ) {
} } void nmm2DFullBody_StancePhase_l ( SimStruct * S , real_T rtu_Enable ,
real_T rtu_FmSOL , real_T rtu_LceTA , B_StancePhase_nmm2DFullBody_e_T *
localB , DW_StancePhase_nmm2DFullBody_l_T * localDW ,
P_StancePhase_nmm2DFullBody_di_T * localP ) { int32_T isHit ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( ( isHit != 0 ) && ( ssIsMajorTimeStep ( S
) != 0 ) ) { if ( rtu_Enable > 0.0 ) { if ( ! localDW -> StancePhase_MODE ) {
if ( ssGetTaskTime ( S , 1 ) != ssGetTStart ( S ) ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } localDW ->
StancePhase_MODE = true ; } } else { if ( localDW -> StancePhase_MODE ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ;
nmm2DFullBody_StancePhase_d_Disable ( S , localB , localDW , localP ) ; } } }
if ( localDW -> StancePhase_MODE ) { localB -> B_20_0_0 = localP -> P_2 *
rtu_FmSOL ; localB -> B_20_1_0 = localP -> P_3 * rtu_FmSOL ; localB ->
B_20_2_0 = localP -> P_4 * rtu_LceTA ; localB -> B_20_5_0 = localB ->
B_20_3_0 - localB -> B_20_0_0 ; localB -> B_20_6_0 = localB -> B_20_4_0 +
localB -> B_20_1_0 ; localB -> B_20_7_0 = localB -> B_20_5_0 + localB ->
B_20_2_0 ; if ( ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( localDW ->
StancePhase_SubsysRanBC ) ; } } } void nmm2DFullBody_StancePhase_lTID4 (
SimStruct * S , B_StancePhase_nmm2DFullBody_e_T * localB ,
DW_StancePhase_nmm2DFullBody_l_T * localDW , P_StancePhase_nmm2DFullBody_di_T
* localP ) { localB -> B_20_3_0 = localP -> P_5 ; localB -> B_20_4_0 = localP
-> P_6 ; if ( ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( localDW ->
StancePhase_SubsysRanBC ) ; } } void nmm2DFullBody_StancePhase_k_Term (
SimStruct * const S ) { } void nmm2DFullBody_SwingPhase_g_Init ( SimStruct *
S , B_SwingPhase_nmm2DFullBody_a_T * localB , P_SwingPhase_nmm2DFullBody_p_T
* localP ) { localB -> B_21_2_0 = localP -> P_0 ; localB -> B_21_3_0 = localP
-> P_1 ; } void nmm2DFullBody_SwingPhase_f_Disable ( SimStruct * S ,
B_SwingPhase_nmm2DFullBody_a_T * localB , DW_SwingPhase_nmm2DFullBody_p_T *
localDW , P_SwingPhase_nmm2DFullBody_p_T * localP ) { localB -> B_21_2_0 =
localP -> P_0 ; localB -> B_21_3_0 = localP -> P_1 ; localDW ->
SwingPhase_MODE = false ; } void nmm2DFullBody_SwingPhase_f ( SimStruct * S ,
boolean_T rtu_Enable , real_T rtu_LceTA , B_SwingPhase_nmm2DFullBody_a_T *
localB , DW_SwingPhase_nmm2DFullBody_p_T * localDW ,
P_SwingPhase_nmm2DFullBody_p_T * localP ) { int32_T isHit ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( ( isHit != 0 ) && ( ssIsMajorTimeStep ( S
) != 0 ) ) { if ( rtu_Enable ) { if ( ! localDW -> SwingPhase_MODE ) { if (
ssGetTaskTime ( S , 1 ) != ssGetTStart ( S ) ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } localDW ->
SwingPhase_MODE = true ; } } else { if ( localDW -> SwingPhase_MODE ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ;
nmm2DFullBody_SwingPhase_f_Disable ( S , localB , localDW , localP ) ; } } }
if ( localDW -> SwingPhase_MODE ) { localB -> B_21_0_0 = localP -> P_2 *
rtu_LceTA ; localB -> B_21_2_0 = localB -> B_21_1_0 + localB -> B_21_0_0 ;
isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { localB -> B_21_3_0
= localP -> P_4 ; } if ( ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC (
localDW -> SwingPhase_SubsysRanBC ) ; } } } void
nmm2DFullBody_SwingPhase_fTID4 ( SimStruct * S ,
B_SwingPhase_nmm2DFullBody_a_T * localB , DW_SwingPhase_nmm2DFullBody_p_T *
localDW , P_SwingPhase_nmm2DFullBody_p_T * localP ) { localB -> B_21_1_0 =
localP -> P_3 ; if ( ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( localDW ->
SwingPhase_SubsysRanBC ) ; } } void nmm2DFullBody_SwingPhase_m_Term (
SimStruct * const S ) { } void nmm2DFullBody_StancePhase_j_Init ( SimStruct *
S , B_StancePhase_nmm2DFullBody_g_T * localB ,
P_StancePhase_nmm2DFullBody_e_T * localP ) { localB -> B_23_8_0 = localP ->
P_0 ; } void nmm2DFullBody_StancePhase_ox_Disable ( SimStruct * S ,
B_StancePhase_nmm2DFullBody_g_T * localB , DW_StancePhase_nmm2DFullBody_p_T *
localDW , P_StancePhase_nmm2DFullBody_e_T * localP ) { localB -> B_23_8_0 =
localP -> P_0 ; localDW -> StancePhase_MODE = false ; } void
nmm2DFullBody_StancePhase_ga_ZC ( SimStruct * S , real_T rtu_Enable ,
DW_StancePhase_nmm2DFullBody_p_T * localDW ,
ZCV_StancePhase_nmm2DFullBody_o_T * localZCSV ) { localZCSV ->
StancePhase_Enable_ZC = rtu_Enable ; if ( ! localDW -> StancePhase_MODE ) { }
} void nmm2DFullBody_StancePhase_g ( SimStruct * S , real_T rtu_Enable ,
real_T rtu_CLKneeState , real_T rtu_OLonCL , real_T rtu_OLdxThighN , real_T
rtu_FmVAS , B_StancePhase_nmm2DFullBody_g_T * localB ,
DW_StancePhase_nmm2DFullBody_p_T * localDW , P_StancePhase_nmm2DFullBody_e_T
* localP ) { int32_T isHit ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( (
isHit != 0 ) && ( ssIsMajorTimeStep ( S ) != 0 ) ) { if ( rtu_Enable > 0.0 )
{ if ( ! localDW -> StancePhase_MODE ) { if ( ssGetTaskTime ( S , 1 ) !=
ssGetTStart ( S ) ) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; }
localDW -> StancePhase_MODE = true ; } } else { if ( localDW ->
StancePhase_MODE ) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ;
nmm2DFullBody_StancePhase_ox_Disable ( S , localB , localDW , localP ) ; } }
} if ( localDW -> StancePhase_MODE ) { localB -> B_23_0_0 = localP -> P_1 *
rtu_FmVAS ; if ( rtu_OLdxThighN > localP -> P_2 ) { localB -> B_23_1_0 =
localP -> P_2 ; } else if ( rtu_OLdxThighN < localP -> P_3 ) { localB ->
B_23_1_0 = localP -> P_3 ; } else { localB -> B_23_1_0 = rtu_OLdxThighN ; }
localB -> B_23_2_0 = localP -> P_4 * localB -> B_23_1_0 ; localB -> B_23_3_0
= localP -> P_5 * rtu_CLKneeState ; localB -> B_23_4_0 = rtu_OLonCL * localB
-> B_23_2_0 ; localB -> B_23_6_0 = localB -> B_23_5_0 - localB -> B_23_4_0 ;
localB -> B_23_7_0 = localB -> B_23_6_0 + localB -> B_23_0_0 ; localB ->
B_23_8_0 = localB -> B_23_7_0 - localB -> B_23_3_0 ; if ( ssIsMajorTimeStep (
S ) != 0 ) { srUpdateBC ( localDW -> StancePhase_SubsysRanBC ) ; } } } void
nmm2DFullBody_StancePhase_gTID4 ( SimStruct * S ,
B_StancePhase_nmm2DFullBody_g_T * localB , DW_StancePhase_nmm2DFullBody_p_T *
localDW , P_StancePhase_nmm2DFullBody_e_T * localP ) { localB -> B_23_5_0 =
localP -> P_6 ; if ( ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( localDW ->
StancePhase_SubsysRanBC ) ; } } void nmm2DFullBody_StancePhase_a_Term (
SimStruct * const S ) { } static void mdlOutputs ( SimStruct * S , int_T tid
) { boolean_T B_38_701_0 ; boolean_T B_38_710_0 ; boolean_T B_38_737_0 ;
boolean_T B_38_754_0 ; boolean_T B_38_763_0 ; boolean_T B_38_789_0 ;
ZCEventType zcEvent ; real_T rtb_B_38_176_0 ; real_T rtb_B_38_177_0 ; real_T
rtb_B_38_207_0 ; real_T rtb_B_38_238_0 ; real_T rtb_B_38_285_0 ; real_T
rtb_B_38_315_0 ; real_T rtb_B_38_355_0 ; real_T rtb_B_38_380_0 ; real_T
rtb_B_38_419_0 ; real_T rtb_B_38_444_0 ; real_T rtb_B_38_480_0 ; real_T
rtb_B_38_563_0 ; real_T rtb_B_38_564_0 ; real_T rtb_B_38_643_0 ; real_T
rtb_B_38_693_0 ; int32_T isHit ; B_nmm2DFullBody_T * _rtB ; P_nmm2DFullBody_T
* _rtP ; X_nmm2DFullBody_T * _rtX ; PrevZCX_nmm2DFullBody_T * _rtZCE ;
DW_nmm2DFullBody_T * _rtDW ; _rtDW = ( ( DW_nmm2DFullBody_T * )
ssGetRootDWork ( S ) ) ; _rtZCE = ( ( PrevZCX_nmm2DFullBody_T * )
_ssGetPrevZCSigState ( S ) ) ; _rtX = ( ( X_nmm2DFullBody_T * )
ssGetContStates ( S ) ) ; _rtP = ( ( P_nmm2DFullBody_T * ) ssGetModelRtp ( S
) ) ; _rtB = ( ( B_nmm2DFullBody_T * ) _ssGetModelBlockIO ( S ) ) ;
ssCallAccelRunBlock ( S , 38 , 1 , SS_CALL_MDL_OUTPUTS ) ; _rtB -> B_38_2_0 [
0 ] = _rtP -> P_11 * _rtB -> B_38_1_0 [ 0 ] ; _rtB -> B_38_4_0 [ 0 ] = _rtP
-> P_12 * _rtB -> B_38_1_0 [ 3 ] ; _rtB -> B_38_2_0 [ 1 ] = _rtP -> P_11 *
_rtB -> B_38_1_0 [ 1 ] ; _rtB -> B_38_4_0 [ 1 ] = _rtP -> P_12 * _rtB ->
B_38_1_0 [ 4 ] ; _rtB -> B_38_2_0 [ 2 ] = _rtP -> P_11 * _rtB -> B_38_1_0 [ 2
] ; _rtB -> B_38_4_0 [ 2 ] = _rtP -> P_12 * _rtB -> B_38_1_0 [ 5 ] ; _rtB ->
B_38_3_0 = _rtB -> B_38_2_0 [ 1 ] - _rtB -> B_38_0_0 ; isHit = ssIsSampleHit
( S , 1 , 0 ) ; if ( isHit != 0 ) { if ( ssIsMajorTimeStep ( S ) != 0 ) {
_rtDW -> Compare_Mode = ( _rtB -> B_38_3_0 <= nmm2DFullBody_rtC ( S ) ->
B_38_5_0 ) ; } _rtB -> B_38_6_0 = _rtDW -> Compare_Mode ; }
nmm2DFullBody_GroundFrictionModel ( S , _rtB -> B_38_6_0 , _rtB -> B_38_3_0 ,
_rtB -> B_38_4_0 [ 1 ] , _rtB -> B_38_2_0 [ 0 ] , _rtB -> B_38_4_0 [ 0 ] , &
_rtB -> GroundFrictionModel_j , & nmm2DFullBody_rtC ( S ) ->
GroundFrictionModel_j , & _rtDW -> GroundFrictionModel_j , & _rtP ->
GroundFrictionModel_j ) ; _rtB -> B_38_9_0 [ 0 ] = _rtP -> P_15 * _rtB ->
B_38_1_0 [ 6 ] ; _rtB -> B_38_11_0 [ 0 ] = _rtP -> P_16 * _rtB -> B_38_1_0 [
9 ] ; _rtB -> B_38_9_0 [ 1 ] = _rtP -> P_15 * _rtB -> B_38_1_0 [ 7 ] ; _rtB
-> B_38_11_0 [ 1 ] = _rtP -> P_16 * _rtB -> B_38_1_0 [ 10 ] ; _rtB ->
B_38_9_0 [ 2 ] = _rtP -> P_15 * _rtB -> B_38_1_0 [ 8 ] ; _rtB -> B_38_11_0 [
2 ] = _rtP -> P_16 * _rtB -> B_38_1_0 [ 11 ] ; _rtB -> B_38_10_0 = _rtB ->
B_38_9_0 [ 1 ] - _rtB -> B_38_8_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if
( isHit != 0 ) { if ( ssIsMajorTimeStep ( S ) != 0 ) { _rtDW ->
Compare_Mode_a = ( _rtB -> B_38_10_0 <= nmm2DFullBody_rtC ( S ) -> B_38_12_0
) ; } _rtB -> B_38_13_0 = _rtDW -> Compare_Mode_a ; }
nmm2DFullBody_GroundFrictionModel ( S , _rtB -> B_38_13_0 , _rtB -> B_38_10_0
, _rtB -> B_38_11_0 [ 1 ] , _rtB -> B_38_9_0 [ 0 ] , _rtB -> B_38_11_0 [ 0 ]
, & _rtB -> GroundFrictionModel_i , & nmm2DFullBody_rtC ( S ) ->
GroundFrictionModel_i , & _rtDW -> GroundFrictionModel_i , & _rtP ->
GroundFrictionModel_i ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_38_17_0 = ( ( ( _rtB -> B_38_6_0 ? ( int32_T ) _rtP -> P_829
: 0 ) != 0 ) || ( ( _rtB -> B_38_13_0 ? ( int32_T ) _rtP -> P_830 : 0 ) != 0
) ) ; } nmm2DFullBody_LeftForcePlate ( S , _rtB -> B_38_17_0 , _rtB ->
GroundFrictionModel_j . B_2_9_0 , _rtB -> GroundFrictionModel_j . B_2_3_0 ,
_rtB -> GroundFrictionModel_i . B_2_9_0 , _rtB -> GroundFrictionModel_i .
B_2_3_0 , & _rtB -> RightForcePlate , & _rtDW -> RightForcePlate , & _rtP ->
RightForcePlate ) ; ssCallAccelRunBlock ( S , 38 , 19 , SS_CALL_MDL_OUTPUTS )
; _rtB -> B_38_25_0 [ 0 ] = _rtB -> B_38_3_0 ; _rtB -> B_38_25_0 [ 1 ] = _rtB
-> B_38_4_0 [ 1 ] ; _rtB -> B_38_25_0 [ 2 ] = _rtB -> B_38_2_0 [ 0 ] ; _rtB
-> B_38_25_0 [ 3 ] = _rtB -> B_38_4_0 [ 0 ] ; ssCallAccelRunBlock ( S , 38 ,
26 , SS_CALL_MDL_OUTPUTS ) ; _rtB -> B_38_35_0 [ 0 ] = _rtP -> P_19 * _rtB ->
B_38_1_0 [ 52 ] ; _rtB -> B_38_37_0 [ 0 ] = _rtP -> P_20 * _rtB -> B_38_1_0 [
55 ] ; _rtB -> B_38_35_0 [ 1 ] = _rtP -> P_19 * _rtB -> B_38_1_0 [ 53 ] ;
_rtB -> B_38_37_0 [ 1 ] = _rtP -> P_20 * _rtB -> B_38_1_0 [ 56 ] ; _rtB ->
B_38_35_0 [ 2 ] = _rtP -> P_19 * _rtB -> B_38_1_0 [ 54 ] ; _rtB -> B_38_37_0
[ 2 ] = _rtP -> P_20 * _rtB -> B_38_1_0 [ 57 ] ; _rtB -> B_38_36_0 = _rtB ->
B_38_35_0 [ 1 ] - _rtB -> B_38_34_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { if ( ssIsMajorTimeStep ( S ) != 0 ) { _rtDW ->
Compare_Mode_d = ( _rtB -> B_38_36_0 <= nmm2DFullBody_rtC ( S ) -> B_38_38_0
) ; } _rtB -> B_38_39_0 = _rtDW -> Compare_Mode_d ; }
nmm2DFullBody_GroundFrictionModel ( S , _rtB -> B_38_39_0 , _rtB -> B_38_36_0
, _rtB -> B_38_37_0 [ 1 ] , _rtB -> B_38_35_0 [ 0 ] , _rtB -> B_38_37_0 [ 0 ]
, & _rtB -> GroundFrictionModel , & nmm2DFullBody_rtC ( S ) ->
GroundFrictionModel , & _rtDW -> GroundFrictionModel , & _rtP ->
GroundFrictionModel ) ; _rtB -> B_38_42_0 [ 0 ] = _rtP -> P_23 * _rtB ->
B_38_1_0 [ 46 ] ; _rtB -> B_38_44_0 [ 0 ] = _rtP -> P_24 * _rtB -> B_38_1_0 [
49 ] ; _rtB -> B_38_42_0 [ 1 ] = _rtP -> P_23 * _rtB -> B_38_1_0 [ 47 ] ;
_rtB -> B_38_44_0 [ 1 ] = _rtP -> P_24 * _rtB -> B_38_1_0 [ 50 ] ; _rtB ->
B_38_42_0 [ 2 ] = _rtP -> P_23 * _rtB -> B_38_1_0 [ 48 ] ; _rtB -> B_38_44_0
[ 2 ] = _rtP -> P_24 * _rtB -> B_38_1_0 [ 51 ] ; _rtB -> B_38_43_0 = _rtB ->
B_38_42_0 [ 1 ] - _rtB -> B_38_41_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { if ( ssIsMajorTimeStep ( S ) != 0 ) { _rtDW ->
Compare_Mode_h = ( _rtB -> B_38_43_0 <= nmm2DFullBody_rtC ( S ) -> B_38_45_0
) ; } _rtB -> B_38_46_0 = _rtDW -> Compare_Mode_h ; }
nmm2DFullBody_GroundFrictionModel ( S , _rtB -> B_38_46_0 , _rtB -> B_38_43_0
, _rtB -> B_38_44_0 [ 1 ] , _rtB -> B_38_42_0 [ 0 ] , _rtB -> B_38_44_0 [ 0 ]
, & _rtB -> GroundFrictionModel_m , & nmm2DFullBody_rtC ( S ) ->
GroundFrictionModel_m , & _rtDW -> GroundFrictionModel_m , & _rtP ->
GroundFrictionModel_m ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_38_50_0 = ( ( ( _rtB -> B_38_39_0 ? ( int32_T ) _rtP -> P_831
: 0 ) != 0 ) || ( ( _rtB -> B_38_46_0 ? ( int32_T ) _rtP -> P_832 : 0 ) != 0
) ) ; } nmm2DFullBody_LeftForcePlate ( S , _rtB -> B_38_50_0 , _rtB ->
GroundFrictionModel . B_2_9_0 , _rtB -> GroundFrictionModel . B_2_3_0 , _rtB
-> GroundFrictionModel_m . B_2_9_0 , _rtB -> GroundFrictionModel_m . B_2_3_0
, & _rtB -> LeftForcePlate , & _rtDW -> LeftForcePlate , & _rtP ->
LeftForcePlate ) ; ssCallAccelRunBlock ( S , 38 , 52 , SS_CALL_MDL_OUTPUTS )
; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
ssCallAccelRunBlock ( S , 38 , 53 , SS_CALL_MDL_OUTPUTS ) ; } _rtB ->
B_38_59_0 [ 0 ] = _rtB -> B_38_36_0 ; _rtB -> B_38_59_0 [ 1 ] = _rtB ->
B_38_37_0 [ 1 ] ; _rtB -> B_38_59_0 [ 2 ] = _rtB -> B_38_35_0 [ 0 ] ; _rtB ->
B_38_59_0 [ 3 ] = _rtB -> B_38_37_0 [ 0 ] ; ssCallAccelRunBlock ( S , 38 , 60
, SS_CALL_MDL_OUTPUTS ) ; _rtB -> B_38_68_0 = _rtP -> P_26 * _rtB -> B_38_1_0
[ 40 ] ; _rtB -> B_38_69_0 = _rtP -> P_27 * _rtB -> B_38_68_0 ; _rtB ->
B_38_71_0 = _rtP -> P_28 * _rtB -> B_38_68_0 ; _rtB -> B_38_72_0 = _rtP ->
P_29 * _rtB -> B_38_1_0 [ 41 ] ; _rtB -> B_38_73_0 = _rtP -> P_30 * _rtB ->
B_38_72_0 ; _rtB -> B_38_74_0 = ( muDoubleScalarSign ( _rtB -> B_38_71_0 ) *
_rtB -> B_38_73_0 + 1.0 ) * - _rtB -> B_38_71_0 ; _rtB -> B_38_80_0 = _rtP ->
P_31 * _rtB -> B_38_1_0 [ 16 ] ; _rtB -> B_38_81_0 = _rtP -> P_32 * _rtB ->
B_38_80_0 ; _rtB -> B_38_83_0 = _rtP -> P_33 * _rtB -> B_38_80_0 ; _rtB ->
B_38_84_0 = _rtP -> P_34 * _rtB -> B_38_1_0 [ 17 ] ; _rtB -> B_38_85_0 = _rtP
-> P_35 * _rtB -> B_38_84_0 ; _rtB -> B_38_86_0 = ( muDoubleScalarSign ( _rtB
-> B_38_83_0 ) * _rtB -> B_38_85_0 + 1.0 ) * - _rtB -> B_38_83_0 ; _rtB ->
B_38_96_0 = _rtP -> P_38 * _rtB -> B_38_1_0 [ 44 ] ; _rtB -> B_38_97_0 = _rtP
-> P_39 * _rtB -> B_38_1_0 [ 45 ] ; _rtB -> B_38_99_0 = ( real_T ) ( _rtB ->
B_38_96_0 >= 0.0 ) * _rtB -> B_38_96_0 + ( 6.2831853071795862 + _rtB ->
B_38_96_0 ) * ( real_T ) ( _rtB -> B_38_96_0 < 0.0 ) ; _rtB -> B_38_101_0 =
_rtP -> P_40 * _rtB -> B_38_1_0 [ 38 ] ; _rtB -> B_38_102_0 = _rtP -> P_41 *
_rtB -> B_38_1_0 [ 39 ] ; _rtB -> B_38_104_0 = ( real_T ) ( _rtB ->
B_38_101_0 >= 0.0 ) * _rtB -> B_38_101_0 + ( 6.2831853071795862 + _rtB ->
B_38_101_0 ) * ( real_T ) ( _rtB -> B_38_101_0 < 0.0 ) ; _rtB -> B_38_92_0 [
0 ] = _rtP -> P_36 * _rtB -> B_38_1_0 [ 20 ] ; _rtB -> B_38_93_0 [ 0 ] = _rtP
-> P_37 * _rtB -> B_38_1_0 [ 23 ] ; _rtB -> B_38_105_0 [ 0 ] = _rtP -> P_42 *
_rtB -> B_38_1_0 [ 32 ] ; _rtB -> B_38_106_0 [ 0 ] = _rtP -> P_43 * _rtB ->
B_38_1_0 [ 35 ] ; _rtB -> B_38_108_0 [ 0 ] = _rtP -> P_44 * _rtB -> B_38_1_0
[ 26 ] ; _rtB -> B_38_109_0 [ 0 ] = _rtP -> P_45 * _rtB -> B_38_1_0 [ 29 ] ;
_rtB -> B_38_92_0 [ 1 ] = _rtP -> P_36 * _rtB -> B_38_1_0 [ 21 ] ; _rtB ->
B_38_93_0 [ 1 ] = _rtP -> P_37 * _rtB -> B_38_1_0 [ 24 ] ; _rtB -> B_38_105_0
[ 1 ] = _rtP -> P_42 * _rtB -> B_38_1_0 [ 33 ] ; _rtB -> B_38_106_0 [ 1 ] =
_rtP -> P_43 * _rtB -> B_38_1_0 [ 36 ] ; _rtB -> B_38_108_0 [ 1 ] = _rtP ->
P_44 * _rtB -> B_38_1_0 [ 27 ] ; _rtB -> B_38_109_0 [ 1 ] = _rtP -> P_45 *
_rtB -> B_38_1_0 [ 30 ] ; _rtB -> B_38_92_0 [ 2 ] = _rtP -> P_36 * _rtB ->
B_38_1_0 [ 22 ] ; _rtB -> B_38_93_0 [ 2 ] = _rtP -> P_37 * _rtB -> B_38_1_0 [
25 ] ; _rtB -> B_38_105_0 [ 2 ] = _rtP -> P_42 * _rtB -> B_38_1_0 [ 34 ] ;
_rtB -> B_38_106_0 [ 2 ] = _rtP -> P_43 * _rtB -> B_38_1_0 [ 37 ] ; _rtB ->
B_38_108_0 [ 2 ] = _rtP -> P_44 * _rtB -> B_38_1_0 [ 28 ] ; _rtB ->
B_38_109_0 [ 2 ] = _rtP -> P_45 * _rtB -> B_38_1_0 [ 31 ] ; _rtB ->
B_38_112_0 = _rtP -> P_46 * _rtB -> B_38_1_0 [ 42 ] ; _rtB -> B_38_113_0 =
_rtP -> P_47 * _rtB -> B_38_1_0 [ 43 ] ; _rtB -> B_38_115_0 = ( real_T ) (
_rtB -> B_38_112_0 >= 0.0 ) * _rtB -> B_38_112_0 + ( 6.2831853071795862 +
_rtB -> B_38_112_0 ) * ( real_T ) ( _rtB -> B_38_112_0 < 0.0 ) ; _rtB ->
B_38_116_0 = _rtP -> P_48 * _rtB -> B_38_1_0 [ 18 ] ; _rtB -> B_38_117_0 =
_rtP -> P_49 * _rtB -> B_38_1_0 [ 19 ] ; _rtB -> B_38_119_0 = ( real_T ) (
_rtB -> B_38_116_0 >= 0.0 ) * _rtB -> B_38_116_0 + ( 6.2831853071795862 +
_rtB -> B_38_116_0 ) * ( real_T ) ( _rtB -> B_38_116_0 < 0.0 ) ; _rtB ->
B_38_120_0 = _rtP -> P_50 * _rtB -> B_38_1_0 [ 14 ] ; _rtB -> B_38_121_0 =
_rtP -> P_51 * _rtB -> B_38_1_0 [ 15 ] ; _rtB -> B_38_123_0 = ( real_T ) (
_rtB -> B_38_120_0 >= 0.0 ) * _rtB -> B_38_120_0 + ( 6.2831853071795862 +
_rtB -> B_38_120_0 ) * ( real_T ) ( _rtB -> B_38_120_0 < 0.0 ) ; _rtB ->
B_38_124_0 = _rtP -> P_52 * _rtB -> B_38_1_0 [ 12 ] ; _rtB -> B_38_125_0 =
_rtP -> P_53 * _rtB -> B_38_1_0 [ 13 ] ; _rtB -> B_38_127_0 = ( real_T ) (
_rtB -> B_38_124_0 >= 0.0 ) * _rtB -> B_38_124_0 + ( 6.2831853071795862 +
_rtB -> B_38_124_0 ) * ( real_T ) ( _rtB -> B_38_124_0 < 0.0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { ssCallAccelRunBlock ( S ,
38 , 131 , SS_CALL_MDL_OUTPUTS ) ; } _rtB -> B_38_133_0 = _rtB -> B_38_127_0
- _rtB -> B_38_132_0 ; _rtB -> B_38_134_0 = _rtP -> P_55 * _rtB -> B_38_133_0
; _rtB -> B_38_135_0 = _rtP -> P_56 * _rtB -> B_38_125_0 ; _rtB -> B_38_136_0
= _rtP -> P_57 * _rtB -> B_38_135_0 ; _rtB -> B_38_139_0 = _rtB -> B_38_127_0
- _rtB -> B_38_138_0 ; _rtB -> B_38_140_0 = _rtP -> P_59 * _rtB -> B_38_139_0
; _rtB -> B_38_143_0 = ( 1.0 - _rtB -> B_38_136_0 ) * - _rtB -> B_38_134_0 *
( real_T ) ( _rtB -> B_38_136_0 < 1.0 ) * ( real_T ) ( _rtB -> B_38_134_0 >
0.0 ) + ( 1.0 - _rtB -> B_38_135_0 ) * - _rtB -> B_38_140_0 * ( real_T ) (
_rtB -> B_38_135_0 < 1.0 ) * ( real_T ) ( _rtB -> B_38_140_0 < 0.0 ) ; _rtB
-> B_38_144_0 = 0.0 * _rtB -> B_38_143_0 ; _rtB -> B_38_149_0 = _rtB ->
B_38_123_0 - _rtB -> B_38_146_0 ; _rtB -> B_38_151_0 = _rtB -> B_38_148_0 -
muDoubleScalarSin ( _rtB -> B_38_149_0 ) ; _rtB -> B_38_153_0 [ 0 ] = _rtP ->
P_62 * _rtB -> B_38_151_0 ; _rtB -> B_38_153_0 [ 1 ] = _rtP -> P_62 *
muDoubleScalarCos ( _rtB -> B_38_149_0 ) ; _rtB -> B_38_154_0 = _rtP -> P_63
* _rtB -> B_38_153_0 [ 0 ] ; _rtB -> B_38_160_0 = _rtB -> B_38_127_0 - _rtB
-> B_38_157_0 ; _rtB -> B_38_162_0 = _rtB -> B_38_159_0 - muDoubleScalarSin (
_rtB -> B_38_160_0 ) ; _rtB -> B_38_164_0 [ 0 ] = _rtP -> P_67 * _rtB ->
B_38_162_0 ; _rtB -> B_38_164_0 [ 1 ] = _rtP -> P_67 * muDoubleScalarCos (
_rtB -> B_38_160_0 ) ; _rtB -> B_38_165_0 = _rtP -> P_68 * _rtB -> B_38_164_0
[ 0 ] ; _rtB -> B_38_166_0 = ( _rtB -> B_38_155_0 - _rtB -> B_38_154_0 ) +
_rtB -> B_38_165_0 ; _rtB -> B_38_168_0 = _rtB -> B_38_166_0 - _rtB ->
B_38_167_0 ; if ( _rtDW -> Integrator_IWORK != 0 ) { _rtX ->
Integrator_CSTATE = _rtB -> B_38_168_0 ; } _rtB -> B_38_169_0 = _rtX ->
Integrator_CSTATE ; _rtB -> B_38_170_0 = _rtB -> B_38_166_0 - _rtB ->
B_38_169_0 ; _rtB -> B_38_171_0 = _rtP -> P_70 * _rtB -> B_38_170_0 ; _rtB ->
B_38_173_0 = _rtB -> B_38_171_0 - _rtB -> B_38_172_0 ; _rtB -> B_38_175_0 =
_rtB -> B_38_173_0 / _rtB -> B_38_174_0 ; rtb_B_38_177_0 = _rtB -> B_38_175_0
* _rtB -> B_38_175_0 * ( real_T ) ( _rtB -> B_38_171_0 > 1.0 ) ; _rtB ->
B_38_178_0 = _rtP -> P_73 * rtb_B_38_177_0 ; _rtB -> B_38_179_0 = _rtB ->
B_38_178_0 ; if ( ssIsMajorTimeStep ( S ) != 0 ) { _rtDW -> Switch_Mode = (
_rtB -> B_38_179_0 > _rtP -> P_75 ) ; } if ( _rtDW -> Switch_Mode ) { _rtB ->
B_38_181_0 = _rtB -> B_38_179_0 ; } else { _rtB -> B_38_181_0 = _rtB ->
B_38_180_0 ; } _rtB -> B_38_183_0 = _rtB -> B_38_181_0 * _rtB -> B_38_164_0 [
1 ] ; _rtB -> B_38_246_0 [ 0 ] = _rtB -> B_38_183_0 ; _rtB -> B_38_190_0 =
_rtB -> B_38_127_0 - _rtB -> B_38_187_0 ; _rtB -> B_38_192_0 = _rtB ->
B_38_189_0 - muDoubleScalarSin ( _rtB -> B_38_190_0 ) ; _rtB -> B_38_194_0 [
0 ] = _rtP -> P_79 * _rtB -> B_38_192_0 ; _rtB -> B_38_194_0 [ 1 ] = _rtP ->
P_79 * muDoubleScalarCos ( _rtB -> B_38_190_0 ) ; _rtB -> B_38_195_0 = _rtP
-> P_80 * _rtB -> B_38_194_0 [ 0 ] ; _rtB -> B_38_196_0 = _rtB -> B_38_185_0
- _rtB -> B_38_195_0 ; _rtB -> B_38_198_0 = _rtB -> B_38_196_0 - _rtB ->
B_38_197_0 ; if ( _rtDW -> Integrator_IWORK_j != 0 ) { _rtX ->
Integrator_CSTATE_m = _rtB -> B_38_198_0 ; } _rtB -> B_38_199_0 = _rtX ->
Integrator_CSTATE_m ; _rtB -> B_38_200_0 = _rtB -> B_38_196_0 - _rtB ->
B_38_199_0 ; _rtB -> B_38_201_0 = _rtP -> P_82 * _rtB -> B_38_200_0 ; _rtB ->
B_38_203_0 = _rtB -> B_38_201_0 - _rtB -> B_38_202_0 ; _rtB -> B_38_205_0 =
_rtB -> B_38_203_0 / _rtB -> B_38_204_0 ; rtb_B_38_207_0 = _rtB -> B_38_205_0
* _rtB -> B_38_205_0 * ( real_T ) ( _rtB -> B_38_201_0 > 1.0 ) ; _rtB ->
B_38_208_0 = _rtP -> P_85 * rtb_B_38_207_0 ; _rtB -> B_38_209_0 = _rtB ->
B_38_208_0 ; if ( ssIsMajorTimeStep ( S ) != 0 ) { _rtDW -> Switch_Mode_m = (
_rtB -> B_38_209_0 > _rtP -> P_87 ) ; } if ( _rtDW -> Switch_Mode_m ) { _rtB
-> B_38_211_0 = _rtB -> B_38_209_0 ; } else { _rtB -> B_38_211_0 = _rtB ->
B_38_210_0 ; } _rtB -> B_38_212_0 = _rtB -> B_38_211_0 * _rtB -> B_38_194_0 [
1 ] ; _rtB -> B_38_214_0 = _rtP -> P_88 * _rtB -> B_38_212_0 ; _rtB ->
B_38_246_0 [ 1 ] = _rtB -> B_38_214_0 ; _rtB -> B_38_220_0 = _rtB ->
B_38_127_0 - _rtB -> B_38_217_0 ; _rtB -> B_38_222_0 = _rtB -> B_38_219_0 -
muDoubleScalarSin ( _rtB -> B_38_220_0 ) ; _rtB -> B_38_224_0 [ 0 ] = _rtP ->
P_91 * _rtB -> B_38_222_0 ; _rtB -> B_38_224_0 [ 1 ] = _rtP -> P_91 *
muDoubleScalarCos ( _rtB -> B_38_220_0 ) ; _rtB -> B_38_226_0 = _rtP -> P_93
* _rtB -> B_38_224_0 [ 0 ] ; _rtB -> B_38_227_0 = _rtB -> B_38_225_0 + _rtB
-> B_38_226_0 ; _rtB -> B_38_229_0 = _rtB -> B_38_227_0 - _rtB -> B_38_228_0
; if ( _rtDW -> Integrator_IWORK_p != 0 ) { _rtX -> Integrator_CSTATE_f =
_rtB -> B_38_229_0 ; } _rtB -> B_38_230_0 = _rtX -> Integrator_CSTATE_f ;
_rtB -> B_38_231_0 = _rtB -> B_38_227_0 - _rtB -> B_38_230_0 ; _rtB ->
B_38_232_0 = _rtP -> P_95 * _rtB -> B_38_231_0 ; _rtB -> B_38_234_0 = _rtB ->
B_38_232_0 - _rtB -> B_38_233_0 ; _rtB -> B_38_236_0 = _rtB -> B_38_234_0 /
_rtB -> B_38_235_0 ; rtb_B_38_238_0 = _rtB -> B_38_236_0 * _rtB -> B_38_236_0
* ( real_T ) ( _rtB -> B_38_232_0 > 1.0 ) ; _rtB -> B_38_239_0 = _rtP -> P_98
* rtb_B_38_238_0 ; _rtB -> B_38_240_0 = _rtB -> B_38_239_0 ; if (
ssIsMajorTimeStep ( S ) != 0 ) { _rtDW -> Switch_Mode_ma = ( _rtB ->
B_38_240_0 > _rtP -> P_100 ) ; } if ( _rtDW -> Switch_Mode_ma ) { _rtB ->
B_38_242_0 = _rtB -> B_38_240_0 ; } else { _rtB -> B_38_242_0 = _rtB ->
B_38_241_0 ; } _rtB -> B_38_244_0 = _rtB -> B_38_224_0 [ 1 ] * _rtB ->
B_38_242_0 ; _rtB -> B_38_246_0 [ 2 ] = _rtB -> B_38_244_0 ; _rtB ->
B_38_247_0 [ 0 ] = _rtP -> P_101 * _rtB -> B_38_246_0 [ 0 ] ; _rtB ->
B_38_247_0 [ 1 ] = _rtP -> P_101 * _rtB -> B_38_246_0 [ 1 ] ; _rtB ->
B_38_247_0 [ 2 ] = _rtP -> P_101 * _rtB -> B_38_246_0 [ 2 ] ; _rtB ->
B_38_248_0 = ( _rtB -> B_38_247_0 [ 0 ] + _rtB -> B_38_247_0 [ 1 ] ) + _rtB
-> B_38_247_0 [ 2 ] ; _rtB -> B_38_249_0 = _rtB -> B_38_144_0 + _rtB ->
B_38_248_0 ; _rtB -> B_38_252_0 = _rtB -> B_38_123_0 - _rtB -> B_38_251_0 ;
_rtB -> B_38_253_0 = _rtP -> P_104 * _rtB -> B_38_252_0 ; _rtB -> B_38_254_0
= _rtP -> P_105 * _rtB -> B_38_121_0 ; _rtB -> B_38_255_0 = _rtP -> P_106 *
_rtB -> B_38_254_0 ; _rtB -> B_38_257_0 = ( 1.0 - _rtB -> B_38_255_0 ) * -
_rtB -> B_38_253_0 * ( real_T ) ( _rtB -> B_38_255_0 < 1.0 ) * ( real_T ) (
_rtB -> B_38_253_0 > 0.0 ) * _rtB -> B_38_250_0 ; _rtB -> B_38_259_0 = _rtB
-> B_38_119_0 - _rtB -> B_38_258_0 ; _rtB -> B_38_261_0 = _rtB -> B_38_259_0
* _rtB -> B_38_260_0 ; _rtB -> B_38_262_0 = _rtP -> P_109 * _rtB ->
B_38_261_0 ; _rtB -> B_38_268_0 = _rtB -> B_38_123_0 - _rtB -> B_38_265_0 ;
_rtB -> B_38_270_0 = _rtB -> B_38_267_0 - muDoubleScalarSin ( _rtB ->
B_38_268_0 ) ; _rtB -> B_38_272_0 [ 0 ] = _rtP -> P_113 * _rtB -> B_38_270_0
; _rtB -> B_38_272_0 [ 1 ] = _rtP -> P_113 * muDoubleScalarCos ( _rtB ->
B_38_268_0 ) ; _rtB -> B_38_273_0 = _rtP -> P_114 * _rtB -> B_38_272_0 [ 0 ]
; _rtB -> B_38_274_0 = ( _rtB -> B_38_263_0 - _rtB -> B_38_262_0 ) - _rtB ->
B_38_273_0 ; _rtB -> B_38_276_0 = _rtB -> B_38_274_0 - _rtB -> B_38_275_0 ;
if ( _rtDW -> Integrator_IWORK_f != 0 ) { _rtX -> Integrator_CSTATE_k = _rtB
-> B_38_276_0 ; } _rtB -> B_38_277_0 = _rtX -> Integrator_CSTATE_k ; _rtB ->
B_38_278_0 = _rtB -> B_38_274_0 - _rtB -> B_38_277_0 ; _rtB -> B_38_279_0 =
_rtP -> P_116 * _rtB -> B_38_278_0 ; _rtB -> B_38_281_0 = _rtB -> B_38_279_0
- _rtB -> B_38_280_0 ; _rtB -> B_38_283_0 = _rtB -> B_38_281_0 / _rtB ->
B_38_282_0 ; rtb_B_38_285_0 = _rtB -> B_38_283_0 * _rtB -> B_38_283_0 * (
real_T ) ( _rtB -> B_38_279_0 > 1.0 ) ; _rtB -> B_38_286_0 = _rtP -> P_119 *
rtb_B_38_285_0 ; _rtB -> B_38_287_0 = _rtB -> B_38_286_0 ; if (
ssIsMajorTimeStep ( S ) != 0 ) { _rtDW -> Switch_Mode_g = ( _rtB ->
B_38_287_0 > _rtP -> P_121 ) ; } if ( _rtDW -> Switch_Mode_g ) { _rtB ->
B_38_289_0 = _rtB -> B_38_287_0 ; } else { _rtB -> B_38_289_0 = _rtB ->
B_38_288_0 ; } _rtB -> B_38_291_0 = _rtB -> B_38_289_0 * _rtB -> B_38_272_0 [
1 ] ; _rtB -> B_38_327_0 [ 0 ] = _rtB -> B_38_291_0 ; _rtB -> B_38_297_0 =
_rtB -> B_38_123_0 - _rtB -> B_38_294_0 ; _rtB -> B_38_299_0 = _rtB ->
B_38_296_0 - muDoubleScalarSin ( _rtB -> B_38_297_0 ) ; _rtB -> B_38_301_0 [
0 ] = _rtP -> P_124 * _rtB -> B_38_299_0 ; _rtB -> B_38_301_0 [ 1 ] = _rtP ->
P_124 * muDoubleScalarCos ( _rtB -> B_38_297_0 ) ; _rtB -> B_38_303_0 = _rtP
-> P_126 * _rtB -> B_38_301_0 [ 0 ] ; _rtB -> B_38_304_0 = _rtB -> B_38_302_0
+ _rtB -> B_38_303_0 ; _rtB -> B_38_306_0 = _rtB -> B_38_304_0 - _rtB ->
B_38_305_0 ; if ( _rtDW -> Integrator_IWORK_i != 0 ) { _rtX ->
Integrator_CSTATE_b = _rtB -> B_38_306_0 ; } _rtB -> B_38_307_0 = _rtX ->
Integrator_CSTATE_b ; _rtB -> B_38_308_0 = _rtB -> B_38_304_0 - _rtB ->
B_38_307_0 ; _rtB -> B_38_309_0 = _rtP -> P_128 * _rtB -> B_38_308_0 ; _rtB
-> B_38_311_0 = _rtB -> B_38_309_0 - _rtB -> B_38_310_0 ; _rtB -> B_38_313_0
= _rtB -> B_38_311_0 / _rtB -> B_38_312_0 ; rtb_B_38_315_0 = _rtB ->
B_38_313_0 * _rtB -> B_38_313_0 * ( real_T ) ( _rtB -> B_38_309_0 > 1.0 ) ;
_rtB -> B_38_316_0 = _rtP -> P_131 * rtb_B_38_315_0 ; _rtB -> B_38_317_0 =
_rtB -> B_38_316_0 ; if ( ssIsMajorTimeStep ( S ) != 0 ) { _rtDW ->
Switch_Mode_c = ( _rtB -> B_38_317_0 > _rtP -> P_133 ) ; } if ( _rtDW ->
Switch_Mode_c ) { _rtB -> B_38_319_0 = _rtB -> B_38_317_0 ; } else { _rtB ->
B_38_319_0 = _rtB -> B_38_318_0 ; } _rtB -> B_38_320_0 = _rtB -> B_38_301_0 [
1 ] * _rtB -> B_38_319_0 ; _rtB -> B_38_322_0 = _rtP -> P_134 * _rtB ->
B_38_320_0 ; _rtB -> B_38_327_0 [ 1 ] = _rtB -> B_38_322_0 ; _rtB ->
B_38_325_0 = _rtB -> B_38_181_0 * _rtB -> B_38_153_0 [ 1 ] ; _rtB ->
B_38_327_0 [ 2 ] = _rtB -> B_38_325_0 ; _rtB -> B_38_328_0 = ( _rtB ->
B_38_327_0 [ 0 ] + _rtB -> B_38_327_0 [ 1 ] ) + _rtB -> B_38_327_0 [ 2 ] ;
_rtB -> B_38_329_0 = _rtP -> P_135 * _rtB -> B_38_328_0 ; _rtB -> B_38_330_0
= _rtB -> B_38_257_0 + _rtB -> B_38_329_0 ; _rtB -> B_38_332_0 = _rtB ->
B_38_119_0 - _rtB -> B_38_331_0 ; _rtB -> B_38_333_0 = _rtP -> P_137 * _rtB
-> B_38_332_0 ; _rtB -> B_38_334_0 = _rtP -> P_138 * _rtB -> B_38_117_0 ;
_rtB -> B_38_335_0 = _rtP -> P_139 * _rtB -> B_38_334_0 ; _rtB -> B_38_337_0
= ( 1.0 - _rtB -> B_38_335_0 ) * - _rtB -> B_38_333_0 * ( real_T ) ( _rtB ->
B_38_335_0 < 1.0 ) * ( real_T ) ( _rtB -> B_38_333_0 > 0.0 ) * _rtB ->
B_38_250_0 ; _rtB -> B_38_340_0 = _rtB -> B_38_119_0 - _rtB -> B_38_339_0 ;
_rtB -> B_38_342_0 = _rtB -> B_38_340_0 * _rtB -> B_38_341_0 ; _rtB ->
B_38_343_0 = _rtP -> P_143 * _rtB -> B_38_342_0 ; _rtB -> B_38_344_0 = _rtB
-> B_38_338_0 - _rtB -> B_38_343_0 ; _rtB -> B_38_346_0 = _rtB -> B_38_344_0
- _rtB -> B_38_345_0 ; if ( _rtDW -> Integrator_IWORK_o != 0 ) { _rtX ->
Integrator_CSTATE_p = _rtB -> B_38_346_0 ; } _rtB -> B_38_347_0 = _rtX ->
Integrator_CSTATE_p ; _rtB -> B_38_348_0 = _rtB -> B_38_344_0 - _rtB ->
B_38_347_0 ; _rtB -> B_38_349_0 = _rtP -> P_145 * _rtB -> B_38_348_0 ; _rtB
-> B_38_351_0 = _rtB -> B_38_349_0 - _rtB -> B_38_350_0 ; _rtB -> B_38_353_0
= _rtB -> B_38_351_0 / _rtB -> B_38_352_0 ; rtb_B_38_355_0 = _rtB ->
B_38_353_0 * _rtB -> B_38_353_0 * ( real_T ) ( _rtB -> B_38_349_0 > 1.0 ) ;
_rtB -> B_38_356_0 = _rtP -> P_148 * rtb_B_38_355_0 ; _rtB -> B_38_357_0 =
_rtB -> B_38_356_0 ; if ( ssIsMajorTimeStep ( S ) != 0 ) { _rtDW ->
Switch_Mode_a = ( _rtB -> B_38_357_0 > _rtP -> P_150 ) ; } if ( _rtDW ->
Switch_Mode_a ) { _rtB -> B_38_359_0 = _rtB -> B_38_357_0 ; } else { _rtB ->
B_38_359_0 = _rtB -> B_38_358_0 ; } _rtB -> B_38_361_0 = _rtB -> B_38_359_0 *
_rtB -> B_38_341_0 ; _rtB -> B_38_392_0 [ 0 ] = _rtB -> B_38_361_0 ; _rtB ->
B_38_366_0 = _rtB -> B_38_119_0 - _rtB -> B_38_365_0 ; _rtB -> B_38_367_0 =
_rtB -> B_38_366_0 * _rtB -> B_38_363_0 ; _rtB -> B_38_368_0 = _rtP -> P_154
* _rtB -> B_38_367_0 ; _rtB -> B_38_369_0 = _rtB -> B_38_364_0 + _rtB ->
B_38_368_0 ; _rtB -> B_38_371_0 = _rtB -> B_38_369_0 - _rtB -> B_38_370_0 ;
if ( _rtDW -> Integrator_IWORK_pr != 0 ) { _rtX -> Integrator_CSTATE_j = _rtB
-> B_38_371_0 ; } _rtB -> B_38_372_0 = _rtX -> Integrator_CSTATE_j ; _rtB ->
B_38_373_0 = _rtB -> B_38_369_0 - _rtB -> B_38_372_0 ; _rtB -> B_38_374_0 =
_rtP -> P_156 * _rtB -> B_38_373_0 ; _rtB -> B_38_376_0 = _rtB -> B_38_374_0
- _rtB -> B_38_375_0 ; _rtB -> B_38_378_0 = _rtB -> B_38_376_0 / _rtB ->
B_38_377_0 ; rtb_B_38_380_0 = _rtB -> B_38_378_0 * _rtB -> B_38_378_0 * (
real_T ) ( _rtB -> B_38_374_0 > 1.0 ) ; _rtB -> B_38_381_0 = _rtP -> P_159 *
rtb_B_38_380_0 ; _rtB -> B_38_382_0 = _rtB -> B_38_381_0 ; if (
ssIsMajorTimeStep ( S ) != 0 ) { _rtDW -> Switch_Mode_d = ( _rtB ->
B_38_382_0 > _rtP -> P_161 ) ; } if ( _rtDW -> Switch_Mode_d ) { _rtB ->
B_38_384_0 = _rtB -> B_38_382_0 ; } else { _rtB -> B_38_384_0 = _rtB ->
B_38_383_0 ; } _rtB -> B_38_385_0 = _rtB -> B_38_363_0 * _rtB -> B_38_384_0 ;
_rtB -> B_38_387_0 = _rtP -> P_162 * _rtB -> B_38_385_0 ; _rtB -> B_38_392_0
[ 1 ] = _rtB -> B_38_387_0 ; _rtB -> B_38_390_0 = _rtB -> B_38_289_0 * _rtB
-> B_38_260_0 ; _rtB -> B_38_392_0 [ 2 ] = _rtB -> B_38_390_0 ; _rtB ->
B_38_393_0 = ( _rtB -> B_38_392_0 [ 0 ] + _rtB -> B_38_392_0 [ 1 ] ) + _rtB
-> B_38_392_0 [ 2 ] ; _rtB -> B_38_394_0 = _rtB -> B_38_337_0 + _rtB ->
B_38_393_0 ; _rtB -> B_38_396_0 = _rtB -> B_38_104_0 - _rtB -> B_38_395_0 ;
_rtB -> B_38_397_0 = _rtP -> P_164 * _rtB -> B_38_396_0 ; _rtB -> B_38_398_0
= _rtP -> P_165 * _rtB -> B_38_102_0 ; _rtB -> B_38_399_0 = _rtP -> P_166 *
_rtB -> B_38_398_0 ; _rtB -> B_38_401_0 = ( 1.0 - _rtB -> B_38_399_0 ) * -
_rtB -> B_38_397_0 * ( real_T ) ( _rtB -> B_38_399_0 < 1.0 ) * ( real_T ) (
_rtB -> B_38_397_0 > 0.0 ) * _rtB -> B_38_250_0 ; _rtB -> B_38_404_0 = _rtB
-> B_38_104_0 - _rtB -> B_38_403_0 ; _rtB -> B_38_406_0 = _rtB -> B_38_404_0
* _rtB -> B_38_405_0 ; _rtB -> B_38_407_0 = _rtP -> P_170 * _rtB ->
B_38_406_0 ; _rtB -> B_38_408_0 = _rtB -> B_38_402_0 - _rtB -> B_38_407_0 ;
_rtB -> B_38_410_0 = _rtB -> B_38_408_0 - _rtB -> B_38_409_0 ; if ( _rtDW ->
Integrator_IWORK_g != 0 ) { _rtX -> Integrator_CSTATE_js = _rtB -> B_38_410_0
; } _rtB -> B_38_411_0 = _rtX -> Integrator_CSTATE_js ; _rtB -> B_38_412_0 =
_rtB -> B_38_408_0 - _rtB -> B_38_411_0 ; _rtB -> B_38_413_0 = _rtP -> P_172
* _rtB -> B_38_412_0 ; _rtB -> B_38_415_0 = _rtB -> B_38_413_0 - _rtB ->
B_38_414_0 ; _rtB -> B_38_417_0 = _rtB -> B_38_415_0 / _rtB -> B_38_416_0 ;
rtb_B_38_419_0 = _rtB -> B_38_417_0 * _rtB -> B_38_417_0 * ( real_T ) ( _rtB
-> B_38_413_0 > 1.0 ) ; _rtB -> B_38_420_0 = _rtP -> P_175 * rtb_B_38_419_0 ;
_rtB -> B_38_421_0 = _rtB -> B_38_420_0 ; if ( ssIsMajorTimeStep ( S ) != 0 )
{ _rtDW -> Switch_Mode_p = ( _rtB -> B_38_421_0 > _rtP -> P_177 ) ; } if (
_rtDW -> Switch_Mode_p ) { _rtB -> B_38_423_0 = _rtB -> B_38_421_0 ; } else {
_rtB -> B_38_423_0 = _rtB -> B_38_422_0 ; } _rtB -> B_38_425_0 = _rtB ->
B_38_423_0 * _rtB -> B_38_405_0 ; _rtB -> B_38_488_0 [ 0 ] = _rtB ->
B_38_425_0 ; _rtB -> B_38_430_0 = _rtB -> B_38_104_0 - _rtB -> B_38_429_0 ;
_rtB -> B_38_431_0 = _rtB -> B_38_430_0 * _rtB -> B_38_427_0 ; _rtB ->
B_38_432_0 = _rtP -> P_181 * _rtB -> B_38_431_0 ; _rtB -> B_38_433_0 = _rtB
-> B_38_428_0 + _rtB -> B_38_432_0 ; _rtB -> B_38_435_0 = _rtB -> B_38_433_0
- _rtB -> B_38_434_0 ; if ( _rtDW -> Integrator_IWORK_h != 0 ) { _rtX ->
Integrator_CSTATE_h = _rtB -> B_38_435_0 ; } _rtB -> B_38_436_0 = _rtX ->
Integrator_CSTATE_h ; _rtB -> B_38_437_0 = _rtB -> B_38_433_0 - _rtB ->
B_38_436_0 ; _rtB -> B_38_438_0 = _rtP -> P_183 * _rtB -> B_38_437_0 ; _rtB
-> B_38_440_0 = _rtB -> B_38_438_0 - _rtB -> B_38_439_0 ; _rtB -> B_38_442_0
= _rtB -> B_38_440_0 / _rtB -> B_38_441_0 ; rtb_B_38_444_0 = _rtB ->
B_38_442_0 * _rtB -> B_38_442_0 * ( real_T ) ( _rtB -> B_38_438_0 > 1.0 ) ;
_rtB -> B_38_445_0 = _rtP -> P_186 * rtb_B_38_444_0 ; _rtB -> B_38_446_0 =
_rtB -> B_38_445_0 ; if ( ssIsMajorTimeStep ( S ) != 0 ) { _rtDW ->
Switch_Mode_aw = ( _rtB -> B_38_446_0 > _rtP -> P_188 ) ; } if ( _rtDW ->
Switch_Mode_aw ) { _rtB -> B_38_448_0 = _rtB -> B_38_446_0 ; } else { _rtB ->
B_38_448_0 = _rtB -> B_38_447_0 ; } _rtB -> B_38_449_0 = _rtB -> B_38_427_0 *
_rtB -> B_38_448_0 ; _rtB -> B_38_451_0 = _rtP -> P_189 * _rtB -> B_38_449_0
; _rtB -> B_38_488_0 [ 1 ] = _rtB -> B_38_451_0 ; _rtB -> B_38_454_0 = _rtB
-> B_38_104_0 - _rtB -> B_38_453_0 ; _rtB -> B_38_456_0 = _rtB -> B_38_454_0
* _rtB -> B_38_455_0 ; _rtB -> B_38_457_0 = _rtP -> P_192 * _rtB ->
B_38_456_0 ; _rtB -> B_38_463_0 = _rtB -> B_38_115_0 - _rtB -> B_38_460_0 ;
_rtB -> B_38_465_0 = _rtB -> B_38_462_0 - muDoubleScalarSin ( _rtB ->
B_38_463_0 ) ; _rtB -> B_38_467_0 [ 0 ] = _rtP -> P_196 * _rtB -> B_38_465_0
; _rtB -> B_38_467_0 [ 1 ] = _rtP -> P_196 * muDoubleScalarCos ( _rtB ->
B_38_463_0 ) ; _rtB -> B_38_468_0 = _rtP -> P_197 * _rtB -> B_38_467_0 [ 0 ]
; _rtB -> B_38_469_0 = ( _rtB -> B_38_458_0 - _rtB -> B_38_457_0 ) - _rtB ->
B_38_468_0 ; _rtB -> B_38_471_0 = _rtB -> B_38_469_0 - _rtB -> B_38_470_0 ;
if ( _rtDW -> Integrator_IWORK_hf != 0 ) { _rtX -> Integrator_CSTATE_i = _rtB
-> B_38_471_0 ; } _rtB -> B_38_472_0 = _rtX -> Integrator_CSTATE_i ; _rtB ->
B_38_473_0 = _rtB -> B_38_469_0 - _rtB -> B_38_472_0 ; _rtB -> B_38_474_0 =
_rtP -> P_199 * _rtB -> B_38_473_0 ; _rtB -> B_38_476_0 = _rtB -> B_38_474_0
- _rtB -> B_38_475_0 ; _rtB -> B_38_478_0 = _rtB -> B_38_476_0 / _rtB ->
B_38_477_0 ; rtb_B_38_480_0 = _rtB -> B_38_478_0 * _rtB -> B_38_478_0 * (
real_T ) ( _rtB -> B_38_474_0 > 1.0 ) ; _rtB -> B_38_481_0 = _rtP -> P_202 *
rtb_B_38_480_0 ; _rtB -> B_38_482_0 = _rtB -> B_38_481_0 ; if (
ssIsMajorTimeStep ( S ) != 0 ) { _rtDW -> Switch_Mode_b = ( _rtB ->
B_38_482_0 > _rtP -> P_204 ) ; } if ( _rtDW -> Switch_Mode_b ) { _rtB ->
B_38_484_0 = _rtB -> B_38_482_0 ; } else { _rtB -> B_38_484_0 = _rtB ->
B_38_483_0 ; } _rtB -> B_38_486_0 = _rtB -> B_38_484_0 * _rtB -> B_38_455_0 ;
_rtB -> B_38_488_0 [ 2 ] = _rtB -> B_38_486_0 ; _rtB -> B_38_489_0 = ( _rtB
-> B_38_488_0 [ 0 ] + _rtB -> B_38_488_0 [ 1 ] ) + _rtB -> B_38_488_0 [ 2 ] ;
_rtB -> B_38_490_0 = _rtB -> B_38_401_0 + _rtB -> B_38_489_0 ; _rtB ->
B_38_492_0 = _rtB -> B_38_115_0 - _rtB -> B_38_491_0 ; _rtB -> B_38_493_0 =
_rtP -> P_206 * _rtB -> B_38_492_0 ; _rtB -> B_38_494_0 = _rtP -> P_207 *
_rtB -> B_38_113_0 ; _rtB -> B_38_495_0 = _rtP -> P_208 * _rtB -> B_38_494_0
; _rtB -> B_38_497_0 = ( 1.0 - _rtB -> B_38_495_0 ) * - _rtB -> B_38_493_0 *
( real_T ) ( _rtB -> B_38_495_0 < 1.0 ) * ( real_T ) ( _rtB -> B_38_493_0 >
0.0 ) * _rtB -> B_38_250_0 ; _rtB -> B_38_499_0 = _rtB -> B_38_484_0 * _rtB
-> B_38_467_0 [ 1 ] ; _rtB -> B_38_572_0 [ 0 ] = _rtB -> B_38_499_0 ; _rtB ->
B_38_505_0 = _rtB -> B_38_115_0 - _rtB -> B_38_502_0 ; _rtB -> B_38_507_0 =
_rtB -> B_38_504_0 - muDoubleScalarSin ( _rtB -> B_38_505_0 ) ; _rtB ->
B_38_509_0 [ 0 ] = _rtP -> P_211 * _rtB -> B_38_507_0 ; _rtB -> B_38_509_0 [
1 ] = _rtP -> P_211 * muDoubleScalarCos ( _rtB -> B_38_505_0 ) ; _rtB ->
B_38_511_0 = _rtP -> P_213 * _rtB -> B_38_509_0 [ 0 ] ; _rtB -> B_38_512_0 =
_rtB -> B_38_510_0 + _rtB -> B_38_511_0 ; _rtB -> B_38_514_0 = _rtB ->
B_38_512_0 - _rtB -> B_38_513_0 ; if ( _rtDW -> Integrator_IWORK_m != 0 ) {
_rtX -> Integrator_CSTATE_hj = _rtB -> B_38_514_0 ; } _rtB -> B_38_515_0 =
_rtX -> Integrator_CSTATE_hj ; _rtB -> B_38_516_0 = _rtB -> B_38_512_0 - _rtB
-> B_38_515_0 ; _rtB -> B_38_517_0 = _rtP -> P_215 * _rtB -> B_38_516_0 ;
_rtB -> B_38_519_0 = _rtB -> B_38_517_0 - _rtB -> B_38_518_0 ; _rtB ->
B_38_521_0 = _rtB -> B_38_519_0 / _rtB -> B_38_520_0 ; rtb_B_38_176_0 = _rtB
-> B_38_521_0 * _rtB -> B_38_521_0 * ( real_T ) ( _rtB -> B_38_517_0 > 1.0 )
; _rtB -> B_38_524_0 = _rtP -> P_218 * rtb_B_38_176_0 ; _rtB -> B_38_525_0 =
_rtB -> B_38_524_0 ; if ( ssIsMajorTimeStep ( S ) != 0 ) { _rtDW ->
Switch_Mode_k = ( _rtB -> B_38_525_0 > _rtP -> P_220 ) ; } if ( _rtDW ->
Switch_Mode_k ) { _rtB -> B_38_527_0 = _rtB -> B_38_525_0 ; } else { _rtB ->
B_38_527_0 = _rtB -> B_38_526_0 ; } _rtB -> B_38_528_0 = _rtB -> B_38_509_0 [
1 ] * _rtB -> B_38_527_0 ; _rtB -> B_38_530_0 = _rtP -> P_221 * _rtB ->
B_38_528_0 ; _rtB -> B_38_572_0 [ 1 ] = _rtB -> B_38_530_0 ; _rtB ->
B_38_536_0 = _rtB -> B_38_115_0 - _rtB -> B_38_533_0 ; _rtB -> B_38_538_0 =
_rtB -> B_38_535_0 - muDoubleScalarSin ( _rtB -> B_38_536_0 ) ; _rtB ->
B_38_540_0 [ 0 ] = _rtP -> P_224 * _rtB -> B_38_538_0 ; _rtB -> B_38_540_0 [
1 ] = _rtP -> P_224 * muDoubleScalarCos ( _rtB -> B_38_536_0 ) ; _rtB ->
B_38_541_0 = _rtP -> P_225 * _rtB -> B_38_540_0 [ 0 ] ; _rtB -> B_38_547_0 =
_rtB -> B_38_99_0 - _rtB -> B_38_544_0 ; _rtB -> B_38_549_0 = _rtB ->
B_38_546_0 - muDoubleScalarSin ( _rtB -> B_38_547_0 ) ; _rtB -> B_38_551_0 [
0 ] = _rtP -> P_229 * _rtB -> B_38_549_0 ; _rtB -> B_38_551_0 [ 1 ] = _rtP ->
P_229 * muDoubleScalarCos ( _rtB -> B_38_547_0 ) ; _rtB -> B_38_552_0 = _rtP
-> P_230 * _rtB -> B_38_551_0 [ 0 ] ; _rtB -> B_38_553_0 = ( _rtB ->
B_38_542_0 - _rtB -> B_38_541_0 ) + _rtB -> B_38_552_0 ; _rtB -> B_38_555_0 =
_rtB -> B_38_553_0 - _rtB -> B_38_554_0 ; if ( _rtDW -> Integrator_IWORK_jn
!= 0 ) { _rtX -> Integrator_CSTATE_n = _rtB -> B_38_555_0 ; } _rtB ->
B_38_556_0 = _rtX -> Integrator_CSTATE_n ; _rtB -> B_38_557_0 = _rtB ->
B_38_553_0 - _rtB -> B_38_556_0 ; _rtB -> B_38_558_0 = _rtP -> P_232 * _rtB
-> B_38_557_0 ; _rtB -> B_38_560_0 = _rtB -> B_38_558_0 - _rtB -> B_38_559_0
; _rtB -> B_38_562_0 = _rtB -> B_38_560_0 / _rtB -> B_38_561_0 ;
rtb_B_38_564_0 = _rtB -> B_38_562_0 * _rtB -> B_38_562_0 * ( real_T ) ( _rtB
-> B_38_558_0 > 1.0 ) ; _rtB -> B_38_565_0 = _rtP -> P_235 * rtb_B_38_564_0 ;
_rtB -> B_38_566_0 = _rtB -> B_38_565_0 ; if ( ssIsMajorTimeStep ( S ) != 0 )
{ _rtDW -> Switch_Mode_mf = ( _rtB -> B_38_566_0 > _rtP -> P_237 ) ; } if (
_rtDW -> Switch_Mode_mf ) { _rtB -> B_38_568_0 = _rtB -> B_38_566_0 ; } else
{ _rtB -> B_38_568_0 = _rtB -> B_38_567_0 ; } _rtB -> B_38_570_0 = _rtB ->
B_38_568_0 * _rtB -> B_38_540_0 [ 1 ] ; _rtB -> B_38_572_0 [ 2 ] = _rtB ->
B_38_570_0 ; _rtB -> B_38_573_0 = ( _rtB -> B_38_572_0 [ 0 ] + _rtB ->
B_38_572_0 [ 1 ] ) + _rtB -> B_38_572_0 [ 2 ] ; _rtB -> B_38_574_0 = _rtP ->
P_238 * _rtB -> B_38_573_0 ; _rtB -> B_38_575_0 = _rtB -> B_38_497_0 + _rtB
-> B_38_574_0 ; _rtB -> B_38_577_0 = _rtB -> B_38_99_0 - _rtB -> B_38_576_0 ;
_rtB -> B_38_578_0 = _rtP -> P_240 * _rtB -> B_38_577_0 ; _rtB -> B_38_579_0
= _rtP -> P_241 * _rtB -> B_38_97_0 ; _rtB -> B_38_580_0 = _rtP -> P_242 *
_rtB -> B_38_579_0 ; _rtB -> B_38_583_0 = _rtB -> B_38_99_0 - _rtB ->
B_38_582_0 ; _rtB -> B_38_584_0 = _rtP -> P_244 * _rtB -> B_38_583_0 ; _rtB
-> B_38_586_0 = ( 1.0 - _rtB -> B_38_580_0 ) * - _rtB -> B_38_578_0 * (
real_T ) ( _rtB -> B_38_580_0 < 1.0 ) * ( real_T ) ( _rtB -> B_38_578_0 > 0.0
) + ( 1.0 - _rtB -> B_38_579_0 ) * - _rtB -> B_38_584_0 * ( real_T ) ( _rtB
-> B_38_579_0 < 1.0 ) * ( real_T ) ( _rtB -> B_38_584_0 < 0.0 ) ; _rtB ->
B_38_587_0 = _rtB -> B_38_250_0 * _rtB -> B_38_586_0 ; _rtB -> B_38_589_0 =
_rtB -> B_38_568_0 * _rtB -> B_38_551_0 [ 1 ] ; _rtB -> B_38_652_0 [ 0 ] =
_rtB -> B_38_589_0 ; _rtB -> B_38_596_0 = _rtB -> B_38_99_0 - _rtB ->
B_38_593_0 ; _rtB -> B_38_598_0 = _rtB -> B_38_595_0 - muDoubleScalarSin (
_rtB -> B_38_596_0 ) ; _rtB -> B_38_600_0 [ 0 ] = _rtP -> P_248 * _rtB ->
B_38_598_0 ; _rtB -> B_38_600_0 [ 1 ] = _rtP -> P_248 * muDoubleScalarCos (
_rtB -> B_38_596_0 ) ; _rtB -> B_38_601_0 = _rtP -> P_249 * _rtB ->
B_38_600_0 [ 0 ] ; _rtB -> B_38_602_0 = _rtB -> B_38_591_0 - _rtB ->
B_38_601_0 ; _rtB -> B_38_604_0 = _rtB -> B_38_602_0 - _rtB -> B_38_603_0 ;
if ( _rtDW -> Integrator_IWORK_pk != 0 ) { _rtX -> Integrator_CSTATE_bt =
_rtB -> B_38_604_0 ; } _rtB -> B_38_605_0 = _rtX -> Integrator_CSTATE_bt ;
_rtB -> B_38_606_0 = _rtB -> B_38_602_0 - _rtB -> B_38_605_0 ; _rtB ->
B_38_607_0 = _rtP -> P_251 * _rtB -> B_38_606_0 ; _rtB -> B_38_609_0 = _rtB
-> B_38_607_0 - _rtB -> B_38_608_0 ; _rtB -> B_38_611_0 = _rtB -> B_38_609_0
/ _rtB -> B_38_610_0 ; rtb_B_38_563_0 = _rtB -> B_38_611_0 * _rtB ->
B_38_611_0 * ( real_T ) ( _rtB -> B_38_607_0 > 1.0 ) ; _rtB -> B_38_614_0 =
_rtP -> P_254 * rtb_B_38_563_0 ; _rtB -> B_38_615_0 = _rtB -> B_38_614_0 ; if
( ssIsMajorTimeStep ( S ) != 0 ) { _rtDW -> Switch_Mode_o = ( _rtB ->
B_38_615_0 > _rtP -> P_256 ) ; } if ( _rtDW -> Switch_Mode_o ) { _rtB ->
B_38_617_0 = _rtB -> B_38_615_0 ; } else { _rtB -> B_38_617_0 = _rtB ->
B_38_616_0 ; } _rtB -> B_38_618_0 = _rtB -> B_38_617_0 * _rtB -> B_38_600_0 [
1 ] ; _rtB -> B_38_620_0 = _rtP -> P_257 * _rtB -> B_38_618_0 ; _rtB ->
B_38_652_0 [ 1 ] = _rtB -> B_38_620_0 ; _rtB -> B_38_626_0 = _rtB ->
B_38_99_0 - _rtB -> B_38_623_0 ; _rtB -> B_38_628_0 = _rtB -> B_38_625_0 -
muDoubleScalarSin ( _rtB -> B_38_626_0 ) ; _rtB -> B_38_630_0 [ 0 ] = _rtP ->
P_260 * _rtB -> B_38_628_0 ; _rtB -> B_38_630_0 [ 1 ] = _rtP -> P_260 *
muDoubleScalarCos ( _rtB -> B_38_626_0 ) ; _rtB -> B_38_632_0 = _rtP -> P_262
* _rtB -> B_38_630_0 [ 0 ] ; _rtB -> B_38_633_0 = _rtB -> B_38_631_0 + _rtB
-> B_38_632_0 ; _rtB -> B_38_635_0 = _rtB -> B_38_633_0 - _rtB -> B_38_634_0
; if ( _rtDW -> Integrator_IWORK_fi != 0 ) { _rtX -> Integrator_CSTATE_be =
_rtB -> B_38_635_0 ; } _rtB -> B_38_636_0 = _rtX -> Integrator_CSTATE_be ;
_rtB -> B_38_637_0 = _rtB -> B_38_633_0 - _rtB -> B_38_636_0 ; _rtB ->
B_38_638_0 = _rtP -> P_264 * _rtB -> B_38_637_0 ; _rtB -> B_38_640_0 = _rtB
-> B_38_638_0 - _rtB -> B_38_639_0 ; _rtB -> B_38_642_0 = _rtB -> B_38_640_0
/ _rtB -> B_38_641_0 ; rtb_B_38_643_0 = _rtB -> B_38_642_0 * _rtB ->
B_38_642_0 * ( real_T ) ( _rtB -> B_38_638_0 > 1.0 ) ; _rtB -> B_38_645_0 =
_rtP -> P_267 * rtb_B_38_643_0 ; _rtB -> B_38_646_0 = _rtB -> B_38_645_0 ; if
( ssIsMajorTimeStep ( S ) != 0 ) { _rtDW -> Switch_Mode_k2 = ( _rtB ->
B_38_646_0 > _rtP -> P_269 ) ; } if ( _rtDW -> Switch_Mode_k2 ) { _rtB ->
B_38_648_0 = _rtB -> B_38_646_0 ; } else { _rtB -> B_38_648_0 = _rtB ->
B_38_647_0 ; } _rtB -> B_38_650_0 = _rtB -> B_38_630_0 [ 1 ] * _rtB ->
B_38_648_0 ; _rtB -> B_38_652_0 [ 2 ] = _rtB -> B_38_650_0 ; _rtB ->
B_38_653_0 [ 0 ] = _rtP -> P_270 * _rtB -> B_38_652_0 [ 0 ] ; _rtB ->
B_38_653_0 [ 1 ] = _rtP -> P_270 * _rtB -> B_38_652_0 [ 1 ] ; _rtB ->
B_38_653_0 [ 2 ] = _rtP -> P_270 * _rtB -> B_38_652_0 [ 2 ] ; _rtB ->
B_38_654_0 = ( _rtB -> B_38_653_0 [ 0 ] + _rtB -> B_38_653_0 [ 1 ] ) + _rtB
-> B_38_653_0 [ 2 ] ; _rtB -> B_38_655_0 = _rtB -> B_38_587_0 + _rtB ->
B_38_654_0 ; ssCallAccelRunBlock ( S , 38 , 658 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 38 , 659 , SS_CALL_MDL_OUTPUTS ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_38_686_0 [ 0 ] =
_rtB -> B_38_50_0 ; _rtB -> B_38_686_0 [ 1 ] = _rtB -> B_38_17_0 ; } { real_T
* * uBuffer = ( real_T * * ) & _rtDW -> BodyPitchloopdelay_PWORK .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
BodyPitchloopdelay_PWORK . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_273 ; _rtB -> B_38_687_0 =
nmm2DFullBody_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , *
uBuffer , _rtDW -> BodyPitchloopdelay_IWORK . CircularBufSize , & _rtDW ->
BodyPitchloopdelay_IWORK . Last , _rtDW -> BodyPitchloopdelay_IWORK . Tail ,
_rtDW -> BodyPitchloopdelay_IWORK . Head , _rtP -> P_274 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
BodyPitchloopdelay_PWORK_j . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> BodyPitchloopdelay_PWORK_j . TUbufferPtrs [ 1 ] ;
real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_275
; _rtB -> B_38_688_0 = nmm2DFullBody_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> BodyPitchloopdelay_IWORK_d .
CircularBufSize , & _rtDW -> BodyPitchloopdelay_IWORK_d . Last , _rtDW ->
BodyPitchloopdelay_IWORK_d . Tail , _rtDW -> BodyPitchloopdelay_IWORK_d .
Head , _rtP -> P_276 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } { real_T * * uBuffer =
( real_T * * ) & _rtDW -> LThighafferentdelay_PWORK . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> LThighafferentdelay_PWORK .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_277 ; _rtB -> B_38_689_0 =
nmm2DFullBody_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , *
uBuffer , _rtDW -> LThighafferentdelay_IWORK . CircularBufSize , & _rtDW ->
LThighafferentdelay_IWORK . Last , _rtDW -> LThighafferentdelay_IWORK . Tail
, _rtDW -> LThighafferentdelay_IWORK . Head , _rtP -> P_278 , 0 , ( boolean_T
) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S )
) ) ) ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
LStanceafferentdelay_PWORK . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> LStanceafferentdelay_PWORK . TUbufferPtrs [ 1 ] ;
real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_279
; _rtB -> B_38_690_0 = nmm2DFullBody_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> LStanceafferentdelay_IWORK .
CircularBufSize , & _rtDW -> LStanceafferentdelay_IWORK . Last , _rtDW ->
LStanceafferentdelay_IWORK . Tail , _rtDW -> LStanceafferentdelay_IWORK .
Head , _rtP -> P_280 , 1 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } { real_T * * uBuffer =
( real_T * * ) & _rtDW -> RStanceafferentdelay_PWORK . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> RStanceafferentdelay_PWORK .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_281 ; _rtB -> B_38_691_0 =
nmm2DFullBody_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , *
uBuffer , _rtDW -> RStanceafferentdelay_IWORK . CircularBufSize , & _rtDW ->
RStanceafferentdelay_IWORK . Last , _rtDW -> RStanceafferentdelay_IWORK .
Tail , _rtDW -> RStanceafferentdelay_IWORK . Head , _rtP -> P_282 , 1 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } _rtB -> B_38_692_0 = ( ( _rtB -> B_38_690_0 != 0.0 )
&& ( _rtB -> B_38_691_0 != 0.0 ) ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if
( isHit != 0 ) { rtb_B_38_693_0 = _rtDW -> Memory_PreviousInput ; zcEvent =
rt_ZCFcn ( RISING_ZERO_CROSSING , & _rtZCE -> RLegstartsDSup_Input_ZCE ,
_rtDW -> Memory_PreviousInput - _rtP -> P_284 ) ; if ( _rtDW ->
RLegstartsDSup_MODE == 0 ) { if ( zcEvent != NO_ZCEVENT ) { _rtB ->
B_38_694_0 = ! ( _rtB -> B_38_694_0 != 0.0 ) ; _rtDW -> RLegstartsDSup_MODE =
1 ; } else { if ( ( _rtB -> B_38_694_0 == 1.0 ) && ( rtb_B_38_693_0 != _rtP
-> P_284 ) ) { _rtB -> B_38_694_0 = 0.0 ; } } } else { if ( rtb_B_38_693_0 !=
_rtP -> P_284 ) { _rtB -> B_38_694_0 = 0.0 ; } _rtDW -> RLegstartsDSup_MODE =
0 ; } } rtb_B_38_693_0 = ( ( _rtB -> B_38_692_0 != 0.0 ) && ( _rtB ->
B_38_694_0 != 0.0 ) ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { zcEvent = rt_ZCFcn ( FALLING_ZERO_CROSSING , & _rtZCE ->
DSupends_Input_ZCE , _rtB -> B_38_692_0 - _rtP -> P_285 ) ; if ( _rtDW ->
DSupends_MODE == 0 ) { if ( zcEvent != NO_ZCEVENT ) { _rtB -> B_38_696_0 = !
( _rtB -> B_38_696_0 != 0.0 ) ; _rtDW -> DSupends_MODE = 1 ; } else { if ( (
_rtB -> B_38_696_0 == 1.0 ) && ( _rtB -> B_38_692_0 != _rtP -> P_285 ) ) {
_rtB -> B_38_696_0 = 0.0 ; } } } else { if ( _rtB -> B_38_692_0 != _rtP ->
P_285 ) { _rtB -> B_38_696_0 = 0.0 ; } _rtDW -> DSupends_MODE = 0 ; } _rtB ->
B_38_697_0 = _rtDW -> Memory_PreviousInput_i ; } isHit = ( int32_T ) ( ( ( (
( uint32_T ) ( ( int32_T ) rtb_B_38_693_0 != 0 ) << 1 ) + ( _rtB ->
B_38_696_0 != 0.0 ) ) << 1 ) + ( _rtB -> B_38_697_0 != 0.0 ) ) ; _rtB ->
B_38_698_0 [ 0U ] = _rtP -> P_287 [ ( uint32_T ) isHit ] ; _rtB -> B_38_698_0
[ 1U ] = _rtP -> P_287 [ isHit + 8U ] ; nmm2DFullBody_StancePhase_i ( S ,
_rtB -> B_38_690_0 , _rtB -> B_38_687_0 , _rtB -> B_38_688_0 , _rtB ->
B_38_689_0 , _rtB -> B_38_698_0 [ 0 ] , & _rtB -> StancePhase_i , & _rtDW ->
StancePhase_i , & _rtP -> StancePhase_i ) ; { real_T * * uBuffer = ( real_T *
* ) & _rtDW -> GLUFmloopdelay_PWORK . TUbufferPtrs [ 0 ] ; real_T * * tBuffer
= ( real_T * * ) & _rtDW -> GLUFmloopdelay_PWORK . TUbufferPtrs [ 1 ] ;
real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_288
; _rtB -> B_38_700_0 = nmm2DFullBody_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> GLUFmloopdelay_IWORK . CircularBufSize
, & _rtDW -> GLUFmloopdelay_IWORK . Last , _rtDW -> GLUFmloopdelay_IWORK .
Tail , _rtDW -> GLUFmloopdelay_IWORK . Head , _rtP -> P_289 , 0 , ( boolean_T
) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S )
) ) ) ; } B_38_701_0 = ! ( _rtB -> B_38_690_0 != 0.0 ) ;
nmm2DFullBody_SwingPhase ( S , B_38_701_0 , _rtB -> B_38_700_0 , & _rtB ->
SwingPhase_k , & _rtDW -> SwingPhase_k , & _rtP -> SwingPhase_k ) ; _rtB ->
B_38_703_0 = _rtB -> StancePhase_i . B_17_14_0 + _rtB -> SwingPhase_k .
B_16_2_0 ; if ( _rtB -> B_38_703_0 > _rtP -> P_290 ) { _rtB -> B_38_705_0 =
_rtP -> P_290 ; } else if ( _rtB -> B_38_703_0 < _rtP -> P_291 ) { _rtB ->
B_38_705_0 = _rtP -> P_291 ; } else { _rtB -> B_38_705_0 = _rtB -> B_38_703_0
; } nmm2DFullBody_StancePhase_o ( S , _rtB -> B_38_690_0 , _rtB -> B_38_687_0
, _rtB -> B_38_688_0 , _rtB -> B_38_689_0 , _rtB -> B_38_698_0 [ 0 ] , & _rtB
-> StancePhase_o , & _rtDW -> StancePhase_o , & _rtP -> StancePhase_o ) ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> HFLLceloopdelay_PWORK .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
HFLLceloopdelay_PWORK . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ;
real_T tMinusDelay = simTime - _rtP -> P_292 ; _rtB -> B_38_707_0 =
nmm2DFullBody_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , *
uBuffer , _rtDW -> HFLLceloopdelay_IWORK . CircularBufSize , & _rtDW ->
HFLLceloopdelay_IWORK . Last , _rtDW -> HFLLceloopdelay_IWORK . Tail , _rtDW
-> HFLLceloopdelay_IWORK . Head , _rtP -> P_293 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW -> HAMLceloopdelay_PWORK
. TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
HAMLceloopdelay_PWORK . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ;
real_T tMinusDelay = simTime - _rtP -> P_294 ; _rtB -> B_38_708_0 =
nmm2DFullBody_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , *
uBuffer , _rtDW -> HAMLceloopdelay_IWORK . CircularBufSize , & _rtDW ->
HAMLceloopdelay_IWORK . Last , _rtDW -> HAMLceloopdelay_IWORK . Tail , _rtDW
-> HAMLceloopdelay_IWORK . Head , _rtP -> P_295 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW -> HAMFmloopdelay_PWORK .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
HAMFmloopdelay_PWORK . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ;
real_T tMinusDelay = simTime - _rtP -> P_296 ; _rtB -> B_38_709_0 =
nmm2DFullBody_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , *
uBuffer , _rtDW -> HAMFmloopdelay_IWORK . CircularBufSize , & _rtDW ->
HAMFmloopdelay_IWORK . Last , _rtDW -> HAMFmloopdelay_IWORK . Tail , _rtDW ->
HAMFmloopdelay_IWORK . Head , _rtP -> P_297 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } B_38_710_0 = ! ( _rtB -> B_38_690_0 != 0.0 ) ;
nmm2DFullBody_SwingPhase_b ( S , B_38_710_0 , _rtB -> StancePhase_o .
B_18_2_0 , _rtB -> B_38_707_0 , _rtB -> B_38_708_0 , _rtB -> B_38_709_0 , &
_rtB -> SwingPhase_b , & _rtDW -> SwingPhase_b , & _rtP -> SwingPhase_b ) ;
_rtB -> B_38_712_0 [ 0 ] = _rtB -> StancePhase_o . B_18_18_0 + _rtB ->
SwingPhase_b . B_19_6_0 ; _rtB -> B_38_712_0 [ 1 ] = _rtB -> StancePhase_o .
B_18_15_0 + _rtB -> SwingPhase_b . B_19_9_0 ; if ( _rtB -> B_38_712_0 [ 0 ] >
_rtP -> P_298 ) { _rtB -> B_38_713_0 [ 0 ] = _rtP -> P_298 ; } else if ( _rtB
-> B_38_712_0 [ 0 ] < _rtP -> P_299 ) { _rtB -> B_38_713_0 [ 0 ] = _rtP ->
P_299 ; } else { _rtB -> B_38_713_0 [ 0 ] = _rtB -> B_38_712_0 [ 0 ] ; } if (
_rtB -> B_38_712_0 [ 1 ] > _rtP -> P_298 ) { _rtB -> B_38_713_0 [ 1 ] = _rtP
-> P_298 ; } else if ( _rtB -> B_38_712_0 [ 1 ] < _rtP -> P_299 ) { _rtB ->
B_38_713_0 [ 1 ] = _rtP -> P_299 ; } else { _rtB -> B_38_713_0 [ 1 ] = _rtB
-> B_38_712_0 [ 1 ] ; } _rtB -> B_38_715_0 = _rtB -> B_38_713_0 [ 0 ] ; _rtB
-> B_38_717_0 = _rtB -> B_38_713_0 [ 1 ] ; { real_T * * uBuffer = ( real_T *
* ) & _rtDW -> LKneeloopdelay_PWORK . TUbufferPtrs [ 0 ] ; real_T * * tBuffer
= ( real_T * * ) & _rtDW -> LKneeloopdelay_PWORK . TUbufferPtrs [ 1 ] ;
real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_300
; _rtB -> B_38_718_0 = nmm2DFullBody_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> LKneeloopdelay_IWORK . CircularBufSize
, & _rtDW -> LKneeloopdelay_IWORK . Last , _rtDW -> LKneeloopdelay_IWORK .
Tail , _rtDW -> LKneeloopdelay_IWORK . Head , _rtP -> P_301 , 0 , ( boolean_T
) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S )
) ) ) ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
RThighafferentdelay_PWORK . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> RThighafferentdelay_PWORK . TUbufferPtrs [ 1 ] ;
real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_302
; _rtB -> B_38_719_0 = nmm2DFullBody_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> RThighafferentdelay_IWORK .
CircularBufSize , & _rtDW -> RThighafferentdelay_IWORK . Last , _rtDW ->
RThighafferentdelay_IWORK . Tail , _rtDW -> RThighafferentdelay_IWORK . Head
, _rtP -> P_303 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } { real_T * * uBuffer =
( real_T * * ) & _rtDW -> VASloopdelay_PWORK . TUbufferPtrs [ 0 ] ; real_T *
* tBuffer = ( real_T * * ) & _rtDW -> VASloopdelay_PWORK . TUbufferPtrs [ 1 ]
; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP ->
P_304 ; _rtB -> B_38_720_0 = nmm2DFullBody_acc_rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , _rtDW -> VASloopdelay_IWORK .
CircularBufSize , & _rtDW -> VASloopdelay_IWORK . Last , _rtDW ->
VASloopdelay_IWORK . Tail , _rtDW -> VASloopdelay_IWORK . Head , _rtP ->
P_305 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; }
nmm2DFullBody_StancePhase_g ( S , _rtB -> B_38_690_0 , _rtB -> B_38_718_0 ,
_rtB -> B_38_698_0 [ 0 ] , _rtB -> B_38_719_0 , _rtB -> B_38_720_0 , & _rtB
-> StancePhase_g , & _rtDW -> StancePhase_g , & _rtP -> StancePhase_g ) ;
_rtB -> B_38_722_0 = ! ( _rtB -> B_38_690_0 != 0.0 ) ; isHit = ssIsSampleHit
( S , 1 , 0 ) ; if ( isHit != 0 ) { if ( ssIsMajorTimeStep ( S ) != 0 ) { if
( _rtB -> B_38_722_0 ) { if ( ! _rtDW -> SwingPhase_MODE_p ) { if (
ssGetTaskTime ( S , 1 ) != ssGetTStart ( S ) ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } _rtDW ->
SwingPhase_MODE_p = true ; } } else { if ( _rtDW -> SwingPhase_MODE_p ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; _rtB -> B_22_0_0 = _rtP ->
P_2 ; _rtDW -> SwingPhase_MODE_p = false ; } } } if ( _rtDW ->
SwingPhase_MODE_p ) { _rtB -> B_22_0_0 = _rtP -> P_3 ; if ( ssIsMajorTimeStep
( S ) != 0 ) { srUpdateBC ( _rtDW -> SwingPhase_SubsysRanBC_l ) ; } } } _rtB
-> B_38_724_0 = _rtB -> StancePhase_g . B_23_8_0 + _rtB -> B_22_0_0 ; if (
_rtB -> B_38_724_0 > _rtP -> P_306 ) { _rtB -> B_38_726_0 = _rtP -> P_306 ; }
else if ( _rtB -> B_38_724_0 < _rtP -> P_307 ) { _rtB -> B_38_726_0 = _rtP ->
P_307 ; } else { _rtB -> B_38_726_0 = _rtB -> B_38_724_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> GASloopdelay_PWORK . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> GASloopdelay_PWORK .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_308 ; _rtB -> B_38_727_0 =
nmm2DFullBody_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , *
uBuffer , _rtDW -> GASloopdelay_IWORK . CircularBufSize , & _rtDW ->
GASloopdelay_IWORK . Last , _rtDW -> GASloopdelay_IWORK . Tail , _rtDW ->
GASloopdelay_IWORK . Head , _rtP -> P_309 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } nmm2DFullBody_StancePhase ( S , _rtB -> B_38_690_0 , _rtB -> B_38_727_0
, & _rtB -> StancePhase , & _rtDW -> StancePhase , & _rtP -> StancePhase ) ;
_rtB -> B_38_729_0 = ! ( _rtB -> B_38_690_0 != 0.0 ) ; isHit = ssIsSampleHit
( S , 1 , 0 ) ; if ( isHit != 0 ) { if ( ssIsMajorTimeStep ( S ) != 0 ) { if
( _rtB -> B_38_729_0 ) { if ( ! _rtDW -> SwingPhase_MODE_c ) { if (
ssGetTaskTime ( S , 1 ) != ssGetTStart ( S ) ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } _rtDW ->
SwingPhase_MODE_c = true ; } } else { if ( _rtDW -> SwingPhase_MODE_c ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; _rtB -> B_14_0_0 = _rtP ->
P_0 ; _rtDW -> SwingPhase_MODE_c = false ; } } } if ( _rtDW ->
SwingPhase_MODE_c ) { _rtB -> B_14_0_0 = _rtP -> P_1 ; if ( ssIsMajorTimeStep
( S ) != 0 ) { srUpdateBC ( _rtDW -> SwingPhase_SubsysRanBC_d ) ; } } } _rtB
-> B_38_731_0 = _rtB -> StancePhase . B_15_2_0 + _rtB -> B_14_0_0 ; if ( _rtB
-> B_38_731_0 > _rtP -> P_310 ) { _rtB -> B_38_733_0 = _rtP -> P_310 ; } else
if ( _rtB -> B_38_731_0 < _rtP -> P_311 ) { _rtB -> B_38_733_0 = _rtP ->
P_311 ; } else { _rtB -> B_38_733_0 = _rtB -> B_38_731_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> SOLloopdelay_PWORK . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> SOLloopdelay_PWORK .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_312 ; _rtB -> B_38_734_0 =
nmm2DFullBody_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , *
uBuffer , _rtDW -> SOLloopdelay_IWORK . CircularBufSize , & _rtDW ->
SOLloopdelay_IWORK . Last , _rtDW -> SOLloopdelay_IWORK . Tail , _rtDW ->
SOLloopdelay_IWORK . Head , _rtP -> P_313 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW -> TAloopdelay_PWORK .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TAloopdelay_PWORK . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ;
real_T tMinusDelay = simTime - _rtP -> P_314 ; _rtB -> B_38_735_0 =
nmm2DFullBody_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , *
uBuffer , _rtDW -> TAloopdelay_IWORK . CircularBufSize , & _rtDW ->
TAloopdelay_IWORK . Last , _rtDW -> TAloopdelay_IWORK . Tail , _rtDW ->
TAloopdelay_IWORK . Head , _rtP -> P_315 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } nmm2DFullBody_StancePhase_l ( S , _rtB -> B_38_690_0 , _rtB ->
B_38_734_0 , _rtB -> B_38_735_0 , & _rtB -> StancePhase_l , & _rtDW ->
StancePhase_l , & _rtP -> StancePhase_l ) ; B_38_737_0 = ! ( _rtB ->
B_38_690_0 != 0.0 ) ; nmm2DFullBody_SwingPhase_f ( S , B_38_737_0 , _rtB ->
B_38_735_0 , & _rtB -> SwingPhase_f , & _rtDW -> SwingPhase_f , & _rtP ->
SwingPhase_f ) ; _rtB -> B_38_739_0 [ 0 ] = _rtB -> StancePhase_l . B_20_7_0
+ _rtB -> SwingPhase_f . B_21_2_0 ; _rtB -> B_38_739_0 [ 1 ] = _rtB ->
StancePhase_l . B_20_6_0 + _rtB -> SwingPhase_f . B_21_3_0 ; if ( _rtB ->
B_38_739_0 [ 0 ] > _rtP -> P_316 ) { _rtB -> B_38_740_0 [ 0 ] = _rtP -> P_316
; } else if ( _rtB -> B_38_739_0 [ 0 ] < _rtP -> P_317 ) { _rtB -> B_38_740_0
[ 0 ] = _rtP -> P_317 ; } else { _rtB -> B_38_740_0 [ 0 ] = _rtB ->
B_38_739_0 [ 0 ] ; } if ( _rtB -> B_38_739_0 [ 1 ] > _rtP -> P_316 ) { _rtB
-> B_38_740_0 [ 1 ] = _rtP -> P_316 ; } else if ( _rtB -> B_38_739_0 [ 1 ] <
_rtP -> P_317 ) { _rtB -> B_38_740_0 [ 1 ] = _rtP -> P_317 ; } else { _rtB ->
B_38_740_0 [ 1 ] = _rtB -> B_38_739_0 [ 1 ] ; } _rtB -> B_38_743_0 = _rtB ->
B_38_740_0 [ 1 ] ; _rtB -> B_38_746_0 = _rtB -> B_38_740_0 [ 0 ] ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { rtb_B_38_693_0 = _rtDW ->
Memory3_PreviousInput ; zcEvent = rt_ZCFcn ( RISING_ZERO_CROSSING , & _rtZCE
-> LLegstartsDSup_Input_ZCE , _rtDW -> Memory3_PreviousInput - _rtP -> P_319
) ; if ( _rtDW -> LLegstartsDSup_MODE == 0 ) { if ( zcEvent != NO_ZCEVENT ) {
_rtB -> B_38_748_0 = ! ( _rtB -> B_38_748_0 != 0.0 ) ; _rtDW ->
LLegstartsDSup_MODE = 1 ; } else { if ( ( _rtB -> B_38_748_0 == 1.0 ) && (
rtb_B_38_693_0 != _rtP -> P_319 ) ) { _rtB -> B_38_748_0 = 0.0 ; } } } else {
if ( rtb_B_38_693_0 != _rtP -> P_319 ) { _rtB -> B_38_748_0 = 0.0 ; } _rtDW
-> LLegstartsDSup_MODE = 0 ; } } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_38_750_0 = _rtDW -> Memory_PreviousInput_a ; } isHit
= ( int32_T ) ( ( ( ( ( uint32_T ) ( ( _rtB -> B_38_748_0 != 0.0 ) && ( _rtB
-> B_38_692_0 != 0.0 ) ) << 1 ) + ( _rtB -> B_38_696_0 != 0.0 ) ) << 1 ) + (
_rtB -> B_38_750_0 != 0.0 ) ) ; _rtB -> B_38_751_0 [ 0U ] = _rtP -> P_321 [ (
uint32_T ) isHit ] ; _rtB -> B_38_751_0 [ 1U ] = _rtP -> P_321 [ isHit + 8U ]
; nmm2DFullBody_StancePhase_i ( S , _rtB -> B_38_691_0 , _rtB -> B_38_687_0 ,
_rtB -> B_38_688_0 , _rtB -> B_38_719_0 , _rtB -> B_38_751_0 [ 0 ] , & _rtB
-> StancePhase_k , & _rtDW -> StancePhase_k , & _rtP -> StancePhase_k ) ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> GLUFmloopdelay_PWORK_g .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
GLUFmloopdelay_PWORK_g . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ;
real_T tMinusDelay = simTime - _rtP -> P_322 ; _rtB -> B_38_753_0 =
nmm2DFullBody_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , *
uBuffer , _rtDW -> GLUFmloopdelay_IWORK_m . CircularBufSize , & _rtDW ->
GLUFmloopdelay_IWORK_m . Last , _rtDW -> GLUFmloopdelay_IWORK_m . Tail ,
_rtDW -> GLUFmloopdelay_IWORK_m . Head , _rtP -> P_323 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } B_38_754_0 = ! ( _rtB -> B_38_691_0 != 0.0 ) ; nmm2DFullBody_SwingPhase
( S , B_38_754_0 , _rtB -> B_38_753_0 , & _rtB -> SwingPhase_j , & _rtDW ->
SwingPhase_j , & _rtP -> SwingPhase_j ) ; _rtB -> B_38_756_0 = _rtB ->
StancePhase_k . B_17_14_0 + _rtB -> SwingPhase_j . B_16_2_0 ; if ( _rtB ->
B_38_756_0 > _rtP -> P_324 ) { _rtB -> B_38_758_0 = _rtP -> P_324 ; } else if
( _rtB -> B_38_756_0 < _rtP -> P_325 ) { _rtB -> B_38_758_0 = _rtP -> P_325 ;
} else { _rtB -> B_38_758_0 = _rtB -> B_38_756_0 ; }
nmm2DFullBody_StancePhase_o ( S , _rtB -> B_38_691_0 , _rtB -> B_38_687_0 ,
_rtB -> B_38_688_0 , _rtB -> B_38_719_0 , _rtB -> B_38_751_0 [ 0 ] , & _rtB
-> StancePhase_j , & _rtDW -> StancePhase_j , & _rtP -> StancePhase_j ) ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> HFLLceloopdelay_PWORK_g .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
HFLLceloopdelay_PWORK_g . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_326 ; _rtB -> B_38_760_0 =
nmm2DFullBody_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , *
uBuffer , _rtDW -> HFLLceloopdelay_IWORK_p . CircularBufSize , & _rtDW ->
HFLLceloopdelay_IWORK_p . Last , _rtDW -> HFLLceloopdelay_IWORK_p . Tail ,
_rtDW -> HFLLceloopdelay_IWORK_p . Head , _rtP -> P_327 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
HAMLceloopdelay_PWORK_d . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> HAMLceloopdelay_PWORK_d . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_328 ; _rtB
-> B_38_761_0 = nmm2DFullBody_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 ,
* tBuffer , * uBuffer , _rtDW -> HAMLceloopdelay_IWORK_e . CircularBufSize ,
& _rtDW -> HAMLceloopdelay_IWORK_e . Last , _rtDW -> HAMLceloopdelay_IWORK_e
. Tail , _rtDW -> HAMLceloopdelay_IWORK_e . Head , _rtP -> P_329 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
HAMFmloopdelay_PWORK_h . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T *
* ) & _rtDW -> HAMFmloopdelay_PWORK_h . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_330 ; _rtB ->
B_38_762_0 = nmm2DFullBody_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> HAMFmloopdelay_IWORK_l . CircularBufSize , &
_rtDW -> HAMFmloopdelay_IWORK_l . Last , _rtDW -> HAMFmloopdelay_IWORK_l .
Tail , _rtDW -> HAMFmloopdelay_IWORK_l . Head , _rtP -> P_331 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } B_38_763_0 = ! ( _rtB -> B_38_691_0 != 0.0 ) ;
nmm2DFullBody_SwingPhase_b ( S , B_38_763_0 , _rtB -> StancePhase_j .
B_18_2_0 , _rtB -> B_38_760_0 , _rtB -> B_38_761_0 , _rtB -> B_38_762_0 , &
_rtB -> SwingPhase_m , & _rtDW -> SwingPhase_m , & _rtP -> SwingPhase_m ) ;
_rtB -> B_38_765_0 [ 0 ] = _rtB -> StancePhase_j . B_18_18_0 + _rtB ->
SwingPhase_m . B_19_6_0 ; _rtB -> B_38_765_0 [ 1 ] = _rtB -> StancePhase_j .
B_18_15_0 + _rtB -> SwingPhase_m . B_19_9_0 ; if ( _rtB -> B_38_765_0 [ 0 ] >
_rtP -> P_332 ) { _rtB -> B_38_766_0 [ 0 ] = _rtP -> P_332 ; } else if ( _rtB
-> B_38_765_0 [ 0 ] < _rtP -> P_333 ) { _rtB -> B_38_766_0 [ 0 ] = _rtP ->
P_333 ; } else { _rtB -> B_38_766_0 [ 0 ] = _rtB -> B_38_765_0 [ 0 ] ; } if (
_rtB -> B_38_765_0 [ 1 ] > _rtP -> P_332 ) { _rtB -> B_38_766_0 [ 1 ] = _rtP
-> P_332 ; } else if ( _rtB -> B_38_765_0 [ 1 ] < _rtP -> P_333 ) { _rtB ->
B_38_766_0 [ 1 ] = _rtP -> P_333 ; } else { _rtB -> B_38_766_0 [ 1 ] = _rtB
-> B_38_765_0 [ 1 ] ; } _rtB -> B_38_768_0 = _rtB -> B_38_766_0 [ 0 ] ; _rtB
-> B_38_770_0 = _rtB -> B_38_766_0 [ 1 ] ; { real_T * * uBuffer = ( real_T *
* ) & _rtDW -> RKneeloopdelay_PWORK . TUbufferPtrs [ 0 ] ; real_T * * tBuffer
= ( real_T * * ) & _rtDW -> RKneeloopdelay_PWORK . TUbufferPtrs [ 1 ] ;
real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_334
; _rtB -> B_38_771_0 = nmm2DFullBody_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> RKneeloopdelay_IWORK . CircularBufSize
, & _rtDW -> RKneeloopdelay_IWORK . Last , _rtDW -> RKneeloopdelay_IWORK .
Tail , _rtDW -> RKneeloopdelay_IWORK . Head , _rtP -> P_335 , 0 , ( boolean_T
) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S )
) ) ) ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
VASloopdelay_PWORK_o . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * *
) & _rtDW -> VASloopdelay_PWORK_o . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_336 ; _rtB ->
B_38_772_0 = nmm2DFullBody_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> VASloopdelay_IWORK_p . CircularBufSize , &
_rtDW -> VASloopdelay_IWORK_p . Last , _rtDW -> VASloopdelay_IWORK_p . Tail ,
_rtDW -> VASloopdelay_IWORK_p . Head , _rtP -> P_337 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } nmm2DFullBody_StancePhase_g ( S , _rtB -> B_38_691_0 , _rtB ->
B_38_771_0 , _rtB -> B_38_751_0 [ 0 ] , _rtB -> B_38_689_0 , _rtB ->
B_38_772_0 , & _rtB -> StancePhase_b , & _rtDW -> StancePhase_b , & _rtP ->
StancePhase_b ) ; _rtB -> B_38_774_0 = ! ( _rtB -> B_38_691_0 != 0.0 ) ;
isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { if (
ssIsMajorTimeStep ( S ) != 0 ) { if ( _rtB -> B_38_774_0 ) { if ( ! _rtDW ->
SwingPhase_MODE ) { if ( ssGetTaskTime ( S , 1 ) != ssGetTStart ( S ) ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } _rtDW -> SwingPhase_MODE
= true ; } } else { if ( _rtDW -> SwingPhase_MODE ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; _rtB -> B_32_0_0 = _rtP ->
P_6 ; _rtDW -> SwingPhase_MODE = false ; } } } if ( _rtDW -> SwingPhase_MODE
) { _rtB -> B_32_0_0 = _rtP -> P_7 ; if ( ssIsMajorTimeStep ( S ) != 0 ) {
srUpdateBC ( _rtDW -> SwingPhase_SubsysRanBC ) ; } } } _rtB -> B_38_776_0 =
_rtB -> StancePhase_b . B_23_8_0 + _rtB -> B_32_0_0 ; if ( _rtB -> B_38_776_0
> _rtP -> P_338 ) { _rtB -> B_38_778_0 = _rtP -> P_338 ; } else if ( _rtB ->
B_38_776_0 < _rtP -> P_339 ) { _rtB -> B_38_778_0 = _rtP -> P_339 ; } else {
_rtB -> B_38_778_0 = _rtB -> B_38_776_0 ; } { real_T * * uBuffer = ( real_T *
* ) & _rtDW -> GASloopdelay_PWORK_f . TUbufferPtrs [ 0 ] ; real_T * * tBuffer
= ( real_T * * ) & _rtDW -> GASloopdelay_PWORK_f . TUbufferPtrs [ 1 ] ;
real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_340
; _rtB -> B_38_779_0 = nmm2DFullBody_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> GASloopdelay_IWORK_j . CircularBufSize
, & _rtDW -> GASloopdelay_IWORK_j . Last , _rtDW -> GASloopdelay_IWORK_j .
Tail , _rtDW -> GASloopdelay_IWORK_j . Head , _rtP -> P_341 , 0 , ( boolean_T
) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S )
) ) ) ; } nmm2DFullBody_StancePhase ( S , _rtB -> B_38_691_0 , _rtB ->
B_38_779_0 , & _rtB -> StancePhase_a , & _rtDW -> StancePhase_a , & _rtP ->
StancePhase_a ) ; _rtB -> B_38_781_0 = ! ( _rtB -> B_38_691_0 != 0.0 ) ;
isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { if (
ssIsMajorTimeStep ( S ) != 0 ) { if ( _rtB -> B_38_781_0 ) { if ( ! _rtDW ->
SwingPhase_MODE_k ) { if ( ssGetTaskTime ( S , 1 ) != ssGetTStart ( S ) ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } _rtDW ->
SwingPhase_MODE_k = true ; } } else { if ( _rtDW -> SwingPhase_MODE_k ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; _rtB -> B_24_0_0 = _rtP ->
P_4 ; _rtDW -> SwingPhase_MODE_k = false ; } } } if ( _rtDW ->
SwingPhase_MODE_k ) { _rtB -> B_24_0_0 = _rtP -> P_5 ; if ( ssIsMajorTimeStep
( S ) != 0 ) { srUpdateBC ( _rtDW -> SwingPhase_SubsysRanBC_j ) ; } } } _rtB
-> B_38_783_0 = _rtB -> StancePhase_a . B_15_2_0 + _rtB -> B_24_0_0 ; if (
_rtB -> B_38_783_0 > _rtP -> P_342 ) { _rtB -> B_38_785_0 = _rtP -> P_342 ; }
else if ( _rtB -> B_38_783_0 < _rtP -> P_343 ) { _rtB -> B_38_785_0 = _rtP ->
P_343 ; } else { _rtB -> B_38_785_0 = _rtB -> B_38_783_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> SOLloopdelay_PWORK_c . TUbufferPtrs [ 0 ]
; real_T * * tBuffer = ( real_T * * ) & _rtDW -> SOLloopdelay_PWORK_c .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_344 ; _rtB -> B_38_786_0 =
nmm2DFullBody_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , *
uBuffer , _rtDW -> SOLloopdelay_IWORK_a . CircularBufSize , & _rtDW ->
SOLloopdelay_IWORK_a . Last , _rtDW -> SOLloopdelay_IWORK_a . Tail , _rtDW ->
SOLloopdelay_IWORK_a . Head , _rtP -> P_345 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW -> TAloopdelay_PWORK_d .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TAloopdelay_PWORK_d . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ;
real_T tMinusDelay = simTime - _rtP -> P_346 ; _rtB -> B_38_787_0 =
nmm2DFullBody_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , *
uBuffer , _rtDW -> TAloopdelay_IWORK_o . CircularBufSize , & _rtDW ->
TAloopdelay_IWORK_o . Last , _rtDW -> TAloopdelay_IWORK_o . Tail , _rtDW ->
TAloopdelay_IWORK_o . Head , _rtP -> P_347 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } nmm2DFullBody_StancePhase_l ( S , _rtB -> B_38_691_0 , _rtB ->
B_38_786_0 , _rtB -> B_38_787_0 , & _rtB -> StancePhase_n , & _rtDW ->
StancePhase_n , & _rtP -> StancePhase_n ) ; B_38_789_0 = ! ( _rtB ->
B_38_691_0 != 0.0 ) ; nmm2DFullBody_SwingPhase_f ( S , B_38_789_0 , _rtB ->
B_38_787_0 , & _rtB -> SwingPhase_i , & _rtDW -> SwingPhase_i , & _rtP ->
SwingPhase_i ) ; _rtB -> B_38_791_0 [ 0 ] = _rtB -> StancePhase_n . B_20_7_0
+ _rtB -> SwingPhase_i . B_21_2_0 ; _rtB -> B_38_791_0 [ 1 ] = _rtB ->
StancePhase_n . B_20_6_0 + _rtB -> SwingPhase_i . B_21_3_0 ; if ( _rtB ->
B_38_791_0 [ 0 ] > _rtP -> P_348 ) { _rtB -> B_38_792_0 [ 0 ] = _rtP -> P_348
; } else if ( _rtB -> B_38_791_0 [ 0 ] < _rtP -> P_349 ) { _rtB -> B_38_792_0
[ 0 ] = _rtP -> P_349 ; } else { _rtB -> B_38_792_0 [ 0 ] = _rtB ->
B_38_791_0 [ 0 ] ; } if ( _rtB -> B_38_791_0 [ 1 ] > _rtP -> P_348 ) { _rtB
-> B_38_792_0 [ 1 ] = _rtP -> P_348 ; } else if ( _rtB -> B_38_791_0 [ 1 ] <
_rtP -> P_349 ) { _rtB -> B_38_792_0 [ 1 ] = _rtP -> P_349 ; } else { _rtB ->
B_38_792_0 [ 1 ] = _rtB -> B_38_791_0 [ 1 ] ; } _rtB -> B_38_795_0 = _rtB ->
B_38_792_0 [ 1 ] ; _rtB -> B_38_798_0 = _rtB -> B_38_792_0 [ 0 ] ; isHit =
ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_38_799_0 [ 0 ] =
_rtB -> B_38_686_0 [ 0 ] ; _rtB -> B_38_799_0 [ 1 ] = _rtB -> B_38_686_0 [ 1
] ; _rtB -> B_38_799_0 [ 2 ] = _rtB -> B_38_104_0 ; _rtB -> B_38_799_0 [ 3 ]
= _rtB -> B_38_115_0 ; _rtB -> B_38_799_0 [ 4 ] = _rtB -> B_38_99_0 ; _rtB ->
B_38_799_0 [ 5 ] = _rtB -> B_38_119_0 ; _rtB -> B_38_799_0 [ 6 ] = _rtB ->
B_38_123_0 ; _rtB -> B_38_799_0 [ 7 ] = _rtB -> B_38_127_0 ; _rtB ->
B_38_799_0 [ 8 ] = _rtB -> B_38_102_0 ; _rtB -> B_38_799_0 [ 9 ] = _rtB ->
B_38_113_0 ; _rtB -> B_38_799_0 [ 10 ] = _rtB -> B_38_97_0 ; _rtB ->
B_38_799_0 [ 11 ] = _rtB -> B_38_117_0 ; _rtB -> B_38_799_0 [ 12 ] = _rtB ->
B_38_121_0 ; _rtB -> B_38_799_0 [ 13 ] = _rtB -> B_38_125_0 ; _rtB ->
B_38_799_0 [ 14 ] = _rtB -> B_38_490_0 ; _rtB -> B_38_799_0 [ 15 ] = _rtB ->
B_38_575_0 ; _rtB -> B_38_799_0 [ 16 ] = _rtB -> B_38_655_0 ; _rtB ->
B_38_799_0 [ 17 ] = _rtB -> B_38_394_0 ; _rtB -> B_38_799_0 [ 18 ] = _rtB ->
B_38_330_0 ; _rtB -> B_38_799_0 [ 19 ] = _rtB -> B_38_249_0 ; _rtB ->
B_38_799_0 [ 20 ] = _rtB -> LeftForcePlate . B_6_2_0 [ 1 ] ; _rtB ->
B_38_799_0 [ 21 ] = _rtB -> RightForcePlate . B_6_2_0 [ 1 ] ; _rtB ->
B_38_799_0 [ 22 ] = _rtB -> LeftForcePlate . B_6_2_0 [ 0 ] ; _rtB ->
B_38_799_0 [ 23 ] = _rtB -> RightForcePlate . B_6_2_0 [ 0 ] ; _rtB ->
B_38_799_0 [ 24 ] = _rtB -> B_38_705_0 ; _rtB -> B_38_799_0 [ 25 ] = _rtB ->
B_38_715_0 ; _rtB -> B_38_799_0 [ 26 ] = _rtB -> B_38_717_0 ; _rtB ->
B_38_799_0 [ 27 ] = _rtB -> B_38_726_0 ; _rtB -> B_38_799_0 [ 28 ] = _rtB ->
B_38_733_0 ; _rtB -> B_38_799_0 [ 29 ] = _rtB -> B_38_743_0 ; _rtB ->
B_38_799_0 [ 30 ] = _rtB -> B_38_746_0 ; _rtB -> B_38_799_0 [ 31 ] = _rtB ->
B_38_758_0 ; _rtB -> B_38_799_0 [ 32 ] = _rtB -> B_38_768_0 ; _rtB ->
B_38_799_0 [ 33 ] = _rtB -> B_38_770_0 ; _rtB -> B_38_799_0 [ 34 ] = _rtB ->
B_38_778_0 ; _rtB -> B_38_799_0 [ 35 ] = _rtB -> B_38_785_0 ; _rtB ->
B_38_799_0 [ 36 ] = _rtB -> B_38_795_0 ; _rtB -> B_38_799_0 [ 37 ] = _rtB ->
B_38_798_0 ; ssCallAccelRunBlock ( S , 38 , 800 , SS_CALL_MDL_OUTPUTS ) ; }
isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_38_801_0
[ 0 ] = _rtB -> B_38_50_0 ; _rtB -> B_38_801_0 [ 1 ] = _rtB -> B_38_17_0 ; }
_rtB -> B_38_802_0 = _rtB -> B_38_575_0 + _rtB -> B_38_499_0 ; _rtB ->
B_38_803_0 = _rtB -> B_38_330_0 + _rtB -> B_38_291_0 ; isHit = ssIsSampleHit
( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_38_804_0 [ 0 ] = _rtB ->
B_38_801_0 [ 0 ] ; _rtB -> B_38_804_0 [ 1 ] = _rtB -> B_38_801_0 [ 1 ] ; _rtB
-> B_38_804_0 [ 2 ] = _rtB -> B_38_115_0 ; _rtB -> B_38_804_0 [ 3 ] = _rtB ->
B_38_99_0 ; _rtB -> B_38_804_0 [ 4 ] = _rtB -> B_38_123_0 ; _rtB ->
B_38_804_0 [ 5 ] = _rtB -> B_38_127_0 ; _rtB -> B_38_804_0 [ 6 ] = _rtB ->
B_38_113_0 ; _rtB -> B_38_804_0 [ 7 ] = _rtB -> B_38_97_0 ; _rtB ->
B_38_804_0 [ 8 ] = _rtB -> B_38_121_0 ; _rtB -> B_38_804_0 [ 9 ] = _rtB ->
B_38_125_0 ; _rtB -> B_38_804_0 [ 10 ] = _rtB -> B_38_802_0 ; _rtB ->
B_38_804_0 [ 11 ] = _rtB -> B_38_655_0 ; _rtB -> B_38_804_0 [ 12 ] = _rtB ->
B_38_803_0 ; _rtB -> B_38_804_0 [ 13 ] = _rtB -> B_38_249_0 ;
ssCallAccelRunBlock ( S , 38 , 805 , SS_CALL_MDL_OUTPUTS ) ; } _rtB ->
B_38_808_0 [ 0 ] = _rtB -> B_38_718_0 ; _rtB -> B_38_808_0 [ 1 ] = _rtB ->
B_38_771_0 ; ssCallAccelRunBlock ( S , 38 , 809 , SS_CALL_MDL_OUTPUTS ) ;
_rtB -> B_38_812_0 = _rtB -> B_38_115_0 - _rtB -> B_38_810_0 ; _rtB ->
B_38_813_0 = _rtB -> B_38_123_0 - _rtB -> B_38_811_0 ; _rtB -> B_38_814_0 = (
real_T ) ( _rtB -> B_38_813_0 > 0.0 ) * _rtB -> B_38_813_0 * ( real_T ) (
_rtB -> B_38_121_0 > 0.0 ) ; _rtB -> B_38_815_0 = ( real_T ) ( _rtB ->
B_38_812_0 > 0.0 ) * _rtB -> B_38_812_0 * ( real_T ) ( _rtB -> B_38_113_0 >
0.0 ) ; _rtB -> B_38_818_0 [ 0 ] = _rtB -> B_38_690_0 ; _rtB -> B_38_818_0 [
1 ] = _rtB -> B_38_691_0 ; ssCallAccelRunBlock ( S , 38 , 819 ,
SS_CALL_MDL_OUTPUTS ) ; _rtB -> B_38_822_0 [ 0 ] = _rtB -> B_38_698_0 [ 0 ] ;
_rtB -> B_38_822_0 [ 1 ] = _rtB -> B_38_751_0 [ 0 ] ; ssCallAccelRunBlock ( S
, 38 , 823 , SS_CALL_MDL_OUTPUTS ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if
( isHit != 0 ) { _rtB -> B_38_824_0 = _rtB -> B_38_50_0 ; _rtB -> B_38_825_0
= _rtB -> B_38_17_0 ; } ssCallAccelRunBlock ( S , 38 , 826 ,
SS_CALL_MDL_OUTPUTS ) ; _rtB -> B_38_829_0 [ 0 ] = _rtB -> B_38_689_0 ; _rtB
-> B_38_829_0 [ 1 ] = _rtB -> B_38_719_0 ; ssCallAccelRunBlock ( S , 38 , 830
, SS_CALL_MDL_OUTPUTS ) ; _rtB -> B_38_831_0 = 0.0 ; _rtB -> B_38_831_0 +=
_rtP -> P_353 * _rtX -> smooth_CSTATE ; _rtB -> B_38_832_0 = 0.0 ; _rtB ->
B_38_832_0 += _rtP -> P_355 * _rtX -> smooth1_CSTATE ; ssCallAccelRunBlock (
S , 38 , 833 , SS_CALL_MDL_OUTPUTS ) ; if ( ssIsMajorTimeStep ( S ) != 0 ) {
_rtDW -> Abs1_MODE = ( _rtB -> B_38_92_0 [ 1 ] >= 0.0 ) ; } _rtB ->
B_38_834_0 = _rtDW -> Abs1_MODE > 0 ? _rtB -> B_38_92_0 [ 1 ] : - _rtB ->
B_38_92_0 [ 1 ] ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
if ( ssIsMajorTimeStep ( S ) != 0 ) { _rtDW -> Compare_Mode_m = ( _rtB ->
B_38_834_0 < _rtB -> B_38_835_0 ) ; } if ( _rtDW -> Compare_Mode_m ) {
ssSetStopRequested ( S , 1 ) ; } } _rtB -> B_38_838_0 [ 0 ] = _rtB ->
B_38_105_0 [ 0 ] + _rtB -> B_38_108_0 [ 0 ] ; _rtB -> B_38_839_0 [ 0 ] = _rtP
-> P_357 * _rtB -> B_38_838_0 [ 0 ] ; _rtB -> B_38_844_0 [ 0 ] = _rtB ->
B_38_839_0 [ 0 ] ; _rtB -> B_38_841_0 [ 0 ] = _rtB -> B_38_106_0 [ 0 ] + _rtB
-> B_38_109_0 [ 0 ] ; _rtB -> B_38_842_0 [ 0 ] = _rtP -> P_358 * _rtB ->
B_38_841_0 [ 0 ] ; _rtB -> B_38_844_0 [ 3 ] = _rtB -> B_38_842_0 [ 0 ] ; _rtB
-> B_38_838_0 [ 1 ] = _rtB -> B_38_105_0 [ 1 ] + _rtB -> B_38_108_0 [ 1 ] ;
_rtB -> B_38_839_0 [ 1 ] = _rtP -> P_357 * _rtB -> B_38_838_0 [ 1 ] ; _rtB ->
B_38_844_0 [ 1 ] = _rtB -> B_38_839_0 [ 1 ] ; _rtB -> B_38_841_0 [ 1 ] = _rtB
-> B_38_106_0 [ 1 ] + _rtB -> B_38_109_0 [ 1 ] ; _rtB -> B_38_842_0 [ 1 ] =
_rtP -> P_358 * _rtB -> B_38_841_0 [ 1 ] ; _rtB -> B_38_844_0 [ 4 ] = _rtB ->
B_38_842_0 [ 1 ] ; _rtB -> B_38_838_0 [ 2 ] = _rtB -> B_38_105_0 [ 2 ] + _rtB
-> B_38_108_0 [ 2 ] ; _rtB -> B_38_839_0 [ 2 ] = _rtP -> P_357 * _rtB ->
B_38_838_0 [ 2 ] ; _rtB -> B_38_844_0 [ 2 ] = _rtB -> B_38_839_0 [ 2 ] ; _rtB
-> B_38_841_0 [ 2 ] = _rtB -> B_38_106_0 [ 2 ] + _rtB -> B_38_109_0 [ 2 ] ;
_rtB -> B_38_842_0 [ 2 ] = _rtP -> P_358 * _rtB -> B_38_841_0 [ 2 ] ; _rtB ->
B_38_844_0 [ 5 ] = _rtB -> B_38_842_0 [ 2 ] ; ssCallAccelRunBlock ( S , 38 ,
845 , SS_CALL_MDL_OUTPUTS ) ; _rtB -> B_38_848_0 [ 0 ] = _rtB -> B_38_92_0 [
0 ] ; _rtB -> B_38_848_0 [ 3 ] = _rtB -> B_38_93_0 [ 0 ] ; _rtB -> B_38_848_0
[ 1 ] = _rtB -> B_38_92_0 [ 1 ] ; _rtB -> B_38_848_0 [ 4 ] = _rtB ->
B_38_93_0 [ 1 ] ; _rtB -> B_38_848_0 [ 2 ] = _rtB -> B_38_92_0 [ 2 ] ; _rtB
-> B_38_848_0 [ 5 ] = _rtB -> B_38_93_0 [ 2 ] ; ssCallAccelRunBlock ( S , 38
, 849 , SS_CALL_MDL_OUTPUTS ) ; _rtB -> B_38_850_0 [ 0 ] = _rtB -> B_38_92_0
[ 0 ] - _rtB -> B_38_839_0 [ 0 ] ; _rtB -> B_38_851_0 [ 0 ] = _rtB ->
B_38_93_0 [ 0 ] - _rtB -> B_38_842_0 [ 0 ] ; _rtB -> B_38_850_0 [ 1 ] = _rtB
-> B_38_92_0 [ 1 ] - _rtB -> B_38_839_0 [ 1 ] ; _rtB -> B_38_851_0 [ 1 ] =
_rtB -> B_38_93_0 [ 1 ] - _rtB -> B_38_842_0 [ 1 ] ; _rtB -> B_38_850_0 [ 2 ]
= _rtB -> B_38_92_0 [ 2 ] - _rtB -> B_38_839_0 [ 2 ] ; _rtB -> B_38_851_0 [ 2
] = _rtB -> B_38_93_0 [ 2 ] - _rtB -> B_38_842_0 [ 2 ] ; _rtB -> B_38_852_0 =
( _rtB -> B_38_850_0 [ 1 ] * _rtB -> B_38_851_0 [ 0 ] - _rtB -> B_38_850_0 [
0 ] * _rtB -> B_38_851_0 [ 1 ] ) / 0.12249999999999998 ; _rtB -> B_38_853_0 =
muDoubleScalarAtan2 ( _rtB -> B_38_850_0 [ 0 ] , _rtB -> B_38_850_0 [ 1 ] ) ;
_rtB -> B_38_854_0 = ssGetT ( S ) ; ssCallAccelRunBlock ( S , 38 , 855 ,
SS_CALL_MDL_OUTPUTS ) ; _rtB -> B_38_859_0 [ 0 ] = _rtB -> B_38_394_0 ; _rtB
-> B_38_859_0 [ 1 ] = _rtB -> B_38_330_0 ; _rtB -> B_38_859_0 [ 2 ] = _rtB ->
B_38_249_0 ; ssCallAccelRunBlock ( S , 38 , 860 , SS_CALL_MDL_OUTPUTS ) ;
_rtB -> B_38_864_0 [ 0 ] = _rtB -> B_38_119_0 ; _rtB -> B_38_864_0 [ 1 ] =
_rtB -> B_38_117_0 ; _rtB -> B_38_864_0 [ 2 ] = _rtB -> B_38_123_0 ; _rtB ->
B_38_864_0 [ 3 ] = _rtB -> B_38_121_0 ; _rtB -> B_38_864_0 [ 4 ] = _rtB ->
B_38_127_0 ; _rtB -> B_38_864_0 [ 5 ] = _rtB -> B_38_125_0 ;
ssCallAccelRunBlock ( S , 38 , 865 , SS_CALL_MDL_OUTPUTS ) ; _rtB ->
B_38_875_0 [ 0 ] = _rtB -> B_38_361_0 ; _rtB -> B_38_875_0 [ 1 ] = _rtB ->
B_38_387_0 ; _rtB -> B_38_875_0 [ 2 ] = _rtB -> B_38_390_0 ; _rtB ->
B_38_875_0 [ 3 ] = _rtB -> B_38_291_0 ; _rtB -> B_38_875_0 [ 4 ] = _rtB ->
B_38_322_0 ; _rtB -> B_38_875_0 [ 5 ] = _rtB -> B_38_325_0 ; _rtB ->
B_38_875_0 [ 6 ] = _rtB -> B_38_183_0 ; _rtB -> B_38_875_0 [ 7 ] = _rtB ->
B_38_214_0 ; _rtB -> B_38_875_0 [ 8 ] = _rtB -> B_38_244_0 ;
ssCallAccelRunBlock ( S , 38 , 876 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 38 , 877 , SS_CALL_MDL_OUTPUTS ) ; _rtB ->
B_38_878_0 = _rtP -> P_359 * _rtB -> B_38_169_0 ; _rtB -> B_38_880_0 = _rtB
-> B_38_878_0 - _rtB -> B_38_879_0 ; if ( ssIsMajorTimeStep ( S ) != 0 ) {
_rtDW -> Abs_MODE = ( _rtB -> B_38_880_0 >= 0.0 ) ; } _rtB -> B_38_881_0 =
_rtDW -> Abs_MODE > 0 ? _rtB -> B_38_880_0 : - _rtB -> B_38_880_0 ; _rtB ->
B_38_883_0 = _rtB -> B_38_881_0 / _rtB -> B_38_882_0 ; if ( ( _rtB ->
B_38_883_0 < 0.0 ) && ( _rtB -> B_38_884_0 > muDoubleScalarFloor ( _rtB ->
B_38_884_0 ) ) ) { rtb_B_38_693_0 = - muDoubleScalarPower ( - _rtB ->
B_38_883_0 , _rtB -> B_38_884_0 ) ; } else { rtb_B_38_693_0 =
muDoubleScalarPower ( _rtB -> B_38_883_0 , _rtB -> B_38_884_0 ) ; } _rtB ->
B_38_886_0 = _rtP -> P_363 * rtb_B_38_693_0 ; _rtB -> B_38_888_0 = 0.0 ; _rtB
-> B_38_888_0 += _rtP -> P_366 * _rtX -> StateSpace_CSTATE ; _rtB ->
B_38_889_0 = muDoubleScalarExp ( _rtB -> B_38_886_0 ) * _rtB -> B_38_888_0 ;
_rtB -> B_38_891_0 = _rtB -> B_38_878_0 - _rtB -> B_38_890_0 ; _rtB ->
B_38_893_0 = _rtB -> B_38_891_0 / _rtB -> B_38_892_0 ; _rtB -> B_38_899_0 =
_rtB -> B_38_893_0 * _rtB -> B_38_893_0 * ( real_T ) ( _rtB -> B_38_891_0 <
0.0 ) + rtb_B_38_177_0 ; _rtB -> B_38_901_0 = _rtB -> B_38_878_0 - _rtB ->
B_38_900_0 ; _rtB -> B_38_903_0 = _rtB -> B_38_901_0 / _rtB -> B_38_902_0 ;
_rtB -> B_38_906_0 = _rtB -> B_38_903_0 * _rtB -> B_38_903_0 * ( real_T ) (
_rtB -> B_38_878_0 > 1.0 ) + _rtB -> B_38_889_0 ; _rtB -> B_38_907_0 = _rtB
-> B_38_899_0 / _rtB -> B_38_906_0 ; _rtB -> B_38_908_0 = _rtB -> B_38_907_0
- _rtB -> B_38_897_0 ; _rtB -> B_38_909_0 = 1.0 / _rtB -> B_38_898_0 * _rtB
-> B_38_908_0 ; _rtB -> B_38_914_0 = _rtP -> P_378 * _rtB -> B_38_907_0 ;
_rtB -> B_38_915_0 = _rtP -> P_379 * _rtB -> B_38_909_0 ; _rtB -> B_38_916_0
= _rtP -> P_380 * _rtB -> B_38_908_0 ; _rtB -> B_38_917_0 = _rtB ->
B_38_916_0 + _rtB -> B_38_913_0 ; _rtB -> B_38_918_0 = _rtB -> B_38_909_0 +
_rtB -> B_38_911_0 ; _rtB -> B_38_919_0 = _rtB -> B_38_911_0 - _rtB ->
B_38_915_0 ; _rtB -> B_38_920_0 = _rtB -> B_38_918_0 / _rtB -> B_38_919_0 ;
_rtB -> B_38_923_0 = ( real_T ) ( _rtB -> B_38_907_0 > 1.0 ) * _rtB ->
B_38_920_0 * ( real_T ) ( _rtB -> B_38_907_0 <= _rtB -> B_38_912_0 ) + (
real_T ) ( _rtB -> B_38_907_0 > _rtB -> B_38_912_0 ) * _rtB -> B_38_917_0 ;
_rtB -> B_38_924_0 = _rtB -> B_38_910_0 + _rtB -> B_38_914_0 ; _rtB ->
B_38_925_0 = _rtB -> B_38_907_0 - _rtB -> B_38_910_0 ; _rtB -> B_38_926_0 =
_rtB -> B_38_925_0 / _rtB -> B_38_924_0 ; _rtB -> B_38_928_0 = ( real_T ) (
_rtB -> B_38_907_0 <= 1.0 ) * _rtB -> B_38_926_0 + _rtB -> B_38_923_0 ; _rtB
-> B_38_929_0 = _rtP -> P_381 * _rtB -> B_38_928_0 ; _rtB -> B_38_930_0 =
_rtB -> B_38_181_0 ; if ( ssIsMajorTimeStep ( S ) != 0 ) { _rtDW ->
Switch_Mode_de = ( _rtB -> B_38_930_0 > _rtP -> P_383 ) ; } if ( _rtDW ->
Switch_Mode_de ) { _rtB -> B_38_932_0 = _rtB -> B_38_930_0 ; } else { _rtB ->
B_38_932_0 = _rtB -> B_38_931_0 ; } _rtB -> B_38_933_0 = _rtP -> P_384 * _rtB
-> B_38_932_0 ; _rtB -> B_38_935_0 = _rtB -> B_38_785_0 ; if (
ssIsMajorTimeStep ( S ) != 0 ) { _rtDW -> Switch_Mode_ou = ( _rtB ->
B_38_935_0 > _rtP -> P_386 ) ; } if ( _rtDW -> Switch_Mode_ou ) { _rtB ->
B_38_936_0 = _rtB -> B_38_935_0 ; } else { _rtB -> B_38_936_0 = _rtB ->
B_38_934_0 ; } _rtB -> B_38_938_0 = _rtB -> B_38_169_0 ; _rtB -> B_38_941_0 =
_rtB -> B_38_929_0 ; if ( ssIsMajorTimeStep ( S ) != 0 ) { _rtDW ->
Switch_Mode_kr = ( _rtB -> B_38_938_0 > _rtP -> P_388 ) ; _rtDW ->
Switch_Mode_ka = ( _rtB -> B_38_941_0 > _rtP -> P_390 ) ; }
ssCallAccelRunBlock ( S , 38 , 943 , SS_CALL_MDL_OUTPUTS ) ; _rtB ->
B_38_944_0 = _rtP -> P_391 * _rtB -> B_38_347_0 ; _rtB -> B_38_946_0 = _rtB
-> B_38_944_0 - _rtB -> B_38_945_0 ; if ( ssIsMajorTimeStep ( S ) != 0 ) {
_rtDW -> Abs_MODE_l = ( _rtB -> B_38_946_0 >= 0.0 ) ; } _rtB -> B_38_947_0 =
_rtDW -> Abs_MODE_l > 0 ? _rtB -> B_38_946_0 : - _rtB -> B_38_946_0 ; _rtB ->
B_38_949_0 = _rtB -> B_38_947_0 / _rtB -> B_38_948_0 ; if ( ( _rtB ->
B_38_949_0 < 0.0 ) && ( _rtB -> B_38_950_0 > muDoubleScalarFloor ( _rtB ->
B_38_950_0 ) ) ) { rtb_B_38_693_0 = - muDoubleScalarPower ( - _rtB ->
B_38_949_0 , _rtB -> B_38_950_0 ) ; } else { rtb_B_38_693_0 =
muDoubleScalarPower ( _rtB -> B_38_949_0 , _rtB -> B_38_950_0 ) ; } _rtB ->
B_38_952_0 = _rtP -> P_395 * rtb_B_38_693_0 ; _rtB -> B_38_954_0 = 0.0 ; _rtB
-> B_38_954_0 += _rtP -> P_398 * _rtX -> StateSpace_CSTATE_f ; _rtB ->
B_38_955_0 = muDoubleScalarExp ( _rtB -> B_38_952_0 ) * _rtB -> B_38_954_0 ;
_rtB -> B_38_957_0 = _rtB -> B_38_944_0 - _rtB -> B_38_956_0 ; _rtB ->
B_38_959_0 = _rtB -> B_38_957_0 / _rtB -> B_38_958_0 ; _rtB -> B_38_965_0 =
_rtB -> B_38_959_0 * _rtB -> B_38_959_0 * ( real_T ) ( _rtB -> B_38_957_0 <
0.0 ) + rtb_B_38_355_0 ; _rtB -> B_38_967_0 = _rtB -> B_38_944_0 - _rtB ->
B_38_966_0 ; _rtB -> B_38_969_0 = _rtB -> B_38_967_0 / _rtB -> B_38_968_0 ;
_rtB -> B_38_972_0 = _rtB -> B_38_969_0 * _rtB -> B_38_969_0 * ( real_T ) (
_rtB -> B_38_944_0 > 1.0 ) + _rtB -> B_38_955_0 ; _rtB -> B_38_973_0 = _rtB
-> B_38_965_0 / _rtB -> B_38_972_0 ; _rtB -> B_38_974_0 = _rtB -> B_38_973_0
- _rtB -> B_38_963_0 ; _rtB -> B_38_975_0 = 1.0 / _rtB -> B_38_964_0 * _rtB
-> B_38_974_0 ; _rtB -> B_38_980_0 = _rtP -> P_410 * _rtB -> B_38_973_0 ;
_rtB -> B_38_981_0 = _rtP -> P_411 * _rtB -> B_38_975_0 ; _rtB -> B_38_982_0
= _rtP -> P_412 * _rtB -> B_38_974_0 ; _rtB -> B_38_983_0 = _rtB ->
B_38_982_0 + _rtB -> B_38_979_0 ; _rtB -> B_38_984_0 = _rtB -> B_38_975_0 +
_rtB -> B_38_977_0 ; _rtB -> B_38_985_0 = _rtB -> B_38_977_0 - _rtB ->
B_38_981_0 ; _rtB -> B_38_986_0 = _rtB -> B_38_984_0 / _rtB -> B_38_985_0 ;
_rtB -> B_38_989_0 = ( real_T ) ( _rtB -> B_38_973_0 > 1.0 ) * _rtB ->
B_38_986_0 * ( real_T ) ( _rtB -> B_38_973_0 <= _rtB -> B_38_978_0 ) + (
real_T ) ( _rtB -> B_38_973_0 > _rtB -> B_38_978_0 ) * _rtB -> B_38_983_0 ;
_rtB -> B_38_990_0 = _rtB -> B_38_976_0 + _rtB -> B_38_980_0 ; _rtB ->
B_38_991_0 = _rtB -> B_38_973_0 - _rtB -> B_38_976_0 ; _rtB -> B_38_992_0 =
_rtB -> B_38_991_0 / _rtB -> B_38_990_0 ; _rtB -> B_38_994_0 = ( real_T ) (
_rtB -> B_38_973_0 <= 1.0 ) * _rtB -> B_38_992_0 + _rtB -> B_38_989_0 ; _rtB
-> B_38_995_0 = _rtP -> P_413 * _rtB -> B_38_994_0 ; _rtB -> B_38_996_0 =
_rtB -> B_38_359_0 ; if ( ssIsMajorTimeStep ( S ) != 0 ) { _rtDW ->
Switch_Mode_cj = ( _rtB -> B_38_996_0 > _rtP -> P_415 ) ; } if ( _rtDW ->
Switch_Mode_cj ) { _rtB -> B_38_998_0 = _rtB -> B_38_996_0 ; } else { _rtB ->
B_38_998_0 = _rtB -> B_38_997_0 ; } _rtB -> B_38_999_0 = _rtP -> P_416 * _rtB
-> B_38_998_0 ; _rtB -> B_38_1001_0 = _rtB -> B_38_758_0 ; if (
ssIsMajorTimeStep ( S ) != 0 ) { _rtDW -> Switch_Mode_e = ( _rtB ->
B_38_1001_0 > _rtP -> P_418 ) ; } if ( _rtDW -> Switch_Mode_e ) { _rtB ->
B_38_1002_0 = _rtB -> B_38_1001_0 ; } else { _rtB -> B_38_1002_0 = _rtB ->
B_38_1000_0 ; } _rtB -> B_38_1004_0 = _rtB -> B_38_347_0 ; _rtB ->
B_38_1007_0 = _rtB -> B_38_995_0 ; if ( ssIsMajorTimeStep ( S ) != 0 ) {
_rtDW -> Switch_Mode_m3 = ( _rtB -> B_38_1004_0 > _rtP -> P_420 ) ; _rtDW ->
Switch_Mode_cl = ( _rtB -> B_38_1007_0 > _rtP -> P_422 ) ; }
ssCallAccelRunBlock ( S , 38 , 1009 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 38 , 1010 , SS_CALL_MDL_OUTPUTS ) ; isHit =
ssIsSampleHit ( S , 3 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_38_1012_0 = _rtP
-> P_424 ; ssCallAccelRunBlock ( S , 36 , 0 , SS_CALL_MDL_OUTPUTS ) ; } _rtB
-> B_38_1014_0 = _rtB -> B_36_0_1 [ 0 ] + _rtB -> B_38_277_0 ; if (
ssIsMajorTimeStep ( S ) != 0 ) { _rtDW -> Switch_Mode_f = ( _rtB ->
B_38_1014_0 > _rtP -> P_426 ) ; } if ( _rtDW -> Switch_Mode_f ) { _rtB ->
B_38_1016_0 = _rtB -> B_38_1014_0 ; } else { _rtB -> B_38_1016_0 = _rtB ->
B_38_1015_0 ; } _rtB -> B_38_1017_0 = _rtP -> P_427 * _rtB -> B_38_1016_0 ;
_rtB -> B_38_1018_0 = _rtP -> P_428 * _rtB -> B_38_277_0 ; _rtB ->
B_38_1020_0 = _rtB -> B_38_1018_0 - _rtB -> B_38_1019_0 ; if (
ssIsMajorTimeStep ( S ) != 0 ) { _rtDW -> Abs_MODE_a = ( _rtB -> B_38_1020_0
>= 0.0 ) ; } _rtB -> B_38_1021_0 = _rtDW -> Abs_MODE_a > 0 ? _rtB ->
B_38_1020_0 : - _rtB -> B_38_1020_0 ; _rtB -> B_38_1023_0 = _rtB ->
B_38_1021_0 / _rtB -> B_38_1022_0 ; if ( ( _rtB -> B_38_1023_0 < 0.0 ) && (
_rtB -> B_38_1024_0 > muDoubleScalarFloor ( _rtB -> B_38_1024_0 ) ) ) {
rtb_B_38_693_0 = - muDoubleScalarPower ( - _rtB -> B_38_1023_0 , _rtB ->
B_38_1024_0 ) ; } else { rtb_B_38_693_0 = muDoubleScalarPower ( _rtB ->
B_38_1023_0 , _rtB -> B_38_1024_0 ) ; } _rtB -> B_38_1026_0 = _rtP -> P_432 *
rtb_B_38_693_0 ; _rtB -> B_38_1028_0 = 0.0 ; _rtB -> B_38_1028_0 += _rtP ->
P_435 * _rtX -> StateSpace_CSTATE_a ; _rtB -> B_38_1029_0 = muDoubleScalarExp
( _rtB -> B_38_1026_0 ) * _rtB -> B_38_1028_0 ; _rtB -> B_38_1031_0 = _rtB ->
B_38_1018_0 - _rtB -> B_38_1030_0 ; _rtB -> B_38_1033_0 = _rtB -> B_38_1031_0
/ _rtB -> B_38_1032_0 ; _rtB -> B_38_1039_0 = _rtB -> B_38_1033_0 * _rtB ->
B_38_1033_0 * ( real_T ) ( _rtB -> B_38_1031_0 < 0.0 ) + rtb_B_38_285_0 ;
_rtB -> B_38_1041_0 = _rtB -> B_38_1018_0 - _rtB -> B_38_1040_0 ; _rtB ->
B_38_1043_0 = _rtB -> B_38_1041_0 / _rtB -> B_38_1042_0 ; _rtB -> B_38_1046_0
= _rtB -> B_38_1043_0 * _rtB -> B_38_1043_0 * ( real_T ) ( _rtB ->
B_38_1018_0 > 1.0 ) + _rtB -> B_38_1029_0 ; _rtB -> B_38_1047_0 = _rtB ->
B_38_1039_0 / _rtB -> B_38_1046_0 ; _rtB -> B_38_1048_0 = _rtB -> B_38_1047_0
- _rtB -> B_38_1037_0 ; _rtB -> B_38_1049_0 = 1.0 / _rtB -> B_38_1038_0 *
_rtB -> B_38_1048_0 ; _rtB -> B_38_1054_0 = _rtP -> P_447 * _rtB ->
B_38_1047_0 ; _rtB -> B_38_1055_0 = _rtP -> P_448 * _rtB -> B_38_1049_0 ;
_rtB -> B_38_1056_0 = _rtP -> P_449 * _rtB -> B_38_1048_0 ; _rtB ->
B_38_1057_0 = _rtB -> B_38_1056_0 + _rtB -> B_38_1053_0 ; _rtB -> B_38_1058_0
= _rtB -> B_38_1049_0 + _rtB -> B_38_1051_0 ; _rtB -> B_38_1059_0 = _rtB ->
B_38_1051_0 - _rtB -> B_38_1055_0 ; _rtB -> B_38_1060_0 = _rtB -> B_38_1058_0
/ _rtB -> B_38_1059_0 ; _rtB -> B_38_1063_0 = ( real_T ) ( _rtB ->
B_38_1047_0 > 1.0 ) * _rtB -> B_38_1060_0 * ( real_T ) ( _rtB -> B_38_1047_0
<= _rtB -> B_38_1052_0 ) + ( real_T ) ( _rtB -> B_38_1047_0 > _rtB ->
B_38_1052_0 ) * _rtB -> B_38_1057_0 ; _rtB -> B_38_1064_0 = _rtB ->
B_38_1050_0 + _rtB -> B_38_1054_0 ; _rtB -> B_38_1065_0 = _rtB -> B_38_1047_0
- _rtB -> B_38_1050_0 ; _rtB -> B_38_1066_0 = _rtB -> B_38_1065_0 / _rtB ->
B_38_1064_0 ; _rtB -> B_38_1068_0 = ( real_T ) ( _rtB -> B_38_1047_0 <= 1.0 )
* _rtB -> B_38_1066_0 + _rtB -> B_38_1063_0 ; _rtB -> B_38_1069_0 = _rtP ->
P_450 * _rtB -> B_38_1068_0 ; _rtB -> B_38_1070_0 = _rtB -> B_38_289_0 ; if (
ssIsMajorTimeStep ( S ) != 0 ) { _rtDW -> Switch_Mode_n = ( _rtB ->
B_38_1070_0 > _rtP -> P_452 ) ; } if ( _rtDW -> Switch_Mode_n ) { _rtB ->
B_38_1072_0 = _rtB -> B_38_1070_0 ; } else { _rtB -> B_38_1072_0 = _rtB ->
B_38_1071_0 ; } _rtB -> B_38_1073_0 = _rtP -> P_453 * _rtB -> B_38_1072_0 ;
_rtB -> B_38_1075_0 = _rtB -> B_38_1017_0 - _rtB -> B_38_1074_0 ; if ( _rtB
-> B_38_1075_0 > _rtP -> P_455 ) { _rtB -> B_38_1076_0 = _rtP -> P_455 ; }
else if ( _rtB -> B_38_1075_0 < _rtP -> P_456 ) { _rtB -> B_38_1076_0 = _rtP
-> P_456 ; } else { _rtB -> B_38_1076_0 = _rtB -> B_38_1075_0 ; } _rtB ->
B_38_1078_0 = _rtB -> B_38_770_0 ; if ( ssIsMajorTimeStep ( S ) != 0 ) {
_rtDW -> Switch_Mode_h = ( _rtB -> B_38_1078_0 > _rtP -> P_458 ) ; } if (
_rtDW -> Switch_Mode_h ) { _rtB -> B_38_1079_0 = _rtB -> B_38_1078_0 ; } else
{ _rtB -> B_38_1079_0 = _rtB -> B_38_1077_0 ; } _rtB -> B_38_1081_0 = _rtB ->
B_38_1069_0 ; _rtB -> B_38_1083_0 = _rtP -> P_461 * _rtB -> B_38_372_0 ; _rtB
-> B_38_1085_0 = _rtB -> B_38_1083_0 - _rtB -> B_38_1084_0 ; if (
ssIsMajorTimeStep ( S ) != 0 ) { _rtDW -> Switch_Mode_o2 = ( _rtB ->
B_38_1081_0 > _rtP -> P_460 ) ; _rtDW -> Abs_MODE_f = ( _rtB -> B_38_1085_0
>= 0.0 ) ; } _rtB -> B_38_1086_0 = _rtDW -> Abs_MODE_f > 0 ? _rtB ->
B_38_1085_0 : - _rtB -> B_38_1085_0 ; _rtB -> B_38_1088_0 = _rtB ->
B_38_1086_0 / _rtB -> B_38_1087_0 ; if ( ( _rtB -> B_38_1088_0 < 0.0 ) && (
_rtB -> B_38_1089_0 > muDoubleScalarFloor ( _rtB -> B_38_1089_0 ) ) ) {
rtb_B_38_693_0 = - muDoubleScalarPower ( - _rtB -> B_38_1088_0 , _rtB ->
B_38_1089_0 ) ; } else { rtb_B_38_693_0 = muDoubleScalarPower ( _rtB ->
B_38_1088_0 , _rtB -> B_38_1089_0 ) ; } _rtB -> B_38_1091_0 = _rtP -> P_465 *
rtb_B_38_693_0 ; _rtB -> B_38_1093_0 = 0.0 ; _rtB -> B_38_1093_0 += _rtP ->
P_468 * _rtX -> StateSpace_CSTATE_m ; _rtB -> B_38_1094_0 = muDoubleScalarExp
( _rtB -> B_38_1091_0 ) * _rtB -> B_38_1093_0 ; _rtB -> B_38_1096_0 = _rtB ->
B_38_1083_0 - _rtB -> B_38_1095_0 ; _rtB -> B_38_1098_0 = _rtB -> B_38_1096_0
/ _rtB -> B_38_1097_0 ; _rtB -> B_38_1104_0 = _rtB -> B_38_1098_0 * _rtB ->
B_38_1098_0 * ( real_T ) ( _rtB -> B_38_1096_0 < 0.0 ) + rtb_B_38_380_0 ;
_rtB -> B_38_1106_0 = _rtB -> B_38_1083_0 - _rtB -> B_38_1105_0 ; _rtB ->
B_38_1108_0 = _rtB -> B_38_1106_0 / _rtB -> B_38_1107_0 ; _rtB -> B_38_1111_0
= _rtB -> B_38_1108_0 * _rtB -> B_38_1108_0 * ( real_T ) ( _rtB ->
B_38_1083_0 > 1.0 ) + _rtB -> B_38_1094_0 ; _rtB -> B_38_1112_0 = _rtB ->
B_38_1104_0 / _rtB -> B_38_1111_0 ; _rtB -> B_38_1113_0 = _rtB -> B_38_1112_0
- _rtB -> B_38_1102_0 ; _rtB -> B_38_1114_0 = 1.0 / _rtB -> B_38_1103_0 *
_rtB -> B_38_1113_0 ; _rtB -> B_38_1119_0 = _rtP -> P_480 * _rtB ->
B_38_1112_0 ; _rtB -> B_38_1120_0 = _rtP -> P_481 * _rtB -> B_38_1114_0 ;
_rtB -> B_38_1121_0 = _rtP -> P_482 * _rtB -> B_38_1113_0 ; _rtB ->
B_38_1122_0 = _rtB -> B_38_1121_0 + _rtB -> B_38_1118_0 ; _rtB -> B_38_1123_0
= _rtB -> B_38_1114_0 + _rtB -> B_38_1116_0 ; _rtB -> B_38_1124_0 = _rtB ->
B_38_1116_0 - _rtB -> B_38_1120_0 ; _rtB -> B_38_1125_0 = _rtB -> B_38_1123_0
/ _rtB -> B_38_1124_0 ; _rtB -> B_38_1128_0 = ( real_T ) ( _rtB ->
B_38_1112_0 > 1.0 ) * _rtB -> B_38_1125_0 * ( real_T ) ( _rtB -> B_38_1112_0
<= _rtB -> B_38_1117_0 ) + ( real_T ) ( _rtB -> B_38_1112_0 > _rtB ->
B_38_1117_0 ) * _rtB -> B_38_1122_0 ; _rtB -> B_38_1129_0 = _rtB ->
B_38_1115_0 + _rtB -> B_38_1119_0 ; _rtB -> B_38_1130_0 = _rtB -> B_38_1112_0
- _rtB -> B_38_1115_0 ; _rtB -> B_38_1131_0 = _rtB -> B_38_1130_0 / _rtB ->
B_38_1129_0 ; _rtB -> B_38_1133_0 = ( real_T ) ( _rtB -> B_38_1112_0 <= 1.0 )
* _rtB -> B_38_1131_0 + _rtB -> B_38_1128_0 ; _rtB -> B_38_1134_0 = _rtP ->
P_483 * _rtB -> B_38_1133_0 ; isHit = ssIsSampleHit ( S , 3 , 0 ) ; if (
isHit != 0 ) { ssCallAccelRunBlock ( S , 34 , 0 , SS_CALL_MDL_OUTPUTS ) ;
_rtB -> B_38_1136_0 = _rtP -> P_484 * _rtB -> B_34_0_1 [ 0 ] ; } _rtB ->
B_38_1137_0 = _rtB -> B_38_1136_0 + _rtB -> B_38_372_0 ; if (
ssIsMajorTimeStep ( S ) != 0 ) { _rtDW -> Switch_Mode_l = ( _rtB ->
B_38_1137_0 > _rtP -> P_486 ) ; } if ( _rtDW -> Switch_Mode_l ) { _rtB ->
B_38_1139_0 = _rtB -> B_38_1137_0 ; } else { _rtB -> B_38_1139_0 = _rtB ->
B_38_1138_0 ; } _rtB -> B_38_1140_0 = _rtP -> P_487 * _rtB -> B_38_1139_0 ;
_rtB -> B_38_1142_0 = _rtB -> B_38_1140_0 - _rtB -> B_38_1141_0 ; if ( _rtB
-> B_38_1142_0 > _rtP -> P_489 ) { _rtB -> B_38_1143_0 = _rtP -> P_489 ; }
else if ( _rtB -> B_38_1142_0 < _rtP -> P_490 ) { _rtB -> B_38_1143_0 = _rtP
-> P_490 ; } else { _rtB -> B_38_1143_0 = _rtB -> B_38_1142_0 ; } _rtB ->
B_38_1145_0 = _rtB -> B_38_768_0 ; if ( ssIsMajorTimeStep ( S ) != 0 ) {
_rtDW -> Switch_Mode_ol = ( _rtB -> B_38_1145_0 > _rtP -> P_492 ) ; } if (
_rtDW -> Switch_Mode_ol ) { _rtB -> B_38_1146_0 = _rtB -> B_38_1145_0 ; }
else { _rtB -> B_38_1146_0 = _rtB -> B_38_1144_0 ; } _rtB -> B_38_1148_0 =
_rtB -> B_38_1134_0 ; if ( ssIsMajorTimeStep ( S ) != 0 ) { _rtDW ->
Switch_Mode_el = ( _rtB -> B_38_1148_0 > _rtP -> P_494 ) ; }
ssCallAccelRunBlock ( S , 38 , 1150 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 38 , 1151 , SS_CALL_MDL_OUTPUTS ) ; _rtB ->
B_38_1152_0 = _rtP -> P_495 * _rtB -> B_38_230_0 ; _rtB -> B_38_1154_0 = _rtB
-> B_38_1152_0 - _rtB -> B_38_1153_0 ; if ( ssIsMajorTimeStep ( S ) != 0 ) {
_rtDW -> Abs_MODE_ao = ( _rtB -> B_38_1154_0 >= 0.0 ) ; } _rtB -> B_38_1155_0
= _rtDW -> Abs_MODE_ao > 0 ? _rtB -> B_38_1154_0 : - _rtB -> B_38_1154_0 ;
_rtB -> B_38_1157_0 = _rtB -> B_38_1155_0 / _rtB -> B_38_1156_0 ; if ( ( _rtB
-> B_38_1157_0 < 0.0 ) && ( _rtB -> B_38_1158_0 > muDoubleScalarFloor ( _rtB
-> B_38_1158_0 ) ) ) { rtb_B_38_693_0 = - muDoubleScalarPower ( - _rtB ->
B_38_1157_0 , _rtB -> B_38_1158_0 ) ; } else { rtb_B_38_693_0 =
muDoubleScalarPower ( _rtB -> B_38_1157_0 , _rtB -> B_38_1158_0 ) ; } _rtB ->
B_38_1160_0 = _rtP -> P_499 * rtb_B_38_693_0 ; _rtB -> B_38_1162_0 = 0.0 ;
_rtB -> B_38_1162_0 += _rtP -> P_502 * _rtX -> StateSpace_CSTATE_e ; _rtB ->
B_38_1163_0 = muDoubleScalarExp ( _rtB -> B_38_1160_0 ) * _rtB -> B_38_1162_0
; _rtB -> B_38_1165_0 = _rtB -> B_38_1152_0 - _rtB -> B_38_1164_0 ; _rtB ->
B_38_1167_0 = _rtB -> B_38_1165_0 / _rtB -> B_38_1166_0 ; _rtB -> B_38_1173_0
= _rtB -> B_38_1167_0 * _rtB -> B_38_1167_0 * ( real_T ) ( _rtB ->
B_38_1165_0 < 0.0 ) + rtb_B_38_238_0 ; _rtB -> B_38_1175_0 = _rtB ->
B_38_1152_0 - _rtB -> B_38_1174_0 ; _rtB -> B_38_1177_0 = _rtB -> B_38_1175_0
/ _rtB -> B_38_1176_0 ; _rtB -> B_38_1180_0 = _rtB -> B_38_1177_0 * _rtB ->
B_38_1177_0 * ( real_T ) ( _rtB -> B_38_1152_0 > 1.0 ) + _rtB -> B_38_1163_0
; _rtB -> B_38_1181_0 = _rtB -> B_38_1173_0 / _rtB -> B_38_1180_0 ; _rtB ->
B_38_1182_0 = _rtB -> B_38_1181_0 - _rtB -> B_38_1171_0 ; _rtB -> B_38_1183_0
= 1.0 / _rtB -> B_38_1172_0 * _rtB -> B_38_1182_0 ; _rtB -> B_38_1188_0 =
_rtP -> P_514 * _rtB -> B_38_1181_0 ; _rtB -> B_38_1189_0 = _rtP -> P_515 *
_rtB -> B_38_1183_0 ; _rtB -> B_38_1190_0 = _rtP -> P_516 * _rtB ->
B_38_1182_0 ; _rtB -> B_38_1191_0 = _rtB -> B_38_1190_0 + _rtB -> B_38_1187_0
; _rtB -> B_38_1192_0 = _rtB -> B_38_1183_0 + _rtB -> B_38_1185_0 ; _rtB ->
B_38_1193_0 = _rtB -> B_38_1185_0 - _rtB -> B_38_1189_0 ; _rtB -> B_38_1194_0
= _rtB -> B_38_1192_0 / _rtB -> B_38_1193_0 ; _rtB -> B_38_1197_0 = ( real_T
) ( _rtB -> B_38_1181_0 > 1.0 ) * _rtB -> B_38_1194_0 * ( real_T ) ( _rtB ->
B_38_1181_0 <= _rtB -> B_38_1186_0 ) + ( real_T ) ( _rtB -> B_38_1181_0 >
_rtB -> B_38_1186_0 ) * _rtB -> B_38_1191_0 ; _rtB -> B_38_1198_0 = _rtB ->
B_38_1184_0 + _rtB -> B_38_1188_0 ; _rtB -> B_38_1199_0 = _rtB -> B_38_1181_0
- _rtB -> B_38_1184_0 ; _rtB -> B_38_1200_0 = _rtB -> B_38_1199_0 / _rtB ->
B_38_1198_0 ; _rtB -> B_38_1202_0 = ( real_T ) ( _rtB -> B_38_1181_0 <= 1.0 )
* _rtB -> B_38_1200_0 + _rtB -> B_38_1197_0 ; _rtB -> B_38_1203_0 = _rtP ->
P_517 * _rtB -> B_38_1202_0 ; _rtB -> B_38_1204_0 = _rtB -> B_38_242_0 ; if (
ssIsMajorTimeStep ( S ) != 0 ) { _rtDW -> Switch_Mode_cu = ( _rtB ->
B_38_1204_0 > _rtP -> P_519 ) ; } if ( _rtDW -> Switch_Mode_cu ) { _rtB ->
B_38_1206_0 = _rtB -> B_38_1204_0 ; } else { _rtB -> B_38_1206_0 = _rtB ->
B_38_1205_0 ; } _rtB -> B_38_1207_0 = _rtP -> P_520 * _rtB -> B_38_1206_0 ;
_rtB -> B_38_1209_0 = _rtB -> B_38_795_0 ; if ( ssIsMajorTimeStep ( S ) != 0
) { _rtDW -> Switch_Mode_ow = ( _rtB -> B_38_1209_0 > _rtP -> P_522 ) ; } if
( _rtDW -> Switch_Mode_ow ) { _rtB -> B_38_1210_0 = _rtB -> B_38_1209_0 ; }
else { _rtB -> B_38_1210_0 = _rtB -> B_38_1208_0 ; } _rtB -> B_38_1212_0 =
_rtB -> B_38_230_0 ; _rtB -> B_38_1215_0 = _rtB -> B_38_1203_0 ; if (
ssIsMajorTimeStep ( S ) != 0 ) { _rtDW -> Switch_Mode_ct = ( _rtB ->
B_38_1212_0 > _rtP -> P_524 ) ; } _rtB -> B_38_1217_0 = _rtB -> B_38_199_0 ;
if ( ssIsMajorTimeStep ( S ) != 0 ) { _rtDW -> Switch_Mode_hx = ( _rtB ->
B_38_1215_0 > _rtP -> P_526 ) ; _rtDW -> Switch_Mode_h2 = ( _rtB ->
B_38_1217_0 > _rtP -> P_528 ) ; } if ( _rtDW -> Switch_Mode_h2 ) { _rtB ->
B_38_1219_0 = _rtB -> B_38_1217_0 ; } else { _rtB -> B_38_1219_0 = _rtB ->
B_38_1218_0 ; } _rtB -> B_38_1220_0 = _rtP -> P_529 * _rtB -> B_38_1219_0 ;
_rtB -> B_38_1222_0 = _rtB -> B_38_1220_0 - _rtB -> B_38_1221_0 ; if ( _rtB
-> B_38_1222_0 > _rtP -> P_531 ) { _rtB -> B_38_1223_0 = _rtP -> P_531 ; }
else if ( _rtB -> B_38_1222_0 < _rtP -> P_532 ) { _rtB -> B_38_1223_0 = _rtP
-> P_532 ; } else { _rtB -> B_38_1223_0 = _rtB -> B_38_1222_0 ; } _rtB ->
B_38_1225_0 = _rtB -> B_38_798_0 ; if ( ssIsMajorTimeStep ( S ) != 0 ) {
_rtDW -> Switch_Mode_dei = ( _rtB -> B_38_1225_0 > _rtP -> P_534 ) ; } if (
_rtDW -> Switch_Mode_dei ) { _rtB -> B_38_1226_0 = _rtB -> B_38_1225_0 ; }
else { _rtB -> B_38_1226_0 = _rtB -> B_38_1224_0 ; } _rtB -> B_38_1228_0 =
_rtP -> P_536 * _rtB -> B_38_199_0 ; _rtB -> B_38_1230_0 = _rtB ->
B_38_1228_0 - _rtB -> B_38_1229_0 ; _rtB -> B_38_1232_0 = _rtB -> B_38_1230_0
/ _rtB -> B_38_1231_0 ; _rtB -> B_38_1235_0 = _rtB -> B_38_1232_0 * _rtB ->
B_38_1232_0 * ( real_T ) ( _rtB -> B_38_1230_0 < 0.0 ) + rtb_B_38_207_0 ;
_rtB -> B_38_1237_0 = _rtB -> B_38_1228_0 - _rtB -> B_38_1236_0 ; _rtB ->
B_38_1239_0 = _rtB -> B_38_1237_0 / _rtB -> B_38_1238_0 ; _rtB -> B_38_1243_0
= _rtB -> B_38_1228_0 - _rtB -> B_38_1242_0 ; if ( ssIsMajorTimeStep ( S ) !=
0 ) { _rtDW -> Abs_MODE_d = ( _rtB -> B_38_1243_0 >= 0.0 ) ; } _rtB ->
B_38_1244_0 = _rtDW -> Abs_MODE_d > 0 ? _rtB -> B_38_1243_0 : - _rtB ->
B_38_1243_0 ; _rtB -> B_38_1246_0 = _rtB -> B_38_1244_0 / _rtB -> B_38_1245_0
; if ( ( _rtB -> B_38_1246_0 < 0.0 ) && ( _rtB -> B_38_1247_0 >
muDoubleScalarFloor ( _rtB -> B_38_1247_0 ) ) ) { rtb_B_38_693_0 = -
muDoubleScalarPower ( - _rtB -> B_38_1246_0 , _rtB -> B_38_1247_0 ) ; } else
{ rtb_B_38_693_0 = muDoubleScalarPower ( _rtB -> B_38_1246_0 , _rtB ->
B_38_1247_0 ) ; } _rtB -> B_38_1249_0 = _rtP -> P_544 * rtb_B_38_693_0 ; _rtB
-> B_38_1251_0 = 0.0 ; _rtB -> B_38_1251_0 += _rtP -> P_547 * _rtX ->
StateSpace_CSTATE_ai ; _rtB -> B_38_1252_0 = muDoubleScalarExp ( _rtB ->
B_38_1249_0 ) * _rtB -> B_38_1251_0 ; _rtB -> B_38_1253_0 = _rtB ->
B_38_1239_0 * _rtB -> B_38_1239_0 * ( real_T ) ( _rtB -> B_38_1228_0 > 1.0 )
+ _rtB -> B_38_1252_0 ; _rtB -> B_38_1254_0 = _rtB -> B_38_1235_0 / _rtB ->
B_38_1253_0 ; _rtB -> B_38_1256_0 = _rtB -> B_38_1254_0 - _rtB -> B_38_1255_0
; _rtB -> B_38_1257_0 = _rtP -> P_550 * _rtB -> B_38_1254_0 ; _rtB ->
B_38_1258_0 = _rtB -> B_38_1255_0 + _rtB -> B_38_1257_0 ; _rtB -> B_38_1259_0
= _rtB -> B_38_1256_0 / _rtB -> B_38_1258_0 ; _rtB -> B_38_1264_0 = _rtB ->
B_38_1254_0 - _rtB -> B_38_1262_0 ; _rtB -> B_38_1265_0 = 1.0 / _rtB ->
B_38_1263_0 * _rtB -> B_38_1264_0 ; _rtB -> B_38_1267_0 = _rtB -> B_38_1265_0
+ _rtB -> B_38_1266_0 ; _rtB -> B_38_1268_0 = _rtP -> P_554 * _rtB ->
B_38_1265_0 ; _rtB -> B_38_1269_0 = _rtB -> B_38_1266_0 - _rtB -> B_38_1268_0
; _rtB -> B_38_1270_0 = _rtB -> B_38_1267_0 / _rtB -> B_38_1269_0 ; _rtB ->
B_38_1273_0 = _rtP -> P_556 * _rtB -> B_38_1264_0 ; _rtB -> B_38_1275_0 =
_rtB -> B_38_1273_0 + _rtB -> B_38_1274_0 ; _rtB -> B_38_1277_0 = ( real_T )
( _rtB -> B_38_1254_0 > 1.0 ) * _rtB -> B_38_1270_0 * ( real_T ) ( _rtB ->
B_38_1254_0 <= _rtB -> B_38_1271_0 ) + ( real_T ) ( _rtB -> B_38_1254_0 >
_rtB -> B_38_1271_0 ) * _rtB -> B_38_1275_0 ; _rtB -> B_38_1278_0 = ( real_T
) ( _rtB -> B_38_1254_0 <= 1.0 ) * _rtB -> B_38_1259_0 + _rtB -> B_38_1277_0
; _rtB -> B_38_1279_0 = _rtP -> P_558 * _rtB -> B_38_1278_0 ; _rtB ->
B_38_1280_0 = _rtB -> B_38_1279_0 ; if ( ssIsMajorTimeStep ( S ) != 0 ) {
_rtDW -> Switch_Mode_i = ( _rtB -> B_38_1280_0 > _rtP -> P_559 ) ; }
ssCallAccelRunBlock ( S , 38 , 1282 , SS_CALL_MDL_OUTPUTS ) ; _rtB ->
B_38_1283_0 = _rtB -> B_38_319_0 ; if ( ssIsMajorTimeStep ( S ) != 0 ) {
_rtDW -> Switch_Mode_e0 = ( _rtB -> B_38_1283_0 > _rtP -> P_561 ) ; } if (
_rtDW -> Switch_Mode_e0 ) { _rtB -> B_38_1285_0 = _rtB -> B_38_1283_0 ; }
else { _rtB -> B_38_1285_0 = _rtB -> B_38_1284_0 ; } _rtB -> B_38_1286_0 =
_rtP -> P_562 * _rtB -> B_38_1285_0 ; _rtB -> B_38_1288_0 = _rtB ->
B_38_778_0 ; if ( ssIsMajorTimeStep ( S ) != 0 ) { _rtDW -> Switch_Mode_l1 =
( _rtB -> B_38_1288_0 > _rtP -> P_564 ) ; } if ( _rtDW -> Switch_Mode_l1 ) {
_rtB -> B_38_1289_0 = _rtB -> B_38_1288_0 ; } else { _rtB -> B_38_1289_0 =
_rtB -> B_38_1287_0 ; } _rtB -> B_38_1291_0 = _rtB -> B_38_307_0 ; _rtB ->
B_38_1294_0 = _rtP -> P_568 * _rtB -> B_38_307_0 ; _rtB -> B_38_1296_0 = _rtB
-> B_38_1294_0 - _rtB -> B_38_1295_0 ; _rtB -> B_38_1298_0 = _rtB ->
B_38_1296_0 / _rtB -> B_38_1297_0 ; _rtB -> B_38_1301_0 = _rtB -> B_38_1298_0
* _rtB -> B_38_1298_0 * ( real_T ) ( _rtB -> B_38_1296_0 < 0.0 ) +
rtb_B_38_315_0 ; _rtB -> B_38_1303_0 = _rtB -> B_38_1294_0 - _rtB ->
B_38_1302_0 ; _rtB -> B_38_1305_0 = _rtB -> B_38_1303_0 / _rtB -> B_38_1304_0
; _rtB -> B_38_1309_0 = _rtB -> B_38_1294_0 - _rtB -> B_38_1308_0 ; if (
ssIsMajorTimeStep ( S ) != 0 ) { _rtDW -> Switch_Mode_nw = ( _rtB ->
B_38_1291_0 > _rtP -> P_566 ) ; _rtDW -> Abs_MODE_a5 = ( _rtB -> B_38_1309_0
>= 0.0 ) ; } _rtB -> B_38_1310_0 = _rtDW -> Abs_MODE_a5 > 0 ? _rtB ->
B_38_1309_0 : - _rtB -> B_38_1309_0 ; _rtB -> B_38_1312_0 = _rtB ->
B_38_1310_0 / _rtB -> B_38_1311_0 ; if ( ( _rtB -> B_38_1312_0 < 0.0 ) && (
_rtB -> B_38_1313_0 > muDoubleScalarFloor ( _rtB -> B_38_1313_0 ) ) ) {
rtb_B_38_693_0 = - muDoubleScalarPower ( - _rtB -> B_38_1312_0 , _rtB ->
B_38_1313_0 ) ; } else { rtb_B_38_693_0 = muDoubleScalarPower ( _rtB ->
B_38_1312_0 , _rtB -> B_38_1313_0 ) ; } _rtB -> B_38_1315_0 = _rtP -> P_576 *
rtb_B_38_693_0 ; _rtB -> B_38_1317_0 = 0.0 ; _rtB -> B_38_1317_0 += _rtP ->
P_579 * _rtX -> StateSpace_CSTATE_h ; _rtB -> B_38_1318_0 = muDoubleScalarExp
( _rtB -> B_38_1315_0 ) * _rtB -> B_38_1317_0 ; _rtB -> B_38_1319_0 = _rtB ->
B_38_1305_0 * _rtB -> B_38_1305_0 * ( real_T ) ( _rtB -> B_38_1294_0 > 1.0 )
+ _rtB -> B_38_1318_0 ; _rtB -> B_38_1320_0 = _rtB -> B_38_1301_0 / _rtB ->
B_38_1319_0 ; _rtB -> B_38_1322_0 = _rtB -> B_38_1320_0 - _rtB -> B_38_1321_0
; _rtB -> B_38_1323_0 = _rtP -> P_582 * _rtB -> B_38_1320_0 ; _rtB ->
B_38_1324_0 = _rtB -> B_38_1321_0 + _rtB -> B_38_1323_0 ; _rtB -> B_38_1325_0
= _rtB -> B_38_1322_0 / _rtB -> B_38_1324_0 ; _rtB -> B_38_1330_0 = _rtB ->
B_38_1320_0 - _rtB -> B_38_1328_0 ; _rtB -> B_38_1331_0 = 1.0 / _rtB ->
B_38_1329_0 * _rtB -> B_38_1330_0 ; _rtB -> B_38_1333_0 = _rtB -> B_38_1331_0
+ _rtB -> B_38_1332_0 ; _rtB -> B_38_1334_0 = _rtP -> P_586 * _rtB ->
B_38_1331_0 ; _rtB -> B_38_1335_0 = _rtB -> B_38_1332_0 - _rtB -> B_38_1334_0
; _rtB -> B_38_1336_0 = _rtB -> B_38_1333_0 / _rtB -> B_38_1335_0 ; _rtB ->
B_38_1339_0 = _rtP -> P_588 * _rtB -> B_38_1330_0 ; _rtB -> B_38_1341_0 =
_rtB -> B_38_1339_0 + _rtB -> B_38_1340_0 ; _rtB -> B_38_1343_0 = ( real_T )
( _rtB -> B_38_1320_0 > 1.0 ) * _rtB -> B_38_1336_0 * ( real_T ) ( _rtB ->
B_38_1320_0 <= _rtB -> B_38_1337_0 ) + ( real_T ) ( _rtB -> B_38_1320_0 >
_rtB -> B_38_1337_0 ) * _rtB -> B_38_1341_0 ; _rtB -> B_38_1344_0 = ( real_T
) ( _rtB -> B_38_1320_0 <= 1.0 ) * _rtB -> B_38_1325_0 + _rtB -> B_38_1343_0
; _rtB -> B_38_1345_0 = _rtP -> P_590 * _rtB -> B_38_1344_0 ; _rtB ->
B_38_1346_0 = _rtB -> B_38_1345_0 ; if ( ssIsMajorTimeStep ( S ) != 0 ) {
_rtDW -> Switch_Mode_ed = ( _rtB -> B_38_1346_0 > _rtP -> P_591 ) ; } _rtB ->
B_38_1351_0 [ 0 ] = _rtB -> B_38_490_0 ; _rtB -> B_38_1351_0 [ 1 ] = _rtB ->
B_38_575_0 ; _rtB -> B_38_1351_0 [ 2 ] = _rtB -> B_38_655_0 ;
ssCallAccelRunBlock ( S , 38 , 1352 , SS_CALL_MDL_OUTPUTS ) ; _rtB ->
B_38_1356_0 [ 0 ] = _rtB -> B_38_104_0 ; _rtB -> B_38_1356_0 [ 1 ] = _rtB ->
B_38_102_0 ; _rtB -> B_38_1356_0 [ 2 ] = _rtB -> B_38_115_0 ; _rtB ->
B_38_1356_0 [ 3 ] = _rtB -> B_38_113_0 ; _rtB -> B_38_1356_0 [ 4 ] = _rtB ->
B_38_99_0 ; _rtB -> B_38_1356_0 [ 5 ] = _rtB -> B_38_97_0 ;
ssCallAccelRunBlock ( S , 38 , 1357 , SS_CALL_MDL_OUTPUTS ) ; _rtB ->
B_38_1367_0 [ 0 ] = _rtB -> B_38_425_0 ; _rtB -> B_38_1367_0 [ 1 ] = _rtB ->
B_38_451_0 ; _rtB -> B_38_1367_0 [ 2 ] = _rtB -> B_38_486_0 ; _rtB ->
B_38_1367_0 [ 3 ] = _rtB -> B_38_499_0 ; _rtB -> B_38_1367_0 [ 4 ] = _rtB ->
B_38_530_0 ; _rtB -> B_38_1367_0 [ 5 ] = _rtB -> B_38_570_0 ; _rtB ->
B_38_1367_0 [ 6 ] = _rtB -> B_38_589_0 ; _rtB -> B_38_1367_0 [ 7 ] = _rtB ->
B_38_620_0 ; _rtB -> B_38_1367_0 [ 8 ] = _rtB -> B_38_650_0 ;
ssCallAccelRunBlock ( S , 38 , 1368 , SS_CALL_MDL_OUTPUTS ) ; _rtB ->
B_38_1374_0 = ( ( ! ( _rtB -> B_38_1369_0 <= _rtB -> B_38_99_0 ) ) || ( ! (
_rtB -> B_38_99_0 <= _rtB -> B_38_1371_0 ) ) ) ; isHit = ssIsSampleHit ( S ,
1 , 0 ) ; if ( ( isHit != 0 ) && _rtB -> B_38_1374_0 ) { ssSetStopRequested (
S , 1 ) ; } ssCallAccelRunBlock ( S , 38 , 1376 , SS_CALL_MDL_OUTPUTS ) ;
_rtB -> B_38_1377_0 = _rtP -> P_594 * _rtB -> B_38_556_0 ; _rtB ->
B_38_1379_0 = _rtB -> B_38_1377_0 - _rtB -> B_38_1378_0 ; if (
ssIsMajorTimeStep ( S ) != 0 ) { _rtDW -> Abs_MODE_m = ( _rtB -> B_38_1379_0
>= 0.0 ) ; } _rtB -> B_38_1380_0 = _rtDW -> Abs_MODE_m > 0 ? _rtB ->
B_38_1379_0 : - _rtB -> B_38_1379_0 ; _rtB -> B_38_1382_0 = _rtB ->
B_38_1380_0 / _rtB -> B_38_1381_0 ; if ( ( _rtB -> B_38_1382_0 < 0.0 ) && (
_rtB -> B_38_1383_0 > muDoubleScalarFloor ( _rtB -> B_38_1383_0 ) ) ) {
rtb_B_38_693_0 = - muDoubleScalarPower ( - _rtB -> B_38_1382_0 , _rtB ->
B_38_1383_0 ) ; } else { rtb_B_38_693_0 = muDoubleScalarPower ( _rtB ->
B_38_1382_0 , _rtB -> B_38_1383_0 ) ; } _rtB -> B_38_1385_0 = _rtP -> P_598 *
rtb_B_38_693_0 ; _rtB -> B_38_1387_0 = 0.0 ; _rtB -> B_38_1387_0 += _rtP ->
P_601 * _rtX -> StateSpace_CSTATE_i ; _rtB -> B_38_1388_0 = muDoubleScalarExp
( _rtB -> B_38_1385_0 ) * _rtB -> B_38_1387_0 ; _rtB -> B_38_1390_0 = _rtB ->
B_38_1377_0 - _rtB -> B_38_1389_0 ; _rtB -> B_38_1392_0 = _rtB -> B_38_1390_0
/ _rtB -> B_38_1391_0 ; _rtB -> B_38_1398_0 = _rtB -> B_38_1392_0 * _rtB ->
B_38_1392_0 * ( real_T ) ( _rtB -> B_38_1390_0 < 0.0 ) + rtb_B_38_564_0 ;
_rtB -> B_38_1400_0 = _rtB -> B_38_1377_0 - _rtB -> B_38_1399_0 ; _rtB ->
B_38_1402_0 = _rtB -> B_38_1400_0 / _rtB -> B_38_1401_0 ; _rtB -> B_38_1405_0
= _rtB -> B_38_1402_0 * _rtB -> B_38_1402_0 * ( real_T ) ( _rtB ->
B_38_1377_0 > 1.0 ) + _rtB -> B_38_1388_0 ; _rtB -> B_38_1406_0 = _rtB ->
B_38_1398_0 / _rtB -> B_38_1405_0 ; _rtB -> B_38_1407_0 = _rtB -> B_38_1406_0
- _rtB -> B_38_1396_0 ; _rtB -> B_38_1408_0 = 1.0 / _rtB -> B_38_1397_0 *
_rtB -> B_38_1407_0 ; _rtB -> B_38_1413_0 = _rtP -> P_613 * _rtB ->
B_38_1406_0 ; _rtB -> B_38_1414_0 = _rtP -> P_614 * _rtB -> B_38_1408_0 ;
_rtB -> B_38_1415_0 = _rtP -> P_615 * _rtB -> B_38_1407_0 ; _rtB ->
B_38_1416_0 = _rtB -> B_38_1415_0 + _rtB -> B_38_1412_0 ; _rtB -> B_38_1417_0
= _rtB -> B_38_1408_0 + _rtB -> B_38_1410_0 ; _rtB -> B_38_1418_0 = _rtB ->
B_38_1410_0 - _rtB -> B_38_1414_0 ; _rtB -> B_38_1419_0 = _rtB -> B_38_1417_0
/ _rtB -> B_38_1418_0 ; _rtB -> B_38_1422_0 = ( real_T ) ( _rtB ->
B_38_1406_0 > 1.0 ) * _rtB -> B_38_1419_0 * ( real_T ) ( _rtB -> B_38_1406_0
<= _rtB -> B_38_1411_0 ) + ( real_T ) ( _rtB -> B_38_1406_0 > _rtB ->
B_38_1411_0 ) * _rtB -> B_38_1416_0 ; _rtB -> B_38_1423_0 = _rtB ->
B_38_1409_0 + _rtB -> B_38_1413_0 ; _rtB -> B_38_1424_0 = _rtB -> B_38_1406_0
- _rtB -> B_38_1409_0 ; _rtB -> B_38_1425_0 = _rtB -> B_38_1424_0 / _rtB ->
B_38_1423_0 ; _rtB -> B_38_1427_0 = ( real_T ) ( _rtB -> B_38_1406_0 <= 1.0 )
* _rtB -> B_38_1425_0 + _rtB -> B_38_1422_0 ; _rtB -> B_38_1428_0 = _rtP ->
P_616 * _rtB -> B_38_1427_0 ; _rtB -> B_38_1429_0 = _rtB -> B_38_568_0 ; if (
ssIsMajorTimeStep ( S ) != 0 ) { _rtDW -> Switch_Mode_pd = ( _rtB ->
B_38_1429_0 > _rtP -> P_618 ) ; } if ( _rtDW -> Switch_Mode_pd ) { _rtB ->
B_38_1431_0 = _rtB -> B_38_1429_0 ; } else { _rtB -> B_38_1431_0 = _rtB ->
B_38_1430_0 ; } _rtB -> B_38_1432_0 = _rtP -> P_619 * _rtB -> B_38_1431_0 ;
_rtB -> B_38_1434_0 = _rtB -> B_38_733_0 ; if ( ssIsMajorTimeStep ( S ) != 0
) { _rtDW -> Switch_Mode_nd = ( _rtB -> B_38_1434_0 > _rtP -> P_621 ) ; } if
( _rtDW -> Switch_Mode_nd ) { _rtB -> B_38_1435_0 = _rtB -> B_38_1434_0 ; }
else { _rtB -> B_38_1435_0 = _rtB -> B_38_1433_0 ; } _rtB -> B_38_1437_0 =
_rtB -> B_38_556_0 ; _rtB -> B_38_1440_0 = _rtB -> B_38_1428_0 ; if (
ssIsMajorTimeStep ( S ) != 0 ) { _rtDW -> Switch_Mode_h2v = ( _rtB ->
B_38_1437_0 > _rtP -> P_623 ) ; _rtDW -> Switch_Mode_ah = ( _rtB ->
B_38_1440_0 > _rtP -> P_625 ) ; } ssCallAccelRunBlock ( S , 38 , 1442 ,
SS_CALL_MDL_OUTPUTS ) ; _rtB -> B_38_1443_0 = _rtP -> P_626 * _rtB ->
B_38_411_0 ; _rtB -> B_38_1445_0 = _rtB -> B_38_1443_0 - _rtB -> B_38_1444_0
; if ( ssIsMajorTimeStep ( S ) != 0 ) { _rtDW -> Abs_MODE_h = ( _rtB ->
B_38_1445_0 >= 0.0 ) ; } _rtB -> B_38_1446_0 = _rtDW -> Abs_MODE_h > 0 ? _rtB
-> B_38_1445_0 : - _rtB -> B_38_1445_0 ; _rtB -> B_38_1448_0 = _rtB ->
B_38_1446_0 / _rtB -> B_38_1447_0 ; if ( ( _rtB -> B_38_1448_0 < 0.0 ) && (
_rtB -> B_38_1449_0 > muDoubleScalarFloor ( _rtB -> B_38_1449_0 ) ) ) {
rtb_B_38_693_0 = - muDoubleScalarPower ( - _rtB -> B_38_1448_0 , _rtB ->
B_38_1449_0 ) ; } else { rtb_B_38_693_0 = muDoubleScalarPower ( _rtB ->
B_38_1448_0 , _rtB -> B_38_1449_0 ) ; } _rtB -> B_38_1451_0 = _rtP -> P_630 *
rtb_B_38_693_0 ; _rtB -> B_38_1453_0 = 0.0 ; _rtB -> B_38_1453_0 += _rtP ->
P_633 * _rtX -> StateSpace_CSTATE_ml ; _rtB -> B_38_1454_0 =
muDoubleScalarExp ( _rtB -> B_38_1451_0 ) * _rtB -> B_38_1453_0 ; _rtB ->
B_38_1456_0 = _rtB -> B_38_1443_0 - _rtB -> B_38_1455_0 ; _rtB -> B_38_1458_0
= _rtB -> B_38_1456_0 / _rtB -> B_38_1457_0 ; _rtB -> B_38_1464_0 = _rtB ->
B_38_1458_0 * _rtB -> B_38_1458_0 * ( real_T ) ( _rtB -> B_38_1456_0 < 0.0 )
+ rtb_B_38_419_0 ; _rtB -> B_38_1466_0 = _rtB -> B_38_1443_0 - _rtB ->
B_38_1465_0 ; _rtB -> B_38_1468_0 = _rtB -> B_38_1466_0 / _rtB -> B_38_1467_0
; _rtB -> B_38_1471_0 = _rtB -> B_38_1468_0 * _rtB -> B_38_1468_0 * ( real_T
) ( _rtB -> B_38_1443_0 > 1.0 ) + _rtB -> B_38_1454_0 ; _rtB -> B_38_1472_0 =
_rtB -> B_38_1464_0 / _rtB -> B_38_1471_0 ; _rtB -> B_38_1473_0 = _rtB ->
B_38_1472_0 - _rtB -> B_38_1462_0 ; _rtB -> B_38_1474_0 = 1.0 / _rtB ->
B_38_1463_0 * _rtB -> B_38_1473_0 ; _rtB -> B_38_1479_0 = _rtP -> P_645 *
_rtB -> B_38_1472_0 ; _rtB -> B_38_1480_0 = _rtP -> P_646 * _rtB ->
B_38_1474_0 ; _rtB -> B_38_1481_0 = _rtP -> P_647 * _rtB -> B_38_1473_0 ;
_rtB -> B_38_1482_0 = _rtB -> B_38_1481_0 + _rtB -> B_38_1478_0 ; _rtB ->
B_38_1483_0 = _rtB -> B_38_1474_0 + _rtB -> B_38_1476_0 ; _rtB -> B_38_1484_0
= _rtB -> B_38_1476_0 - _rtB -> B_38_1480_0 ; _rtB -> B_38_1485_0 = _rtB ->
B_38_1483_0 / _rtB -> B_38_1484_0 ; _rtB -> B_38_1488_0 = ( real_T ) ( _rtB
-> B_38_1472_0 > 1.0 ) * _rtB -> B_38_1485_0 * ( real_T ) ( _rtB ->
B_38_1472_0 <= _rtB -> B_38_1477_0 ) + ( real_T ) ( _rtB -> B_38_1472_0 >
_rtB -> B_38_1477_0 ) * _rtB -> B_38_1482_0 ; _rtB -> B_38_1489_0 = _rtB ->
B_38_1475_0 + _rtB -> B_38_1479_0 ; _rtB -> B_38_1490_0 = _rtB -> B_38_1472_0
- _rtB -> B_38_1475_0 ; _rtB -> B_38_1491_0 = _rtB -> B_38_1490_0 / _rtB ->
B_38_1489_0 ; _rtB -> B_38_1493_0 = ( real_T ) ( _rtB -> B_38_1472_0 <= 1.0 )
* _rtB -> B_38_1491_0 + _rtB -> B_38_1488_0 ; _rtB -> B_38_1494_0 = _rtP ->
P_648 * _rtB -> B_38_1493_0 ; _rtB -> B_38_1495_0 = _rtB -> B_38_423_0 ; if (
ssIsMajorTimeStep ( S ) != 0 ) { _rtDW -> Switch_Mode_d4 = ( _rtB ->
B_38_1495_0 > _rtP -> P_650 ) ; } if ( _rtDW -> Switch_Mode_d4 ) { _rtB ->
B_38_1497_0 = _rtB -> B_38_1495_0 ; } else { _rtB -> B_38_1497_0 = _rtB ->
B_38_1496_0 ; } _rtB -> B_38_1498_0 = _rtP -> P_651 * _rtB -> B_38_1497_0 ;
_rtB -> B_38_1500_0 = _rtB -> B_38_705_0 ; if ( ssIsMajorTimeStep ( S ) != 0
) { _rtDW -> Switch_Mode_pw = ( _rtB -> B_38_1500_0 > _rtP -> P_653 ) ; } if
( _rtDW -> Switch_Mode_pw ) { _rtB -> B_38_1501_0 = _rtB -> B_38_1500_0 ; }
else { _rtB -> B_38_1501_0 = _rtB -> B_38_1499_0 ; } _rtB -> B_38_1503_0 =
_rtB -> B_38_411_0 ; _rtB -> B_38_1506_0 = _rtB -> B_38_1494_0 ; if (
ssIsMajorTimeStep ( S ) != 0 ) { _rtDW -> Switch_Mode_bf = ( _rtB ->
B_38_1503_0 > _rtP -> P_655 ) ; _rtDW -> Switch_Mode_j = ( _rtB ->
B_38_1506_0 > _rtP -> P_657 ) ; } ssCallAccelRunBlock ( S , 38 , 1508 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 38 , 1509 ,
SS_CALL_MDL_OUTPUTS ) ; isHit = ssIsSampleHit ( S , 3 , 0 ) ; if ( isHit != 0
) { ssCallAccelRunBlock ( S , 37 , 0 , SS_CALL_MDL_OUTPUTS ) ; } _rtB ->
B_38_1511_0 = _rtB -> B_37_0_1 [ 0 ] + _rtB -> B_38_472_0 ; if (
ssIsMajorTimeStep ( S ) != 0 ) { _rtDW -> Switch_Mode_pc = ( _rtB ->
B_38_1511_0 > _rtP -> P_659 ) ; } if ( _rtDW -> Switch_Mode_pc ) { _rtB ->
B_38_1513_0 = _rtB -> B_38_1511_0 ; } else { _rtB -> B_38_1513_0 = _rtB ->
B_38_1512_0 ; } _rtB -> B_38_1514_0 = _rtP -> P_660 * _rtB -> B_38_1513_0 ;
_rtB -> B_38_1515_0 = _rtP -> P_661 * _rtB -> B_38_472_0 ; _rtB ->
B_38_1517_0 = _rtB -> B_38_1515_0 - _rtB -> B_38_1516_0 ; if (
ssIsMajorTimeStep ( S ) != 0 ) { _rtDW -> Abs_MODE_he = ( _rtB -> B_38_1517_0
>= 0.0 ) ; } _rtB -> B_38_1518_0 = _rtDW -> Abs_MODE_he > 0 ? _rtB ->
B_38_1517_0 : - _rtB -> B_38_1517_0 ; _rtB -> B_38_1520_0 = _rtB ->
B_38_1518_0 / _rtB -> B_38_1519_0 ; if ( ( _rtB -> B_38_1520_0 < 0.0 ) && (
_rtB -> B_38_1521_0 > muDoubleScalarFloor ( _rtB -> B_38_1521_0 ) ) ) {
rtb_B_38_693_0 = - muDoubleScalarPower ( - _rtB -> B_38_1520_0 , _rtB ->
B_38_1521_0 ) ; } else { rtb_B_38_693_0 = muDoubleScalarPower ( _rtB ->
B_38_1520_0 , _rtB -> B_38_1521_0 ) ; } _rtB -> B_38_1523_0 = _rtP -> P_665 *
rtb_B_38_693_0 ; _rtB -> B_38_1525_0 = 0.0 ; _rtB -> B_38_1525_0 += _rtP ->
P_668 * _rtX -> StateSpace_CSTATE_p ; _rtB -> B_38_1526_0 = muDoubleScalarExp
( _rtB -> B_38_1523_0 ) * _rtB -> B_38_1525_0 ; _rtB -> B_38_1528_0 = _rtB ->
B_38_1515_0 - _rtB -> B_38_1527_0 ; _rtB -> B_38_1530_0 = _rtB -> B_38_1528_0
/ _rtB -> B_38_1529_0 ; _rtB -> B_38_1536_0 = _rtB -> B_38_1530_0 * _rtB ->
B_38_1530_0 * ( real_T ) ( _rtB -> B_38_1528_0 < 0.0 ) + rtb_B_38_480_0 ;
_rtB -> B_38_1538_0 = _rtB -> B_38_1515_0 - _rtB -> B_38_1537_0 ; _rtB ->
B_38_1540_0 = _rtB -> B_38_1538_0 / _rtB -> B_38_1539_0 ; _rtB -> B_38_1543_0
= _rtB -> B_38_1540_0 * _rtB -> B_38_1540_0 * ( real_T ) ( _rtB ->
B_38_1515_0 > 1.0 ) + _rtB -> B_38_1526_0 ; _rtB -> B_38_1544_0 = _rtB ->
B_38_1536_0 / _rtB -> B_38_1543_0 ; _rtB -> B_38_1545_0 = _rtB -> B_38_1544_0
- _rtB -> B_38_1534_0 ; _rtB -> B_38_1546_0 = 1.0 / _rtB -> B_38_1535_0 *
_rtB -> B_38_1545_0 ; _rtB -> B_38_1551_0 = _rtP -> P_680 * _rtB ->
B_38_1544_0 ; _rtB -> B_38_1552_0 = _rtP -> P_681 * _rtB -> B_38_1546_0 ;
_rtB -> B_38_1553_0 = _rtP -> P_682 * _rtB -> B_38_1545_0 ; _rtB ->
B_38_1554_0 = _rtB -> B_38_1553_0 + _rtB -> B_38_1550_0 ; _rtB -> B_38_1555_0
= _rtB -> B_38_1546_0 + _rtB -> B_38_1548_0 ; _rtB -> B_38_1556_0 = _rtB ->
B_38_1548_0 - _rtB -> B_38_1552_0 ; _rtB -> B_38_1557_0 = _rtB -> B_38_1555_0
/ _rtB -> B_38_1556_0 ; _rtB -> B_38_1560_0 = ( real_T ) ( _rtB ->
B_38_1544_0 > 1.0 ) * _rtB -> B_38_1557_0 * ( real_T ) ( _rtB -> B_38_1544_0
<= _rtB -> B_38_1549_0 ) + ( real_T ) ( _rtB -> B_38_1544_0 > _rtB ->
B_38_1549_0 ) * _rtB -> B_38_1554_0 ; _rtB -> B_38_1561_0 = _rtB ->
B_38_1547_0 + _rtB -> B_38_1551_0 ; _rtB -> B_38_1562_0 = _rtB -> B_38_1544_0
- _rtB -> B_38_1547_0 ; _rtB -> B_38_1563_0 = _rtB -> B_38_1562_0 / _rtB ->
B_38_1561_0 ; _rtB -> B_38_1565_0 = ( real_T ) ( _rtB -> B_38_1544_0 <= 1.0 )
* _rtB -> B_38_1563_0 + _rtB -> B_38_1560_0 ; _rtB -> B_38_1566_0 = _rtP ->
P_683 * _rtB -> B_38_1565_0 ; _rtB -> B_38_1567_0 = _rtB -> B_38_484_0 ; if (
ssIsMajorTimeStep ( S ) != 0 ) { _rtDW -> Switch_Mode_b1 = ( _rtB ->
B_38_1567_0 > _rtP -> P_685 ) ; } if ( _rtDW -> Switch_Mode_b1 ) { _rtB ->
B_38_1569_0 = _rtB -> B_38_1567_0 ; } else { _rtB -> B_38_1569_0 = _rtB ->
B_38_1568_0 ; } _rtB -> B_38_1570_0 = _rtP -> P_686 * _rtB -> B_38_1569_0 ;
_rtB -> B_38_1572_0 = _rtB -> B_38_1514_0 - _rtB -> B_38_1571_0 ; if ( _rtB
-> B_38_1572_0 > _rtP -> P_688 ) { _rtB -> B_38_1573_0 = _rtP -> P_688 ; }
else if ( _rtB -> B_38_1572_0 < _rtP -> P_689 ) { _rtB -> B_38_1573_0 = _rtP
-> P_689 ; } else { _rtB -> B_38_1573_0 = _rtB -> B_38_1572_0 ; } _rtB ->
B_38_1575_0 = _rtB -> B_38_717_0 ; if ( ssIsMajorTimeStep ( S ) != 0 ) {
_rtDW -> Switch_Mode_od = ( _rtB -> B_38_1575_0 > _rtP -> P_691 ) ; } if (
_rtDW -> Switch_Mode_od ) { _rtB -> B_38_1576_0 = _rtB -> B_38_1575_0 ; }
else { _rtB -> B_38_1576_0 = _rtB -> B_38_1574_0 ; } _rtB -> B_38_1578_0 =
_rtB -> B_38_1566_0 ; _rtB -> B_38_1580_0 = _rtP -> P_694 * _rtB ->
B_38_436_0 ; _rtB -> B_38_1582_0 = _rtB -> B_38_1580_0 - _rtB -> B_38_1581_0
; if ( ssIsMajorTimeStep ( S ) != 0 ) { _rtDW -> Switch_Mode_bv = ( _rtB ->
B_38_1578_0 > _rtP -> P_693 ) ; _rtDW -> Abs_MODE_hi = ( _rtB -> B_38_1582_0
>= 0.0 ) ; } _rtB -> B_38_1583_0 = _rtDW -> Abs_MODE_hi > 0 ? _rtB ->
B_38_1582_0 : - _rtB -> B_38_1582_0 ; _rtB -> B_38_1585_0 = _rtB ->
B_38_1583_0 / _rtB -> B_38_1584_0 ; if ( ( _rtB -> B_38_1585_0 < 0.0 ) && (
_rtB -> B_38_1586_0 > muDoubleScalarFloor ( _rtB -> B_38_1586_0 ) ) ) {
rtb_B_38_693_0 = - muDoubleScalarPower ( - _rtB -> B_38_1585_0 , _rtB ->
B_38_1586_0 ) ; } else { rtb_B_38_693_0 = muDoubleScalarPower ( _rtB ->
B_38_1585_0 , _rtB -> B_38_1586_0 ) ; } _rtB -> B_38_1588_0 = _rtP -> P_698 *
rtb_B_38_693_0 ; _rtB -> B_38_1590_0 = 0.0 ; _rtB -> B_38_1590_0 += _rtP ->
P_701 * _rtX -> StateSpace_CSTATE_po ; _rtB -> B_38_1591_0 =
muDoubleScalarExp ( _rtB -> B_38_1588_0 ) * _rtB -> B_38_1590_0 ; _rtB ->
B_38_1593_0 = _rtB -> B_38_1580_0 - _rtB -> B_38_1592_0 ; _rtB -> B_38_1595_0
= _rtB -> B_38_1593_0 / _rtB -> B_38_1594_0 ; _rtB -> B_38_1601_0 = _rtB ->
B_38_1595_0 * _rtB -> B_38_1595_0 * ( real_T ) ( _rtB -> B_38_1593_0 < 0.0 )
+ rtb_B_38_444_0 ; _rtB -> B_38_1603_0 = _rtB -> B_38_1580_0 - _rtB ->
B_38_1602_0 ; _rtB -> B_38_1605_0 = _rtB -> B_38_1603_0 / _rtB -> B_38_1604_0
; _rtB -> B_38_1608_0 = _rtB -> B_38_1605_0 * _rtB -> B_38_1605_0 * ( real_T
) ( _rtB -> B_38_1580_0 > 1.0 ) + _rtB -> B_38_1591_0 ; _rtB -> B_38_1609_0 =
_rtB -> B_38_1601_0 / _rtB -> B_38_1608_0 ; _rtB -> B_38_1610_0 = _rtB ->
B_38_1609_0 - _rtB -> B_38_1599_0 ; _rtB -> B_38_1611_0 = 1.0 / _rtB ->
B_38_1600_0 * _rtB -> B_38_1610_0 ; _rtB -> B_38_1616_0 = _rtP -> P_713 *
_rtB -> B_38_1609_0 ; _rtB -> B_38_1617_0 = _rtP -> P_714 * _rtB ->
B_38_1611_0 ; _rtB -> B_38_1618_0 = _rtP -> P_715 * _rtB -> B_38_1610_0 ;
_rtB -> B_38_1619_0 = _rtB -> B_38_1618_0 + _rtB -> B_38_1615_0 ; _rtB ->
B_38_1620_0 = _rtB -> B_38_1611_0 + _rtB -> B_38_1613_0 ; _rtB -> B_38_1621_0
= _rtB -> B_38_1613_0 - _rtB -> B_38_1617_0 ; _rtB -> B_38_1622_0 = _rtB ->
B_38_1620_0 / _rtB -> B_38_1621_0 ; _rtB -> B_38_1625_0 = ( real_T ) ( _rtB
-> B_38_1609_0 > 1.0 ) * _rtB -> B_38_1622_0 * ( real_T ) ( _rtB ->
B_38_1609_0 <= _rtB -> B_38_1614_0 ) + ( real_T ) ( _rtB -> B_38_1609_0 >
_rtB -> B_38_1614_0 ) * _rtB -> B_38_1619_0 ; _rtB -> B_38_1626_0 = _rtB ->
B_38_1612_0 + _rtB -> B_38_1616_0 ; _rtB -> B_38_1627_0 = _rtB -> B_38_1609_0
- _rtB -> B_38_1612_0 ; _rtB -> B_38_1628_0 = _rtB -> B_38_1627_0 / _rtB ->
B_38_1626_0 ; _rtB -> B_38_1630_0 = ( real_T ) ( _rtB -> B_38_1609_0 <= 1.0 )
* _rtB -> B_38_1628_0 + _rtB -> B_38_1625_0 ; _rtB -> B_38_1631_0 = _rtP ->
P_716 * _rtB -> B_38_1630_0 ; isHit = ssIsSampleHit ( S , 3 , 0 ) ; if (
isHit != 0 ) { ssCallAccelRunBlock ( S , 35 , 0 , SS_CALL_MDL_OUTPUTS ) ;
_rtB -> B_38_1633_0 = _rtP -> P_717 * _rtB -> B_35_0_1 [ 0 ] ; } _rtB ->
B_38_1634_0 = _rtB -> B_38_1633_0 + _rtB -> B_38_436_0 ; if (
ssIsMajorTimeStep ( S ) != 0 ) { _rtDW -> Switch_Mode_det = ( _rtB ->
B_38_1634_0 > _rtP -> P_719 ) ; } if ( _rtDW -> Switch_Mode_det ) { _rtB ->
B_38_1636_0 = _rtB -> B_38_1634_0 ; } else { _rtB -> B_38_1636_0 = _rtB ->
B_38_1635_0 ; } _rtB -> B_38_1637_0 = _rtP -> P_720 * _rtB -> B_38_1636_0 ;
_rtB -> B_38_1639_0 = _rtB -> B_38_1637_0 - _rtB -> B_38_1638_0 ; if ( _rtB
-> B_38_1639_0 > _rtP -> P_722 ) { _rtB -> B_38_1640_0 = _rtP -> P_722 ; }
else if ( _rtB -> B_38_1639_0 < _rtP -> P_723 ) { _rtB -> B_38_1640_0 = _rtP
-> P_723 ; } else { _rtB -> B_38_1640_0 = _rtB -> B_38_1639_0 ; } _rtB ->
B_38_1642_0 = _rtB -> B_38_715_0 ; if ( ssIsMajorTimeStep ( S ) != 0 ) {
_rtDW -> Switch_Mode_pn = ( _rtB -> B_38_1642_0 > _rtP -> P_725 ) ; } if (
_rtDW -> Switch_Mode_pn ) { _rtB -> B_38_1643_0 = _rtB -> B_38_1642_0 ; }
else { _rtB -> B_38_1643_0 = _rtB -> B_38_1641_0 ; } _rtB -> B_38_1645_0 =
_rtB -> B_38_1631_0 ; if ( ssIsMajorTimeStep ( S ) != 0 ) { _rtDW ->
Switch_Mode_ck = ( _rtB -> B_38_1645_0 > _rtP -> P_727 ) ; } _rtB ->
B_38_1652_0 = ( ( ! ( _rtB -> B_38_1647_0 <= _rtB -> B_38_104_0 ) ) || ( ! (
_rtB -> B_38_104_0 <= _rtB -> B_38_1649_0 ) ) ) ; isHit = ssIsSampleHit ( S ,
1 , 0 ) ; if ( ( isHit != 0 ) && _rtB -> B_38_1652_0 ) { ssSetStopRequested (
S , 1 ) ; } _rtB -> B_38_1659_0 = ( ( ! ( _rtB -> B_38_1654_0 <= _rtB ->
B_38_115_0 ) ) || ( ! ( _rtB -> B_38_115_0 <= _rtB -> B_38_1656_0 ) ) ) ;
isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( ( isHit != 0 ) && _rtB ->
B_38_1659_0 ) { ssSetStopRequested ( S , 1 ) ; } ssCallAccelRunBlock ( S , 38
, 1661 , SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 38 , 1662 ,
SS_CALL_MDL_OUTPUTS ) ; _rtB -> B_38_1663_0 = _rtP -> P_732 * _rtB ->
B_38_636_0 ; _rtB -> B_38_1665_0 = _rtB -> B_38_1663_0 - _rtB -> B_38_1664_0
; if ( ssIsMajorTimeStep ( S ) != 0 ) { _rtDW -> Abs_MODE_i = ( _rtB ->
B_38_1665_0 >= 0.0 ) ; } _rtB -> B_38_1666_0 = _rtDW -> Abs_MODE_i > 0 ? _rtB
-> B_38_1665_0 : - _rtB -> B_38_1665_0 ; _rtB -> B_38_1668_0 = _rtB ->
B_38_1666_0 / _rtB -> B_38_1667_0 ; if ( ( _rtB -> B_38_1668_0 < 0.0 ) && (
_rtB -> B_38_1669_0 > muDoubleScalarFloor ( _rtB -> B_38_1669_0 ) ) ) {
rtb_B_38_693_0 = - muDoubleScalarPower ( - _rtB -> B_38_1668_0 , _rtB ->
B_38_1669_0 ) ; } else { rtb_B_38_693_0 = muDoubleScalarPower ( _rtB ->
B_38_1668_0 , _rtB -> B_38_1669_0 ) ; } _rtB -> B_38_1671_0 = _rtP -> P_736 *
rtb_B_38_693_0 ; _rtB -> B_38_1673_0 = 0.0 ; _rtB -> B_38_1673_0 += _rtP ->
P_739 * _rtX -> StateSpace_CSTATE_fw ; _rtB -> B_38_1674_0 =
muDoubleScalarExp ( _rtB -> B_38_1671_0 ) * _rtB -> B_38_1673_0 ; _rtB ->
B_38_1676_0 = _rtB -> B_38_1663_0 - _rtB -> B_38_1675_0 ; _rtB -> B_38_1678_0
= _rtB -> B_38_1676_0 / _rtB -> B_38_1677_0 ; _rtB -> B_38_1684_0 = _rtB ->
B_38_1678_0 * _rtB -> B_38_1678_0 * ( real_T ) ( _rtB -> B_38_1676_0 < 0.0 )
+ rtb_B_38_643_0 ; _rtB -> B_38_1686_0 = _rtB -> B_38_1663_0 - _rtB ->
B_38_1685_0 ; _rtB -> B_38_1688_0 = _rtB -> B_38_1686_0 / _rtB -> B_38_1687_0
; _rtB -> B_38_1691_0 = _rtB -> B_38_1688_0 * _rtB -> B_38_1688_0 * ( real_T
) ( _rtB -> B_38_1663_0 > 1.0 ) + _rtB -> B_38_1674_0 ; _rtB -> B_38_1692_0 =
_rtB -> B_38_1684_0 / _rtB -> B_38_1691_0 ; _rtB -> B_38_1693_0 = _rtB ->
B_38_1692_0 - _rtB -> B_38_1682_0 ; _rtB -> B_38_1694_0 = 1.0 / _rtB ->
B_38_1683_0 * _rtB -> B_38_1693_0 ; _rtB -> B_38_1699_0 = _rtP -> P_751 *
_rtB -> B_38_1692_0 ; _rtB -> B_38_1700_0 = _rtP -> P_752 * _rtB ->
B_38_1694_0 ; _rtB -> B_38_1701_0 = _rtP -> P_753 * _rtB -> B_38_1693_0 ;
_rtB -> B_38_1702_0 = _rtB -> B_38_1701_0 + _rtB -> B_38_1698_0 ; _rtB ->
B_38_1703_0 = _rtB -> B_38_1694_0 + _rtB -> B_38_1696_0 ; _rtB -> B_38_1704_0
= _rtB -> B_38_1696_0 - _rtB -> B_38_1700_0 ; _rtB -> B_38_1705_0 = _rtB ->
B_38_1703_0 / _rtB -> B_38_1704_0 ; _rtB -> B_38_1708_0 = ( real_T ) ( _rtB
-> B_38_1692_0 > 1.0 ) * _rtB -> B_38_1705_0 * ( real_T ) ( _rtB ->
B_38_1692_0 <= _rtB -> B_38_1697_0 ) + ( real_T ) ( _rtB -> B_38_1692_0 >
_rtB -> B_38_1697_0 ) * _rtB -> B_38_1702_0 ; _rtB -> B_38_1709_0 = _rtB ->
B_38_1695_0 + _rtB -> B_38_1699_0 ; _rtB -> B_38_1710_0 = _rtB -> B_38_1692_0
- _rtB -> B_38_1695_0 ; _rtB -> B_38_1711_0 = _rtB -> B_38_1710_0 / _rtB ->
B_38_1709_0 ; _rtB -> B_38_1713_0 = ( real_T ) ( _rtB -> B_38_1692_0 <= 1.0 )
* _rtB -> B_38_1711_0 + _rtB -> B_38_1708_0 ; _rtB -> B_38_1714_0 = _rtP ->
P_754 * _rtB -> B_38_1713_0 ; _rtB -> B_38_1715_0 = _rtB -> B_38_648_0 ; if (
ssIsMajorTimeStep ( S ) != 0 ) { _rtDW -> Switch_Mode_cf = ( _rtB ->
B_38_1715_0 > _rtP -> P_756 ) ; } if ( _rtDW -> Switch_Mode_cf ) { _rtB ->
B_38_1717_0 = _rtB -> B_38_1715_0 ; } else { _rtB -> B_38_1717_0 = _rtB ->
B_38_1716_0 ; } _rtB -> B_38_1718_0 = _rtP -> P_757 * _rtB -> B_38_1717_0 ;
_rtB -> B_38_1720_0 = _rtB -> B_38_743_0 ; if ( ssIsMajorTimeStep ( S ) != 0
) { _rtDW -> Switch_Mode_hh = ( _rtB -> B_38_1720_0 > _rtP -> P_759 ) ; } if
( _rtDW -> Switch_Mode_hh ) { _rtB -> B_38_1721_0 = _rtB -> B_38_1720_0 ; }
else { _rtB -> B_38_1721_0 = _rtB -> B_38_1719_0 ; } _rtB -> B_38_1723_0 =
_rtB -> B_38_636_0 ; _rtB -> B_38_1726_0 = _rtB -> B_38_1714_0 ; if (
ssIsMajorTimeStep ( S ) != 0 ) { _rtDW -> Switch_Mode_mo = ( _rtB ->
B_38_1723_0 > _rtP -> P_761 ) ; } _rtB -> B_38_1728_0 = _rtB -> B_38_605_0 ;
if ( ssIsMajorTimeStep ( S ) != 0 ) { _rtDW -> Switch_Mode_nn = ( _rtB ->
B_38_1726_0 > _rtP -> P_763 ) ; _rtDW -> Switch_Mode_b2 = ( _rtB ->
B_38_1728_0 > _rtP -> P_765 ) ; } if ( _rtDW -> Switch_Mode_b2 ) { _rtB ->
B_38_1730_0 = _rtB -> B_38_1728_0 ; } else { _rtB -> B_38_1730_0 = _rtB ->
B_38_1729_0 ; } _rtB -> B_38_1731_0 = _rtP -> P_766 * _rtB -> B_38_1730_0 ;
_rtB -> B_38_1733_0 = _rtB -> B_38_1731_0 - _rtB -> B_38_1732_0 ; if ( _rtB
-> B_38_1733_0 > _rtP -> P_768 ) { _rtB -> B_38_1734_0 = _rtP -> P_768 ; }
else if ( _rtB -> B_38_1733_0 < _rtP -> P_769 ) { _rtB -> B_38_1734_0 = _rtP
-> P_769 ; } else { _rtB -> B_38_1734_0 = _rtB -> B_38_1733_0 ; } _rtB ->
B_38_1736_0 = _rtB -> B_38_746_0 ; if ( ssIsMajorTimeStep ( S ) != 0 ) {
_rtDW -> Switch_Mode_cr = ( _rtB -> B_38_1736_0 > _rtP -> P_771 ) ; } if (
_rtDW -> Switch_Mode_cr ) { _rtB -> B_38_1737_0 = _rtB -> B_38_1736_0 ; }
else { _rtB -> B_38_1737_0 = _rtB -> B_38_1735_0 ; } _rtB -> B_38_1739_0 =
_rtP -> P_773 * _rtB -> B_38_605_0 ; _rtB -> B_38_1741_0 = _rtB ->
B_38_1739_0 - _rtB -> B_38_1740_0 ; _rtB -> B_38_1743_0 = _rtB -> B_38_1741_0
/ _rtB -> B_38_1742_0 ; _rtB -> B_38_1746_0 = _rtB -> B_38_1743_0 * _rtB ->
B_38_1743_0 * ( real_T ) ( _rtB -> B_38_1741_0 < 0.0 ) + rtb_B_38_563_0 ;
_rtB -> B_38_1748_0 = _rtB -> B_38_1739_0 - _rtB -> B_38_1747_0 ; _rtB ->
B_38_1750_0 = _rtB -> B_38_1748_0 / _rtB -> B_38_1749_0 ; _rtB -> B_38_1754_0
= _rtB -> B_38_1739_0 - _rtB -> B_38_1753_0 ; if ( ssIsMajorTimeStep ( S ) !=
0 ) { _rtDW -> Abs_MODE_c = ( _rtB -> B_38_1754_0 >= 0.0 ) ; } _rtB ->
B_38_1755_0 = _rtDW -> Abs_MODE_c > 0 ? _rtB -> B_38_1754_0 : - _rtB ->
B_38_1754_0 ; _rtB -> B_38_1757_0 = _rtB -> B_38_1755_0 / _rtB -> B_38_1756_0
; if ( ( _rtB -> B_38_1757_0 < 0.0 ) && ( _rtB -> B_38_1758_0 >
muDoubleScalarFloor ( _rtB -> B_38_1758_0 ) ) ) { rtb_B_38_693_0 = -
muDoubleScalarPower ( - _rtB -> B_38_1757_0 , _rtB -> B_38_1758_0 ) ; } else
{ rtb_B_38_693_0 = muDoubleScalarPower ( _rtB -> B_38_1757_0 , _rtB ->
B_38_1758_0 ) ; } _rtB -> B_38_1760_0 = _rtP -> P_781 * rtb_B_38_693_0 ; _rtB
-> B_38_1762_0 = 0.0 ; _rtB -> B_38_1762_0 += _rtP -> P_784 * _rtX ->
StateSpace_CSTATE_h0 ; _rtB -> B_38_1763_0 = muDoubleScalarExp ( _rtB ->
B_38_1760_0 ) * _rtB -> B_38_1762_0 ; _rtB -> B_38_1764_0 = _rtB ->
B_38_1750_0 * _rtB -> B_38_1750_0 * ( real_T ) ( _rtB -> B_38_1739_0 > 1.0 )
+ _rtB -> B_38_1763_0 ; _rtB -> B_38_1765_0 = _rtB -> B_38_1746_0 / _rtB ->
B_38_1764_0 ; _rtB -> B_38_1767_0 = _rtB -> B_38_1765_0 - _rtB -> B_38_1766_0
; _rtB -> B_38_1768_0 = _rtP -> P_787 * _rtB -> B_38_1765_0 ; _rtB ->
B_38_1769_0 = _rtB -> B_38_1766_0 + _rtB -> B_38_1768_0 ; _rtB -> B_38_1770_0
= _rtB -> B_38_1767_0 / _rtB -> B_38_1769_0 ; _rtB -> B_38_1775_0 = _rtB ->
B_38_1765_0 - _rtB -> B_38_1773_0 ; _rtB -> B_38_1776_0 = 1.0 / _rtB ->
B_38_1774_0 * _rtB -> B_38_1775_0 ; _rtB -> B_38_1778_0 = _rtB -> B_38_1776_0
+ _rtB -> B_38_1777_0 ; _rtB -> B_38_1779_0 = _rtP -> P_791 * _rtB ->
B_38_1776_0 ; _rtB -> B_38_1780_0 = _rtB -> B_38_1777_0 - _rtB -> B_38_1779_0
; _rtB -> B_38_1781_0 = _rtB -> B_38_1778_0 / _rtB -> B_38_1780_0 ; _rtB ->
B_38_1784_0 = _rtP -> P_793 * _rtB -> B_38_1775_0 ; _rtB -> B_38_1786_0 =
_rtB -> B_38_1784_0 + _rtB -> B_38_1785_0 ; _rtB -> B_38_1788_0 = ( real_T )
( _rtB -> B_38_1765_0 > 1.0 ) * _rtB -> B_38_1781_0 * ( real_T ) ( _rtB ->
B_38_1765_0 <= _rtB -> B_38_1782_0 ) + ( real_T ) ( _rtB -> B_38_1765_0 >
_rtB -> B_38_1782_0 ) * _rtB -> B_38_1786_0 ; _rtB -> B_38_1789_0 = ( real_T
) ( _rtB -> B_38_1765_0 <= 1.0 ) * _rtB -> B_38_1770_0 + _rtB -> B_38_1788_0
; _rtB -> B_38_1790_0 = _rtP -> P_795 * _rtB -> B_38_1789_0 ; _rtB ->
B_38_1791_0 = _rtB -> B_38_1790_0 ; if ( ssIsMajorTimeStep ( S ) != 0 ) {
_rtDW -> Switch_Mode_mm = ( _rtB -> B_38_1791_0 > _rtP -> P_796 ) ; }
ssCallAccelRunBlock ( S , 38 , 1793 , SS_CALL_MDL_OUTPUTS ) ; _rtB ->
B_38_1794_0 = _rtB -> B_38_527_0 ; if ( ssIsMajorTimeStep ( S ) != 0 ) {
_rtDW -> Switch_Mode_by = ( _rtB -> B_38_1794_0 > _rtP -> P_798 ) ; } if (
_rtDW -> Switch_Mode_by ) { _rtB -> B_38_1796_0 = _rtB -> B_38_1794_0 ; }
else { _rtB -> B_38_1796_0 = _rtB -> B_38_1795_0 ; } _rtB -> B_38_1797_0 =
_rtP -> P_799 * _rtB -> B_38_1796_0 ; _rtB -> B_38_1799_0 = _rtB ->
B_38_726_0 ; if ( ssIsMajorTimeStep ( S ) != 0 ) { _rtDW -> Switch_Mode_ai =
( _rtB -> B_38_1799_0 > _rtP -> P_801 ) ; } if ( _rtDW -> Switch_Mode_ai ) {
_rtB -> B_38_1800_0 = _rtB -> B_38_1799_0 ; } else { _rtB -> B_38_1800_0 =
_rtB -> B_38_1798_0 ; } _rtB -> B_38_1802_0 = _rtB -> B_38_515_0 ; _rtB ->
B_38_1805_0 = _rtP -> P_805 * _rtB -> B_38_515_0 ; _rtB -> B_38_1807_0 = _rtB
-> B_38_1805_0 - _rtB -> B_38_1806_0 ; _rtB -> B_38_1809_0 = _rtB ->
B_38_1807_0 / _rtB -> B_38_1808_0 ; _rtB -> B_38_1812_0 = _rtB -> B_38_1809_0
* _rtB -> B_38_1809_0 * ( real_T ) ( _rtB -> B_38_1807_0 < 0.0 ) +
rtb_B_38_176_0 ; _rtB -> B_38_1814_0 = _rtB -> B_38_1805_0 - _rtB ->
B_38_1813_0 ; _rtB -> B_38_1816_0 = _rtB -> B_38_1814_0 / _rtB -> B_38_1815_0
; _rtB -> B_38_1820_0 = _rtB -> B_38_1805_0 - _rtB -> B_38_1819_0 ; if (
ssIsMajorTimeStep ( S ) != 0 ) { _rtDW -> Switch_Mode_kd = ( _rtB ->
B_38_1802_0 > _rtP -> P_803 ) ; _rtDW -> Abs_MODE_di = ( _rtB -> B_38_1820_0
>= 0.0 ) ; } _rtB -> B_38_1821_0 = _rtDW -> Abs_MODE_di > 0 ? _rtB ->
B_38_1820_0 : - _rtB -> B_38_1820_0 ; _rtB -> B_38_1823_0 = _rtB ->
B_38_1821_0 / _rtB -> B_38_1822_0 ; if ( ( _rtB -> B_38_1823_0 < 0.0 ) && (
_rtB -> B_38_1824_0 > muDoubleScalarFloor ( _rtB -> B_38_1824_0 ) ) ) {
rtb_B_38_693_0 = - muDoubleScalarPower ( - _rtB -> B_38_1823_0 , _rtB ->
B_38_1824_0 ) ; } else { rtb_B_38_693_0 = muDoubleScalarPower ( _rtB ->
B_38_1823_0 , _rtB -> B_38_1824_0 ) ; } _rtB -> B_38_1826_0 = _rtP -> P_813 *
rtb_B_38_693_0 ; _rtB -> B_38_1828_0 = 0.0 ; _rtB -> B_38_1828_0 += _rtP ->
P_816 * _rtX -> StateSpace_CSTATE_d ; _rtB -> B_38_1829_0 = muDoubleScalarExp
( _rtB -> B_38_1826_0 ) * _rtB -> B_38_1828_0 ; _rtB -> B_38_1830_0 = _rtB ->
B_38_1816_0 * _rtB -> B_38_1816_0 * ( real_T ) ( _rtB -> B_38_1805_0 > 1.0 )
+ _rtB -> B_38_1829_0 ; _rtB -> B_38_1831_0 = _rtB -> B_38_1812_0 / _rtB ->
B_38_1830_0 ; _rtB -> B_38_1833_0 = _rtB -> B_38_1831_0 - _rtB -> B_38_1832_0
; _rtB -> B_38_1834_0 = _rtP -> P_819 * _rtB -> B_38_1831_0 ; _rtB ->
B_38_1835_0 = _rtB -> B_38_1832_0 + _rtB -> B_38_1834_0 ; _rtB -> B_38_1836_0
= _rtB -> B_38_1833_0 / _rtB -> B_38_1835_0 ; _rtB -> B_38_1841_0 = _rtB ->
B_38_1831_0 - _rtB -> B_38_1839_0 ; _rtB -> B_38_1842_0 = 1.0 / _rtB ->
B_38_1840_0 * _rtB -> B_38_1841_0 ; _rtB -> B_38_1844_0 = _rtB -> B_38_1842_0
+ _rtB -> B_38_1843_0 ; _rtB -> B_38_1845_0 = _rtP -> P_823 * _rtB ->
B_38_1842_0 ; _rtB -> B_38_1846_0 = _rtB -> B_38_1843_0 - _rtB -> B_38_1845_0
; _rtB -> B_38_1847_0 = _rtB -> B_38_1844_0 / _rtB -> B_38_1846_0 ; _rtB ->
B_38_1850_0 = _rtP -> P_825 * _rtB -> B_38_1841_0 ; _rtB -> B_38_1852_0 =
_rtB -> B_38_1850_0 + _rtB -> B_38_1851_0 ; _rtB -> B_38_1854_0 = ( real_T )
( _rtB -> B_38_1831_0 > 1.0 ) * _rtB -> B_38_1847_0 * ( real_T ) ( _rtB ->
B_38_1831_0 <= _rtB -> B_38_1848_0 ) + ( real_T ) ( _rtB -> B_38_1831_0 >
_rtB -> B_38_1848_0 ) * _rtB -> B_38_1852_0 ; _rtB -> B_38_1855_0 = ( real_T
) ( _rtB -> B_38_1831_0 <= 1.0 ) * _rtB -> B_38_1836_0 + _rtB -> B_38_1854_0
; _rtB -> B_38_1856_0 = _rtP -> P_827 * _rtB -> B_38_1855_0 ; _rtB ->
B_38_1857_0 = _rtB -> B_38_1856_0 ; if ( ssIsMajorTimeStep ( S ) != 0 ) {
_rtDW -> Switch_Mode_my = ( _rtB -> B_38_1857_0 > _rtP -> P_828 ) ; }
ssCallAccelRunBlock ( S , 38 , 1859 , SS_CALL_MDL_OUTPUTS ) ; isHit =
ssIsSampleHit ( S , 3 , 0 ) ; if ( isHit != 0 ) { ssCallAccelRunBlock ( S ,
38 , 1860 , SS_CALL_MDL_OUTPUTS ) ; } UNUSED_PARAMETER ( tid ) ; } static
void mdlOutputsTID4 ( SimStruct * S , int_T tid ) { B_nmm2DFullBody_T * _rtB
; P_nmm2DFullBody_T * _rtP ; DW_nmm2DFullBody_T * _rtDW ; _rtDW = ( (
DW_nmm2DFullBody_T * ) ssGetRootDWork ( S ) ) ; _rtP = ( ( P_nmm2DFullBody_T
* ) ssGetModelRtp ( S ) ) ; _rtB = ( ( B_nmm2DFullBody_T * )
_ssGetModelBlockIO ( S ) ) ; _rtB -> B_38_0_0 = _rtP -> P_8 ;
nmm2DFullBody_GroundFrictionModelTID4 ( S , & _rtB -> GroundFrictionModel_j ,
& _rtDW -> GroundFrictionModel_j , & _rtP -> GroundFrictionModel_j ) ; _rtB
-> B_38_8_0 = _rtP -> P_14 ; nmm2DFullBody_GroundFrictionModelTID4 ( S , &
_rtB -> GroundFrictionModel_i , & _rtDW -> GroundFrictionModel_i , & _rtP ->
GroundFrictionModel_i ) ; _rtB -> B_38_34_0 = _rtP -> P_18 ;
nmm2DFullBody_GroundFrictionModelTID4 ( S , & _rtB -> GroundFrictionModel , &
_rtDW -> GroundFrictionModel , & _rtP -> GroundFrictionModel ) ; _rtB ->
B_38_41_0 = _rtP -> P_22 ; nmm2DFullBody_GroundFrictionModelTID4 ( S , & _rtB
-> GroundFrictionModel_m , & _rtDW -> GroundFrictionModel_m , & _rtP ->
GroundFrictionModel_m ) ; _rtB -> B_38_132_0 = _rtP -> P_54 ; _rtB ->
B_38_138_0 = _rtP -> P_58 ; _rtB -> B_38_146_0 = _rtP -> P_61 ; _rtB ->
B_38_148_0 = muDoubleScalarSin ( _rtP -> P_60 - _rtB -> B_38_146_0 ) ; _rtB
-> B_38_155_0 = _rtP -> P_64 ; _rtB -> B_38_157_0 = _rtP -> P_66 ; _rtB ->
B_38_159_0 = muDoubleScalarSin ( _rtP -> P_65 - _rtB -> B_38_157_0 ) ; _rtB
-> B_38_167_0 = _rtP -> P_69 ; _rtB -> B_38_172_0 = _rtP -> P_71 ; _rtB ->
B_38_174_0 = _rtP -> P_72 ; _rtB -> B_38_180_0 = _rtP -> P_74 ; _rtB ->
B_38_185_0 = _rtP -> P_76 ; _rtB -> B_38_187_0 = _rtP -> P_78 ; _rtB ->
B_38_189_0 = muDoubleScalarSin ( _rtP -> P_77 - _rtB -> B_38_187_0 ) ; _rtB
-> B_38_197_0 = _rtP -> P_81 ; _rtB -> B_38_202_0 = _rtP -> P_83 ; _rtB ->
B_38_204_0 = _rtP -> P_84 ; _rtB -> B_38_210_0 = _rtP -> P_86 ; _rtB ->
B_38_217_0 = _rtP -> P_90 ; _rtB -> B_38_219_0 = muDoubleScalarSin ( _rtP ->
P_89 - _rtB -> B_38_217_0 ) ; _rtB -> B_38_225_0 = _rtP -> P_92 ; _rtB ->
B_38_228_0 = _rtP -> P_94 ; _rtB -> B_38_233_0 = _rtP -> P_96 ; _rtB ->
B_38_235_0 = _rtP -> P_97 ; _rtB -> B_38_241_0 = _rtP -> P_99 ; _rtB ->
B_38_250_0 = _rtP -> P_102 ; _rtB -> B_38_251_0 = _rtP -> P_103 ; _rtB ->
B_38_258_0 = _rtP -> P_107 ; _rtB -> B_38_260_0 = _rtP -> P_108 ; _rtB ->
B_38_263_0 = _rtP -> P_110 ; _rtB -> B_38_265_0 = _rtP -> P_112 ; _rtB ->
B_38_267_0 = muDoubleScalarSin ( _rtP -> P_111 - _rtB -> B_38_265_0 ) ; _rtB
-> B_38_275_0 = _rtP -> P_115 ; _rtB -> B_38_280_0 = _rtP -> P_117 ; _rtB ->
B_38_282_0 = _rtP -> P_118 ; _rtB -> B_38_288_0 = _rtP -> P_120 ; _rtB ->
B_38_294_0 = _rtP -> P_123 ; _rtB -> B_38_296_0 = muDoubleScalarSin ( _rtP ->
P_122 - _rtB -> B_38_294_0 ) ; _rtB -> B_38_302_0 = _rtP -> P_125 ; _rtB ->
B_38_305_0 = _rtP -> P_127 ; _rtB -> B_38_310_0 = _rtP -> P_129 ; _rtB ->
B_38_312_0 = _rtP -> P_130 ; _rtB -> B_38_318_0 = _rtP -> P_132 ; _rtB ->
B_38_331_0 = _rtP -> P_136 ; _rtB -> B_38_338_0 = _rtP -> P_140 ; _rtB ->
B_38_339_0 = _rtP -> P_141 ; _rtB -> B_38_341_0 = _rtP -> P_142 ; _rtB ->
B_38_345_0 = _rtP -> P_144 ; _rtB -> B_38_350_0 = _rtP -> P_146 ; _rtB ->
B_38_352_0 = _rtP -> P_147 ; _rtB -> B_38_358_0 = _rtP -> P_149 ; _rtB ->
B_38_363_0 = _rtP -> P_151 ; _rtB -> B_38_364_0 = _rtP -> P_152 ; _rtB ->
B_38_365_0 = _rtP -> P_153 ; _rtB -> B_38_370_0 = _rtP -> P_155 ; _rtB ->
B_38_375_0 = _rtP -> P_157 ; _rtB -> B_38_377_0 = _rtP -> P_158 ; _rtB ->
B_38_383_0 = _rtP -> P_160 ; _rtB -> B_38_395_0 = _rtP -> P_163 ; _rtB ->
B_38_402_0 = _rtP -> P_167 ; _rtB -> B_38_403_0 = _rtP -> P_168 ; _rtB ->
B_38_405_0 = _rtP -> P_169 ; _rtB -> B_38_409_0 = _rtP -> P_171 ; _rtB ->
B_38_414_0 = _rtP -> P_173 ; _rtB -> B_38_416_0 = _rtP -> P_174 ; _rtB ->
B_38_422_0 = _rtP -> P_176 ; _rtB -> B_38_427_0 = _rtP -> P_178 ; _rtB ->
B_38_428_0 = _rtP -> P_179 ; _rtB -> B_38_429_0 = _rtP -> P_180 ; _rtB ->
B_38_434_0 = _rtP -> P_182 ; _rtB -> B_38_439_0 = _rtP -> P_184 ; _rtB ->
B_38_441_0 = _rtP -> P_185 ; _rtB -> B_38_447_0 = _rtP -> P_187 ; _rtB ->
B_38_453_0 = _rtP -> P_190 ; _rtB -> B_38_455_0 = _rtP -> P_191 ; _rtB ->
B_38_458_0 = _rtP -> P_193 ; _rtB -> B_38_460_0 = _rtP -> P_195 ; _rtB ->
B_38_462_0 = muDoubleScalarSin ( _rtP -> P_194 - _rtB -> B_38_460_0 ) ; _rtB
-> B_38_470_0 = _rtP -> P_198 ; _rtB -> B_38_475_0 = _rtP -> P_200 ; _rtB ->
B_38_477_0 = _rtP -> P_201 ; _rtB -> B_38_483_0 = _rtP -> P_203 ; _rtB ->
B_38_491_0 = _rtP -> P_205 ; _rtB -> B_38_502_0 = _rtP -> P_210 ; _rtB ->
B_38_504_0 = muDoubleScalarSin ( _rtP -> P_209 - _rtB -> B_38_502_0 ) ; _rtB
-> B_38_510_0 = _rtP -> P_212 ; _rtB -> B_38_513_0 = _rtP -> P_214 ; _rtB ->
B_38_518_0 = _rtP -> P_216 ; _rtB -> B_38_520_0 = _rtP -> P_217 ; _rtB ->
B_38_526_0 = _rtP -> P_219 ; _rtB -> B_38_533_0 = _rtP -> P_223 ; _rtB ->
B_38_535_0 = muDoubleScalarSin ( _rtP -> P_222 - _rtB -> B_38_533_0 ) ; _rtB
-> B_38_542_0 = _rtP -> P_226 ; _rtB -> B_38_544_0 = _rtP -> P_228 ; _rtB ->
B_38_546_0 = muDoubleScalarSin ( _rtP -> P_227 - _rtB -> B_38_544_0 ) ; _rtB
-> B_38_554_0 = _rtP -> P_231 ; _rtB -> B_38_559_0 = _rtP -> P_233 ; _rtB ->
B_38_561_0 = _rtP -> P_234 ; _rtB -> B_38_567_0 = _rtP -> P_236 ; _rtB ->
B_38_576_0 = _rtP -> P_239 ; _rtB -> B_38_582_0 = _rtP -> P_243 ; _rtB ->
B_38_591_0 = _rtP -> P_245 ; _rtB -> B_38_593_0 = _rtP -> P_247 ; _rtB ->
B_38_595_0 = muDoubleScalarSin ( _rtP -> P_246 - _rtB -> B_38_593_0 ) ; _rtB
-> B_38_603_0 = _rtP -> P_250 ; _rtB -> B_38_608_0 = _rtP -> P_252 ; _rtB ->
B_38_610_0 = _rtP -> P_253 ; _rtB -> B_38_616_0 = _rtP -> P_255 ; _rtB ->
B_38_623_0 = _rtP -> P_259 ; _rtB -> B_38_625_0 = muDoubleScalarSin ( _rtP ->
P_258 - _rtB -> B_38_623_0 ) ; _rtB -> B_38_631_0 = _rtP -> P_261 ; _rtB ->
B_38_634_0 = _rtP -> P_263 ; _rtB -> B_38_639_0 = _rtP -> P_265 ; _rtB ->
B_38_641_0 = _rtP -> P_266 ; _rtB -> B_38_647_0 = _rtP -> P_268 ; _rtB ->
B_38_657_0 [ 0 ] = _rtP -> P_272 * _rtP -> P_271 [ 0 ] ; _rtB -> B_38_657_0 [
1 ] = _rtP -> P_272 * _rtP -> P_271 [ 1 ] ; _rtB -> B_38_657_0 [ 2 ] = _rtP
-> P_272 * _rtP -> P_271 [ 2 ] ; nmm2DFullBody_StancePhase_iTID4 ( S , & _rtB
-> StancePhase_i , & _rtDW -> StancePhase_i , & _rtP -> StancePhase_i ) ;
nmm2DFullBody_SwingPhaseTID4 ( S , & _rtB -> SwingPhase_k , & _rtDW ->
SwingPhase_k , & _rtP -> SwingPhase_k ) ; nmm2DFullBody_StancePhase_oTID4 ( S
, & _rtB -> StancePhase_o , & _rtDW -> StancePhase_o , & _rtP ->
StancePhase_o ) ; nmm2DFullBody_SwingPhase_bTID4 ( S , & _rtB -> SwingPhase_b
, & _rtDW -> SwingPhase_b , & _rtP -> SwingPhase_b ) ;
nmm2DFullBody_StancePhase_gTID4 ( S , & _rtB -> StancePhase_g , & _rtDW ->
StancePhase_g , & _rtP -> StancePhase_g ) ; nmm2DFullBody_StancePhaseTID4 ( S
, & _rtB -> StancePhase , & _rtDW -> StancePhase , & _rtP -> StancePhase ) ;
nmm2DFullBody_StancePhase_lTID4 ( S , & _rtB -> StancePhase_l , & _rtDW ->
StancePhase_l , & _rtP -> StancePhase_l ) ; nmm2DFullBody_SwingPhase_fTID4 (
S , & _rtB -> SwingPhase_f , & _rtDW -> SwingPhase_f , & _rtP -> SwingPhase_f
) ; nmm2DFullBody_StancePhase_iTID4 ( S , & _rtB -> StancePhase_k , & _rtDW
-> StancePhase_k , & _rtP -> StancePhase_k ) ; nmm2DFullBody_SwingPhaseTID4 (
S , & _rtB -> SwingPhase_j , & _rtDW -> SwingPhase_j , & _rtP -> SwingPhase_j
) ; nmm2DFullBody_StancePhase_oTID4 ( S , & _rtB -> StancePhase_j , & _rtDW
-> StancePhase_j , & _rtP -> StancePhase_j ) ; nmm2DFullBody_SwingPhase_bTID4
( S , & _rtB -> SwingPhase_m , & _rtDW -> SwingPhase_m , & _rtP ->
SwingPhase_m ) ; nmm2DFullBody_StancePhase_gTID4 ( S , & _rtB ->
StancePhase_b , & _rtDW -> StancePhase_b , & _rtP -> StancePhase_b ) ;
nmm2DFullBody_StancePhaseTID4 ( S , & _rtB -> StancePhase_a , & _rtDW ->
StancePhase_a , & _rtP -> StancePhase_a ) ; nmm2DFullBody_StancePhase_lTID4 (
S , & _rtB -> StancePhase_n , & _rtDW -> StancePhase_n , & _rtP ->
StancePhase_n ) ; nmm2DFullBody_SwingPhase_fTID4 ( S , & _rtB -> SwingPhase_i
, & _rtDW -> SwingPhase_i , & _rtP -> SwingPhase_i ) ; _rtB -> B_38_810_0 =
_rtP -> P_350 ; _rtB -> B_38_811_0 = _rtP -> P_351 ; _rtB -> B_38_835_0 =
_rtP -> P_356 ; _rtB -> B_38_879_0 = _rtP -> P_360 ; _rtB -> B_38_882_0 =
_rtP -> P_361 ; _rtB -> B_38_884_0 = _rtP -> P_362 ; _rtB -> B_38_890_0 =
_rtP -> P_368 ; _rtB -> B_38_892_0 = _rtP -> P_369 ; _rtB -> B_38_897_0 =
_rtP -> P_371 ; _rtB -> B_38_898_0 = _rtB -> B_38_897_0 - _rtP -> P_370 ;
_rtB -> B_38_900_0 = _rtP -> P_372 ; _rtB -> B_38_902_0 = _rtP -> P_373 ;
_rtB -> B_38_910_0 = _rtP -> P_374 ; _rtB -> B_38_911_0 = _rtP -> P_375 ;
_rtB -> B_38_912_0 = _rtP -> P_376 ; _rtB -> B_38_913_0 = _rtP -> P_377 ;
_rtB -> B_38_931_0 = _rtP -> P_382 ; _rtB -> B_38_934_0 = _rtP -> P_385 ;
_rtB -> B_38_937_0 = _rtP -> P_387 ; _rtB -> B_38_940_0 = _rtP -> P_389 ;
_rtB -> B_38_945_0 = _rtP -> P_392 ; _rtB -> B_38_948_0 = _rtP -> P_393 ;
_rtB -> B_38_950_0 = _rtP -> P_394 ; _rtB -> B_38_956_0 = _rtP -> P_400 ;
_rtB -> B_38_958_0 = _rtP -> P_401 ; _rtB -> B_38_963_0 = _rtP -> P_403 ;
_rtB -> B_38_964_0 = _rtB -> B_38_963_0 - _rtP -> P_402 ; _rtB -> B_38_966_0
= _rtP -> P_404 ; _rtB -> B_38_968_0 = _rtP -> P_405 ; _rtB -> B_38_976_0 =
_rtP -> P_406 ; _rtB -> B_38_977_0 = _rtP -> P_407 ; _rtB -> B_38_978_0 =
_rtP -> P_408 ; _rtB -> B_38_979_0 = _rtP -> P_409 ; _rtB -> B_38_997_0 =
_rtP -> P_414 ; _rtB -> B_38_1000_0 = _rtP -> P_417 ; _rtB -> B_38_1003_0 =
_rtP -> P_419 ; _rtB -> B_38_1006_0 = _rtP -> P_421 ; _rtB -> B_38_1011_0 =
_rtP -> P_423 ; _rtB -> B_38_1015_0 = _rtP -> P_425 ; _rtB -> B_38_1019_0 =
_rtP -> P_429 ; _rtB -> B_38_1022_0 = _rtP -> P_430 ; _rtB -> B_38_1024_0 =
_rtP -> P_431 ; _rtB -> B_38_1030_0 = _rtP -> P_437 ; _rtB -> B_38_1032_0 =
_rtP -> P_438 ; _rtB -> B_38_1037_0 = _rtP -> P_440 ; _rtB -> B_38_1038_0 =
_rtB -> B_38_1037_0 - _rtP -> P_439 ; _rtB -> B_38_1040_0 = _rtP -> P_441 ;
_rtB -> B_38_1042_0 = _rtP -> P_442 ; _rtB -> B_38_1050_0 = _rtP -> P_443 ;
_rtB -> B_38_1051_0 = _rtP -> P_444 ; _rtB -> B_38_1052_0 = _rtP -> P_445 ;
_rtB -> B_38_1053_0 = _rtP -> P_446 ; _rtB -> B_38_1071_0 = _rtP -> P_451 ;
_rtB -> B_38_1074_0 = _rtP -> P_454 ; _rtB -> B_38_1077_0 = _rtP -> P_457 ;
_rtB -> B_38_1080_0 = _rtP -> P_459 ; _rtB -> B_38_1084_0 = _rtP -> P_462 ;
_rtB -> B_38_1087_0 = _rtP -> P_463 ; _rtB -> B_38_1089_0 = _rtP -> P_464 ;
_rtB -> B_38_1095_0 = _rtP -> P_470 ; _rtB -> B_38_1097_0 = _rtP -> P_471 ;
_rtB -> B_38_1102_0 = _rtP -> P_473 ; _rtB -> B_38_1103_0 = _rtB ->
B_38_1102_0 - _rtP -> P_472 ; _rtB -> B_38_1105_0 = _rtP -> P_474 ; _rtB ->
B_38_1107_0 = _rtP -> P_475 ; _rtB -> B_38_1115_0 = _rtP -> P_476 ; _rtB ->
B_38_1116_0 = _rtP -> P_477 ; _rtB -> B_38_1117_0 = _rtP -> P_478 ; _rtB ->
B_38_1118_0 = _rtP -> P_479 ; _rtB -> B_38_1138_0 = _rtP -> P_485 ; _rtB ->
B_38_1141_0 = _rtP -> P_488 ; _rtB -> B_38_1144_0 = _rtP -> P_491 ; _rtB ->
B_38_1147_0 = _rtP -> P_493 ; _rtB -> B_38_1153_0 = _rtP -> P_496 ; _rtB ->
B_38_1156_0 = _rtP -> P_497 ; _rtB -> B_38_1158_0 = _rtP -> P_498 ; _rtB ->
B_38_1164_0 = _rtP -> P_504 ; _rtB -> B_38_1166_0 = _rtP -> P_505 ; _rtB ->
B_38_1171_0 = _rtP -> P_507 ; _rtB -> B_38_1172_0 = _rtB -> B_38_1171_0 -
_rtP -> P_506 ; _rtB -> B_38_1174_0 = _rtP -> P_508 ; _rtB -> B_38_1176_0 =
_rtP -> P_509 ; _rtB -> B_38_1184_0 = _rtP -> P_510 ; _rtB -> B_38_1185_0 =
_rtP -> P_511 ; _rtB -> B_38_1186_0 = _rtP -> P_512 ; _rtB -> B_38_1187_0 =
_rtP -> P_513 ; _rtB -> B_38_1205_0 = _rtP -> P_518 ; _rtB -> B_38_1208_0 =
_rtP -> P_521 ; _rtB -> B_38_1211_0 = _rtP -> P_523 ; _rtB -> B_38_1214_0 =
_rtP -> P_525 ; _rtB -> B_38_1218_0 = _rtP -> P_527 ; _rtB -> B_38_1221_0 =
_rtP -> P_530 ; _rtB -> B_38_1224_0 = _rtP -> P_533 ; _rtB -> B_38_1227_0 =
_rtP -> P_535 ; _rtB -> B_38_1229_0 = _rtP -> P_537 ; _rtB -> B_38_1231_0 =
_rtP -> P_538 ; _rtB -> B_38_1236_0 = _rtP -> P_539 ; _rtB -> B_38_1238_0 =
_rtP -> P_540 ; _rtB -> B_38_1242_0 = _rtP -> P_541 ; _rtB -> B_38_1245_0 =
_rtP -> P_542 ; _rtB -> B_38_1247_0 = _rtP -> P_543 ; _rtB -> B_38_1255_0 =
_rtP -> P_549 ; _rtB -> B_38_1262_0 = _rtP -> P_552 ; _rtB -> B_38_1263_0 =
_rtB -> B_38_1262_0 - _rtP -> P_551 ; _rtB -> B_38_1266_0 = _rtP -> P_553 ;
_rtB -> B_38_1271_0 = _rtP -> P_555 ; _rtB -> B_38_1274_0 = _rtP -> P_557 ;
_rtB -> B_38_1284_0 = _rtP -> P_560 ; _rtB -> B_38_1287_0 = _rtP -> P_563 ;
_rtB -> B_38_1290_0 = _rtP -> P_565 ; _rtB -> B_38_1293_0 = _rtP -> P_567 ;
_rtB -> B_38_1295_0 = _rtP -> P_569 ; _rtB -> B_38_1297_0 = _rtP -> P_570 ;
_rtB -> B_38_1302_0 = _rtP -> P_571 ; _rtB -> B_38_1304_0 = _rtP -> P_572 ;
_rtB -> B_38_1308_0 = _rtP -> P_573 ; _rtB -> B_38_1311_0 = _rtP -> P_574 ;
_rtB -> B_38_1313_0 = _rtP -> P_575 ; _rtB -> B_38_1321_0 = _rtP -> P_581 ;
_rtB -> B_38_1328_0 = _rtP -> P_584 ; _rtB -> B_38_1329_0 = _rtB ->
B_38_1328_0 - _rtP -> P_583 ; _rtB -> B_38_1332_0 = _rtP -> P_585 ; _rtB ->
B_38_1337_0 = _rtP -> P_587 ; _rtB -> B_38_1340_0 = _rtP -> P_589 ; _rtB ->
B_38_1369_0 = _rtP -> P_592 ; _rtB -> B_38_1371_0 = _rtP -> P_593 ; _rtB ->
B_38_1378_0 = _rtP -> P_595 ; _rtB -> B_38_1381_0 = _rtP -> P_596 ; _rtB ->
B_38_1383_0 = _rtP -> P_597 ; _rtB -> B_38_1389_0 = _rtP -> P_603 ; _rtB ->
B_38_1391_0 = _rtP -> P_604 ; _rtB -> B_38_1396_0 = _rtP -> P_606 ; _rtB ->
B_38_1397_0 = _rtB -> B_38_1396_0 - _rtP -> P_605 ; _rtB -> B_38_1399_0 =
_rtP -> P_607 ; _rtB -> B_38_1401_0 = _rtP -> P_608 ; _rtB -> B_38_1409_0 =
_rtP -> P_609 ; _rtB -> B_38_1410_0 = _rtP -> P_610 ; _rtB -> B_38_1411_0 =
_rtP -> P_611 ; _rtB -> B_38_1412_0 = _rtP -> P_612 ; _rtB -> B_38_1430_0 =
_rtP -> P_617 ; _rtB -> B_38_1433_0 = _rtP -> P_620 ; _rtB -> B_38_1436_0 =
_rtP -> P_622 ; _rtB -> B_38_1439_0 = _rtP -> P_624 ; _rtB -> B_38_1444_0 =
_rtP -> P_627 ; _rtB -> B_38_1447_0 = _rtP -> P_628 ; _rtB -> B_38_1449_0 =
_rtP -> P_629 ; _rtB -> B_38_1455_0 = _rtP -> P_635 ; _rtB -> B_38_1457_0 =
_rtP -> P_636 ; _rtB -> B_38_1462_0 = _rtP -> P_638 ; _rtB -> B_38_1463_0 =
_rtB -> B_38_1462_0 - _rtP -> P_637 ; _rtB -> B_38_1465_0 = _rtP -> P_639 ;
_rtB -> B_38_1467_0 = _rtP -> P_640 ; _rtB -> B_38_1475_0 = _rtP -> P_641 ;
_rtB -> B_38_1476_0 = _rtP -> P_642 ; _rtB -> B_38_1477_0 = _rtP -> P_643 ;
_rtB -> B_38_1478_0 = _rtP -> P_644 ; _rtB -> B_38_1496_0 = _rtP -> P_649 ;
_rtB -> B_38_1499_0 = _rtP -> P_652 ; _rtB -> B_38_1502_0 = _rtP -> P_654 ;
_rtB -> B_38_1505_0 = _rtP -> P_656 ; _rtB -> B_38_1512_0 = _rtP -> P_658 ;
_rtB -> B_38_1516_0 = _rtP -> P_662 ; _rtB -> B_38_1519_0 = _rtP -> P_663 ;
_rtB -> B_38_1521_0 = _rtP -> P_664 ; _rtB -> B_38_1527_0 = _rtP -> P_670 ;
_rtB -> B_38_1529_0 = _rtP -> P_671 ; _rtB -> B_38_1534_0 = _rtP -> P_673 ;
_rtB -> B_38_1535_0 = _rtB -> B_38_1534_0 - _rtP -> P_672 ; _rtB ->
B_38_1537_0 = _rtP -> P_674 ; _rtB -> B_38_1539_0 = _rtP -> P_675 ; _rtB ->
B_38_1547_0 = _rtP -> P_676 ; _rtB -> B_38_1548_0 = _rtP -> P_677 ; _rtB ->
B_38_1549_0 = _rtP -> P_678 ; _rtB -> B_38_1550_0 = _rtP -> P_679 ; _rtB ->
B_38_1568_0 = _rtP -> P_684 ; _rtB -> B_38_1571_0 = _rtP -> P_687 ; _rtB ->
B_38_1574_0 = _rtP -> P_690 ; _rtB -> B_38_1577_0 = _rtP -> P_692 ; _rtB ->
B_38_1581_0 = _rtP -> P_695 ; _rtB -> B_38_1584_0 = _rtP -> P_696 ; _rtB ->
B_38_1586_0 = _rtP -> P_697 ; _rtB -> B_38_1592_0 = _rtP -> P_703 ; _rtB ->
B_38_1594_0 = _rtP -> P_704 ; _rtB -> B_38_1599_0 = _rtP -> P_706 ; _rtB ->
B_38_1600_0 = _rtB -> B_38_1599_0 - _rtP -> P_705 ; _rtB -> B_38_1602_0 =
_rtP -> P_707 ; _rtB -> B_38_1604_0 = _rtP -> P_708 ; _rtB -> B_38_1612_0 =
_rtP -> P_709 ; _rtB -> B_38_1613_0 = _rtP -> P_710 ; _rtB -> B_38_1614_0 =
_rtP -> P_711 ; _rtB -> B_38_1615_0 = _rtP -> P_712 ; _rtB -> B_38_1635_0 =
_rtP -> P_718 ; _rtB -> B_38_1638_0 = _rtP -> P_721 ; _rtB -> B_38_1641_0 =
_rtP -> P_724 ; _rtB -> B_38_1644_0 = _rtP -> P_726 ; _rtB -> B_38_1647_0 =
_rtP -> P_728 ; _rtB -> B_38_1649_0 = _rtP -> P_729 ; _rtB -> B_38_1654_0 =
_rtP -> P_730 ; _rtB -> B_38_1656_0 = _rtP -> P_731 ; _rtB -> B_38_1664_0 =
_rtP -> P_733 ; _rtB -> B_38_1667_0 = _rtP -> P_734 ; _rtB -> B_38_1669_0 =
_rtP -> P_735 ; _rtB -> B_38_1675_0 = _rtP -> P_741 ; _rtB -> B_38_1677_0 =
_rtP -> P_742 ; _rtB -> B_38_1682_0 = _rtP -> P_744 ; _rtB -> B_38_1683_0 =
_rtB -> B_38_1682_0 - _rtP -> P_743 ; _rtB -> B_38_1685_0 = _rtP -> P_745 ;
_rtB -> B_38_1687_0 = _rtP -> P_746 ; _rtB -> B_38_1695_0 = _rtP -> P_747 ;
_rtB -> B_38_1696_0 = _rtP -> P_748 ; _rtB -> B_38_1697_0 = _rtP -> P_749 ;
_rtB -> B_38_1698_0 = _rtP -> P_750 ; _rtB -> B_38_1716_0 = _rtP -> P_755 ;
_rtB -> B_38_1719_0 = _rtP -> P_758 ; _rtB -> B_38_1722_0 = _rtP -> P_760 ;
_rtB -> B_38_1725_0 = _rtP -> P_762 ; _rtB -> B_38_1729_0 = _rtP -> P_764 ;
_rtB -> B_38_1732_0 = _rtP -> P_767 ; _rtB -> B_38_1735_0 = _rtP -> P_770 ;
_rtB -> B_38_1738_0 = _rtP -> P_772 ; _rtB -> B_38_1740_0 = _rtP -> P_774 ;
_rtB -> B_38_1742_0 = _rtP -> P_775 ; _rtB -> B_38_1747_0 = _rtP -> P_776 ;
_rtB -> B_38_1749_0 = _rtP -> P_777 ; _rtB -> B_38_1753_0 = _rtP -> P_778 ;
_rtB -> B_38_1756_0 = _rtP -> P_779 ; _rtB -> B_38_1758_0 = _rtP -> P_780 ;
_rtB -> B_38_1766_0 = _rtP -> P_786 ; _rtB -> B_38_1773_0 = _rtP -> P_789 ;
_rtB -> B_38_1774_0 = _rtB -> B_38_1773_0 - _rtP -> P_788 ; _rtB ->
B_38_1777_0 = _rtP -> P_790 ; _rtB -> B_38_1782_0 = _rtP -> P_792 ; _rtB ->
B_38_1785_0 = _rtP -> P_794 ; _rtB -> B_38_1795_0 = _rtP -> P_797 ; _rtB ->
B_38_1798_0 = _rtP -> P_800 ; _rtB -> B_38_1801_0 = _rtP -> P_802 ; _rtB ->
B_38_1804_0 = _rtP -> P_804 ; _rtB -> B_38_1806_0 = _rtP -> P_806 ; _rtB ->
B_38_1808_0 = _rtP -> P_807 ; _rtB -> B_38_1813_0 = _rtP -> P_808 ; _rtB ->
B_38_1815_0 = _rtP -> P_809 ; _rtB -> B_38_1819_0 = _rtP -> P_810 ; _rtB ->
B_38_1822_0 = _rtP -> P_811 ; _rtB -> B_38_1824_0 = _rtP -> P_812 ; _rtB ->
B_38_1832_0 = _rtP -> P_818 ; _rtB -> B_38_1839_0 = _rtP -> P_821 ; _rtB ->
B_38_1840_0 = _rtB -> B_38_1839_0 - _rtP -> P_820 ; _rtB -> B_38_1843_0 =
_rtP -> P_822 ; _rtB -> B_38_1848_0 = _rtP -> P_824 ; _rtB -> B_38_1851_0 =
_rtP -> P_826 ; UNUSED_PARAMETER ( tid ) ; }
#define MDL_UPDATE
static void mdlUpdate ( SimStruct * S , int_T tid ) { int32_T isHit ;
B_nmm2DFullBody_T * _rtB ; P_nmm2DFullBody_T * _rtP ; DW_nmm2DFullBody_T *
_rtDW ; _rtDW = ( ( DW_nmm2DFullBody_T * ) ssGetRootDWork ( S ) ) ; _rtP = (
( P_nmm2DFullBody_T * ) ssGetModelRtp ( S ) ) ; _rtB = ( ( B_nmm2DFullBody_T
* ) _ssGetModelBlockIO ( S ) ) ; ssCallAccelRunBlock ( S , 38 , 1 ,
SS_CALL_MDL_UPDATE ) ; nmm2DFullBody_GroundFrictionModel_Update ( S , & _rtB
-> GroundFrictionModel_j , & _rtDW -> GroundFrictionModel_j ) ;
nmm2DFullBody_GroundFrictionModel_Update ( S , & _rtB ->
GroundFrictionModel_i , & _rtDW -> GroundFrictionModel_i ) ;
nmm2DFullBody_GroundFrictionModel_Update ( S , & _rtB -> GroundFrictionModel
, & _rtDW -> GroundFrictionModel ) ; nmm2DFullBody_GroundFrictionModel_Update
( S , & _rtB -> GroundFrictionModel_m , & _rtDW -> GroundFrictionModel_m ) ;
_rtDW -> Integrator_IWORK = 0 ; _rtDW -> Integrator_IWORK_j = 0 ; _rtDW ->
Integrator_IWORK_p = 0 ; _rtDW -> Integrator_IWORK_f = 0 ; _rtDW ->
Integrator_IWORK_i = 0 ; _rtDW -> Integrator_IWORK_o = 0 ; _rtDW ->
Integrator_IWORK_pr = 0 ; _rtDW -> Integrator_IWORK_g = 0 ; _rtDW ->
Integrator_IWORK_h = 0 ; _rtDW -> Integrator_IWORK_hf = 0 ; _rtDW ->
Integrator_IWORK_m = 0 ; _rtDW -> Integrator_IWORK_jn = 0 ; _rtDW ->
Integrator_IWORK_pk = 0 ; _rtDW -> Integrator_IWORK_fi = 0 ;
ssCallAccelRunBlock ( S , 38 , 658 , SS_CALL_MDL_UPDATE ) ;
ssCallAccelRunBlock ( S , 38 , 659 , SS_CALL_MDL_UPDATE ) ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> BodyPitchloopdelay_PWORK . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> BodyPitchloopdelay_PWORK
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
BodyPitchloopdelay_IWORK . Head = ( ( _rtDW -> BodyPitchloopdelay_IWORK .
Head < ( _rtDW -> BodyPitchloopdelay_IWORK . CircularBufSize - 1 ) ) ? (
_rtDW -> BodyPitchloopdelay_IWORK . Head + 1 ) : 0 ) ; if ( _rtDW ->
BodyPitchloopdelay_IWORK . Head == _rtDW -> BodyPitchloopdelay_IWORK . Tail )
{ if ( ! nmm2DFullBody_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
BodyPitchloopdelay_IWORK . CircularBufSize , & _rtDW ->
BodyPitchloopdelay_IWORK . Tail , & _rtDW -> BodyPitchloopdelay_IWORK . Head
, & _rtDW -> BodyPitchloopdelay_IWORK . Last , simTime - _rtP -> P_273 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
BodyPitchloopdelay_IWORK . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
BodyPitchloopdelay_IWORK . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
BodyPitchloopdelay_IWORK . Head ] = _rtB -> B_38_853_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> BodyPitchloopdelay_PWORK_j . TUbufferPtrs
[ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
BodyPitchloopdelay_PWORK_j . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S
) ; _rtDW -> BodyPitchloopdelay_IWORK_d . Head = ( ( _rtDW ->
BodyPitchloopdelay_IWORK_d . Head < ( _rtDW -> BodyPitchloopdelay_IWORK_d .
CircularBufSize - 1 ) ) ? ( _rtDW -> BodyPitchloopdelay_IWORK_d . Head + 1 )
: 0 ) ; if ( _rtDW -> BodyPitchloopdelay_IWORK_d . Head == _rtDW ->
BodyPitchloopdelay_IWORK_d . Tail ) { if ( !
nmm2DFullBody_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
BodyPitchloopdelay_IWORK_d . CircularBufSize , & _rtDW ->
BodyPitchloopdelay_IWORK_d . Tail , & _rtDW -> BodyPitchloopdelay_IWORK_d .
Head , & _rtDW -> BodyPitchloopdelay_IWORK_d . Last , simTime - _rtP -> P_275
, tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
BodyPitchloopdelay_IWORK_d . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
BodyPitchloopdelay_IWORK_d . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
BodyPitchloopdelay_IWORK_d . Head ] = _rtB -> B_38_852_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> LThighafferentdelay_PWORK . TUbufferPtrs
[ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
LThighafferentdelay_PWORK . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S
) ; _rtDW -> LThighafferentdelay_IWORK . Head = ( ( _rtDW ->
LThighafferentdelay_IWORK . Head < ( _rtDW -> LThighafferentdelay_IWORK .
CircularBufSize - 1 ) ) ? ( _rtDW -> LThighafferentdelay_IWORK . Head + 1 ) :
0 ) ; if ( _rtDW -> LThighafferentdelay_IWORK . Head == _rtDW ->
LThighafferentdelay_IWORK . Tail ) { if ( !
nmm2DFullBody_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
LThighafferentdelay_IWORK . CircularBufSize , & _rtDW ->
LThighafferentdelay_IWORK . Tail , & _rtDW -> LThighafferentdelay_IWORK .
Head , & _rtDW -> LThighafferentdelay_IWORK . Last , simTime - _rtP -> P_277
, tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
LThighafferentdelay_IWORK . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
LThighafferentdelay_IWORK . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
LThighafferentdelay_IWORK . Head ] = _rtB -> B_38_831_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> LStanceafferentdelay_PWORK . TUbufferPtrs
[ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
LStanceafferentdelay_PWORK . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S
) ; _rtDW -> LStanceafferentdelay_IWORK . Head = ( ( _rtDW ->
LStanceafferentdelay_IWORK . Head < ( _rtDW -> LStanceafferentdelay_IWORK .
CircularBufSize - 1 ) ) ? ( _rtDW -> LStanceafferentdelay_IWORK . Head + 1 )
: 0 ) ; if ( _rtDW -> LStanceafferentdelay_IWORK . Head == _rtDW ->
LStanceafferentdelay_IWORK . Tail ) { if ( !
nmm2DFullBody_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
LStanceafferentdelay_IWORK . CircularBufSize , & _rtDW ->
LStanceafferentdelay_IWORK . Tail , & _rtDW -> LStanceafferentdelay_IWORK .
Head , & _rtDW -> LStanceafferentdelay_IWORK . Last , simTime - _rtP -> P_279
, tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
LStanceafferentdelay_IWORK . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
LStanceafferentdelay_IWORK . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
LStanceafferentdelay_IWORK . Head ] = _rtB -> B_38_824_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> RStanceafferentdelay_PWORK . TUbufferPtrs
[ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
RStanceafferentdelay_PWORK . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S
) ; _rtDW -> RStanceafferentdelay_IWORK . Head = ( ( _rtDW ->
RStanceafferentdelay_IWORK . Head < ( _rtDW -> RStanceafferentdelay_IWORK .
CircularBufSize - 1 ) ) ? ( _rtDW -> RStanceafferentdelay_IWORK . Head + 1 )
: 0 ) ; if ( _rtDW -> RStanceafferentdelay_IWORK . Head == _rtDW ->
RStanceafferentdelay_IWORK . Tail ) { if ( !
nmm2DFullBody_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
RStanceafferentdelay_IWORK . CircularBufSize , & _rtDW ->
RStanceafferentdelay_IWORK . Tail , & _rtDW -> RStanceafferentdelay_IWORK .
Head , & _rtDW -> RStanceafferentdelay_IWORK . Last , simTime - _rtP -> P_281
, tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
RStanceafferentdelay_IWORK . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
RStanceafferentdelay_IWORK . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
RStanceafferentdelay_IWORK . Head ] = _rtB -> B_38_825_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput = _rtB -> B_38_691_0 ; _rtDW -> Memory_PreviousInput_i =
_rtB -> B_38_698_0 [ 0 ] ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
GLUFmloopdelay_PWORK . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * *
) & _rtDW -> GLUFmloopdelay_PWORK . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; _rtDW -> GLUFmloopdelay_IWORK . Head = ( ( _rtDW ->
GLUFmloopdelay_IWORK . Head < ( _rtDW -> GLUFmloopdelay_IWORK .
CircularBufSize - 1 ) ) ? ( _rtDW -> GLUFmloopdelay_IWORK . Head + 1 ) : 0 )
; if ( _rtDW -> GLUFmloopdelay_IWORK . Head == _rtDW -> GLUFmloopdelay_IWORK
. Tail ) { if ( ! nmm2DFullBody_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
GLUFmloopdelay_IWORK . CircularBufSize , & _rtDW -> GLUFmloopdelay_IWORK .
Tail , & _rtDW -> GLUFmloopdelay_IWORK . Head , & _rtDW ->
GLUFmloopdelay_IWORK . Last , simTime - _rtP -> P_288 , tBuffer , uBuffer , (
NULL ) , ( boolean_T ) 0 , false , & _rtDW -> GLUFmloopdelay_IWORK .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> GLUFmloopdelay_IWORK . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> GLUFmloopdelay_IWORK . Head ] = _rtB ->
B_38_1498_0 ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
HFLLceloopdelay_PWORK . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T *
* ) & _rtDW -> HFLLceloopdelay_PWORK . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; _rtDW -> HFLLceloopdelay_IWORK . Head = ( ( _rtDW ->
HFLLceloopdelay_IWORK . Head < ( _rtDW -> HFLLceloopdelay_IWORK .
CircularBufSize - 1 ) ) ? ( _rtDW -> HFLLceloopdelay_IWORK . Head + 1 ) : 0 )
; if ( _rtDW -> HFLLceloopdelay_IWORK . Head == _rtDW ->
HFLLceloopdelay_IWORK . Tail ) { if ( !
nmm2DFullBody_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
HFLLceloopdelay_IWORK . CircularBufSize , & _rtDW -> HFLLceloopdelay_IWORK .
Tail , & _rtDW -> HFLLceloopdelay_IWORK . Head , & _rtDW ->
HFLLceloopdelay_IWORK . Last , simTime - _rtP -> P_292 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> HFLLceloopdelay_IWORK .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> HFLLceloopdelay_IWORK . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> HFLLceloopdelay_IWORK . Head ] = _rtB ->
B_38_1640_0 ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
HAMLceloopdelay_PWORK . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T *
* ) & _rtDW -> HAMLceloopdelay_PWORK . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; _rtDW -> HAMLceloopdelay_IWORK . Head = ( ( _rtDW ->
HAMLceloopdelay_IWORK . Head < ( _rtDW -> HAMLceloopdelay_IWORK .
CircularBufSize - 1 ) ) ? ( _rtDW -> HAMLceloopdelay_IWORK . Head + 1 ) : 0 )
; if ( _rtDW -> HAMLceloopdelay_IWORK . Head == _rtDW ->
HAMLceloopdelay_IWORK . Tail ) { if ( !
nmm2DFullBody_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
HAMLceloopdelay_IWORK . CircularBufSize , & _rtDW -> HAMLceloopdelay_IWORK .
Tail , & _rtDW -> HAMLceloopdelay_IWORK . Head , & _rtDW ->
HAMLceloopdelay_IWORK . Last , simTime - _rtP -> P_294 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> HAMLceloopdelay_IWORK .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> HAMLceloopdelay_IWORK . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> HAMLceloopdelay_IWORK . Head ] = _rtB ->
B_38_1573_0 ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
HAMFmloopdelay_PWORK . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * *
) & _rtDW -> HAMFmloopdelay_PWORK . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; _rtDW -> HAMFmloopdelay_IWORK . Head = ( ( _rtDW ->
HAMFmloopdelay_IWORK . Head < ( _rtDW -> HAMFmloopdelay_IWORK .
CircularBufSize - 1 ) ) ? ( _rtDW -> HAMFmloopdelay_IWORK . Head + 1 ) : 0 )
; if ( _rtDW -> HAMFmloopdelay_IWORK . Head == _rtDW -> HAMFmloopdelay_IWORK
. Tail ) { if ( ! nmm2DFullBody_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
HAMFmloopdelay_IWORK . CircularBufSize , & _rtDW -> HAMFmloopdelay_IWORK .
Tail , & _rtDW -> HAMFmloopdelay_IWORK . Head , & _rtDW ->
HAMFmloopdelay_IWORK . Last , simTime - _rtP -> P_296 , tBuffer , uBuffer , (
NULL ) , ( boolean_T ) 0 , false , & _rtDW -> HAMFmloopdelay_IWORK .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> HAMFmloopdelay_IWORK . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> HAMFmloopdelay_IWORK . Head ] = _rtB ->
B_38_1570_0 ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
LKneeloopdelay_PWORK . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * *
) & _rtDW -> LKneeloopdelay_PWORK . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; _rtDW -> LKneeloopdelay_IWORK . Head = ( ( _rtDW ->
LKneeloopdelay_IWORK . Head < ( _rtDW -> LKneeloopdelay_IWORK .
CircularBufSize - 1 ) ) ? ( _rtDW -> LKneeloopdelay_IWORK . Head + 1 ) : 0 )
; if ( _rtDW -> LKneeloopdelay_IWORK . Head == _rtDW -> LKneeloopdelay_IWORK
. Tail ) { if ( ! nmm2DFullBody_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
LKneeloopdelay_IWORK . CircularBufSize , & _rtDW -> LKneeloopdelay_IWORK .
Tail , & _rtDW -> LKneeloopdelay_IWORK . Head , & _rtDW ->
LKneeloopdelay_IWORK . Last , simTime - _rtP -> P_300 , tBuffer , uBuffer , (
NULL ) , ( boolean_T ) 0 , false , & _rtDW -> LKneeloopdelay_IWORK .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> LKneeloopdelay_IWORK . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> LKneeloopdelay_IWORK . Head ] = _rtB ->
B_38_815_0 ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
RThighafferentdelay_PWORK . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> RThighafferentdelay_PWORK . TUbufferPtrs [ 1 ] ;
real_T simTime = ssGetT ( S ) ; _rtDW -> RThighafferentdelay_IWORK . Head = (
( _rtDW -> RThighafferentdelay_IWORK . Head < ( _rtDW ->
RThighafferentdelay_IWORK . CircularBufSize - 1 ) ) ? ( _rtDW ->
RThighafferentdelay_IWORK . Head + 1 ) : 0 ) ; if ( _rtDW ->
RThighafferentdelay_IWORK . Head == _rtDW -> RThighafferentdelay_IWORK . Tail
) { if ( ! nmm2DFullBody_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
RThighafferentdelay_IWORK . CircularBufSize , & _rtDW ->
RThighafferentdelay_IWORK . Tail , & _rtDW -> RThighafferentdelay_IWORK .
Head , & _rtDW -> RThighafferentdelay_IWORK . Last , simTime - _rtP -> P_302
, tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
RThighafferentdelay_IWORK . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
RThighafferentdelay_IWORK . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
RThighafferentdelay_IWORK . Head ] = _rtB -> B_38_832_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> VASloopdelay_PWORK . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> VASloopdelay_PWORK .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
VASloopdelay_IWORK . Head = ( ( _rtDW -> VASloopdelay_IWORK . Head < ( _rtDW
-> VASloopdelay_IWORK . CircularBufSize - 1 ) ) ? ( _rtDW ->
VASloopdelay_IWORK . Head + 1 ) : 0 ) ; if ( _rtDW -> VASloopdelay_IWORK .
Head == _rtDW -> VASloopdelay_IWORK . Tail ) { if ( !
nmm2DFullBody_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
VASloopdelay_IWORK . CircularBufSize , & _rtDW -> VASloopdelay_IWORK . Tail ,
& _rtDW -> VASloopdelay_IWORK . Head , & _rtDW -> VASloopdelay_IWORK . Last ,
simTime - _rtP -> P_304 , tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 ,
false , & _rtDW -> VASloopdelay_IWORK . MaxNewBufSize ) ) { ssSetErrorStatus
( S , "tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW
-> VASloopdelay_IWORK . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
VASloopdelay_IWORK . Head ] = _rtB -> B_38_1797_0 ; } { real_T * * uBuffer =
( real_T * * ) & _rtDW -> GASloopdelay_PWORK . TUbufferPtrs [ 0 ] ; real_T *
* tBuffer = ( real_T * * ) & _rtDW -> GASloopdelay_PWORK . TUbufferPtrs [ 1 ]
; real_T simTime = ssGetT ( S ) ; _rtDW -> GASloopdelay_IWORK . Head = ( (
_rtDW -> GASloopdelay_IWORK . Head < ( _rtDW -> GASloopdelay_IWORK .
CircularBufSize - 1 ) ) ? ( _rtDW -> GASloopdelay_IWORK . Head + 1 ) : 0 ) ;
if ( _rtDW -> GASloopdelay_IWORK . Head == _rtDW -> GASloopdelay_IWORK . Tail
) { if ( ! nmm2DFullBody_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
GASloopdelay_IWORK . CircularBufSize , & _rtDW -> GASloopdelay_IWORK . Tail ,
& _rtDW -> GASloopdelay_IWORK . Head , & _rtDW -> GASloopdelay_IWORK . Last ,
simTime - _rtP -> P_308 , tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 ,
false , & _rtDW -> GASloopdelay_IWORK . MaxNewBufSize ) ) { ssSetErrorStatus
( S , "tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW
-> GASloopdelay_IWORK . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
GASloopdelay_IWORK . Head ] = _rtB -> B_38_1432_0 ; } { real_T * * uBuffer =
( real_T * * ) & _rtDW -> SOLloopdelay_PWORK . TUbufferPtrs [ 0 ] ; real_T *
* tBuffer = ( real_T * * ) & _rtDW -> SOLloopdelay_PWORK . TUbufferPtrs [ 1 ]
; real_T simTime = ssGetT ( S ) ; _rtDW -> SOLloopdelay_IWORK . Head = ( (
_rtDW -> SOLloopdelay_IWORK . Head < ( _rtDW -> SOLloopdelay_IWORK .
CircularBufSize - 1 ) ) ? ( _rtDW -> SOLloopdelay_IWORK . Head + 1 ) : 0 ) ;
if ( _rtDW -> SOLloopdelay_IWORK . Head == _rtDW -> SOLloopdelay_IWORK . Tail
) { if ( ! nmm2DFullBody_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
SOLloopdelay_IWORK . CircularBufSize , & _rtDW -> SOLloopdelay_IWORK . Tail ,
& _rtDW -> SOLloopdelay_IWORK . Head , & _rtDW -> SOLloopdelay_IWORK . Last ,
simTime - _rtP -> P_312 , tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 ,
false , & _rtDW -> SOLloopdelay_IWORK . MaxNewBufSize ) ) { ssSetErrorStatus
( S , "tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW
-> SOLloopdelay_IWORK . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
SOLloopdelay_IWORK . Head ] = _rtB -> B_38_1718_0 ; } { real_T * * uBuffer =
( real_T * * ) & _rtDW -> TAloopdelay_PWORK . TUbufferPtrs [ 0 ] ; real_T * *
tBuffer = ( real_T * * ) & _rtDW -> TAloopdelay_PWORK . TUbufferPtrs [ 1 ] ;
real_T simTime = ssGetT ( S ) ; _rtDW -> TAloopdelay_IWORK . Head = ( ( _rtDW
-> TAloopdelay_IWORK . Head < ( _rtDW -> TAloopdelay_IWORK . CircularBufSize
- 1 ) ) ? ( _rtDW -> TAloopdelay_IWORK . Head + 1 ) : 0 ) ; if ( _rtDW ->
TAloopdelay_IWORK . Head == _rtDW -> TAloopdelay_IWORK . Tail ) { if ( !
nmm2DFullBody_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW -> TAloopdelay_IWORK
. CircularBufSize , & _rtDW -> TAloopdelay_IWORK . Tail , & _rtDW ->
TAloopdelay_IWORK . Head , & _rtDW -> TAloopdelay_IWORK . Last , simTime -
_rtP -> P_314 , tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , &
_rtDW -> TAloopdelay_IWORK . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TAloopdelay_IWORK . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TAloopdelay_IWORK . Head ] = _rtB -> B_38_1734_0 ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW -> Memory3_PreviousInput = _rtB ->
B_38_690_0 ; _rtDW -> Memory_PreviousInput_a = _rtB -> B_38_751_0 [ 0 ] ; } {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> GLUFmloopdelay_PWORK_g .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
GLUFmloopdelay_PWORK_g . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ;
_rtDW -> GLUFmloopdelay_IWORK_m . Head = ( ( _rtDW -> GLUFmloopdelay_IWORK_m
. Head < ( _rtDW -> GLUFmloopdelay_IWORK_m . CircularBufSize - 1 ) ) ? (
_rtDW -> GLUFmloopdelay_IWORK_m . Head + 1 ) : 0 ) ; if ( _rtDW ->
GLUFmloopdelay_IWORK_m . Head == _rtDW -> GLUFmloopdelay_IWORK_m . Tail ) {
if ( ! nmm2DFullBody_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
GLUFmloopdelay_IWORK_m . CircularBufSize , & _rtDW -> GLUFmloopdelay_IWORK_m
. Tail , & _rtDW -> GLUFmloopdelay_IWORK_m . Head , & _rtDW ->
GLUFmloopdelay_IWORK_m . Last , simTime - _rtP -> P_322 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> GLUFmloopdelay_IWORK_m .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> GLUFmloopdelay_IWORK_m . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> GLUFmloopdelay_IWORK_m . Head ] = _rtB ->
B_38_999_0 ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
HFLLceloopdelay_PWORK_g . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> HFLLceloopdelay_PWORK_g . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; _rtDW -> HFLLceloopdelay_IWORK_p . Head = ( ( _rtDW
-> HFLLceloopdelay_IWORK_p . Head < ( _rtDW -> HFLLceloopdelay_IWORK_p .
CircularBufSize - 1 ) ) ? ( _rtDW -> HFLLceloopdelay_IWORK_p . Head + 1 ) : 0
) ; if ( _rtDW -> HFLLceloopdelay_IWORK_p . Head == _rtDW ->
HFLLceloopdelay_IWORK_p . Tail ) { if ( !
nmm2DFullBody_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
HFLLceloopdelay_IWORK_p . CircularBufSize , & _rtDW ->
HFLLceloopdelay_IWORK_p . Tail , & _rtDW -> HFLLceloopdelay_IWORK_p . Head ,
& _rtDW -> HFLLceloopdelay_IWORK_p . Last , simTime - _rtP -> P_326 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
HFLLceloopdelay_IWORK_p . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
HFLLceloopdelay_IWORK_p . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
HFLLceloopdelay_IWORK_p . Head ] = _rtB -> B_38_1143_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> HAMLceloopdelay_PWORK_d . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> HAMLceloopdelay_PWORK_d
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
HAMLceloopdelay_IWORK_e . Head = ( ( _rtDW -> HAMLceloopdelay_IWORK_e . Head
< ( _rtDW -> HAMLceloopdelay_IWORK_e . CircularBufSize - 1 ) ) ? ( _rtDW ->
HAMLceloopdelay_IWORK_e . Head + 1 ) : 0 ) ; if ( _rtDW ->
HAMLceloopdelay_IWORK_e . Head == _rtDW -> HAMLceloopdelay_IWORK_e . Tail ) {
if ( ! nmm2DFullBody_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
HAMLceloopdelay_IWORK_e . CircularBufSize , & _rtDW ->
HAMLceloopdelay_IWORK_e . Tail , & _rtDW -> HAMLceloopdelay_IWORK_e . Head ,
& _rtDW -> HAMLceloopdelay_IWORK_e . Last , simTime - _rtP -> P_328 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
HAMLceloopdelay_IWORK_e . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
HAMLceloopdelay_IWORK_e . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
HAMLceloopdelay_IWORK_e . Head ] = _rtB -> B_38_1076_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> HAMFmloopdelay_PWORK_h . TUbufferPtrs [ 0
] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> HAMFmloopdelay_PWORK_h .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
HAMFmloopdelay_IWORK_l . Head = ( ( _rtDW -> HAMFmloopdelay_IWORK_l . Head <
( _rtDW -> HAMFmloopdelay_IWORK_l . CircularBufSize - 1 ) ) ? ( _rtDW ->
HAMFmloopdelay_IWORK_l . Head + 1 ) : 0 ) ; if ( _rtDW ->
HAMFmloopdelay_IWORK_l . Head == _rtDW -> HAMFmloopdelay_IWORK_l . Tail ) {
if ( ! nmm2DFullBody_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
HAMFmloopdelay_IWORK_l . CircularBufSize , & _rtDW -> HAMFmloopdelay_IWORK_l
. Tail , & _rtDW -> HAMFmloopdelay_IWORK_l . Head , & _rtDW ->
HAMFmloopdelay_IWORK_l . Last , simTime - _rtP -> P_330 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> HAMFmloopdelay_IWORK_l .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> HAMFmloopdelay_IWORK_l . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> HAMFmloopdelay_IWORK_l . Head ] = _rtB ->
B_38_1073_0 ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
RKneeloopdelay_PWORK . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * *
) & _rtDW -> RKneeloopdelay_PWORK . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; _rtDW -> RKneeloopdelay_IWORK . Head = ( ( _rtDW ->
RKneeloopdelay_IWORK . Head < ( _rtDW -> RKneeloopdelay_IWORK .
CircularBufSize - 1 ) ) ? ( _rtDW -> RKneeloopdelay_IWORK . Head + 1 ) : 0 )
; if ( _rtDW -> RKneeloopdelay_IWORK . Head == _rtDW -> RKneeloopdelay_IWORK
. Tail ) { if ( ! nmm2DFullBody_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
RKneeloopdelay_IWORK . CircularBufSize , & _rtDW -> RKneeloopdelay_IWORK .
Tail , & _rtDW -> RKneeloopdelay_IWORK . Head , & _rtDW ->
RKneeloopdelay_IWORK . Last , simTime - _rtP -> P_334 , tBuffer , uBuffer , (
NULL ) , ( boolean_T ) 0 , false , & _rtDW -> RKneeloopdelay_IWORK .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> RKneeloopdelay_IWORK . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> RKneeloopdelay_IWORK . Head ] = _rtB ->
B_38_814_0 ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
VASloopdelay_PWORK_o . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * *
) & _rtDW -> VASloopdelay_PWORK_o . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; _rtDW -> VASloopdelay_IWORK_p . Head = ( ( _rtDW ->
VASloopdelay_IWORK_p . Head < ( _rtDW -> VASloopdelay_IWORK_p .
CircularBufSize - 1 ) ) ? ( _rtDW -> VASloopdelay_IWORK_p . Head + 1 ) : 0 )
; if ( _rtDW -> VASloopdelay_IWORK_p . Head == _rtDW -> VASloopdelay_IWORK_p
. Tail ) { if ( ! nmm2DFullBody_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
VASloopdelay_IWORK_p . CircularBufSize , & _rtDW -> VASloopdelay_IWORK_p .
Tail , & _rtDW -> VASloopdelay_IWORK_p . Head , & _rtDW ->
VASloopdelay_IWORK_p . Last , simTime - _rtP -> P_336 , tBuffer , uBuffer , (
NULL ) , ( boolean_T ) 0 , false , & _rtDW -> VASloopdelay_IWORK_p .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> VASloopdelay_IWORK_p . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> VASloopdelay_IWORK_p . Head ] = _rtB ->
B_38_1286_0 ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
GASloopdelay_PWORK_f . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * *
) & _rtDW -> GASloopdelay_PWORK_f . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; _rtDW -> GASloopdelay_IWORK_j . Head = ( ( _rtDW ->
GASloopdelay_IWORK_j . Head < ( _rtDW -> GASloopdelay_IWORK_j .
CircularBufSize - 1 ) ) ? ( _rtDW -> GASloopdelay_IWORK_j . Head + 1 ) : 0 )
; if ( _rtDW -> GASloopdelay_IWORK_j . Head == _rtDW -> GASloopdelay_IWORK_j
. Tail ) { if ( ! nmm2DFullBody_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
GASloopdelay_IWORK_j . CircularBufSize , & _rtDW -> GASloopdelay_IWORK_j .
Tail , & _rtDW -> GASloopdelay_IWORK_j . Head , & _rtDW ->
GASloopdelay_IWORK_j . Last , simTime - _rtP -> P_340 , tBuffer , uBuffer , (
NULL ) , ( boolean_T ) 0 , false , & _rtDW -> GASloopdelay_IWORK_j .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> GASloopdelay_IWORK_j . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> GASloopdelay_IWORK_j . Head ] = _rtB ->
B_38_933_0 ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
SOLloopdelay_PWORK_c . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * *
) & _rtDW -> SOLloopdelay_PWORK_c . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; _rtDW -> SOLloopdelay_IWORK_a . Head = ( ( _rtDW ->
SOLloopdelay_IWORK_a . Head < ( _rtDW -> SOLloopdelay_IWORK_a .
CircularBufSize - 1 ) ) ? ( _rtDW -> SOLloopdelay_IWORK_a . Head + 1 ) : 0 )
; if ( _rtDW -> SOLloopdelay_IWORK_a . Head == _rtDW -> SOLloopdelay_IWORK_a
. Tail ) { if ( ! nmm2DFullBody_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
SOLloopdelay_IWORK_a . CircularBufSize , & _rtDW -> SOLloopdelay_IWORK_a .
Tail , & _rtDW -> SOLloopdelay_IWORK_a . Head , & _rtDW ->
SOLloopdelay_IWORK_a . Last , simTime - _rtP -> P_344 , tBuffer , uBuffer , (
NULL ) , ( boolean_T ) 0 , false , & _rtDW -> SOLloopdelay_IWORK_a .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> SOLloopdelay_IWORK_a . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> SOLloopdelay_IWORK_a . Head ] = _rtB ->
B_38_1207_0 ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TAloopdelay_PWORK_d . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * *
) & _rtDW -> TAloopdelay_PWORK_d . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; _rtDW -> TAloopdelay_IWORK_o . Head = ( ( _rtDW ->
TAloopdelay_IWORK_o . Head < ( _rtDW -> TAloopdelay_IWORK_o . CircularBufSize
- 1 ) ) ? ( _rtDW -> TAloopdelay_IWORK_o . Head + 1 ) : 0 ) ; if ( _rtDW ->
TAloopdelay_IWORK_o . Head == _rtDW -> TAloopdelay_IWORK_o . Tail ) { if ( !
nmm2DFullBody_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TAloopdelay_IWORK_o . CircularBufSize , & _rtDW -> TAloopdelay_IWORK_o . Tail
, & _rtDW -> TAloopdelay_IWORK_o . Head , & _rtDW -> TAloopdelay_IWORK_o .
Last , simTime - _rtP -> P_346 , tBuffer , uBuffer , ( NULL ) , ( boolean_T )
0 , false , & _rtDW -> TAloopdelay_IWORK_o . MaxNewBufSize ) ) {
ssSetErrorStatus ( S , "tdelay memory allocation error" ) ; return ; } } ( *
tBuffer ) [ _rtDW -> TAloopdelay_IWORK_o . Head ] = simTime ; ( * uBuffer ) [
_rtDW -> TAloopdelay_IWORK_o . Head ] = _rtB -> B_38_1223_0 ; }
UNUSED_PARAMETER ( tid ) ; }
#define MDL_UPDATE
static void mdlUpdateTID4 ( SimStruct * S , int_T tid ) { UNUSED_PARAMETER (
tid ) ; }
#define MDL_DERIVATIVES
static void mdlDerivatives ( SimStruct * S ) { B_nmm2DFullBody_T * _rtB ;
P_nmm2DFullBody_T * _rtP ; X_nmm2DFullBody_T * _rtX ; XDot_nmm2DFullBody_T *
_rtXdot ; DW_nmm2DFullBody_T * _rtDW ; _rtDW = ( ( DW_nmm2DFullBody_T * )
ssGetRootDWork ( S ) ) ; _rtXdot = ( ( XDot_nmm2DFullBody_T * ) ssGetdX ( S )
) ; _rtX = ( ( X_nmm2DFullBody_T * ) ssGetContStates ( S ) ) ; _rtP = ( (
P_nmm2DFullBody_T * ) ssGetModelRtp ( S ) ) ; _rtB = ( ( B_nmm2DFullBody_T *
) _ssGetModelBlockIO ( S ) ) ; ssCallAccelRunBlock ( S , 38 , 1 ,
SS_CALL_MDL_DERIVATIVES ) ; _rtXdot -> Integrator_CSTATE = _rtB -> B_38_929_0
; _rtXdot -> Integrator_CSTATE_m = _rtB -> B_38_1279_0 ; _rtXdot ->
Integrator_CSTATE_f = _rtB -> B_38_1203_0 ; _rtXdot -> Integrator_CSTATE_k =
_rtB -> B_38_1069_0 ; _rtXdot -> Integrator_CSTATE_b = _rtB -> B_38_1345_0 ;
_rtXdot -> Integrator_CSTATE_p = _rtB -> B_38_995_0 ; _rtXdot ->
Integrator_CSTATE_j = _rtB -> B_38_1134_0 ; _rtXdot -> Integrator_CSTATE_js =
_rtB -> B_38_1494_0 ; _rtXdot -> Integrator_CSTATE_h = _rtB -> B_38_1631_0 ;
_rtXdot -> Integrator_CSTATE_i = _rtB -> B_38_1566_0 ; _rtXdot ->
Integrator_CSTATE_hj = _rtB -> B_38_1856_0 ; _rtXdot -> Integrator_CSTATE_n =
_rtB -> B_38_1428_0 ; _rtXdot -> Integrator_CSTATE_bt = _rtB -> B_38_1790_0 ;
_rtXdot -> Integrator_CSTATE_be = _rtB -> B_38_1714_0 ; _rtXdot ->
smooth_CSTATE = 0.0 ; _rtXdot -> smooth_CSTATE += _rtP -> P_352 * _rtX ->
smooth_CSTATE ; _rtXdot -> smooth_CSTATE += _rtB -> B_38_69_0 ; _rtXdot ->
smooth1_CSTATE = 0.0 ; _rtXdot -> smooth1_CSTATE += _rtP -> P_354 * _rtX ->
smooth1_CSTATE ; _rtXdot -> smooth1_CSTATE += _rtB -> B_38_81_0 ; _rtXdot ->
StateSpace_CSTATE = 0.0 ; _rtXdot -> StateSpace_CSTATE += _rtP -> P_364 *
_rtX -> StateSpace_CSTATE ; _rtXdot -> StateSpace_CSTATE += _rtP -> P_365 *
_rtB -> B_38_936_0 ; _rtXdot -> StateSpace_CSTATE_f = 0.0 ; _rtXdot ->
StateSpace_CSTATE_f += _rtP -> P_396 * _rtX -> StateSpace_CSTATE_f ; _rtXdot
-> StateSpace_CSTATE_f += _rtP -> P_397 * _rtB -> B_38_1002_0 ; _rtXdot ->
StateSpace_CSTATE_a = 0.0 ; _rtXdot -> StateSpace_CSTATE_a += _rtP -> P_433 *
_rtX -> StateSpace_CSTATE_a ; _rtXdot -> StateSpace_CSTATE_a += _rtP -> P_434
* _rtB -> B_38_1079_0 ; _rtXdot -> StateSpace_CSTATE_m = 0.0 ; _rtXdot ->
StateSpace_CSTATE_m += _rtP -> P_466 * _rtX -> StateSpace_CSTATE_m ; _rtXdot
-> StateSpace_CSTATE_m += _rtP -> P_467 * _rtB -> B_38_1146_0 ; _rtXdot ->
StateSpace_CSTATE_e = 0.0 ; _rtXdot -> StateSpace_CSTATE_e += _rtP -> P_500 *
_rtX -> StateSpace_CSTATE_e ; _rtXdot -> StateSpace_CSTATE_e += _rtP -> P_501
* _rtB -> B_38_1210_0 ; _rtXdot -> StateSpace_CSTATE_ai = 0.0 ; _rtXdot ->
StateSpace_CSTATE_ai += _rtP -> P_545 * _rtX -> StateSpace_CSTATE_ai ;
_rtXdot -> StateSpace_CSTATE_ai += _rtP -> P_546 * _rtB -> B_38_1226_0 ;
_rtXdot -> StateSpace_CSTATE_h = 0.0 ; _rtXdot -> StateSpace_CSTATE_h += _rtP
-> P_577 * _rtX -> StateSpace_CSTATE_h ; _rtXdot -> StateSpace_CSTATE_h +=
_rtP -> P_578 * _rtB -> B_38_1289_0 ; _rtXdot -> StateSpace_CSTATE_i = 0.0 ;
_rtXdot -> StateSpace_CSTATE_i += _rtP -> P_599 * _rtX -> StateSpace_CSTATE_i
; _rtXdot -> StateSpace_CSTATE_i += _rtP -> P_600 * _rtB -> B_38_1435_0 ;
_rtXdot -> StateSpace_CSTATE_ml = 0.0 ; _rtXdot -> StateSpace_CSTATE_ml +=
_rtP -> P_631 * _rtX -> StateSpace_CSTATE_ml ; _rtXdot ->
StateSpace_CSTATE_ml += _rtP -> P_632 * _rtB -> B_38_1501_0 ; _rtXdot ->
StateSpace_CSTATE_p = 0.0 ; _rtXdot -> StateSpace_CSTATE_p += _rtP -> P_666 *
_rtX -> StateSpace_CSTATE_p ; _rtXdot -> StateSpace_CSTATE_p += _rtP -> P_667
* _rtB -> B_38_1576_0 ; _rtXdot -> StateSpace_CSTATE_po = 0.0 ; _rtXdot ->
StateSpace_CSTATE_po += _rtP -> P_699 * _rtX -> StateSpace_CSTATE_po ;
_rtXdot -> StateSpace_CSTATE_po += _rtP -> P_700 * _rtB -> B_38_1643_0 ;
_rtXdot -> StateSpace_CSTATE_fw = 0.0 ; _rtXdot -> StateSpace_CSTATE_fw +=
_rtP -> P_737 * _rtX -> StateSpace_CSTATE_fw ; _rtXdot ->
StateSpace_CSTATE_fw += _rtP -> P_738 * _rtB -> B_38_1721_0 ; _rtXdot ->
StateSpace_CSTATE_h0 = 0.0 ; _rtXdot -> StateSpace_CSTATE_h0 += _rtP -> P_782
* _rtX -> StateSpace_CSTATE_h0 ; _rtXdot -> StateSpace_CSTATE_h0 += _rtP ->
P_783 * _rtB -> B_38_1737_0 ; _rtXdot -> StateSpace_CSTATE_d = 0.0 ; _rtXdot
-> StateSpace_CSTATE_d += _rtP -> P_814 * _rtX -> StateSpace_CSTATE_d ;
_rtXdot -> StateSpace_CSTATE_d += _rtP -> P_815 * _rtB -> B_38_1800_0 ; }
#define MDL_PROJECTION
static void mdlProjection ( SimStruct * S ) { B_nmm2DFullBody_T * _rtB ;
P_nmm2DFullBody_T * _rtP ; DW_nmm2DFullBody_T * _rtDW ; _rtDW = ( (
DW_nmm2DFullBody_T * ) ssGetRootDWork ( S ) ) ; _rtP = ( ( P_nmm2DFullBody_T
* ) ssGetModelRtp ( S ) ) ; _rtB = ( ( B_nmm2DFullBody_T * )
_ssGetModelBlockIO ( S ) ) ; ssCallAccelRunBlock ( S , 38 , 1 ,
SS_CALL_MDL_PROJECTION ) ; }
#define MDL_ZERO_CROSSINGS
static void mdlZeroCrossings ( SimStruct * S ) { B_nmm2DFullBody_T * _rtB ;
P_nmm2DFullBody_T * _rtP ; ZCV_nmm2DFullBody_T * _rtZCSV ; DW_nmm2DFullBody_T
* _rtDW ; _rtDW = ( ( DW_nmm2DFullBody_T * ) ssGetRootDWork ( S ) ) ; _rtZCSV
= ( ( ZCV_nmm2DFullBody_T * ) ssGetSolverZcSignalVector ( S ) ) ; _rtP = ( (
P_nmm2DFullBody_T * ) ssGetModelRtp ( S ) ) ; _rtB = ( ( B_nmm2DFullBody_T *
) _ssGetModelBlockIO ( S ) ) ; _rtZCSV -> Compare_RelopInput_ZC = _rtB ->
B_38_3_0 - nmm2DFullBody_rtC ( S ) -> B_38_5_0 ;
nmm2DFullBody_GroundFrictionModel_ZC ( S , _rtB -> B_38_4_0 [ 0 ] , & _rtB ->
GroundFrictionModel_j , & nmm2DFullBody_rtC ( S ) -> GroundFrictionModel_j ,
& _rtDW -> GroundFrictionModel_j , & _rtZCSV -> GroundFrictionModel_j ) ;
_rtZCSV -> Compare_RelopInput_ZC_e = _rtB -> B_38_10_0 - nmm2DFullBody_rtC (
S ) -> B_38_12_0 ; nmm2DFullBody_GroundFrictionModel_ZC ( S , _rtB ->
B_38_11_0 [ 0 ] , & _rtB -> GroundFrictionModel_i , & nmm2DFullBody_rtC ( S )
-> GroundFrictionModel_i , & _rtDW -> GroundFrictionModel_i , & _rtZCSV ->
GroundFrictionModel_i ) ; _rtZCSV -> Compare_RelopInput_ZC_o = _rtB ->
B_38_36_0 - nmm2DFullBody_rtC ( S ) -> B_38_38_0 ;
nmm2DFullBody_GroundFrictionModel_ZC ( S , _rtB -> B_38_37_0 [ 0 ] , & _rtB
-> GroundFrictionModel , & nmm2DFullBody_rtC ( S ) -> GroundFrictionModel , &
_rtDW -> GroundFrictionModel , & _rtZCSV -> GroundFrictionModel ) ; _rtZCSV
-> Compare_RelopInput_ZC_f = _rtB -> B_38_43_0 - nmm2DFullBody_rtC ( S ) ->
B_38_45_0 ; nmm2DFullBody_GroundFrictionModel_ZC ( S , _rtB -> B_38_44_0 [ 0
] , & _rtB -> GroundFrictionModel_m , & nmm2DFullBody_rtC ( S ) ->
GroundFrictionModel_m , & _rtDW -> GroundFrictionModel_m , & _rtZCSV ->
GroundFrictionModel_m ) ; _rtZCSV -> Switch_SwitchCond_ZC = _rtB ->
B_38_179_0 - _rtP -> P_75 ; _rtZCSV -> Switch_SwitchCond_ZC_h = _rtB ->
B_38_209_0 - _rtP -> P_87 ; _rtZCSV -> Switch_SwitchCond_ZC_o = _rtB ->
B_38_240_0 - _rtP -> P_100 ; _rtZCSV -> Switch_SwitchCond_ZC_hq = _rtB ->
B_38_287_0 - _rtP -> P_121 ; _rtZCSV -> Switch_SwitchCond_ZC_b = _rtB ->
B_38_317_0 - _rtP -> P_133 ; _rtZCSV -> Switch_SwitchCond_ZC_j = _rtB ->
B_38_357_0 - _rtP -> P_150 ; _rtZCSV -> Switch_SwitchCond_ZC_oi = _rtB ->
B_38_382_0 - _rtP -> P_161 ; _rtZCSV -> Switch_SwitchCond_ZC_js = _rtB ->
B_38_421_0 - _rtP -> P_177 ; _rtZCSV -> Switch_SwitchCond_ZC_k = _rtB ->
B_38_446_0 - _rtP -> P_188 ; _rtZCSV -> Switch_SwitchCond_ZC_p = _rtB ->
B_38_482_0 - _rtP -> P_204 ; _rtZCSV -> Switch_SwitchCond_ZC_kd = _rtB ->
B_38_525_0 - _rtP -> P_220 ; _rtZCSV -> Switch_SwitchCond_ZC_f = _rtB ->
B_38_566_0 - _rtP -> P_237 ; _rtZCSV -> Switch_SwitchCond_ZC_ji = _rtB ->
B_38_615_0 - _rtP -> P_256 ; _rtZCSV -> Switch_SwitchCond_ZC_d = _rtB ->
B_38_646_0 - _rtP -> P_269 ; nmm2DFullBody_StancePhase_g_ZC ( S , _rtB ->
B_38_690_0 , & _rtDW -> StancePhase_i , & _rtZCSV -> StancePhase_i ) ;
nmm2DFullBody_StancePhase_f_ZC ( S , _rtB -> B_38_690_0 , & _rtDW ->
StancePhase_o , & _rtZCSV -> StancePhase_o ) ;
nmm2DFullBody_StancePhase_ga_ZC ( S , _rtB -> B_38_690_0 , & _rtDW ->
StancePhase_g , & _rtZCSV -> StancePhase_g ) ; nmm2DFullBody_StancePhase_ZC (
S , _rtB -> B_38_690_0 , & _rtDW -> StancePhase , & _rtZCSV -> StancePhase )
; nmm2DFullBody_StancePhase_b_ZC ( S , _rtB -> B_38_690_0 , & _rtDW ->
StancePhase_l , & _rtZCSV -> StancePhase_l ) ; nmm2DFullBody_StancePhase_g_ZC
( S , _rtB -> B_38_691_0 , & _rtDW -> StancePhase_k , & _rtZCSV ->
StancePhase_k ) ; nmm2DFullBody_StancePhase_f_ZC ( S , _rtB -> B_38_691_0 , &
_rtDW -> StancePhase_j , & _rtZCSV -> StancePhase_j ) ;
nmm2DFullBody_StancePhase_ga_ZC ( S , _rtB -> B_38_691_0 , & _rtDW ->
StancePhase_b , & _rtZCSV -> StancePhase_b ) ; nmm2DFullBody_StancePhase_ZC (
S , _rtB -> B_38_691_0 , & _rtDW -> StancePhase_a , & _rtZCSV ->
StancePhase_a ) ; nmm2DFullBody_StancePhase_b_ZC ( S , _rtB -> B_38_691_0 , &
_rtDW -> StancePhase_n , & _rtZCSV -> StancePhase_n ) ; _rtZCSV ->
Abs1_AbsZc_ZC = _rtB -> B_38_92_0 [ 1 ] ; _rtZCSV -> Compare_RelopInput_ZC_h
= _rtB -> B_38_834_0 - _rtB -> B_38_835_0 ; _rtZCSV -> Abs_AbsZc_ZC = _rtB ->
B_38_880_0 ; _rtZCSV -> Switch_SwitchCond_ZC_bd = _rtB -> B_38_930_0 - _rtP
-> P_383 ; _rtZCSV -> Switch_SwitchCond_ZC_dk = _rtB -> B_38_935_0 - _rtP ->
P_386 ; _rtZCSV -> Switch_SwitchCond_ZC_pi = _rtB -> B_38_938_0 - _rtP ->
P_388 ; _rtZCSV -> Switch_SwitchCond_ZC_e = _rtB -> B_38_941_0 - _rtP ->
P_390 ; _rtZCSV -> Abs_AbsZc_ZC_f = _rtB -> B_38_946_0 ; _rtZCSV ->
Switch_SwitchCond_ZC_ej = _rtB -> B_38_996_0 - _rtP -> P_415 ; _rtZCSV ->
Switch_SwitchCond_ZC_du = _rtB -> B_38_1001_0 - _rtP -> P_418 ; _rtZCSV ->
Switch_SwitchCond_ZC_i = _rtB -> B_38_1004_0 - _rtP -> P_420 ; _rtZCSV ->
Switch_SwitchCond_ZC_a = _rtB -> B_38_1007_0 - _rtP -> P_422 ; _rtZCSV ->
Switch_SwitchCond_ZC_n = _rtB -> B_38_1014_0 - _rtP -> P_426 ; _rtZCSV ->
Abs_AbsZc_ZC_e = _rtB -> B_38_1020_0 ; _rtZCSV -> Switch_SwitchCond_ZC_l =
_rtB -> B_38_1070_0 - _rtP -> P_452 ; _rtZCSV -> Switch_SwitchCond_ZC_nr =
_rtB -> B_38_1078_0 - _rtP -> P_458 ; _rtZCSV -> Switch_SwitchCond_ZC_et =
_rtB -> B_38_1081_0 - _rtP -> P_460 ; _rtZCSV -> Abs_AbsZc_ZC_a = _rtB ->
B_38_1085_0 ; _rtZCSV -> Switch_SwitchCond_ZC_fa = _rtB -> B_38_1137_0 - _rtP
-> P_486 ; _rtZCSV -> Switch_SwitchCond_ZC_im = _rtB -> B_38_1145_0 - _rtP ->
P_492 ; _rtZCSV -> Switch_SwitchCond_ZC_c = _rtB -> B_38_1148_0 - _rtP ->
P_494 ; _rtZCSV -> Abs_AbsZc_ZC_fc = _rtB -> B_38_1154_0 ; _rtZCSV ->
Switch_SwitchCond_ZC_pt = _rtB -> B_38_1204_0 - _rtP -> P_519 ; _rtZCSV ->
Switch_SwitchCond_ZC_m = _rtB -> B_38_1209_0 - _rtP -> P_522 ; _rtZCSV ->
Switch_SwitchCond_ZC_a5 = _rtB -> B_38_1212_0 - _rtP -> P_524 ; _rtZCSV ->
Switch_SwitchCond_ZC_ny = _rtB -> B_38_1215_0 - _rtP -> P_526 ; _rtZCSV ->
Switch_SwitchCond_ZC_g = _rtB -> B_38_1217_0 - _rtP -> P_528 ; _rtZCSV ->
Switch_SwitchCond_ZC_dq = _rtB -> B_38_1225_0 - _rtP -> P_534 ; _rtZCSV ->
Abs_AbsZc_ZC_h = _rtB -> B_38_1243_0 ; _rtZCSV -> Switch_SwitchCond_ZC_cw =
_rtB -> B_38_1280_0 - _rtP -> P_559 ; _rtZCSV -> Switch_SwitchCond_ZC_hp =
_rtB -> B_38_1283_0 - _rtP -> P_561 ; _rtZCSV -> Switch_SwitchCond_ZC_jk =
_rtB -> B_38_1288_0 - _rtP -> P_564 ; _rtZCSV -> Switch_SwitchCond_ZC_kk =
_rtB -> B_38_1291_0 - _rtP -> P_566 ; _rtZCSV -> Abs_AbsZc_ZC_ee = _rtB ->
B_38_1309_0 ; _rtZCSV -> Switch_SwitchCond_ZC_hb = _rtB -> B_38_1346_0 - _rtP
-> P_591 ; _rtZCSV -> Abs_AbsZc_ZC_ag = _rtB -> B_38_1379_0 ; _rtZCSV ->
Switch_SwitchCond_ZC_kz = _rtB -> B_38_1429_0 - _rtP -> P_618 ; _rtZCSV ->
Switch_SwitchCond_ZC_img = _rtB -> B_38_1434_0 - _rtP -> P_621 ; _rtZCSV ->
Switch_SwitchCond_ZC_d4 = _rtB -> B_38_1437_0 - _rtP -> P_623 ; _rtZCSV ->
Switch_SwitchCond_ZC_kj = _rtB -> B_38_1440_0 - _rtP -> P_625 ; _rtZCSV ->
Abs_AbsZc_ZC_a5 = _rtB -> B_38_1445_0 ; _rtZCSV -> Switch_SwitchCond_ZC_ci =
_rtB -> B_38_1495_0 - _rtP -> P_650 ; _rtZCSV -> Switch_SwitchCond_ZC_m1 =
_rtB -> B_38_1500_0 - _rtP -> P_653 ; _rtZCSV -> Switch_SwitchCond_ZC_mp =
_rtB -> B_38_1503_0 - _rtP -> P_655 ; _rtZCSV -> Switch_SwitchCond_ZC_gs =
_rtB -> B_38_1506_0 - _rtP -> P_657 ; _rtZCSV -> Switch_SwitchCond_ZC_av =
_rtB -> B_38_1511_0 - _rtP -> P_659 ; _rtZCSV -> Abs_AbsZc_ZC_d = _rtB ->
B_38_1517_0 ; _rtZCSV -> Switch_SwitchCond_ZC_mg = _rtB -> B_38_1567_0 - _rtP
-> P_685 ; _rtZCSV -> Switch_SwitchCond_ZC_np = _rtB -> B_38_1575_0 - _rtP ->
P_691 ; _rtZCSV -> Switch_SwitchCond_ZC_b2 = _rtB -> B_38_1578_0 - _rtP ->
P_693 ; _rtZCSV -> Abs_AbsZc_ZC_o = _rtB -> B_38_1582_0 ; _rtZCSV ->
Switch_SwitchCond_ZC_fk = _rtB -> B_38_1634_0 - _rtP -> P_719 ; _rtZCSV ->
Switch_SwitchCond_ZC_bl = _rtB -> B_38_1642_0 - _rtP -> P_725 ; _rtZCSV ->
Switch_SwitchCond_ZC_el = _rtB -> B_38_1645_0 - _rtP -> P_727 ; _rtZCSV ->
Abs_AbsZc_ZC_c = _rtB -> B_38_1665_0 ; _rtZCSV -> Switch_SwitchCond_ZC_ky =
_rtB -> B_38_1715_0 - _rtP -> P_756 ; _rtZCSV -> Switch_SwitchCond_ZC_gi =
_rtB -> B_38_1720_0 - _rtP -> P_759 ; _rtZCSV -> Switch_SwitchCond_ZC_jsd =
_rtB -> B_38_1723_0 - _rtP -> P_761 ; _rtZCSV -> Switch_SwitchCond_ZC_ka =
_rtB -> B_38_1726_0 - _rtP -> P_763 ; _rtZCSV -> Switch_SwitchCond_ZC_db =
_rtB -> B_38_1728_0 - _rtP -> P_765 ; _rtZCSV -> Switch_SwitchCond_ZC_c3 =
_rtB -> B_38_1736_0 - _rtP -> P_771 ; _rtZCSV -> Abs_AbsZc_ZC_fh = _rtB ->
B_38_1754_0 ; _rtZCSV -> Switch_SwitchCond_ZC_nu = _rtB -> B_38_1791_0 - _rtP
-> P_796 ; _rtZCSV -> Switch_SwitchCond_ZC_ck = _rtB -> B_38_1794_0 - _rtP ->
P_798 ; _rtZCSV -> Switch_SwitchCond_ZC_le = _rtB -> B_38_1799_0 - _rtP ->
P_801 ; _rtZCSV -> Switch_SwitchCond_ZC_fn = _rtB -> B_38_1802_0 - _rtP ->
P_803 ; _rtZCSV -> Abs_AbsZc_ZC_o2 = _rtB -> B_38_1820_0 ; _rtZCSV ->
Switch_SwitchCond_ZC_lg = _rtB -> B_38_1857_0 - _rtP -> P_828 ; } static void
mdlInitializeSizes ( SimStruct * S ) { ssSetChecksumVal ( S , 0 , 299094133U
) ; ssSetChecksumVal ( S , 1 , 3568375185U ) ; ssSetChecksumVal ( S , 2 ,
2295214137U ) ; ssSetChecksumVal ( S , 3 , 2528947403U ) ; { mxArray *
slVerStructMat = NULL ; mxArray * slStrMat = mxCreateString ( "simulink" ) ;
char slVerChar [ 10 ] ; int status = mexCallMATLAB ( 1 , & slVerStructMat , 1
, & slStrMat , "ver" ) ; if ( status == 0 ) { mxArray * slVerMat = mxGetField
( slVerStructMat , 0 , "Version" ) ; if ( slVerMat == NULL ) { status = 1 ; }
else { status = mxGetString ( slVerMat , slVerChar , 10 ) ; } }
mxDestroyArray ( slStrMat ) ; mxDestroyArray ( slVerStructMat ) ; if ( (
status == 1 ) || ( strcmp ( slVerChar , "9.3" ) != 0 ) ) { return ; } }
ssSetOptions ( S , SS_OPTION_EXCEPTION_FREE_CODE ) ; if ( ssGetSizeofDWork (
S ) != sizeof ( DW_nmm2DFullBody_T ) ) { ssSetErrorStatus ( S ,
"Unexpected error: Internal DWork sizes do "
"not match for accelerator mex file." ) ; } if ( ssGetSizeofGlobalBlockIO ( S
) != sizeof ( B_nmm2DFullBody_T ) ) { ssSetErrorStatus ( S ,
"Unexpected error: Internal BlockIO sizes do "
"not match for accelerator mex file." ) ; } { int ssSizeofParams ;
ssGetSizeofParams ( S , & ssSizeofParams ) ; if ( ssSizeofParams != sizeof (
P_nmm2DFullBody_T ) ) { static char msg [ 256 ] ; sprintf ( msg ,
"Unexpected error: Internal Parameters sizes do "
"not match for accelerator mex file." ) ; } } _ssSetModelRtp ( S , ( real_T *
) & nmm2DFullBody_rtDefaultP ) ; _ssSetConstBlockIO ( S , &
nmm2DFullBody_rtInvariant ) ; rt_InitInfAndNaN ( sizeof ( real_T ) ) ; ( (
P_nmm2DFullBody_T * ) ssGetModelRtp ( S ) ) -> P_455 = rtInf ; ( (
P_nmm2DFullBody_T * ) ssGetModelRtp ( S ) ) -> P_489 = rtInf ; ( (
P_nmm2DFullBody_T * ) ssGetModelRtp ( S ) ) -> P_531 = rtInf ; ( (
P_nmm2DFullBody_T * ) ssGetModelRtp ( S ) ) -> P_688 = rtInf ; ( (
P_nmm2DFullBody_T * ) ssGetModelRtp ( S ) ) -> P_722 = rtInf ; ( (
P_nmm2DFullBody_T * ) ssGetModelRtp ( S ) ) -> P_768 = rtInf ; ( (
P_nmm2DFullBody_T * ) ssGetModelRtp ( S ) ) -> StancePhase_b . P_2 = rtInf ;
( ( P_nmm2DFullBody_T * ) ssGetModelRtp ( S ) ) -> StancePhase_j . P_7 =
rtInf ; ( ( P_nmm2DFullBody_T * ) ssGetModelRtp ( S ) ) -> StancePhase_j .
P_9 = rtInf ; ( ( P_nmm2DFullBody_T * ) ssGetModelRtp ( S ) ) ->
StancePhase_j . P_13 = rtMinusInf ; ( ( P_nmm2DFullBody_T * ) ssGetModelRtp (
S ) ) -> StancePhase_k . P_5 = rtInf ; ( ( P_nmm2DFullBody_T * )
ssGetModelRtp ( S ) ) -> StancePhase_k . P_7 = rtInf ; ( ( P_nmm2DFullBody_T
* ) ssGetModelRtp ( S ) ) -> StancePhase_g . P_2 = rtInf ; ( (
P_nmm2DFullBody_T * ) ssGetModelRtp ( S ) ) -> StancePhase_o . P_7 = rtInf ;
( ( P_nmm2DFullBody_T * ) ssGetModelRtp ( S ) ) -> StancePhase_o . P_9 =
rtInf ; ( ( P_nmm2DFullBody_T * ) ssGetModelRtp ( S ) ) -> StancePhase_o .
P_13 = rtMinusInf ; ( ( P_nmm2DFullBody_T * ) ssGetModelRtp ( S ) ) ->
StancePhase_i . P_5 = rtInf ; ( ( P_nmm2DFullBody_T * ) ssGetModelRtp ( S ) )
-> StancePhase_i . P_7 = rtInf ; } static void mdlInitializeSampleTimes (
SimStruct * S ) { { SimStruct * childS ; SysOutputFcn * callSysFcns ; childS
= ssGetSFunction ( S , 0 ) ; callSysFcns = ssGetCallSystemOutputFcnList (
childS ) ; callSysFcns [ 3 + 0 ] = ( SysOutputFcn ) ( NULL ) ; childS =
ssGetSFunction ( S , 1 ) ; callSysFcns = ssGetCallSystemOutputFcnList (
childS ) ; callSysFcns [ 3 + 0 ] = ( SysOutputFcn ) ( NULL ) ; childS =
ssGetSFunction ( S , 2 ) ; callSysFcns = ssGetCallSystemOutputFcnList (
childS ) ; callSysFcns [ 3 + 0 ] = ( SysOutputFcn ) ( NULL ) ; childS =
ssGetSFunction ( S , 3 ) ; callSysFcns = ssGetCallSystemOutputFcnList (
childS ) ; callSysFcns [ 3 + 0 ] = ( SysOutputFcn ) ( NULL ) ; }
slAccRegPrmChangeFcn ( S , mdlOutputsTID4 ) ; } static void mdlTerminate (
SimStruct * S ) { }
#include "simulink.c"

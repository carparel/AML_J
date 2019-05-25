#include "__cf_nmm2DFullBody.h"
#ifndef RTW_HEADER_nmm2DFullBody_acc_private_h_
#define RTW_HEADER_nmm2DFullBody_acc_private_h_
#include "rtwtypes.h"
#include "multiword_types.h"
#include "nmm2DFullBody_acc.h"
#if !defined(ss_VALIDATE_MEMORY)
#define ss_VALIDATE_MEMORY(S, ptr)   if(!(ptr)) {\
  ssSetErrorStatus(S, RT_MEMORY_ALLOCATION_ERROR);\
  }
#endif
#if !defined(rt_FREE)
#if !defined(_WIN32)
#define rt_FREE(ptr)   if((ptr) != (NULL)) {\
  free((ptr));\
  (ptr) = (NULL);\
  }
#else
#define rt_FREE(ptr)   if((ptr) != (NULL)) {\
  free((void *)(ptr));\
  (ptr) = (NULL);\
  }
#endif
#endif
#ifndef UCHAR_MAX
#include <limits.h>
#endif
#if ( UCHAR_MAX != (0xFFU) ) || ( SCHAR_MAX != (0x7F) )
#error Code was generated for compiler with different sized uchar/char. \Consider adjusting Test hardware word size settings on the \Hardware Implementation pane to match your compiler word sizes as \defined in limits.h of the compiler. Alternatively, you can \select the Test hardware is the same as production hardware option and \select the Enable portable word sizes option on the Code Generation > \Verification pane for ERT based targets, which will disable the \preprocessor word size checks.
#endif
#if ( USHRT_MAX != (0xFFFFU) ) || ( SHRT_MAX != (0x7FFF) )
#error Code was generated for compiler with different sized ushort/short. \Consider adjusting Test hardware word size settings on the \Hardware Implementation pane to match your compiler word sizes as \defined in limits.h of the compiler. Alternatively, you can \select the Test hardware is the same as production hardware option and \select the Enable portable word sizes option on the Code Generation > \Verification pane for ERT based targets, which will disable the \preprocessor word size checks.
#endif
#if ( UINT_MAX != (0xFFFFFFFFU) ) || ( INT_MAX != (0x7FFFFFFF) )
#error Code was generated for compiler with different sized uint/int. \Consider adjusting Test hardware word size settings on the \Hardware Implementation pane to match your compiler word sizes as \defined in limits.h of the compiler. Alternatively, you can \select the Test hardware is the same as production hardware option and \select the Enable portable word sizes option on the Code Generation > \Verification pane for ERT based targets, which will disable the \preprocessor word size checks.
#endif
#if ( ULONG_MAX != (0xFFFFFFFFU) ) || ( LONG_MAX != (0x7FFFFFFF) )
#error Code was generated for compiler with different sized ulong/long. \Consider adjusting Test hardware word size settings on the \Hardware Implementation pane to match your compiler word sizes as \defined in limits.h of the compiler. Alternatively, you can \select the Test hardware is the same as production hardware option and \select the Enable portable word sizes option on the Code Generation > \Verification pane for ERT based targets, which will disable the \preprocessor word size checks.
#endif
#ifndef __RTW_UTFREE__
extern void * utMalloc ( size_t ) ; extern void utFree ( void * ) ;
#endif
boolean_T nmm2DFullBody_acc_rt_TDelayUpdateTailOrGrowBuf ( int_T * bufSzPtr ,
int_T * tailPtr , int_T * headPtr , int_T * lastPtr , real_T tMinusDelay ,
real_T * * tBufPtr , real_T * * uBufPtr , real_T * * xBufPtr , boolean_T
isfixedbuf , boolean_T istransportdelay , int_T * maxNewBufSzPtr ) ; real_T
nmm2DFullBody_acc_rt_TDelayInterpolate ( real_T tMinusDelay , real_T tStart ,
real_T * tBuf , real_T * uBuf , int_T bufSz , int_T * lastIdx , int_T
oldestIdx , int_T newIdx , real_T initOutput , boolean_T discrete , boolean_T
minorStepAndTAtLastMajorOutput ) ; void
nmm2DFullBody_GroundFrictionModel_Init ( SimStruct * S ,
B_GroundFrictionModel_nmm2DFullBody_T * localB ,
DW_GroundFrictionModel_nmm2DFullBody_T * localDW ,
P_GroundFrictionModel_nmm2DFullBody_T * localP ) ; void
nmm2DFullBody_GroundFrictionModel_Reset ( SimStruct * S ,
DW_GroundFrictionModel_nmm2DFullBody_T * localDW ,
P_GroundFrictionModel_nmm2DFullBody_T * localP ) ; void
nmm2DFullBody_GroundFrictionModel_ZC ( SimStruct * S , real_T rtu_vxms ,
B_GroundFrictionModel_nmm2DFullBody_T * localB , const
ConstB_GroundFrictionModel_nmm2DFullBody_T * localC ,
DW_GroundFrictionModel_nmm2DFullBody_T * localDW ,
ZCV_GroundFrictionModel_nmm2DFullBody_T * localZCSV ) ; void
nmm2DFullBody_GroundFrictionModel_Disable ( SimStruct * S ,
B_GroundFrictionModel_nmm2DFullBody_T * localB ,
DW_GroundFrictionModel_nmm2DFullBody_T * localDW ,
P_GroundFrictionModel_nmm2DFullBody_T * localP ) ; void
nmm2DFullBody_GroundFrictionModel_Update ( SimStruct * S ,
B_GroundFrictionModel_nmm2DFullBody_T * localB ,
DW_GroundFrictionModel_nmm2DFullBody_T * localDW ) ; void
nmm2DFullBody_GroundFrictionModel ( SimStruct * S , boolean_T rtu_Contact ,
real_T rtu_dym , real_T rtu_vyms , real_T rtu_xm , real_T rtu_vxms ,
B_GroundFrictionModel_nmm2DFullBody_T * localB , const
ConstB_GroundFrictionModel_nmm2DFullBody_T * localC ,
DW_GroundFrictionModel_nmm2DFullBody_T * localDW ,
P_GroundFrictionModel_nmm2DFullBody_T * localP ) ; void
nmm2DFullBody_GroundFrictionModelTID4 ( SimStruct * S ,
B_GroundFrictionModel_nmm2DFullBody_T * localB ,
DW_GroundFrictionModel_nmm2DFullBody_T * localDW ,
P_GroundFrictionModel_nmm2DFullBody_T * localP ) ; void
nmm2DFullBody_LeftForcePlate_Init ( SimStruct * S ,
B_LeftForcePlate_nmm2DFullBody_T * localB , P_LeftForcePlate_nmm2DFullBody_T
* localP ) ; void nmm2DFullBody_LeftForcePlate_Disable ( SimStruct * S ,
B_LeftForcePlate_nmm2DFullBody_T * localB , DW_LeftForcePlate_nmm2DFullBody_T
* localDW , P_LeftForcePlate_nmm2DFullBody_T * localP ) ; void
nmm2DFullBody_LeftForcePlate ( SimStruct * S , boolean_T rtu_Enable , real_T
rtu_GRF_BallNN , real_T rtu_GRF_BallNN_p , real_T rtu_GRF_HeelNN , real_T
rtu_GRF_HeelNN_f , B_LeftForcePlate_nmm2DFullBody_T * localB ,
DW_LeftForcePlate_nmm2DFullBody_T * localDW ,
P_LeftForcePlate_nmm2DFullBody_T * localP ) ; void
nmm2DFullBody_StancePhase_Init ( SimStruct * S ,
B_StancePhase_nmm2DFullBody_T * localB , P_StancePhase_nmm2DFullBody_T *
localP ) ; void nmm2DFullBody_StancePhase_ZC ( SimStruct * S , real_T
rtu_Enable , DW_StancePhase_nmm2DFullBody_T * localDW ,
ZCV_StancePhase_nmm2DFullBody_T * localZCSV ) ; void
nmm2DFullBody_StancePhase_Disable ( SimStruct * S ,
B_StancePhase_nmm2DFullBody_T * localB , DW_StancePhase_nmm2DFullBody_T *
localDW , P_StancePhase_nmm2DFullBody_T * localP ) ; void
nmm2DFullBody_StancePhase ( SimStruct * S , real_T rtu_Enable , real_T
rtu_FmGAS , B_StancePhase_nmm2DFullBody_T * localB ,
DW_StancePhase_nmm2DFullBody_T * localDW , P_StancePhase_nmm2DFullBody_T *
localP ) ; void nmm2DFullBody_StancePhaseTID4 ( SimStruct * S ,
B_StancePhase_nmm2DFullBody_T * localB , DW_StancePhase_nmm2DFullBody_T *
localDW , P_StancePhase_nmm2DFullBody_T * localP ) ; void
nmm2DFullBody_SwingPhase_Init ( SimStruct * S , B_SwingPhase_nmm2DFullBody_T
* localB , P_SwingPhase_nmm2DFullBody_T * localP ) ; void
nmm2DFullBody_SwingPhase_Disable ( SimStruct * S ,
B_SwingPhase_nmm2DFullBody_T * localB , DW_SwingPhase_nmm2DFullBody_T *
localDW , P_SwingPhase_nmm2DFullBody_T * localP ) ; void
nmm2DFullBody_SwingPhase ( SimStruct * S , boolean_T rtu_Enable , real_T
rtu_FmGLU , B_SwingPhase_nmm2DFullBody_T * localB ,
DW_SwingPhase_nmm2DFullBody_T * localDW , P_SwingPhase_nmm2DFullBody_T *
localP ) ; void nmm2DFullBody_SwingPhaseTID4 ( SimStruct * S ,
B_SwingPhase_nmm2DFullBody_T * localB , DW_SwingPhase_nmm2DFullBody_T *
localDW , P_SwingPhase_nmm2DFullBody_T * localP ) ; void
nmm2DFullBody_StancePhase_l_Init ( SimStruct * S ,
B_StancePhase_nmm2DFullBody_o_T * localB , P_StancePhase_nmm2DFullBody_j_T *
localP ) ; void nmm2DFullBody_StancePhase_g_ZC ( SimStruct * S , real_T
rtu_Enable , DW_StancePhase_nmm2DFullBody_i_T * localDW ,
ZCV_StancePhase_nmm2DFullBody_h_T * localZCSV ) ; void
nmm2DFullBody_StancePhase_j_Disable ( SimStruct * S ,
B_StancePhase_nmm2DFullBody_o_T * localB , DW_StancePhase_nmm2DFullBody_i_T *
localDW , P_StancePhase_nmm2DFullBody_j_T * localP ) ; void
nmm2DFullBody_StancePhase_i ( SimStruct * S , real_T rtu_Enable , real_T
rtu_Theta , real_T rtu_dThetarads , real_T rtu_CLdxThighN , real_T rtu_OLonCL
, B_StancePhase_nmm2DFullBody_o_T * localB , DW_StancePhase_nmm2DFullBody_i_T
* localDW , P_StancePhase_nmm2DFullBody_j_T * localP ) ; void
nmm2DFullBody_StancePhase_iTID4 ( SimStruct * S ,
B_StancePhase_nmm2DFullBody_o_T * localB , DW_StancePhase_nmm2DFullBody_i_T *
localDW , P_StancePhase_nmm2DFullBody_j_T * localP ) ; void
nmm2DFullBody_StancePhase_p_Init ( SimStruct * S ,
B_StancePhase_nmm2DFullBody_j_T * localB , P_StancePhase_nmm2DFullBody_d_T *
localP ) ; void nmm2DFullBody_StancePhase_f_ZC ( SimStruct * S , real_T
rtu_Enable , DW_StancePhase_nmm2DFullBody_in_T * localDW ,
ZCV_StancePhase_nmm2DFullBody_hs_T * localZCSV ) ; void
nmm2DFullBody_StancePhase_o_Disable ( SimStruct * S ,
B_StancePhase_nmm2DFullBody_j_T * localB , DW_StancePhase_nmm2DFullBody_in_T
* localDW , P_StancePhase_nmm2DFullBody_d_T * localP ) ; void
nmm2DFullBody_StancePhase_o ( SimStruct * S , real_T rtu_Enable , real_T
rtu_Theta , real_T rtu_dThetarads , real_T rtu_CLdxThighN , real_T rtu_OLonCL
, B_StancePhase_nmm2DFullBody_j_T * localB ,
DW_StancePhase_nmm2DFullBody_in_T * localDW , P_StancePhase_nmm2DFullBody_d_T
* localP ) ; void nmm2DFullBody_StancePhase_oTID4 ( SimStruct * S ,
B_StancePhase_nmm2DFullBody_j_T * localB , DW_StancePhase_nmm2DFullBody_in_T
* localDW , P_StancePhase_nmm2DFullBody_d_T * localP ) ; void
nmm2DFullBody_SwingPhase_f_Init ( SimStruct * S ,
B_SwingPhase_nmm2DFullBody_n_T * localB , P_SwingPhase_nmm2DFullBody_i_T *
localP ) ; void nmm2DFullBody_SwingPhase_l_Disable ( SimStruct * S ,
B_SwingPhase_nmm2DFullBody_n_T * localB , DW_SwingPhase_nmm2DFullBody_n_T *
localDW , P_SwingPhase_nmm2DFullBody_i_T * localP ) ; void
nmm2DFullBody_SwingPhase_b ( SimStruct * S , boolean_T rtu_Enable , real_T
rtu_Delta_Theta , real_T rtu_LceHFL , real_T rtu_LceHAM , real_T rtu_FmHAM ,
B_SwingPhase_nmm2DFullBody_n_T * localB , DW_SwingPhase_nmm2DFullBody_n_T *
localDW , P_SwingPhase_nmm2DFullBody_i_T * localP ) ; void
nmm2DFullBody_SwingPhase_bTID4 ( SimStruct * S ,
B_SwingPhase_nmm2DFullBody_n_T * localB , DW_SwingPhase_nmm2DFullBody_n_T *
localDW , P_SwingPhase_nmm2DFullBody_i_T * localP ) ; void
nmm2DFullBody_StancePhase_b_Init ( SimStruct * S ,
B_StancePhase_nmm2DFullBody_e_T * localB , P_StancePhase_nmm2DFullBody_di_T *
localP ) ; void nmm2DFullBody_StancePhase_b_ZC ( SimStruct * S , real_T
rtu_Enable , DW_StancePhase_nmm2DFullBody_l_T * localDW ,
ZCV_StancePhase_nmm2DFullBody_n_T * localZCSV ) ; void
nmm2DFullBody_StancePhase_d_Disable ( SimStruct * S ,
B_StancePhase_nmm2DFullBody_e_T * localB , DW_StancePhase_nmm2DFullBody_l_T *
localDW , P_StancePhase_nmm2DFullBody_di_T * localP ) ; void
nmm2DFullBody_StancePhase_l ( SimStruct * S , real_T rtu_Enable , real_T
rtu_FmSOL , real_T rtu_LceTA , B_StancePhase_nmm2DFullBody_e_T * localB ,
DW_StancePhase_nmm2DFullBody_l_T * localDW , P_StancePhase_nmm2DFullBody_di_T
* localP ) ; void nmm2DFullBody_StancePhase_lTID4 ( SimStruct * S ,
B_StancePhase_nmm2DFullBody_e_T * localB , DW_StancePhase_nmm2DFullBody_l_T *
localDW , P_StancePhase_nmm2DFullBody_di_T * localP ) ; void
nmm2DFullBody_SwingPhase_g_Init ( SimStruct * S ,
B_SwingPhase_nmm2DFullBody_a_T * localB , P_SwingPhase_nmm2DFullBody_p_T *
localP ) ; void nmm2DFullBody_SwingPhase_f_Disable ( SimStruct * S ,
B_SwingPhase_nmm2DFullBody_a_T * localB , DW_SwingPhase_nmm2DFullBody_p_T *
localDW , P_SwingPhase_nmm2DFullBody_p_T * localP ) ; void
nmm2DFullBody_SwingPhase_f ( SimStruct * S , boolean_T rtu_Enable , real_T
rtu_LceTA , B_SwingPhase_nmm2DFullBody_a_T * localB ,
DW_SwingPhase_nmm2DFullBody_p_T * localDW , P_SwingPhase_nmm2DFullBody_p_T *
localP ) ; void nmm2DFullBody_SwingPhase_fTID4 ( SimStruct * S ,
B_SwingPhase_nmm2DFullBody_a_T * localB , DW_SwingPhase_nmm2DFullBody_p_T *
localDW , P_SwingPhase_nmm2DFullBody_p_T * localP ) ; void
nmm2DFullBody_StancePhase_j_Init ( SimStruct * S ,
B_StancePhase_nmm2DFullBody_g_T * localB , P_StancePhase_nmm2DFullBody_e_T *
localP ) ; void nmm2DFullBody_StancePhase_ga_ZC ( SimStruct * S , real_T
rtu_Enable , DW_StancePhase_nmm2DFullBody_p_T * localDW ,
ZCV_StancePhase_nmm2DFullBody_o_T * localZCSV ) ; void
nmm2DFullBody_StancePhase_ox_Disable ( SimStruct * S ,
B_StancePhase_nmm2DFullBody_g_T * localB , DW_StancePhase_nmm2DFullBody_p_T *
localDW , P_StancePhase_nmm2DFullBody_e_T * localP ) ; void
nmm2DFullBody_StancePhase_g ( SimStruct * S , real_T rtu_Enable , real_T
rtu_CLKneeState , real_T rtu_OLonCL , real_T rtu_OLdxThighN , real_T
rtu_FmVAS , B_StancePhase_nmm2DFullBody_g_T * localB ,
DW_StancePhase_nmm2DFullBody_p_T * localDW , P_StancePhase_nmm2DFullBody_e_T
* localP ) ; void nmm2DFullBody_StancePhase_gTID4 ( SimStruct * S ,
B_StancePhase_nmm2DFullBody_g_T * localB , DW_StancePhase_nmm2DFullBody_p_T *
localDW , P_StancePhase_nmm2DFullBody_e_T * localP ) ;
#endif

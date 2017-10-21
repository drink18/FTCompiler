/*--------------------------------------------------------------------------------------------------
该软件为私有的保密文件，只有在菲特通信科技有限公司授权的情况下方可使用。                            
(C) COPYRIGHT 2016 FIRSTECH INC. ALL RIGHTS RESERVED                                                
项目名称     :     MRI                                                                           
文件名称     :     tx.h                                                                   
作者         :     zhaoke                                                                       
作者联系方式 :     zhaoke@afirstech.com                                                         
时间         :     2016-01-11                                                                       
版本         :     V1.0                                                                             
关键字       :     TX                                                                             
文件描述     :                                            
修改情况     :                                                                                      
时间         修改作者           版本          修改说明                                              
=========================================================                                           
2016-1-11    zhaoke             v1.1         创建 
2016-5-23    pengjing                        定义两种版本下  TX_FILTER_DELAY 值      
2016-10-31   zhaoke                           deleteted TxPhaseOffsetReg 参数rst    
2017-2-22    zhaoke                          添加函数注释      
2017-3-17    pengjing                        TX_GRA_DELAY 修改 15.11US 新增TR3DMOTSAEnScanCnt函数  
2017-5-9     zhaoke                          1.频率和相位查表/计算选择分开控制。宏定义TxFreqPhaseSel()拆分为 TxFreqSel()和TxPhaseSel()
                                             2.设置频率和相位时，相位累加器均复位
											 3.TxPhaseOffsetReg相位不再取负                               
========================================================= */                                          
#ifndef TX_H

#define TX_H

#define TX_CHANNEL1_START    0x1
#define TX_CHANNEL2_START    0x2	
#define TX_CHANNEL3_START    0x4 
#define TX_CHANNEL4_START    0x8
#define TX_CHANNEL5_START    0x10
#define TX_CHANNEL6_START    0x20
#define TX_CHANNEL7_START    0x40
#define TX_CHANNEL8_START    0x80

#define TX_BRAM_NO1          0x0
#define TX_BRAM_NO2          0x1
#define TX_BRAM_NO3          0x2
#define TX_BRAM_NO4          0x3
#define TX_BRAM_NO5          0x4
#define TX_BRAM_NO6          0x5
#define TX_BRAM_NO7          0x6
#define TX_BRAM_NO8          0x7
#define TX_BRAM_NO9          0x8
#define TX_BRAM_NO10         0x9
#define TX_BRAM_NO11         0xA
#define TX_BRAM_NO12         0xB
#define TX_BRAM_NO13         0xC
#define TX_BRAM_NO14         0xD
#define TX_BRAM_NO15         0xE
#define TX_BRAM_NO16         0xF

#ifdef MARIX_VERSION
#define TX_FILTER_DELAY          191.994//190.01+1427*0.00125-0.2
#else

#define TX_FILTER_DELAY          95.04//260.49//185.49//260.50//259.95375//261.425//259.95375//247.58//
#endif
    

//#define TX_GRA_DELAY         15.11

//#ifdef MARIX_VERSION
//#define TX_GRA_FIS_RF_SEC_DELAY 160.2//118.0
//#else
//#define TX_GRA_FIS_RF_SEC_DELAY 199.95375//259.95375-60
//#endif

//#ifdef MARIX_VERSION
//#define TX_RF_FIS_GRA_SEC_DELAY 125.994
//#else
//#define TX_RF_FIS_GRA_SEC_DELAY 0//200//147//202.18375//264.32-61.34-1.58375//102//199.95375//259.95375-60
//#endif



#define TX_FIR_RESET_TIME    300//ns
#define TX_START_ADDR                  0x86
#define SHAPE_NO_ADDR_INITIAL          0x8e
#define TX_CENTER_FRE_ADDR_INITIAL     0x13a
#define TX_FRE_OFFSET_ADDR_INITIAL     0xa5
#define TX_FRE_RAM_ADDR_INITIAL        0xfa
#define TX_PHASE_OFFSET_ADDR_INITIAL   0xb5
//#define TX_PHASE_OFFSET_ADDR_INITIAL   0x165
#define TX_PHASE_RAM_ADDR_INITIAL      0x112
#define TX_GAIN_ADDR_INITIAL           0xc6
#define TX_GAIN_RAM_ADDR_INITIAL       0x12a
#define TX_FIR_RESET_ADDR              0xd9
#define TX_FRE_INVERSE_EN_ADDR         0xda
#define TX_DUC_BYPASS_ADDR_INITIAL     0xef
#define TX_GATE_ADDR                   0x30
#define TX_FREQ_EN_ADDR                0xc7
#define TX_OFFSET_DISABLE_ADDR         0x13b
#define TX_GAIN_SEL_ADDR               0xfa
#define TX_FREQ_SEL_ADDR               0xf9
#define TX_PHASE_SEL_ADDR              0x178
#define TX_SQUARE_PERIOD_ADDR_INITIAL  0x153
#define TX_SQUARE_EN_ADDR              0x14b
#define TX_END_LAST_TIME_ADDR_INITIAL  0x15c

/*采用宏的方式声明函数可以大大简化生成的序列指令。                             */

#define TxChannelShapeSel(channelNo,shapeNo)                     do{ \
	                                                                int __shapeNoInt; \
	                                                                 __shapeNoInt = shapeNo;\
                                                                   setr(SHAPE_NO_ADDR_INITIAL + channelNo ,shapeNo );  \
                                                                   setr(TX_SQUARE_EN_ADDR + channelNo ,0 );\
                                                                 }while(0)
                                                                 
#define TxAttRam(channelNo,attRamAddr)                        do{\
                                                                   	int attRamAddrInt;\
                                                                   	attRamAddrInt = attRamAddr;\
                                                                   	setr(TX_GAIN_SEL_ADDR ,255 );\
                                                                    setr(TX_GAIN_RAM_ADDR_INITIAL + channelNo * 2 ,attRamAddrInt );\
                                                                  }while(0)
#define TxFreInverseEn(freInverseEn)                               setr(TX_FRE_INVERSE_EN_ADDR ,freInverseEn)

#define TxBypass(channelNo,ducBypass)                              setr(TX_DUC_BYPASS_ADDR_INITIAL + channelNo,ducBypass )

#define TxOffsetFreqDisable(disableEn)                             setr(TX_OFFSET_DISABLE_ADDR ,disableEn )

#define TxGainSel(gainSel)                                         setr(TX_GAIN_SEL_ADDR ,gainSel )	

#define TxFreqSel(freqSel)                                 setr(TX_FREQ_SEL_ADDR ,freqSel )
#define TxPhaseSel(phaseSel)                               setr(TX_PHASE_SEL_ADDR ,phaseSel )                                                                
#define TxNcoFreqEn(ncoFreqEn)                                     setr(TX_FREQ_EN_ADDR ,ncoFreqEn )
#define TxGateOut(voltageLevel)                                    setr(TX_GATE_ADDR ,voltageLevel )
                          

#define TxFreqOffsetReg(channelNo,offsetFreq,freqUnit) \
																									 do{ \
	                                                 double __temp; \
	                                                 int __freq;\
													 double __addrFloat;\
													 __addrFloat = TX_FRE_OFFSET_ADDR_INITIAL + channelNo * 2;\
													 __temp = offsetFreq * freqUnit; \
	                                                 __freq = __temp;\
	                                                 TxFreqSel(0);\
                                                   TxOffsetFreqDisable(0); \
	                                                 setr(__addrFloat,__freq); \
	                                                 setr(TX_FREQ_EN_ADDR ,65535 ); \
	                                                }while(0)

                                                                  

#define TxNcoFreq(channelNo,ncoFreq,freqUnit)           \
																								 do{ \
	                                                 double __temp; \
	                                                 int __freq; \
													 double __addrFloat;\
													 __addrFloat = TX_CENTER_FRE_ADDR_INITIAL + channelNo * 2;\
	                                                 __temp = ncoFreq * freqUnit; \
	                                                 __freq = __temp; \
	                                                 TxOffsetFreqDisable(255); \
	                                                 setr(__addrFloat,__freq); \
	                                                 setr(TX_FREQ_EN_ADDR ,255 ); \
	                                                }while(0) 

#define TxAttReg(channelNo,txAtt)             do{ \
	                                                 double __temp;\
	                                                 int __gain;\
													 int __addrInt;\
													 double __addrFloat;\
													 __addrFloat = TX_GAIN_ADDR_INITIAL + channelNo * 2;\
													 __addrInt = __addrFloat;\
	                                                 __temp = txAtt * GAIN_FACTOR;\
	                                                 __gain = __temp;\
	                                                 TxGainSel(0);\
	                                                 setr(__addrFloat,__gain); \
	                                                }while(0)
                                            

#define TxPhaseOffsetRam(channelNo, phaseRamAddr)  do{\
                                                    int phaseRamAddrInt;\
													 double __addrFloat;\
													 __addrFloat = TX_PHASE_RAM_ADDR_INITIAL + channelNo * 3;\
                                                    phaseRamAddrInt = phaseRamAddr;\
                                                    TxPhaseSel(255);\
                                                    setr(__addrFloat,phaseRamAddrInt);\
	                                                  	 setr(TX_FREQ_EN_ADDR ,65535 );\
                                                    }while(0) 

#define TxFreqOffsetRam(channelNo, freNo)         do{\
                                                    int freNoInt;\
													 double __addrFloat;\
													 __addrFloat = TX_FRE_RAM_ADDR_INITIAL + channelNo * 3;\
                                                    freNoInt = freNo;\
                                                    TxFreqSel(255 ); \
                                                    TxOffsetFreqDisable(0);\
                                                    setr(__addrFloat,freNoInt);\
	                                                  setr(TX_FREQ_EN_ADDR ,65535 );\
	                                                   }while(0)
                                                    
#define TxFirReset()                                setr(TX_FIR_RESET_ADDR,255 ); \
                                                    delay(TX_FIR_RESET_TIME); \
                                                    setr(TX_FIR_RESET_ADDR,0 )
#define TxHardPulseShape(channelNo,pulsePeriod)   do{\
	                                                                int __temp;\
	                                                                __temp = pulsePeriod;\
	                                                                setr(TX_GAIN_SEL_ADDR ,0 );\
	                                                                setr(TX_SQUARE_PERIOD_ADDR_INITIAL + channelNo,__temp );\
	                                                                setr(TX_SQUARE_EN_ADDR + channelNo ,1 ); \
	                                                            }while(0)
#define TxEndLastTime(lastTime,timeUnit)          do{\
	                                                                double __temp1;\
	                                                                int __temp2;\
	                                                                __temp1 = lastTime * timeUnit;\
	                                                                __temp2 = __temp1;\
	                                                                setr(TX_END_LAST_TIME_ADDR_INITIAL,__temp2 );\
	                                                            }while(0)


/*********************************************************************/

/*功能描述：用相位偏移寄存器值设置对应通道中相位偏移值               */ 

/*入口参数：channelNo，发端通道号,取值范围：1-8，表示选择通道的通道号*/

/*          ncoPhase,相位值，单位：度。                              */

/*********************************************************************/

 
void TxPhaseOffsetReg(int channelNo,double ncoPhase)

{

    double txPhaseAddrTemp;

   // int txPhaseAddr;

    double channelNoTemp;

    double ncoPhaseTemp;

    int phase;

    channelNoTemp = channelNo;

    txPhaseAddrTemp = TX_PHASE_OFFSET_ADDR_INITIAL + channelNoTemp * 2.0;

  //  txPhaseAddr = txPhaseAddrTemp;

    ncoPhaseTemp = ncoPhase * PHASE_CONVERSION_FACTOR;

  //  phase = 0-ncoPhaseTemp;
    phase = ncoPhaseTemp;
    
    TxPhaseSel(0); 
  //  setr(TX_PHASE_OFFSET_ADDR_INITIAL + channelNoTemp * 2.0 ,phase );
   setr(txPhaseAddrTemp ,phase );
    
//    if(rst==0.0)
      setr(TX_FREQ_EN_ADDR ,65535 );
//    else
//    	setr(TX_FREQ_EN_ADDR ,255 );
    	
   

}





/***************************************************************/

/*功能描述：设置各通道发射开始信号，并更新各通道频率相位控制字 */ 

/*          此函数用于单通道发射或多通道异步发射的情况。函数不 */

/*          持续到发射波形结束。                               */

/*入口参数：txStart，Bram发端数据的起始信号，该信号宽度为8bit，*/

/*                   从低到高依次对应发射板的8个通。           */

/***************************************************************/

void TxStartAsync(int txStart, double txGatePreTime)

{

//	  RfAmpOn(txStart);

//	  Delay(txGatePreTime,US);
    double __filterDelay;
    __filterDelay=TX_FILTER_DELAY;
	  timer(1);
    setr(TX_START_ADDR ,txStart );
    TimerCmp(__filterDelay-txGatePreTime,US);
    TxGateOut(txStart);
    TimerCmp(__filterDelay,US);
    /*从波形发射开始到发射板滤波器输出，最大延时为20247个时钟周期*/

    /*为使相位和数据严格对起，须延时20247个时钟周期使频率相位生效*/

//    Delay(TX_FILTER_DELAY,US);

    setr(TX_FREQ_EN_ADDR ,0 );
    timer(2);
}

/***************************************************************/

//功能描述：设置各通道发射开始信号，并更新各通道频率相位控制字  
//
//          该函数一直持续到波形发射结束。此函数只可用于单通道 
//
//          发射或通道同步发射的情况。                         
//
//入口参数：txStart，Bram发端数据的起始信号，该信号宽度为8bit，
//
//                   从低到高依次对应发射板的8个通。           
//
//          rfWaveWidth，发射板波形持续的时间 ,单位us          
//
//          txGatePreTime,发射门控提前打开时间，单位us     
//          txGatePostTime，发射门控延迟关闭时间，单位us 
/***************************************************************/

void TxStart(int txStart,double rfWaveWidth,double txGatePreTime,double txGatePostTime)
{
	  int __ampOff;

	  double __filterDelay;
	  double __txStartTemp;

	  double __temp;
    double txGateOff;
	  __filterDelay=TX_FILTER_DELAY;
    __txStartTemp = txStart;

	  timer(1);
    setr(TX_FREQ_EN_ADDR ,0 );
    setr(TX_START_ADDR ,txStart );
    TimerCmp(__filterDelay-txGatePreTime,US);
    TxGateOut(txStart);   
    __temp = 255.0-__txStartTemp;
    __ampOff = __temp;
     txGateOff = txGatePostTime+TX_FILTER_DELAY+rfWaveWidth;
    TimerCmp(txGateOff,US);    
    TxGateOut(__ampOff);
    timer(2);
    TxFirReset(); //发端滤波器清零，消除门控信号关闭以后数据不为0的现象 2016.4.19
}
/*********************************************************************/

//功能描述：用频率偏移寄存器值设置对应通道中频率偏移值 
//          用相位偏移寄存器值设置对应通道中相位偏移值               
//		频率偏移和相位偏移同时有效                               
//
//入口参数：channelNo，发端通道号,取值范围：1-8，表示选择通道的通道号
//          offsetFreq,偏移频率
//          ncoPhase,相位值，单位：度。                              
//          freqUnit，频率偏移单位，HZ，KHZ，MHZ分别对应1Hz，1000Hz，1000,000Hz
/*********************************************************************/

void TxFreqPhaseOffsetReg(int channelNo,double offsetFreq ,double ncoPhase,double freqUnit)

{

    double txPhaseAddrTemp;

   // int txPhaseAddr;

    double channelNoTemp;

    double ncoPhaseTemp;
	
	double offsetFreqTemp;
	
	double txOffsetFreqAddrTemp;

    int phase;
	
    int freq;
	
    channelNoTemp = channelNo;

    txPhaseAddrTemp = TX_PHASE_OFFSET_ADDR_INITIAL + channelNoTemp * 2.0;

	txOffsetFreqAddrTemp = TX_FRE_OFFSET_ADDR_INITIAL + channelNoTemp * 2.0;

    ncoPhaseTemp = ncoPhase * PHASE_CONVERSION_FACTOR;
    offsetFreqTemp = offsetFreq * freqUnit;

    phase = ncoPhaseTemp;
    freq = offsetFreqTemp;
	
    TxPhaseSel(0); 
    TxFreqSel(0); 
    setr(txPhaseAddrTemp ,phase );
    TxOffsetFreqDisable(0);
   	setr(txOffsetFreqAddrTemp,freq);
	
//    if(rst==0.0)
      setr(TX_FREQ_EN_ADDR ,65535 );
//    else
//    	setr(TX_FREQ_EN_ADDR ,255 );
    	
}

/*********************************************************************/

/*功能描述：根据平均次数，循环产生标号，结合TxAvePhase使用产生不同的相位

/*入口参数：noAverages，平均次数                                     */

/*返回参数：标号                                                     */

/*********************************************************************/
double TxAvePhaseCnt(double noAverages)
{	
	static double TxAvePhase_cnt1 = 0.0;	  
  double averagesCnt;
  
  if(noAverages>1.0)
  {
     if(TxAvePhase_cnt1==noAverages)
        TxAvePhase_cnt1 = 0.0;
           
     TxAvePhase_cnt1 =  TxAvePhase_cnt1+1.0;   
  }else
     TxAvePhase_cnt1 = 0.0;
     
	averagesCnt = TxAvePhase_cnt1;
	
	return averagesCnt;
}

/*****************************************************************************/
/*功能描述：3D成像时根据平均次数，循环产生标号，结合TxAvePhase使用产生不同的相位  */
/*入口参数： noAverages，平均次数                                              */
/*         noViewsSec,选层梯度相位编码次数                                    */
/*返回参数：标号                                                             */
/****************************************************************************/

double TR3DMOTSAEnScanCnt(double noAverages,double noScans,double noViewsSec) 
{
	static double noviewsCnt=0.0;
	static double scanCnt = 1.0;
	double scanCntOut;
	double noviewstmp;
	
	noviewstmp = noScans * noViewsSec *noAverages;
	noviewsCnt = noviewsCnt+1.0;
	if(noviewsCnt > noviewstmp)
		scanCnt =  scanCnt+1.0;   	
    else
        scanCnt =  scanCnt;   	
	
	scanCntOut = scanCnt;
	return scanCntOut;
}

/**********************************************************/
//功能描述：计算不同选层的频率           */
//入口参数：thickSliceRatioGain 选层厚度修正比例，选层实际厚度为thickSlice*thickSliceRatioGain*/  
//orderSlice：选择交织方式，保留    
//distanceSlice1:原始数据中第一个层面相对磁体中心（ISOCENTER）的距离，单位mm
//seperationSlice：层间距，单位mm 
//rf90Bandwidth:90度脉冲带宽，单位Hz                          
//SliceCnt:选层序号                                   
//thickSliceRatioGain:选层厚度修正比例，选层实际厚度为thickSlice*thickSliceRatioGain 
//thickSlice:层厚，单位mm
//channelNo:发端通道号,取值范围：1-8，表示选择通道的通道号
/**********************************************************/
void TxFreqSet(double orderSlice, double distanceSlice1,
               double seperationSlice,double thickSlice, double SliceCnt,\
               double rf90Bandwidth, int channelNo, double thickSliceRatioGain)
{
	double freqtmp;
	double channelNotmp;
  freqtmp = rf90Bandwidth/(thickSlice*thickSliceRatioGain) * (distanceSlice1 + (SliceCnt-1.0) * seperationSlice);
	channelNotmp = channelNo;
	TxFreqPhaseOffsetReg(channelNo,freqtmp ,0.0,HZ);
}

/*********************************************************************/

//功能描述：根据TxAvePhaseCnt产生的标号，在不同modeAverage下设置不同
//          的绝对相位偏移
//
//入口参数：noAverages，平均次数 
//          modeAverage 平均方式 ,详见脉冲序列编程手册                                  
//          offsetFreq：频率偏移值                                       
//           ncoPhase：相位偏移值                                         
//           freqUnit：频率偏移单位，HZ，KHZ，MHZ分别对应1Hz，1000Hz，1000,000Hz     
//           channelNo：发端通道号,取值范围：1-8，表示选择通道的通道号
//
/*********************************************************************/

void TxAvePhase(double modeAverage,double noAverages,double offsetFreq,double ncoPhase,double freqUnit,int channelNo)
{
	static double phase180 = 0.0;
	double phase;
	double TxAvePhase_cnt1;
	
//	TxAvePhase_cnt1 = TxAvePhaseCnt( noAverages);
  if(noAverages<2.0)
  { 
  	TxFreqPhaseOffsetReg( channelNo, offsetFreq , ncoPhase, freqUnit);
  }else{
  	if(modeAverage==0.0)	{
	   TxFreqPhaseOffsetReg( channelNo, offsetFreq , ncoPhase, freqUnit);
	  }
    if(modeAverage==1.0)	{
	   TxFreqPhaseOffsetReg( channelNo, offsetFreq , ncoPhase, freqUnit);
	  }
	  if(modeAverage==2.0){
	   phase = phase180+ncoPhase;
     TxFreqPhaseOffsetReg( channelNo, offsetFreq , phase, freqUnit);
     if(phase180 == 0.0)
  	   phase180 = 180.0;
  	   else
  	 	 phase180 = 0.0;
  	}
  }
}

/**********************************************************/
//功能描述：根据规定的选层顺序计算不同选层的位置标号           */
//入口参数：orderSlice、noSliceBlock：详见脉冲序列编程手册
//          slicecont ：输入选层顺序号                         
//
/**********************************************************/

double TxSliceCount(double orderSlice, double noSliceBlock,double slicecont)
{
	static double txSliceCnt_cnt = 0.0;
  static double txSliceCnt_cyccont = 1.0;
  static double txSliceCnt_sign = 1.0;
  static double txSliceCnt_doublecont = 0.0;
	double noSlice;
	double noSliceBlockhalf;
	int noSliceBlockhalf_int;
  double noSliceBlockhalf_double;
  double txSliceCnt_cnt_tmp;

  noSliceBlockhalf = noSliceBlock*0.5;
  noSliceBlockhalf_int = noSliceBlockhalf;
  noSliceBlockhalf_double = noSliceBlockhalf_int;
         
  if(noSliceBlockhalf>noSliceBlockhalf_double)
     txSliceCnt_cnt_tmp = noSliceBlockhalf_double + 1.0;
  else
     txSliceCnt_cnt_tmp = noSliceBlockhalf_double;
            	
  if(orderSlice<1.0)
  	orderSlice = 1.0;
  	          	
	if(slicecont==0.0)
	{
          if(orderSlice==4.0)
          {
            txSliceCnt_sign = 1.0;
            txSliceCnt_doublecont = 0.0;
            txSliceCnt_cnt= txSliceCnt_cnt_tmp ;
          }else
              txSliceCnt_cnt = 1.0;     
        txSliceCnt_cyccont = 1.0;
       }else{
         
          if(orderSlice==4.0)
          {
              if(txSliceCnt_sign==1.0)
                  txSliceCnt_doublecont = txSliceCnt_doublecont + 1.0;
              txSliceCnt_cnt = txSliceCnt_cnt_tmp + txSliceCnt_doublecont * txSliceCnt_sign;
              txSliceCnt_sign = (-1.0)*txSliceCnt_sign;
           }else 
               if(orderSlice<4.0)
               {

                   txSliceCnt_cnt = txSliceCnt_cnt+orderSlice;
                   if(txSliceCnt_cnt>noSliceBlock)
                   {
                       txSliceCnt_cyccont = txSliceCnt_cyccont+1.0;
                       txSliceCnt_cnt = txSliceCnt_cyccont;
                    }

               
                }
	}
	noSlice = txSliceCnt_cnt;
	return noSlice;
	
}

/*********************************************************************/
//
//功能描述：根据平均次数，循环产生标号，在不同phaseCycle下设置不同
//          的相对的相位偏移
//
//入口参数：noAverages，平均次数 
//          phaseCycle  相位循环                                   */
//           tag，       新的slice开始标志
/*********************************************************************/

void TxPhaseCycle(int channelNo,double phaseCycle,double noAverages,double tag)
{
	 static double TxPhaseCycleCnt_cnt1 = 0.0;
   static double TxPhaseCycleCnt_phase = 180.0;

	 if(phaseCycle==1.0)
   {

	   if(tag==0)
	  TxPhaseCycleCnt_cnt1 = TxPhaseCycleCnt_cnt1 + 1.0;

   	  if(TxPhaseCycleCnt_cnt1 == noAverages)
      {
         TxPhaseCycleCnt_cnt1 = 0.0;
         if(TxPhaseCycleCnt_phase == 180.0)
         TxPhaseCycleCnt_phase = 0.0;
         else
         TxPhaseCycleCnt_phase = 180.0;
        }

   	TxPhaseOffsetReg(channelNo,TxPhaseCycleCnt_phase);
   }

}

/*
void TxPhaseCycle180(int channelNo,double phaseCycle,double noAverages,double tag)
{
	 static double TxPhaseCycleCnt_cnt1 = 0.0;
   static double TxPhaseCycleCnt_phase = -180.0;

	 if(phaseCycle==1.0)
   {
   	if(tag==0.0){

      TxPhaseCycleCnt_cnt1 = TxPhaseCycleCnt_cnt1 + 1.0;
   	}

   	  if(TxPhaseCycleCnt_cnt1 == noAverages)
      {
         TxPhaseCycleCnt_cnt1 = 0.0;
         if(TxPhaseCycleCnt_phase == -180.0)
             TxPhaseCycleCnt_phase = 0.0;
         else
             TxPhaseCycleCnt_phase = -180.0;
        }
   	TxPhaseOffsetReg(channelNo,TxPhaseCycleCnt_phase);
   }

}
*/
#endif


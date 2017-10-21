/*--------------------------------------------------------------------------------------------------
该软件为私有的保密文件，只有在菲特通信科技有限公司授权的情况下方可使用。                            
(C) COPYRIGHT 2016 FIRSTECH INC. ALL RIGHTS RESERVED                                                
项目名称     :     MRI                                                                           
文件名称     :     rx.h                                                                   
作者         :     zhaoke                                                                       
作者联系方式 :     zhaoke@afirstech.com                                                         
时间         :     2016-01-11                                                                       
版本         :     V1.0                                                                             
关键字       :     rx                                                                             
文件描述     :                                            
修改情况     :                                                                                      
时间         修改作者           版本          修改说明                                              
=========================================================                                           
2016-1-11    zhaoke             v1.1         创建 
2016-5-23    pengjing                        修改  RxChannelAcquirePara输入参数 
2016-10-31   zhaoke                           deleteted RxPhaseOffsetReg 参数rst    
2017-2-22    zhaoke                          添加函数注释 
2017-5-9     zhaoke                          1.频率和相位查表/计算选择分开控制。宏定义RxFreqPhaseSel()拆分为 RxFreqSel()和RxPhaseSel()
                                             2.设置频率和相位时，相位累加器均复位                                                                                          
=========================================================        */
#define RX_CHANNEL1_START    0x1
#define RX_CHANNEL2_START    0x2	
#define RX_CHANNEL3_START    0x4 
#define RX_CHANNEL4_START    0x8
#define RX_CHANNEL5_START    0x10
#define RX_CHANNEL6_START    0x20
#define RX_CHANNEL7_START    0x40
#define RX_CHANNEL8_START    0x80
#define RX_FILTER_DELAY      20233.0

#define RX_START_ADDR                   0x7d
#define RX_BW_TYPE_ADDR_INITIAL         0x43
#define RX_SAMPLE_POINT_ADDR_INITIAL    0x6b
#define RX_CENTER_FRE_ADDR_INITIAL      0xb7
#define RX_FRE_OFFSET_ADDR_INITIAL      0x4a
#define RX_FRE_RAM_ADDR_INITIAL         0x87
#define RX_PHASE_OFFSET_ADDR_INITIAL    0x5a
#define RX_PHASE_RAM_ADDR_INITIAL       0x9f
#define RX_FREQ_EN_ADDR                 0x6c
#define RX_GAIN_ADDR_INITIAL            0x7d
#define RX_FIR_RESET_ADDR               0x43
#define RX_BYPASS_ADDR                  0x86
#define RX_ANALOG_CTRL_ADDR_INITIAL     0x04
#define ADC_OFFSET_ADDR_INITIAL         0xc8
#define AMP_GAIN1_ADDR_INITIAL          0xd0
#define AMP_GAIN2_ADDR_INITIAL          0xd8
#define AMP_GAIN3_ADDR_INITIAL          0xe0
#define ATT_ADDR_INITIAL                0xe8
#define RX_FREQ_SEL_ADDR                0x87
#define RX_PHASE_SEL_ADDR               0x113
#define RX_OFFSET_DISABLE_ADDR          0xb8

/*以下是可以直接宏定义的函数，具体各函数的定义及说明详见梯度模块库函数定义部分,*/
/*采用宏的方式声明函数可以大大简化生成的序列指令。                             */
#define RxFirReset()                                              setr(RX_FIR_RESET_ADDR ,255 );\
                                                                  setr(RX_FIR_RESET_ADDR ,0 )
                                                                  
#define RxFreqOffsetRam(channelNo,freNo)                        do{\
                                                                  int freNoInt;\
																  double __addrFloat;\
																  __addrFloat = RX_FRE_RAM_ADDR_INITIAL + channelNo * 3;\
                                                                  freNoInt = freNo;\
                                                                  setr(RX_FREQ_SEL_ADDR ,255 );\
                                                                  setr(RX_OFFSET_DISABLE_ADDR ,0 );\
                                                                  setr( __addrFloat,freNoInt);\
                                                                  setr(RX_FREQ_EN_ADDR ,65535);\
                                                                 }while(0) 
                                                                  
                                                                    
#define RxPhaseOffsetRam(channelNo,phaseRamAddr)                  do{\
                                                                   int phaseRamAddrInt;\
																  double __addrFloat;\
																  __addrFloat = RX_PHASE_RAM_ADDR_INITIAL + channelNo * 3;\
                                                                  phaseRamAddrInt = phaseRamAddr;\
                                                                  setr(RX_PHASE_SEL_ADDR ,255 );\
                                                                  setr( __addrFloat,phaseRamAddrInt);\
	                                                  	              setr(RX_FREQ_EN_ADDR ,65535 );\
                                                                  }while(0)
                                                                  
                                                                  
                              

#define RxBypass(rxBypass)                                        setr(RX_BYPASS_ADDR ,rxBypass )
#define RxGateOut(voltageLevel)                                   setr(RX_ANALOG_CTRL_ADDR_INITIAL,voltageLevel )
                                                                
#define RxOffsetFreqSel(freqSel)                                  setr(RX_FREQ_SEL_ADDR ,freqSel )
#define RxOffsetPhaseSel(phaseSel)                                setr(RX_PHASE_SEL_ADDR ,phaseSel )
#define RxOffsetFreqDisable(disableEn)                            setr(RX_OFFSET_DISABLE_ADDR ,disableEn )
// 

#define RxGainReg(channelNo,rxGain)                           do{          \
	                                                                      double __temp; \
	                                                                      int __gain;     \
																  double __addrFloat;\
																  __addrFloat = RX_GAIN_ADDR_INITIAL + channelNo;\
	                                                                      __temp = rxGain * GAIN_FACTOR;\
	                                                                      __gain = __temp; \
	                                                                      setr(__addrFloat,__gain); \
	                                                                  }while(0)
                                                          
#define RxFreqOffsetReg(channelNo,offsetFreq,freqUnit)                    do{\
	                                                                   double __temp;\
	                                                                   int __freq;\
																       double __addrFloat;\
																       __addrFloat = RX_FRE_OFFSET_ADDR_INITIAL + channelNo*2;\
	                                                                   __temp = offsetFreq * freqUnit;\
	                                                                   __freq = __temp;\
	                                                                   RxOffsetFreqDisable(0);\
	                                                                   RxOffsetFreqSel(0);\
	                                                                   setr(__addrFloat,__freq);\
	                                                                   setr(RX_FREQ_EN_ADDR ,65535);\
	                                                                 }while(0)
#define RxNcoFreq(channelNo,ncoFreq,freqUnit)           \
																								 do{ \
	                                                 double __temp; \
	                                                 int __freq; \
													 double __addrFloat;\
													 __addrFloat = RX_CENTER_FRE_ADDR_INITIAL + channelNo * 2;\
	                                                 __temp = ncoFreq * freqUnit; \
	                                                 __freq = __temp; \
	                                                 RxOffsetFreqDisable(255); \
	                                                 setr(__addrFloat,__freq); \
	                                                 setr(RX_FREQ_EN_ADDR ,65535 ); \
	                                                }while(0) 
/*==========================================梯度模块库函数定义====================================*/

/***************************************************
//功能描述：调用该函数后，1.打开门控信号;                                                
//          2. 生效NCO使能信号;                                                          
//          2. 对应的接收板的通道开始采集.                                               
//入口参数：rxStart，基带信号开始接收命令，该信号宽度为8bit，从低到高依次对应发射板的8个通道.
//         rxGatePreTime，接收门控提前打开时间，单位us                                   
//         rxGatePostTime，接收门控推迟关闭时间，单位us                                  
//         rxWaveTime，接收采集时间，单位us   
 CAUTION: 
        1.频率配置，滤波器复位，开始采集三个动作顺序不能改变，否则会有阶跃使得图像0相位上有干扰
        2. rxGatePreTime必须大于1us,否则会导致前1us信号丢失                                    
***************************************************/
void RxStart(int rxStart, double rxGatePreTime, double rxGatePostTime, double rxWaveTime)
{
	  int __ampOff;
	  double __rxStartTemp;
	  double __temp;
	  timer(1);
	  RxGateOut(1);
     
	  TimerCmp(rxGatePreTime,US);	  
	   setr(RX_FREQ_EN_ADDR ,0 );
	   RxFirReset();
    setr(RX_START_ADDR ,rxStart ); 
    TimerCmp(rxGatePostTime+rxWaveTime+rxGatePreTime,US);
    RxGateOut(0);
    timer(2);
}


/***********************************************************/
//功能描述：设置通道采样参数，包括采样带宽(间隔)，采样点数 
//          计算接收波形所需要的时间，单位us               
//入口参数：samplePeriod: 接收板通道的采样周期  ，共29种   
//          samplePoint:基带接收采样点数                   
//          channelNo:选择通道 ，1-8分别对应C1 - C8        
//返回值；采样持续时间，单位us                             
//CAUTION:此函数不同于AcquireDuration()!!!!!!!!!!!!!       
/***********************************************************/
double RxChannelAcquirePara(double __samplePeriod,double __samplePoint)
{
  //  int samplePointAddr;
  //  int bwTypeAddr;
    double __samplePointAddrTemp;
    //double __bwTypeAddrTemp;
    double __channelNoTemp;
   // double __samplePoint;
    double __waveReceiveTime;
   // double __samplePeriod;
   // double __bwType;
   // int __bwTypeInt;
    int __samplePointInt;
//    __channelNoTemp = channelNo;
    __samplePointInt = __samplePoint;
  // __bwTypeInt = __bwType;
    setr(0x6d ,__samplePointInt );
    //setr(0x44 ,__bwTypeInt );
    setr(0x6f ,__samplePointInt );
   // setr(0x45 ,__bwTypeInt );    
    setr(0x71 ,__samplePointInt );
   // setr(0x46 ,__bwTypeInt );    
    setr(0x73 ,__samplePointInt );
   // setr(0x47 ,__bwTypeInt );    
    setr(0x75 ,__samplePointInt );
   // setr(0x48 ,__bwTypeInt );    
    setr(0x77 ,__samplePointInt );
   // setr(0x49 ,__bwTypeInt );    
    setr(0x79 ,__samplePointInt );
   // setr(0x4a ,__bwTypeInt );    
    setr(0x7b ,__samplePointInt );
   // setr(0x4b ,__bwTypeInt );

   //计算接收波形所需要的时间
   // __dsamplePeriod=__samplePeriod;
    __waveReceiveTime = __samplePoint * __samplePeriod;
    return __waveReceiveTime;
    
}




/*******************************************************/
//功能描述：用相位偏移寄存器值设置对应通道中相位偏移值 
//入口参数：ncoPhase: 相位值，单位：度                 
//          channelNo:选择通道 ，1-8分别对应C1 - C8    
/*******************************************************/

void RxPhaseOffsetReg(int channelNo,double ncoPhase)
{
   // int rxPhaseAddr;
    double rxPhaseAddrTemp;
    double channelNoTemp;
    double ncoPhaseTemp;
    
    int phase;
    channelNoTemp = channelNo;
  
    rxPhaseAddrTemp = RX_PHASE_OFFSET_ADDR_INITIAL + channelNoTemp * 2.0;
   // rxPhaseAddr = rxPhaseAddrTemp;
    ncoPhaseTemp = ncoPhase * PHASE_CONVERSION_FACTOR;
    phase = ncoPhaseTemp;
    RxOffsetPhaseSel(0);
    setr(rxPhaseAddrTemp ,phase );
    
//    if(rst==0.0)
      setr(RX_FREQ_EN_ADDR ,65535 );
     
//    else
//    	setr(RX_FREQ_EN_ADDR ,255 );

}

/*********************************************************************/
//
//功能描述：根据平均次数，循环产生标号，在不同phaseCycle下设置不同
//          的相对的相位偏移
//
//入口参数：noAverages，平均次数 
//          phaseCycle  相位循环 0,180,0,180...                      
//
/*********************************************************************/

void RxPhaseCycle(int channelNo, double phaseCycle,double noAverages)
{
	 static double RxPhaseCycleCnt_cnt1 = 0.0;
   static double RxPhaseCycleCnt_phase = 0.0;

	 if(phaseCycle==1.0)
   {
	  RxPhaseCycleCnt_cnt1 = RxPhaseCycleCnt_cnt1 + 1.0;
   	  RxPhaseOffsetReg(channelNo,RxPhaseCycleCnt_phase);
   	  if(RxPhaseCycleCnt_cnt1 == noAverages)
      {
         RxPhaseCycleCnt_cnt1 = 0.0;
         if(RxPhaseCycleCnt_phase == 0.0)
         RxPhaseCycleCnt_phase = 180.0;
         else
         RxPhaseCycleCnt_phase = 0.0;
        }

   }
}
/*********************************************************************/

//功能描述：根据平均次数，循环产生标号
//
//入口参数：noAverages，平均次数                                    
//
//返回参数：标号                                                    
//
/*********************************************************************/
double RxAvePhaseCnt(double noAverages)
{	
	static double RxAvePhase_cnt1 = 0.0;	  
  double averagesCnt;
  
  if(noAverages>1.0)
  {
     if(RxAvePhase_cnt1==noAverages)
        RxAvePhase_cnt1 = 0.0;
           
     RxAvePhase_cnt1 =  RxAvePhase_cnt1+1.0;   
  }else
     RxAvePhase_cnt1 = 0.0;
     
	averagesCnt = RxAvePhase_cnt1;
	
	return averagesCnt;
}



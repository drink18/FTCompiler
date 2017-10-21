/*--------------------------------------------------------------------------------------------------
该软件为私有的保密文件，只有在菲特通信科技有限公司授权的情况下方可使用。                            
(C) COPYRIGHT 2016 FIRSTECH INC. ALL RIGHTS RESERVED                                                
项目名称     :     MRI                                                                           
文件名称     :     grad.h                                                                   
作者         :     zhaoke                                                                       
作者联系方式 :     zhaoke@afirstech.com                                                         
时间         :     2016-01-11                                                                       
版本         :     V1.0                                                                             
关键字       :     Grad                                                                             
文件描述     :                                            
修改情况     :                                                                                      
时间         修改作者           版本          修改说明                                              
=========================================================                                           
2016-1-11    zhaoke             v1.1         创建 
2016-10-28   zhaoke                          梯度旋转矩阵选择函数GradMatSel参数定义为double型   
2017-2-22    zhaoke                          添加函数注释    
2017-3-17    pengjing                        修改所有梯度产生函数的延时和内部命令，新增GradStartnew函数 
2017-3-17    pengjing                        增加快速双回波的相位编码fastDualSeq()     
2017-5-6     zhaoke                          1.添加GradRegStart和GradRamStart宏定义。 
                                             2.GradScaleRam地址参数改为int型，避免类型转换增加的85个clk延时 
                                             3.TrapelutzoidOneFixTrampTimeStart的 scaleRamAddr参数改为int型                                        
=========================================================  */      
#ifndef GRAD_H
#define GRAD_H

#define GRAD_OUTPUT_SEL_ADDR 	        0x101
#define GRAD_DATA_SET_ADDR 	          0x102
#define GRAD_WAVE_CALC_SEL_ADDR_SLICE 0x103
#define GRAD_WAVE_CALC_SEL_ADDR_READ  0x222
#define GRAD_WAVE_CALC_SEL_ADDR_PHASE 0x242
#define GRAD_PARA_RAM_RD_ADDR         0x127
#define GRAD_SAMPLE_PERIOD_ADDR       0x112
#define GRAD_MAT_RAM_RD_ADDR 	        0x111
#define GRAD_SCALE_ADDR               432
#define GRAD_SCALE_RAM_ADDR           0x128
#define GRAD_CALC_SEL_ADDR           0x129

#define GRAD_SRP_SLICE                0x1
#define GRAD_SRP_READ          	      0x2
#define GRAD_SRP_PHASE                0x4
#define S_STEP_LENGTH_ADDR            0x1db
#define S_ADD_NO_ADDR                 0x1dc   
#define WAVE_GEN_TYPE_SEL_ADDR        0x1dd

/*以下是可以直接宏定义的函数，具体各函数的定义及说明详见梯度模块库函数定义部分,*/
/*采用宏的方式声明函数可以大大简化生成的序列指令。                             */
#define GradWaveCalcSelSlice(gradWaveCalcSel)		        setr(GRAD_WAVE_CALC_SEL_ADDR_SLICE,gradWaveCalcSel)
#define GradWaveCalcSelRead(gradWaveCalcSel)		        setr(GRAD_WAVE_CALC_SEL_ADDR_READ ,gradWaveCalcSel)
#define GradWaveCalcSelPhase(gradWaveCalcSel)		        setr(GRAD_WAVE_CALC_SEL_ADDR_PHASE,gradWaveCalcSel)

#define GradScaleReg(gradScale)                         setr(GRAD_SCALE_ADDR,gradScale)
 	
#define GradWaveTable(gradWaveNo)                       setr(GRAD_PARA_RAM_RD_ADDR,gradWaveNo)
#define GradScaleNext()                                 setr(GRAD_SCALE_RAM_ADDR,0x10000)
#define GradScaleRst()                                  setr(GRAD_SCALE_RAM_ADDR,0)

#define GradStart(gradWaveWidth)                        timer(1); \
                                                        setr(GRAD_OUTPUT_SEL_ADDR,1); \
                                                        timer(0,gradWaveWidth-24.0); \
                                                        timer(2)

#define GradStartnew(gradWaveWidth1,gradWaveWidth2)     timer(1); \
                                                        setr(GRAD_OUTPUT_SEL_ADDR,1); \
                                                        timer(0,(gradWaveWidth1+gradWaveWidth2)*US-24.0); \
                                                        timer(2)
//para1: ramp duration time,US
//para2: Hold time, US
//para3: gain
//para4: wave table NO.                                                        
#define GradGainRegStart(gradWaveWidth1,gradWaveWidth2,gain,waveNo)     timer(1); \
                                                        GradScaleReg(gain );\
                                                        GradWaveTable(waveNo);\
                                                        setr(GRAD_OUTPUT_SEL_ADDR,1); \
                                                        timer(0,(gradWaveWidth1+gradWaveWidth2)*US-6.0); \
                                                        timer(2)  
//para1: ramp duration time,US
//para2: Hold time, US
//para3: gain table address
//para4: wave table NO.                                                        
#define GradGainRamStart(gradWaveWidth1,gradWaveWidth2,scaleRamAddr,waveNo)     timer(1); \
                                                        GradScaleRam( scaleRamAddr);\
                                                        GradWaveTable(waveNo);\
                                                        setr(GRAD_OUTPUT_SEL_ADDR,1); \
                                                        timer(0,(gradWaveWidth1+gradWaveWidth2)*US-6.0); \
                                                        timer(2)                                                                                                               

#define GradGainRegStart_2(gradWavepoint,gradStepLength,platWidth,gain) do\
                                                       {\
                                                        double gradWavepoint_double;\
                                                        timer(1); \
                                                        setr(GRAD_CALC_SEL_ADDR,0);\
                                                        setr(WAVE_GEN_TYPE_SEL_ADDR,1);\
                                                        setr(S_STEP_LENGTH_ADDR,gradStepLength);\
                                                        setr(S_ADD_NO_ADDR,gradWavepoint);\
                                                        GradScaleReg(gain );\
                                                        setr(GRAD_OUTPUT_SEL_ADDR,1); \
                                                        gradWavepoint_double = gradWavepoint;\
                                                        timer(0,(gradWavepoint_double+platWidth)*US-42.0); \
                                                        timer(2);\
                                                      }while(0)                                                                                                                                                                       

#define GradGainRamStart_2(gradWavepoint,gradStepLength,platWidth,scaleRamAddr)     do\
                                                      {\
	                                                      double gradWavepoint_double;\
                                                        timer(1); \
                                                        setr(GRAD_CALC_SEL_ADDR,0);\
                                                        setr(WAVE_GEN_TYPE_SEL_ADDR,1);\
                                                        setr(S_STEP_LENGTH_ADDR,gradStepLength);\
                                                        setr(S_ADD_NO_ADDR,gradWavepoint);\
                                                        GradScaleRam(scaleRamAddr );\
                                                        setr(GRAD_OUTPUT_SEL_ADDR,1); \
                                                        gradWavepoint_double = gradWavepoint;\
                                                        timer(0,(gradWavepoint_double+platWidth)*US-42.0); \
                                                        timer(2);\
                                                      }while(0)  

                                                                                                                                                                                                                                
#define GradMatSel( gradRoatMatNO) do\
{\
		int  __gradRoatMatNO;\
		__gradRoatMatNO =gradRoatMatNO; \
    setr(GRAD_MAT_RAM_RD_ADDR,__gradRoatMatNO); \
}while(0)

#define GradScaleRam(scaleRamAddr)	 setr(GRAD_SCALE_RAM_ADDR, scaleRamAddr)

/*************************************************************/
//功能描述：设置参数，产生一段梯形波                         
//入口参数：waveSel1，wave1对应的编号                        
//          waveSel2，wave2对应的编号                        
//          wave1Time，wave1产生需要的时间                   
//          wave2Time，wave2产生需要的时间                   
//          holdTime，wave1和wave2之间的保持时间，最小值可为0
/*************************************************************/
void TrapezoidOneStart(int waveSel1, int waveSel2, double holdTime, double wave1Time, double wave2Time,double graGain)
{
	  double __waveTimeTemp;
	  double __waveTime1;
	  double __waveTime2;
	  double delay_tmp ;
	  timer(1);
	  delay_tmp=6.71;
	  TimerCmp(delay_tmp,US);
	  GradScaleReg(graGain);
	  	  
    /*选择第一段波形*/
    GradWaveTable(waveSel1);
    /*开始发射第一段波形*/
    GradStartnew(wave1Time,holdTime);
    /*设置第一段波形和第二段波形之间的保持时间*/
    /*选择第二段波形*/
    GradWaveTable(waveSel2);
    /*开始发射第二段波形*/
//    GradStart(__waveTime2);
    GradStartnew(wave2Time,0);
}


/**************************************************************/
//功能描述：设置参数， 产生两段梯形波                         
//入口参数：waveSel1， wave1对应的编号                        
//          waveSel2， wave2对应的编号                        
//          waveSel3， wave3对应的编号                        
//          waveSel4， wave4对应的编号                        
//          wave1Time，wave1产生需要的时间                    
//          wave2Time，wave2产生需要的时间                    
//          wave3Time，wave3产生需要的时间                    
//          wave4Time，wave4产生需要的时间                    
//          holdTime1，wave1和wave2之间的保持时间，最小值可为0
//          holdTime2，wave2和wave3之间的保持时间，最小值可为0
//          holdTime3，wave3和wave4之间的保持时间，最小值可为0
/**************************************************************/
void TrapezoidConcatStart(int waveSel1,     int waveSel2,     int waveSel3,    int waveSel4,\
                          double holdTime1, double holdTime2, double holdTime3,double wave1Time,\
                          double wave2Time, double wave3Time, double wave4Time,double graGain1,double graGain2)
{
	  double __waveTimeTemp1;
	  double __waveTimeTemp2;
	  double __waveTimeTemp3;
	  double __waveTime1;
	  double __waveTime2;
	  double __waveTime3;
	  double __waveTime4;  
	  double delay_tmp ;
	  timer(1);
	  delay_tmp=3.35;
	  TimerCmp(delay_tmp,US);
	  GradScaleReg(graGain1);
    /*选择第一段波形*/
    GradWaveTable(waveSel1);
    /*开始发射第一段波形*/
    GradStartnew(wave1Time,holdTime1);
    /*设置第一段波形和第二段波形之间的保持时间*/
    /*选择第二段波形*/
    GradWaveTable(waveSel2);
    /*开始发射第二段波形*/
    GradStartnew(wave2Time,holdTime2);
    /*设置第二段波形和第三段波形之间的保持时间*/
    GradScaleReg(graGain2);
    /*选择第三段波形*/
    GradWaveTable(waveSel3);
    /*开始发射第三段波形*/
    GradStartnew(wave3Time,holdTime3);
    /*设置第三段波形和第四段波形之间的保持时间*/
    /*选择第四段波形*/
    GradWaveTable(waveSel4);
    /*开始发射第四段波形*/
    GradStartnew(wave4Time,0);
}

/**************************************************************/
//功能描述：设置参数， 产生三段梯形波                         
//入口参数：waveSel1， wave1对应的编号                        
//          waveSel2， wave2对应的编号                        
//          waveSel3， wave3对应的编号                        
//          waveSel4， wave4对应的编号                        
//          waveSel5， wave5对应的编号                        
//          waveSel6， wave6对应的编号                        
//          wave1Time，wave1产生需要的时间                    
//          wave2Time，wave2产生需要的时间                    
//          wave3Time，wave3产生需要的时间                    
//          wave4Time，wave4产生需要的时间                    
//          wave5Time，wave5产生需要的时间                    
//          wave6Time，wave6产生需要的时间                    
//          holdTime1，wave1和wave2之间的保持时间，最小值可为0
//          holdTime2，wave2和wave3之间的保持时间，最小值可为0
//          holdTime3，wave3和wave4之间的保持时间，最小值可为0
//          holdTime4，wave4和wave5之间的保持时间，最小值可为0
//          holdTime5，wave5和wave6之间的保持时间，最小值可为0
/**************************************************************/

void TrapezoidConcat3Start(int waveSel1, int waveSel2, int waveSel3, int waveSel4, int waveSel5, int waveSel6,\
                          double holdTime1, double holdTime2, double holdTime3, double holdTime4, double holdTime5, \
                          double wave1Time, double wave2Time, double wave3Time, double wave4Time, double wave5Time, double wave6Time, \
                          double graGain1,  double graGain2 , double graGain3 )
{
	  
	  double __waveTimeTemp1;
	  double __waveTimeTemp2;
	  double __waveTimeTemp3;
	  double __waveTimeTemp4;
	  double __waveTimeTemp5;
	  double __waveTime1;
	  double __waveTime2;
	  double __waveTime3;
	  double __waveTime4;
	  double __waveTime5;
	  double __waveTime6; 
	  double delay_tmp ;

    /*选择第一段波形*/
    GradScaleReg(graGain1);//18
    GradWaveTable(waveSel1);//18
    /*开始发射第一段波形*/
    GradStartnew(wave1Time,holdTime1);
    /*设置第一段波形和第二段波形之间的保持时间*/
    /*选择第二段波形*/
    GradWaveTable(waveSel2);
    /*开始发射第二段波形*/
    GradStartnew(wave2Time,holdTime2);
    /*设置第二段波形和第三段波形之间的保持时间*/
    /*选择第三段波形*/
    GradScaleReg(graGain2);
    GradWaveTable(waveSel3);
    /*开始发射第三段波形*/
    GradStartnew(wave3Time,holdTime3);
    /*设置第三段波形和第四段波形之间的保持时间*/
    /*选择第四段波形*/
    GradWaveTable(waveSel4);
    /*开始发射第四段波形*/
    GradStartnew(wave4Time,holdTime4);
    /*设置第四段波形和第五段波形之间的保持时间*/
    /*选择第五段波形*/
    GradScaleReg(graGain3);
    GradWaveTable(waveSel5);
    /*开始发射第五段波形*/
    GradStartnew(wave5Time,holdTime5);
    /*设置第五段波形和第六段波形之间的保持时间*/
    /*选择第六段波形*/
    GradWaveTable(waveSel6);
    /*开始发射第六段波形*/
    GradStartnew(wave6Time,0);  
       
}
/*************************************************************/
/*功能描述：设置参数，产生一段梯形波 ,固定爬升下降时间       */
/*入口参数：waveSel1，wave1对应的编号                        */
/*          waveSel2，wave2对应的编号                        */
/*          trampTime，wave1、wave2产生需要的时间            */
/*          holdTime，wave1和wave2之间的保持时间，最小值可为0*/
/*************************************************************/
void TrapezoidOneFixTrampTimeStart(int waveSel1, int waveSel2, double holdTime, double trampTime, double graGain)
{
	  double __waveTimeTemp;
	  double __waveTime1;
	  double __waveTime2;
	  double delay_tmp ;
	  timer(1);
	  delay_tmp=7.37;	  
	  GradScaleReg(graGain);
	  TimerCmp(delay_tmp,US);	  
    /*选择第一段波形*/
    GradWaveTable(waveSel1);
    /*开始发射第一段波形*/
    GradStartnew(trampTime,holdTime);
    /*设置第一段波形和第二段波形之间的保持时间*/
    /*选择第二段波形*/
    GradWaveTable(waveSel2);
    /*开始发射第二段波形*/
    GradStartnew(trampTime,0);
}
/**************************************************************/
/*功能描述：设置参数， 产生两段梯形波                         */
/*入口参数：waveSel1， wave1对应的编号                        */
/*          waveSel2， wave2对应的编号                        */
/*          waveSel3， wave3对应的编号                        */
/*          waveSel4， wave4对应的编号                        */
/*          trampTime  wave产生需要的时间                     */
/*          holdTime1，wave1和wave2之间的保持时间，最小值可为0*/
/*          holdTime2，wave2和wave3之间的保持时间，最小值可为0*/
/*          holdTime3，wave3和wave4之间的保持时间，最小值可为0*/
/**************************************************************/
void TrapezoidTwoFixTrampTimeStart(int waveSel1,     int waveSel2,     int waveSel3,    int waveSel4,\
                          double holdTime1, double holdTime2, double holdTime3,double trampTime,\
						  double graGain1,double graGain2)
{
	  double __waveTimeTemp1;
	  double __waveTimeTemp2;
	  double __waveTimeTemp3;
	  double __waveTime1;
	  double __waveTime2;
	  double __waveTime3;
	  double __waveTime4;  
	  double delay_tmp ;
	  timer(1);
	  delay_tmp=4.79;
	  TimerCmp(delay_tmp,US);
	GradScaleReg(graGain1);
    /*选择第一段波形*/
    GradWaveTable(waveSel1);
    /*开始发射第一段波形*/
    GradStartnew(trampTime,holdTime1);
    /*设置第一段波形和第二段波形之间的保持时间*/
    /*选择第二段波形*/
    GradWaveTable(waveSel2);
    /*开始发射第二段波形*/
    GradStartnew(trampTime,holdTime2);
    /*设置第二段波形和第三段波形之间的保持时间*/
    GradScaleReg(graGain2);
    /*选择第三段波形*/
    GradWaveTable(waveSel3);
    /*开始发射第三段波形*/
    GradStartnew(trampTime,holdTime3);
    /*设置第三段波形和第四段波形之间的保持时间*/
    /*选择第四段波形*/
    GradWaveTable(waveSel4);
    /*开始发射第四段波形*/
    GradStartnew(trampTime,0);
    	
}
/**************************************************************/
/*功能描述：设置参数， 产生三段梯形波                         */
/*入口参数：waveSel1， wave1对应的编号                        */
/*          waveSel2， wave2对应的编号                        */
/*          waveSel3， wave3对应的编号                        */
/*          waveSel4， wave4对应的编号                        */
/*          waveSel5， wave5对应的编号                        */
/*          waveSel6， wave6对应的编号                        */
/*          trampTime  wave产生需要的时间                     */
/*          holdTime1，wave1和wave2之间的保持时间，最小值可为0*/
/*          holdTime2，wave2和wave3之间的保持时间，最小值可为0*/
/*          holdTime3，wave3和wave4之间的保持时间，最小值可为0*/
/*          holdTime4，wave4和wave5之间的保持时间，最小值可为0*/
/*          holdTime5，wave5和wave6之间的保持时间，最小值可为0*/
/**************************************************************/

void TrapezoidThreeFixTrampTimeStart(int waveSel1, int waveSel2, int waveSel3, int waveSel4, int waveSel5, int waveSel6,\
                          double holdTime1, double holdTime2, double holdTime3, double holdTime4, double holdTime5, \
                          double trampTime,double graGain1,  double graGain2 , double graGain3 )
{
	  
	  double __waveTimeTemp1;
	  double __waveTimeTemp2;
	  double __waveTimeTemp3;
	  double __waveTimeTemp4;
	  double __waveTimeTemp5;
	  double __waveTime1;
	  double __waveTime2;
	  double __waveTime3;
	  double __waveTime4;
	  double __waveTime5;
	  double __waveTime6; 
	  double delay_tmp ;
	  timer(1);
	  delay_tmp=2.39;
	  TimerCmp(delay_tmp,US);
    /*选择第一段波形*/
    GradScaleReg(graGain1);
    GradWaveTable(waveSel1);
    /*开始发射第一段波形*/
    GradStartnew(trampTime,holdTime1);
    /*设置第一段波形和第二段波形之间的保持时间*/
    /*选择第二段波形*/
    GradWaveTable(waveSel2);
    /*开始发射第二段波形*/
    GradStartnew(trampTime,holdTime2);
    /*设置第二段波形和第三段波形之间的保持时间*/
    /*选择第三段波形*/
    GradScaleReg(graGain2);
    GradWaveTable(waveSel3);
    /*开始发射第三段波形*/
    GradStartnew(trampTime,holdTime3);
    /*设置第三段波形和第四段波形之间的保持时间*/
    /*选择第四段波形*/
    GradWaveTable(waveSel4);
    /*开始发射第四段波形*/
    GradStartnew(trampTime,holdTime4);
    /*设置第四段波形和第五段波形之间的保持时间*/
    /*选择第五段波形*/
    GradScaleReg(graGain3);
    GradWaveTable(waveSel5);
    /*开始发射第五段波形*/
    GradStartnew(trampTime,holdTime5);
    /*设置第五段波形和第六段波形之间的保持时间*/
    /*选择第六段波形*/
    GradWaveTable(waveSel6);
    /*开始发射第六段波形*/
    GradStartnew(trampTime,0);
       
}

/*************************************************************/
/*功能描述：设置参数，产生一段梯形波 ,固定爬升下降时间       */ 
/*          增益为查表值                                   */
/*入口参数：waveSel1，wave1对应的编号                        */
/*          waveSel2，wave2对应的编号                        */
/*          trampTime，wave1、wave2产生需要的时间            */
/*          holdTime，wave1和wave2之间的保持时间，最小值可为0*/
/*************************************************************/
void TrapelutzoidOneFixTrampTimeStart(int waveSel1, int waveSel2, double holdTime, double trampTime, int scaleRamAddr)
{
	double delay_tmp ;
	timer(1);
	delay_tmp=7.37;	
	GradScaleRam(scaleRamAddr);
	TimerCmp(delay_tmp,US);
	  
    /*选择第一段波形*/
    GradWaveTable(waveSel1);
    /*开始发射第一段波形*/
    GradStartnew(trampTime,holdTime);
    /*设置第一段波形和第二段波形之间的保持时间*/
    /*选择第二段波形*/
    GradWaveTable(waveSel2);
    /*开始发射第二段波形*/
    GradStartnew(trampTime,0);
}

/**************************************************************/
/*功能描述：设置参数， 产生两段梯形波                         */
/*入口参数：waveSel1， wave1对应的编号                        */
/*          waveSel2， wave2对应的编号                        */
/*          waveSel3， wave3对应的编号                        */
/*          waveSel4， wave4对应的编号                        */
/*          trampTime  wave产生需要的时间                     */
/*          holdTime1，wave1和wave2之间的保持时间，最小值可为0*/
/*          holdTime2，wave2和wave3之间的保持时间，最小值可为0*/
/*          holdTime3，wave3和wave4之间的保持时间，最小值可为0*/
/**************************************************************/
// void TrapezoidTwoCycleStart(double noCycle,int waveSel1,     int waveSel2,     int waveSel3,    int waveSel4,\
                            // double holdTime1, double holdTime2, double holdTime3,double trampTime,\
						    // double graGain1,  double graGain2)
// {
	  // double __waveTimeTemp1;
	  // double __waveTimeTemp2;
	  // double __waveTimeTemp3;
	  // double __waveTime1;
	  // double __waveTime2;
	  // double __waveTime3;
	  // double __waveTime4;  
	  // double delay_tmp ;
	  // double count = 0.0;
	  // timer(1);
	  // delay_tmp=4.31;
	  // TimerCmp(delay_tmp,US);
	  
	// TrapezoidLoop:
	// GradScaleReg(graGain1);
    // /*选择第一段波形*/
    // GradWaveTable(waveSel1);
    // /*开始发射第一段波形*/
    // GradStartnew(trampTime,holdTime1);
    // /*设置第一段波形和第二段波形之间的保持时间*/
    // /*选择第二段波形*/
    // GradWaveTable(waveSel2);
    // /*开始发射第二段波形*/
    // GradStartnew(trampTime,holdTime2);
    // /*设置第二段波形和第三段波形之间的保持时间*/
    // GradScaleReg(graGain2);
    // /*选择第三段波形*/
    // GradWaveTable(waveSel3);
    // /*开始发射第三段波形*/
    // GradStartnew(trampTime,holdTime3);
    // /*设置第三段波形和第四段波形之间的保持时间*/
    // /*选择第四段波形*/
    // GradWaveTable(waveSel4);
    // /*开始发射第四段波形*/
    // GradStartnew(trampTime,0);
	
	// count = count + 1.0;
    // if(count<noCycle)
    // goto TrapezoidLoop;		
// }


/**************************************************************/
/*功能描述：设置参数， 产生两段梯形波                         */
/*入口参数：waveSel1， wave1对应的编号                        */
/*          waveSel2， wave2对应的编号                        */
/*          noCycle,   一对正负梯度波形的循环次数             */
/*          graGain，  增益                                   */
/*          trampTime  wave产生需要的时间                     */
/*          holdTime， wave1和wave2之间的保持时间，最小值可为1.66*/
/**************************************************************/
void epiReadStart(double noCycle,int waveSel1,int waveSel2,double holdTime,double trampTime,double graGain)
{
	  double __waveTimeTemp2;	     	  
	  double delay_tmp ;
	  double count = 1.0;
	  timer(1);
	    delay_tmp=6.35;	 
	  __waveTimeTemp2 =  holdTime- 1.66;
	  TimerCmp(delay_tmp,US);
	  GradScaleReg(graGain);
	
    /* 选择第一段波形*/
    GradWaveTable(waveSel1);
    /*开始发射第一段波形*/
    GradStartnew(trampTime,holdTime);
    
    TrapezoidLoop: 
 	/* 选择第二段波形*/		       
    GradWaveTable(waveSel2);
    /*开始发射第二段波形*/
    GradStartnew(trampTime,0);
    /*开始发射第三段波形*/
    GradWaveTable(waveSel2);
    /*开始发射第三段波形*/
    GradStartnew(trampTime,holdTime);
    /*选择第四段波形*/
    GradWaveTable(waveSel1);
    /*开始发射第四段波形*/
    GradStartnew(trampTime,0);
	/*选择第五段波形*/
    GradWaveTable(waveSel1);
    /*开始发射第五段波形*/
    GradStartnew(trampTime,__waveTimeTemp2);
    /*循环发射一组梯度波形*/
    count = count + 1.0;
    if(count<noCycle) 
    {  
    goto TrapezoidLoop;	
    }

    /*选择第六段波形*/
    GradWaveTable(waveSel2);
    /*开始发射第六段波形*/
    GradStartnew(trampTime,0);
	/*选择第七段波形*/
    GradWaveTable(waveSel2);
    /*开始发射第七段波形*/
    GradStartnew(trampTime,holdTime);    
	/*选择第八段波形*/
    GradWaveTable(waveSel1);
    /*开始发射第八段波形*/
    GradStartnew(trampTime,0);

}
/**************************************************************/
/*功能描述：设置参数， 产生两段梯形波 该函数与其他梯度波形的间*/
/*          隔时间最少为(tplat+12)微秒，配合epiReadStart使用，*/
/*          延时(trampTime + tplat)微秒输出第一个三角波形     */
/*入口参数：waveSel1， wave1对应的编号                        */
/*          waveSel2， wave2对应的编号                        */
/*          noCycle,   一个三角波形的循环次数                 */
/*          graGain，  增益                                   */
/*          trampTime  wave产生需要的时间                     */
/*          tplat，    三角波形的间隔时间                     */
/**************************************************************/
void epiPhaseStart(double noCycle,int waveSel1,int waveSel2,double tplat,double trampTime,double graGain)
{	    
    double count = 0.0;
    double __waveTimeTemp1;
	double delay_tmp ;
	timer(1);	 
	delay_tmp = trampTime + tplat + 6.35;
	__waveTimeTemp1 = tplat-1.66;
	TimerCmp(delay_tmp,US);
	GradScaleReg(graGain);
	TrapezoidLoop:
    /*选择第一段波形*/
    GradWaveTable(waveSel1);
    /*开始发射第一段波形*/
    GradStartnew(trampTime,0);
    /*选择第二段波形*/
    GradWaveTable(waveSel2);
    /*开始发射第二段波形*/
    GradStartnew(trampTime,__waveTimeTemp1);    
	count = count + 1.0;
	if(count<noCycle)
    goto TrapezoidLoop;	 
}
/**********************************************************/
/*功能描述：产生读梯度的幅度值                            */
/*入口参数：*/
/**********************************************************/
double GardSliceGain(double rfBandwidth,double thickSlice,double thickSliceRatioGain)
{

	double gradsgain;

    gradsgain = rfBandwidth/(thickSlice*thickSliceRatioGain);  
	
	return gradsgain;
}
/********************************************************************/
/*功能描述：产生读梯度的预聚相梯度保持时间或者选层梯度重聚相梯度保持时间 */
/*入口参数： tref 预设梯度保持时间                                     */
/*         tsel180， 180°脉冲保持时间，GE序列中端口赋值0               */
/*         tcrush， GE序列中端口赋值0                                 */
/********************************************************************/
double PreGardTime(double tcrush,double tsel90,double tsel180, double tref, double TE ,double trampTime)
{
	double tmp;
	double timeOut;

	tmp = TE*0.5 - tcrush - tsel90*0.5 -tsel180*0.5  - trampTime*4.0;
	if(tmp< tref)
		timeOut = tmp;
	else
		timeOut = tref;
	return timeOut;
}
/**********************************************************/
/*功能描述：产生选层梯度的crush时间                       */
/*入口参数：*/
/**********************************************************/
double TcrushGenerate(double tcrushIn,double gradsgain, double thickSlice,double thickSliceRatioGain,double trampTime)
{
	double tcrush_double;
    int tcrush_int;
	double tcrush;
	double tmp1;
	
	  tmp1 = 2.0/gradsgain/(thickSlice*thickSliceRatioGain);
	  tcrush_double=1000000.0*(tmp1-0.5*0.000001*trampTime)+tcrushIn;
    tcrush_int = tcrush_double;
    tcrush = tcrush_int;
	return tcrush;
}
/**********************************************************/
/*功能描述：产生读梯度的幅度值                            */
/*入口参数：*/
/**********************************************************/
double GardReadGain(double samplePeriod,double fovRead,double ratiaFreqOversampling)
{
	double read_bandwidth_tmp;
	double read_bandwidth;
	double gradrgain;
	read_bandwidth = AcquireDuration(samplePeriod,1.0);  
    read_bandwidth_tmp = 1000000.0/read_bandwidth;
    gradrgain = read_bandwidth_tmp/fovRead/ratiaFreqOversampling ;
	return gradrgain;
}
/**********************************************************/
/*功能描述：产生相位编码梯度的保持时间                    */
/*入口参数：*/
/**********************************************************/
double GardPhaseTime(double fovPhase,double gainEffPhase,double noScans , double noEchoes, double dummyShot,double trampTime)
{
	double no_View_Block;
	double timeOut;
	no_View_Block = (noScans - dummyShot)*noEchoes;
	timeOut=(no_View_Block-1)/2/fovPhase/gainEffPhase*1000000.0-trampTime;  
	return timeOut;
}

/**********************************************************/
/*功能描述：产生相位编码梯度的最大增益                    */
/*入口参数：*/
/**********************************************************/
double GainEffPhase(double fovPhase,double holdTime,double noScans , double noEchoes, double dummyShot,double trampTime)
{
	double no_View_Block;
	double gainEffPhase;
	no_View_Block = (noScans - dummyShot)*noEchoes;
	gainEffPhase = (no_View_Block-1)/2/fovPhase/(holdTime+trampTime)*1000000.0;
//	timeOut=(no_View_Block-1)/2/fovPhase/gainEffPhase*1000000.0-trampTime;  
	return gainEffPhase;
}
/**********************************************************/
/*功能描述：产生scan标号                                  */
/*入口参数：*/
/**********************************************************/
double GradScanCnt (double noAverages) 
{
	static double AveCnt=0.0;
	static double scanCnt = 0.0;
	double scanCntOut;
      if(noAverages>1)
      {
           if(AveCnt==noAverages)
              AveCnt = 0.0;
            AveCnt =  AveCnt+1.0; 
            if(AveCnt == 1.0)
             scanCnt =  scanCnt+1.0;   
       }else
         scanCnt =  scanCnt+1.0;   	
	
	scanCntOut = scanCnt;
	return scanCntOut;
}
/**********************************************************/
/*功能描述：产生scan标号                                  */
/*入口参数：*/
/**********************************************************/
double GradEnScanCnt(double noAverages) 
{
	static double AveCnt=0.0;
	static double scanCnt = 0.0;
	double scanCntOut;
      if(noAverages>1)
      {
           if(AveCnt==noAverages)
              AveCnt = 0.0;
            AveCnt =  AveCnt+1.0; 
            if(AveCnt == 1.0)
             scanCnt =  scanCnt+1.0;   
       }else
         scanCnt =  scanCnt+1.0;   	
	
	scanCntOut = scanCnt;
	return scanCntOut;
}

/**********************************************************/
/*功能描述：产生scan标号                                  */
/*入口参数：*/
/**********************************************************/
double Grad3DEnScanCnt(double noAverages,double noScans) 
{
	static double AveCnt=0.0;
	static double scanCnt = 0.0;
	double scanCntOut;
	  if(scanCnt == noScans)
		  scanCnt = 0.0;
      if(noAverages>1.0)
      {
           if(AveCnt==noAverages)
              AveCnt = 0.0;
            AveCnt =  AveCnt+1.0; 
            if(AveCnt == 1.0)
             scanCnt =  scanCnt+1.0;   
       }else
         scanCnt =  scanCnt+1.0;   	
	
	scanCntOut = scanCnt;
	return scanCntOut;
}
/**********************************************************/
/*功能描述：产生具有DummyShot的指示信号， 最多有20个DummyShot位置可选*/
/*入口参数：
/**********************************************************/
double DummyShotEn(double dummyShot,double noAverages,double dummy1,double dummy2,double dummy3,double dummy4,\
                 double dummy5,double dummy6,double dummy7,double dummy8,double dummy9,double dummy10,\
				 double dummy11,double dummy12,double dummy13,double dummy14,double dummy15,\
                 double dummy16,double dummy17,double dummy18,double dummy19,double dummy20)
{
	double dummyShotEn = 0.0;
	double dummyShotEn_out;
  double noScansCnt;
  
  noScansCnt = GradScanCnt ( noAverages) ;

  if(dummyShot ==0.0)
	  dummyShotEn = 0.0;
	else{

    if(noScansCnt == dummy1)
	    dummyShotEn = 1.0;
	  if(noScansCnt == dummy2)
		  dummyShotEn = 1.0;
	  if(noScansCnt == dummy3)
		  dummyShotEn = 1.0;
	  if(noScansCnt == dummy4)
		  dummyShotEn = 1.0;
	  if(noScansCnt == dummy5)
		dummyShotEn = 1.0;
	  if(noScansCnt == dummy6)
		dummyShotEn = 1.0;
	  if(noScansCnt == dummy7)
		dummyShotEn = 1.0;
	  if(noScansCnt == dummy8)
		dummyShotEn = 1.0;
	  if(noScansCnt == dummy9)
		dummyShotEn = 1.0;
	  if(noScansCnt == dummy10)
		dummyShotEn = 1.0;
	  if(noScansCnt == dummy11)
	  dummyShotEn = 1.0;
	  if(noScansCnt == dummy12)
		dummyShotEn = 1.0;
	  if(noScansCnt == dummy13)
		dummyShotEn = 1.0;
	  if(noScansCnt == dummy14)
		dummyShotEn = 1.0;
	  if(noScansCnt == dummy15)
		dummyShotEn = 1.0;
	  if(noScansCnt == dummy16)
		dummyShotEn = 1.0;
	  if(noScansCnt == dummy17)
		dummyShotEn = 1.0;
	  if(noScansCnt == dummy18)
		dummyShotEn = 1.0;
	  if(noScansCnt == dummy19)
		dummyShotEn = 1.0;
	  if(noScansCnt == dummy20)
		dummyShotEn = 1.0;
		
	}
	dummyShotEn_out = dummyShotEn;
	return dummyShotEn_out;
}

/**********************************************************/
/*功能描述：相位编码顺序，相同的持续时间，改变梯度幅度值实现相位编码
/*入口参数：effectiveEcho 有效回波序号，单回波时为1.有相位编码的TR数目effectiveNoScans = noScans - dummyShot . 
            echoCnt 单个TR的回波数序号，单回波时为1
            echoBlockCnt 有效TR序号
/**********************************************************/
double ModePhaseCodeGain(double modePhaseCode,double effectiveEcho,double noScans,double noEchoes,double dummyShot,\
                         double orderPhaseCode,double modePartialPhaseSampling,double partialPhaseSampling,\
                         double echoBlockCnt,double echoCnt,double graGain)
{
	double PhaseCodeGain = 0.0;
	double PhaseCodeGain_tmp = 0.0;
	double grad_tmp = 0.0;
	double grad_tmp1 = 0.0;
	double echoBlockCntOdd = 0.0;
	double partialPhaseSamplingHalf = 0.0;
	double comp1;
	double comp2;
	double tmp;
	int tmp1;
	double tmp2;
	double echoBlockCntOdd_tmp1;
	int echoBlockCntOdd_tmp2;
	double echoBlockCntOdd_tmp3;
	double effNoViewsOddTmp1;
	int effNoViewsOddTmp2;
	double effNoViewsOddTmp3;
	double effNoViewsOdd;
	double effectiveNoScans;//有相位编码的TR数目effectiveNoScans = noScans - dummyShot . 
	double effNoViewsHalf; //effNoViewsHalf = (effectiveNoScans * noEchoes +partialPhaseSampling)* 0.5 

	effectiveNoScans = noScans - dummyShot ;
	effNoViewsHalf = (effectiveNoScans * noEchoes +partialPhaseSampling)* 0.5 ;
	if(modePhaseCode == 1.0)
	    grad_tmp=(echoBlockCnt-(effectiveNoScans+1.0)*0.5)+(echoCnt-effectiveEcho)*effectiveNoScans;
    else if (modePhaseCode == 2.0)
		grad_tmp=((effectiveNoScans+1)*0.5- echoBlockCnt)+(echoCnt-effectiveEcho)*effectiveNoScans;
	else if (modePhaseCode == 3.0)
	{
		echoBlockCntOdd_tmp1 = echoBlockCnt/2.0;
		echoBlockCntOdd_tmp2 = echoBlockCntOdd_tmp1;
		echoBlockCntOdd_tmp3 = echoBlockCntOdd_tmp2;
		echoBlockCntOdd = echoBlockCntOdd_tmp1 - echoBlockCntOdd_tmp3;

		effNoViewsOddTmp1 = effNoViewsHalf;
		effNoViewsOddTmp2 = effNoViewsOddTmp1;
		effNoViewsOddTmp3 = effNoViewsOddTmp2;
		effNoViewsOdd = effNoViewsOddTmp1 - effNoViewsOddTmp3;

		if(effNoViewsOdd==0.0)
		{
			tmp = (echoBlockCnt-1.0)/2;
			tmp1 = tmp;
			tmp2 = tmp1;
			if(echoBlockCntOdd==0.0)
		    {
			    grad_tmp = (-1.0)*(0.5+tmp2)+(echoCnt-effectiveEcho)*effectiveNoScans;
		    }else{
				grad_tmp = (0.5+tmp2)+(echoCnt-effectiveEcho)*effectiveNoScans;
			}
		}else{
			tmp = echoBlockCnt/2.0;
			tmp1 = tmp;
			tmp2 = tmp1;
			if(echoBlockCntOdd==0.0)
		    {
			    grad_tmp = (-1.0)*tmp2+(echoCnt-effectiveEcho)*effectiveNoScans;
		    }else{
				grad_tmp = tmp2+(echoCnt-effectiveEcho)*effectiveNoScans;
			}
		}

	}
	
    grad_tmp1 = 0.0-grad_tmp;
    if (grad_tmp>effNoViewsHalf)
        grad_tmp=grad_tmp-effNoViewsHalf*2.0;
    if (grad_tmp1>effNoViewsHalf)
        grad_tmp=grad_tmp+effNoViewsHalf*2.0;
	
	partialPhaseSamplingHalf = partialPhaseSampling/2.0;
	if(partialPhaseSampling>0.0)
	{
	if(noEchoes  == 1.0)
	{
		if(modePhaseCode <3.0)
		{
			if(modePartialPhaseSampling==1.0)
                grad_tmp=grad_tmp-partialPhaseSamplingHalf;
            else
            	if(modePartialPhaseSampling==2)
                    grad_tmp=partialPhaseSamplingHalf-grad_tmp;
		}else{
					
			if(modePartialPhaseSampling==1.0)
			{
				comp1 = grad_tmp - effNoViewsHalf + partialPhaseSamplingHalf;	
				if(comp1>0.0)
					grad_tmp = 0.0-grad_tmp-partialPhaseSamplingHalf;
			}else{
				if(modePartialPhaseSampling==2)
				{
				    comp2 = grad_tmp + effNoViewsHalf - partialPhaseSamplingHalf;
				    if(comp2<0.0)
				    	grad_tmp = 0.0-grad_tmp+partialPhaseSamplingHalf;
				}
			}
				
		}
			
	}else{
		if(modePartialPhaseSampling>0.0)
		{
			if(modePartialPhaseSampling==1.0)
			{
				comp1 = grad_tmp - effNoViewsHalf + partialPhaseSamplingHalf;
				if(comp1>0.0)
					grad_tmp = 0.0-grad_tmp-partialPhaseSamplingHalf;
			}else{
				if(modePartialPhaseSampling==2)
				{
				    comp2 = grad_tmp + effNoViewsHalf - partialPhaseSamplingHalf;
				    if(comp2<0.0)
				    	grad_tmp = 0.0-grad_tmp+partialPhaseSamplingHalf;
				}
			}
		}
	}
    }
	PhaseCodeGain = graGain*grad_tmp/effNoViewsHalf;

	return PhaseCodeGain;
}

/**********************************************************/
/*功能描述：快速双回波的相位编码
/*入口参数：views_per_seg ：noEchoes
            shared_echoes 两个图像采集共享回波数
            te PDWI成像有效TE
			te_2 T2WI成像有效TE
			gp_loc 有效TR序号
			echo_cnt 单个TR的回波数序号
/**********************************************************/
double fastDualSeq(double no_views,double views_per_seg,double shared_echoes,double te,double te_2,double gp_loc,double echo_cnt)
{
	static int echo_centre, echo_location, echo_location0,extra_echo;
	int views_per_echo, phase_count;
	int no_views_eff, u_echoes, echo_sum, extra_val;
	int te_eff,l_limit,jump;
	static int array_count = 0;
	static int array_count1 = 0;
	static int phase_offset;
	static int gp_loc_tmp=0;
	int gp_cnt_tmp,echo_gain;
	static int current_view = 0;
	int gp_mul;
	double out;

	te_eff = te_2/te; 
	u_echoes = (views_per_seg-shared_echoes)/2; 
	echo_sum = u_echoes+shared_echoes; 
	no_views_eff = (no_views*(views_per_seg-u_echoes))/views_per_seg; 
	if (u_echoes > 1) 
	{	l_limit = echo_sum+1;//7
		jump = 0;
	}else {	l_limit = 2;
		jump = 1;
	}

	views_per_echo = no_views/(2*views_per_seg); 
	phase_count = views_per_echo/2; 
	
	if(gp_loc == gp_loc_tmp)
		gp_loc_tmp = gp_loc_tmp;
    else{
		gp_loc_tmp = gp_loc;
		if(gp_loc==1)
	    phase_offset = phase_count/2;
	//echo_location
    if(gp_loc<=phase_count)
	{
		array_count = array_count+1;
		echo_location = phase_count-array_count+phase_offset;
		echo_location0 = phase_count-array_count;
		phase_offset = phase_offset-1;
	    if(phase_offset < 0) 
			phase_offset = 0;
	
	}		
	else if(gp_loc<=views_per_echo)
	{
		if(array_count <= phase_count/2) 
			phase_offset = phase_offset+1;
		echo_location = array_count-phase_count-phase_offset-1;
		echo_location0 = array_count-phase_count-1;
		array_count = array_count-1;		
	}	    
    else if(gp_loc<=views_per_echo+phase_count)
	{
		array_count = array_count+1;
		echo_location = phase_count-array_count+phase_offset;
		echo_location0 = phase_count*2.0-array_count+phase_offset;
		phase_offset = phase_offset-1;
	    if(phase_offset < 0) 
			phase_offset = 0;
	}
	else{
		if(array_count <= phase_count/2) 
			phase_offset = phase_offset+1;	
		echo_location = array_count-phase_count-phase_offset-1;
		echo_location0 = array_count-phase_count*2.0-phase_offset-1;
		array_count = array_count-1;			
	}
	
	}

//echo_centre
    if (current_view > (no_views/2)-1)	
		gp_cnt_tmp = echo_cnt+1+views_per_seg;
	else
		gp_cnt_tmp = echo_cnt+1;	
	
	current_view = current_view+1;
	
	if(array_count1==views_per_seg)
		array_count1 = 0;
	array_count1 = array_count1+1;	
	
	if(gp_cnt_tmp<=views_per_seg)
	    echo_gain = 1;
    else
	    echo_gain = -1;

	if(array_count1==1){
		echo_centre = 0;
		extra_echo = 0;
	}else{
		if(array_count1==te_eff){
			echo_centre = 0;	
            extra_echo = 1; 		
		}else{
			echo_centre = ((views_per_echo*(array_count1-1)) + phase_count)*echo_gain;
			if(array_count1<te_eff){	
			    if (array_count1 > echo_sum) 			
					echo_centre = (views_per_echo*(te_eff-array_count1)+ phase_count)*echo_gain;
			}else{
				if (array_count1 > echo_sum+1) 
					echo_centre = (views_per_echo*(array_count1-echo_sum-1)+ phase_count)*echo_gain;
			}
			if(array_count1>u_echoes){
				if(array_count1<=echo_sum+jump)
				    echo_centre= ((array_count1-extra_echo)*views_per_echo - phase_count)*echo_gain;	                    
			}
			
		}
	}
	
	if (echo_cnt == 0 | echo_cnt == te_eff-1)	{
		gp_mul = echo_centre + echo_location0 ;
		}
	else	{
		gp_mul = echo_centre + echo_location;
		}
	if(gp_mul < (-no_views_eff/2)) gp_mul = gp_mul + no_views_eff;
	if((gp_mul+1) > (no_views_eff/2)) gp_mul = gp_mul - no_views_eff;
	out = gp_mul;
	
	return out;
}

#endif 
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      
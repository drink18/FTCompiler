/*--------------------------------------------------------------------------------------------------
该软件为私有的保密文件，只有在菲特通信科技有限公司授权的情况下方可使用。                            
(C) COPYRIGHT 2016 FIRSTECH INC. ALL RIGHTS RESERVED                                                
项目名称     :     MRI                                                                           
文件名称     :     common.h                                                                   
作者         :     zhaoke                                                                       
作者联系方式 :     zhaoke@afirstech.com                                                         
时间         :     2016-01-11                                                                       
版本         :     V1.0                                                                             
关键字       :     Common                                                                             
文件描述     :                                            
修改情况     :                                                                                      
时间         修改作者           版本          修改说明                                              
=========================================================                                           
2016-1-11    zhaoke             v1.1         创建 
2016-5-25    pengjing                        修改  PHASE_CONVERSION_FACTOR值  
2017-2-22    zhaoke                          添加函数注释                                                                                         
=========================================================    */    
#ifndef COMMON_H
#define COMMON_H
//100MHz clk 10ns period
#define REFClk                   10.0

#define NS                       0.1
#define US                       100.0
#define MS                       100000.0
#define S                        100000000.0
//#define FREQ_CONVERSION_FACTOR   1342177.28
#define MHZ                      2684354.56
#define KHZ                      2684.35456
#define  HZ                      2.68435456
#define PHASE_CONVERSION_FACTOR  745654.0444444
#define GAIN_FACTOR              32.0
#define ONE_CYCLE 1.0
#define CHECK      0
#define INTERNAL_TRIGGER 0

#define CHANNEL1             1
#define CHANNEL2             2
#define CHANNEL3             3
#define CHANNEL4             4
#define CHANNEL5             5
#define CHANNEL6             6
#define CHANNEL7             7
#define CHANNEL8             8

/*第一次Trigger来临时，则执行变量更新;第二次Trigger来临时，进行事件触发。*/
#define WaitTrigger()	                  wait(1,1); \
  				                              update;\
  				                              wait(1,1)


#define Delay(delayValue , delayUnit)            do {\
	                                                     double __realDelayTime;\
	                                                     double __delayValueTemp;\
	                                                     __delayValueTemp = delayValue;\
	                                                     __realDelayTime = __delayValueTemp*delayUnit-222.0;\
	                                                     delay(__realDelayTime);\
	                                                 }while(0)

#define TimerCmp(timerValue , timerUnit)         timer(0,(timerValue)*timerUnit)


/**************************************************/
/*功能描述：将时间转化为以时钟周期为单位          */
/*入口参数：timeValue:时间值。                    */
/*          timeUnit:ns,us,ms,s对应的时钟周期数。 */
/**************************************************/
double TimeToClkCycle(double timeValue, double timeUnit)
{
    double __clkNo;
    __clkNo = timeValue * timeUnit;
    return __clkNo;

}

/**************************************************/
/*功能描述：计算采样时间                           */
/*入口参数：__samplePeriod:采样间隔，单位us。      */
/*          __samplePoint:采样点数                */
/*返回值：采样时间，单位us                         */
/*CAUTION:此函数不同于RxChannelAcquirePara()!!!!!!!!!!!!!       */
/**************************************************/
double AcquireDuration(double __samplePeriod, double __samplePoint)
{

    double __waveReceiveTime;
   //计算接收波形所需要的时间

    __waveReceiveTime = __samplePoint * __samplePeriod;
    return __waveReceiveTime;
    
}    
#endif

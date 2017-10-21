/*--------------------------------------------------------------------------------------------------
该软件为私有的保密文件，只有在菲特通信科技有限公司授权的情况下方可使用。                            
(C) COPYRIGHT 2016 FIRSTECH INC. ALL RIGHTS RESERVED                                                
项目名称     :     MRI                                                                           
文件名称     :     mainctrl.h                                                                   
作者         :     zhaoke                                                                       
作者联系方式 :     zhaoke@afirstech.com                                                         
时间         :     2016-01-11                                                                       
版本         :     V1.0                                                                             
关键字       :     mainctrl                                                                             
文件描述     :                                            
修改情况     :                                                                                      
时间         修改作者           版本          修改说明                                              
=========================================================                                           
2016-1-11    zhaoke             v1.1         创建 
2017-2-22    zhaoke                          添加函数注释                                                                                                  
=========================================================   */     
#ifndef MAINCTRL_H
#define MAINCTRL_H
#define INNER_TRIGGER               0
#define EXTERNAL_TRIGGER            1
#define MAIN_CTRL_TRIGGER_SEL_ADDR  0X91

/*************************************************************/
/*功能描述：等待特定的时间发生                                 */ 
/*入口参数：eventNo,事件号(0-31)                              */
/*          eventLevel:等待事件处于什么状态，有2种，高低电平.  */
/* Port     eventNO                                          */
/* USERIN0    24                                             */
/* USERIN1    25                                             */
/* USERIN2    26                                             */
/* USERIN3    27                                             */
/*************************************************************/

#define WaitEvent(eventLevel,eventNo)	                  wait(eventLevel,eventNo)
/*==========================================主控模块库函数定义====================================*/

/**********************************************************/
/*功能描述：主控板产生宽度为 1个 时钟周期的Tirgger脉冲*/ 

/**********************************************************/
void GenerateTrigger()
{
      static double flag = 0;
       if(flag==0){
       	flag = 1;
       	Delay(10.0,MS);
       	}

	  //等待上位机传送更新变量命令
	  wait(0,0);
	  //产生变量更新trigger
    trigger(4.0);
    //各板用3us时间更新变量	
    Delay(3.0,US);
    update;
    wait(1,2);
    //产生时间更新trigger
    trigger(4.0);
}

/**********************************************************/
/*功能描述：主控板产生Trigger信号                         */ 
/*入口参数：noAverages，平均次数                          */
/*          noScans, TR次数                               */
/*          TR,扫描间隔,单位MS   */
/**********************************************************/
double MainctrlTrigger_scan_cnt = 0.0;
double MainctrlTrigger_Ave_cnt = 0.0;
void MainctrlTrigger(double noAverages,double noScans,double TR)
{	
    main_ctrl_start:
    MainctrlTrigger_Ave_cnt = 0.0;
    noAverages_loop: 
    GenerateTrigger(); 
    TimerCmp(TR,MS); 
	  MainctrlTrigger_Ave_cnt = MainctrlTrigger_Ave_cnt + 1.0;    
    if(MainctrlTrigger_Ave_cnt < noAverages)
    goto noAverages_loop;
    
    MainctrlTrigger_scan_cnt = MainctrlTrigger_scan_cnt + 1.0;    
    if(MainctrlTrigger_scan_cnt < noScans)
    goto main_ctrl_start;
    Lend:
    end;
}

#endif 	
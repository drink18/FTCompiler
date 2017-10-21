/*--------------------------------------------------------------------------------------------------
该软件为私有的保密文件，只有在菲特通信科技有限公司授权的情况下方可使用。                            
(C) COPYRIGHT 2016 FIRSTECH INC. ALL RIGHTS RESERVED                                                
项目名称     :     MRI                                                                           
文件名称     :     var.h                                                                   
作者         :     zhaoke                                                                       
作者联系方式 :     zhaoke@afirstech.com                                                         
时间         :     2016-01-11                                                                       
版本         :     V1.0                                                                             
关键字       :     var                                                                             
文件描述     :                                            
修改情况     :                                                                                      
时间         修改作者           版本          修改说明                                              
=========================================================                                           
2016-1-11    zhaoke             v1.1         创建 
2016-1=14    yuanjian           V1.2         增加gradScaleRamAddr                                                                                                   
=========================================================                                           
--------------------------------------------------------------------------------------------------*/

int gradScaleRamAddr ;
int noEchoes      ;
int noSlices      ;
int noSliceBlock  ;
int noViewBlock   ;
int noAverages    ;
int noViews       ;
int noViewsSec    ;
int noSamples     ;
int noScans       ;
int bandWidthNo   ;
int noDiscard     ;
double repetitionDelay  ;//us
double echoDelay        ;//us
double receiverGain     ;//total dB
double sysFrequency     ;//nco frequency?
//tx
double txAtt           ;
int rfShape            ;
double rf90Scale       ;
double rf180Scale      ;

int rfSpoil            ;
double seAsmmetry      ;

int inGating           ;
double cardiacDelay    ;

//Gradient
double sliceThickness  ;//mm
double sliceSeparation ;//mm
double rampTime        ;//us
int    graOrientation     ;// 0: 1: 2: 3: 4: 5:?
int    offsetX            ;//DAC unit
int    offsetY            ;//DAC unit
int    offsetZ            ;//DAC unit
int    offsetBzero        ;//DAC unit
int    gateSlice          ;
int    gateRead           ;
int    gatePhase          ;
int    rephaseLobe        ; //us
int    sliceGraComp       ; //us
int    readGraComp        ; //us
int    phaseGraComp       ; //us
double crusherPiFID           ;
double sliceRehpase           ;
double crusherPresatDur       ;
double crusherPresatAmp       ;
double presatOrientation      ;
double presatRF               ;
double presatRFShape          ;
double presatScale            ;
double mtcPulse               ;
double mtcScale               ;
double mtcCrusherDur          ;
double mtcCrusherAmp          ;
double mtcpulseoffset         ;
double mtcPulseDela           ;



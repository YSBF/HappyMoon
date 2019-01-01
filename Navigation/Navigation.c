#include "Navigation.h"
NAVGATION_t nav;
KalmanVel_t kalmanVel;
Kalman_t kalmanPos;
FPS_Navigation FPSNavigation;
void KalmanVelInit(void);
void KalmanPosInit(void);
/**********************************************************************************************************
*函 数 名: NavigationInit
*功能说明: 导航参数初始化
*形    参: 无
*返 回 值: 无
**********************************************************************************************************/
void NavigationInit(void)
{
    KalmanVelInit();
    KalmanPosInit();
}

/**********************************************************************************************************
*函 数 名: KalmanPosInit
*功能说明: 位置估计的卡尔曼结构体初始化
*形    参: 无
*返 回 值: 无
**********************************************************************************************************/
void KalmanPosInit(void)
{
    float qMatInit[9] = {0.003, 0, 0, 0, 0.003, 0, 0, 0, 0.003};
    float rMatInit[9] = {500, 0,  0, 0, 500, 0, 0, 0, 500};
    float pMatInit[9] = {3, 0, 0, 0, 3, 0, 0, 0, 3};
    float fMatInit[9] = {1, 0, 0, 0, 1, 0, 0, 0, 1};
    float hMatInit[9] = {1, 0, 0, 0, 1, 0, 0, 0, 1};
    float bMatInit[9] = {1, 0, 0, 0, 1, 0, 0, 0, 1};

    //初始化卡尔曼滤波器的相关矩阵
    KalmanQMatSet(&kalmanPos, qMatInit);
    KalmanRMatSet(&kalmanPos, rMatInit);
    KalmanBMatSet(&kalmanPos, bMatInit);
    KalmanCovarianceMatSet(&kalmanPos, pMatInit);
    KalmanStateTransMatSet(&kalmanPos, fMatInit);
    KalmanObserveMapMatSet(&kalmanPos, hMatInit);

    //状态滑动窗口，用于解决卡尔曼状态估计量与观测量之间的相位差问题
    kalmanPos.slidWindowSize = 100;
    kalmanPos.fuseDelay.x = 100;    //0.1s延时
    kalmanPos.fuseDelay.y = 100;    //0.1s延时
    kalmanPos.fuseDelay.z = 100;    //0.1s延时
}
/**********************************************************************************************************
*函 数 名: KalmanVelInit
*功能说明: 飞行速度估计的卡尔曼结构体初始化
*形    参: 无
*返 回 值: 无
**********************************************************************************************************/
void KalmanVelInit(void)
{
	
    float qMatInit[6][6] = {{0.05, 0, 0, 0, 0, 0},
                            {0, 0.05, 0, 0, 0, 0},
                            {0, 0, 0.05, 0, 0, 0},      
                            {0.003, 0, 0, 0, 0, 0},
                            {0, 0.003, 0, 0, 0, 0},
                            {0, 0, 0.003, 0, 0, 0}};

    float rMatInit[6][6] = {{50, 0, 0, 0, 0, 0},            //VIO速度x轴数据噪声方差
                            {0, 50, 0, 0, 0, 0},            //VIO速度y轴数据噪声方差
                            {0, 0, 50, 0, 0, 0},            //VIO速度z轴数据噪声方差     
                            {0, 0, 0, 2500, 0, 0},          //气压速度数据噪声方差
                            {0, 0, 0, 0, 2000, 0},          //TOF速度数据噪声方差
                            {0, 0, 0, 0, 0, 500000}};       //z轴速度高通滤波系数

    float pMatInit[6][6] = {{5, 0, 0, 0, 0, 0},
                            {0, 5, 0, 0, 0, 0},
                            {0, 0, 5, 0, 0, 0},      
                            {2, 0, 0, 2, 0, 0},
                            {0, 2, 0, 0, 2, 0},
                            {0, 0, 6, 0, 0, 2}};    //增大协方差P的初值，可以提高初始化时bias的收敛速度

    float hMatInit[6][6] = {{1, 0, 0, 0, 0, 0},
                            {0, 1, 0, 0, 0, 0},
                            {0, 0, 1, 0, 0, 0},      
                            {0, 0, 1, 0, 0, 0},
                            {0, 0, 1, 0, 0, 0},
                            {0, 0, 1, 0, 0, 0}};    //h[5][2]:速度z轴增加少许高通滤波效果

    float fMatInit[6][6] = {{1, 0, 0, 0, 0, 0},
                            {0, 1, 0, 0, 0, 0},
                            {0, 0, 1, 0, 0, 0},      
                            {0, 0, 0, 1, 0, 0},
                            {0, 0, 0, 0, 1, 0},
                            {0, 0, 0, 0, 0, 1}};
    
    float bMatInit[6][6] = {{1, 0, 0, 0, 0, 0},
                            {0, 1, 0, 0, 0, 0},
                            {0, 0, 1, 0, 0, 0},      
                            {0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0}};
    
    //初始化卡尔曼滤波器的相关矩阵
    KalmanVelQMatSet(&kalmanVel, qMatInit);
    KalmanVelRMatSet(&kalmanVel, rMatInit);
    KalmanVelCovarianceMatSet(&kalmanVel, pMatInit);
    KalmanVelObserveMapMatSet(&kalmanVel, hMatInit);
    KalmanVelStateTransMatSet(&kalmanVel, fMatInit);
    KalmanVelBMatSet(&kalmanVel, bMatInit);
                            
    //状态滑动窗口，用于解决卡尔曼状态估计量与观测量之间的相位差问题
    kalmanVel.slidWindowSize = 100;
    kalmanVel.fuseDelay[VIO_VEL_X] = 100;    //VIO速度x轴数据延迟参数：0.1s
    kalmanVel.fuseDelay[VIO_VEL_Y] = 100;    //VIO速度y轴数据延迟参数：0.1s
    kalmanVel.fuseDelay[VIO_VEL_Z] = 100;    //VIO速度z轴数据延迟参数：0.1s
    kalmanVel.fuseDelay[BARO_VEL]  = 100;    //气压速度数据延迟参数：0.1s
    kalmanVel.fuseDelay[TOF_VEL]   = 100;    //TOF速度数据延迟参数：
}

/**********************************************************************************************************
*函 数 名: VelocityEstimate
*功能说明: 
*形    参: 无
*返 回 值: 无
**********************************************************************************************************/
void VelocityEstimate(void)
{
	OS_ERR err;
	Vector3f_t VIOVel;
	static uint32_t count;
	static bool fuseFlag;
			
	//计算时间间隔，用于积分
	FPSNavigation.VelCurrentTime = (OSTimeGet(&err) - FPSNavigation.VelLastTime) * 1e-3;
	FPSNavigation.VelCurrentTime = ConstrainFloat(FPSNavigation.VelCurrentTime, 0.0005, 0.005);
	FPSNavigation.VelLastTime = OSTimeGet(&err);

	//获取运动加速度
	nav.accel = EarthAccGetData();

	//加速度数据更新频率1KHz，VIO频率10hz
	//融合的频率强制统一为20Hz 
	if(count++ % 50 == 0)
	{
		VIOVel = GetVisualOdometryVel();
		nav.velMeasure[0] = VIOVel.x;           //VIO速度x轴
		nav.velMeasure[1] = VIOVel.y;           //VIO速度y轴
		nav.velMeasure[2] = VIOVel.z;           //VIO速度z轴
		nav.velMeasure[3] = 0;  								//气压速度值
		nav.velMeasure[4] = 0;                  //TOF速度值
		nav.velMeasure[5] = 0; 
			
		//启用VIO观测量
		KalmanVelUseMeasurement(&kalmanVel, VIO_VEL_Z, true);
		
		//禁用气压观测
		KalmanVelUseMeasurement(&kalmanVel, BARO_VEL, false);
					
		//禁用TOF速度观测量：尚未装备TOF传感器
		KalmanVelUseMeasurement(&kalmanVel, TOF_VEL, false);
			
		fuseFlag = true;
	}
	else
	{
			fuseFlag = false;
	}
	
	/*
	更新卡尔曼滤波器
	估计飞行速度及加速度bias
	*/
	KalmanVelUpdate(&kalmanVel, &nav.velocity, &nav.accel_bias, nav.accel, nav.velMeasure, FPSNavigation.VelCurrentTime, fuseFlag);
}

/**********************************************************************************************************
*函 数 名: PositionEstimate
*功能说明: 
*形    参: 无
*返 回 值: 无
**********************************************************************************************************/
void PositionEstimate(void)
{
	OS_ERR err;
	Vector3f_t input;
	static uint32_t count;
	static bool fuseFlag;

	//计算时间间隔，用于积分
	FPSNavigation.PosCurrentTime = (OSTimeGet(&err) - FPSNavigation.PosLastTime) * 1e-3;
	FPSNavigation.PosCurrentTime = ConstrainFloat(FPSNavigation.PosCurrentTime, 0.0005, 0.002);
	FPSNavigation.PosLastTime = OSTimeGet(&err);

	//速度数据更新频率1KHz，VIO数据只有10hz
	//这里将VIO更新频率拉到20hz
	if(count++ % 50 == 0)
	{
		//获取VIO位置
		nav.posMeasure = GetVisualOdometryPos();

		fuseFlag = true;
	}
	else
	{
		fuseFlag = false;
	}

	//速度积分
	input.x = nav.velocity.x * FPSNavigation.PosCurrentTime;
	input.y = nav.velocity.y * FPSNavigation.PosCurrentTime;
	input.z = nav.velocity.z * FPSNavigation.PosCurrentTime;

	//位置估计
	KalmanUpdate(&kalmanPos, input, nav.posMeasure, fuseFlag);
	nav.position = kalmanPos.state;
}

/**********************************************************************************************************
*函 数 名: NavigationReset
*功能说明: 导航相关数据复位
*形    参: 无
*返 回 值: 无
**********************************************************************************************************/
void NavigationReset(void)
{
    kalmanVel.state[0] = 0;
    kalmanVel.state[1] = 0;
    kalmanVel.state[2] = 0;

}
/**********************************************************************************************************
*函 数 名: GetCopterAccel
*功能说明: 获取飞行加速度
*形    参: 无
*返 回 值: 加速度值
**********************************************************************************************************/
Vector3f_t GetCopterAccel(void)
{
    return nav.accel;
}
/**********************************************************************************************************
*函 数 名: GetAccelBias
*功能说明: 获取加速度bias
*形    参: 无
*返 回 值: 加速度bias值
**********************************************************************************************************/
Vector3f_t GetAccelBias(void)
{
    return nav.accel_bias;
}
/**********************************************************************************************************
*函 数 名: GetCopterVelocity
*功能说明: 获取飞行速度估计值
*形    参: 无
*返 回 值: 速度值
**********************************************************************************************************/
Vector3f_t GetCopterVelocity(void)
{
    return nav.velocity;
}
/**********************************************************************************************************
*函 数 名: GetCopterVelMeasure
*功能说明: 获取飞行速度测量值
*形    参: 无
*返 回 值: 速度值
**********************************************************************************************************/
float* GetCopterVelMeasure(void)
{
    return nav.velMeasure;
}
/**********************************************************************************************************
*函 数 名: GetCopterPosition
*功能说明: 获取位置估计值
*形    参: 无
*返 回 值: 位置值
**********************************************************************************************************/
Vector3f_t GetCopterPosition(void)
{
    return nav.position;
}
/**********************************************************************************************************
*函 数 名: GetCopterPosMeasure
*功能说明: 获取位置测量值
*形    参: 无
*返 回 值: 速度值
**********************************************************************************************************/
Vector3f_t GetCopterPosMeasure(void)
{
    return nav.posMeasure;
}
/**********************************************************************************************************
*函 数 名: GetFPSNavigationPos
*功能说明: 获取位置融合FPS
*形    参: 无
*返 回 值: 
**********************************************************************************************************/
float GetFPSNavigationPos(void)
{
    return FPSNavigation.PosCurrentTime;
}
/**********************************************************************************************************
*函 数 名: GetFPSNavigationVel
*功能说明: 获取位置融合FPS
*形    参: 无
*返 回 值: 
**********************************************************************************************************/
float GetFPSNavigationVel(void)
{
    return FPSNavigation.VelCurrentTime;
}


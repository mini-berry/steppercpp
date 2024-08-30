# 基于C++的STM32步进电机库
## 使用指南
- 如果你使用Keil\
如果你使用ac6：在编译器配置中增加-xc++ -std=c++11\
项目不支持ac5，因为没有std::forward\
你还需要注意，中断回调函数可能需要extern C来避免错误\
eide配置相同
- 如果你使用CMake\
将main.c改为main.cpp\
在cmake文件夹的.cmake文件中将main.c改为main.cpp
## 函数使用方法
- 配置引脚\
需要一个dir_pin和一个pul_pin，提前配置为Output模式
- 时钟配置\
需要一个us定时器作为基准时钟，默认为tim7，分频71。重载值最大即可。\
可能需要在.hpp文件中修改TIM_TYPE，默认uint16_t\
如果你选的定时器最大为65535，可以不管\
如果更大需要可选配置为更大位数（如F4系列板子，提供了更大的定时器）\
别忘了开启定时器！
```cpp
HAL_TIM_Base_Start(&US_TIM);
```
- 新建电机对象\
Stepper M(dirPort, dirPin, pulPort, pulPin)\
填入GPIOA,GPIO_PIN_5之类即可
- 设置速度
``` cpp
#define MAXSPEED 500
#define HIGHTIME 250
#define HIGHATTACHTIME 250
#define MAXATTACHTIME 20000
```
.hpp内宏定义可修改\
默认最大速度500，对应的高低电平时间为250。\
默认最小速度低电平时间20000，改大此项可以有效改进加减速过程中的抖动。
- 加减速配置
``` cpp
#define ACCFORM 0
```
提供两种加速方式\
0：每个脉冲改变速度，根据速度计算时间\
1：每个脉冲改变时间，固定减少一定时间\
可以通过M.SetAccWay(0)设置
- 目标设置
``` cpp
M.SetTarget(100);
```
- 反转设置
``` cpp
M.SetReverse();
```
关闭
``` cpp
M.SetReverse(false);
```
- 运行\
执行Stepper::RunAll(M)即可\
多个电机可以执行Stepper::RunAll(M1,M2...)\
会阻塞运行！需要改进可以添加中断定时执行M.Run()\
最多带多少电机没试过，理论带50个没问题
## 例子
共阳极接法
```cpp
Stepper M(GPIOC, GPIO_PIN_5, GPIOC, GPIO_PIN_4);
HAL_TIM_Base_Start(&htim7);
while (1)
{
    M.SetTarget(1000);
    Stepper::RunAll(M);
    HAL_Delay(1000);
    M.SetTarget(0);
    Stepper::RunAll(M);
    HAL_Delay(1000);
}
```

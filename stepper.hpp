#ifndef STEPPER_H
#define STEPPER_H

// 提供两种加速方式
// 0：每个脉冲改变速度，根据速度计算时间
// 1：每个脉冲改变时间，固定减少一定时间
#define ACCFORM 0
#define MAXSPEED 500
#define HIGHTIME 250
#define HIGHATTACHTIME 250
#define MAXATTACHTIME 20000
#define ACC 1

// us定时器选择
#define US_TIM htim7
// us定时器的类型配置，不配置延时必出问题！
#define TIM_TYPE uint16_t

#include "main.h"
#include "tim.h"
#include <utility>

void usInit();
TIM_TYPE GetUsTick();
void delayUs(TIM_TYPE us);

template <typename T>
inline T abs(const T &input)
{
    return (input > 0) ? input : -input;
}

template <typename T>
inline T constrain(T &input, T maxvalue)
{
    input = (input > maxvalue) ? maxvalue : input;
    input = (input < -maxvalue) ? -maxvalue : input;
}

class Stepper
{
private:
    int32_t calStopLength(int32_t speed);
    void slowdown();
    void fastup();
    void spin(const int32_t &speed);

    uint8_t _accway = ACCFORM;
    // 正反转设置
    GPIO_PinState _forward = GPIO_PIN_RESET;
    GPIO_PinState _backward = GPIO_PIN_SET;
    // 方向GPIO GPIO_TypeDef *
    GPIO_TypeDef *_dirPort;
    // 方向Pin uint16_t
    uint16_t _dirPin;
    // 脉冲GPIO GPIO_TypeDef *
    GPIO_TypeDef *_pulPort;
    // 脉冲Pin uint16_t
    uint16_t _pulPin;
    // 最大速度
    int32_t _maxspeed = MAXSPEED;
    // 高电平时间(us)
    TIM_TYPE _highTime = HIGHTIME;
    // 最大速度的空闲时间
    TIM_TYPE _highSpeedAttachTime = HIGHATTACHTIME;
    // 最小速度的空闲时间，小于此速度均拉高至此
    // 增大此数值可以有效减小震动！受限于定时器，必须小于65535！
    TIM_TYPE _maxAttachTime = MAXATTACHTIME;
    // 当前速度
    int32_t _speed = 0;
    // 加速度
    uint32_t _accelSpeed = ACC;
    // 当前位置
    int32_t _currentPos = 0;
    // 目标位置
    int32_t _targetPos = 0;
    TIM_TYPE _stamp = 0;
    uint8_t _entry = 0;
    TIM_TYPE _delayTime = 0;
    // 正在产生波形
    bool _isWaving = false;
    void _runson()
    {
    }

    // 可变参数模板函数
    template <typename First, typename... Rest>
    void _runson(First &first, Rest &...rest)
    {
        first.Run();
        _runson(rest...); // 递归调用
    }

    static bool _runall()
    {
        return true; // 当没有更多 steppers 时返回 true
    }

    // 可变参数模板函数
    template <typename First, typename... Rest>
    static bool _runall(First &first, Rest &...rest)
    {
        bool ans = first.Run();
        return _runall(rest...) && ans; // 递归调用并与后续结果求与
    }

public:
    // 方向GPIO,方向PIN，脉冲GPIO，脉冲PIN
    Stepper(GPIO_TypeDef *dirPort, uint16_t dirPin, GPIO_TypeDef *pulPort, uint16_t pulPin);
    void SetTarget(int32_t _targetPos);
    void SetSpeed(TIM_TYPE hightime, TIM_TYPE lowtime, TIM_TYPE maxattachtime);
    void SetOrigin();
    bool Run();
    void SetAcc(uint32_t Acc);
    void SetMaxSpeed(uint32_t maxspeed);
    int32_t GetPos();
    int32_t GetSpeed();
    void SetReverse(bool value = true);
    // 配置最大速度高电平时间，配置最大速度低电平时间，最小速度低电平时间
    void SetAcc(const uint32_t &acc);
    void SetAccWay(uint8_t way);
    template <typename... Steppers>
    static void RunAll(Steppers &...steppers)
    {
        while (true)
        {
            bool ans = _runall(steppers...);
            if (ans)
                break;
        }
    }

    template <typename T, typename... Args>
    void RunUntil(T &&fun, Args &&...args)
    {
        auto boundFunc = [&]()
        {
            return std::forward<T>(fun)(std::forward<Args>(args)...);
        };
        while (!boundFunc())
        {
            this->Run();
        }
    }
};
#endif

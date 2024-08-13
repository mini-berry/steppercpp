#ifndef STEPPER_H
#define STEPPER_H
#include "main.h"
#include "tim.h"
// us定时器选择
#define US_TIM htim7
// us定时器的类型配置，不配置延时必出问题！
#define TIM_TYPE uint16_t

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
// 方向GPIO,方向PIN，脉冲GPIO，脉冲PIN
class Stepper
{
private:
    int32_t calStopLength(int32_t speed);
    void slowdown();
    void fastup();
    void spin(const int32_t &speed);

    // 正反转设置
    GPIO_PinState _forward  = GPIO_PIN_RESET;
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
    uint32_t _maxspeed = 500;
    // 高电平时间(us)
    TIM_TYPE _highTime = 500;
    // 最大速度的空闲时间
    TIM_TYPE _highSpeedAttachTime = 500;
    // 最小速度的空闲时间，小于此速度均拉高至此
    // 增大此数值可以有效减小震动！受限于定时器，必须小于65535！
    TIM_TYPE _maxAttachTime = 45000;
    // 当前速度
    int32_t _speed = 0;
    // 加速度
    uint32_t _accelSpeed = 4;
    // 当前位置
    int32_t _currentPos = 0;
    // 目标位置
    int32_t _targetPos  = 0;
    TIM_TYPE _stamp     = 0;
    uint8_t _entry      = 0;
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
    Stepper(GPIO_TypeDef *dirPort, uint16_t dirPin, GPIO_TypeDef *pulPort, uint16_t pulPin);
    void SetTarget(int32_t _targetPos);
    void SetOrigin();
    bool Run();
    void SetAcc(uint32_t Acc);
    void SetMaxSpeed(uint32_t maxspeed);
    int32_t GetPos();
    int32_t GetSpeed();
    void SetReverse(bool value = true);
    // 配置最大速度高电平时间，配置最大速度低电平时间，最小速度低电平时间
    void configure(const TIM_TYPE &hightime = 500, const TIM_TYPE &highSpeedAttachTime = 500, const TIM_TYPE &maxAttachTime = 45000);

    template <typename... Steppers>
    void RunAllUntil(Steppers &...steppers)
    {
        while (!this->Run()) {
            _runson(steppers...); // 依次调用 Run
        }
    }

    template <typename... Steppers>
    static void RunAll(Steppers &...steppers)
    {
        while (true) {
            bool ans = _runall(steppers...);
            if (ans) break;
        }
    }
};
#endif

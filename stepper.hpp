#ifndef STEPPER_H
#define STEPPER_H

// �ṩ���ּ��ٷ�ʽ
// 0��ÿ������ı��ٶȣ������ٶȼ���ʱ��
// 1��ÿ������ı�ʱ�䣬�̶�����һ��ʱ��
#define ACCFORM 0
#define MAXSPEED 500
#define HIGHTIME 250
#define HIGHATTACHTIME 250
#define MAXATTACHTIME 20000
#define ACC 1

// us��ʱ��ѡ��
#define US_TIM htim7
// us��ʱ�����������ã���������ʱ�س����⣡
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
    // ����ת����
    GPIO_PinState _forward = GPIO_PIN_RESET;
    GPIO_PinState _backward = GPIO_PIN_SET;
    // ����GPIO GPIO_TypeDef *
    GPIO_TypeDef *_dirPort;
    // ����Pin uint16_t
    uint16_t _dirPin;
    // ����GPIO GPIO_TypeDef *
    GPIO_TypeDef *_pulPort;
    // ����Pin uint16_t
    uint16_t _pulPin;
    // ����ٶ�
    int32_t _maxspeed = MAXSPEED;
    // �ߵ�ƽʱ��(us)
    TIM_TYPE _highTime = HIGHTIME;
    // ����ٶȵĿ���ʱ��
    TIM_TYPE _highSpeedAttachTime = HIGHATTACHTIME;
    // ��С�ٶȵĿ���ʱ�䣬С�ڴ��ٶȾ���������
    // �������ֵ������Ч��С�𶯣������ڶ�ʱ��������С��65535��
    TIM_TYPE _maxAttachTime = MAXATTACHTIME;
    // ��ǰ�ٶ�
    int32_t _speed = 0;
    // ���ٶ�
    uint32_t _accelSpeed = ACC;
    // ��ǰλ��
    int32_t _currentPos = 0;
    // Ŀ��λ��
    int32_t _targetPos = 0;
    TIM_TYPE _stamp = 0;
    uint8_t _entry = 0;
    TIM_TYPE _delayTime = 0;
    // ���ڲ�������
    bool _isWaving = false;
    void _runson()
    {
    }

    // �ɱ����ģ�庯��
    template <typename First, typename... Rest>
    void _runson(First &first, Rest &...rest)
    {
        first.Run();
        _runson(rest...); // �ݹ����
    }

    static bool _runall()
    {
        return true; // ��û�и��� steppers ʱ���� true
    }

    // �ɱ����ģ�庯��
    template <typename First, typename... Rest>
    static bool _runall(First &first, Rest &...rest)
    {
        bool ans = first.Run();
        return _runall(rest...) && ans; // �ݹ���ò�������������
    }

public:
    // ����GPIO,����PIN������GPIO������PIN
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
    // ��������ٶȸߵ�ƽʱ�䣬��������ٶȵ͵�ƽʱ�䣬��С�ٶȵ͵�ƽʱ��
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

#ifndef STEPPER_H
#define STEPPER_H
#include "main.h"
#include "tim.h"
// us��ʱ��ѡ��
#define US_TIM htim7
// us��ʱ�����������ã���������ʱ�س����⣡
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
// ����GPIO,����PIN������GPIO������PIN
class Stepper
{
private:
    int32_t calStopLength(int32_t speed);
    void slowdown();
    void fastup();
    void spin(const int32_t &speed);

    // ����ת����
    GPIO_PinState _forward  = GPIO_PIN_RESET;
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
    uint32_t _maxspeed = 500;
    // �ߵ�ƽʱ��(us)
    TIM_TYPE _highTime = 500;
    // ����ٶȵĿ���ʱ��
    TIM_TYPE _highSpeedAttachTime = 500;
    // ��С�ٶȵĿ���ʱ�䣬С�ڴ��ٶȾ���������
    // �������ֵ������Ч��С�𶯣������ڶ�ʱ��������С��65535��
    TIM_TYPE _maxAttachTime = 45000;
    // ��ǰ�ٶ�
    int32_t _speed = 0;
    // ���ٶ�
    uint32_t _accelSpeed = 4;
    // ��ǰλ��
    int32_t _currentPos = 0;
    // Ŀ��λ��
    int32_t _targetPos  = 0;
    TIM_TYPE _stamp     = 0;
    uint8_t _entry      = 0;
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
    Stepper(GPIO_TypeDef *dirPort, uint16_t dirPin, GPIO_TypeDef *pulPort, uint16_t pulPin);
    void SetTarget(int32_t _targetPos);
    void SetOrigin();
    bool Run();
    void SetAcc(uint32_t Acc);
    void SetMaxSpeed(uint32_t maxspeed);
    int32_t GetPos();
    int32_t GetSpeed();
    void SetReverse(bool value = true);
    // ��������ٶȸߵ�ƽʱ�䣬��������ٶȵ͵�ƽʱ�䣬��С�ٶȵ͵�ƽʱ��
    void configure(const TIM_TYPE &hightime = 500, const TIM_TYPE &highSpeedAttachTime = 500, const TIM_TYPE &maxAttachTime = 45000);

    template <typename... Steppers>
    void RunAllUntil(Steppers &...steppers)
    {
        while (!this->Run()) {
            _runson(steppers...); // ���ε��� Run
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

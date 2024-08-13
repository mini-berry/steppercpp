#include "stepper.h"
#include "debug.h"

// #define DEBUG
void usInit()
{
    HAL_TIM_Base_Start(&US_TIM);
}
inline void delayUs(TIM_TYPE us)
{
    TIM_TYPE now = GetUsTick();
    while ((TIM_TYPE)(GetUsTick() - now) < us);
}
inline TIM_TYPE GetUsTick()
{
    return __HAL_TIM_GetCounter(&US_TIM);
}
Stepper::Stepper(GPIO_TypeDef *dirPort, uint16_t dirPin, GPIO_TypeDef *pulPort, uint16_t pulPin)
{
    _dirPort = dirPort;
    _dirPin  = dirPin;
    _pulPort = pulPort;
    _pulPin  = pulPin;
}
void Stepper::SetTarget(int32_t targetPos)
{
    _targetPos = targetPos;
}
void Stepper::SetOrigin()
{
    _currentPos = 0;
}
void Stepper::SetAcc(uint32_t Acc)
{
    _accelSpeed = Acc;
}
void Stepper::slowdown()
{
    if (_speed > 0) {
        _speed -= _accelSpeed;
        if (_speed < 0) {
            _speed = 0;
        }
    } else {
        _speed += _accelSpeed;
        if (_speed > 0) {
            _speed = 0;
        }
    }
}
void Stepper::fastup()
{
    if (_speed > 0) {
        _speed += _accelSpeed;
        if (_speed > _maxspeed) {
            _speed = _maxspeed;
        }
    } else if (_speed < 0) {
        _speed -= _accelSpeed;
        if (_speed < -_maxspeed) {
            _speed = -_maxspeed;
        }
    } else {
        if (_targetPos > _currentPos)
            _speed += _accelSpeed;
        else
            _speed -= _accelSpeed;
    }
}
void Stepper::SetMaxSpeed(uint32_t maxspeed)
{
    _maxspeed = maxspeed;
}
int32_t Stepper::calStopLength(int32_t speed)
{
    speed = abs(speed);
    return speed / _accelSpeed + ((speed % _accelSpeed > 0) ? 0 : -1);
}
bool Stepper::Run()
{
    if (!_isWaving) {

        if (_speed == 0 && _targetPos == _currentPos) {

            return true;
        }
        // 同向
        // 可能需要减速
        else if (_speed >= 0 && _targetPos > _currentPos) {
            if ((calStopLength(_speed) >= _targetPos - _currentPos)) {

                slowdown();
            } else if (calStopLength(_speed + _accelSpeed) <= _targetPos - _currentPos) {

                fastup();
            }
        }
        // 同向
        // 可能需要减速
        else if (_speed <= 0 && _targetPos < _currentPos) {
            if ((calStopLength(_speed) >= _currentPos - _targetPos)) {

                slowdown();
            } else if (calStopLength(_speed - _accelSpeed) <= _currentPos - _targetPos) {

                fastup();
            }

        }
        // 需要减速转向
        else if (_speed < 0 && _targetPos >= _currentPos) {
            slowdown();
        }
        // 需要减速转向
        else if (_speed > 0 && _targetPos <= _currentPos) {
            slowdown();
        }

        _isWaving = true;
    }

    spin(_speed);
    return false;
}

void Stepper::spin(const int32_t &speed)
{
    if (speed == 0) {
        _isWaving = false;
        return;
    } else if (speed > 0) {

        TIM_TYPE t = GetUsTick();

        switch (_entry) {
            case 0:
                _stamp = t;
                _entry = 1;
                // 设置方向
                HAL_GPIO_WritePin(_dirPort, _dirPin, _forward);
                // 设置脉冲
                HAL_GPIO_WritePin(_pulPort, _pulPin, GPIO_PIN_SET);
                return;
            case 1:
                if ((TIM_TYPE)(t - _stamp) > _highTime) {
                    _stamp = t;
                    _entry = 2;

                    HAL_GPIO_WritePin(_pulPort, _pulPin, GPIO_PIN_RESET);
                    _delayTime = (_highSpeedAttachTime + _highTime) * _maxspeed / (speed)-_highTime;
                    _delayTime = (_delayTime > _maxAttachTime) ? _maxAttachTime : _delayTime;
                }
                return;
            case 2:
                if ((TIM_TYPE)(t - _stamp) > _delayTime) {
                    _entry = 0;
                    _currentPos++;
                    _isWaving = false;
                }
                return;
        }
    } else {
        uint16_t t = GetUsTick();

        switch (_entry) {
            case 0:
                _stamp = t;
                _entry = 1;
                // 设置方向
                HAL_GPIO_WritePin(_dirPort, _dirPin, _backward);
                // 设置脉冲
                HAL_GPIO_WritePin(_pulPort, _pulPin, GPIO_PIN_SET);
                return;
            case 1:
                if ((TIM_TYPE)(t - _stamp) > _highTime) {
                    _stamp = t;
                    _entry = 2;
                    HAL_GPIO_WritePin(_pulPort, _pulPin, GPIO_PIN_RESET);
                    _delayTime = (_highSpeedAttachTime + _highTime) * _maxspeed / (-speed) - _highTime;
                    _delayTime = (_delayTime > _maxAttachTime) ? _maxAttachTime : _delayTime;
                }
                return;
            case 2:
                if ((TIM_TYPE)(t - _stamp) > _delayTime) {
                    _entry = 0;
                    _currentPos--;
                    _isWaving = false;
                }
                return;
        }
    }
}
int32_t Stepper::GetPos()
{
    return _currentPos;
}
int32_t Stepper::GetSpeed()
{
    return _speed;
}
void Stepper::SetReverse(bool value)
{
    if (value) {
        _forward  = GPIO_PIN_SET;
        _backward = GPIO_PIN_RESET;
    } else {
        _forward  = GPIO_PIN_RESET;
        _backward = GPIO_PIN_SET;
    }
}
void Stepper::configure(const TIM_TYPE &hightime, const TIM_TYPE &highSpeedAttachTime, const TIM_TYPE &maxAttachTime)
{
    _highSpeedAttachTime = highSpeedAttachTime;
    _highTime            = hightime;
    _maxAttachTime       = maxAttachTime;
}

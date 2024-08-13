# steppercpp
A cpp library for stepper.

---

# Stepper Motor Control Library

## Overview

This library provides a comprehensive solution for controlling stepper motors using STM32 microcontrollers. The `Stepper` class offers functionalities to manage motor acceleration, deceleration, speed control, and precise positioning. The library is designed to be flexible, allowing for control of multiple stepper motors simultaneously.

## Features

- **Precise Step Control:** Control stepper motor movement with fine granularity using GPIO pins.
- **Acceleration and Deceleration:** Smooth acceleration and deceleration with customizable parameters.
- **Direction Control:** Easily set and reverse motor direction.
- **Microsecond Delay:** Accurate timing functions using hardware timers for precise motor control.
- **Multi-Motor Synchronization:** Manage multiple motors simultaneously using template functions.

## Getting Started

### Prerequisites

- STM32 microcontroller (e.g., STM32F4 series)
- STM32 HAL Library
- TIM peripherals configured for microsecond timing
- A development environment (e.g., STM32CubeIDE)

### Installation

1. Clone or download this repository to your local development environment.
2. Include the `stepper.h` and `stepper.cpp` files in your STM32 project.
3. Make sure to configure your hardware timers and GPIO pins correctly according to your microcontroller's setup.

### Configuration

#### Timer Configuration

The library requires a hardware timer to generate microsecond delays. Ensure that your timer is initialized correctly in your `main.c` or equivalent file:

```cpp
void usInit()
{
    HAL_TIM_Base_Start(&US_TIM);
}
```

#### GPIO Configuration

You must configure the GPIO pins that will be used to control the stepper motor's direction and pulse signals. The pins should be defined when creating a `Stepper` object.

### Usage

#### Creating a Stepper Object

To control a stepper motor, create a `Stepper` object with the appropriate GPIO ports and pins:

```cpp
#include "stepper.h"

// Initialize the stepper motor
Stepper stepper(GPIOA, GPIO_PIN_0, GPIOA, GPIO_PIN_1);
```

#### Setting Target Position and Running the Motor

Set the target position and run the motor:

```cpp
// Set the target position
stepper.SetTarget(1000);

// Run the motor to the target position
while (!stepper.Run()) {
    // Do something else if needed
}
```

#### Configuring Motor Parameters

You can customize the motor's speed, acceleration, and pulse timing:

```cpp
stepper.SetMaxSpeed(500);
stepper.SetAcc(10);
stepper.configure(500, 500, 45000);
```

#### Reversing Motor Direction

To reverse the motor's direction:

```cpp
stepper.SetReverse(true); // Reverse direction
```

### Example

Here is a simple example to control a single stepper motor:

```cpp
#include "main.h"
#include "stepper.h"

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_TIM7_Init();  // Assuming TIM7 is used for microsecond timing

    usInit();

    // Initialize the stepper motor
    Stepper stepper(GPIOA, GPIO_PIN_0, GPIOA, GPIO_PIN_1);

    // Set motor parameters
    stepper.SetMaxSpeed(1000);
    stepper.SetAcc(20);

    // Set the target position
    stepper.SetTarget(2000);

    // Run the motor to the target position
    while (!stepper.Run()) {
        // Other code can run here
    }

    while (1) {
    }
}
```
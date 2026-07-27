# STM32 RC Car with Joystick and Sensors

## 소개

이 프로젝트는 STM32F411RE Nucleo 보드를 기반으로 한 RC 자동차 제어 시스템입니다.
PWM 제어와 PID 기법을 활용해 모터 속도를 안정적으로 제어하고, 조이스틱 입력과 센서 데이터를 연동하여 차량을 제어할 수 있도록 구성했습니다.

## 주요 기능

- PID 기반 모터 속도 제어
  - PWM Duty Cycle을 제어 변수로 사용하여 목표 RPM에 가깝게 따라가도록 구현했습니다.
- 조이스틱 기반 원격 제어
  - Bluetooth UART 통신을 통해 조이스틱 입력값을 수신하고, 차량의 조향 및 속도 명령으로 변환합니다.
- IMU 센서 연동
  - ADXL345를 이용해 가속도/자세 정보를 수집합니다.
- 초음파 거리 측정
  - HC-SR04 센서를 사용해 장애물 거리를 측정합니다.
- 조도 센서 및 LED 제어
  - 조도 변화에 따라 LED 밝기를 조절하는 기능을 포함합니다.
- 실시간 상태 표시
  - TFT LCD를 통해 제어 상태와 센서 정보를 표시합니다.

## 사용 하드웨어

- MCU: STM32F411RE (Nucleo-64)
- 모터 제어: PWM 기반 DC 모터 제어
- 통신: UART, I2C, SPI
- 센서:
  - ADXL345 (IMU)
  - HC-SR04 (초음파 거리 센서)
  - 조도 센서
- 디스플레이: 2.2-inch QVGA TFT LCD

## 개발 환경

- 언어: C
- 빌드 도구: CMake
- IDE: STM32CubeIDE / STM32CubeMX
- 툴체인: ARM GCC

## 프로젝트 구성

- Core/Inc: 헤더 파일
- Core/Src: 메인 제어 로직 및 센서/모터 제어 구현
- Drivers: STM32 HAL 및 CMSIS 드라이버

## 실행 방법

### 1. 저장소 클론

```bash
git clone https://github.com/heeyeon5877-tech/Lcd_RC_Car_With_STM32.git
cd MiniP
```

### 2. 빌드

STM32CubeIDE 또는 CMake 기반 환경에서 프로젝트를 빌드할 수 있습니다.

### 3. 보드 업로드

ST-Link를 이용해 STM32F411RE 보드에 펌웨어를 업로드합니다.

### 4. 동작 확인

- 조이스틱 입력에 따라 차량의 방향과 속도가 변하는지 확인합니다.
- LCD 화면에서 센서 값과 제어 상태를 확인합니다.

## 팀원

| 이름 | 역할 |
|------|------|
| 변정제 | Bluetooth 조종기 및 UART 통신 |
| 김소정 | TFT LCD 디스플레이 제어 |
| 오은지 | PID 제어 기반 모터 제어 |
| 박희연 | IMU, 초음파 센서, 조도 센서 및 LED 제어 |

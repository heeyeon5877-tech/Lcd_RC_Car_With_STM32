# STM32-Joystick-RC-Car

## 소개

STM32F411RE(Nucleo) 보드를 기반으로 제작한 RC카 프로젝트입니다.
단순 PWM 출력을 통한 모터 제어가 아니라, **PWM Duty를 제어 변수로 하는 PID 제어**를 적용하여
모터의 정밀한 RPM 제어를 구현했습니다. 조이스틱 기반 무선 조종기(BT)로 차량을 제어하며,
IMU 센서를 통한 자세 데이터 수집과 TFT LCD를 통한 실시간 상태 표시 기능을 포함합니다.

## 주요 기능

- **PID 기반 모터 속도 제어**
  PWM Duty Cycle을 PID 제어 변수로 사용하여 목표 RPM을 정밀하게 추종합니다. 단순 Open-loop PWM 제어 대비 부하 변화에도 안정적인 속도를 유지합니다.

- **조이스틱 기반 무선 조종**
  별도 제작한 BT(Bluetooth) 조종기에서 조이스틱 입력값을 UART로 송신하고, 수신부(RC카)에서 조향/속도 명령으로 변환하여 모터 제어에 반영합니다.

- **IMU 센서 연동 (MPU-6050)**
  I2C 통신으로 자세/가속도 데이터를 수집하여 차량 상태 모니터링 및 제어 보정에 활용합니다.

- **TFT LCD 상태 디스플레이**
  2.2인치 QVGA(240x320) SPI TFT LCD를 통해 실시간 속도, 제어 상태 등을 시각적으로 출력합니다.

## 기술 스택

- **MCU**: STM32F411RE (Nucleo-64)
- **언어**: C
- **통신 프로토콜**
  - UART — BT 모듈을 이용한 조이스틱 조종기 데이터 통신
  - I2C — IMU 센서(MPU-6050) 데이터 수집
  - SPI — TFT LCD 디스플레이 출력
- **제어**: PID 제어 기반 PWM Duty 제어 (모터 RPM 정밀 제어)
- **디스플레이**: 2.2" QVGA TFT LCD (240x320, SPI)
- **개발 환경**: STM32CubeIDE, STM32CubeMX

## 시스템 구성도

```
[조종기]                          [RC카]
조이스틱 입력                     STM32F411RE
   │                                 │
   ▼                                 │
BT 모듈 ──── UART ──────────────►  BT 모듈
                                      │
                          ┌───────────┼───────────┐
                          ▼           ▼            ▼
                    PID 제어      MPU-6050      TFT LCD
                    (PWM Duty)    (I2C, IMU)    (SPI)
                          │
                          ▼
                       DC 모터
```

## 팀원

| 이름 | 역할 |
|------|------|
| 변정제 |  |
| 김소정 | |
| 오은지 | |
| 박희연 |

## 실행 방법

### 1. 개발 환경 설정

1. [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html) 설치
2. 본 저장소 클론
   ```bash
   git clone https://github.com/사용자명/저장소명.git
   ```
3. STM32CubeIDE에서 프로젝트 Import

### 2. 하드웨어 연결

| 모듈 | 통신 방식 | 비고 |
|------|-----------|------|
| BT 모듈 | UART | 조이스틱 조종기 데이터 수신 |
| MPU-6050 | I2C | IMU 자세/가속도 센서 |
| TFT LCD (2.2" QVGA 240x320) | SPI | 상태 디스플레이 |
| DC 모터 | PWM | PID 제어 기반 속도 제어 |

### 3. 빌드 및 업로드

1. STM32CubeIDE에서 프로젝트 Build
2. ST-Link를 통해 STM32F411RE 보드에 Flash
3. 조종기 BT 모듈과 페어링 후 조이스틱으로 제어

### 4. 동작 확인

- LCD 화면에서 실시간 속도 및 제어 상태 확인
- 조이스틱 입력에 따른 조향/속도 반응 확인

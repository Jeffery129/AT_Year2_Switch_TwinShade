#pragma once


#include <nn/hid/hid_NpadJoy.h>
#include <nn/hid/hid_Vibration.h>
#include <nn/hid/hid_NpadSixAxisSensor.h>
#include <nn/hid/hid_TouchScreen.h>

using namespace nn::hid;


//システム系関数
void InitController();
void UninitController();
void UpdateController();

//入力取得系関数

//デジタル入力取得
bool GetControllerPress(int button);//ボタンの状態そのまま（今押してるか押してないか）
bool GetControllerTrigger(int button);//押しっぱなし禁止

//3Dスティック　{ X = -1.0f~1.0f, Y = -1.0f~1.0f }
Float2 GetControllerLeftStick();//左スティック
Float2 GetControllerRightStick();//右スティック

//振動 frame = 振動時間（frame数）
void SetControllerLeftVibration(int frame);//左振動
void SetControllerRightVibration(int frame);//右振動

//センサー関連

//加速度センサー　XYZ方向のどちらに力がかかっているか？
//X 左右方向の力, Y 上下方向の力, Z 前後方向の力
Float3 GetControllerLeftAcceleration();
Float3 GetControllerRightAcceleration();

//ジャイロセンサー
//X X軸回転, Y Y軸回転, Z Z軸回転
Float3 GetControllerLeftAngle();
Float3 GetControllerRightAngle();

//タッチセンサー
bool GetControllerTouchScreen();//触ってる？触ってない？
Float2 GetControllerTouchScreenPosition();//触った場所の座標返す

// Right stick sensitivity
void SetRightStickSensitivity(float sensitivity);
float GetRightStickSensitivity();
void ResetRightStickInput();
#include <Nefry.h>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// UUIDは以下で自分のものを必ず生成して利用のこと
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "2fb65514-1a38-4597-bf63-590e175a262f"
#define CHARACTERISTIC_UUID "1b68902e-da10-40d8-a58f-c68da82c3021"

#define pin1A D0
#define pin1B D1
#define pin2A D2
#define pin2B D3
#define pin3A D4
#define pin3B D5

// LED制御
void led(char id, bool onoff) {
  // GPIO間で電流の方向を切り替えます
  switch(id) {
    case 'A':
      digitalWrite(pin1A, onoff);
      digitalWrite(pin1B, !onoff);
      Nefry.setLed(100, 100, 0);
      break;
    case 'B':
      digitalWrite(pin2A, onoff);
      digitalWrite(pin2B, !onoff);
      Nefry.setLed(0, 100, 100);
      break;
    case 'C':
      digitalWrite(pin3A, onoff);
      digitalWrite(pin3B, !onoff);
      Nefry.setLed(100, 0, 100);
      break;
    default:
      ledOff();
      break;
  }
}

// ツリーのLEDをぜんぶけす
void ledOff() {
  digitalWrite(pin1A, LOW);
  digitalWrite(pin1B, LOW);
  digitalWrite(pin2A, LOW);
  digitalWrite(pin2B, LOW);
  digitalWrite(pin3A, LOW);
  digitalWrite(pin3B, LOW);
  Nefry.setLed(50, 0, 0);
}

// コールバック
// ブラウザ側からwriteValue()されたら実行
class MyCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    
    // シリアルにデバッグ出力(受信した文字列)
    if (value.length() > 0) {
      Nefry.print("Received: ");
      // 1文字ずつ取得
      for (int i = 0; i < value.length(); i++) Nefry.print(value[i]);
    }
    Nefry.println("");
    
    // 受信した文字列が2文字のときのみコマンドとして実行
    if (value.length() == 2) {
      // i=0 : LEDチェーンのid番号
      // i=1 : LED状態(0/1)
      if (value[1] == '0') {
        led(value[0], false);
      } else {
        led(value[0], true);
      }
    }
  }
};

void setup() {
  Nefry.setLed(0, 0, 100); // 起動直後:本体LED青点灯
  Nefry.disableWifi();
  pinMode(pin1A, OUTPUT);
  pinMode(pin1B, OUTPUT);
  pinMode(pin2A, OUTPUT);
  pinMode(pin2B, OUTPUT);
  pinMode(pin3A, OUTPUT);
  pinMode(pin3B, OUTPUT);

  // BLEデバイスに俺はなる
  BLEDevice::init("NefryBT(X'mas)");
  Nefry.println("NefryBT(X'mas) Started.");

  // サンプルスケッチ: ESP32 BLE Arduino -> BLE_write からの流用です。
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  pCharacteristic->setCallbacks(new MyCallbacks());
  pCharacteristic->setValue("Merry X'mas!");
  pService->start();
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();

  Nefry.setLed(0, 100, 0); // 初期化終了:本体LED緑点灯
}

void loop() {
  // 待ちループ
  Nefry.ndelay(500);
}

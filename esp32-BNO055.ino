#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <splash.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <math.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define BNO055_SAMPLERATE_DELAY_MS (100)
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x29, &Wire);

adafruit_bno055_offsets_t calibrationData;

/**************************************************************************/
/*
    Display sensor calibration status
    */
/**************************************************************************/
void displayCalStatus(void)
{
    /* Get the four calibration values (0..3) */
    /* Any sensor data reporting 0 should be ignored, */
    /* 3 means 'fully calibrated" */
    uint8_t system, gyro, accel, mag;
    system = gyro = accel = mag = 0;
    bno.getCalibration(&system, &gyro, &accel, &mag);

    /* The data should be ignored until the system calibration is > 0 */
    Serial.print("\t");
    if (!system)
    {
        Serial.print("! ");
    }

    /* Display the individual values */
    Serial.print("Sys:");
    Serial.print(system, DEC);
    Serial.print(" G:");
    Serial.print(gyro, DEC);
    Serial.print(" A:");
    Serial.print(accel, DEC);
    Serial.print(" M:");
    Serial.print(mag, DEC);
}

/**************************************************************************/
/*
    Display the raw calibration offset and radius data
    */
/**************************************************************************/
void displaySensorOffsets(const adafruit_bno055_offsets_t &calibData)
{
    Serial.print("Accelerometer: ");
    Serial.print(calibData.accel_offset_x); Serial.print(" ");
    Serial.print(calibData.accel_offset_y); Serial.print(" ");
    Serial.print(calibData.accel_offset_z); Serial.print(" ");

    Serial.print("\nGyro: ");
    Serial.print(calibData.gyro_offset_x); Serial.print(" ");
    Serial.print(calibData.gyro_offset_y); Serial.print(" ");
    Serial.print(calibData.gyro_offset_z); Serial.print(" ");

    Serial.print("\nMag: ");
    Serial.print(calibData.mag_offset_x); Serial.print(" ");
    Serial.print(calibData.mag_offset_y); Serial.print(" ");
    Serial.print(calibData.mag_offset_z); Serial.print(" ");

    Serial.print("\nAccel Radius: ");
    Serial.print(calibData.accel_radius);

    Serial.print("\nMag Radius: ");
    Serial.print(calibData.mag_radius);
}
// ====================================================================
// ====================================================================



void setup() {

  // Акселерометр
  calibrationData.accel_offset_x = -14;
  calibrationData.accel_offset_y = 24;
  calibrationData.accel_offset_z = -7;
  calibrationData.accel_radius = 1000;

  // Гироскоп
  calibrationData.gyro_offset_x = -4;
  calibrationData.gyro_offset_y = -3;
  calibrationData.gyro_offset_z = 0;

  // Магнитометр
  calibrationData.mag_offset_x = 0;
  calibrationData.mag_offset_y = 0;
  calibrationData.mag_offset_z = 0;
  calibrationData.mag_radius = 0;


  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(1000);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED not found!");
    while(1);
  }

  display.setRotation(2); // 0-нормально, 1-90°, 2-180°, 3-270°

  display.display(); // Показать лого Adafruit
  delay(500);
  display.clearDisplay();
  
  display.setTextSize(1);               // Размер шрифта (1-8)
  display.setTextColor(SSD1306_WHITE);  // Цвет текста


  /* Initialise the sensor */
  bno.begin();
  delay(1000);


  bno.setMode(OPERATION_MODE_CONFIG);  
  delay(25); // Ждём завершения перехода
  bno.setMode(OPERATION_MODE_NDOF);  
  delay(100); // Даём время на инициализацию                     

  uint8_t sys, gyro, accel, mag;
  do {
    bno.getCalibration(&sys, &gyro, &accel, &mag);
    Serial.print("M:"); Serial.println(mag);

      // display.clearDisplay();
      display.setCursor(0, 0);              
      display.println("Rotate the sensor in a figure-eight motion for 15-20 seconds...");
      display.drawCircle(mag*32+16, 40, 10, 1 ); 
      display.display();

    delay(500);
  } while (mag < 3); // Ждём, пока магнитометр не откалибруется

  adafruit_bno055_offsets_t newOffsets;
  bno.getSensorOffsets(newOffsets); // Получаем обновлённые смещения

  
  // Обновляем ТОЛЬКО магнитометр (остальные оставляем старыми)
  calibrationData.mag_offset_x = newOffsets.mag_offset_x;
  calibrationData.mag_offset_y = newOffsets.mag_offset_y;
  calibrationData.mag_offset_z = newOffsets.mag_offset_z;
  calibrationData.mag_radius = newOffsets.mag_radius;

  displaySensorOffsets(calibrationData);
  bno.setSensorOffsets(calibrationData);
  bno.setExtCrystalUse(true);

 
 display.clearDisplay();
 display.display();  
 display.setTextSize(2); 
}

void loop() {

imu::Quaternion quat = bno.getQuat();
    float q0 = quat.w();  // Проверьте порядок! Возможно, quat.x() — это w.
    float q1 = quat.x();
    float q2 = quat.y();
    float q3 = quat.z();

    // Опционально: нормализация (если кватернион не нормализован)
    float norm = sqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
    q0 /= norm; q1 /= norm; q2 /= norm; q3 /= norm;

    // Правильные формулы для углов Эйлера
    float roll  = atan2(2*(q0*q1 + q2*q3), 1 - 2*(q1*q1 + q2*q2));
    float pitch = asin(2*(q0*q2 - q3*q1));
    float yaw   = atan2(2*(q0*q3 + q1*q2), 1 - 2*(q2*q2 + q3*q3));  // Без -M_PI/2!

    // Перевод в градусы (опционально)
    roll  *= 180.0 / M_PI;
    pitch *= 180.0 / M_PI;
    yaw   *= 180.0 / M_PI;

    float magnetic_declination = 7.5;  // Для Киева (уточните для вашего местоположения!)
    float true_yaw = yaw + magnetic_declination;  // Учитываем склонение

    // Приводим к диапазону 0°...360°
    if (true_yaw >= 360) true_yaw -= 360;
    if (true_yaw < 0) true_yaw += 360;

  

    // Вывод на дисплей
    display.clearDisplay();
    display.setCursor(0, 0);                
    display.println(true_yaw);
    display.display();

    delay(BNO055_SAMPLERATE_DELAY_MS);
}

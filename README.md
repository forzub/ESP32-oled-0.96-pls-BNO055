# ESP32-oled-0.96-pls-BNO055
ESP32-oled 0.96 pls BNO055

Microcontroller:32-bit Xtensa@ dual-core @240MHz

Operating Voltage:3.3V

Driver:CH340

Wi-Fi IEEE 802.11 b/g/n 2.4GHz,BLE 4.2 BR/EDR Flash:4MB,520 KB SRAM (16 KB for cache),448 KB ROM 34 GPIOs, 4x SPI, 3x UART, 2x 12C

2x 12S, RMT, LED PWM, 1 host SD/MMC/SDIO

1 slave SDIO/SPI, TWAI, 12-bit ADC, Ethernet USB-TTL based on CH340 is included onboard, Enabling Pn Play,
OLED-дисплей использует протокол связи I2C

![image](https://github.com/user-attachments/assets/945c455d-6a71-4794-b64a-de934d914d0d)

![image](https://github.com/user-attachments/assets/bbcdb57a-747a-4119-bd07-30d4185f80e3)

# Устанавливаем библиотеки для OLED:

esp8622 and esp32 oled driver for SSD1306 displays

u8g2 by oliver

Adafruit GFX libruary

Adafruit SSD1306

# Основные функции:

# Очистка дисплея 

display.clearDisplay(); // Очищает буфер (но не обновляет дисплей)

# Обновление дисплея 

display.display(); // Выводит буфер на экран

# Рисование пикселя 

display.drawPixel(x, y, color); // color: SSD1306_WHITE, SSD1306_BLACK

# Рисование линии 

display.drawLine(x0, y0, x1, y1, color);

# Рисование прямоугольника 

display.drawRect(x, y, width, height, color); // Контур

display.fillRect(x, y, width, height, color); // Залитый

# Рисование окружности

display.drawCircle(x, y, radius, color); // Контур

display.fillCircle(x, y, radius, color); // Залитая

# Рисование треугольника

display.drawTriangle(x0, y0, x1, y1, x2, y2, color); // Контур

display.fillTriangle(x0, y0, x1, y1, x2, y2, color); // Залитый

# Рисование прямоугольника со скругленными углами

display.drawRoundRect(x, y, width, height, radius, color); // Контур

display.fillRoundRect(x, y, width, height, radius, color); // Залитый

# Настройка шрифта и цвета
 
display.setTextSize(1);      // Размер текста (1-8)

display.setTextColor(SSD1306_WHITE); // Цвет (SSD1306_WHITE или SSD1306_BLACK)

display.setTextWrap(true);   // Перенос текста (true/false)

# Установка курсора (позиция текста)
 
display.setCursor(x, y); // (x, y) - начальная позиция текста

# Печать текста
 
display.print("Hello, World!"); // Вывод текста

display.println("New line");    // С новой строки

# Использование разных шрифтов (требует подключения GFX)
 
#include <Fonts/FreeSans9pt7b.h> // Подключение шрифта

display.setFont(&FreeSans9pt7b); // Установка шрифта

display.print("Custom Font");

display.setFont(NULL); // Вернуться к стандартному шрифту

#  Дополнительные функции
# Инверсия цветов
 
display.invertDisplay(true);  // Инвертировать цвета

display.invertDisplay(false); // Вернуть нормальные цвета

# Прокрутка (скроллинг)
 
display.startscrollright(start, stop);  // Прокрутка вправо

display.startscrollleft(start, stop);   // Прокрутка влево

display.startscrolldiagright(start, stop); // Диагональная прокрутка

display.startscrolldiagleft(start, stop);  // Диагональная прокрутка

display.stopscroll(); // Остановить прокрутку 

# Установка контраста
 
display.setContrast(contrast); // contrast: 0-255

# Пример:

 display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Инициализация + создание буфера
 
 display.clearDisplay(); // Буфер заполнен 0 (экран чёрный)
  
 // Рисуем пиксель в буфере (но на экране пока ничего нет!)
 
 display.drawPixel(10, 10, SSD1306_WHITE);
  
 // Теперь выводим буфер на экран
 
 display.display();






![image](https://github.com/user-attachments/assets/a6524f43-1c7a-46b6-b425-dd3ca4a4e8b8)

# BNO055

Адрес узкого BNO - 0x29, а не 0x28 как у его кадратного собрата. Поэтому изменяем его:

Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x29, &Wire);

Провода должны быть припаяны.

# Генерация данных калибровки

Для получения достоверных данных калибровки необходимо соблюдать следующие критерии:

Гироскоп : Устройство должно стоять неподвижно в любом положении.
Магнитометр : в прошлом требовались движения по «восьмерке» в трех измерениях, но в современных устройствах быстрая магнитная компенсация происходит при достаточном нормальном движении устройства.
Акселерометр : BNO055 необходимо разместить в 6 положениях стоя для +X, -X, +Y, -Y, +Z и -Z. Это самый обременительный датчик для калибровки, но лучшее решение для генерации данных калибровки — найти деревянный брусок или подобный предмет и разместить датчик на каждой из 6 «граней» бруска, что поможет сохранить выравнивание датчика во время процесса калибровки. Однако вы все равно сможете получить приемлемые качественные данные от BNO055, даже если акселерометр не полностью или идеально откалиброван.

![image](https://github.com/user-attachments/assets/a6e38fc4-505d-410e-9163-592a05b29e52) 

Пример калибровки с сохранением данных и восстановлением после перезапуска.

https://github.com/adafruit/Adafruit_BNO055/blob/master/examples/restore_offsets/restore_offsets.ino

Однако следует помнить, что датчик не обязательно «plug and play» с загрузкой данных калибровки, в частности, магнитометр необходимо перекалибровать, даже если загружены смещения. Калибровка магнитометра очень динамична, поэтому сохранение значений один раз может не помочь, когда они будут перезагружены, и ЭДС вокруг датчика изменится.

Шаг первый: восьмерка
Чтобы откалибровать магнитометр , медленно проведите датчиком по траектории «восьмерка».

Шаг второй: Шестишаговое вращение
Затем акселерометр калибруется путем удержания его на краю, обращенном к вам, в течение нескольких секунд, затем поворота по часовой стрелке на 90 градусов , ожидания и повторения в общей сложности 4 положений . Затем положите его лицевой стороной вверх на плоскую поверхность и удерживайте его там в течение нескольких секунд . Наконец , переверните его лицевой стороной вниз и удерживайте, чтобы завершить калибровку акселерометра .

Шаг третий: Взгляд вверх и ожидание
Последний шаг для гироскопа. Все, что нужно, это подержать его неподвижно в течение нескольких секунд в положении лицом вверх.





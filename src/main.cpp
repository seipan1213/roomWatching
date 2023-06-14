#include <M5Unified.h>
#include <M5_ENV.h>
#include <WiFiClientSecure.h>
#include <Ambient.h>

#if __has_include(<Config.h>)
#include <Config.hpp>
#endif

#define SLEEP_SEC(us) (((uint64_t)us) * 1000000L)

unsigned int ambient_channel_id = AMBIENT_CHNNEL_ID;
const char *ambient_write_key = AMBIENT_WRITE_KEY;

const char *ssid = SSID;
const char *password = PASS;

const char *line_host = LINE_HOST;
const char *line_key = LINE_KEY;

void Line_notify(String msg);

Ambient ambient;
SHT3X sht30;
QMP6988 qmp6988;
WiFiClientSecure line_client;
WiFiClient ambient_client;

float tmp = 0.0;
float hum = 0.0;
float pressure = 0.0;
double vbat = 0.0;
float battery = 0.0;

void setup()
{
  M5.begin();
  M5.Power.begin();
  ambient.begin(ambient_channel_id, ambient_write_key, &ambient_client);

  setCpuFrequencyMhz(80);
  M5.Lcd.setCursor(0, 0, 4);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    M5.Lcd.print('.');
    delay(500);
  }
  M5.Lcd.print("\r\nWiFi connected\r\nIP address: ");
  M5.Lcd.println(WiFi.localIP());

  Wire.begin(0, 26);
  qmp6988.init();

  pressure = qmp6988.calcPressure();
  if (sht30.get() == 0)
  {
    tmp = sht30.cTemp;
    hum = sht30.humidity;
  }
  else
  {
    tmp = 0, hum = 0;
  }
  battery = M5.Power.getBatteryLevel();
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0, 4);
  M5.Lcd.printf("Temp: %2.1f  \r\nHumi: %2.0f  \r\nPressure: %2.0fPa\r\nVolt: %.2fV  \r\nBattery: %3.0f%  \r\n", tmp, hum, pressure, vbat, battery);

  char msg[100];
  sprintf(msg, "Temp: %2.1f  \r\nHumi: %2.0f  \r\nPressure: %2.0fPa\r\nVolt: %.2fV  \r\nBattery: %3.0f%  \r\n", tmp, hum, pressure, vbat, battery);

  ambient.set(1, tmp);
  ambient.set(2, hum);
  ambient.set(3, pressure);
  ambient.set(4, vbat);
  ambient.set(5, battery);

  ambient.send();

  Line_notify(msg);
  delay(10000);
  M5.Power.deepSleep(SLEEP_SEC(3 * 60 * 60));
}

void loop()
{
}

void Line_notify(String msg)
{
  line_client.setInsecure();
  if (!line_client.connect(line_host, 443))
  {
    delay(2000);
    M5.Lcd.print("Not Connected");
    return;
  }
  String query = String("message=") + msg;
  String request = String("") +
                   "POST /api/notify HTTP/1.1\r\n" +
                   "Host: " + line_host + "\r\n" +
                   "Authorization: Bearer " + line_key + "\r\n" +
                   "Content-Length: " + String(query.length()) + "\r\n" +
                   "Content-Type: application/x-www-form-urlencoded\r\n\r\n" +
                   query + "\r\n";
  line_client.print(request);
  while (line_client.connected())
  {
    String line = line_client.readStringUntil('\n');
    if (line == "\r")
    {
      break;
    }
  }
}
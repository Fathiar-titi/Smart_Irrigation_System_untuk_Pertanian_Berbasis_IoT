#include <Wire.h>
#include <RTClib.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>

#define RELAY1 26

RTC_DS3231 rtc;
hd44780_I2Cexp lcd;

// jadwal relay
const int jamON = 7;
const int jamOFF = 19;

bool relayStatus = false;
unsigned long lastUpdate = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);

  pinMode(RELAY1, OUTPUT);
  digitalWrite(RELAY1, HIGH); // relay OFF

  // LCD init (auto detect)
  int status = lcd.begin(16, 2);
  if (status) {
    Serial.print("LCD error: ");
    Serial.println(status);
    while (1); // berhenti kalau LCD gagal
  }
  lcd.backlight();

  // RTC init
  if (!rtc.begin()) {
    Serial.println("RTC tidak ditemukan!");
    while (1);
  }

  // ⚠️ AKTIFKAN SEKALI SAJA JIKA RTC BELUM DISET
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  lcd.setCursor(0, 0);
  lcd.print("SYSTEM START");
  lcd.setCursor(0, 1);
  lcd.print("PLEASE WAIT...");
  delay(2000);
  lcd.clear();
}

void loop() {
  DateTime now = rtc.now();

  int jam = now.hour();

  // =========================
  // RELAY CONTROL (ANTI FLICKER)
  // =========================
  if (jam >= jamON && jam < jamOFF) {
    if (!relayStatus) {
      digitalWrite(RELAY1, LOW);
      relayStatus = true;
    }
  } else {
    if (relayStatus) {
      digitalWrite(RELAY1, HIGH);
      relayStatus = false;
    }
  }

  // =========================
  // LCD UPDATE (STABIL)
  // =========================
  if (millis() - lastUpdate >= 1000) {
    lastUpdate = millis();

    lcd.setCursor(0, 0);
    lcd.print("Tgl:");
    lcd.print(now.day());
    lcd.print("/");
    lcd.print(now.month());
    lcd.print("/");
    lcd.print(now.year());
    lcd.print("  ");

    lcd.setCursor(0, 1);
    lcd.print("Jam:");

    if (now.hour() < 10) lcd.print("0");
    lcd.print(now.hour());
    lcd.print(":");

    if (now.minute() < 10) lcd.print("0");
    lcd.print(now.minute());
    lcd.print(":");

    if (now.second() < 10) lcd.print("0");
    lcd.print(now.second());
    lcd.print("  ");
  }
}
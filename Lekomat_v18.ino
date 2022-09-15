#include "MenuData.h"
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <Rotary.h>
//#include <TimerOne.h>
#include <Servo.h>
#include "RTClib.h"
#include <EEPROM.h>

#define MyVersionNumber 18
#define MaxAlarmNumber 1
#define MaxFeederNumber 1

// Hardware configuration
// #define WiFiRXPin 2?
// #define WiFiTXPin 3?
#define Servo1Pin 4
#define Servo2Pin 5
#define BacklightPin 6
#define AckSensorPin 7
// #define LCDPins 8..12
// #define LED/BeeperPin 13
#define RotaryDtPin A0
#define RotaryClkPin A1
#define RotarySwPin A2
#define RTCBatSensorPin A3
// RTC: SDA->A4, SCL->A5
#define Feeder1SensorPin A6
#define Feeder2SensorPin A7

#define MinSensorVal 0
#define MaxSensorVal 1023

Adafruit_PCD8544 display = Adafruit_PCD8544(8, 9, 10, 12, 11);
RTC_DS1307 rtc;
Rotary rotary = Rotary(RotaryDtPin, RotaryClkPin);

struct LogEntry {
  char Symbol, Parameter;
  uint32_t UDT;
};

struct Log {
  LogEntry log1;
  int StartAddress;
  byte SizeRec, MaxRec, First, Last, Count, Pos;
};

struct MyLevel {
  int LMin, LMedium, LMax;
};

struct MyFeeder {
  byte ServoPin, SensorPin;
  Servo myservo;  // create servo objects to control a servo
  int posOpen, posClose, dir;    // servo position
  MyLevel Level[4]; // input levels (SensorPin)
  int Threshold[3]; // thresholds between 4 input levels
};

struct MyAlarm {
  byte Enable, Hour, Minute;
  DateTime DT;
};

struct MyConf {
  byte VersionNumber;
  byte BacklightValue, ContrastValue; // LCD
  byte RotaryDirection;
  MyAlarm Alarm[MaxAlarmNumber + 1];
  MyFeeder Feeder[2];
};

// Configuration
MyConf conf;

// Global variables
unsigned char RotaryResult;
boolean RotarySwPressed, RotaryUp, RotaryDown, RotaryAnything;
boolean RotarySwEnable = true;
DateTime now, NextAlarmTime, DT;
byte NextAlarmNumber;
boolean AnyValidAlarm = false;
LogEntry log1;
Log MyLog;

char nameBuf[15];
String valueS;
uint32_t StartTimeout;

// Menu & app mode
MenuManager Menu1(LekomatMenu_Root, menuCount(LekomatMenu_Root));
void refreshMenuDisplay (byte refreshMode);
byte getNavAction();
byte MaxMenuItemsDisplayed = 4;
byte StartMenuItemDisplay = 0;
byte appMode;

enum AppModeValues
{
  APP_NORMAL_MODE,
  APP_MENU_MODE,
  APP_PROCESS_MENU_CMD
};

void setup()
{
  Serial.begin(115200);

  conf.VersionNumber = EEPROM.read(0);
  if (conf.VersionNumber == MyVersionNumber)
  {
    EEPROM.get(0, conf);
    CmdInit();
  }
  else CmdFactoryReset();

  // Emergency reset
  RotaryTest();
  if (RotarySwPressed)
  {
    Beep();
    CmdFactoryReset();
  }

  // fall in to menu mode by default.
  appMode = APP_NORMAL_MODE;
  // appMode = APP_MENU_MODE;

  refreshMenuDisplay(REFRESH_DESCEND);
}

void loop()

{
  RotaryTest();

  switch (appMode)
  {
    case APP_NORMAL_MODE :
      {
        NextAlarmCalculate();

        display.setTextSize(1);
        display.clearDisplay();
        display.setTextColor(BLACK, WHITE);
        now = rtc.now();
        display.print("Godz. ");
        valueS = TimeToString (now.hour(), now.minute(), now.second());
        display.println(valueS);
        if (now.hour() > 23) Beep();
        //Serial.println(TimeS);
        display.drawFastHLine(0, 8, 83, BLACK);
        display.drawFastHLine(0, 36, 83, BLACK);

        display.setCursor(0, 11);

        if (AckPressed())
        {
          display.println("Poprz. zdarz.:");
          display.print(log1.Symbol);
          display.print(log1.Parameter);
          display.print(": ");
          DT = log1.UDT;
          if (DT.hour() < 10) display.print(" ");
          display.print(DT.hour(), DEC);
          display.print(":");
          if (DT.minute() < 10) display.print("0");
          display.print(DT.minute(), DEC);
          display.print(":");
          if (DT.second() < 10) display.print("0");
          display.println(DT.second(), DEC);
          display.print("  ");
          valueS = DateToString(DT.year(), DT.month(), DT.day());
          display.print(valueS);
        }
        else
        {
          display.println("Nastepne leki:");

          display.setTextSize(2);
          if (AnyValidAlarm)
          {
            if (NextAlarmNumber == 0) display.print("L"); else display.print("P");
            display.print(" ");
            if (conf.Alarm[NextAlarmNumber].Hour < 10) display.print(" ");
            display.print(conf.Alarm[NextAlarmNumber].Hour, DEC);
            display.print(":");
            if (conf.Alarm[NextAlarmNumber].Minute < 10) display.print("0");
            display.print(conf.Alarm[NextAlarmNumber].Minute, DEC);
          }
          else
            display.print("  ##:##");


        }
        display.setTextSize(1);

        display.setCursor(0, 38);
        display.print(StatusFeederS(0));
        display.print("  ");
        display.print(StatusFeederS(1));
        display.display();
      }
      if (RotaryAnything)
      {
        if (SimpleProtection(5))
        {
          WriteLog('B', '-');
          appMode = APP_MENU_MODE;
          refreshMenuDisplay(REFRESH_DESCEND);
        }
      }
      break;
    case APP_MENU_MODE :
      {
        if (RotaryAnything)
        {
          byte menuMode = Menu1.handleNavigation(getNavAction, refreshMenuDisplay);

          if (menuMode == MENU_EXIT) appMode = APP_NORMAL_MODE;
          else if (menuMode == MENU_INVOKE_ITEM)
          {
            appMode = APP_PROCESS_MENU_CMD;

            // Indicate selected item.
            if (Menu1.getCurrentItemCmdId())
            {

            }
          }
        }
        if (AckPressed())
        {
          do {} while (AckPressed());
          appMode = APP_NORMAL_MODE;
        }
        break;
      }
    case APP_PROCESS_MENU_CMD :
      {
        byte processingComplete = processMenuCommand(Menu1.getCurrentItemCmdId());

        if (processingComplete)
        {
          appMode = APP_MENU_MODE;
          refreshMenuDisplay(REFRESH_DESCEND);
        }
        break;
      }
  }
}

//----------------------------------------------------------------------
// Addition or removal of menu items in MenuData.h will require this method
// to be modified accordingly.
byte processMenuCommand(byte cmdId)
{
  byte complete = false;  // set to true when menu command processing complete.
  //Serial.print("processMenuCommand ");
  //Serial.println(cmdId);
  Menu1.getItemName(nameBuf, Menu1.getCurrentItemIndex());
  switch (cmdId)
  {
    case mnuCmdLid1OpenClose :
      CmdLidOpenClose(0);
      break;
    case mnuCmdLid1ForceOpen :
      CmdLidForceOpen(0);
      break;
    case mnuCmdLid1ForceClose :
      CmdLidForceClose(0);
      break;
    case mnuCmdLid2OpenClose :
      CmdLidOpenClose(1);
      break;
    case mnuCmdLid2ForceOpen :
      CmdLidForceOpen(1);
      break;
    case mnuCmdLid2ForceClose :
      CmdLidForceClose(1);
      break;
    case mnuCmdA1Enable :
      CmdAlarmEnable(0);
      break;
    case mnuCmdA1Hour :
      CmdAlarmHour(0);
      break;
    case mnuCmdA1Minute :
      CmdAlarmMinute(0);
      break;
    case mnuCmdA2Enable :
      CmdAlarmEnable(1);
      break;
    case mnuCmdA2Hour :
      CmdAlarmHour(1);
      break;
    case mnuCmdA2Minute :
      CmdAlarmMinute(1);
      break;
    case mnuCmdTimeSetHour :
      CmdTimeSetHour();
      break;
    case mnuCmdTimeSetMinute :
      CmdTimeSetMinute();
      break;
    case mnuCmdTimeSetYear :
      CmdTimeSetYear();
      break;
    case mnuCmdTimeSetMonth :
      CmdTimeSetMonth();
      break;
    case mnuCmdTimeSetDay :
      CmdTimeSetDay();
      break;
    case mnuCmdLCDSetBack :
      CmdLCDSetBack();
      break;
    case mnuCmdLCDSetContrast :
      CmdLCDSetContrast();
      break;
    case mnuCmdFeeder1SensorsCalibration :
      CmdFeederSensorsCalibration(0);
      break;
    case mnuCmdFeeder2SensorsCalibration :
      CmdFeederSensorsCalibration(1);
      break;
    case mnuCmdLoadConf :
      CmdLoadConf();
      break;
    case mnuCmdSaveConf :
      CmdSaveConf();
      break;
    case mnuCmdFactoryReset :
      CmdFactoryReset();
      break;
    case mnuCmdFeeder1ServoTest :
      CmdFeederServoTest(0);
      break;
    case mnuCmdFeeder1SensorsTest :
      CmdFeederSensorsTest(0);
      break;
    case mnuCmdFeeder2ServoTest :
      CmdFeederServoTest(1);
      break;
    case mnuCmdFeeder2SensorsTest :
      CmdFeederSensorsTest(1);
      break;
    case mnuCmdRTCTimeTest :
      CmdRTCTimeTest();
      break;
    case mnuCmdRTCBatLevel :
      CmdRTCBatLevel();
      break;
    case mnuCmdBuzzerLEDTest :
      CmdBuzzerLEDTest();
      break;
    case mnuCmdAckTest :
      CmdAckTest();
      break;
  }

  complete = true;
  return complete;
}


//----------------------------------------------------------------------
// Callback to convert button press to navigation action.
byte getNavAction()
{
  byte navAction = 0;
  byte currentItemHasChildren = Menu1.currentItemHasChildren();
  byte currentItemIndex = Menu1.getCurrentItemIndex();
  byte menuItemCount = Menu1.getMenuItemCount();

  if (RotaryUp) navAction = MENU_ITEM_PREV;
  else if (RotaryDown) navAction = MENU_ITEM_NEXT;
  else if (RotarySwPressed)
    if (currentItemIndex == (menuItemCount - 1)) navAction = MENU_BACK;
    else navAction = MENU_ITEM_SELECT;

  /*Serial.print("navAction: ");
    Serial.print(navAction);
    Serial.print(" menuItemCount: ");
    Serial.print(menuItemCount);
    Serial.print(" StartMenuItemDisplay: ");
    Serial.print(StartMenuItemDisplay);
    Serial.print(" currentItemIndex: ");
    Serial.print(currentItemIndex);
    Serial.print(" currentItemHasChildren: ");
    Serial.print(currentItemHasChildren);
    Serial.print(" RotarySwPressed: ");
    Serial.println(RotarySwPressed);
  */

  if (menuItemCount > MaxMenuItemsDisplayed)
  {
    if (RotaryUp && (currentItemIndex > 0) && (StartMenuItemDisplay > 0) && (StartMenuItemDisplay == currentItemIndex)) StartMenuItemDisplay--;
    else if (RotaryDown && (currentItemIndex < (menuItemCount - 1)) && ((currentItemIndex - StartMenuItemDisplay) > (MaxMenuItemsDisplayed - 2))) StartMenuItemDisplay++;
    //else StartMenuItemDisplay = (menuItemCount - MaxMenuItemsDisplayed);
  }
  if (RotarySwPressed)
    //if (navAction == MENU_BACK)
    StartMenuItemDisplay = 127;
  //else StartMenuItemDisplay = 0;

  return navAction;
}

// Callback to refresh display during menu navigation, using parameter of type enum DisplayRefreshMode.
void refreshMenuDisplay (byte refreshMode)
{
  byte menuItemCount = Menu1.getMenuItemCount();

  /*
    if (refreshMode == REFRESH_DESCEND || refreshMode == REFRESH_ASCEND)
    {

    // uncomment below code to output menus to serial monitor
    if (Menu1.currentMenuHasParent())
    {
      Serial.print("Parent menu: ");
      Serial.println(Menu1.getParentItemName(nameBuf));
    }
    else
    {
      Serial.println("Main menu:");
    }

    for (int i = 0; i < menuCount; i++)
    {
      Serial.print(Menu1.getItemName(nameBuf, i));

      if (Menu1.itemHasChildren(i))
      {
        Serial.println("->");
      }
      else
      {
        Serial.println();
      }
    }
    }
  */

  if (StartMenuItemDisplay == 127) // Return from submenu
    if ((Menu1.getCurrentItemIndex()) < MaxMenuItemsDisplayed) StartMenuItemDisplay = 0;
    else StartMenuItemDisplay = (menuItemCount - MaxMenuItemsDisplayed);
  display.setTextSize(1);
  display.clearDisplay();
  display.setTextColor(BLACK, WHITE);
  if (Menu1.currentMenuHasParent())
  {
    display.print(Menu1.getParentItemName(nameBuf));
  }
  else
  {
    display.print("Menu");
  }
  display.println(":");

  display.println();
  display.drawFastHLine(0, 10, 83, BLACK);

  for (int i = StartMenuItemDisplay; i < menuItemCount; i++)
  {
    if (Menu1.getCurrentItemIndex() == i)
    {
      display.setTextColor(WHITE, BLACK);
    }
    else
    {
      display.setTextColor(BLACK, WHITE);
    }
    if (Menu1.itemHasChildren(i))
    {
      display.print("+");
    }
    else
    {
      display.print(" ");
    }
    display.println(Menu1.getItemName(nameBuf, i));
  }
  display.display();
}

void displayIntPage(String item, String descr, int value)
{
  String valueS = FiveDigits(value);
  display.setTextSize(1);
  display.clearDisplay();
  display.setTextColor(BLACK, WHITE);
  display.setCursor(0, 0);
  display.print(item);
  display.drawFastHLine(0, 10, 83, BLACK);
  display.setCursor(5, 15);
  display.print(descr);
  display.setTextSize(2);
  display.setCursor(5, 25);
  display.print(valueS);
  display.display();
}

void displayStringPage(String item, String descr, String value)
{
  display.setTextSize(1);
  display.clearDisplay();
  display.setTextColor(BLACK, WHITE);
  display.println(item);
  display.println();
  display.drawFastHLine(0, 10, 83, BLACK);
  display.println(descr);
  display.print(value);
  display.display();
}

void MessageMenuString(String descr, String value)
{
  displayStringPage(nameBuf, descr, value);
  do
  {
    RotaryTest();
  }
  while (!(RotaryAnything));
}

void MessageMenuInt(String descr, int value)
{
  displayIntPage(nameBuf, descr, value);
  do
  {
    RotaryTest();
  }
  while (!(RotaryAnything));
}

void CmdFeederServoTest(byte valFeeder)
{
  String valueS;
  boolean Start = true;
  byte pos = 90;
  RotaryTest();
  while (!RotarySwPressed)
  {
    if (RotaryAnything || Start)
    {
      Start = false;
      if (AckPressed())
      {
        if (RotaryUp) pos += 10;
        if (RotaryDown) pos += -10;
      }
      else
      {
        if (RotaryUp) pos += 1;
        if (RotaryDown) pos += -1;
      }
      if (pos < 0) pos = 0;
      if (pos > 179) pos = 179;
      valueS = "Pokr.";
      if (LidOpened(valFeeder)) valueS += "OTW"; else valueS += "ZAM";
      valueS += " kat:";
      displayIntPage(nameBuf, valueS, pos);
      conf.Feeder[valFeeder].myservo.write(pos);
    }
    // delay(15);
    RotaryTest();
  }
}

void RotaryTest()
{
  // remember that the switch is active low
  RotarySwPressed = false;
  if (digitalRead(RotarySwPin) == LOW)
  {
    if (RotarySwEnable)
    {
      RotarySwEnable = false;
      RotarySwPressed = true;
      //delay(100);
    }
  }
  else
    RotarySwEnable = true;

  RotaryResult = rotary.process();

  RotaryUp = (RotaryResult == DIR_CW);
  RotaryDown = (RotaryResult == DIR_CCW);
  RotaryAnything = (RotaryUp | RotaryDown | RotarySwPressed);
  if (RotaryAnything)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(3);
    digitalWrite(LED_BUILTIN, LOW);
    delay(3);
  }
}

void Beep()
{
  digitalWrite(LED_BUILTIN, HIGH);
  analogWrite(BacklightPin, 255);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  analogWrite(BacklightPin, 0);
  delay(500);
  analogWrite(BacklightPin, conf.BacklightValue);
}

void CmdBuzzerLEDTest()
{
  displayStringPage(nameBuf, "Piszczenie/LED", "f=1 Hz");
  do
  {
    RotaryTest();
    Beep();
  }
  while (!(RotaryAnything));
}

int TimeSetCommon(int low, int high, int value)
{
  String lowS = String(low, DEC);
  String highS = String(high, DEC);
  String bounds = String(lowS + ".." + highS);
  displayIntPage(nameBuf, bounds, value);
  do
  {
    RotaryTest();
    if (RotaryAnything)
    {
      if (RotaryUp) value += 1;
      if (RotaryDown) value += -1;
      if (value < low && RotaryDown) value = high;
      if (value > high && RotaryUp) value = low;
      displayIntPage(nameBuf, bounds, value);
    }

  }
  while (!RotarySwPressed);
  now = rtc.now();
  return value;
}

void CmdTimeSetYear()
{
  now = rtc.now();
  rtc.adjust(DateTime(TimeSetCommon(2017, 2099, now.year()), now.month(), now.day(), now.hour(), now.minute(), now.second()));
}

void CmdTimeSetMonth()
{
  now = rtc.now();
  rtc.adjust(DateTime(now.year(), TimeSetCommon(1, 12, now.month()), now.day(), now.hour(), now.minute(), now.second()));
}

void CmdTimeSetDay()
{
  now = rtc.now();
  rtc.adjust(DateTime(now.year(), now.month(), TimeSetCommon(1, 31, now.day()), now.hour(), now.minute(), now.second()));
}

void CmdTimeSetHour()
{
  now = rtc.now();
  rtc.adjust(DateTime(now.year(), now.month(), now.day(), TimeSetCommon(0, 23, now.hour()), now.minute(), now.second()));
}

void CmdTimeSetMinute()
{
  now = rtc.now();
  rtc.adjust(DateTime(now.year(), now.month(), now.day(), now.hour(), TimeSetCommon(0, 59, now.minute()), 0));
}

void CmdLCDSetBack()
{
  displayIntPage(nameBuf, "0..255", conf.BacklightValue);
  do
  {
    RotaryTest();
    if (RotaryAnything)
    {
      if (RotaryUp) conf.BacklightValue += 1;
      if (RotaryDown) conf.BacklightValue += -1;
      displayIntPage(nameBuf, "0..255", conf.BacklightValue);
      turnBacklightOn();
    }
  }
  while (!RotarySwPressed);
}

void CmdLCDSetContrast()
{
  displayIntPage(nameBuf, "0..255", conf.ContrastValue);
  do
  {
    RotaryTest();
    if (RotaryAnything)
    {
      if (RotaryUp) conf.ContrastValue += 1;
      if (RotaryDown) conf.ContrastValue += -1;
      displayIntPage(nameBuf, "0..255", conf.ContrastValue);
      turnBacklightOn();
    }
  }
  while (!RotarySwPressed);
}

void CmdAlarmEnable(byte valAlarm)
{
  displayIntPage(nameBuf, "0=Nie, 1=Tak", conf.Alarm[valAlarm].Enable);
  do
  {
    RotaryTest();
    if (RotaryAnything)
    {
      if (RotaryUp) conf.Alarm[valAlarm].Enable += 1;
      if (RotaryDown) conf.Alarm[valAlarm].Enable += -1;
      displayIntPage(nameBuf, "0=Nie, 1=Tak", conf.Alarm[valAlarm].Enable);
    }
  }
  while (!RotarySwPressed);
}

void CmdAlarmHour(byte valAlarm)
{
  displayIntPage(nameBuf, "0..23", conf.Alarm[valAlarm].Hour);
  do
  {
    RotaryTest();
    if (RotaryAnything)
    {
      if (RotaryUp) conf.Alarm[valAlarm].Hour += 1;
      if (RotaryDown) conf.Alarm[valAlarm].Hour += -1;
      if (conf.Alarm[valAlarm].Hour > 250) conf.Alarm[valAlarm].Hour = 23;
      if (conf.Alarm[valAlarm].Hour > 23) conf.Alarm[valAlarm].Hour = 0;
      displayIntPage(nameBuf, "0..23", conf.Alarm[valAlarm].Hour);
    }
  }
  while (!RotarySwPressed);
}

void CmdAlarmMinute(byte valAlarm)
{
  displayIntPage(nameBuf, "0..59", conf.Alarm[valAlarm].Minute);
  do
  {
    RotaryTest();
    if (RotaryAnything)
    {
      if (AckPressed())
      {
        if (RotaryUp) conf.Alarm[valAlarm].Minute += 10;
        if (RotaryDown) conf.Alarm[valAlarm].Minute += -10;
      }
      else
      {
        if (RotaryUp) conf.Alarm[valAlarm].Minute += 1;
        if (RotaryDown) conf.Alarm[valAlarm].Minute += -1;
      }
      if (conf.Alarm[valAlarm].Minute > 200) conf.Alarm[valAlarm].Minute = conf.Alarm[valAlarm].Minute + 60;
      if (conf.Alarm[valAlarm].Minute > 59) conf.Alarm[valAlarm].Minute = conf.Alarm[valAlarm].Minute - 60;
      displayIntPage(nameBuf, "0..59", conf.Alarm[valAlarm].Minute);
    }
  }
  while (!RotarySwPressed);
}

boolean SimpleProtection(byte TimeOutSeconds)
{
  byte PasswordValue, AnswerValue = 4;
  now = rtc.now();
  StartTimeout = now.unixtime();
  PasswordValue = now.second() / 6;
  String Password = String(PasswordValue, DEC);
  displayIntPage("Wybierz", Password, AnswerValue);
  RotaryTest();
  while (!RotarySwPressed && (now.unixtime() - StartTimeout) <= TimeOutSeconds)
  {
    RotaryTest();
    if (RotaryAnything)
    {
      if (RotaryUp) AnswerValue += 1;
      if (RotaryDown) AnswerValue += -1;
      if (AnswerValue > 250) AnswerValue = 9;
      if (AnswerValue > 10) AnswerValue = 0;
      displayIntPage("Wybierz", Password, AnswerValue);
      StartTimeout = now.unixtime();
    }
    now = rtc.now();
  }
  return (AnswerValue == PasswordValue && (now.unixtime() - StartTimeout) <= TimeOutSeconds);
}

void CmdRTCTimeTest()
{
  now = rtc.now();
  MessageMenuString(DateToString(now.year(), now.month(), now.day()), TimeToString(now.hour(), now.minute(), now.second()));
}

String TimeToString(byte h, byte m, byte s)
{
  String hs = TwoDigitsSpace(h);
  String ms = TwoDigits(m);
  String ss = TwoDigits(s);
  String TimeS = String(hs + ":" + ms + ":" + ss);
  return TimeS;
}

String DateToString(int y, byte m, byte d)
{
  String ys = String(y, DEC);
  String ms = TwoDigits(m);
  String ds = TwoDigits(d);
  String DateS = String(ys + "." + ms + "." + ds);
  return DateS;
}

String TwoDigits(int value)
{
  String valueS = String(value, DEC);
  if (value < 10) valueS = String("0" + valueS);
  return valueS;
}

String FiveDigits(int value)
{
  String valueS = String(value, DEC);
  if (value < 10000) valueS = String(" " + valueS);
  if (value < 1000) valueS = String(" " + valueS);
  if (value < 100) valueS = String(" " + valueS);
  if (value < 10) valueS = String(" " + valueS);
  return valueS;
}

String TwoDigitsSpace(int value)
{
  String valueS = String(value, DEC);
  if (value < 10) valueS = String(" " + valueS);
  return valueS;
}

String ConvertAR2VoltStr (int value)
{
  String valueS = String (value / 204.6, 2);
  return valueS;
}

void CmdRTCBatLevel()
{
  RotaryTest();
  int valueMin = 1023;
  int valueMax = 0;
  while (!RotarySwPressed)
  {
    int value = analogRead(RTCBatSensorPin);
    valueMin = min(valueMin, value);
    valueMax = max(valueMax, value);
    String valueS = ConvertAR2VoltStr(valueMin);
    valueS += "V - ";
    valueS += ConvertAR2VoltStr(valueMax);
    valueS += "V";
    displayIntPage(nameBuf, valueS, value);
    delay(500);
    RotaryTest();
  }
}

void CmdFeederSensorsTest(byte valFeeder)
{
  String valueS;
  RotaryTest();
  while (!RotarySwPressed)
  {
    int value = analogRead(conf.Feeder[valFeeder].SensorPin);
    if (value < conf.Feeder[valFeeder].Threshold[0]) valueS = "1*"; else valueS = "1 ";
    if ((value >= conf.Feeder[valFeeder].Threshold[0]) && (value < conf.Feeder[valFeeder].Threshold[1])) valueS += " 2*"; else valueS += " 2 ";
    if ((value >= conf.Feeder[valFeeder].Threshold[1]) && (value < conf.Feeder[valFeeder].Threshold[2])) valueS += " 3*"; else valueS += " 3 ";
    if (value >= conf.Feeder[valFeeder].Threshold[2]) valueS += " 4*"; else valueS += " 4 ";
    displayIntPage(nameBuf, valueS, value);
    delay(500);
    RotaryTest();
  }

}

void CmdLidForceOpen(byte valFeeder)
{
  WriteLog('O', valFeeder);
  conf.Feeder[valFeeder].myservo.write(conf.Feeder[valFeeder].posOpen + conf.Feeder[valFeeder].dir * 15);
  delay(600);
  conf.Feeder[valFeeder].myservo.write(conf.Feeder[valFeeder].posOpen);
  delay(60);
  if (!LidOpened(valFeeder))
  {
    WriteLog('E', valFeeder);
    Beep();
  }
}


void CmdLidForceClose(byte valFeeder)
{
  WriteLog('Z', valFeeder);
  conf.Feeder[valFeeder].myservo.write(conf.Feeder[valFeeder].posClose - conf.Feeder[valFeeder].dir * 15);
  delay(600);
  while (LidOpened(valFeeder))
  {
    WriteLog('E', valFeeder);
    conf.Feeder[valFeeder].myservo.write((conf.Feeder[valFeeder].posOpen + conf.Feeder[valFeeder].posClose) / 2);
    delay(400);
    conf.Feeder[valFeeder].myservo.write(conf.Feeder[valFeeder].posClose - conf.Feeder[valFeeder].dir * 15);
    delay(400);
  }
  conf.Feeder[valFeeder].myservo.write(conf.Feeder[valFeeder].posClose);
  delay(60);
}

boolean LidOpened(byte valFeeder)
{
  int value = analogRead(conf.Feeder[valFeeder].SensorPin);
  boolean opened = ((value >= conf.Feeder[valFeeder].Threshold[0]) && (value < conf.Feeder[valFeeder].Threshold[1])) || (value >= conf.Feeder[valFeeder].Threshold[2]);
  return opened;
}

boolean FeederEmpty(byte valFeeder)
{
  boolean empty = (analogRead(conf.Feeder[valFeeder].SensorPin) >= conf.Feeder[valFeeder].Threshold[1]);
  return empty;
}

void CmdLidOpenClose(byte valFeeder)
{
  if (LidOpened(valFeeder)) CmdLidForceClose(valFeeder); else CmdLidForceOpen(valFeeder);
}

String StatusFeederS(byte valFeeder)
{
  String valueS;
  if (valFeeder == 0) valueS = "L:"; else valueS = "P:";
  if (LidOpened(valFeeder)) valueS += "OTW"; else valueS += "ZAM";
  if (FeederEmpty(valFeeder)) valueS += "-"; else valueS += "*";
  return valueS;
}

void CmdAckTest()
{
  RotaryTest();
  while (!RotarySwPressed)
  {
    byte value = AckPressed();
    displayIntPage(nameBuf, "logiczne", value);
    delay(500);
    RotaryTest();
  }
}

void CmdFeederSensorsCalibration(byte valFeeder)
{
  byte i;
  int k;
  MyLevel tempLevel;
  boolean validLevels = true;
  String valueS1 = "";
  String valueS2 = "";

  CmdLidForceOpen(valFeeder);
  for (i = 0; i <= 3; i++) {
    OneLevelSensorTest(valFeeder, i);
  }
  // BubbleSort
  for (k = 2; k >= 0; k--) {
    for (i = 0; i <= k; i++) {
      if (conf.Feeder[valFeeder].Level[i].LMedium > conf.Feeder[valFeeder].Level[i + 1].LMedium)
      {
        tempLevel = conf.Feeder[valFeeder].Level[i + 1];
        conf.Feeder[valFeeder].Level[i + 1] = conf.Feeder[valFeeder].Level[i];
        conf.Feeder[valFeeder].Level[i] = tempLevel;
      }
    }
  }
  // No overlaps?
  for (i = 0; i <= 2; i++) {
    if (conf.Feeder[valFeeder].Level[i].LMax >= conf.Feeder[valFeeder].Level[i + 1].LMin)
    {
      /*Serial.print(valFeeder);
        Serial.print(".");
        Serial.print(i);
        Serial.print(" LMax: ");
        Serial.print(conf.Feeder[valFeeder].Level[i].LMax);
        Serial.print(" ");
        Serial.print(valFeeder);
        Serial.print(".");
        Serial.print(i + 1);
        Serial.print(" LMin: ");
        Serial.println(conf.Feeder[valFeeder].Level[i + 1].LMin);*/
      validLevels = false;
    }
  }

  if (validLevels)
    // calculate thresholds
    for (i = 0; i <= 2; i++) {
      conf.Feeder[valFeeder].Threshold[i] = (conf.Feeder[valFeeder].Level[i].LMedium + conf.Feeder[valFeeder].Level[i + 1].LMedium) / 2;
    }
  else Beep();

  for (i = 0; i <= 2; i++) {
    valueS1 += String(valFeeder);
    valueS1 += ".";
    valueS1 += String(i);
    valueS1 += " ";
    valueS2 += String(conf.Feeder[valFeeder].Threshold[i]);
    valueS2 += " ";
  }
  MessageMenuString(valueS1, valueS2);

  /*for (i = 0; i <= 3; i++) {
    Serial.print("Level ");
    Serial.print(valFeeder);
    Serial.print(".");
    Serial.print(i);
    Serial.print(" Min: ");
    Serial.print(conf.Feeder[valFeeder].Level[i].LMin);
    Serial.print(" Max: ");
    Serial.print(conf.Feeder[valFeeder].Level[i].LMax);
    Serial.print(" Medium: ");
    Serial.print(conf.Feeder[valFeeder].Level[i].LMedium);
    Serial.println();
    }
    for (i = 0; i <= 2; i++) {
    Serial.print("Threshold ");
    Serial.print(valFeeder);
    Serial.print(".");
    Serial.print(i);
    Serial.print(": ");
    Serial.print(conf.Feeder[valFeeder].Threshold[i]);
    Serial.print(" ");
    }
    Serial.println();*/
}

void OneLevelSensorTest(byte valFeeder, byte valLevel)
{
  String valueS;
  conf.Feeder[valFeeder].Level[valLevel].LMin = MaxSensorVal;
  conf.Feeder[valFeeder].Level[valLevel].LMax = MinSensorVal;
  valueS = "czujnik ";
  valueS += String(valFeeder, DEC);
  valueS += ".";
  valueS += String(valLevel, DEC);
  MessageMenuString("Przygotuj", valueS);
  RotaryTest();
  while (!RotarySwPressed)
  {
    int value = analogRead(conf.Feeder[valFeeder].SensorPin);
    conf.Feeder[valFeeder].Level[valLevel].LMin = min(conf.Feeder[valFeeder].Level[valLevel].LMin, value);
    conf.Feeder[valFeeder].Level[valLevel].LMax = max(conf.Feeder[valFeeder].Level[valLevel].LMax, value);
    valueS = String(valFeeder, DEC);
    valueS += ".";
    valueS += String(valLevel, DEC);
    valueS += ": ";
    valueS += String(conf.Feeder[valFeeder].Level[valLevel].LMin, DEC);
    valueS += "..";
    valueS += String(conf.Feeder[valFeeder].Level[valLevel].LMax, DEC);
    valueS += "  ";
    displayIntPage(nameBuf, valueS, value);
    delay(150);
    RotaryTest();
  }
  conf.Feeder[valFeeder].Level[valLevel].LMedium = (conf.Feeder[valFeeder].Level[valLevel].LMin + conf.Feeder[valFeeder].Level[valLevel].LMax) / 2;
  /*Serial.print("Level ");
    Serial.print(valFeeder);
    Serial.print(".");
    Serial.print(valLevel);
    Serial.print(" Min: ");
    Serial.print(conf.Feeder[valFeeder].Level[valLevel].LMin);
    Serial.print(" Max: ");
    Serial.print(conf.Feeder[valFeeder].Level[valLevel].LMax);
    Serial.print(" Medium: ");
    Serial.print(conf.Feeder[valFeeder].Level[valLevel].LMedium);
    Serial.println();*/
}

void CmdSaveConf()
{
  EEPROM.put(0, conf);
  MessageMenuInt("Zapis bajtow", sizeof(conf));
}

void CmdLoadConf()
{
  MessageMenuInt("Konfig.wersji", EEPROM.read(0));
  EEPROM.get(0, conf);
  MessageMenuInt("Odczyt bajtow", sizeof(conf));
  CmdInit();
}

void CmdFactoryReset()
{
  WriteLog('R', '-');
  conf.VersionNumber = MyVersionNumber;
  conf.Feeder[0].dir = 1;
  conf.Feeder[1].dir = -1;
  conf.Feeder[0].posOpen = 160;
  conf.Feeder[0].posClose = 20;
  conf.Feeder[1].posOpen = 15;
  conf.Feeder[1].posClose = 165;
  // F1: 210, 250, 355, 400 albo 214, 275, 397, 466
  conf.Feeder[0].Threshold[0] = 228; //249
  conf.Feeder[0].Threshold[1] = 285; //338
  conf.Feeder[0].Threshold[2] = 349; //428
  // F2: 504, 567, 656, 711
  conf.Feeder[1].Threshold[0] = 474; //535
  conf.Feeder[1].Threshold[1] = 521; //611
  conf.Feeder[1].Threshold[2] = 567; //683

  conf.Feeder[0].SensorPin = Feeder1SensorPin;
  conf.Feeder[1].SensorPin = Feeder2SensorPin;
  conf.Feeder[0].ServoPin = Servo1Pin;
  conf.Feeder[1].ServoPin = Servo2Pin;

  conf.BacklightValue = 80;
  conf.ContrastValue = 80;
  conf.Alarm[0].Enable = 1;
  conf.Alarm[0].Hour = 15;
  conf.Alarm[0].Minute = 30;
  conf.Alarm[1].Enable = 1;
  conf.Alarm[1].Hour = 21;
  conf.Alarm[1].Minute = 30;

  CmdInit();
}

void CmdInit()
{
  byte index;

  MyLog.StartAddress = sizeof(conf);
  MyLog.SizeRec = sizeof(MyLog.log1);
  MyLog.MaxRec = (EEPROM.length() - MyLog.StartAddress) / MyLog.SizeRec;
  MyLog.First = 0;
  MyLog.Last = 0;
  MyLog.Count = 0;
  index = 0;
  EEPROM.get(MyLog.StartAddress, MyLog.log1);
  while ((MyLog.log1.Symbol != 0) && (index < MyLog.MaxRec ))
  {
    index++;
    EEPROM.get(MyLog.StartAddress + index * MyLog.SizeRec, MyLog.log1);
    if (MyLog.log1.Symbol = '*')
    {
      MyLog.Last = index - 1;
      //First?
    }
    else
    {
      MyLog.Count++;
    }
    if (MyLog.Count == MyLog.MaxRec - 1)
      MyLog.First = index + 1;
  }

  WriteLog ('S', '-');

  analogReference(DEFAULT);
  pinMode(conf.Feeder[0].SensorPin, INPUT);
  pinMode(conf.Feeder[1].SensorPin, INPUT);
  pinMode(RotarySwPin, INPUT_PULLUP);
  pinMode(AckSensorPin, INPUT_PULLUP);
  pinMode(RTCBatSensorPin, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  display.begin();
  turnBacklightOn();
  display.setTextSize(1);
  display.clearDisplay();
  display.setTextColor(BLACK, WHITE);

  if (! rtc.begin()) {
    display.println("Brak zegara");
    display.display();
    WriteLog('T', '-');
    Beep();
  }

  if (! rtc.isrunning()) {
    display.println("Ustaw zegar!");
    display.display();
    WriteLog('T', '-');
    Beep();
  }

  display.setTextSize(2);
  display.println("LEKOMAT");
  display.setTextSize(1);
  valueS = "v";
  valueS += String(MyVersionNumber, DEC);
  valueS += " ";
  now = DateTime(F(__DATE__), F(__TIME__));
  valueS += DateToString(now.year(), now.month(), now.day());
  display.println(valueS);
  display.println();
  display.println("autor: zoon");
  display.println(" @lu.onet.pl");
  display.display();

  conf.Feeder[0].myservo.attach(conf.Feeder[0].ServoPin);
  delay(500);
  conf.Feeder[1].myservo.attach(conf.Feeder[1].ServoPin);
  delay(500);

  if (!AckPressed())
  {
    conf.Feeder[0].myservo.write(90);
    delay(500);
    conf.Feeder[1].myservo.write(90);
    delay(500);

    for (int i = 0; i <= MaxFeederNumber; i++)
    {
      CmdLidForceClose(i);
    }
  }
}

void turnBacklightOn()
{
  pinMode(BacklightPin, OUTPUT);
  // digitalWrite(BacklightPin, LOW);
  analogWrite(BacklightPin, conf.BacklightValue);
  display.setContrast(conf.ContrastValue);
}

boolean AckPressed()
{
  byte value = digitalRead(AckSensorPin);
  return (value == 0);
}

void NextAlarmCalculate()
{
  DateTime AlarmVal;

  CheckRefill();

  now = rtc.now();
  NextAlarmTime = {now.year() + 2, 1, 1, 1, 1, 1};
  AnyValidAlarm = false;
  // *** wybór najbliższego alarmu i sprawdzenie czy jest jakiś włączony alarm z pełnym dozownikiem i zamkniętą pokrywą
  for (int i = 0; i <= MaxAlarmNumber; i++)
  {
    if (conf.Alarm[i].Enable && !LidOpened(i) && !FeederEmpty(i))
    {
      AnyValidAlarm = true;
      AlarmVal = {now.year(), now.month(), now.day(), conf.Alarm[i].Hour, conf.Alarm[i].Minute, 0};
      if (now.unixtime() > AlarmVal.unixtime())
        AlarmVal = {now.year(), now.month(), uint8_t(now.day() + 1), conf.Alarm[i].Hour, conf.Alarm[i].Minute, 0};
      if (NextAlarmTime.unixtime() > AlarmVal.unixtime())
      {
        NextAlarmTime = AlarmVal;
        NextAlarmNumber = i;
      }
    }
  }
  if (AnyValidAlarm && now.hour() == conf.Alarm[NextAlarmNumber].Hour && now.minute() == conf.Alarm[NextAlarmNumber].Minute && now.second() < 5)
    RunAlarm(NextAlarmNumber);
}

void RunAlarm(byte valAlarm)
{
  WriteLog ('A', valAlarm);
  AlarmMessage("NACISN.", " PRZY-", " CISK!", true, 0);
  CmdLidForceOpen(valAlarm);
  for (int i = 0; (i <= 8) && (!FeederEmpty(valAlarm)); i++)
    delay(1000);
  AlarmMessage("WYJMIJ", "  KIE-", "LISZEK!", false, valAlarm);
  WriteLog ('W', valAlarm);
}

void AlarmMessage(String S1, String S2, String S3, boolean CheckAck, byte valFeeder)
{
  display.setTextSize(2);
  display.clearDisplay();
  display.setTextColor(BLACK, WHITE);
  display.println(S1);
  display.println(S2);
  display.println(S3);
  display.display();
  RotaryTest();
  while (CheckAck && (!AckPressed() && !RotarySwPressed) || (!CheckAck && !FeederEmpty(valFeeder)))
  {
    now = rtc.now();
    if (bitRead(now.second(), 0) == 0)
    {
      digitalWrite(LED_BUILTIN, HIGH);
      analogWrite(BacklightPin, 0);
    }
    else
    {
      digitalWrite(LED_BUILTIN, LOW);
      analogWrite(BacklightPin, 255);
    }
    RotaryTest();
  }
  analogWrite(BacklightPin, conf.BacklightValue);
  digitalWrite(LED_BUILTIN, LOW);
  display.clearDisplay();
  display.println();
  display.println("  OK!");
  display.display();
}

void CheckRefill()
{
  for (int i = 0; i <= MaxFeederNumber; i++)
  {
    if (LidOpened(i) && !FeederEmpty(i))
    {
      AlarmMessage("NACISN.", " PRZY-", " CISK!", true, 0);
      if (!FeederEmpty(i))
      {
        WriteLog('K', i);
        CmdLidForceClose(i);
      }
    }
  }
}

void WriteLog(char Symbol, char Parameter)
{
  log1.Symbol = Symbol;
  now = rtc.now();
  if (Parameter == 0) log1.Parameter = 'L';
  else if (Parameter == 1) log1.Parameter = 'P';
  else log1.Parameter = Parameter;
  log1.UDT = now.unixtime();
  //Serial.print(Symbol);
  //Serial.println(log1.Parameter);
}


#ifndef _LekomatMenu_
#define _LekomatMenu_
#include "MenuManager.h"
#include <avr/pgmspace.h>

/*
Generated using LCD Menu Builder at http://lcd-menu-bulder.cohesivecomputing.co.uk/.
*/

enum LekomatMenuCommandId
{
  mnuCmdBack = 0,
  mnuCmdLidsRoot,
  mnuCmdLid1Root,
  mnuCmdLid1OpenClose,
  mnuCmdLid1ForceOpen,
  mnuCmdLid1ForceClose,
  mnuCmdLid2Root,
  mnuCmdLid2OpenClose,
  mnuCmdLid2ForceOpen,
  mnuCmdLid2ForceClose,
  mnuCmdSettingsRoot,
  mnuCmdAlarmsRoot,
  mnuCmdAlarm1Root,
  mnuCmdA1Hour,
  mnuCmdA1Minute,
  mnuCmdA1Enable,
  mnuCmdAlarm2Root,
  mnuCmdA2Hour,
  mnuCmdA2Minute,
  mnuCmdA2Enable,
  mnuCmdTimeSetRoot,
  mnuCmdTimeSetYear,
  mnuCmdTimeSetMonth,
  mnuCmdTimeSetDay,
  mnuCmdTimeSetHour,
  mnuCmdTimeSetMinute,
  mnuCmdLCDSetRoot,
  mnuCmdLCDSetBack,
  mnuCmdLCDSetContrast,
  mnuCmdFeeder1SensorsCalibration,
  mnuCmdFeeder2SensorsCalibration,
  mnuCmdLoadConf,
  mnuCmdSaveConf,
  mnuCmdFactoryReset,
  mnuCmdTestsRoot,
  mnuCmdFeeder1TestRoot,
  mnuCmdFeeder1SensorsTest,
  mnuCmdFeeder1ServoTest,
  mnuCmdFeeder2TestRoot,
  mnuCmdFeeder2SensorsTest,
  mnuCmdFeeder2ServoTest,
  mnuCmdRTCTest,
  mnuCmdRTCTimeTest,
  mnuCmdRTCBatLevel,
  mnuCmdBuzzerLEDTest,
  mnuCmdAckTest
};

PROGMEM const char LekomatMenu_back[] = "Powrot";
PROGMEM const char LekomatMenu_exit[] = "Wyjscie";

PROGMEM const char LekomatMenu_2_1_1_1[] = "Godzina";
PROGMEM const char LekomatMenu_2_1_1_2[] = "Minuta";
PROGMEM const char LekomatMenu_2_1_1_3[] = "Wlaczony?";
PROGMEM const MenuItem LekomatMenu_List_2_1_1[] = {{mnuCmdA1Hour, LekomatMenu_2_1_1_1}, {mnuCmdA1Minute, LekomatMenu_2_1_1_2}, {mnuCmdA1Enable, LekomatMenu_2_1_1_3}, {mnuCmdBack, LekomatMenu_back}};

PROGMEM const char LekomatMenu_2_1_2_1[] = "Godzina";
PROGMEM const char LekomatMenu_2_1_2_2[] = "Minuta";
PROGMEM const char LekomatMenu_2_1_2_3[] = "Wlaczony?";
PROGMEM const MenuItem LekomatMenu_List_2_1_2[] = {{mnuCmdA2Hour, LekomatMenu_2_1_2_1}, {mnuCmdA2Minute, LekomatMenu_2_1_2_2}, {mnuCmdA2Enable, LekomatMenu_2_1_2_3}, {mnuCmdBack, LekomatMenu_back}};

PROGMEM const char LekomatMenu_1_1_1[] = "Zmiana stanu";
PROGMEM const char LekomatMenu_1_1_2[] = "Wymusz. otw.";
PROGMEM const char LekomatMenu_1_1_3[] = "Wymusz. zam.";
PROGMEM const MenuItem LekomatMenu_List_1_1[] = {{mnuCmdLid1OpenClose, LekomatMenu_1_1_1}, {mnuCmdLid1ForceOpen, LekomatMenu_1_1_2}, {mnuCmdLid1ForceClose, LekomatMenu_1_1_3}, {mnuCmdBack, LekomatMenu_back}};

PROGMEM const char LekomatMenu_1_2_1[] = "Zmiana stanu";
PROGMEM const char LekomatMenu_1_2_2[] = "Wymusz. otw.";
PROGMEM const char LekomatMenu_1_2_3[] = "Wymusz. zam.";
PROGMEM const MenuItem LekomatMenu_List_1_2[] = {{mnuCmdLid2OpenClose, LekomatMenu_1_2_1}, {mnuCmdLid2ForceOpen, LekomatMenu_1_2_2}, {mnuCmdLid2ForceClose, LekomatMenu_1_2_3}, {mnuCmdBack, LekomatMenu_back}};

PROGMEM const char LekomatMenu_2_1_1[] = "Alarm lewy";
PROGMEM const char LekomatMenu_2_1_2[] = "Alarm prawy";
PROGMEM const MenuItem LekomatMenu_List_2_1[] = {{mnuCmdAlarm1Root, LekomatMenu_2_1_1, LekomatMenu_List_2_1_1, menuCount(LekomatMenu_List_2_1_1)}, {mnuCmdAlarm2Root, LekomatMenu_2_1_2, LekomatMenu_List_2_1_2, menuCount(LekomatMenu_List_2_1_2)}, {mnuCmdBack, LekomatMenu_back}};

PROGMEM const char LekomatMenu_2_2_1[] = "Rok";
PROGMEM const char LekomatMenu_2_2_2[] = "Miesiac";
PROGMEM const char LekomatMenu_2_2_3[] = "Dzien";
PROGMEM const char LekomatMenu_2_2_4[] = "Godzina";
PROGMEM const char LekomatMenu_2_2_5[] = "Minuta";
PROGMEM const MenuItem LekomatMenu_List_2_2[] = {{mnuCmdTimeSetYear, LekomatMenu_2_2_1}, {mnuCmdTimeSetMonth, LekomatMenu_2_2_2}, {mnuCmdTimeSetDay, LekomatMenu_2_2_3}, {mnuCmdTimeSetHour, LekomatMenu_2_2_4}, {mnuCmdTimeSetMinute, LekomatMenu_2_2_5}, {mnuCmdBack, LekomatMenu_back}};

PROGMEM const char LekomatMenu_2_3_1[] = "Podswietl.";
PROGMEM const char LekomatMenu_2_3_2[] = "Kontrast";
PROGMEM const MenuItem LekomatMenu_List_2_3[] = {{mnuCmdLCDSetBack, LekomatMenu_2_3_1}, {mnuCmdLCDSetContrast, LekomatMenu_2_3_2}, {mnuCmdBack, LekomatMenu_back}};

PROGMEM const char LekomatMenu_3_1_1[] = "Czujniki";
PROGMEM const char LekomatMenu_3_1_2[] = "Serwo";
PROGMEM const MenuItem LekomatMenu_List_3_1[] = {{mnuCmdFeeder1SensorsTest, LekomatMenu_3_1_1}, {mnuCmdFeeder1ServoTest, LekomatMenu_3_1_2}, {mnuCmdBack, LekomatMenu_back}};

PROGMEM const char LekomatMenu_3_2_1[] = "Czujniki";
PROGMEM const char LekomatMenu_3_2_2[] = "Serwo";
PROGMEM const MenuItem LekomatMenu_List_3_2[] = {{mnuCmdFeeder2SensorsTest, LekomatMenu_3_2_1}, {mnuCmdFeeder2ServoTest, LekomatMenu_3_2_2}, {mnuCmdBack, LekomatMenu_back}};

PROGMEM const char LekomatMenu_3_3_1[] = "Czas";
PROGMEM const char LekomatMenu_3_3_2[] = "Bateria";
PROGMEM const MenuItem LekomatMenu_List_3_3[] = {{mnuCmdRTCTimeTest, LekomatMenu_3_3_1}, {mnuCmdRTCBatLevel, LekomatMenu_3_3_2}, {mnuCmdBack, LekomatMenu_back}};

PROGMEM const char LekomatMenu_1_1[] = "Pokrywa L";
PROGMEM const char LekomatMenu_1_2[] = "Pokrywa P";
PROGMEM const MenuItem LekomatMenu_List_1[] = {{mnuCmdLid1Root, LekomatMenu_1_1, LekomatMenu_List_1_1, menuCount(LekomatMenu_List_1_1)}, {mnuCmdLid2Root, LekomatMenu_1_2, LekomatMenu_List_1_2, menuCount(LekomatMenu_List_1_2)}, {mnuCmdBack, LekomatMenu_back}};

PROGMEM const char LekomatMenu_2_1[] = "Alarmy";
PROGMEM const char LekomatMenu_2_2[] = "Zegar";
PROGMEM const char LekomatMenu_2_3[] = "Ekran LCD";
PROGMEM const char LekomatMenu_2_4[] = "Czujniki L";
PROGMEM const char LekomatMenu_2_5[] = "Czujniki P";
PROGMEM const char LekomatMenu_2_6[] = "Wczyt.konfig";
PROGMEM const char LekomatMenu_2_7[] = "Zapis konfig";
PROGMEM const char LekomatMenu_2_8[] = "Ustaw. fabr.";
PROGMEM const MenuItem LekomatMenu_List_2[] = {{mnuCmdAlarmsRoot, LekomatMenu_2_1, LekomatMenu_List_2_1, menuCount(LekomatMenu_List_2_1)}, {mnuCmdTimeSetRoot, LekomatMenu_2_2, LekomatMenu_List_2_2, menuCount(LekomatMenu_List_2_2)}, {mnuCmdLCDSetRoot, LekomatMenu_2_3, LekomatMenu_List_2_3, menuCount(LekomatMenu_List_2_3)}, {mnuCmdFeeder1SensorsCalibration, LekomatMenu_2_4}, {mnuCmdFeeder2SensorsCalibration, LekomatMenu_2_5}, {mnuCmdLoadConf, LekomatMenu_2_6}, {mnuCmdSaveConf, LekomatMenu_2_7}, {mnuCmdFactoryReset, LekomatMenu_2_8}, {mnuCmdBack, LekomatMenu_back}};

PROGMEM const char LekomatMenu_3_1[] = "Podajnik L";
PROGMEM const char LekomatMenu_3_2[] = "Podajnik P";
PROGMEM const char LekomatMenu_3_3[] = "Zegar";
PROGMEM const char LekomatMenu_3_4[] = "Glosnik/LED";
PROGMEM const char LekomatMenu_3_5[] = "Przycisk";
PROGMEM const MenuItem LekomatMenu_List_3[] = {{mnuCmdFeeder1TestRoot, LekomatMenu_3_1, LekomatMenu_List_3_1, menuCount(LekomatMenu_List_3_1)}, {mnuCmdFeeder2TestRoot, LekomatMenu_3_2, LekomatMenu_List_3_2, menuCount(LekomatMenu_List_3_2)}, {mnuCmdRTCTest, LekomatMenu_3_3, LekomatMenu_List_3_3, menuCount(LekomatMenu_List_3_3)}, {mnuCmdBuzzerLEDTest, LekomatMenu_3_4}, {mnuCmdAckTest, LekomatMenu_3_5}, {mnuCmdBack, LekomatMenu_back}};

PROGMEM const char LekomatMenu_1[] = "Pokrywy";
PROGMEM const char LekomatMenu_2[] = "Ustawienia";
PROGMEM const char LekomatMenu_3[] = "Testy";
PROGMEM const MenuItem LekomatMenu_Root[] = {{mnuCmdLidsRoot, LekomatMenu_1, LekomatMenu_List_1, menuCount(LekomatMenu_List_1)}, {mnuCmdSettingsRoot, LekomatMenu_2, LekomatMenu_List_2, menuCount(LekomatMenu_List_2)}, {mnuCmdTestsRoot, LekomatMenu_3, LekomatMenu_List_3, menuCount(LekomatMenu_List_3)}, {mnuCmdBack, LekomatMenu_exit}};

/*
case mnuCmdLid1OpenClose :
  break;
case mnuCmdLid1ForceOpen :
  break;
case mnuCmdLid1ForceClose :
  break;
case mnuCmdLid2OpenClose :
  break;
case mnuCmdLid2ForceOpen :
  break;
case mnuCmdLid2ForceClose :
  break;
case mnuCmdA1Hour :
  break;
case mnuCmdA1Minute :
  break;
case mnuCmdA1Enable :
  break;
case mnuCmdA2Hour :
  break;
case mnuCmdA2Minute :
  break;
case mnuCmdA2Enable :
  break;
case mnuCmdTimeSetYear :
  break;
case mnuCmdTimeSetMonth :
  break;
case mnuCmdTimeSetDay :
  break;
case mnuCmdTimeSetHour :
  break;
case mnuCmdTimeSetMinute :
  break;
case mnuCmdLCDSetBack :
  break;
case mnuCmdLCDSetContrast :
  break;
case mnuCmdFeeder1SensorsCalibration :
  break;
case mnuCmdFeeder2SensorsCalibration :
  break;
case mnuCmdLoadConf :
  break;
case mnuCmdSaveConf :
  break;
case mnuCmdFactoryReset :
  break;
case mnuCmdFeeder1SensorsTest :
  break;
case mnuCmdFeeder1ServoTest :
  break;
case mnuCmdFeeder2SensorsTest :
  break;
case mnuCmdFeeder2ServoTest :
  break;
case mnuCmdRTCTimeTest :
  break;
case mnuCmdRTCBatLevel :
  break;
case mnuCmdBuzzerLEDTest :
  break;
case mnuCmdAckTest :
  break;
*/

/*
<?xml version="1.0"?>
<RootMenu xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema">
  <Config IdPrefix="mnuCmd" VarPrefix="LekomatMenu" UseNumbering="false" IncludeNumberHierarchy="false"
          MaxNameLen="12" MenuBackFirstItem="false" BackText="Powrot" ExitText="Wyjscie" AvrProgMem="true" />
  <MenuItems>
    <Item Id="LidsRoot" Name="Pokrywy">
      <MenuItems>
        <Item Id="Lid1Root" Name="Pokrywa L">
          <MenuItems>
            <Item Id="Lid1OpenClose" Name="Zmiana stanu" />
            <Item Id="Lid1ForceOpen" Name="Wymusz. otw." />
            <Item Id="Lid1ForceClose" Name="Wymusz. zam." />
          </MenuItems>
        </Item>
        <Item Id="Lid2Root" Name="Pokrywa P">
          <MenuItems>
            <Item Id="Lid2OpenClose" Name="Zmiana stanu" />
            <Item Id="Lid2ForceOpen" Name="Wymusz. otw." />
            <Item Id="Lid2ForceClose" Name="Wymusz. zam." />
          </MenuItems>
        </Item>
      </MenuItems>
    </Item>
    <Item Id="SettingsRoot" Name="Ustawienia">
      <MenuItems>
        <Item Id="AlarmsRoot" Name="Alarmy">
          <MenuItems>
            <Item Id="Alarm1Root" Name="Alarm lewy">
              <MenuItems>
                <Item Id="A1Hour" Name="Godzina"/>
                <Item Id="A1Minute" Name="Minuta"/>
                <Item Id="A1Enable" Name="Wlaczony?"/>
              </MenuItems>
            </Item>
            <Item Id="Alarm2Root" Name="Alarm prawy">
              <MenuItems>
                <Item Id="A2Hour" Name="Godzina"/>
                <Item Id="A2Minute" Name="Minuta"/>
                <Item Id="A2Enable" Name="Wlaczony?"/>
              </MenuItems>
            </Item>
          </MenuItems>
        </Item>
        <Item Id="TimeSetRoot" Name="Zegar">
          <MenuItems>
            <Item Id="TimeSetYear" Name="Rok" />
            <Item Id="TimeSetMonth" Name="Miesiac" />
            <Item Id="TimeSetDay" Name="Dzien" />
            <Item Id="TimeSetHour" Name="Godzina" />
            <Item Id="TimeSetMinute" Name="Minuta" />
          </MenuItems>
        </Item>
        <Item Id="LCDSetRoot" Name="Ekran LCD">
          <MenuItems>
            <Item Id="LCDSetBack" Name="Podswietl." />
            <Item Id="LCDSetContrast" Name="Kontrast" />
          </MenuItems>
        </Item>
        <Item Id="Feeder1SensorsCalibration" Name="Czujniki L" />
        <Item Id="Feeder2SensorsCalibration" Name="Czujniki P" />
        <Item Id="LoadConf" Name="Wczyt.konfig"/>
        <Item Id="SaveConf" Name="Zapis konfig"/>
        <Item Id="FactoryReset" Name="Ustaw. fabr."/>
      </MenuItems>
    </Item>
    <Item Id="TestsRoot" Name="Testy">
      <MenuItems>
        <Item Id="Feeder1TestRoot" Name="Podajnik L">
          <MenuItems>
            <Item Id="Feeder1SensorsTest" Name="Czujniki" />
            <Item Id="Feeder1ServoTest" Name="Serwo" />
          </MenuItems>
        </Item>
        <Item Id="Feeder2TestRoot" Name="Podajnik P">
          <MenuItems>
            <Item Id="Feeder2SensorsTest" Name="Czujniki" />
            <Item Id="Feeder2ServoTest" Name="Serwo" />
          </MenuItems>
        </Item>
        <Item Id="RTCTest" Name="Zegar">
          <MenuItems>
            <Item Id="RTCTimeTest" Name="Czas" />
            <Item Id="RTCBatLevel" Name="Bateria" />
          </MenuItems>
        </Item>
        <Item Id="BuzzerLEDTest" Name="Glosnik/LED" />
        <Item Id="AckTest" Name="Przycisk" />
      </MenuItems>
    </Item>
  </MenuItems>
</RootMenu>
*/
#endif


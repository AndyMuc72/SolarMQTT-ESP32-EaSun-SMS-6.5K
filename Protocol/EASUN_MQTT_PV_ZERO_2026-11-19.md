# EASUN MQTT: Solar-Nullwerte – 19.11.2026

Öffentliche Version: 0.1.0-easun.10 (funktional geprüft als 0.1.0-easun.10-cvte-test.2), Board wemos_d1_mini32.

## Ursache und Korrektur
Die Weboberfläche hat bisher PV-Spannungen unter 10 V und PV-Leistungen unter 10 W nur bei der Darstellung auf null gesetzt. MQTT verwendete die unveränderten Messwerte. Jetzt wird die vorhandene MQTT-Snapshot-Kopie vor dem Senden normalisiert, gemeinsam für einzelne LiveData-Topics und das Data-Gesamt-JSON.

Nur MODBUS_EASUN_SMS:
- Betrag der PV-Spannung kleiner 10 V: MQTT-Spannung 0.
- Betrag der PV-Leistung kleiner 10 W: MQTT-Leistung 0.
- Bei einer dieser Bedingungen: zugehöriger PV-Strom ebenfalls 0. Es gibt ausdrücklich keine 10-A-Schwelle. Kleine Ströme bei normaler Spannung und mindestens 10 W bleiben erhalten.
- Die Grenzen sind strikt kleiner: genau 10 V / 10 W bleiben bestehen.
- Gesamt-PV, PV1 und gegebenenfalls vorhandene PV2-Felder werden berücksichtigt. Fehlende Felder werden nicht ergänzt, kein zweiter Tracker wird aktiviert.
- Beispiel aus dem Screenshot: 1,7 V / 0,58 A / 1 W ergibt ausgehend 0 V / 0 A / 0 W; PV1 mit 0,59 A ebenfalls 0.
- SolarState, Web-Datenvorschau, Register-Rohdaten und Energiezähler bleiben unverändert. AC-Wirkleistung, VA, Ausgangsstrom und Batterie/BMS werden nicht verändert.
- Bereits retained gespeicherte PV-Topics werden beim nächsten regulären Publish überschrieben, keine Löschung anderer Topics.

## Leerlauf-Beobachtung
Der Nutzer bestätigt Inselbetrieb ohne AC-Eingang und ohne angeschlossene AC-Last. Im Screenshot: AC_Out_Watt=-8, AC_Out_VA=75, AC_Out_Current_L1=0,33 A. 230 V × 0,33 A ≈ 76 VA. Das ist numerisch konsistent, jedoch kein Beweis für 75 W Eigenverbrauch oder für einen ausschließlich externen Laststrom. Interner Filterstrom oder Messoffset bleiben unbestätigte Erklärungen. VA/A werden nicht durch einen pauschalen Offset oder eine 80-VA-Schwelle verändert.

## Prüfung
Native Regression: Screenshotwerte, Grenzwerte, kleine gültige Tagesströme, unveränderte andere Protokolle/AC-/Rohwerte/Energiezähler und fehlende Werte. Zusätzlich bestehender UI-Test. Build-/Geräteteststatus separat dokumentiert.

void loop() {
  setDeadband();
  salinityReading = salinity_value(sensor_reading( salinity_power_pin, salinity_input_pin ));
  LCDUpdate();
  if (millis()-last_salinity_update > deadtime) {
    if ( salinityReading>UCL ) {
      
      DIOpenTime = (massInTank*((G*(setpoint-salinityReading))/((1-F)*(0-salinityReading))))/DI_flowrate;
      cTime = millis();

      digitalWrite(solDI_pin, HIGH); 
      
      for (float i = millis(); i < (cTime + DIOpenTime); i = millis())
      {
        LCDSerial.write(254);
        LCDSerial.write(229);
        LCDSerial.write("ON ");
        
        Serial.print(setpoint,4); Serial.print(" ");
        Serial.print(salinityReading,4); Serial.print(" ");
        Serial.println(DIOpenTime);    
        last_salinity_update = millis();

        setDeadband();
        salinityReading = salinity_value(sensor_reading( salinity_power_pin, salinity_input_pin ));
        LCDUpdate();
      }
      digitalWrite(solDI_pin, LOW);      
    }
    if ( salinityReading<LCL ) {

      DIOpenTime = (massInTank*((G*(setpoint-salinityReading))/((1-F)*(0-salinityReading))))/DI_flowrate;
      cTime = millis();

      digitalWrite(solSalt_pin, HIGH);

      for (float i = millis(); i < (cTime + DIOpenTime); i = millis())
      {
        LCDSerial.write(254);
        LCDSerial.write(213);
        LCDSerial.write("ON ");

        Serial.print(setpoint,4); Serial.print(" ");
        Serial.print(salinityReading,4); Serial.print(" ");
        Serial.println(saltOpenTime);       
        last_salinity_update = millis();

        setDeadband();
        salinityReading = salinity_value(sensor_reading( salinity_power_pin, salinity_input_pin ));
        LCDUpdate();
      }
      digitalWrite(solSalt_pin, LOW); 
      
    } 
  }
}

void set_pin_directions_and_default_pin_states()
{
  pinMode(CG_fire, OUTPUT);
  pinMode(CG_position, OUTPUT);
  pinMode(CG_charge, OUTPUT);
  

  digitalWrite(CG_fire,HIGH);
  digitalWrite(CG_position,HIGH);
  digitalWrite(CG_charge,LOW);

  pinMode(LED, OUTPUT);
  digitalWrite(LED,HIGH);  
  pinMode(STP, OUTPUT);
  pinMode(DIR, OUTPUT);
  pinMode(MS1, OUTPUT);
  pinMode(MS2, OUTPUT);
  pinMode(MS3, OUTPUT);
  pinMode(EN, OUTPUT);
  resetMotorDriverPins(); 
}

void resetMotorDriverPins()
{
  digitalWrite(STP, LOW);
  digitalWrite(DIR, LOW);
  digitalWrite(MS1, HIGH);
  digitalWrite(MS2, HIGH);
  digitalWrite(MS3, HIGH);
  digitalWrite(EN, HIGH);
}

void StepForwardDefault()
{
  digitalWrite(EN, LOW);
  memory_card.println("Moving forward at default step mode.");
  digitalWrite(DIR, LOW); //Pull direction pin low to move "forward"
  for(int x= 1; x<step_increment; x++)  //Loop the forward stepping enough times for motion to be visible
  {
    digitalWrite(STP,HIGH); //Trigger one step forward
    delay(1);
    digitalWrite(STP,LOW); //Pull step pin low so it can be triggered again
    delay(1);
  }
  set_motor_position_to_flash(step_increment);
  resetMotorDriverPins();
}

void StepReverseDefault()
{
  digitalWrite(EN, LOW);
  memory_card.println("Moving in reverse at default step mode.");
  digitalWrite(DIR, HIGH); //Pull direction pin high to move in "reverse"
  for(int x= 1; x<step_increment; x++)  //Loop the stepping enough times for motion to be visible
  {
    digitalWrite(STP,HIGH); //Trigger one step
    delay(1);
    digitalWrite(STP,LOW); //Pull step pin low so it can be triggered again
    delay(1);
  }
  set_motor_position_to_flash(step_increment*-1);
  resetMotorDriverPins();
}
double depth()
{
  double P=sensor.getPressure(ADC_4096);
  return(44330.0*(1-pow(P/pressure_baseline,1/5.255)));
}

void set_state_to_flash(int new_state)
{
  int old_state=dueFlashStorage.read(0);
  if(old_state==255)
     old_state=CHILLIN;
  memory_card.print("Old state: ");
  memory_card.print(state_array[old_state]);
  while(dueFlashStorage.read(0)!=new_state)
    dueFlashStorage.write(0,new_state);
  state=new_state;
  memory_card.print(" New state: ");
  memory_card.println(state_array[state]);
}
void get_state_from_flash()
{
  state=dueFlashStorage.read(0);
  if(state==255)
    state=CHILLIN;
  memory_card.print("State read from Flash: ");
  memory_card.println(state_array[state]);
}

void set_motor_position_to_flash(int position_difference)
{
  int old_position=dueFlashStorage.read(1);
  if(old_position==255)
     old_position=0;
  memory_card.print("Old Position: ");
  memory_card.print(old_position);
  int new_position=old_position+position_difference;
  while(dueFlashStorage.read(1)!=new_position)
    dueFlashStorage.write(1,new_position);
  motor_position=new_position;
  memory_card.print(" New Position: ");
  memory_card.println(new_position);
}

void get_motor_position_from_flash()
{
  motor_position=dueFlashStorage.read(1);
  if(motor_position==255)
    motor_position=CHILLIN;
  memory_card.print("Motor Position read from Flash: ");
  memory_card.println(motor_position);
}

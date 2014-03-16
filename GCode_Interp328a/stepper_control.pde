#include "parameters.h"
#include "pins.h"

//init our variables
long max_delta;
long x_counter;
long y_counter;
long z_counter;
long x_pos = 0; // x position in terms of absoloute motor stepps
long y_pos = 0; // y position in terms of absoloute motor stepps
long z_pos = 0; // z position in terms of absoloute motor stepps
bool x_can_step;
bool y_can_step;
bool z_can_step;
int milli_delay;
  byte switchState = 0xff;

void init_steppers()
{       //init our points.
	current_units.x = 0.0;
	current_units.y = 0.0;
	current_units.z = 0.0;
	target_units.x = 0.0;
	target_units.y = 0.0;
	target_units.z = 0.0;
	stepperByte=0;
	disable_steppers();
        dda_checkStops(); // check we are not against an end stop

	//figure our stuff.
	calculate_deltas();
}

// Check the endstop inputs
void dda_checkStops()    
{ 
  // check the endstops and update variables if needed
  if(digitalRead(LIMIT_CHANGE_PIN) == LOW || switchState != (byte)0xff) {
  switchState = expanderR(LIMIT_READ, GPIOA);
  if(switchState != (byte)0xff){
    lcd.setCursor(0,0);
         lcd.clear();
         lcd.print("Limit Switch ");
  }
  if((switchState & X_MAX_BIT) == 0)  {x_max_OK = false; lcd.print("Xmax "); } else  x_max_OK = true;
  if((switchState & X_MIN_BIT) == 0)  {x_min_OK = false; lcd.print("Xmin "); } else  x_min_OK = true;
  if((switchState & Y_MAX_BIT) == 0)  {y_max_OK = false; lcd.print("Ymax "); } else  y_max_OK = true;
  if((switchState & Y_MIN_BIT) == 0)  {y_min_OK = false; lcd.print("Ymin "); } else  x_min_OK = true;
  if((switchState & Z_MAX_BIT) == 0)  {z_max_OK = false; lcd.print("Zmax "); } else  z_max_OK = true;
  if((switchState & Z_MIN_BIT) == 0)  {z_min_OK = false; lcd.print("Zmin "); } else  z_min_OK = true;
  if((switchState & TOOL_HEIGHT_BIT) == 0)  {tool_height_OK = false; lcd.print("Tool Switch "); } else  tool_height_OK = true;
  } 
}

void dda_move(long micro_delay)
{
	//turn on steppers to start moving =)
	enable_steppers();
	
	//figure out our maximu delta
	max_delta = max(delta_steps.x, delta_steps.y);
	max_delta = max(delta_steps.z, max_delta);

	//init stuff.
	long x_counter = -max_delta/2;
	long y_counter = -max_delta/2;
	long z_counter = -max_delta/2;
	
	//our step flags
	bool x_can_step = 0;
	bool y_can_step = 0;
	bool z_can_step = 0;

	//how long do we delay for?
	if (micro_delay >= 16383)
		milli_delay = micro_delay / 1000;
	else
		milli_delay = 0;

	//do our DDA line!
	do
	{
                dda_checkStops();  // check we are not up against any end stops / limit switches
		x_can_step = can_step(current_steps.x, target_steps.x, x_direction);
                if(x_can_step){
                 if( (x_direction == 1 && x_max_OK==false) || (x_direction == 0 && x_min_OK==false) ) {
                   x_can_step= false;
                   target_steps.x = current_steps.x;
                 }
                }
		y_can_step = can_step(current_steps.y, target_steps.y, y_direction);
                if(y_can_step){
                 if( (y_direction == 1 && y_max_OK==false) || (y_direction == 0 && y_min_OK==false) ) {
                   y_can_step= false;
                   target_steps.y = current_steps.y;
                 }
                }
		z_can_step = can_step(current_steps.z, target_steps.z, z_direction);
                if(z_can_step){
                 if( (z_direction == 1 && z_max_OK==false) || (z_direction == 0 && z_min_OK==false) ) {
                   z_can_step= false;
                   target_steps.z = current_steps.z;
                 }
                }

		if (x_can_step)
		{
			x_counter += delta_steps.x;		
			if (x_counter > 0)
			{
				stepperByte |= X_STEP_BIT;
				x_counter -= max_delta;
				
				if (x_direction)
					{ current_steps.x++; x_pos++; }
				else
				        { current_steps.x--; x_pos--; }
			}
		}

		if (y_can_step)
		{
			y_counter += delta_steps.y;			
			if (y_counter > 0)
			{
				stepperByte |= Y_STEP_BIT;
				y_counter -= max_delta;

				if (y_direction)
					{ current_steps.y++; y_pos++; }
				else
					{ current_steps.y--; y_pos--; }
			}
		}
		
		if (z_can_step)
		{
			z_counter += delta_steps.z;			
			if (z_counter > 0)
			{
				stepperByte |= Z_STEP_BIT;
				z_counter -= max_delta;
				
				if (z_direction)
					{ current_steps.z++; z_pos++; }
				else
					{ current_steps.z--; z_pos--; }
			}
		}
              // do the step
		expanderW(MOTOR_WRITE, OLATB, stepperByte);  // step pulse high
                delayMicroseconds(5);
		stepperByte &= 	~(X_STEP_BIT | Y_STEP_BIT | Z_STEP_BIT);
                expanderW(MOTOR_WRITE, OLATB, stepperByte);   // step pulse low
                	
		//wait for next step.
		if (milli_delay > 0)
			delay(milli_delay);			
		else
			delayMicrosecondsInterruptible(micro_delay);
	}
	while (x_can_step || y_can_step || z_can_step);
	
	// we have now finished the move so set our current points to where we were going
//	current_units.x = target_units.x;  // old version
//	current_units.y = target_units.y;  // old version
//	current_units.z = target_units.z;  // old version
//        calculate_deltas();   // old version  -  question - why do this?
        current_units.x = (float) x_pos / X_STEPS_PER_MM;
        current_units.y = (float) y_pos / Y_STEPS_PER_MM;
        current_units.z = (float) z_pos / Z_STEPS_PER_MM;

        disable_steppers();
        displaySteps();
}

void  displaySteps(){
  if(switchState == (byte)0xff){  // only display if we have no limit switcht rigerd
  // display the current position in motor pulses
        lcd.setCursor(0,0);
        lcd.print("X= ");
        lcdDisplayInt(x_pos);
        lcd.print("Y= ");
        lcdDisplayInt(y_pos);
        lcd.print("Z= ");
        lcdDisplayInt(z_pos);
  }       
}

void  lcdDisplayInt(long number){  // display hex number on LCD followed by a space
    long j = 0xf0000000;
    int i = 28;
    byte n;
    for( int k = 0; k<8; k++){
    n = ((number & j) >> i) & 0xf;
    if( n>9 ) n = n + 0x37; else n = n | 0x30;   
    lcd.write(n);
    i-=4;
    j = j >> 4;
    }
   lcd.print(" "); 
}

bool can_step(long current, long target, byte dir)
{

  //stop us if we're at the target position

	if (target == current)
		return false;

  // All OK - we can step
  
	return true;
}

long to_steps(float steps_per_unit, float units)
{
	return steps_per_unit * units;
}

void set_target(float x, float y, float z)
{
	target_units.x = x;
	target_units.y = y;
	target_units.z = z;
	
	calculate_deltas();
}

void set_position(float x, float y, float z)
{
	current_units.x = x;
	current_units.y = y;
	current_units.z = z;
	
	calculate_deltas();
        x_pos = x * X_STEPS_PER_MM;
        y_pos = y * Y_STEPS_PER_MM;
        z_pos = z * Y_STEPS_PER_MM;
        displaySteps();
}

void calculate_deltas()
{
	//figure our deltas.
	delta_units.x = abs(target_units.x - current_units.x);
	delta_units.y = abs(target_units.y - current_units.y);
	delta_units.z = abs(target_units.z - current_units.z);
				
	//set our steps current, target, and delta
	current_steps.x = to_steps(x_units, current_units.x);
	current_steps.y = to_steps(y_units, current_units.y);
	current_steps.z = to_steps(z_units, current_units.z);

	target_steps.x = to_steps(x_units, target_units.x);
	target_steps.y = to_steps(y_units, target_units.y);
	target_steps.z = to_steps(z_units, target_units.z);

	delta_steps.x = abs(target_steps.x - current_steps.x);
	delta_steps.y = abs(target_steps.y - current_steps.y);
	delta_steps.z = abs(target_steps.z - current_steps.z);
	
	//what is our direction
	x_direction = (target_units.x >= current_units.x);
	y_direction = (target_units.y >= current_units.y);
	z_direction = (target_units.z >= current_units.z);

	//set our direction pins as well
     if(x_direction) stepperByte &=~X_DIR_BIT; else stepperByte |= X_DIR_BIT;   // X is mechanically diffrent direction  
     if(y_direction) stepperByte |= Y_DIR_BIT; else stepperByte &=~Y_DIR_BIT;
     if(z_direction) stepperByte |= Z_DIR_BIT; else stepperByte &=~Z_DIR_BIT;
     // write this out to expander
     expanderW(MOTOR_WRITE, OLATB, stepperByte);    
}


long calculate_feedrate_delay(float feedrate)
{
	//how long is our line length?
	float distance = sqrt(delta_units.x*delta_units.x + 
                              delta_units.y*delta_units.y + 
                              delta_units.z*delta_units.z);
	long master_steps = 0;
	
	//find the dominant axis.
	if (delta_steps.x > delta_steps.y)
	{
		if (delta_steps.z > delta_steps.x)
			master_steps = delta_steps.z;
		else
			master_steps = delta_steps.x;
	}
	else
	{
		if (delta_steps.z > delta_steps.y)
			master_steps = delta_steps.z;
		else
			master_steps = delta_steps.y;
	}

	//calculate delay between steps in microseconds.  this is sort of tricky, but not too bad.
	//the formula has been condensed to save space.  here it is in english:
        // (feedrate is in mm/minute)
	// distance / feedrate * 60000000.0 = move duration in microseconds
	// move duration / master_steps = time between steps for master axis.

	return ((distance * 60000000.0) / feedrate) / master_steps;	
}

long getMaxSpeed()
{
	if (delta_steps.z > 0)
		return calculate_feedrate_delay(FAST_Z_FEEDRATE);
	else
		return calculate_feedrate_delay(FAST_XY_FEEDRATE);
}

void enable_steppers()
{
    stepperByte &=  ~(XY_ENABLE_BIT | Z_ENABLE_BIT);
    expanderW(MOTOR_WRITE, OLATB, stepperByte); 
}


void disable_steppers()
{

	//disable our steppers
    stepperByte |=  (XY_ENABLE_BIT | Z_ENABLE_BIT);
    expanderW(MOTOR_WRITE, OLATB, stepperByte); 
}

void delayMicrosecondsInterruptible(unsigned int us)
{


    // for the 16 MHz clock on most Arduino boards

	// for a one-microsecond delay, simply return.  the overhead
	// of the function call yields a delay of approximately 1 1/8 us.
	if (--us == 0)
		return;

	// the following loop takes a quarter of a microsecond (4 cycles)
	// per iteration, so execute it four times for each microsecond of
	// delay requested.
	us <<= 2;

	// account for the time taken in the preceeding commands.
	us -= 2;

	// busy wait
	__asm__ __volatile__ (
		"1: sbiw %0,1" "\n\t" // 2 cycles
		"brne 1b" : "=w" (us) : "0" (us) // 2 cycles
	);
}

#ifdef TEST_MACHINE

void X_motor_test()
{
    Serial.println("Moving X forward by 10 mm at half maximum speed.");
    set_target(10, 0, 0);
    enable_steppers();
    dda_move(calculate_feedrate_delay(FAST_XY_FEEDRATE/2));
    
    Serial.println("Pause for 2 seconds.");
    delay(2000);
    
    Serial.println("Moving X back to the start.");
    set_target(0, 0, 0);
    enable_steppers();
    dda_move(calculate_feedrate_delay(FAST_XY_FEEDRATE/2));
    
    Serial.println("Pause for 2 seconds."); 
    delay(2000);   
}

void Y_motor_test()
{

    Serial.println("Moving Y forward by 10 mm at half maximum speed.");
    set_target(0, 10, 0);
    enable_steppers();
    dda_move(calculate_feedrate_delay(FAST_XY_FEEDRATE/2));
    
    Serial.println("Pause for 2 seconds.");
    delay(2000);
    
    Serial.println("Moving Y back to the start.");
    set_target(0, 0, 0);
    enable_steppers();
    dda_move(calculate_feedrate_delay(FAST_XY_FEEDRATE/2));
    
    Serial.println("Pause for 2 seconds."); 
    delay(2000);     
}

void Z_motor_test()
{
    Serial.println("Moving Z down by 5 mm at half maximum speed.");
    set_target(0, 0, 5);
    enable_steppers();
    dda_move(calculate_feedrate_delay(FAST_Z_FEEDRATE/2));
    
    Serial.println("Pause for 2 seconds.");
    delay(2000);
    
    Serial.println("Moving Z back to the start.");
    set_target(0, 0, 0);
    enable_steppers();
    dda_move(calculate_feedrate_delay(FAST_Z_FEEDRATE/2));
    
    Serial.println("Pause for 2 seconds."); 
    delay(2000);     
}

#endif

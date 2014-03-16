#ifndef PARAMETERS_H
#define PARAMETERS_H

//our command string length
#define COMMAND_SIZE 128

// Uncomment the next line to run stand-alone tests on the machine (also see the 
// ends of this, the process_string, and the stepper_control tabs).
// #define TEST_MACHINE

#define INCHES_TO_MM 25.4

// define the parameters of our machine.
#define X_STEPS_PER_MM   200      // 1.8 deg per step - 1mm thread pitch
#define X_STEPS_PER_INCH (X_STEPS_PER_MM*INCHES_TO_MM)
//#define X_MOTOR_STEPS    400


#define Y_STEPS_PER_MM   200
#define Y_STEPS_PER_INCH (Y_STEPS_PER_MM*INCHES_TO_MM)
// #define Y_MOTOR_STEPS    400


#define Z_STEPS_PER_MM   200
#define Z_STEPS_PER_INCH (Z_STEPS_PER_MM*INCHES_TO_MM)
// #define Z_MOTOR_STEPS    400

//our maximum feedrates
#define FAST_XY_FEEDRATE 300.0
#define FAST_Z_FEEDRATE  100.0
#define DEFAULT_XY_FEEDRATE 150.0
#define DEFAULT_Z_FEEDRATE  50.0

// Units in curve section
#define CURVE_SECTION_MM 0.5
#define CURVE_SECTION_INCHES (CURVE_SECTION_MM*INCHES_TO_MM)

#endif

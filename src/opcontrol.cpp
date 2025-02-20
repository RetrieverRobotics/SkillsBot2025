/**
 * \file opcontrol.cpp
 *
 * Contains user implemented opcontrol. User must use the
 * parameters to the opcontrol function when referencing
 * the master V5 controller or partner V5 controller.
 */

 #include "api.h"
 #include "umbc.h"
 
 #include <cstdint>
 #include <vector>
  
 using namespace pros;
 using namespace umbc;
 using namespace std;
 
 
 #define MOTOR_RED_GEAR_MULTIPLIER       100
 #define MOTOR_GREEN_GEAR_MULTIPLIER     200
 #define MOTOR_BLUE_GEAR_MULTIPLIER      600
 #define MOTOR_REVERSE                   true        //may or may not use idk yet
 
 //ports for left motors
 #define LEFT_FRONT_MOTOR_PORT   1
 #define LEFT_BACK_MOTOR_PORT    2
 
 //ports for right motors
 #define RIGHT_FRONT_MOTOR_PORT  3
 #define RIGHT_BACK_MOTOR_PORT   4
 
 //lift
 #define LIFT_MOTOR_PORT         5       //might add more
 
 //intake
 #define INTAKE_MOTOR_PORT_FRONT 6
 #define INTAKE_MOTOR_PORT_RAMP  7
 
 //goal post thing
 #define GOAL_GRABBER_MOTOR_PORT 8
 
 
 void umbc::Robot::opcontrol() {
 
     // nice names for controllers (do not edit)
     umbc::Controller* controller_master = this->controller_master;
     umbc::Controller* controller_partner = this->controller_partner;
 
     // initialize motors and sensors
 
     std::vector<int8_t> left_motors, right_motors, intake_motors, grabber_motor, lift_motors;
 
 
     //left drive
     left_motors.assign({LEFT_FRONT_MOTOR_PORT, LEFT_BACK_MOTOR_PORT});
 
     MotorGroup drive_left (left_motors);
     drive_left.set_brake_modes(E_MOTOR_BRAKE_COAST);
     drive_left.set_gearing(E_MOTOR_GEAR_BLUE);         //maybe blue
 
     //right drive
     right_motors.assign({RIGHT_FRONT_MOTOR_PORT, RIGHT_BACK_MOTOR_PORT});
     
     MotorGroup drive_right(right_motors); 
     drive_right.set_brake_modes(E_MOTOR_BRAKE_COAST);
     drive_right.set_gearing(E_MOTOR_GEAR_BLUE);         //maybe blue
 
    
     //lift
     lift_motors.assign({LIFT_MOTOR_PORT});
     
     MotorGroup lift (lift_motors);
     lift.set_brake_modes(E_MOTOR_BRAKE_HOLD);
     lift.set_gearing(E_MOTOR_GEAR_BLUE);
 
 
     //intake
     intake_motors.assign({INTAKE_MOTOR_PORT_FRONT, INTAKE_MOTOR_PORT_RAMP});   //this should just be constant

     MotorGroup intake(intake_motors);
     intake.set_gearing(E_MOTOR_GEAR_BLUE);                                     //no brake mode; we ball    (probably just a hard stop if needed)
 
 
     //goal grabber
     grabber_motor.assign({GOAL_GRABBER_MOTOR_PORT});

     MotorGroup goal_grabber (grabber_motor);
     goal_grabber.set_gearing(E_MOTOR_GEAR_GREEN);
     goal_grabber.set_brake_modes(E_MOTOR_BRAKE_HOLD)
     
 
     while(1) {
 
         // implement opcontrols
         
 
         //getting the values from the controller
         int32_t arcade_y = controller_master->get_analog(E_CONTROLLER_ANALOG_LEFT_Y);
         int32_t arcade_x = controller_master->get_analog(E_CONTROLLER_ANALOG_RIGHT_X);
 
         int32_t drive_left_velocity = (int32_t)(((double)(arcade_y + arcade_x) / (double)E_CONTROLLER_ANALOG_MAX)
                                         * MOTOR_BLUE_GEAR_MULTIPLIER); //rpm              ^ to scale power 

         int32_t drive_right_velocity = (int32_t)(((double)(arcade_y - arcade_x) / (double)E_CONTROLLER_ANALOG_MAX)
                                         * MOTOR_BLUE_GEAR_MULTIPLIER);
 
         //setting drive velocity
         drive_left.move_velocity(drive_left_velocity);
         drive_right.move_velocity(drive_right_velocity);
 
 
        
         //probably no partner
 
         //lift
         if (controller_master->get_digital(E_CONTROLLER_DIGITAL_R1)) {          //seems to be held down rather than hard coded??  i want it to be hard coded (new_press)
             lift.move_velocity(MOTOR_RED_GEAR_MULTIPLIER);                      //maybe different color                    
         } else if (controller_master->get_digital(E_CONTROLLER_DIGITAL_L1)){    //will probably change
             lift.move_velocity(-MOTOR_RED_GEAR_MULTIPLIER);
         } else {
             lift.brake();
         }
 
 
 
         //intake
         intake.move_velocity(MOTOR_BLUE_GEAR_MULTIPLIER);
 
 
         //goal grabber
         if (controller_master->get_digital_new_press(E_CONTROLLER_DIGITAL_L1)){     //copied from mechanum
             goal_grabber.move_velocity(MOTOR_RED_GEAR_MULTIPLIER);                  //assuming red gearbox
         }
         else{
             goal_grabber.move_velocity(-MOTOR_RED_GEAR_MULTIPLIER);                 //this thing needs to move quicker i think (yikes)
         }
        
 
 
         // required loop delay (do not edit)
         pros::Task::delay(this->opcontrol_delay_ms);
     }
 }

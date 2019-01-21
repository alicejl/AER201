/**
 * @file
 * @author Alice Liang
 *
 * Created on January 21, 2019
 * 
 * @brief Main Function of Autonomous Robot
 * 
 * Preconditions
 * @pre Co-processor running default program
 * @pre Correct connections and wiring of peripherals and sensors
 */

void main(void){
	while (1){ //while loop that keeps the robot running
		mainMenu(); //alter main funtion of menu.c
		initRun(); //initiate motors, timer, distance sensor
		set return to 0; //return is a variable that states 
		run(); //regular run of robot
	}
}

/**
 * @brief Set up and initiate peripherals, timer, and motors
 */

void initRun(void){
	enable interupts for peripherals
	set up peripherals
	enable interupt for timer
	set timer to 3:50 minutes
	start timer
	start motors
}

/**
 * @brief regular run of robot
 */

void run(void){
	
}
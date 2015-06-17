#pragma config(Sensor, S1,     touchSensor,    sensorEV3_Touch)
#pragma config(Sensor, S2,     gyroSensor,     sensorEV3_Gyro)
#pragma config(Sensor, S3,     colorSensor,    sensorEV3_Color)
#pragma config(Sensor, S4,     sonarSensor,    sensorEV3_Ultrasonic)
#pragma config(Motor,  motorA,          armMotor,      tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor,  motorB,          leftMotor,     tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor,  motorC,          rightMotor,    tmotorEV3_Large, PIDControl, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#define DISTANCE_TO_90 182
#define CM_CORRELATION 21

int distanceB,distanceC;

task main()
{
		moveMotorTarget(motorB,250,20);
		moveMotorTarget(motorC,250,20);
		waitUntilMotorStop(motorB);
		waitUntilMotorStop(motorC);
		distanceB = getMotorEncoder(motorB);
		distanceC = getMotorEncoder(motorC);
		resetMotorEncoder(motorC);
		resetMotorEncoder(motorB);
		moveMotorTarget(motorC,200,20);
		moveMotorTarget(motorB,200,20);
		waitUntilMotorStop(motorB);
		waitUntilMotorStop(motorC);
		distanceB = getMotorEncoder(motorB);
		distanceC = getMotorEncoder(motorC);
	return;
}
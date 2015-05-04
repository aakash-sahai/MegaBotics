#include "FreeSixIMU.h"
#include "FIMU_ADXL345.h"
#include "FIMU_ITG3200.h"

#include <Wire.h>

float angles[3]; // yaw pitch roll

// Set the FreeSixIMU object
FreeSixIMU sixDOF = FreeSixIMU();

// OUTPUT OPTIONS
/*****************************************************************/
// Set your serial port baud rate used to send out data here!
#define OUTPUT__BAUD_RATE 57600

// Sensor data output interval in milliseconds
// This may not work, if faster than 20ms (=50Hz)
// Code is tuned for 20ms, so better leave it like that
#define OUTPUT__DATA_INTERVAL 20  // in milliseconds

// Output mode definitions (do not change)
#define OUTPUT__MODE_CALIBRATE_SENSORS 0 // Outputs sensor min/max values as text for manual calibration
#define OUTPUT__MODE_ANGLES 1 // Outputs yaw/pitch/roll in degrees
#define OUTPUT__MODE_SENSORS_CALIB 2 // Outputs calibrated sensor values for all 9 axes
#define OUTPUT__MODE_SENSORS_RAW 3 // Outputs raw (uncalibrated) sensor values for all 9 axes
#define OUTPUT__MODE_SENSORS_BOTH 4 // Outputs calibrated AND raw sensor values for all 9 axes
// Output format definitions (do not change)
#define OUTPUT__FORMAT_TEXT 0 // Outputs data as text
#define OUTPUT__FORMAT_BINARY 1 // Outputs data as binary float

// Select your startup output mode and format here!
int output_mode = OUTPUT__MODE_ANGLES;
int output_format = OUTPUT__FORMAT_TEXT;

// Select if serial continuous streaming output is enabled per default on startup.
#define OUTPUT__STARTUP_STREAM_ON true  // true or false

// If set true, an error message will be output if we fail to read sensor data.
// Message format: "!ERR: reading <sensor>", followed by "\r\n".
boolean output_errors = false;  // true or false

// Euler angles
float yaw;
float pitch;
float roll;

// DCM timing in the main loop
unsigned long timestamp;
unsigned long timestamp_old;
float G_Dt; // Integration time for DCM algorithm

// More output-state variables
boolean output_stream_on;
boolean output_single_on;
int curr_calibration_sensor = 0;
boolean reset_calibration_session_flag = true;
int num_accel_errors = 0;
int num_magn_errors = 0;
int num_gyro_errors = 0;

void turn_output_stream_on() {
	output_stream_on = true;
}

void turn_output_stream_off() {
	output_stream_on = false;
}

// Blocks until another byte is available on serial port
char readChar() {
	while (Serial.available() < 1) {
	} // Block
	return Serial.read();
}

void read_sensors() {
	sixDOF.getEuler(angles);
}

void setup() {
	Serial.begin(57600);
	Wire.begin();

	delay(5);
	sixDOF.init(); //begin the IMU
	delay(5);
}

void loop() {
// Read incoming control messages
	if (Serial.available() >= 2) {
		if (Serial.read() == '#') // Start of new control message
				{
			int command = Serial.read(); // Commands
			if (command == 'f') // request one output _f_rame
				output_single_on = true;
			else if (command == 's') // _s_ynch request
					{
				// Read ID
				byte id[2];
				id[0] = readChar();
				id[1] = readChar();

				// Reply with synch message
				Serial.print("#SYNCH");
				Serial.write(id, 2);
				Serial.println();
			} else if (command == 'o') // Set _o_utput mode
					{
				char output_param = readChar();
				if (output_param == 'n')  // Calibrate _n_ext sensor
						{
					curr_calibration_sensor = (curr_calibration_sensor + 1) % 3;
					reset_calibration_session_flag = true;
				} else if (output_param == 't') // Output angles as _t_ext
						{
					output_mode = OUTPUT__MODE_ANGLES;
					output_format = OUTPUT__FORMAT_TEXT;
				} else if (output_param == 'b') // Output angles in _b_inary format
						{
					output_mode = OUTPUT__MODE_ANGLES;
					output_format = OUTPUT__FORMAT_BINARY;
				} else if (output_param == 'c') // Go to _c_alibration mode
						{
					output_mode = OUTPUT__MODE_CALIBRATE_SENSORS;
					reset_calibration_session_flag = true;
				} else if (output_param == 's') // Output _s_ensor values
						{
					char values_param = readChar();
					char format_param = readChar();
					if (values_param == 'r')  // Output _r_aw sensor values
						output_mode = OUTPUT__MODE_SENSORS_RAW;
					else if (values_param == 'c') // Output _c_alibrated sensor values
						output_mode = OUTPUT__MODE_SENSORS_CALIB;
					else if (values_param == 'b') // Output _b_oth sensor values (raw and calibrated)
						output_mode = OUTPUT__MODE_SENSORS_BOTH;

					if (format_param == 't')  // Output values as _t_text
						output_format = OUTPUT__FORMAT_TEXT;
					else if (format_param == 'b') // Output values in _b_inary format
						output_format = OUTPUT__FORMAT_BINARY;
				} else if (output_param == '0') // Disable continuous streaming output
						{
					turn_output_stream_off();
					reset_calibration_session_flag = true;
				} else if (output_param == '1') // Enable continuous streaming output
						{
					reset_calibration_session_flag = true;
					turn_output_stream_on();
				} else if (output_param == 'e') // _e_rror output settings
						{
					char error_param = readChar();
					if (error_param == '0')
						output_errors = false;
					else if (error_param == '1')
						output_errors = true;
					else if (error_param == 'c') // get error count
							{
						Serial.print("#AMG-ERR:");
						Serial.print(num_accel_errors);
						Serial.print(",");
						Serial.print(num_magn_errors);
						Serial.print(",");
						Serial.println(num_gyro_errors);
					}
				}
			}
#if OUTPUT__HAS_RN_BLUETOOTH == true
			// Read messages from bluetooth module
			// For this to work, the connect/disconnect message prefix of the module has to be set to "#".
			else if (command == 'C')// Bluetooth "#CONNECT" message (does the same as "#o1")
			turn_output_stream_on();
			else if (command == 'D')// Bluetooth "#DISCONNECT" message (does the same as "#o0")
			turn_output_stream_off();
#endif // OUTPUT__HAS_RN_BLUETOOTH == true
		} else {
		} // Skip character
	}

// Time to read the sensors again?
	if ((millis() - timestamp) >= OUTPUT__DATA_INTERVAL) {
		timestamp_old = timestamp;
		timestamp = millis();
		if (timestamp > timestamp_old)
			G_Dt = (float) (timestamp - timestamp_old) / 1000.0f; // Real time of loop run. We use this on the DCM algorithm (gyro integration time)
		else
			G_Dt = 0;

		// Update sensor readings
		read_sensors();

		if (output_mode == OUTPUT__MODE_ANGLES)  // Output angles
		{
			if (output_stream_on || output_single_on)
				output_angles();
		} else  // Output sensor values
		{
			if (output_stream_on || output_single_on) {
				//output_sensors();
			}
		}

		output_single_on = false;

#if DEBUG__PRINT_LOOP_TIME == true
		Serial.print("loop time (ms) = ");
		Serial.println(millis() - timestamp);
#endif
	}
#if DEBUG__PRINT_LOOP_TIME == true
	else
	{
		Serial.println("waiting...");
	}
#endif
}

void output_angles() {
	float ypr[3];
	ypr[0] = angles[0];
	ypr[1] = angles[1];
	ypr[2] = angles[2];
	Serial.write((byte*) ypr, 12);  // No new-line

	/*
	  Serial.print(angles[0]);
	  Serial.print(" | ");
	  Serial.print(angles[1]);
	  Serial.print(" | ");
	  Serial.println(angles[2]);
*/
}

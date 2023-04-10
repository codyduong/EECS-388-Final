# EECS-388-Final

This is the version control for Cody, Spencer, Caden, Rahul, and Ellia's EECS 388 Final Project

# Functionality
  
  -  Use a new board (i.e., PCA9695) for driving servoand DC motors
  -  PCA9695 uses I2C to receive commands from Hi-Five board to generate PWM signal for the servo- and DC-motors

# Prototype Components 
  
  1. Hi-five board
  2. Pi board
  3. Motor driver (PCA9695) 4- LIDAR
  4. DC motor
  5. Servo motor
  6. Electronic Speed Controller (ESC

# Milestones
  
  - (Milestone 1)
    - Use the Hi-five board to send I2C commands to PCA9695 to drive the servo motor (for steering) and DC motors (for moving forward and backwards)
  - (Milestone 2)   
    - Connect Pi to Hi-five board using UART to set up a connection between the two boards for sending steering commands from the Pi to the Hi-five board
  -  (Milestone 3)
    - Control the car using a sequence of commands sending the steering commands from the Pi to the Hi-five board (using UART) and then to the motors (using PWM I2C controller) 

# Final Goal 
  
  - Implement an RC car’s components by incorporating elements from the previous labs

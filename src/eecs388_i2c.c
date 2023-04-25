#include <stdio.h>
#include <stdint.h>
#include "eecs388_lib.h"
#include "metal/i2c.h"


struct metal_i2c *i2c;
uint8_t bufWrite[9];
uint8_t bufRead[1];


//The entire setup sequence
void set_up_I2C(){
    uint8_t oldMode;
    uint8_t newMode;
    _Bool success;


    bufWrite[0] = PCA9685_MODE1;
    bufWrite[1] = MODE1_RESTART;
    printf("%d\n",bufWrite[0]);
    
    i2c = metal_i2c_get_device(0);

    if(i2c == NULL){
        printf("Connection Unsuccessful\n");
    }
    else{
        printf("Connection Successful\n");
    }
    
    //Setup Sequence
    metal_i2c_init(i2c,I2C_BAUDRATE,METAL_I2C_MASTER);
    success = metal_i2c_write(i2c,PCA9685_I2C_ADDRESS,2,bufWrite,METAL_I2C_STOP_DISABLE);//reset
    delay(100);
    printf("resetting PCA9685 control 1\n");

    //Initial Read of control 1
    bufWrite[0] = PCA9685_MODE1;//Address
    success = metal_i2c_transfer(i2c,PCA9685_I2C_ADDRESS,bufWrite,1,bufRead,1);//initial read
    printf("Read success: %d and control value is: %d\n", success, bufWrite[0]);
    
    //Configuring Control 1
    oldMode = bufRead[0];
    newMode = (oldMode & ~MODE1_RESTART) | MODE1_SLEEP;
    printf("sleep setting is %d\n", newMode);
    bufWrite[0] = PCA9685_MODE1;//address
    bufWrite[1] = newMode;//writing to register
    success = metal_i2c_write(i2c,PCA9685_I2C_ADDRESS,2,bufWrite,METAL_I2C_STOP_DISABLE);//sleep
    bufWrite[0] = PCA9685_PRESCALE;//Setting PWM prescale
    bufWrite[1] = 0x79;
    success = metal_i2c_write(i2c,PCA9685_I2C_ADDRESS,2,bufWrite,METAL_I2C_STOP_DISABLE);//sets prescale
    bufWrite[0] = PCA9685_MODE1;
    bufWrite[1] = 0x01 | MODE1_AI | MODE1_RESTART;
    printf("on setting is %d\n", bufWrite[1]);
    success = metal_i2c_write(i2c,PCA9685_I2C_ADDRESS,2,bufWrite,METAL_I2C_STOP_DISABLE);//awake
    delay(100);
    printf("Setting the control register\n");
    bufWrite[0] = PCA9685_MODE1;
    success = metal_i2c_transfer(i2c,PCA9685_I2C_ADDRESS,bufWrite,1,bufRead,1);//initial read
    printf("Set register is %d\n",bufRead[0]);

} 


void breakup(int bigNum, uint8_t* low, uint8_t* high){
    *low = (uint8_t)bigNum;
    *high = (uint8_t)(bigNum >> 8);
}

void steering(int angle){
    uint8_t lowOffset = 3;
    uint8_t highOffset = 4;
    uint8_t code;
    int angleCycle = getServoCycle(angle);
    bufWrite[0] = PCA9685_LED0_ON_L + 0x04;
    bufWrite[1] = 0x00;
    bufWrite[2] = 0x00;
    printf("Steering %d %d\n", bufWrite[highOffset], bufWrite[lowOffset]);
    breakup(angleCycle, &bufWrite[lowOffset], &bufWrite[highOffset]);
    code = metal_i2c_transfer(i2c,PCA9685_I2C_ADDRESS,bufWrite,5,bufRead,1);
    printf("Steering transfer code %d\n", code);
}

void stopMotor(){
    uint8_t lowOffset = 3;
    uint8_t highOffset = 4;
    uint8_t code;
    bufWrite[0] = PCA9685_LED0_ON_L;
    bufWrite[1] = 0x00;
    bufWrite[2] = 0x00;
    breakup(280, &bufWrite[lowOffset], &bufWrite[highOffset]);
    printf("StopMotor %d %d\n", bufWrite[highOffset], bufWrite[lowOffset]);
    code = metal_i2c_transfer(i2c,PCA9685_I2C_ADDRESS,bufWrite,5,bufRead,1);
    printf("Stopmotor transfer code %d\n", code);
}

void driveForward(uint8_t speedFlag){
    uint8_t lowOffset = 3;
    uint8_t highOffset = 4;
    uint8_t code;
    bufWrite[0] = PCA9685_LED0_ON_L;
    bufWrite[1] = 0x00;
    bufWrite[2] = 0x00;
    switch (speedFlag) {
        case 1:
            breakup(313, &bufWrite[lowOffset], &bufWrite[highOffset]);
            break;
        case 2:
            breakup(315, &bufWrite[lowOffset], &bufWrite[highOffset]);
            break;
        case 3:
            breakup(317, &bufWrite[lowOffset], &bufWrite[highOffset]);
            break;    
        default:
            // If invalid, just stop the motor
            breakup(280, &bufWrite[lowOffset], &bufWrite[highOffset]);
    }
    printf("ForwardMotor %d %d\n", bufWrite[highOffset], bufWrite[lowOffset]);
    code = metal_i2c_transfer(i2c,PCA9685_I2C_ADDRESS,bufWrite,5,bufRead,1);
    printf("ForwardMotor transfer code %d\n", code);
}

void driveReverse(uint8_t speedFlag){
    uint8_t lowOffset = 3;
    uint8_t highOffset = 4;
    uint8_t code;
    bufWrite[0] = PCA9685_LED0_ON_L;
    bufWrite[1] = 0x00;
    bufWrite[2] = 0x00;
    switch (speedFlag) {
        case 1:
            breakup(267, &bufWrite[lowOffset], &bufWrite[highOffset]);
            break;
        case 2:
            breakup(265, &bufWrite[lowOffset], &bufWrite[highOffset]);
            break;
        case 3:
            breakup(263, &bufWrite[lowOffset], &bufWrite[highOffset]);
            break;    
        default:
            // If invalid, just stop the motor
            breakup(280, &bufWrite[lowOffset], &bufWrite[highOffset]);
    }
    printf("ReverseMotor %d %d\n", bufWrite[highOffset], bufWrite[lowOffset]);
    code = metal_i2c_transfer(i2c,PCA9685_I2C_ADDRESS,bufWrite,5,bufRead,1);
    printf("ReverseMotor transfer code %d\n", code);
}

int main()
{
    set_up_I2C();
    
    //Defining the breakup function
    /*
        Task 1: breaking 12 bit into two 8-bit
        Define the function created that recieves a 12 bit number,
        0 to 4096 and breaks it up into two 8 bit numbers.

        Assign these values to a referenced value handed into
        the function. 

        ex: 
        uint8_t variable1;
        uint8_t variable2;
        breakup(2000,&variable1,&variable2);
        variable1 -> low 8 bits of 2000
        variable2 -> high 8 bits of 2000


    */   
    // uint8_t variable1;
    // uint8_t variable2;
    // breakup(2000,&variable1,&variable2);
    // printf("Task 1 variable1: %d variable2: %d\n", variable1, variable2);    
    //Changing Steering Heading
    /*
        Task 2: using getServoCycle(), bufWrite, bufRead, 
        breakup(), and and metal_i2c_transfer(), implement 
        the function defined above to control the servo
        by sending it an angle ranging from -45 to 45.

        Use the getServoCycle function to get the value to 
        breakup.

        ex: 
        int valToBreak = getServoCycle(45);
        >>>sets valToBreak to 355
        
        note: the motor's speed controller is either 
        LED0 or LED1 depending on where its plugged into 
        the board. If its ~~~~~~~~~    LED1, simply add 4 to the LED0
        address

        ex: steering(0); -> driving angle forward
    */
    // printf("Task 2");
    // steering(0);
    // delay(1000);
    

    //Motor config/stop. This will cause a second beep upon completion
    /*
        -Task 3: using bufWrite, bufRead, breakup(), and
        and metal_i2c_transfer(), implement the funcion made
        above. This function Configure the motor by 
        writing a value of 280 to the motor.

        -include a 2 second delay after calling this function
        in order to calibrate

        -Note: the motor's speed controller is either 
        LED0 or LED1 depending on where its plugged into 
        the board. If its LED1, simply add 4 to the LED0
        address

        ex: stopMotor();
    */
    // printf("Task 3");
    // stopMotor();
    // delay(2000);


    /*
    ############################################################
        ATTENTION: The following section will cause the        
        wheels to move. Confirm that the robot is              
        Propped up to avoid it driving away, as well as         
        that nothing is touching the wheels and can get 
        caught in them

        If anything goes wrong, unplug the hifive board from
        the computer to stop the motors from moving 
        
        Avoid sticking your hand inside the 
        car while its wheels are spinning
    #############################################################
    */
    

    //Motor Forward
    /*
        -Task 4: using bufWrite, bufRead, breakup(), and
        and metal_i2c_transfer(), implement the function
        made above to Drive the motor forward. The given
        speedFlag will alter the motor speed as follows:
        
        speedFlag = 1 -> value to breakup = 313 
        speedFlag = 2 -> value to breakup = 315(Optional)
        speedFlag = 3 -> value to breakup = 317(Optional)

        -note 1: the motor's speed controller is either 
        LED0 or LED1 depending on where its plugged into 
        the board. If its LED1, simply add 4 to the LED0
        address or type and replace LED1 with LED0

        ex: driveForward(1);
    */
    // printf("Task 4");
    // driveForward(1);
    // delay(1000);
    // driveForward(2);
    // delay(1000);
    // driveForward(3);
    // delay(1000);
    // stopMotor();
    
    //Motor Reverse
    /*
        -Task 5: using bufWrite, bufRead, breakup(), and
        and metal_i2c_transfer(), implement the function
        made above to Drive the motor backward. The given
        speedFlag will alter the motor speed as follows:
        
        speedFlag = 1 -> value to breakup = 267 
        speedFlag = 2 -> value to breakup = 265(Optional)
        speedFlag = 3 -> value to breakup = 263(Optional)

        -note 1: the motor's speed controller is either 
        LED0 or LED1 depending on where its plugged into 
        the board. If its LED1, simply add 4 to the LED0
        address or type and replace LED1 with LED0

        ex: driveReverse(1);
    */

    // printf("Task 5");
    // driveReverse(1);
    // delay(1000);
    // driveReverse(2);
    // delay(1000);
    // driveReverse(3);
    // delay(1000);
    // stopMotor();
    
    //Fully Controlling the PCA9685
    /*
        -Task 6: using previously defined functions, 
        perform the following sequence of actions
        
        -Configure the motors (wait for 2 seconds)
        -Set the steering heading to 0 degrees 
        -Drive forward (wait for 2 seconds)
        -Change the steering heading to 20 degrees (wait for 2 seconds)
        -Stop the motor (wait for 2 seconds)
        -Drive forward (wait for 2 seconds)
        -Set steering heading to 0 degrees (wait for 2 seconds)
        -Stop the motor
    */

    printf("Task 6");

    // // Configure the motors (wait for 2 seconds)
    stopMotor();
    delay(2000);

    // // Set the steering heading to 0 degrees
    steering(0);

    // // Drive forward (wait for 2 seconds)
    driveForward(1);
    delay(2000);

    // // Change the steering heading to 20 degrees (wait for 2 seconds)
    steering(20);
    delay(2000);

    // // Stop the motor (wait for 2 seconds)
    stopMotor(1);
    delay(2000);

    // // Drive forward (wait for 2 seconds)
    driveReverse(1);
    delay(2000);

    // // Set steering heading to 0 degrees (wait for 2 seconds)
    steering(0);
    delay(2000);

    // // Stop the motor
    stopMotor(1);
}

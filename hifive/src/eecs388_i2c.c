#include <stdio.h>
#include <stdint.h>
#include "eecs388_lib.h"
#include "metal/i2c.h"

struct metal_i2c *i2c;
uint8_t bufWrite[9];
uint8_t bufRead[1];

// The entire setup sequence
void set_up_I2C()
{
    uint8_t oldMode;
    uint8_t newMode;
    _Bool success;

    bufWrite[0] = PCA9685_MODE1;
    bufWrite[1] = MODE1_RESTART;
    printf("%d\n", bufWrite[0]);

    i2c = metal_i2c_get_device(0);

    if (i2c == NULL)
    {
        printf("Connection Unsuccessful\n");
    }
    else
    {
        printf("Connection Successful\n");
    }

    // Setup Sequence
    metal_i2c_init(i2c, I2C_BAUDRATE, METAL_I2C_MASTER);
    success = metal_i2c_write(i2c, PCA9685_I2C_ADDRESS, 2, bufWrite, METAL_I2C_STOP_DISABLE); // reset
    delay(100);
    printf("resetting PCA9685 control 1\n");

    // Initial Read of control 1
    bufWrite[0] = PCA9685_MODE1;                                                     // Address
    success = metal_i2c_transfer(i2c, PCA9685_I2C_ADDRESS, bufWrite, 1, bufRead, 1); // initial read
    printf("Read success: %d and control value is: %d\n", success, bufWrite[0]);

    // Configuring Control 1
    oldMode = bufRead[0];
    newMode = (oldMode & ~MODE1_RESTART) | MODE1_SLEEP;
    printf("sleep setting is %d\n", newMode);
    bufWrite[0] = PCA9685_MODE1;                                                              // address
    bufWrite[1] = newMode;                                                                    // writing to register
    success = metal_i2c_write(i2c, PCA9685_I2C_ADDRESS, 2, bufWrite, METAL_I2C_STOP_DISABLE); // sleep
    bufWrite[0] = PCA9685_PRESCALE;                                                           // Setting PWM prescale
    bufWrite[1] = 0x79;
    success = metal_i2c_write(i2c, PCA9685_I2C_ADDRESS, 2, bufWrite, METAL_I2C_STOP_DISABLE); // sets prescale
    bufWrite[0] = PCA9685_MODE1;
    bufWrite[1] = 0x01 | MODE1_AI | MODE1_RESTART;
    printf("on setting is %d\n", bufWrite[1]);
    success = metal_i2c_write(i2c, PCA9685_I2C_ADDRESS, 2, bufWrite, METAL_I2C_STOP_DISABLE); // awake
    delay(100);
    printf("Setting the control register\n");
    bufWrite[0] = PCA9685_MODE1;
    success = metal_i2c_transfer(i2c, PCA9685_I2C_ADDRESS, bufWrite, 1, bufRead, 1); // initial read
    printf("Set register is %d\n", bufRead[0]);
}

void breakup(int bigNum, uint8_t *low, uint8_t *high)
{
    *low = (uint8_t)bigNum;
    *high = (uint8_t)(bigNum >> 8);
}

void steering(int angle)
{
    uint8_t lowOffset = 3;
    uint8_t highOffset = 4;
    uint8_t code;
    int angleCycle = getServoCycle(angle);
    bufWrite[0] = PCA9685_LED0_ON_L + 0x04;
    bufWrite[1] = 0x00;
    bufWrite[2] = 0x00;
    printf("Steering %d %d\n", bufWrite[highOffset], bufWrite[lowOffset]);
    breakup(angleCycle, &bufWrite[lowOffset], &bufWrite[highOffset]);
    // code = metal_i2c_transfer(i2c, PCA9685_I2C_ADDRESS, bufWrite, 5, bufRead, 1);
    printf("Steering transfer code %d\n", code);
}

void stopMotor()
{
    uint8_t lowOffset = 3;
    uint8_t highOffset = 4;
    uint8_t code;
    bufWrite[0] = PCA9685_LED0_ON_L;
    bufWrite[1] = 0x00;
    bufWrite[2] = 0x00;
    breakup(280, &bufWrite[lowOffset], &bufWrite[highOffset]);
    printf("StopMotor %d %d\n", bufWrite[highOffset], bufWrite[lowOffset]);
    // code = metal_i2c_transfer(i2c, PCA9685_I2C_ADDRESS, bufWrite, 5, bufRead, 1);
    printf("Stopmotor transfer code %d\n", code);
}

void driveForward(uint8_t speedFlag)
{
    uint8_t lowOffset = 3;
    uint8_t highOffset = 4;
    uint8_t code;
    bufWrite[0] = PCA9685_LED0_ON_L;
    bufWrite[1] = 0x00;
    bufWrite[2] = 0x00;
    switch (speedFlag)
    {
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
    // code = metal_i2c_transfer(i2c, PCA9685_I2C_ADDRESS, bufWrite, 5, bufRead, 1);
    printf("ForwardMotor transfer code %d\n", code);
}

void driveReverse(uint8_t speedFlag)
{
    uint8_t lowOffset = 3;
    uint8_t highOffset = 4;
    uint8_t code;
    bufWrite[0] = PCA9685_LED0_ON_L;
    bufWrite[1] = 0x00;
    bufWrite[2] = 0x00;
    switch (speedFlag)
    {
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
    // code = metal_i2c_transfer(i2c, PCA9685_I2C_ADDRESS, bufWrite, 5, bufRead, 1);
    printf("ReverseMotor transfer code %d\n", code);
}

int parseCommand(char *str)
{
    // Run through the string array

    int parsingType = 0;
    char commandType = '\0';
    char commandValueStr[2] = {'\0'};
    int commandValueStrInt = 0;
    int commandValue = 0;
    int i;
    for (i = 0;; i++)
    {
        if (str[i] == '\0')
        {
            break;
        }
        // If we have terminated the command, go ahead and run it
        if (str[i] == ';')
        {
            // parse commandValueStr
            commandValue = sscanf(commandValueStr, "%d", &commandValue);
            switch (commandType)
            {
            case 'a':
                // angle
                steering(commandValue);
                break;
            case 's':
                // speed
                if (commandValue > 0)
                {
                    driveForward(commandValue);
                }
                else if (commandValue < 0)
                {
                    driveReverse(commandValue);
                }
                else
                {
                    stopMotor();
                }
                break;
            case 'd':
                delay(commandValue * 1000);
                break;
            default:
                printf("Failed to parse command! %c", commandType);
            }

            parsingType = 0;
            commandType = '\0';
            commandValueStr[0] = '\0';
            commandValueStrInt = 0;
        }
        else if (str[i] == ':')
        {
            parsingType = 1;
        }
        // Getting commandValue
        else if (parsingType == 1)
        {
            commandValueStr[commandValueStrInt] = str[i];
            commandValueStrInt += 1;
        }
        // Getting commandType
        else if (parsingType == 0)
        {
            commandType = str[i];
        }
        else
        {
            printf('We ended up in an invalid state! What happened?');
        }
    }
}

int main()
{
    set_up_I2C();

    stopMotor();
    delay(2000);

    char charbytes[2] = {'\0'};
    char buffer[100] = {'\0'};
    int bufferInt = 0;
    // initialize UART channels
    ser_setup(0); // uart0 (debug)
    ser_setup(1); // uart1 (raspberry pi)
    printf("Serial connection completed.\n");
    printf("Begin the main loop.\n");
    while (1)
    {
        if (ser_isready(1))
        {
            ser_readline(1, 1, charbytes);
            ser_printline(0, charbytes);
            // Add to buffer
            buffer[bufferInt] = charbytes[0];
            bufferInt += 1;
            
            // Everytime we encounter a ';', parse the command so far, then clear out the buffer
            if (charbytes[0] == ';') {
                parseCommand(buffer);
                buffer[0] = '\0';
                bufferInt = 0;
            }
            charbytes[0] = '\0';
        }
    }
    return 0;
}

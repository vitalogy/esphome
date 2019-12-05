#pragma once

/*
Register

Sensor and FIFO timing is controlled with a set of registers which is implemented in the
ArduChip. User can send capture commands and read image data with a simple SPI slave
interface. The detail description of registers’ bits can be found in the software section
in this document. As mentioned earlier the first bit[7] of the command phase is
read/write byte, ‘0’ is for read and ‘1’ is for write, and the bit[6:0] is the address
to be read or write in the data phase. So user has to combine the 8 bits address
according to the read or write commands they want to issue.

Register Address          Register Type           Description
  bit[6:0]
    0x00                       RW                 Test Register
    0x01                       RW                 Capture Control Register
                                                    Bit[2:0]: Number of frames to be captured
    0x02                       RW                 Reserved
    0x03                       RW                 Sensor Interface Timing Register
                                                    Bit[0]: Sensor Hsync Polarity,
                                                      0 = active high,
                                                      1 = active low
                                                    Bit[1]: Sensor Vsync Polarity
                                                      0 = active high,
                                                      1 = active low
                                                    Bit[3]: Sensor data delay
                                                      0 = no delay,
                                                      1= delay 1 PCLK
                                                    Bit[4]: FIFO mode control
                                                      0 = FIFO mode disable,
                                                      1 = enable FIFO mode
                                                    Bit[6]: low power mode control
                                                      0 = normal mode,
                                                      1 = low power mode
    0x04                        RW                FIFO control Register
                                                    Bit[0]: write ‘1’ to clear FIFO write done flag
                                                    Bit[1]: write ‘1’ to start capture
                                                    Bit[4]: write ‘1’ to reset FIFO write pointer
                                                    Bit[5]: write ‘1’ to reset FIFO read pointer
    0x05                        RW                GPIO Direction Register
                                                    Bit[0]: Sensor reset IO direction
                                                    Bit[1]: Sensor power down IO direction
                                                    Bit[2]: Sensor power enable IO direction
                                                      0 = input,
                                                      1 = output
    0x06                        RW                GPIO Write Register
                                                    Bit[0]: Sensor reset IO value
                                                    Bit[1]: Sensor power down IO value
                                                    Bit[1]: Sensor power enable IO value
    0x3B                        RO                Reserved
    0x3C                        RO                Burst FIFO read operation
    0x3D                        RO                Single FIFO read operation
    0x3E                        RO                Reserved
    0x3F                        RO                Reserved
    0x40                        RO                ArduChip version, constant value 0x40 for 2MP model
                                                    Bit[7:4]: integer part of the revision number
                                                    Bit[3:0]: decimal part of the revision number
    0x41                        RO                Bit[0]: camera vsync pin status
                                                  Bit[3]: camera write FIFO done flag
    0x42                        RO                Camera write FIFO size[7:0]
    0x43                        RO                Camera write FIFO size[15:8]
    0x44                        RO                Camera write FIFO size[18:16]
    0x45                        RO                GPIO Read Register
                                                    Bit[0]: Sensor reset IO value
                                                    Bit[1]: Sensor power down IO value
                                                    Bit[1]: Sensor power enable IO value
*/
namespace esphome {
namespace arducam {

#define ARDUCHIP_WRITE_OPERATION 0x80
#define ARDUCHIP_READ_OPERATION  0x7F

#define ARDUCHIP_TEST_REGISTER 0x00



#define ARDUCHIP_FIFO_CONTROL 0x04  //FIFO and I2C control
#define   FIFO_CLEAR_MASK    	0x01
#define   FIFO_START_MASK    	0x02

#define ARDUCHIP_REV       		0x40  //ArduCHIP revision
#define ARDUCHIP_TRIG      		0x41  //Trigger source
#define   VSYNC_MASK         	0x01
#define   SHUTTER_MASK       	0x02
#define   CAPTURE_DONE_MASK   0x08

#define BURST_FIFO_READ			  0x3C  //Burst FIFO read operation
#define SINGLE_FIFO_READ		  0x3D  //Single FIFO read operation

#define FIFO_SIZE1				    0x42  //Camera write FIFO size[7:0] for burst to read
#define FIFO_SIZE2				    0x43  //Camera write FIFO size[15:8]
#define FIFO_SIZE3				    0x44  //Camera write FIFO size[18:16]

}  // namespace arducam
}  // namespace esphome


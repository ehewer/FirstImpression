import time
from pySerialTransfer import pySerialTransfer as txfer


if __name__ == '__main__':
    try:
        link = txfer.SerialTransfer('COM3')
        
        link.open()
        time.sleep(2) # allow some time for the Arduino to completely reset
        
        while True:
            sendSize = 0
            
            ###################################################################
            # Send a string
            ###################################################################
            print("Ready to enroll a fingerprint")
            userInput = input("Enter the ID # (from 1 to 127) of the enrollment: ")
            
            command = (int(userInput) % 127) + 1
            commandSize = link.tx_obj(command, sendSize) - sendSize
            sendSize += commandSize
            
            ###################################################################
            # Transmit all the data to send in a single packet
            ###################################################################
            link.send(sendSize)
            
            ###################################################################
            # Wait for a response and report any errors while receiving packets
            ###################################################################
            while not link.available():
                if link.status < 0:
                    if link.status == txfer.CRC_ERROR:
                        print('ERROR: CRC_ERROR')
                    elif link.status == txfer.PAYLOAD_ERROR:
                        print('ERROR: PAYLOAD_ERROR')
                    elif link.status == txfer.STOP_BYTE_ERROR:
                        print('ERROR: STOP_BYTE_ERROR')
                    else:
                        print('ERROR: {}'.format(link.status))
            
            ###################################################################
            # Parse response string
            ###################################################################
            echo = link.rx_obj(obj_type=type(command),
                                     obj_byte_size=4,
                                     start_pos=0)
            
            ###################################################################
            # Display the received data
            ###################################################################
            print('SENT: {}'.format(command))
            print('RCVD: {}'.format(echo))
            print(' ')
    
    except KeyboardInterrupt:
        try:
            link.close()
        except:
            pass
    
    except:
        import traceback
        traceback.print_exc()
        
        try:
            link.close()
        except:
            pass
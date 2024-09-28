import pygame 
import socket

# Initialize Pygame and the controller
pygame.init()  # Initialize all imported Pygame modules
pygame.joystick.init()  # Initialize joystick module
controller = pygame.joystick.Joystick(0)  # Assuming the first connected joystick
controller.init()  # Initialize the joystick

# Global variables to keep track of the current packet type and last output packet sent
current_packet = 'D'  # Starts with the Drive packet as specified in the task
last_output = 'D_128_128_128_128_128_128'  # Initial state of the drive packet
rest_values_d = 'D_128_128_128_128_128_128'  # Rest values for drive packet
rest_values_a = 'A_128_128_128_128_128_128'  # Rest values for arm packet

# Set up a UDP socket client to send control packets as required by the task
socket_client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Configuration settings
DEADZONE = 0.1  # Small threshold to prevent minor stick movements from triggering actions
HOST = 'localhost'  # Replace with your server address if necessary
PORT = 12345        # Replace with your server port if necessary
REST_VALUE = 128    # PWM value representing a neutral/rest state for all controls

def send_packet(packet):
    """
    Sends the control packet to the server and prints it for debugging.
    
    Args:
        packet (str): The formatted string packet to be sent.
    """
    print(packet)  # Print the packet as required in the task for debugging and verification
    socket_client.sendto(packet.encode(), (HOST, PORT))  # Send the packet over UDP as required by the task

def map_value(value, min_input, max_input, min_output, max_output):
    """
    Maps a value from one range to another, useful for scaling joystick inputs to PWM values.
    
    Args:
        value (float): The input value to be mapped.
        min_input (float): The minimum value of the input range.
        max_input (float): The maximum value of the input range.
        min_output (int): The minimum value of the output range.
        max_output (int): The maximum value of the output range.
    
    Returns:
        int: The mapped output value.
    """
    return int(min_output + (value - min_input) * (max_output - min_output) / (max_input - min_input))

def get_corrected_analog_value(axis_value):
    """
    Converts joystick input values into corresponding PWM values, handling inversion and deadzone.
    
    Args:
        axis_value (float): The raw input value from the joystick axis (-1 to 1).
    
    Returns:
        int: The corrected PWM value based on the input direction.
    """
    if abs(axis_value) < DEADZONE:
        return 128  # Return to neutral (128) when within the deadzone to avoid jitter
    elif axis_value < 0:  # If the stick is pushed up, increase PWM
        return map_value(axis_value, -1, 0, 255, 128)  # Map upward movement to higher PWM values
    else:  # If the stick is pushed down, decrease PWM
        return map_value(axis_value, 0, 1, 128, 0)  # Map downward movement to lower PWM values

def handle_input():
    """
    Main function to handle all controller inputs, map them to the correct PWM values,
    and format them into the correct packets for the system.
    """
    global current_packet, last_output

    # Initial state for all components, defaulting to the rest value
    left_wheels = right_wheels = [128, 128, 128]  # PWM values for drive wheels, implementing wheel movement
    gantry = shoulder = claw = elbow = 128  # PWM values for the arm components
    wrist_right = wrist_left = 128  # PWM values for the wrists

    # Loop through all events to handle button presses and joystick movements
    for event in pygame.event.get():
        # Handle button presses
        if event.type == pygame.JOYBUTTONDOWN:
            if event.button == 1:  # Circle button is used to switch between D and A packets
                current_packet = 'A' if current_packet == 'D' else 'D'  # Toggle between arm and drive modes
                print(f"Switched to {current_packet} packet.")  # Required to print when packet is switched

        # Get current positions of the analog sticks
        left_analog_x = controller.get_axis(0)  # X-axis of the left analog stick
        left_analog_y = controller.get_axis(1)  # Y-axis of the left analog stick
        right_analog_x = controller.get_axis(2)  # X-axis of the right analog stick
        right_analog_y = controller.get_axis(3)  # Y-axis of the right analog stick

        # Handle drive (D) packet configuration
        if current_packet == 'D':
            # Map the analog stick movements to the correct PWM values for driving the rover
            right_wheels = [get_corrected_analog_value(left_analog_y)] * 3  # Right wheels respond to left stick Y
            left_wheels = [get_corrected_analog_value(right_analog_y)] * 3  # Left wheels respond to right stick Y
            # Format the drive packet string as specified in the task
            packet = f"D_{right_wheels[0]}_{right_wheels[1]}_{right_wheels[2]}_{left_wheels[0]}_{left_wheels[1]}_{left_wheels[2]}"
        
        # Handle arm (A) packet configuration
        elif current_packet == 'A':
            # Map gantry, shoulder, and elbow movements to their respective PWM values
            gantry = get_corrected_analog_value(left_analog_x) if abs(left_analog_x) > DEADZONE else 128
            shoulder = get_corrected_analog_value(right_analog_x) if abs(right_analog_x) > DEADZONE else 128
            elbow = get_corrected_analog_value(right_analog_y) if abs(right_analog_y) > DEADZONE else 128
            
            # Wrist control using left analog stick Y-axis for both wrist movements
            if abs(left_analog_y) > DEADZONE:
                wrist_right = map_value(left_analog_y, -1, 1, 0, 255)
                wrist_left = 255 if left_analog_y < 0 else wrist_right
            else:
                wrist_right = wrist_left = REST_VALUE

            # Claw control with Triangle and X buttons
            if controller.get_button(3):  # Triangle button to open claw
                claw = 255  # Fully open claw when holding Triangle
            elif controller.get_button(0):  # X button to close claw
                claw = 0  # Fully close claw when holding X
            else:
                claw = REST_VALUE  # Return claw to rest when neither button is pressed

            # Format the arm packet string as specified in the task
            packet = f"A_{elbow}_{wrist_right}_{wrist_left}_{claw}_{gantry}_{shoulder}"

        # Send the packet only if it has changed from the last sent packet
        if packet != last_output:
            send_packet(packet)  # Send the packet as required in the task
            last_output = packet  # Update last output to avoid redundant sends

        # Check if packet is at rest state and ensure the system does not keep sending data
        if current_packet == 'D' and packet == rest_values_d:
            last_output = rest_values_d
        elif current_packet == 'A' and packet == rest_values_a:
            last_output = rest_values_a

def main():
    """
    Main loop to continuously handle inputs and send packets based on user interactions.
    """
    while True:
        handle_input()  # Continuously check and handle controller inputs
        pygame.time.wait(10)  # Short delay to prevent excessive CPU usage

# Entry point for the script
if __name__ == "__main__":
    main() 
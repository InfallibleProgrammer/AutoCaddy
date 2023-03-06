import math
import can
import cantools
import time

db = cantools.database.load_file("../../../../dbc/project.dbc")
# print(db)

# bus = can.Bus("vcan0", bustype="virtual")
bus = can.Bus("PCAN_USBBUS1", bustype="pcan", bitrate=200000)
axisID = 0x3

# print("\nRequesting AXIS_STATE_FULL_CALIBRATION_SEQUENCE (0x012) on axisID: " + str(axisID))
# msg = db.get_message_by_name('Set_Input_Vel')
# print(msg.decode(b'\x0A\x00\x00\x00\x01\x00\x00\x00'))
# # print("THIS IS MSG: {}".format(msg))
# # data = db.encode_message('Set_Input_Vel', {'Input_Vel': 10, 'Input_Torque_FF': 1})
# # print("This is the data: {}".format(data))
# # msg = can.Message(arbitration_id=msg.frame_id | axisID << 5, is_extended_id=False, data=data)
# # print(msg)
# # print(db.decode_message('Set_Input_Vel', msg.data))

# print("\nRequesting AXIS_STATE_FULL_CALIBRATION_SEQUENCE (0x03) on axisID: " + str(axisID))
# msg = db.get_message_by_name('Set_Axis_State')
# data = msg.encode({'Axis_Requested_State': 0x03})
# print(data)
# msg = can.Message(arbitration_id=msg.frame_id | axisID << 5, is_extended_id=False, data=data)
# print(db.decode_message('Set_Axis_State', msg.data))
# print(msg)
# while(1):
#   try:
#       bus.send(msg)
#       print("Message sent on {}".format(bus.channel_info))
#   except can.CanError:
#       print("Message NOT sent!  Please verify can0 is working first")
#   time.sleep(1)

# while (1):
#   try:
#       bus.send(msg)
#       print("Message sent on {}".format(bus.channel_info))
#   except can.CanError:
#       print("Message NOT sent!  Please verify can0 is working first")
#   time.sleep(0.5)

print("Waiting for calibration to finish...")
# Read messages infinitely and wait for the right ID to show up
while True:
    msg = bus.recv()
    if msg.arbitration_id == ((axisID << 5) | db.get_message_by_name('Heartbeat').frame_id):
        print("HI")
        current_state = db.decode_message('Heartbeat', msg.data)['Axis_State']
        if current_state == 0x1:
            print("\nAxis has returned to Idle state.")
            break

# for msg in bus:
#     if msg.arbitration_id == ((axisID << 5) | db.get_message_by_name('Heartbeat').frame_id):
#         errorCode = db.decode_message('Heartbeat', msg.data)['Axis_Error']
#         if errorCode == 0x00:
#             print("No errors")
#         else:
#             print("Axis error!  Error code: "+str(hex(errorCode)))
#         break

# print("\nPutting axis",axisID,"into AXIS_STATE_CLOSED_LOOP_CONTROL (0x08)...")
# data = db.encode_message('Set_Axis_State', {'Axis_Requested_State': 0x08})
# msg = can.Message(arbitration_id=0x07 | axisID << 5, is_extended_id=False, data=data)
# print(msg)

# try:
#     bus.send(msg)
#     print("Message sent on {}".format(bus.channel_info))
# except can.CanError:
#     print("Message NOT sent!")

# for msg in bus:
#     if msg.arbitration_id == 0x01 | axisID << 5:
#         print("\nReceived Axis heartbeat message:")
#         msg = db.decode_message('Heartbeat', msg.data)
#         print(msg)
#         if msg['Axis_State'] == 0x8:
#             print("Axis has entered closed loop")
#         else:
#             print("Axis failed to enter closed loop")
#         break

# target = 0

# data = db.encode_message('Set_Limits', {'Velocity_Limit':10.0, 'Current_Limit':10.0})
# msg = can.Message(arbitration_id=axisID << 5 | 0x00F, is_extended_id=False, data=data)
# bus.send(msg)

# t0 = time.monotonic()
# while True:
#     setpoint = 4.0 * math.sin((time.monotonic() - t0)*2)
#     print("goto " + str(setpoint))
#     data = db.encode_message('Set_Input_Pos', {'Input_Pos':setpoint, 'Vel_FF':0.0, 'Torque_FF':0.0})
#     msg = can.Message(arbitration_id=axisID << 5 | 0x00C, data=data, is_extended_id=False)
#     bus.send(msg)
#     time.sleep(0.01)
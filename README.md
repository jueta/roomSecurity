Project Description

HARDWARE:

esp32-cam
FIR sensor
Battery
RGB leds
switch
case

FUNCTIONALITY:

Connect to local wifi through web-page to insert login password and email adress to be sent.

off mode (HW implemented) -> using the switch to ON/OFF the device.

standby mode (Main function) -> reading FIR sensor and, case activated, take 3 pictures and send to email.

    filming mode -> this will require lots of energy to record, connect to wifi, upload and send email. Must happen rarely.

    scanning mode -> PIR is a passive sensor so this must be in low energy mode.

online mode (future idea) -> be able to take pictures from webpage.

RGB led colours will indicate operation mode
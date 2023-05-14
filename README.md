# Smart Trashbin
The goal of the project is to improve the operation of the bin in order to enable hygienic opening and closing of the lid of the bin without using hands, and to make it easier to access the bin and keep the smells inside it.

### How does it works
Movement is detected through a motion sensor mounted on the front of the bucket. There is a motor inside the bucket that lifts the lid when needed.

### Components
+ STM32F407 Discovery Board
+ Ultrasonic Sensor HC-SR04
+ Servo Motor SG-90

### Work Algorithm
When there is a person in front of the bin for more than 3 seconds, the lid is opened in order to put garbage into the bin. The lid is open as long as the person is in front of the bin. When the person moves, the sensor detects it and the lid closes.
The sensor is programmed to detect a person at a distance between 20cm and 150cm.
The reason why a person has to stay 3 seconds in front of the bin in order to open it is that the bin can be placed in a location where there is frequent movement. In order for the bin not to open every time it detects a movement, it has been programmed so that it happens when a person stays for 3 seconds.
Timing is optional and can be changed as desired.

### Photos and video of the project
<img src="https://i.imgur.com/DYi0wyY.png" width="350" height="350">
<img src="https://i.imgur.com/gK0gkXi.png" width="350" height="350">
<img src="https://i.imgur.com/GrMkhZK.png" width="350" height="350">

https://github.com/vedad-e/smart-trashbin/assets/107326832/3ce1a133-bb8c-403c-913d-e939fa974fe0


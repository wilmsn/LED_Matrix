# LED_Matrix
A Arduino driver for LED Matrix based on MAX7219 / MAX7221

Key features:
* No limitation to a fixed number of devices
* Buildin font: just call print('x'); to print a 'x' at curent position
* Some buildin animations like
  * Scroll display: left, right, up down
  * Scroll a single character: left, right, up down 

For more details of buildin function look at the header file!

Quick startup guide:
* open example Matrix1
* connect your panel with 4 devices to Arduino:
```
  Arduino --- LED panel
     6     |    CLK
     5     |    DIN
     4     |    CS
```	 
* transfer the sketch
* enjoy	 


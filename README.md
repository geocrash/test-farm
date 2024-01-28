The purpose of the test farm is to mechanically stress the stack with different pressures and temperatures. The setup uses 1 Raspberry Pi that logs data from 4 Arduinos. Each Arduino separately controls the temperature and pressure of Test Farm.

Arduino1 is connected to ACM0, Arduino2 to ACM1, Arduino3 to ACM2 and Arduino4 connected to ACM3 of the Raspberry Pi. For Test Farm 1 & 4, the electrolyte is sitting in a water bath. One temperature sensor is placed in the water and one in the electrolyte. The temperature is controlled based on the temperature of the electrolyte. For all Test Farms, one pressure sensor is placed in the pipe inlet to read the pressure. A second one is placed on the outlet (should read 0). A counter reads how many cycles are complete. The electrolyte is pumped from the tank, flows into the stack and back to the same tank. The tank sits in a water bath for Test Farm 1 & 4, where a heater sits.

Before starting the process, the pressure is raised to the required pressure slowly. An initial pressure difference from the inlet and outlet is 600mbar and increased until it reaches 900mbar.

The pressure profile of Test Farm 1 & 4 is:
-	hold constant pressure of 300mbar for 10 seconds
-	hold constant pressure of 900mbar for 10 seconds
-	repeat cycle
-	Data is logged every 5 seconds

Simultaneously, the heating profile of Test Farm 1 & 4 is:
-	increase temperature to 37 degrees Celsius (but keep water between 38 and 40 degrees Celsius)
-	decrease temperature to 30 degrees Celsius
-	repeat cycle

The pressure profile of Test Farm 2 & 3 is:
-	hold constant pressure of 300mbar for 10 seconds
-	hold constant pressure of 600mbar for 1.5 minutes
-	repeat cycle
-	Data is logged every 5 seconds

The Raspberry Pi will have 4 scripts running at the same time. Each script will log data from each Arduino onto a certain folder in the desktop. An additional folder is created as a backup. Each junction box will contain a safety switch for the pumps and the heater.
Link to the real time DrawIO page that shows the cabling layout:
https://www.draw.io/index.html#Wb!WUvRvpLwWk-Tl1ZRCYEIOGjomiw5Kl1BtRu60wMxDAXNYOrlqZiCTKluHao_KTp-%2F01Z4PUDU4NBMQ22Q6275AZXAGH4ZYZ7RAP.

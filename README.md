# HeightRandomizerNVSE
This plugin came up for the necessity of simulating realistic heights by using arbitrary parameters in video games.

We include fast approximations to the realistic average male height in USA, 70 inches, with standard deviation of 4 inches.


Below you can see them, the inverseCDF describing height variation in USA and our approximations towards said approximation (plotted from -0.5 to 0.5, 0 being the exact average person).


![Graph for heights](https://staticdelivery.nexusmods.com/mods/130/images/70159/70159-1603791233-1351463435.png)

Option 1 is the most accurate, simulating faithfully height distribution, however, it doesn't translate into in game very well.
Option 2 is a middle ground between option 1 and 3, where heights are varied but not excessively so.
Option 3 is semilinear, made for high variance, which is most noticeable in games.

While the code is geared towards Fallout New Vegas, you can adapt it to any game.

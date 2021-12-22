# airtalk-rdev

This is a Chooser extension for configuring the AirTalk Wireless dongles for classic Macs.

It works by enumerating all the AirTalkAP objects it can see, allowing the user
to choose one, and then sending a single broadcast configuration packet with the 
chosen SSID and password, which the AirTalk will intercept.

This packet interface is not yet stable, please do not make any assumptions
based upon it.

# How to read the code

Control flow starts in "header.c"; the function the Chooser calls is called
"main" in that file.  You should read this in conjunction with either IM:Devices
or preferably my thread about Chooser Extensions on the 68kMLA forums.

A note on what each pair of *.h and *.c files does is in the .h file.

# Building it

This is a CodeWarrior project suitable for building with CodeWarrior Pro 4.  
The resource files are macbinary encoded.  If you are preparing your working
copy under OS X, you can use scripts/unpack-resources to reconstitute them,
then when you change something, scripts/pack-resources to re-encode them
for git.

# Credits

This code owes a lot to the example code provided by Apple DTS back in the day:
my best wishes to them, whatever they are doing now.
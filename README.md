#esp8266-manager
Everything you need to manage the esp8266, from hardware to server software.<br><br>
No need to understand tons of thing about server or arduino, just connect your esp8266 through usb serial and you are done. No need to read a bible before running it, just copy all the files and make few changes, you should be up and running in few minutes, with all this, you own and control your own web server with your own devices and can help other people to manage their devices as well with your server.<br><br>
<b>Php directory</b> contains all the files needed to be on your root directory of your server.<br>
And <b>'espmanager.ino'</b> is the code source to be run in Arduino IDE.  Of course, you must connect your 'esp' to your computer with a usb-serial cable.<br>
<pre>
Only 2 files to modify:

1. <b>serverinfo.php</b> (4 variables to modify)
       // the number of items in the $ServerEmail array
      .$MaxNbOfEmailServer = 2; 

       // type your emails to be used to send email, they are going to act as smtp server, so the more you have
       // the more you can send email from your server (gmail use 99 emails/day as limitations)
      .$ServerEmail = array ("sbelandiot@gmail.com", "sbelandiot2@gmail.com"); 
      
       // next is the password for all these emails, try to use the same password for all OR 
       // make a new array of password and modify the code in session.php to support an array of password
      .$ServerEmailPw = "xxxxxxx"; 
       
       // whatever name you want to be seen when people receive email from your server
      .$ServerFromName = "Serge Beland"; 

2. <b>espmanager.ino</b> (1 variable to modify)
       // where your php/html files are hosted, everything must be under the root directory
      .ServerHost = "sbelandiot.com"; 
      .in Loop function, add your own code to do whatever under the case STATE_WAIT_FOR_SENSOR
      .in the future, i will add my own code to monitor motion detection and camera
</pre>

<br>
This webserver is compatible with any phone size!<br><br>
I made the background color black for all the website, I hate white background and still don't understand why it is 
being used everywhere...damaging your eyes, polluting the planet, and so on...<br><br>

<h1>Server side</h1>

Here is the website that you are going to own and manage.<br><br>
Login page<br>
<img src="http://sbelandiot.com/github/IOT Login.png"><br>
Control Panel page<br>
<img src="http://sbelandiot.com/github/IOT Control Panel.png"><br>
Edit Profile page<br>
<img src="http://sbelandiot.com/github/IOT Profile.png"><br>
Edit Module page<br>
<img src="http://sbelandiot.com/github/IOTModuleespino94779.png"><br>

<h1>Email</h1>
Here is all about emails received and activation link.<br><br>

Activation link, once your device connect to a wifi somewhere, it will connect to your server and send an activation link by email.<br>
<img src="http://sbelandiot.com/github/emailactivate.jpg"><br>
Once you click on the link, it bring you to the following page if it is a new account (new email)<br>
If it is not a new account, then it bring you to the next image (Account with new device)<br>
New Account with new device, if your user doesn't have an account, it is created automatically with the new device.<br>
<img src="http://sbelandiot.com/github/newaccount.jpg"><br>
Account with new device, when the user have an account, the device is added<br>
<img src="http://sbelandiot.com/github/newdevice3.jpg"><br>
How it look on server using FileZilla<br>
<img src="http://sbelandiot.com/github/fz.jpg"><br>
Once it is activated, the device itself will send you an email that look like this<br>
<img src="http://sbelandiot.com/github/started.jpg"><br>

<h1>Communication Device (Esp) <==> Server</h1>
This is the output of the serial monitor window in Arduino IDE.<br>
<pre>
Chip ID: 608121
EspName: ESPINO-94779

EEPROM INFO READED
SSID: Laguna1 A 1
SsidPassword: xxxxxx <== here you should see your ssid password, but i hide it
ESP Password: admin123
Email: sbeland@gmail.com

Trying to connect to wifi with:
Ssid: Laguna1 A 1
Password: xxxxxx <== here you should see your ssid password, but i hide it
.......

WiFi connected to: Laguna1 A 1
with password: xxxxxx <== here you should see your ssid password, but i hide it
IP address: 192.168.103.35

Get Request
Connecting to: sbelandiot.com
Request: getinfo.php?
Port: 80

Data: create=1&iot=RVNQSU5PLTk0Nzc5&email=c2JlbGFuZEBnbWFpbC5jb20=&modpw=YWRtaW4xMjM= <== encrypted using base64
HTTP/1.1 200 OK
Date: Wed, 21 Sep 2016 15:28:05 GMT
Server: Apache
X-Powered-By: PHP/5.4.43
Transfer-Encoding: chunked
Content-Type: text/html
Connection: close

3b
&email=sbeland@gmail.com
&pw=admin123
&nick=nickname&Inf0

*** GotAnswer: &Inf0 ***  <== Inf0 with a zero 0, not the letter o or O

&nick=nickname&Inf0

Closing connection
Inf0 return in mem: 
&nick=nickname&Inf0
Going to: STATE_SEND_MAIL_STARTED

Post Request
Connecting to: sbelandiot.com
Request: smf.php
Port: 80
Data: c2JlbGFuZEBnbWFpbC5jb20=!#RVNQSU5PLTk0Nzc5OiBTdGFydGVkIQ==!#SW5mbyBmcm9tIHdlYiBzZXJ2ZXIgaGFzIGJlZW4gcmVhZGVkDQpNb2R1bGUgdXBkYXRlZCENCg==

Waiting for answer: Mai1~S3nt

HTTP/1.1 200 OK
Date: Wed, 21 Sep 2016 15:28:11 GMT
Server: Apache
X-Powered-By: PHP/5.4.43
Transfer-Encoding: chunked
Content-Type: text/html
Connection: close

9
Mai1~S3nt
GotAnswer: Mai1~S3nt  <== Mai1~S3nt with the number 1, not the letter L, and number 3 as letter e

Mai1~S3nt

0


Closing connection
WiFi Disconnect  // when we don't do communication with server, no need to keep this on, it used energy and ressources
Going in WaitForSensor mode now

</pre>

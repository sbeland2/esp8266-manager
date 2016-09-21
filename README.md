#esp8266-manager
Everything you need to manage the esp8266, from hardware to server software.<br><br>
No need to understand tons of thing about server or arduino, just connect your esp8266 through usb serial and you are done. No need to read a bible before running it, just copy all the files and follow instructions, you should be up and running in few minutes, with all this, you own and control your own web server with your own devices and can help other people to manage their devices as well with your server.<br><br>
This webserver is compatible with any phone size!<br><br>
I made the background color black for all the website, I hate white background and still don't understand why it is being used everywhere...damaging your eyes, polluting the planet, and so on...<br><br>

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
New Account with new device, if your user doesn't have an account, it is created automatically with the new device.<br>
<img src="http://sbelandiot.com/github/newaccount.jpg"><br>
Account with new device, when the user have an account, the device is added<br>
<img src="http://sbelandiot.com/github/newdevice2.jpg"><br>
How it look on server using FileZilla<br>
<img src="http://sbelandiot.com/github/fz.jpg"><br>

<h1>Communication Device (Esp) <==> Server</h1>
This is the output of the serial monitor window in Arduino IDE.<br>
<par>
Chip ID: 608121<br>
EspName: ESPINO-94779<br>
<br>
EEPROM INFO READED<br>
SSID: Laguna1 A 1<br>
SsidPassword: xxxxxx <== here you should see your ssid password, but i hide it<br>
ESP Password: admin123<br>
Email: sbeland@gmail.com<br>
<br>
Trying to connect to wifi with:<br>
Ssid: Laguna1 A 1<br>
Password: xxxxxx <== here you should see your ssid password, but i hide it<br>
.......<br>
<br>
WiFi connected to: Laguna1 A 1<br>
with password: xxxxxx <== here you should see your ssid password, but i hide it<br>
IP address: 192.168.103.35<br>
<br>
Get Request<br>
Connecting to: sbelandiot.com<br>
Request: getinfo.php?<br>
Port: 80<br>
<br>
Data: create=1&iot=RVNQSU5PLTk0Nzc5&email=c2JlbGFuZEBnbWFpbC5jb20=&modpw=YWRtaW4xMjM= <== encrypted using base64<br>
HTTP/1.1 200 OK<br>
Date: Wed, 21 Sep 2016 15:28:05 GMT<br>
Server: Apache<br>
X-Powered-By: PHP/5.4.43<br>
Transfer-Encoding: chunked<br>
Content-Type: text/html<br>
Connection: close<br>
<br>
3b<br>
&email=sbeland@gmail.com<br>
&pw=admin123<br>
&nick=nickname&Inf0<br>
<br>
*** GotAnswer: &Inf0 ***  <== Inf0 with a zero 0, not the letter o or O<br>
<br>
&nick=nickname&Inf0<br>
<br>
0<br>
<br>
<br>
Closing connection<br>
Inf0 return in mem: <br>
&nick=nickname&Inf0<br>
Going to: STATE_SEND_MAIL_STARTED<br>
<br>
Post Request<br>
Connecting to: sbelandiot.com<br>
Request: smf.php<br>
Port: 80<br>
Data: c2JlbGFuZEBnbWFpbC5jb20=!#RVNQSU5PLTk0Nzc5OiBTdGFydGVkIQ==!#SW5mbyBmcm9tIHdlYiBzZXJ2ZXIgaGFzIGJlZW4gcmVhZGVkDQpNb2R1bGUgdXBkYXR<br>lZCENCg==<br>
<br>
Waiting for answer: Mai1~S3nt<br>
<br>
HTTP/1.1 200 OK<br>
Date: Wed, 21 Sep 2016 15:28:11 GMT<br>
Server: Apache<br>
X-Powered-By: PHP/5.4.43<br>
Transfer-Encoding: chunked<br>
Content-Type: text/html<br>
Connection: close<br>
<br>
9<br>
Mai1~S3nt<br>
GotAnswer: Mai1~S3nt  <== Mai1~S3nt with the number 1, not the letter L, and number 3 as letter e<br>
<br>
Mai1~S3nt<br>
<br>
0<br>
<br>
<br>
Closing connection<br>
WiFi Disconnect  // when we don't do communication with server, no need to keep this on, it used energy and ressources<br>
Going in WaitForSensor mode now<br>
<br>
</par>

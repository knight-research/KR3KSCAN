const uint16_t gamma_adjust[] {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };
  

const char page_contents[] PROGMEM = R"=====(
 <!doctype html>
 <html>
 
 <head>
 <style type="text/css">
 .auto-style1 {
   font-size: small;
   color: #FFFFFF;
 }
 </style>
 </head>

 <body style="color: #FF0000; background-color: #000000">
 
 <p align='center'><id='brightness_page'><strong style='font-size:400%'>KNIGHT 3000 SCANNER</strong><p align='center'>
 <strong><span class="auto-style1">by knight-research.de V2020-08-004</span></strong><p align='center'>
 &nbsp;<BR>

<p align='center'><button type='button'id='brightnessBtn'style='height:100px;width:500px;font-size:19px;font-weight:bold;font-size:35px' onclick="location.href='/set_brightness'">SPEED</button></p><BR><BR>
 
<p align='center'> <button type='button'id='modeBtn'style='height:100px;width:500px;font-size:19px;font-weight:bold;font-size:35px' onclick="location.href='/select_mode'">MODES</button></p>

 </body></html>
</body>
</html>)=====";


const char colour_picker[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<body>

<p align='center'><id='colour_page'><BR><strong style='font-size:400%'>Touch below to select a colour</strong></p><BR><BR>


<p id="place_holder_1"></p>

<p id="place_holder_2"></p>


<script type='text/javascript'>
  
var hash_value=unescape((''+self.location.search).substring(1));
if(hash_value==""){hash_value="#0000ff"};

function loadForm()
{
  document.getElementById("place_holder_1").innerHTML ="<form id='colourForm' action='/set_colour_hash' method='GET'><input type='color' name='chosen_colour' onchange='clickColor()' value=" + hash_value + " style='width:100%;height:480px'></form>;<BR><BR>"
}

function loadText(){
  document.getElementById("place_holder_2").innerHTML ="<p align='center'><button type='button' id='homeBtn' style='height:100px;width:500px;font-size:19px;font-weight:bold;font-size:35px' onclick=\"location.href='/'\">Main Menu</button></p><BR><BR>"
}

function clickColor()
{
  document.getElementById('colourForm').submit();
}

loadForm();
loadText();

</script>

</body>
</html>)=====";


const char bright_set[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<body style="color: #FF0000; background-color: #000000">

<p align='center'><id='colour_page'><BR><strong style='font-size:400%'>Move The Slider to Set 
Speed</strong></p>
<BR><BR>


<p id="place_holder_1"></p>

<p id="place_holder_2"></p>


<script type='text/javascript'>
  
var brightness_val=unescape((''+self.location.search).substring(1));
if(brightness_val==""){brightness_val=254};

function loadForm()
{
  document.getElementById("place_holder_1").innerHTML ="<form id='brightForm' action='/set_bright_val' method='get'><input type='range' name='chosen_brightness' onchange='clickBright()' min='1' max='20' value='" + brightness_val + "' style='width:100%;height:100px'></form><BR><BR>"
}

function loadText(){
  document.getElementById("place_holder_2").innerHTML ="<p align='center'><button type='button' id='homeBtn' style='height:100px;width:500px;font-size:19px;font-weight:bold;font-size:35px' onclick=\"location.href='/'\">Main Menu</button></p><BR><BR>"
}

function clickBright()
{
  document.getElementById('brightForm').submit();
}

loadForm();
loadText();

</script>

</body>
</html>)=====";


const char mode_page[] PROGMEM = R"=====(
<!doctype html><body style="color: #FF0000; background-color: #000000"><p align='center'><id='brightness_page'><strong style='font-size:400%'>SCAN 
MODE</strong></p>
<BR>
<p align='center'><button type='button'id='mode1Btn'style='height:100px;width:500px;font-size:19px;font-weight:bold;font-size:35px' onclick="location.href='/set_mode1'">M1 ALL OFF</button></p><BR><BR>
<p align='center'><button type='button'id='mode2Btn'style='height:100px;width:500px;font-size:19px;font-weight:bold;font-size:35px' onclick="location.href='/set_mode2'">M2 KARR 3000</button></p><BR><BR>
<p align='center'><button type='button'id='mode3Btn'style='height:100px;width:500px;font-size:19px;font-weight:bold;font-size:35px' onclick="location.href='/set_mode3'">M3 KITT 3000</button></p><BR><BR>
<p align='center'><button type='button'id='mode4Btn'style='height:100px;width:500px;font-size:19px;font-weight:bold;font-size:35px' onclick="location.href='/set_mode4'">M4 BLUE KNIGHT</button></p><BR><BR>
<p align='center'><button type='button'id='mode5Btn'style='height:100px;width:500px;font-size:19px;font-weight:bold;font-size:35px' onclick="location.href='/set_mode5'">M5 GREEN KNIGHT</button></p><BR><BR>
<p align='center'><button type='button'id='mode6Btn'style='height:100px;width:500px;font-size:19px;font-weight:bold;font-size:35px' onclick="location.href='/set_mode6'">M6 AQUA KNIGHT</button></p><BR><BR>
<p align='center'><button type='button'id='mode7Btn'style='height:100px;width:500px;font-size:19px;font-weight:bold;font-size:35px' onclick="location.href='/set_mode7'">M7 RES1 KNIGHT</button></p><BR><BR>
<p align='center'><button type='button'id='mode8Btn'style='height:100px;width:500px;font-size:19px;font-weight:bold;font-size:35px' onclick="location.href='/set_mode8'">M8 RES2 KNIGHT</button></p><BR><BR>
<p align='center'><button type='button'id='homeBtn'style='height:100px;width:500px;font-size:19px;font-weight:bold;font-size:35px'onclick="location.href='/'">MAIN MENU</button></p>
</body></html>)=====";

# Register Page



## Install

1. Download Xampp [here](https://www.apachefriends.org/download.html)

2. Navigate through the Xampp installer with 

   1. Apache and PHP ticked under "Select Components"
   2. Use the default `C:\xampp` directory

   *By default your website files can go into `C:\xampp\htdocs`* 

3. Go to https://github.com/microsoft/msphpsql/releases click the top release and download the zip file relative to your version of PHP i.e. [Windows-8.1.zip](https://github.com/microsoft/msphpsql/releases/download/v5.10.1/Windows-8.1.zip)

4. Navigate into the zip file open the `x64` folder copy the file that starts with `php_pdo_` and ends with `_ts.dll` into `C:\xampp\php\ext`

5. Open `C:\xampp\php\php.ini` in a text editor, and 

   - Add this new line to the bottom of the file  `extension=php_pdo_sqlsrv_81_ts_x64.dll` where `php_pdo_sqlsrv_81_ts_x64.dll` is the file name you copied out of the zip earlier
   - Save the file

6. Download [this register page](https://raw.githubusercontent.com/soma-space/files/main/www/arcanine/register.php) make sure it has the .php extension and not .txt, move it into `C:\xampp\php\htdocs`


##  Start

1. Open the control Panel, click Start next to Apache
2. Navigate to http://localhost/register.php on the same machine as your Soma server to see your register page

   


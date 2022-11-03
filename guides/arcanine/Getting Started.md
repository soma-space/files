# 🧙‍Soma Server Setup Guide



## Server Hosting

To set up a Soma server we are going to need a computer that has Microsoft Windows.

If you already have a Windows computer then skip this section.

1. Sign in or sign up for AWS developer account here: https://aws.amazon.com/console/
2. Select Lightsail from the services panel in the top left
3. Press the orange Create instance button
4. Under Select a platform select Microsoft Windows blueprints
5. Under Select a blueprint, press OS Only and select the latest Windows Server version
6. Under Choose your instance plan select the first plan that has at least 4GB
7. Click Create Instance at the bottom of the page
8. Once your server is running connect to your server using RDP ✅



## Installing MSSQL Server Express

1. Download MSSQL Express 2019 from  https://go.microsoft.com/fwlink/?linkid=866658
2. Open the installer and select "Basic"
3. Press Accept on Licence Terms screen
4. Press Install on the install location screen, grab a cup of tea ☕️
5. Press New SQL Server stand-alone installation or add features to an existing installation
6. Tick I accept the license terms and press Next on the License Terms screen
7. Tick Use Microsoft Update to check for updates press Next
8. Untick SQL Server Replication, Machine Learning Services and Full-Text, press Next
9. Leave the Named instance as `SQLExpress` and Instance ID as `SQLEXPRESS`
10. Press Next on the Server Configuration page
11. Select Mixed mode, set a strong password for your sa account, press Next and then Close ✅



## Prepare MSSQL 
1. Install SQL Server Management tools from [Microsoft](https://learn.microsoft.com/en-us/sql/ssms/download-sql-server-management-studio-ssms?redirectedfrom=MSDN&view=sql-server-ver16)
2. Open Microsoft SQL Server Management Studio
3. Select Windows Authentication and press Connect
4. Download [your favourite database](https://github.com/soma-space/files/tree/main/databases) 
5. Right click Databases and press Restore Database
6. Select Device and press the ... button, press Add find your downloaded Database
7. Change the Destination Database field to `soma` and press Ok
8. Right Click Security -> Logins press New Login...
9. Put `soma` into the Login name field, select SQL Server authentication
10. Type `soma` into both password fields, untick Enforce password policy
11. Change the default database to `soma` press User Mapping and tick the soma database
12. Right click the main server node and press Server Properties, click Connections untick Allow remote connections to this server
13. Click Security and make sure Server authentication is set to SQL Server and Windows Authentication mode
14. Open SQL Server Configuration Manager, expand the SQL Server Network Configuration on the left hand side, press Protocols for SQLEXPRESS, enable TCP/IP
15. Press SQL Server Services and restart the SQL Server (SQLEXPRESS) service ✅



## Create ODBC connection

1. Open the start menu and search for "ODBC" open the result that says (32-bit)
2. Select System DSN and press Add...
3. Select "SQL Server" and press finish
4. Type `soma` into the Name field and Description field, set the server as your server name you should see this when you login, something like `EC2AHAZ-56LCI5B\SQLEXPRESS` 
5. Select With SQL Server authentication using a login ID and password entered by the user
6. Change the login id to `soma` and the password to `soma` press next
7. Tick Change the default database to press next and finish then press ok ✅



## Start Server

1. Go to Google and search for "what is my ip" copy your public IP address 
2. Paste the IP address in services/dir.ini by replacing the value of SVR01_ADDR=192.168.0.1
    with your public IP address i.e. SVR01_ADDR=X.X.X.X
3. Open Task Manager, more details, press Performance and click your network note down your IPv4 address
4. Download [services](https://github.com/soma-space/files/tree/main/services) and open config.ini, change IP to the IP noted above, change ServicePath to your extracted `services` folder
5. Open Services.exe and press Create Services, each service should say Started at the bottom 
6. Download oneperone files from [here](https://github.com/soma-space/files/tree/main/oneperones/arcanine)
7. Run sharedmem.exe
8. Run oneperone.exe ✅



## Prepare to Connect

1. Open MSSQL Server Studio Press New Query
2. Paste the following query, change the USERNAME, PASSWORD and EMAIL values
```SQL
INSERT INTO NGSCUSER (strUserId, strPasswd, strEmail) VALUES ('USERNAME','PASSWORD', 'EMAIL')
```
3. Press Execute

4. Download and install an esoma client from [here](https://www.reddit.com/r/soma_space/comments/y0kpmp/esoma_client_links/)
5. Right click Myth of Soma shortcut on your desktop right click it and press Properties in the target field add your public IP address so that the target field has this pattern: `C:\Soma\Soma.exe X.X.X.X` ✅



## Networking

1. Forward the TCP ports of 12000, 4110 in your network, add port 80 if you are hosting your own website
   if you are using Lightsail follow the rest of this section
2. Open Amazon Light sail, press the 3 dots next to your server click Manage and then Networking
3. Then add your ports using the IPv4 Firewall, the Application needs to be set to custom for each port ✅



## Play!

1. Double click the shortcut, login with the details you specified earllier and create a character
2. Once you have a character this query can promote your character to game master
```sql 
UPDATE GAMEUSER SET sRANK = '1' WHERE strUserId = 'CharNameHere'
```
3. Login to your server as your character type `/rain 1` to test whether you have game master capabilities ✅



## Troubleshooting

MSSQL Install

- Windows firewall causing issues on install of MSSQL?
- Remove everything that mentions MSSQL via control panel if there is an error on install
- Run the setup file as an administrator

ODBC Connection

- Check the server authentication is still set to `SQL Server and Windows Authentication mode` 
- If your ODBC connection isn't working close the window and press Add and try again

Services

- Ensure your ODBC connection is a 32 bit connection, not a 64 bit connection

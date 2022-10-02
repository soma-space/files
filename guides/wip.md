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



## Setup MSSQL 
1. Install SQL Server Management tools from [Microsoft](https://learn.microsoft.com/en-us/sql/ssms/download-sql-server-management-studio-ssms?redirectedfrom=MSDN&view=sql-server-ver16)
2. Open Microsoft SQL Server Management Studio



## Troubleshooting

- Windows firewall causing issues on install of MSSQL?
- Remove everything that mentions MSSQL via control panel if there is an error on install
- Run the setup file as an administrator

# 🧙‍♂️ Myth of Soma Server Setup Guide



## Server Hosting

In order to set up a Myth of Soma game server we are going to need a computer that can run Microsoft Windows.

If you are going to use your own computer or want to use another host then skip to installation.

1. Sign in or sign up for AWS developer account here: https://aws.amazon.com/console/
2. Select Lightsail from the services panel in the top left
3. Press the orange Create instance button
4. Under Select a platform select Microsoft Windows blueprints
5. Under Select a blueprint, press OS Only and select the latest Windows Server version
6. Under Choose your instance plan select the first plan that has at least 4GB
7. Click Create Instance at the bottom of the page
8. Once your server is running press the screen icon to visually connect to your server
9. Optionally use a Remote Desktop Client by pressing the three dots and then clicking manage to see your login credentials

## Installing MSSQL Server Express

1. Download MSSQL Express 2019 from  https://go.microsoft.com/fwlink/?linkid=866658

2. Right click the installer, press run as administrator and select "Basic"

3. Press Accept on licence terms screen
4. Press Install on the install location screen, grab a cup of tea ☕️
5. Open the SQL Server Installation Center
6. Press New SQL Server stand-alone installation or add features to an existing installation
7. Press Next





```
TITLE: SQL Server Setup failure.
------------------------------

SQL Server Setup has encountered the following error:

Setup encountered a failure while running job UpdateResult.

Error code 0x876E0003.

For help, click: https://go.microsoft.com/fwlink?LinkID=20476&ProdName=Microsoft%20SQL%20Server&EvtSrc=setup.rll&EvtID=50000&EvtType=0x448E4A06%25400x96D07230

------------------------------
BUTTONS:

OK
------------------------------

```



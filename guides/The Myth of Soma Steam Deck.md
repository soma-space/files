## The Myth of Soma Install Guide for Steam Deck (work in progress)



### Dependencies

1. Go into desktop mode
2. Install Lutris from the discover store
3. Download the esoma client and patches from [here](https://www.reddit.com/r/soma_space/comments/y0kpmp/esoma_client_links/)




### Notes


Install lutris via discover app
Download the Myth of Soma installer
Download a 32bit copy of mfc42.dll
https://www.dll-files.com/download/6d564d355d1ab55e6c52d985ecd01c7b/mfc42.dll.html?c=dnh4bGVmRjFSYkJBOUZxcFA4MDZRZz09 (or copy your own from your own windows 32bit install)
Download windows fonts
https://www.w7df.com/p/windows-10.html (or copy your own windows fonts from your own install)

Press add, install a game via its setup media, and point to the Myth of Soma installer
Move mfc42.dll to ~/Games/Myth of Soma/c_drive/Myth of Soma alongside the soma.exe files
Set the game resolution in SomaDevLauncher.ini to the Steam deck Resolution (1280x800) and set the window to 0 (we want full screen all the time)
Move the windows fonts into the wine directory ~/Games/Myth of Soma/c_drive/Windows/fonts

If Lutris has set SomaFull.exe as the target binary change it to the soma dev launcher exe

Change the config for Soma in Lutris to turn on Dgvoodoo2
Right-click on the configuration in Lutris and press create a shortcut for steam


Boot into gaming mode, find the Myth of Soma shortcut and run it, at this point you'll need to create your own steam input configuration to play

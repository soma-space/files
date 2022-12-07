///////////////////////////////////////////////////////////////////////////////
// SMProtocol.h - Session Manager Protocol Define
//

#ifndef _SMPROTOCOL_H_
#define _SMPROTOCOL_H_

// Session Management Messages
#define SMPROTOCOL				(WORD)(0x8000)
#define SM_LOGIN_REQ			(SMPROTOCOL + 0)	// Login Req.
// STRING	UserID
// STRING	Passwd
// STRING	Serial1
// STRING	Serial2
#define SM_LOGIN_ACK			(SMPROTOCOL + 1)	// Login Ack.
// BYTE		LoginRes
// BYTE		SNRes

#define SM_REGUSER_REQ			(SMPROTOCOL + 2)	// Register user Req.
// STRING	UserID		User ID
// STRING	Passwd		Password
// STRING	Name		Name
// STRING	SocNo		Social Number
// STRING	Address		Address
// STRING	TelNo		Telephone Number
// STRING	Email		Email Address
#define SM_REGUSER_ACK			(SMPROTOCOL + 3)	// Register user Ack.
// BYTE		Result		1 on success, or 0

#define SM_GETINFO_REQ			(SMPROTOCOL + 4)	// Get user information Req.
// No Argument
#define SM_GETINFO_ACK			(SMPROTOCOL + 5)	// Get user information Ack.
// STRING	UserID		User ID
// STRING	Passwd		Password
// STRING	Name		Name
// STRING	SocNo		Social Number
// STRING	Address		Address
// STRING	TelNo		Telephone Number
// STRING	Email		Email Address

#define SM_SETINFO_REQ			(SMPROTOCOL + 6)	// Set user information req.
// STRING	Passwd		Password
// STRING	Name		Name
// STRING	SocNo		Social Number
// STRING	Address		Address
// STRING	TelNo		Telephone Number
// STRING	Email		Email Address
#define SM_SETINFO_ACK			(SMPROTOCOL + 7)	// Set user information ack.
// BYTE		Result		1 on success, or 0

#define SM_REGSN_REQ			(SMPROTOCOL + 8)	// Register S/N Req.
// STRING	Passwd		Serial Number Password
#define SM_REGSN_ACK			(SMPROTOCOL + 9)	// Register S/N Ack.
// BYTE		Result		1 on success, or 0

#define SM_DOWN_REQ				(SMPROTOCOL + 10)	// Start downloading Req.
// No Argument
#define SM_DOWN_ACK				(SMPROTOCOL + 11)	// Start downloading Ack.
// BYTE		Result		1 on success, or 0

#define SM_GAME_REQ				(SMPROTOCOL + 12)	// Start game Req.
// BYTE		Success		1 on start game, or 0 on cancel
#define SM_GAME_ACK				(SMPROTOCOL + 13)	// Start game Ack.
// BYTE		Result		1 on success, or 0

#define SM_GAMEINFO_REQ			(SMPROTOCOL + 14)
// No Argument
#define SM_GAMEINFO_ACK			(SMPROTOCOL + 15)
// STRING	ServiceName
// BYTE		PayOpt
// STRING	GameID
// WORD		GameVer
#define SM_DIR_REQ				(SMPROTOCOL + 16)
// DWORD	DesID
// STRING	UserID
// BYTE		Start
#define SM_DIR_ACK				(SMPROTOCOL + 17)
// DWORD	DesID
// STRING	UserID
// BYTE		Start
// STRING	Addr
// DWORD	Port

#define SM_LOGIN_ACK2			(SMPROTOCOL + 18)	// Login Ack.
// BYTE		LoginRes
// BYTE		SNRes

#define SM_UPGRADING_ACK		(SMPROTOCOL + 21)	// Updrading Status...
// STRING	msg

#define SM_GAMEINFO2_REQ		(SMPROTOCOL + 22)
// No Argument

#define SM_CONNECTINFO_ACK      (SMPROTOCOL + 100)
// BYTE     
//userid
//

#define SM_ALIVE_ACK			(SMPROTOCOL + 999)

#endif _SMPROTOCOL_H_

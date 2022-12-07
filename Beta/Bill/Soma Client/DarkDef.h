#ifndef _DARKDEF_H_
#define _DARKDEF_H_

#define ITEM_ALL		0
#define	ITEM_WOMAN		1
#define ITEM_MAN		2

class ITEM_ME
{
public:
	int			m_vIndex;			// 번호
	BYTE		m_vType;			// 종류
	BYTE		m_vGender;			// 성별(0:중성, 1:여자, 2:남자)
	int			m_vPicNum;			// 그림 번호
	short		m_vMinDamage;		// 최소 위력
	short		m_vMaxDamage;		// 최대 위력
	short		m_vLimitStr;		// 최소 Str
	short		m_vLimitInt;		// 최소 Int
	short		m_vLimitSkill;		// 최소 숙련도
	CString		m_strMagic;			// 마법 이름
	CString		m_strSNum;			// S항목
	short		m_vSpecial[7];		// 특수 능력(Str,Dex,Int,Atk,Def,HP,MP)
	DWORD		m_vValue;			// 가격
	CString		m_vName;		// 이름
	int			m_vUsage;			// 사용회수
	short		m_vMaxUsage;		// 최대 사용회수
	short		m_vOrgMaxUsage;		// 원래 최대 사용회수

	ITEM_ME()
	{
		m_vIndex = m_vPicNum = -1;
		m_vGender = ITEM_ALL;
		m_vType = 0;
		m_vUsage = m_vMaxUsage = m_vOrgMaxUsage = 0;
		m_vMinDamage = 0;
		m_vMaxDamage = 0;
		m_vName.Empty();
		for(int i = 0; i < 7; i++)
		{
			m_vSpecial[i] = 0;
		}
		m_vValue = 0;
	};
};

class ITEM_SHOP
{
public:
	int				m_vItemNumber;		// 번호
	short			m_vUsage;			// 사용회수
	short			m_vMaxUsage;		// 최대 사용회수
	short			m_vOrgMaxUsage;		// 원래 최대 사용회수
	int				m_vPicNum;			// 그림 번호
	BYTE			m_vGender;			// 성별(0:중성, 1:여자, 2:남자)
	short			m_vMinDamage;		// 최소 위력
	short			m_vMaxDamage;		// 최대 위력
	short			m_vLimitStr;		// 최소 Str
	short			m_vLimitInt;		// 최소 Int
	short			m_vLimitSkill;		// 최소 숙련도
	CString			m_strMagic;			// 마법 이름
	CString			m_strSNum;			// S항목
	short			m_vSpecial[7];		// 특수 능력(Str,Dex,Int,Atk,Def,HP,MP)
	DWORD			m_vValue;			// 값
	CString			m_vName;		// 이름

	ITEM_SHOP()
	{
		m_vGender = ITEM_ALL;
		m_vItemNumber = m_vPicNum = m_vValue = 0;
		m_vUsage = m_vMaxUsage = m_vOrgMaxUsage = 0;
		m_vMinDamage = 0;
		m_vMaxDamage = 0;
		m_vName.Empty();
		for(int i = 0; i < 7; i++)
		{
			m_vSpecial[i] = 0;
		}
	};
};

class ITEM_INV
{
public:
	int			m_vPicNum;			// 그림 번호
	BYTE		m_vType;			// 종류
	BYTE		m_vArmPos;			// 장착위
	BYTE		m_vGender;			// 성별(0:중성, 1:여자, 2:남자)
	short		m_vMinDamage;		// 최소 위력
	short		m_vMaxDamage;		// 최대 위력
	short		m_vLimitStr;		// 최소 Str
	short		m_vLimitInt;		// 최소 Int
	short		m_vLimitSkill;		// 최소 숙련도
	CString		m_strMagic;			// 마법 이름
	CString		m_strSNum;			// S항목
	short		m_vSpecial[7];		// 특수 능력(Str,Dex,Int,Atk,Def,HP,MP)
	CString		m_vName;		// 이름
	short		m_vUsage;			// 사용회수
	short		m_vMaxUsage;		// 최대 사용회수
	short		m_vOrgMaxUsage;		// 원래 최대 사용회수

	ITEM_INV()
	{
		m_vType = 0;
		m_vGender = ITEM_ALL;
		m_vUsage = m_vMaxUsage = m_vOrgMaxUsage = 0;
		m_vArmPos = 0;
		m_vMinDamage = 0;
		m_vMaxDamage = 0;
		m_vPicNum = -1;
		m_vName.Empty();
		for(int i = 0; i < 7; i++)
		{
			m_vSpecial[i] = 0;
		}
	};
};

class ITEM_STORAGE
{
public:
	int			m_vIndex;			// 아이템 인덱스
	int			m_vPicNum;			// 아이템 그림 번호
	BYTE		m_vType;			// 종류
	BYTE		m_vGender;			// 성별(0:중성, 1:여자, 2:남자)
	short		m_vMinDamage;		// 최소 위력
	short		m_vMaxDamage;		// 최대 위력
	short		m_vLimitStr;		// 최소 Str
	short		m_vLimitInt;		// 최소 Int
	short		m_vLimitSkill;		// 최소 숙련도
	CString		m_strMagic;			// 마법 이름
	CString		m_strSNum;			// S항목
	short		m_vSpecial[7];		// 특수 능력(Str,Dex,Int,Atk,Def,HP,MP)
	short		m_vUsage;			// 아이템 사용 회수
	short		m_vMaxUsage;		// 최대 사용회수
	short		m_vOrgMaxUsage;		// 원래 최대 사용회수
	CString		m_vName;		// 아이템 이름

	ITEM_STORAGE()
	{
		m_vGender = ITEM_ALL;
		m_vType = 0;
		m_vMinDamage = 0;
		m_vMaxDamage = 0;
		m_vIndex = m_vPicNum = -1;
		m_vUsage = m_vMaxUsage = m_vOrgMaxUsage = 0;
		m_vName.Empty();
		for(int i = 0; i < 7; i++)
		{
			m_vSpecial[i] = 0;
		}
	};
};

class ITEM_DETAIL
{
public:
	int			m_vIndex;			// 아이템 인덱스
	int			m_vPicNum;			// 그림 번호
	BYTE		m_vType;			// 종류
	BYTE		m_vArmPos;			// 장착위
	BYTE		m_vGender;			// 성별(0:중성, 1:여자, 2:남자)
	short		m_vMinDamage;		// 최소 위력
	short		m_vMaxDamage;		// 최대 위력
	short		m_vLimitStr;		// 최소 Str
	short		m_vLimitInt;		// 최소 Int
	short		m_vLimitSkill;		// 최소 숙련도
	CString		m_strMagic;			// 마법 이름
	CString		m_strSNum;			// S항목
	short		m_vSpecial[7];		// 특수 능력(Str,Dex,Int,Atk,Def,HP,MP)
	short		m_vUsage;			// 재료 개수
	short		m_vMaxUsage;		// 최대 사용회수
	short		m_vOrgMaxUsage;		// 원래 최대 사용회수
	CString		m_vName;		// 이름

	ITEM_DETAIL()
	{
		m_vGender = ITEM_ALL;
		m_vMinDamage = m_vMaxDamage = 0;
		m_vType = m_vArmPos = 0;
		m_vIndex = m_vPicNum = -1;
		m_vUsage = m_vMaxUsage = m_vOrgMaxUsage = 0;
		for(int i = 0; i < 7; i++)
		{
			m_vSpecial[i] = 0;
		}
		m_vName.Empty();
	};
};

class ITEM_AUCTION
{
public:
	int			m_vIndex;			// 인덱스
	BYTE		m_vType;			// 종류
	BYTE		m_vGender;			// 성별(0:중성, 1:여자, 2:남자)
	short		m_vMinDamage;		// 최소 위력
	short		m_vMaxDamage;		// 최대 위력
	short		m_vWeight;			// 무게
	short		m_vPicNum;			// 그림 번호
	CString		m_vName;			// 이름
	short		m_vLimitStr;		// 필요 Str
	short		m_vLimitInt;		// 필요 Int
	short		m_vLimitSkill;		// 필요 숙련도
	CString		m_strMagic;			// 마법 이름
	CString		m_strSNum;			// S항목
	short		m_vUsage;			// 사용회수(내구력)
	short		m_vMaxUsage;		// 최대 사용회수
	short		m_vOrgMaxUsage;		// 원래 최대 사용회수
	short		m_vSpecial[7];		// 특수 능력(Str,Dex,Int,Atk,Def,HP,MP)
	DWORD		m_vCost;			// 경매 가격
	CString		m_vRegistrant;		// 설명

	ITEM_AUCTION()
	{
		m_vIndex = -1;
		m_vType = 0;
		m_vGender = ITEM_ALL;
		m_vMinDamage = m_vMaxDamage = m_vWeight = m_vPicNum = -1;
		m_vLimitStr = m_vLimitInt = m_vLimitSkill = 0;
		m_vUsage = m_vMaxUsage = m_vOrgMaxUsage = 0;
		for(int i = 0; i < 7; i++)
		{
			m_vSpecial[i] = 0;
		}
		m_vCost = 0;
	};
};

typedef CTypedPtrArray<CPtrArray, ITEM_ME *> CItemMeArray;
typedef CMap< int, int, ITEM_ME *, ITEM_ME *&> CItemMeMap;
typedef CMap< int, int, ITEM_INV *, ITEM_INV *&> CItemInvMap;
typedef CTypedPtrArray<CPtrArray, ITEM_DETAIL *> CItemDetailArray;
typedef CTypedPtrArray<CPtrArray, ITEM_SHOP *> CItemShopArray;
typedef CTypedPtrArray<CPtrArray, ITEM_STORAGE *> CItemStorageArray;
typedef CTypedPtrArray<CPtrArray, ITEM_AUCTION *> CItemAuctionArray;

#define TYPE_SWORD				0
#define TYPE_AXE				1
#define TYPE_BOW				2
#define TYPE_SPEAR				3
#define TYPE_KNUCKLE			4
#define TYPE_STAFF				5
#define TYPE_ACC				8
#define TYPE_POTION				9
#define TYPE_FOOD				10
#define TYPE_MTR				11
#define TYPE_TOKEN				12

#define MTR_METAL				1
#define MTR_WOOD				2
#define MTR_LEATHER				3
#define MTR_BONE				4
#define MTR_CLOTH				5
#define MTR_PLANTS				6
#define MTR_MEAT				7
#define MTR_POWDER				8
#define MTR_SPICE				9
#define MTR_GEMS				10
#define MTR_ETC					11

#define TYPE_WEAPON				6
#define TYPE_ARMOR				7

#define ARM_HEAD				0
#define ARM_EAR					1
#define ARM_NECK				2
#define ARM_CHEST				3
#define ARM_LEG					4
#define ARM_FOOT				5
#define ARM_RHAND				6
#define ARM_LHAND				7
#define ARM_HANDS				8
#define ARM_WRIST				9
#define ARM_USE					10
#define ARM_CON					11

#define BELT_INV				0
#define BASIC_INV				1

#define INFO_ALL				0xff
#define INFO_TYPE				0x01
#define INFO_ARM				0x02
#define INFO_PICNUM				0x04
#define INFO_DUR				0x08
#define INFO_NAME				0x10
#define INFO_DAMAGE				0x20
#define INFO_LIMIT				0x40
#define INFO_SPECIAL			0x80

#define DEFAULT_FONT			"굴림체"
#endif
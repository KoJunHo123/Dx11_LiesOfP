#pragma once

enum class ITEM_SPEC
{

};

enum class INVEN_ARRAY_TYPE
{
	// 인벤토리 들어가는 순서대로 정리 
	TYPE_USING_BASIC, // 사용 아이템	기본아이템
	TYPE_USING_EXPENDABLE, // 사용 아이템	소모품
	TYPE_USING_THROW, // 사용 아이템	투척물
	TYPE_USING_ERGO, // 사용 아이템	에르고
	TYPE_USING_RARE_ERGO, // 사용 아이템	희귀 에르고

	TYPE_INGRADIENT_GROW, // 소재	성장 소재
	TYPE_INGRADIENT_SPECIAL, // 소재	특수 소재

	TYPE_FUNTIONAL_COMMON, // 기능 아이템(기능 확인 필요)

	TYPE_WEAPON_NORMAL_BLADE, // 무기	일반 무기(날)
	TYPE_WEAPON_NORMAL_HANDLE, // 무기	일반 무기(자루)
	TYPE_WEAPON_SPECIAL_BLADE, // 무기	특수 무기(날)
	TYPE_WEAPON_SPECIAL_HANDLE, // 무기 특수 무기(자루)


	TYPE_BLADE_PART, // 날 / 손잡이 날
	TYPE_HANDEL_PART,// 날 / 손잡이 손잡이

	TYPE_REASON_ARM, // 리전 암

	TYPE_DEFENCE_FRAME, // 방어 파츠	프레임
	TYPE_DEFENCE_CONVERTOR, // 방어 파츠	컨버터
	TYPE_DEFENCE_CARTRIGE, // 방어 파츠	카트리지
	TYPE_DEFENCE_RAINER, // 방어 파츠	라이너

	TYPE_AMULET_COMMON, // 아뮬렛

	TYPE_COUSTUME_CLOTHES, // 코스튬	의상
	TYPE_COUSTUME_MASK, // 코스튬	가면
	TYPE_COUSTUME_ACC, // 코스튬	악세서리

	TYPE_GEUSTURE_COMMON, // 제스쳐

	TYPE_COLLECTION_NEWS, // 수집품	신문
	TYPE_COLLECTION_BOOK, // 수집품	서적
	TYPE_COLLECTION_MEMO, // 수집품	쪽지


	TYPE_END
};

enum class EQUIP_SLOT
{
	// 장비창 순서대로 정리 

	EQUIP_WEAPON_BLADE_0,  // 장비	무기 및 리전 암 0
	EQUIP_WEAPON_HANDLE_0,

	EQUIP_WEAPON_BLADE_1,  // 장비	무기 및 리전 암 1
	EQUIP_WEAPON_HANDLE_1,

	EQUIP_RESION_ARM,  // 장비	무기 및 리전 암 2

	EQUIP_AMULET_0, // 장비	아뮬렛 0
	EQUIP_AMULET_1,// 장비	아뮬렛 1

	EQUIP_DEFENCE_FRAME, // 방어 파츠	프레임
	EQUIP_DEFENCE_CONVERTOR, // 방어 파츠	컨버터
	EQUIP_DEFENCE_CARTRIGE, // 방어 파츠	카트리지
	EQUIP_DEFENCE_RAINER, // 방어 파츠	라이너

	EQUIP_USING_TOP_0, // 사용 아이템	위 벨트
	EQUIP_USING_TOP_1,
	EQUIP_USING_TOP_2,

	EQUIP_USING_BOTTOM_0,  // 사용 아이템	아래 벨트
	EQUIP_USING_BOTTOM_1,
	EQUIP_USING_BOTTOM_2,

	EQUIP_USING_BAG_0, // 사용 아이템	보조가방
	EQUIP_USING_BAG_1,
	EQUIP_USING_BAG_2,
	EQUIP_USING_BAG_3,

	EQUIP_HEAD_0, // 코스튬	머리
	EQUIP_HEAD_1,
	EQUIP_CLOTHES, // 코스튬	의상

	EQUIP_END
};


enum class ITEM_TYPE
{
	ITEMTYPE_SWORD,
	ITEMTYPE_HAMMER,
	ITEMTYPE_BIGHAMMER,
	ITEMTYPE_BIGSWORD,
	ITEMTYPE_DAGGER,
	ITEMTYPE_SINGLEHAND,
	ITEMTYPE_TWOHAND,
	ITEMTYPE_POLE,
	ITEMTYPE_ARM,
	ITEMTYPE_AMULET,
	ITEMTYPE_FRAME,
	ITEMTYPE_CONVERTOR,
	ITEMTYPE_CARTRIGE,
	ITEMTYPE_RINER,
	ITEMTYPE_SPEND,
	ITEMTYPE_BASIC,
	ITEMTYPE_RAER_ERGO,
	ITEMTYPE_ERGO,
	ITEMTYPE_THROW,
	ITEMTYPE_BLADE_RAINFORCE,
	ITEMTYPE_SPECIAL_INGRADIANT,
	ITEMTYPE_ACTIVATE_FUNTION,
	ITEMTYPE_EXCHAINGE_INGRADIANT,
	ITEMTYPE_GRAINDER,
	ITEMTYPE_HANDLE_CONVERTOR,
	ITEMTYPE_LEVELUP_INGRADIENT,
	ITEMTYPE_END
};

enum class ITEM_RESULT
{
	RESULT_SUCCESS,
	RESULT_FULL,
	RESULT_INVALID,
	RESULT_END
};

enum class DAMEGE_TYPE
{
	DAMEGE_PHYSICS,
	DAMEGE_FIRE,
	DAMEGE_ELECTRIC,
	DAMEGE_ACID,
	DAMEGE_END
};

enum class DEFENCE_TYPE
{
	DEFENCE_FIRE,
	DEFENCE_ELECTRIC,
	DEFENCE_ACID,
	DEFNECE_COLLAPSE,
	DEFENCE_CRASH,
	DEFENCE_BREAK,
	DEFENCE_SLASH,
	DEFNECE_HIT,
	DEFENCE_PIRCE,
	DEFENCE_END
};

enum class ABLITY_TYPE
{
	ABLITY_POWER,
	ABLITY_SKILL,
	ABLITY_EVOUTION,
	ABLITY_END
};


enum class SPECIAL_ITEM
{
	// 특수 기능의 아이템 인덱스 기록 
	SP_PULSE_BATTERY = 284, // 펄스 전지, 충전 수치 따로 존재하며 사용 시, 체력 회복 O
	SP_DUSTCAT = 238, // 고양이 가루, 기척 감소 
	SP_GRINDER = 239, // 그라인더, 무기 내구도 증가 O 
	SP_LAMP = 243, // 모나드의 등불, 주변 밝기 증가 
	SP_TELEPOT = 244, // 문페이즈 회중시계, 순간이동 O
	SP_RESISTANCE = 255, // 속성 저항 앰플, 속성 저항 증가 O
	SP_PURIFICATION = 256, // 속성 정화 앰플, 디버프 제거 O
	SP_DEAD = 272, // 최후의 수단, 사망 O
	SP_GRANADE = 275, // 클러스터 수류탄 O
	SP_THERMITE = 276, // 테르밋 O
	SP_THROW_BATTERY = 279, // 투척용 전지 O 

	SP_END
};

enum class ITEM_FUNC
{
	FUNC_USING, // <- 일반 아이템을 사용하여 기능 활성화 
	FUNC_TO_INVEN, // <- 인벤으로 이동
	FUNC_TO_EQUIP, // <- 장비로 이동
	FUNC_EQUIP, // <- 일반 아이템 및 장비류를 장착
	FUNC_UNEQUIP, // <- 장착된 것을 해제 
	FUNC_DELETE, // <- 아이템 버리기 (삭제)

	FUNC_EQUIP_TOP, // <- 위 벨트
	FUNC_EQUIP_BOTTOM, // <- 아래 벨트 
	FUNC_EQUIP_BAG, // <- 가방 

	FUNC_EQUIP_WEAPON_FIRST,
	FUNC_EQUIP_WEAPON_SECOND,

	FUNC_EQUIP_TOP_0,
	FUNC_EQUIP_TOP_1,
	FUNC_EQUIP_TOP_2,

	FUNC_EQUIP_BOTTOM_0,
	FUNC_EQUIP_BOTTOM_1,
	FUNC_EQUIP_BOTTOM_2,

	FUNC_EQUIP_BAG_0,
	FUNC_EQUIP_BAG_1,
	FUNC_EQUIP_BAG_2,
	FUNC_EQUIP_BAG_3,

	FUNC_END
};



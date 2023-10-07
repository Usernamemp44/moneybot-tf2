#include "CItem.h"
#include "../../Interfaces/CModelInfo/CModelInfo.h"
int CItem::GetSize()
{
	const char *model_name = CModelInfo::factory()->GetModelName(GetModel());

	int size = 0;

	if (strstr(model_name, "large")) {
		size = 2;
	}
	if (strstr(model_name, "medium")) {
		size = 1;
	}
	return size;
}

int CItem::GetType()
{
	const char *model_name = CModelInfo::factory()->GetModelName(GetModel());
	if (strstr(model_name, "models/items/plate") || ((strstr(model_name, "models/props_halloween/halloween_medkit_")) || strstr(model_name, "models/items/medkit_")))
		return 1; //health
	if (strstr(model_name, "models/items/ammopack") || strstr(model_name, "models/props_halloween/pumpkin_loot") || strstr(model_name, "models/items/tf_gift") )
		return 2; //ammo
	return 0; //ammo
}

const char* CItem::GetPrettyPrintName() {
	int size = GetSize();
	int type = GetType();

	if (type == 1) {
		if (size == 0) {
			return "Small Ammo";
		}
		if (size == 1) {
			return "Medium Ammo";
		}
		return "Large Ammo";
	}
	
	if (size == 0)
		return "Small Health";
	if (size == 1)
		return "Medium Health";
	return "Large Health";
}
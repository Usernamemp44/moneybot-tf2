#include "RenderGroup.h"

//#define CheatModules (*GetEntityTypes())



void RenderGroup::AddEntityType(EntityType type) {
	AllowedEntityTypes |= (1<<type);
}

void RenderGroup::RemoveEntityType(EntityType type) {
	AllowedEntityTypes &= ~(1<<type);
}

bool RenderGroup::ValidEntityType(EntityType type) {
	return (AllowedEntityTypes & (1<<type));
}

bool RenderGroup::ValidCondition(Condition cond) {
	return (Conditions & (1<<cond));
}
void RenderGroup::AddCondition(Condition cond) {
	Conditions |= (1 << cond);
}
void RenderGroup::RemoveCondition(Condition cond) {
	Conditions &= ~(1 << cond);
}
bool RenderGroup::DoesFit(CBaseEntity* entity) {
	if (!entity)
		return false;
	CBasePlayer* LocalPlayer = CEntityList::factory()->GetClientPlayer(CEngineClient::factory()->GetLocalPlayer());

	bool doesFitCondition = true;
	if (MatchAllOrOne) {
		doesFitCondition &= !RenderGroup::ValidCondition(LOCAL_PLAYER) ? true : (RenderGroup::ValidCondition(LOCAL_PLAYER) && entity->GetIndex() == CEngineClient::factory()->GetLocalPlayer());
		doesFitCondition &= !RenderGroup::ValidCondition(FRIENDLY) ? true : (RenderGroup::ValidCondition(FRIENDLY) && entity->GetTeamNum() == LocalPlayer->GetTeamNum());
		doesFitCondition &= !RenderGroup::ValidCondition(HOSTILE) ? true : (RenderGroup::ValidCondition(HOSTILE) && entity->GetTeamNum() != LocalPlayer->GetTeamNum());
		doesFitCondition &= !RenderGroup::ValidCondition(DORMANT) ? true : (RenderGroup::ValidCondition(DORMANT) && entity->IsDormant());
		if (entity->IsPlayer()) {
			CBasePlayer* player = (CBasePlayer*)entity;
			if (player->GetClassNum() == TF2_Spy) {
				doesFitCondition &= !RenderGroup::ValidCondition(DISGUISED) ? true : (RenderGroup::ValidCondition(DISGUISED) && (player->GetCond()& TFCond_Disguised) == TFCond_Disguised);
				doesFitCondition &= !RenderGroup::ValidCondition(CLOAKED) ? true : (RenderGroup::ValidCondition(CLOAKED) && (player->GetCond()& TFCond_Cloaked) == TFCond_Cloaked);
			}
		}
	}
	else {
		doesFitCondition = false;
		doesFitCondition |= (RenderGroup::ValidCondition(LOCAL_PLAYER) && entity->GetIndex() == CEngineClient::factory()->GetLocalPlayer());
		doesFitCondition |= (RenderGroup::ValidCondition(FRIENDLY) && entity->GetTeamNum() == LocalPlayer->GetTeamNum());
		doesFitCondition |= (RenderGroup::ValidCondition(HOSTILE) && entity->GetTeamNum() != LocalPlayer->GetTeamNum());
		doesFitCondition |= (RenderGroup::ValidCondition(DORMANT) && entity->IsDormant());
		if (entity->IsPlayer()) {
			CBasePlayer* player = (CBasePlayer*)entity;
			if (player->GetClassNum() == TF2_Spy) {
				doesFitCondition |= (RenderGroup::ValidCondition(DISGUISED) && (player->GetCond()& TFCond_Disguised) == TFCond_Disguised);
				doesFitCondition |= (RenderGroup::ValidCondition(CLOAKED) && (player->GetCond()& TFCond_Cloaked) == TFCond_Cloaked);
			}
		}
	}

	bool doesFitPlayer = false;

	doesFitPlayer |= (RenderGroup::ValidEntityType(PLAYER) &&	 entity->IsPlayer());
	doesFitPlayer |= (RenderGroup::ValidEntityType(SENTRY) &&		 (entity->IsBuilding() && entity->GetClassId() == CObjectSentrygun));
	doesFitPlayer |= (RenderGroup::ValidEntityType(DISPENSER) &&	 (entity->IsBuilding() && entity->GetClassId() == CObjectDispenser));
	doesFitPlayer |= (RenderGroup::ValidEntityType(TELEPORTER) && (entity->IsBuilding() && entity->GetClassId() == CObjectTeleporter));
	doesFitPlayer |= (RenderGroup::ValidEntityType(HEALTH) && (entity->IsPickup() && ((CItem*)(entity))->GetType() == 1));
	doesFitPlayer |= (RenderGroup::ValidEntityType(AMMO) && (entity->IsPickup() && ((CItem*)(entity))->GetType() == 2));

	return (doesFitPlayer&&doesFitCondition);

}
#ifndef PLAYER_ENTITY_HPP
#define PLAYER_ENTITY_HPP

#include <OGRE/OgreVector3.h>
#include "Mob.hpp"

class PlayerEntity : public Mob
{
public:
	PlayerEntity(std::string a_OgreName, Ogre::SceneManager& a_SceneMgr, Ogre::Camera* a_pCamera);
	virtual ~PlayerEntity();
	//
	void Update(float a_DeltaT);
	Ogre::Camera* GetCamera();
	//
private:
    Ogre::Camera* m_pCamera;
};

#endif	//PLAYER_ENTITY_HPP
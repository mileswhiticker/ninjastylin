#ifndef SHURIKEN_HPP
#define SHURIKEN_HPP

#include <OGRE/OgreVector3.h>
#include "Mob.hpp"

class Shuriken : public Mob
{
public:
	Shuriken(std::string a_OgreName, Ogre::SceneManager& a_SceneMgr, Ogre::Camera* a_pCamera);
	virtual ~Shuriken();
	//
	void Update(float a_DeltaT);
	Ogre::Camera* GetCamera();
	//
private:
    Ogre::Camera* m_pCamera;
};

#endif	//SHURIKEN_HPP
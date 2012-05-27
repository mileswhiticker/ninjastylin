#include "Shuriken.hpp"


#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreEntity.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreVector3.h>
//#include <OGRE/SdkTrays.h>
#include <OGRE/OgreCamera.h>

Shuriken::Shuriken(std::string a_OgreName, Ogre::SceneManager& a_SceneMgr, Ogre::Camera* a_pCamera)
:	Mob(a_OgreName, a_SceneMgr, "ninja.mesh")
,	m_pCamera(a_pCamera)
{
	m_pMyNode->setScale(0.01f, 0.01f, 0.01f);
	//
	m_pMyNode->setOrientation(m_pCamera->getOrientation());
	//
	m_ForwardMove = 1;
}

Shuriken::~Shuriken()
{
	//
}

void Shuriken::Update(float a_DeltaT)
{
	Mob::Update(a_DeltaT);
	
	//walk in the direction the entity is facing
	Ogre::Vector3 curPos = m_pMyNode->getPosition();
	Ogre::Vector3 curOrientation = m_pMyNode->getOrientation() * Ogre::Vector3::NEGATIVE_UNIT_Z;
	curPos -= curOrientation * m_ForwardMove * a_DeltaT * 250 * m_ForwardMoveMultiplier;

	//handle strafing
	curOrientation = m_pMyNode->getOrientation() * Ogre::Vector3::NEGATIVE_UNIT_X;
	curPos -= curOrientation * m_SideMove * a_DeltaT * 30;

	//if we go below the ground, stop
	if(curPos.y < (m_CurGroundHeight))
	{
		m_ForwardMove = 0;
	}
	m_pMyNode->setPosition(curPos);

	//bind the camera to the entity position, but bind the entity to the camera orientation
	if(m_pCamera)
	{
		m_pCamera->setPosition(m_pMyNode->getPosition() - m_pCamera->getDirection() * 100);
		//
		Ogre::Vector3 lookAtPos = m_pCamera->getPosition() - m_pCamera->getDirection() * 200;
		m_pMyNode->lookAt(lookAtPos, Ogre::Node::TS_WORLD);
	}
}

Ogre::Camera* Shuriken::GetCamera()
{
	return m_pCamera;
}

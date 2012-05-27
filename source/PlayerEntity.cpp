#include "PlayerEntity.hpp"

#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreEntity.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreVector3.h>
//#include <OGRE/SdkTrays.h>
#include <OGRE/OgreCamera.h>

#define ENTITY_Y_OFFSET 5

PlayerEntity::PlayerEntity(std::string a_OgreName, Ogre::SceneManager& a_SceneMgr, Ogre::Camera* a_pCamera)
:	Mob(a_OgreName, a_SceneMgr)
,	m_pCamera(a_pCamera)
{
	//m_pCamera->setPosition(m_pMyNode->getPosition());
	//m_pCamera->setOrientation(m_pMyNode->getOrientation());
}

PlayerEntity::~PlayerEntity()
{
	//
}

void PlayerEntity::Update(float a_DeltaT)
{
	Mob::Update(a_DeltaT);
	
	//walk in the direction the entity is facing
	Ogre::Vector3 curPos = m_pMyNode->getPosition();
	Ogre::Vector3 curOrientation = m_pMyNode->getOrientation() * Ogre::Vector3::NEGATIVE_UNIT_Z;
	curPos -= curOrientation * m_ForwardMove * a_DeltaT * 50 * m_ForwardMoveMultiplier;

	//handle strafing
	curOrientation = m_pMyNode->getOrientation() * Ogre::Vector3::NEGATIVE_UNIT_X;
	curPos -= curOrientation * m_SideMove * a_DeltaT * 30;

	//match the height of the ground as closely as possible
	if(curPos.y < (m_CurGroundHeight + ENTITY_Y_OFFSET))
	{
		curPos.y = m_CurGroundHeight + ENTITY_Y_OFFSET;	//climb up a slope, or possibly hover
		m_FallVelocity = 0;
	}
	else if(curPos.y > (m_CurGroundHeight + ENTITY_Y_OFFSET))
	{
		m_FallVelocity += 0.98f * a_DeltaT;	//slow gravity for now
		curPos.y -= m_FallVelocity;
	}
	m_pMyNode->setPosition(curPos);

	//bind the camera to the entity position, but bind the entity to the camera orientation
	if(m_pCamera)
	{
		m_pCamera->setPosition(m_pMyNode->getPosition() - m_pCamera->getDirection() * 100);
		//
		Ogre::Vector3 lookAtPos = m_pCamera->getPosition() - m_pCamera->getDirection() * 200;
		lookAtPos.y = curPos.y;
		m_pMyNode->lookAt(lookAtPos, Ogre::Node::TS_WORLD);
	}
}

Ogre::Camera* PlayerEntity::GetCamera()
{
	return m_pCamera;
}

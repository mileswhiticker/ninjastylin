#include "Mob.hpp"

#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreEntity.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreVector3.h>

Mob::Mob(std::string a_OgreName, Ogre::SceneManager& a_SceneMgr, std::string a_MeshName)
:	m_pMyEntity(NULL)
,	m_pMyNode(NULL)
,	m_SceneMgr(a_SceneMgr)
,	m_ForwardMove(0)
,	m_SideMove(0)
,	m_CurGroundHeight(0)
,	m_FallVelocity(0)
,	m_ForwardMoveMultiplier(1)
{
	static int uid = 0;
	m_NodeUID = uid++;
	//make an entity
	m_pMyEntity = m_SceneMgr.createEntity(a_OgreName, a_MeshName);

    // Create a SceneNode and attach the Entity to it
    m_pMyNode = m_SceneMgr.getRootSceneNode()->createChildSceneNode(a_OgreName);
	m_pMyNode->setScale(0.25f,0.25f,0.25f);
	m_pMyNode->attachObject(m_pMyEntity);
	Ogre::Vector3 startPos = Ogre::Vector3( 1300 * float(rand() / RAND_MAX) + 100, 50,  1300 * float(rand() / RAND_MAX) + 100);
	m_pMyNode->setPosition(startPos);
}

Mob::~Mob()
{
	m_SceneMgr.destroyEntity(m_pMyEntity->getName());
	m_SceneMgr.destroySceneNode(m_pMyNode);
}

void Mob::Update(float a_DeltaT)
{
	//
}

void Mob::SetPosition(Ogre::Vector3 a_NewPos)
{
	m_pMyNode->setPosition(a_NewPos);
}

Ogre::Vector3 Mob::GetPosition()
{
	return m_pMyNode->getPosition();
}

void Mob::SetMoveForward(float a_Forward)
{
	m_ForwardMove = a_Forward;
}

void Mob::SetMoveSide(float a_Left)
{
	m_SideMove = a_Left;
}

void Mob::SetGroundHeight(float a_Height)
{
	m_CurGroundHeight = a_Height;
}

int Mob::GetNodeUID()
{
	return m_NodeUID;
}

void Mob::SetMoveForwardMultiplier(float a_MultiP)
{
	m_ForwardMoveMultiplier = a_MultiP;
}

float Mob::GetMoveForward()
{
	return m_ForwardMove;
}

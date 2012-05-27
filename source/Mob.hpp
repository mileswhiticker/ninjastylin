#ifndef MOB_HPP
#define MOB_HPP

#include <OGRE/OgreVector3.h>

namespace Ogre
{
	class Entity;
	class SceneNode;
	class SceneManager;
	class Camera;
	class Radian;
};

class Mob
{
public:
	Mob(std::string a_OgreName, Ogre::SceneManager& a_SceneMgr, std::string a_MeshName = "penguin.mesh");
	virtual ~Mob();
	//
	int GetNodeUID();
	void Update(float a_DeltaT);
	//
	void SetMoveForward(float a_Forward = 1);
	void SetMoveForwardMultiplier(float a_MultiP = 1);
	void SetMoveSide(float a_Left = 1);
	//
	float GetMoveForward();
	//
	void SetPosition(Ogre::Vector3 a_NewPos);
	Ogre::Vector3 GetPosition();
	//
	void SetGroundHeight(float a_Height);
	//
protected:
	Ogre::Entity* m_pMyEntity;
	Ogre::SceneNode* m_pMyNode;
	Ogre::SceneManager& m_SceneMgr;
	//
	float m_ForwardMove;
	float m_ForwardMoveMultiplier;
	float m_SideMove;
	//
	float m_CurGroundHeight;
	float m_FallVelocity;
	//
private:
	int m_NodeUID;
	//
};

#endif	//MOB_HPP
#ifndef APP_HPP
#define APP_HPP

#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreLogManager.h>
#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreConfigFile.h>
#include <OgrePlane.h>

#include <OGRE/OgreVector3.h>

//#include <Terrain/OgreTerrain.h>
//#include <Terrain/OgreTerrainGroup.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include <SdkTrays.h>
#include <SdkCameraMan.h>

#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/Ogre/CEGUIOgreRenderer.h>

class PlayerEntity;
class Mob;
class Shuriken;
#define SHURIKEN_TTL 20.f

class App : public Ogre::FrameListener, public Ogre::WindowEventListener, public OIS::KeyListener, public OIS::MouseListener//, OgreBites::SdkTrayListener
{
public:
    App(void);
    virtual ~App(void);
	//
    virtual void go(void);
	//
private:
	bool update(float a_DeltaT);

    virtual bool setup();
    virtual bool configure(void);
    virtual void chooseSceneManager(void);
    virtual void createCamera(void);
    virtual void createFrameListener(void);
    virtual void createScene(void);
    virtual void destroyScene(void);
    virtual void createViewports(void);
    virtual void setupResources(void);
    virtual void createResourceListener(void);
    virtual void loadResources(void);

	void ThrowShuriken();
	void ShurikenDie();
	float tLeftShurkenAlive;
	Shuriken* m_pThrownShuriken;
	
	bool m_SceneSetup;

	//ogre renderer module
    CEGUI::OgreRenderer* mRenderer;

    // Ogre::FrameListener
    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);

    // OIS::KeyListener
    virtual bool keyPressed( const OIS::KeyEvent &arg );
    virtual bool keyReleased( const OIS::KeyEvent &arg );
    // OIS::MouseListener
    virtual bool mouseMoved( const OIS::MouseEvent &arg );
    virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
    virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );

    // Ogre::WindowEventListener
    //Adjust mouse clipping area
    virtual void windowResized(Ogre::RenderWindow* rw);
    //Unattach OIS before window shutdown (very important under Linux)
    virtual void windowClosed(Ogre::RenderWindow* rw);

	//some top level ogre managers
    Ogre::Root *mRoot;
    Ogre::Camera* mCamera;
	//Ogre::Plane* m_pPlane;
    Ogre::SceneManager* mSceneMgr;
    Ogre::RenderWindow* mWindow;
    Ogre::String mResourcesCfg;
    Ogre::String mPluginsCfg;

	void setupGUI();
	//gui elements
	CEGUI::Window* m_pSheet;
	CEGUI::Window* m_pTitle;
	CEGUI::Window* m_pQuit;
	CEGUI::Window* m_pNewGameButton;
	CEGUI::Window* m_pMainMenuButton;
	
	//cegui callback functions
	bool NewGame(const CEGUI::EventArgs &e);
	bool Quit(const CEGUI::EventArgs &e);
	bool MainMenu(const CEGUI::EventArgs &e);

    // OgreBites
	OgreBites::SdkTrayManager* mTrayMgr;
    OgreBites::SdkCameraMan* mCameraMan;       // basic camera controller
    OgreBites::ParamsPanel* mDetailsPanel;     // sample details panel
	OgreBites::Label* mInfoLabel;
    bool mCursorWasVisible;                    // was cursor visible before dialog appeared
    bool mShutDown;

    //OIS Input devices
    OIS::InputManager* mInputManager;
    OIS::Mouse*    mMouse;
    OIS::Keyboard* mKeyboard;

	//some ogre scene things
	Ogre::Light* m_pMainlight;
	std::vector<Mob*> m_Mobs;
	PlayerEntity* m_pPlayerEntity;

	//misc
    Ogre::RaySceneQuery *mRaySceneQuery;// The ray scene query pointer
    bool mLMouseDown, mRMouseDown;		// True if the mouse buttons are down
    int mCount;							// The number of robots on the screen
    Ogre::SceneNode *mCurrentObject;	// The newly created object
    CEGUI::Renderer *mGUIRenderer;		// CEGUI renderer
    float mRotateSpeed;	
};

#endif	//APP_HPP